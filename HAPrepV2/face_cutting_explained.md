# How Face Cutting Works

This document explains exactly how cutting a hole into a face works in this editor — from the moment you press `C` to the final triangles on screen. Every step maps directly to code in this project.

---

## Overview

When you cut a shape out of a face, three things happen in sequence:

1. **The user draws a cut contour** inside an existing face using the editor's `CUT_DRAWING` state.
2. **`performCut()`** splits the scene into two independent faces — the inner cut shape and the re-holed outer face.
3. **`tessellate()`** re-triangulates the outer face using a *bridge-edge merge* followed by *ear-clipping*, producing GPU-ready triangles that visually show the hole.

---

## Part 1 — The Editor State Machine

### Relevant file: `src/Editor.cpp`

The editor is always in one of five modes:

```
NORMAL → DRAWING → (right-click) → NORMAL
NORMAL → (click face) → SELECTED → (C key) → CUT_DRAWING → (right-click) → NORMAL
```

#### Step 1: Select a face

In `NORMAL` mode, a left-click calls `hitTestFace(mouseWorld)`. This walks the `faces` array in reverse order, calling `isPointOnFace()` on each face. The first match becomes the `selectedFaceId`. The face's `selected` flag is set to `true` and `rebuildMesh()` is called to re-render it brighter.

#### Step 2: Enter cut mode

Pressing `C` in `SELECTED` mode sets:

```cpp
drawingVerts.clear();
mode = EditorMode::CUT_DRAWING;
```

The `selectedFaceId` is preserved — it remembers *which* face is being cut.

#### Step 3: Place cut vertices

Every left-click in `CUT_DRAWING` mode appends to `drawingVerts`:

```cpp
drawingVerts.push_back(mouseWorld);
```

`mouseWorld` is the 2D world-space coordinate derived from the screen pixel position via `screenToWorld2D()`. The contour is rendered live as a yellow line preview in the left viewport.

#### Step 4: Close the cut

A right-click with at least 3 vertices triggers:

```cpp
int outerIdx = selectedFaceId;
deselectAll();
performCut(outerIdx, drawingVerts);
drawingVerts.clear();
mode = EditorMode::NORMAL;
```

`deselectAll()` runs first because `performCut()` calls `rebuildMesh()` on the outer face — we don't want to rebuild it in the selected (bright) state since it's about to be deselected anyway.

---

## Part 2 — performCut()

### Relevant file: `src/Editor.cpp`, function `EditorState::performCut()`

```cpp
void EditorState::performCut(int outerIdx, const Contour& cutContour)
{
    // 1. Create the inner face
    Face inner;
    inner.id    = nextId++;
    inner.outer = cutContour;
    if (signedArea(inner.outer) < 0)
        std::reverse(inner.outer.begin(), inner.outer.end());
    assignColor(inner);
    inner.rebuildMesh();
    faces.push_back(std::move(inner));

    // 2. Update the outer face
    Contour hole = cutContour;
    if (signedArea(hole) > 0)
        std::reverse(hole.begin(), hole.end());
    faces[outerIdx].holes.push_back(hole);
    faces[outerIdx].rebuildMesh();
}
```

### What this does

**The inner face** is created directly from the cut contour. It becomes a completely independent `Face` in the `faces` array with its own ID and colour. You can select it, move it, cut it again — it has no memory of which face it came from. Its outer contour is forced to CCW winding (positive signed area).

**The outer face** receives the same contour added to its `holes` list, but with the *opposite* winding — CW (negative signed area). The winding direction is how the tessellator later distinguishes "this is a boundary to go around" from "this is a hole to skip over".

Both faces then call `rebuildMesh()`, which triggers full re-tessellation and re-upload to the GPU.

### The winding convention

Winding direction is determined by the *signed area* of a contour, calculated using the shoelace formula:

```cpp
float signedArea(const Contour& c)
{
    float area = 0.0f;
    for (int i = 0; i < n; i++) {
        area += (c[i].x * c[i+1].y - c[i+1].x * c[i].y);
    }
    return area * 0.5f;
}
```

- **Positive area** → vertices go counter-clockwise (CCW) — used for outer boundaries
- **Negative area** → vertices go clockwise (CW) — used for holes

The same `cutContour` is used for both faces. For the inner face, it's forced CCW (it is now an outer boundary). For the hole stored on the outer face, it's forced CW (it is a hole).

---

## Part 3 — rebuildMesh()

### Relevant file: `src/Face.cpp`

```cpp
void Face::rebuildMesh()
{
    std::vector<Vec2> tris2D = tessellate(outer, holes);
    // ... convert tris2D to GPU vertex buffer ...
    mesh.free();
    mesh.upload(triVerts, lineVerts);
}
```

This calls `tessellate()` with the face's outer contour and its list of holes. The result is a flat list of 2D triangle vertices — every 3 entries form one triangle. These are then interleaved with colour values and uploaded to the GPU as a VAO/VBO.

The wireframe (edge) buffer is built separately from the original contours — the outer contour edges in white, hole edges in orange. This is stored in the same `Mesh` alongside the triangle buffer.

---

## Part 4 — tessellate()

### Relevant file: `src/Tessellator.cpp`

This is the core algorithm. It takes a polygon with holes and produces a triangle list. It works in two stages: **bridge-edge merge** then **ear-clipping**.

```
tessellate(outer, holes)
    → for each hole: mergeHole(working, hole)   [bridge-edge merge]
    → earClip(merged_polygon)                   [ear-clipping]
    → triangle list
```

---

## Part 4a — Bridge-Edge Merge

### Why it's needed

Ear-clipping only works on *simple* polygons — a single closed boundary with no holes. A face with a hole is not a simple polygon. The bridge-edge merge converts it into one by connecting the hole to the outer boundary with a "bridge" — a pair of edges that create a seam, turning the two-boundary shape into one continuous boundary.

### How it works

The function is `mergeHole(poly, hole)` in `Tessellator.cpp`.

**Step 1: Find M — the rightmost vertex of the hole**

```cpp
int mIdx = 0;
for (int i = 1; i < (int)hole.size(); i++)
    if (hole[i].x > hole[mIdx].x) mIdx = i;
Vec2 M = hole[mIdx];
```

We pick the rightmost vertex because casting a ray to the right from it is guaranteed to eventually hit the outer boundary (since the hole is inside the outer polygon). This is the vertex that will be the "bridge anchor" on the hole side.

**Step 2: Cast a ray in the +X direction from M**

```cpp
for (int i = 0; i < pn; i++) {
    Vec2 a = poly[i], b = poly[(i + 1) % pn];
    float ix = raySegmentX(M, a, b);
    if (ix < bestX) { bestX = ix; bestEdgeStart = i; }
}
```

`raySegmentX` finds where a horizontal rightward ray from M crosses each outer edge. We want the *nearest* crossing — the first outer edge you hit travelling right from M.

```cpp
static float raySegmentX(Vec2 p, Vec2 a, Vec2 b)
{
    if ((a.y > p.y) == (b.y > p.y)) return FLT_MAX; // edge doesn't straddle p.y
    float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
    if (x < p.x) return FLT_MAX; // intersection is to the left
    return x;
}
```

An edge only qualifies if one endpoint is above M's y-coordinate and the other is below (it "straddles" the ray), and the intersection is to the right of M.

**Step 3: Find P — the mutually visible outer vertex**

From the nearest intersected edge, pick the endpoint with the greater x-coordinate:

```cpp
if (a.x >= b.x)
    pIdx = bestEdgeStart;
else
    pIdx = (bestEdgeStart + 1) % pn;
```

This is the "candidate" bridge vertex P on the outer boundary. We prefer the higher-x endpoint because it's more likely to be directly visible from M without anything blocking the line of sight.

**Robustness check:** if any other outer vertex lies inside the triangle formed by M, the intersection point I, and P, it means P's visibility is blocked. We pick the blocking vertex with the smallest angle from M instead:

```cpp
Vec2 P = poly[pIdx];
Vec2 I = {bestX, M.y};
for (int i = 0; i < pn; i++) {
    if (i == pIdx) continue;
    Vec2 v = poly[i];
    if (pointInTriangle(v, M, I, P)) {
        float angle = atan2(abs(v.y - M.y), v.x - M.x);
        if (angle < bestAngle) { bestAngle = angle; pIdx = i; }
    }
}
```

**Step 4: Splice the hole into the outer polygon**

```cpp
static Contour spliceHole(const Contour& poly, int pIdx,
                           const Contour& hole, int mIdx)
{
    Contour result;
    for (int i = 0; i <= pIdx; i++)          // outer[0..P]
        result.push_back(poly[i]);
    for (int i = 0; i <= hn; i++)            // M, hole..., M  (full loop)
        result.push_back(hole[(mIdx + i) % hn]);
    for (int i = pIdx; i < pn; i++)          // P, outer[P+1..end]
        result.push_back(poly[i]);
    return result;
}
```

The result is a single polygon whose vertex sequence looks like:

```
outer[0], outer[1], ..., P,
M, hole[M+1], hole[M+2], ..., hole[M-1], M,
P, outer[P+1], ..., outer[n-1]
```

Both P and M appear **twice** in this sequence. These are the "seam" vertices — they define the two bridge edges `P→M` and `M→P`. The polygon is now self-touching at those two points, but it is a single continuous boundary that ear-clipping can process.

### Visual example

Before merge (6-vertex outer, 5-vertex hole):
```
Outer:  A - B - C - D - E - F
                     ↑
                     P (rightmost visible from M)

Hole:   1 - 2 - 3 - 4 - 5
                 ↑
                 M (rightmost of hole)
```

After splice (13 vertices):
```
A - B - C - P - M - 4 - 5 - 1 - 2 - 3 - M - P - D - E - F
              ↑bridge↑               ↑bridge↑
```

The single polygon `A-B-C-P-M-4-5-1-2-3-M-P-D-E-F` winds around the exterior and through the hole via the seam.

---

## Part 4b — Ear-Clipping

### Relevant file: `src/Tessellator.cpp`, function `earClip()`

Ear-clipping is a classical triangulation algorithm for simple polygons. It works by repeatedly finding and removing "ears" — triangles that can be cut off from the polygon without creating any overlap — until only one triangle remains.

### What is an ear?

A vertex B is an **ear** if:
1. The triangle formed by its predecessor A, itself B, and its successor C is **CCW** (interior angle less than 180°).
2. **No other vertex** of the polygon lies inside that triangle.

The CCW test uses the 2D cross product:

```cpp
Vec2 ab{B.x-A.x, B.y-A.y}, ac{C.x-A.x, C.y-A.y};
if (cross2D(ab, ac) <= 0.0f) continue;  // CW or degenerate — not an ear
```

`cross2D(ab, ac)` computes `ab.x * ac.y - ab.y * ac.x`. A positive value means the turn from AB to AC is counter-clockwise, i.e. B is a convex vertex.

The containment test uses barycentric signs:

```cpp
bool pointInTriangle(Vec2 p, Vec2 a, Vec2 b, Vec2 c)
{
    float d1 = cross2D(b-a, p-a);
    float d2 = cross2D(c-b, p-b);
    float d3 = cross2D(a-c, p-c);
    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hasNeg && hasPos);
}
```

If all three cross products have the same sign (all positive or all negative), the point is inside the triangle. If they have mixed signs, the point is outside.

### The bridge duplicate problem and the fix

The merged polygon from the bridge-edge step contains two pairs of identical vertices (P appears twice, M appears twice). Without special handling, the inside test would always find the duplicate of P or M "inside" a nearby ear candidate's triangle, blocking *every* ear and causing the algorithm to get stuck after clipping just one triangle.

The fix: skip any vertex that is spatially coincident with A, B, or C during the inside test:

```cpp
auto coincident = [](Vec2 p, Vec2 q) {
    return std::abs(p.x-q.x) < 1e-6f && std::abs(p.y-q.y) < 1e-6f;
};
for (auto jt = ring.begin(); jt != ring.end() && !inside; ++jt) {
    if (jt == prev || jt == it || jt == next) continue;
    if (coincident(*jt, A) || coincident(*jt, B) || coincident(*jt, C)) continue;
    if (pointInTriangle(*jt, A, B, C)) inside = true;
}
```

This lets the ear-clipper treat the seam vertices as transparent during the containment check, while still clipping ears correctly in all other respects.

### The algorithm step by step

The polygon vertices are stored in a `std::list<Vec2>` for O(1) removal:

```cpp
std::list<Vec2> ring(poly.begin(), poly.end());
```

Then:

```
while ring.size() > 3:
    for each vertex B in ring:
        A = predecessor of B
        C = successor of B
        if cross(AB, AC) > 0:          // B is convex
            if no non-coincident vertex inside triangle ABC:
                emit triangle (A, B, C)
                remove B from ring
                break  // restart scan from beginning

emit the final 3-vertex triangle
```

The guard counter `(int)ring.size()² + 10` prevents an infinite loop if the polygon is degenerate. In practice, a valid merged polygon always terminates.

### Triangle count

For a simple polygon with N vertices, ear-clipping produces exactly **N - 2** triangles.

For a polygon with a hole merged via bridge (outer N verts + hole H verts → merged N+H+2 verts), the result is **(N+H+2) - 2 = N+H** triangles.

Example from the logs above: 6-vertex outer + 5-vertex hole → 13-vertex merged polygon → **11 triangles**.

---

## Part 5 — Multiple Cuts

A face can be cut multiple times. Each cut adds another entry to `face.holes`. When `rebuildMesh()` is called, `tessellate()` processes each hole in sequence — calling `mergeHole()` once per hole, stitching each one into the growing working polygon before finally running ear-clipping on the entire combined result.

```
working = outer
for each hole H:
    working = mergeHole(working, H)   // adds H+2 vertices each time
earClip(working)
```

The cut shapes themselves also become independent faces. If you cut the inner face again, *its* `performCut()` runs independently — it has its own `holes` list and its own tessellation. The outer face is not involved.

---

## Part 6 — Hit Testing After a Cut

After a cut, clicking in the hole region of the outer face should *not* select the outer face — it should either select the inner face (which now occupies that space) or nothing.

`isPointOnFace()` handles this:

```cpp
bool Face::isPointOnFace(Vec2 p) const
{
    if (!containsPoint(p)) return false;   // outside outer boundary
    for (const Contour& h : holes) {
        // ray-cast against each hole contour
        if (point is inside hole h) return false;
    }
    return true;
}
```

`containsPoint()` uses a ray-casting algorithm against the outer contour: cast a ray rightward from P and count how many outer edges it crosses. An odd count means inside, even means outside.

Then for each hole, the same ray-casting is done against the hole's contour. If the point is inside any hole, `isPointOnFace()` returns `false` — the point is in the "removed" region.

`hitTestFace()` walks the faces array in reverse (most recently created first, which is the inner face from the last cut) and returns the first face that passes `isPointOnFace()`. This means the inner face is checked before the outer one, so clicking in its area selects it rather than the outer ring.

---

## Summary of Data Flow

```
User right-clicks to close cut contour
          │
          ▼
performCut(outerIdx, cutContour)
    ├── new Face (inner)
    │       outer  = cutContour (forced CCW)
    │       holes  = []
    │       rebuildMesh()
    │           └── tessellate(cutContour, [])
    │                   └── earClip(cutContour)
    │                           └── N-2 triangles → GPU
    │
    └── faces[outerIdx] (outer ring)
            holes += cutContour (forced CW)
            rebuildMesh()
                └── tessellate(outer, [hole])
                        ├── mergeHole(outer, hole)
                        │       find M (rightmost of hole)
                        │       cast +X ray → find P on outer
                        │       spliceHole() → 13-vert polygon
                        └── earClip(merged)
                                skip coincident bridge duplicates
                                clip ears until 3 remain
                                └── 11 triangles → GPU
```
