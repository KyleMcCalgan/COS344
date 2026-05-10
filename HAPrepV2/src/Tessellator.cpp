#include "Tessellator.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <cstdio>


// ─── Basic helpers ────────────────────────────────────────────────────────────

static float cross2D(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }

float signedArea(const Contour& c)
{
    float area = 0.0f;
    int n = (int)c.size();
    for (int i = 0; i < n; i++) {
        const Vec2& a = c[i];
        const Vec2& b = c[(i + 1) % n];
        area += (a.x * b.y - b.x * a.y);
    }
    return area * 0.5f;
}

static void printContour(const char* label, const Contour& c)
{
    float area = signedArea(c);
    printf("  %s: %d verts, area=%.4f (%s)\n",
           label, (int)c.size(), area, area > 0 ? "CCW" : area < 0 ? "CW" : "degenerate");
    for (int i = 0; i < (int)c.size(); i++)
        printf("    [%d] (%.4f, %.4f)\n", i, c[i].x, c[i].y);
}

static bool pointInTriangle(Vec2 p, Vec2 a, Vec2 b, Vec2 c)
{
    Vec2 ab{b.x-a.x, b.y-a.y};
    Vec2 bc{c.x-b.x, c.y-b.y};
    Vec2 ca{a.x-c.x, a.y-c.y};
    Vec2 ap{p.x-a.x, p.y-a.y};
    Vec2 bp{p.x-b.x, p.y-b.y};
    Vec2 cp{p.x-c.x, p.y-c.y};
    float d1 = cross2D(ab, ap);
    float d2 = cross2D(bc, bp);
    float d3 = cross2D(ca, cp);
    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hasNeg && hasPos);
}

// Returns x coordinate of intersection of horizontal ray from p in +X direction
// with the segment (a,b), or FLT_MAX if no intersection.
static float raySegmentX(Vec2 p, Vec2 a, Vec2 b)
{
    if ((a.y > p.y) == (b.y > p.y)) return std::numeric_limits<float>::max();
    float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
    if (x < p.x) return std::numeric_limits<float>::max();
    return x;
}

// ─── Bridge-edge merge ────────────────────────────────────────────────────────
// Splices a hole into the working polygon, returning a merged simple polygon.
// M is the rightmost vertex of the hole; P is its index in 'poly'.

static Contour spliceHole(const Contour& poly, int pIdx,
                           const Contour& hole, int mIdx)
{
    Contour result;
    int pn = (int)poly.size();
    int hn = (int)hole.size();

    // poly[0..pIdx] + bridge + hole[mIdx..mIdx] (full loop) + bridge back + poly[pIdx..end]
    for (int i = 0; i <= pIdx; i++)
        result.push_back(poly[i]);
    for (int i = 0; i <= hn; i++)
        result.push_back(hole[(mIdx + i) % hn]);
    for (int i = pIdx; i < pn; i++)
        result.push_back(poly[i]);

    return result;
}

static Contour mergeHole(const Contour& poly, const Contour& hole)
{
    // Find rightmost vertex M of hole
    int mIdx = 0;
    for (int i = 1; i < (int)hole.size(); i++)
        if (hole[i].x > hole[mIdx].x) mIdx = i;
    Vec2 M = hole[mIdx];

    // Cast ray from M in +X direction through poly edges
    // Find the nearest intersection and candidate visible vertex
    float bestX = std::numeric_limits<float>::max();
    int   bestEdgeStart = -1;
    int   pn = (int)poly.size();

    for (int i = 0; i < pn; i++) {
        Vec2 a = poly[i], b = poly[(i + 1) % pn];
        float ix = raySegmentX(M, a, b);
        if (ix < bestX) {
            bestX = ix;
            bestEdgeStart = i;
        }
    }

    // P is the vertex of the edge with the larger x (the "reflex" candidate)
    int pIdx;
    if (bestEdgeStart < 0) {
        // Fallback: just pick the closest poly vertex by distance
        float bestDist = std::numeric_limits<float>::max();
        pIdx = 0;
        for (int i = 0; i < pn; i++) {
            float dx = poly[i].x - M.x, dy = poly[i].y - M.y;
            float d = dx*dx + dy*dy;
            if (d < bestDist) { bestDist = d; pIdx = i; }
        }
    } else {
        Vec2 a = poly[bestEdgeStart], b = poly[(bestEdgeStart + 1) % pn];
        // Pick the vertex of the edge with max x (more likely to be mutually visible)
        if (a.x >= b.x)
            pIdx = bestEdgeStart;
        else
            pIdx = (bestEdgeStart + 1) % pn;

        // Among reflex vertices inside triangle(M, intersection, poly[pIdx]),
        // pick the one minimising the angle from M if any exist (robustness)
        Vec2 P = poly[pIdx];
        Vec2 I = {bestX, M.y};
        float bestAngle = std::numeric_limits<float>::max();
        for (int i = 0; i < pn; i++) {
            if (i == pIdx) continue;
            Vec2 v = poly[i];
            if (pointInTriangle(v, M, I, P)) {
                float dx = v.x - M.x, dy = v.y - M.y;
                float angle = std::atan2(std::abs(dy), dx);
                if (angle < bestAngle) { bestAngle = angle; pIdx = i; }
            }
        }
    }

    return spliceHole(poly, pIdx, hole, mIdx);
}

// ─── Ear-clipping ─────────────────────────────────────────────────────────────

static std::vector<Vec2> earClip(Contour poly)
{
    std::vector<Vec2> triangles;
    if (poly.size() < 3) return triangles;

    printf("  earClip: input %d verts\n", (int)poly.size());
    int expectedTris = (int)poly.size() - 2;

    std::list<Vec2> ring(poly.begin(), poly.end());

    int earNum = 0;
    int guard = (int)ring.size() * (int)ring.size() + 10;
    while (ring.size() > 3 && guard-- > 0) {
        bool clipped = false;
        for (auto it = ring.begin(); it != ring.end(); ++it) {
            auto prev = (it == ring.begin()) ? std::prev(ring.end()) : std::prev(it);
            auto next = std::next(it);
            if (next == ring.end()) next = ring.begin();

            Vec2 A = *prev, B = *it, C = *next;

            Vec2 ab{B.x-A.x, B.y-A.y}, ac{C.x-A.x, C.y-A.y};
            if (cross2D(ab, ac) <= 0.0f) continue;

            // Skip vertices coincident with A/B/C (bridge duplicates) — they
            // would falsely block every ear in a bridge-merged polygon.
            static const float EPS = 1e-6f;
            auto coincident = [](Vec2 p, Vec2 q) {
                return std::abs(p.x-q.x) < EPS && std::abs(p.y-q.y) < EPS;
            };
            bool inside = false;
            for (auto jt = ring.begin(); jt != ring.end() && !inside; ++jt) {
                if (jt == prev || jt == it || jt == next) continue;
                if (coincident(*jt, A) || coincident(*jt, B) || coincident(*jt, C)) continue;
                if (pointInTriangle(*jt, A, B, C)) inside = true;
            }
            if (inside) continue;

            printf("    ear %d: (%.3f,%.3f)-(%.3f,%.3f)-(%.3f,%.3f)\n",
                   earNum++, A.x,A.y, B.x,B.y, C.x,C.y);
            triangles.push_back(A);
            triangles.push_back(B);
            triangles.push_back(C);
            ring.erase(it);
            clipped = true;
            break;
        }
        if (!clipped) {
            printf("  earClip: STUCK at %d verts remaining (no ear found) — bailing\n",
                   (int)ring.size());
            break;
        }
    }

    if (ring.size() == 3) {
        auto it = ring.begin();
        Vec2 A = *it++, B = *it++, C = *it;
        printf("    ear %d (last): (%.3f,%.3f)-(%.3f,%.3f)-(%.3f,%.3f)\n",
               earNum, A.x,A.y, B.x,B.y, C.x,C.y);
        triangles.push_back(A);
        triangles.push_back(B);
        triangles.push_back(C);
    } else if (ring.size() != 0) {
        printf("  earClip: finished with %d verts remaining (expected 0 or 3)\n",
               (int)ring.size());
    }

    printf("  earClip: produced %d triangles (expected %d)\n",
           (int)triangles.size() / 3, expectedTris);
    return triangles;
}

// ─── Public entry point ───────────────────────────────────────────────────────

std::vector<Vec2> tessellate(const Contour& outer,
                              const std::vector<Contour>& holes)
{
    printf("\n=== tessellate: %d outer verts, %d holes ===\n",
           (int)outer.size(), (int)holes.size());
    printContour("outer (raw)", outer);

    Contour working = outer;

    if (signedArea(working) < 0) {
        printf("  outer was CW — reversing to CCW\n");
        std::reverse(working.begin(), working.end());
    }

    for (int hi = 0; hi < (int)holes.size(); hi++) {
        char label[32];
        snprintf(label, sizeof(label), "hole[%d] (raw)", hi);
        printContour(label, holes[hi]);

        Contour hole = holes[hi];
        if (signedArea(hole) > 0) {
            printf("  hole[%d] was CCW — reversing to CW\n", hi);
            std::reverse(hole.begin(), hole.end());
        }

        working = mergeHole(working, hole);
        printf("  after merging hole[%d]: working polygon has %d verts\n",
               hi, (int)working.size());
        printContour("working after merge", working);
    }

    printf("  sending %d-vert polygon to ear-clipper\n", (int)working.size());
    auto result = earClip(working);
    printf("=== tessellate done: %d triangles ===\n\n", (int)result.size() / 3);
    return result;
}
