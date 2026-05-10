#include "Face.hpp"
#include "Tessellator.hpp"
#include <algorithm>
#include <cmath>

void Face::rebuildMesh()
{
    std::vector<Vec2> tris2D = tessellate(outer, holes);

    float fr = selected ? std::min(r * 1.6f, 1.0f) : r;
    float fg = selected ? std::min(g * 1.6f, 1.0f) : g;
    float fb = selected ? std::min(b * 1.6f, 1.0f) : b;

    std::vector<float> triVerts, lineVerts;

    for (const Vec2& v : tris2D)
        pushV(triVerts, v.x, v.y, 0.0f, fr, fg, fb);

    // Contour edges: outer (white) + holes (orange)
    auto addEdges = [&](const Contour& c, float lr, float lg, float lb) {
        int n = (int)c.size();
        for (int i = 0; i < n; i++) {
            Vec2 a = c[i], b = c[(i + 1) % n];
            pushV(lineVerts, a.x, a.y, 0.0f, lr, lg, lb);
            pushV(lineVerts, b.x, b.y, 0.0f, lr, lg, lb);
        }
    };
    addEdges(outer, 1.0f, 1.0f, 1.0f);
    for (const Contour& h : holes)
        addEdges(h, 1.0f, 0.6f, 0.1f);

    mesh.free();
    mesh.upload(triVerts, lineVerts);
}

Vec2 Face::centroid() const
{
    Vec2 c{0, 0};
    for (const Vec2& v : outer) { c.x += v.x; c.y += v.y; }
    float n = (float)outer.size();
    return {c.x / n, c.y / n};
}

bool Face::containsPoint(Vec2 p) const
{
    int crossings = 0;
    int n = (int)outer.size();
    for (int i = 0; i < n; i++) {
        Vec2 a = outer[i], b = outer[(i + 1) % n];
        if ((a.y > p.y) != (b.y > p.y)) {
            float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
            if (p.x < x) crossings++;
        }
    }
    return (crossings % 2) == 1;
}

bool Face::isPointOnFace(Vec2 p) const
{
    if (!containsPoint(p)) return false;
    for (const Contour& h : holes) {
        int crossings = 0, n = (int)h.size();
        for (int i = 0; i < n; i++) {
            Vec2 a = h[i], b = h[(i + 1) % n];
            if ((a.y > p.y) != (b.y > p.y)) {
                float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
                if (p.x < x) crossings++;
            }
        }
        if ((crossings % 2) == 1) return false; // inside a hole
    }
    return true;
}

void Face::freeGPU()
{
    mesh.free();
}
