# PROGRESS.md — Session Log

This file tracks what has been done each session, the current state of the project,
known issues, and what to tackle next. Update this at the end of every session.

---

## Current Status

**Overall:** 🔄 In progress — full scene built and rendering. All bonus features except golf hole done.

| Area | Status |
|---|---|
| Project scaffold & build | ✅ Done — compiles with `make` |
| Matrix math (from P1) | ✅ Done — prac1/Matrix.h + Vector.h via math3d.hpp |
| Vertex / Fragment shaders | ✅ Done — MVP + per-vertex colour |
| Camera / projection setup | ✅ Done — perspective + lookAt |
| Course geometry | ✅ Done — smooth ramp, wide area, all walls extend to ground |
| Ground plane | ✅ Done — large green slab at Y=-0.85 |
| Windmill geometry | ✅ Done — lower/upper tower, blue trim, roof, axle, hub |
| Rotor & blades | ✅ Done — 4 blades with white spine, 8 red bars, red paddle |
| WASDQE rotations | ✅ Done — cumulative, correct axes/directions |
| IJKLOU translations | ✅ Done — cumulative |
| +/- rotor speed control | ✅ Done — never goes negative |
| Wireframe (GL_LINES) | ✅ Done — Enter toggles, debounced, colour-preserving |
| Background colour | ✅ Done — sky blue |
| HUD / on-screen text | ✅ Done — mode + speed + controls overlay, confirmed working |
| Golf hole (proper cutout) | ✅ Done — circular cutout in wide area slab + cylindrical cup |
| Course slope / incline | ✅ Done — smooth ramp from Y=0 (north) to Y=-0.6 (south) |
| Golf ball | ✅ Done — UV-sphere on starting mat, confirmed rendering |
| Flowers | ✅ Done — 3 flowers (cone petals + stem + centre) outside walls |
| Trees | ✅ Done — 2 large trees on ground plane |
| Windmill windows | ✅ Done — 2 dark blue windows + light blue caps on tower face |
| Ramp side walls | ✅ Done — solid buildRampWall, extends from ground to wall top |
| All walls extend to ground | ✅ Done — all boundary walls reach Y=-0.85 ground plane |
| Reference photo & planning | 🔄 Photo chosen (windmill.jpg), annotation to be done by student |
| Submission packaging | ⬜ Not started |

Status key: ⬜ Not started | 🔄 In progress | ✅ Done | ❌ Broken / needs fix

---

## Spec Compliance Checklist

### Windmill (10 marks)
- [x] Realistic — matches windmill.jpg (tower, trim, roof, blades)
- [x] Ball gap through lower tower base
- [x] Attached to course at ground level
- [x] 4+ rotor blades

### Course (14 marks)
- [x] Boundary walls — red, distinct from green, extend to ground plane
- [x] Golf hole — circular cutout in wide area + cylindrical cup below
- [x] Starting mat — yellow, distinct colour
- [x] 2+ decorations outside boundary — 2 trees + 3 flowers
- [x] Green grass surface (ramp + wide area + ground plane)

### Shape Requirements (12 marks)
- [x] Cuboid (many — walls, slabs, mat, tower, trim, hub, windows)
- [x] Triangular Prism (roof + blade paddles)
- [x] Cone ≥8 vertices (tree foliage × 2 segs=12, flower petals × 15 segs=8)
- [x] Cylinder ≥3, ≥8 circle verts (rotor axle segs=12, tree trunks × 2 segs=8, flower stems × 3 segs=8, flower centres × 3 segs=8)
- [x] 20+ distinct 3D shapes — well over 20, see SHAPES.md

### Colours (14 marks)
- [x] Grass — green (ramp + wide area + ground)
- [x] Boundary walls — red, distinct
- [x] Blades — white spine + red bars
- [x] Starting mat — yellow
- [x] Rotor axle — metallic grey (0.67, 0.67, 0.67)
- [x] Background — sky blue, not black
- [x] 6+ distinct colours — green, red, yellow, beige, blue, grey, white, dark green, brown, pink = 10+

### Transformations (12+12+8 marks)
- [x] W/S rotate X ±
- [x] A/D rotate Y ±
- [x] Q/E rotate Z ±
- [x] I/K translate Y ±
- [x] J/L translate X ±
- [x] U/O translate Z ±
- [x] All cumulative, never reset
- [x] +/- rotor speed, never negative
- [x] Rotor spins around Z axis (rotateAxis)

### Wireframe (12 marks)
- [x] Enter toggles
- [x] GL_LINES only (glPolygonMode never used)
- [x] Colour preserved
- [x] Follows all transforms
- [x] Debounced (0.25s)

### Reference & Planning (6 marks)
- [x] Reference photo: windmill.jpg
- [ ] Annotated photo showing shape mapping — to be done by student before demo
- [ ] Both photos included in submission archive

### Imports — Clean?
- [x] main.cpp — only allowed headers
- [x] src/*.cpp/hpp — only allowed headers
- [x] shader.cpp — provided file (not our responsibility)
- [x] No GLM math functions used anywhere
- [x] No glPolygonMode anywhere

---

## Bonus Marks

| Bonus (+2 each) | Status | Notes |
|---|---|---|
| Doors/windows on windmill | ✅ Done | 2 dark blue windows + light blue caps on upper tower south face |
| Horizontal-axis rotor | ✅ Done | Rotor spins around Z = horizontal axis |
| Additional course details (flowers) | ✅ Done | 3 flowers (pink petals, yellow centre, green stem) outside walls |
| 3D spherical golf ball | ✅ Done | UV-sphere on starting mat, confirmed rendering |
| Course incline/decline | ✅ Done | Smooth ramp, Y=0 at windmill → Y=-0.6 at starting mat |
| Internal tunnel (instead of gap) | ❌ Not planned | Complex, low priority |

**Confirmed bonus: +8 marks** (windows, flowers, ball, incline)
Rotor axis counts if tutor agrees it qualifies — already horizontal by design.

---

## Session Log

### Session 0 — Project Setup (Pre-coding)
**Date:** 2026-04-08
**What was done:**
- Read full P3 spec. Created CLAUDE.md and all CLAUDE/ memory docs.
- Reviewed provided starter files. Made architecture decisions DEC-001 through DEC-005.

### Session 1 — Pipeline Bootstrap
**Date:** 2026-04-11
**What was done:**
- Created math3d.hpp, VertexShader.glsl, FragmentShader.glsl.
- Full main.cpp with WASDQE + IJKLOU + rotor speed + wireframe + sky-blue background.
- Fixed makefile (shader.cpp casing, removed glad.c, -lglfw3 → -lglfw).

### Session 2 — Full Scene + Refactor
**Date:** 2026-04-11
**What was done:**
- Refactored into src/ (Input, Mesh, Scene, math3d, shapes).
- Built complete scene: channel + wide area slabs, 8 boundary walls, starting mat,
  lower/upper tower with ball gap, 4 blue trim strips, roof, axle, hub, 4 blades.
- Two decorative trees outside boundary walls.
- Windmill flipped to face south (+Z toward player).
- Blade redesign: white spine + 8 progressive red cross-bars + red paddle tip.

### Session 3 — Bonus Features + Scene Polish
**Date:** 2026-04-11
**What was done:**
- Added buildSphere (UV-sphere) and buildFlower to shapes.hpp.
- Added buildRamp (smooth inclined quad) and buildRampWall (solid sloped wall) to shapes.hpp.
- Golf ball (UV-sphere) placed on starting mat — confirmed rendering.
- Smooth ramp replacing stepped channel — Y=0 at north, Y=-0.6 at south.
- Ground plane (Y=-0.85) so trees and flowers have ground to sit on.
- Trees made much larger (trunkH=1.6, foliageH=2.2, foliageR=0.9).
- 3 flowers placed outside boundary walls on ground plane.
- Windmill windows: 2 dark blue + light blue cap cuboids on upper tower south face.
- HUD text overlay: mode (wireframe/solid), rotor speed, controls — confirmed working.
- All boundary walls extended to ground plane (Y=-0.85) for solid appearance.
- Ramp side walls replaced with solid buildRampWall — all faces now rendered.
- South cap wall height corrected to match ramp surface at Z=3.5.
- Wide area walls, junction steps, north cap all extended to ground.
- Golf hole geometry scrapped — redesigned in Session 4.

**Current issues (end of session 3):**
- Golf hole missing — resolved in Session 4.
- Annotated reference photo not yet created (student task).

### Session 4 — Golf Hole Cutout + Cup
**Date:** 2026-04-11
**What was done:**
- Implemented circular cutout in wide area slab using 4 rectangle cuboids (square gap)
  + 4 corner fan meshes (N=8 triangles each) that fill the square corners outside the
  circle arc, leaving a clean circular opening.
- Hole centre: (0, -2.2), radius 0.18. Square gap hSq=hR=0.18 so rectangles stop
  exactly at circle bounding box; corner fans round the opening.
- Built cylindrical cup below the opening: 16 flat quad side faces (light grey 0.75)
  + 16-triangle bottom disc (dark grey 0.25), Y=-0.1 to Y=-0.5.
- Confirmed: looking through the hole shows cup walls and bottom — real gap, not illusion.
- Wireframe shows circle arc outline from corner fan line edges.

**Current issues:**
- Annotated reference photo not yet created (student task).

**Next session — planned work:**
1. Submission packaging — zip archive with all files
2. Final spec compliance check: shape count, colour count, all key bindings
3. Student to annotate reference photo before demo
