#ifndef SHAPES_HPP
#define SHAPES_HPP

// shapes.hpp — Geometry generators for all primitives.
// Every shape is built from first principles (no GLM shape generators).
//
// Each generator fills:
//   verts   — interleaved [x,y,z,r,g,b] float array  (caller-owned)
//   nVerts  — number of vertices (for glDrawArrays / GL_TRIANGLES)
//   lverts  — interleaved [x,y,z,r,g,b] float array for GL_LINES wireframe
//   nLVerts — number of line vertices
//
// All positions are in local space; the caller applies model transforms.

#include <vector>
#include <cmath>

static const float PI = 3.14159265358979323846f;

// ─── Low-level helpers ────────────────────────────────────────────────────────

// Push a vertex (pos + colour) into a vector
inline void pushV(std::vector<float>& v,
                  float x, float y, float z,
                  float r, float g, float b)
{
    v.push_back(x); v.push_back(y); v.push_back(z);
    v.push_back(r); v.push_back(g); v.push_back(b);
}

// Push a triangle (3 vertex indices) as 3 explicit vertices from a vertex pool
// src: the vertex pool [x,y,z,r,g,b, ...], i0/i1/i2: vertex indices
inline void pushTri(std::vector<float>& out,
                    const std::vector<float>& src,
                    int i0, int i1, int i2)
{
    for (int idx : {i0, i1, i2})
    {
        int base = idx * 6;
        for (int k = 0; k < 6; k++) out.push_back(src[base + k]);
    }
}

// Push a line (2 vertex indices) as 2 explicit vertices from a vertex pool
inline void pushLine(std::vector<float>& out,
                     const std::vector<float>& src,
                     int i0, int i1)
{
    for (int idx : {i0, i1})
    {
        int base = idx * 6;
        for (int k = 0; k < 6; k++) out.push_back(src[base + k]);
    }
}

// ─── Shape: Cuboid ────────────────────────────────────────────────────────────
// Axis-aligned box centred at (cx,cy,cz) with half-extents hx,hy,hz.
// All 6 faces get the same colour (r,g,b).
inline void buildCuboid(float cx, float cy, float cz,
                        float hx, float hy, float hz,
                        float r, float g, float b,
                        std::vector<float>& tris,
                        std::vector<float>& lines)
{
    // 8 corners
    float x0 = cx - hx, x1 = cx + hx;
    float y0 = cy - hy, y1 = cy + hy;
    float z0 = cz - hz, z1 = cz + hz;

    std::vector<float> vp; // vertex pool
    // 0: x0y0z1  1: x1y0z1  2: x1y1z1  3: x0y1z1  (front +Z)
    // 4: x0y0z0  5: x1y0z0  6: x1y1z0  7: x0y1z0  (back  -Z)
    pushV(vp, x0,y0,z1, r,g,b); // 0
    pushV(vp, x1,y0,z1, r,g,b); // 1
    pushV(vp, x1,y1,z1, r,g,b); // 2
    pushV(vp, x0,y1,z1, r,g,b); // 3
    pushV(vp, x0,y0,z0, r,g,b); // 4
    pushV(vp, x1,y0,z0, r,g,b); // 5
    pushV(vp, x1,y1,z0, r,g,b); // 6
    pushV(vp, x0,y1,z0, r,g,b); // 7

    // Triangles (2 per face, 6 faces)
    // Front (+Z)
    pushTri(tris,vp,0,1,2); pushTri(tris,vp,0,2,3);
    // Back (-Z)
    pushTri(tris,vp,5,4,7); pushTri(tris,vp,5,7,6);
    // Left (-X)
    pushTri(tris,vp,4,0,3); pushTri(tris,vp,4,3,7);
    // Right (+X)
    pushTri(tris,vp,1,5,6); pushTri(tris,vp,1,6,2);
    // Top (+Y)
    pushTri(tris,vp,3,2,6); pushTri(tris,vp,3,6,7);
    // Bottom (-Y)
    pushTri(tris,vp,4,5,1); pushTri(tris,vp,4,1,0);

    // Lines (12 edges)
    // Bottom square
    pushLine(lines,vp,0,1); pushLine(lines,vp,1,5);
    pushLine(lines,vp,5,4); pushLine(lines,vp,4,0);
    // Top square
    pushLine(lines,vp,3,2); pushLine(lines,vp,2,6);
    pushLine(lines,vp,6,7); pushLine(lines,vp,7,3);
    // Verticals
    pushLine(lines,vp,0,3); pushLine(lines,vp,1,2);
    pushLine(lines,vp,5,6); pushLine(lines,vp,4,7);
}

// ─── Shape: Triangular Prism (for roof) ──────────────────────────────────────
// Ridge runs along Z axis. Base width bw, height h, depth d.
// Base of the triangle sits at y=baseY; peak at y=baseY+h.
// Centred at x=cx, z=cz (depth centred around cz).
inline void buildTriPrism(float cx, float baseY,
                           float cz, float bw, float h, float d,
                           float r, float g, float b,
                           std::vector<float>& tris,
                           std::vector<float>& lines)
{
    float x0 = cx - bw * 0.5f, x1 = cx + bw * 0.5f;
    float xm = cx;
    float yb = baseY, yt = baseY + h;
    float z0 = cz - d * 0.5f, z1 = cz + d * 0.5f;

    std::vector<float> vp;
    // Front face (z1): 0=BL, 1=BR, 2=Peak
    pushV(vp, x0,yb,z1, r,g,b); // 0
    pushV(vp, x1,yb,z1, r,g,b); // 1
    pushV(vp, xm,yt,z1, r,g,b); // 2
    // Back face (z0): 3=BL, 4=BR, 5=Peak
    pushV(vp, x0,yb,z0, r,g,b); // 3
    pushV(vp, x1,yb,z0, r,g,b); // 4
    pushV(vp, xm,yt,z0, r,g,b); // 5

    // Front triangle
    pushTri(tris,vp,0,1,2);
    // Back triangle (winding reversed)
    pushTri(tris,vp,4,3,5);
    // Bottom rectangle
    pushTri(tris,vp,0,3,4); pushTri(tris,vp,0,4,1);
    // Left slope
    pushTri(tris,vp,3,0,2); pushTri(tris,vp,3,2,5);
    // Right slope
    pushTri(tris,vp,1,4,5); pushTri(tris,vp,1,5,2);

    // Lines
    // Front triangle
    pushLine(lines,vp,0,1); pushLine(lines,vp,1,2); pushLine(lines,vp,2,0);
    // Back triangle
    pushLine(lines,vp,3,4); pushLine(lines,vp,4,5); pushLine(lines,vp,5,3);
    // Ridge & base edges
    pushLine(lines,vp,0,3); pushLine(lines,vp,1,4); pushLine(lines,vp,2,5);
}

// ─── Shape: Cylinder ─────────────────────────────────────────────────────────
// Axis along Z. Centre of the cylinder body at (cx,cy,cz).
// radius r, half-length hl (extends from cz-hl to cz+hl).
// segs: number of segments around circumference (must be >=8).
inline void buildCylinder(float cx, float cy, float cz,
                           float radius, float hl, int segs,
                           float r, float g, float b,
                           std::vector<float>& tris,
                           std::vector<float>& lines)
{
    std::vector<float> vp;
    float z0 = cz - hl, z1 = cz + hl;

    // Centre vertices: 0=back centre, 1=front centre
    pushV(vp, cx,cy,z0, r,g,b); // 0
    pushV(vp, cx,cy,z1, r,g,b); // 1

    // Ring vertices: [2 .. 2+segs-1] = back ring, [2+segs .. 2+2*segs-1] = front ring
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dy = radius * std::sin(angle);
        pushV(vp, cx+dx, cy+dy, z0, r,g,b); // back ring
    }
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dy = radius * std::sin(angle);
        pushV(vp, cx+dx, cy+dy, z1, r,g,b); // front ring
    }

    // Back cap (fan from vertex 0)
    for (int i = 0; i < segs; i++)
    {
        int a = 2 + i;
        int b2 = 2 + (i + 1) % segs;
        pushTri(tris, vp, 0, b2, a); // reversed winding for back face
    }
    // Front cap (fan from vertex 1)
    for (int i = 0; i < segs; i++)
    {
        int a = 2 + segs + i;
        int b2 = 2 + segs + (i + 1) % segs;
        pushTri(tris, vp, 1, a, b2);
    }
    // Side quads
    for (int i = 0; i < segs; i++)
    {
        int a0 = 2 + i;
        int a1 = 2 + (i + 1) % segs;
        int b0 = 2 + segs + i;
        int b1 = 2 + segs + (i + 1) % segs;
        pushTri(tris, vp, a0, b0, b1);
        pushTri(tris, vp, a0, b1, a1);
    }

    // Lines: back ring, front ring, and connecting lines every segment
    for (int i = 0; i < segs; i++)
    {
        int a0 = 2 + i, a1 = 2 + (i+1)%segs;
        int b0 = 2+segs+i, b1 = 2+segs+(i+1)%segs;
        pushLine(lines, vp, a0, a1); // back ring edge
        pushLine(lines, vp, b0, b1); // front ring edge
        // Draw connecting lines only every other segment to avoid clutter
        if (i % 2 == 0)
            pushLine(lines, vp, a0, b0);
    }
}

// ─── Shape: Cone ─────────────────────────────────────────────────────────────
// Base circle in XZ plane at baseY (Y is up here, not Z).
// apex at apexY. Centre at (cx, -, cz).
// segs >= 8.
inline void buildCone(float cx, float cz,
                       float baseY, float apexY,
                       float radius, int segs,
                       float r, float g, float b,
                       std::vector<float>& tris,
                       std::vector<float>& lines)
{
    std::vector<float> vp;

    // 0 = base centre, 1 = apex
    pushV(vp, cx, baseY, cz, r,g,b); // 0
    pushV(vp, cx, apexY, cz, r,g,b); // 1

    // Base ring: vertices 2..2+segs-1
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dz = radius * std::sin(angle);
        pushV(vp, cx+dx, baseY, cz+dz, r,g,b);
    }

    // Base cap (fan)
    for (int i = 0; i < segs; i++)
    {
        int a = 2 + i;
        int b2 = 2 + (i+1) % segs;
        pushTri(tris, vp, 0, b2, a);
    }
    // Side triangles
    for (int i = 0; i < segs; i++)
    {
        int a = 2 + i;
        int b2 = 2 + (i+1) % segs;
        pushTri(tris, vp, a, b2, 1);
    }

    // Lines: base ring + alternating side edges
    for (int i = 0; i < segs; i++)
    {
        int a = 2+i, b2 = 2+(i+1)%segs;
        pushLine(lines, vp, a, b2);
        if (i % 2 == 0)
            pushLine(lines, vp, a, 1);
    }
}

// ─── Shape: Y-axis cylinder (for trees / hole walls) ─────────────────────────
// Axis along Y. Centre at (cx,cy,cz), radius, half-height hl, segs>=8.
inline void buildYCylinder(float cx, float cy, float cz,
                            float radius, float hl, int segs,
                            float r, float g, float b,
                            std::vector<float>& tris,
                            std::vector<float>& lines)
{
    std::vector<float> vp;
    float y0 = cy - hl, y1 = cy + hl;

    pushV(vp, cx, y0, cz, r,g,b); // 0 bottom centre
    pushV(vp, cx, y1, cz, r,g,b); // 1 top centre

    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dz = radius * std::sin(angle);
        pushV(vp, cx+dx, y0, cz+dz, r,g,b); // bottom ring: 2..2+segs-1
    }
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dz = radius * std::sin(angle);
        pushV(vp, cx+dx, y1, cz+dz, r,g,b); // top ring: 2+segs..2+2*segs-1
    }

    // Bottom cap
    for (int i = 0; i < segs; i++)
    {
        int a = 2+i, b2 = 2+(i+1)%segs;
        pushTri(tris, vp, 0, b2, a);
    }
    // Top cap
    for (int i = 0; i < segs; i++)
    {
        int a = 2+segs+i, b2 = 2+segs+(i+1)%segs;
        pushTri(tris, vp, 1, a, b2);
    }
    // Side quads
    for (int i = 0; i < segs; i++)
    {
        int a0=2+i,       a1=2+(i+1)%segs;
        int b0=2+segs+i,  b1=2+segs+(i+1)%segs;
        pushTri(tris, vp, a0, b0, b1);
        pushTri(tris, vp, a0, b1, a1);
    }

    for (int i = 0; i < segs; i++)
    {
        int a0=2+i, a1=2+(i+1)%segs;
        int b0=2+segs+i, b1=2+segs+(i+1)%segs;
        pushLine(lines, vp, a0, a1);
        pushLine(lines, vp, b0, b1);
        if (i%2==0) pushLine(lines, vp, a0, b0);
    }
}

// ─── Shape: Golf hole ────────────────────────────────────────────────────────
// Builds two meshes into the provided buffers:
//
//  surfTris/surfLines — annular green ring on the ground surface. Connects
//    a square outer boundary (halfSq x halfSq) down to the circular hole
//    edge (radius), completely filling the surface with triangles so there
//    are no gaps between the square green strips and the round opening.
//
//  wallTris/wallLines — open-top cylinder going from surfaceY down to
//    (surfaceY - depth), coloured slightly darker to give depth illusion.
//
// cx,cz    — hole centre (XZ)
// surfaceY — Y of the green surface (top of the green slab)
// radius   — hole radius
// halfSq   — half-side of the surrounding square patch (should match the
//            gap left in the surrounding green strips)
// depth    — how far down the hole goes
// segs     — circle segments (>=12 recommended)
// gR,gG,gB — surface ring colour (match the green)
// wallR/G/B — cylinder side wall colour (light grey)
// floorR/G/B — bottom disc colour (dark grey)
inline void buildGolfHole(float cx, float cz,
                           float surfaceY, float radius, float halfSq,
                           float depth, int segs,
                           float gR, float gG, float gB,
                           float wallR, float wallG, float wallB,
                           float floorR, float floorG, float floorB,
                           std::vector<float>& surfTris,
                           std::vector<float>& surfLines,
                           std::vector<float>& wallTris,
                           std::vector<float>& wallLines)
{
    // ── Surface annular ring ──────────────────────────────────────────────────
    // For each circle segment i we have:
    //   inner[i] — point on the circle edge at angle i
    //   outer[i] — point on the square boundary at the same angle
    //
    // We build quads inner[i]→inner[i+1]→outer[i+1]→outer[i], split into
    // two triangles, for each of the segs segments.

    // Helper: given an angle, return the point on the axis-aligned square
    // boundary of half-size halfSq centred at (cx, cz) in the XZ plane.
    auto squarePt = [&](float angle) -> std::pair<float,float>
    {
        float dx = std::cos(angle);
        float dz = std::sin(angle);
        // Scale factor to reach the square boundary from the origin
        float absDx = std::fabs(dx), absDz = std::fabs(dz);
        float t = halfSq / (absDx > absDz ? absDx : absDz);
        return { cx + dx * t, cz + dz * t };
    };

    // Build vertex pool: segs inner + segs outer
    std::vector<float> svp;
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float ix = cx + radius * std::cos(angle);
        float iz = cz + radius * std::sin(angle);
        pushV(svp, ix, surfaceY, iz, gR, gG, gB); // inner ring: 0..segs-1
    }
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        auto [ox, oz] = squarePt(angle);
        pushV(svp, ox, surfaceY, oz, gR, gG, gB); // outer ring: segs..2*segs-1
    }

    // Quad strip between inner and outer rings
    for (int i = 0; i < segs; i++)
    {
        int i0 = i,              i1 = (i+1) % segs;   // inner
        int o0 = segs + i,       o1 = segs + (i+1)%segs; // outer

        // Two triangles per quad
        pushTri(surfTris, svp, i0, o0, o1);
        pushTri(surfTris, svp, i0, o1, i1);

        // Lines: inner edge, outer edge, one radial per segment
        pushLine(surfLines, svp, i0, i1);
        pushLine(surfLines, svp, o0, o1);
        pushLine(surfLines, svp, i0, o0);
    }

    // ── Hole: open side-wall cylinder (light grey) ───────────────────────────
    // Start the cylinder just below the surface so the green annular ring
    // sits on top of it and the dark floor is visible when looking down.
    float yTop = surfaceY - 0.01f;
    float yBot = surfaceY - depth;

    // Top ring (0..segs-1) and bottom ring (segs..2*segs-1), wall colour
    std::vector<float> wvp;
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        float dx = radius * std::cos(angle);
        float dz = radius * std::sin(angle);
        pushV(wvp, cx+dx, yTop, cz+dz, wallR, wallG, wallB);
        pushV(wvp, cx+dx, yBot, cz+dz, wallR, wallG, wallB);
    }

    // Side quads only — no top or bottom cap on this mesh
    for (int i = 0; i < segs; i++)
    {
        int t0 = i,      t1 = (i+1)%segs;
        int b0 = segs+i, b1 = segs+(i+1)%segs;
        pushTri(wallTris, wvp, t0, b0, b1);
        pushTri(wallTris, wvp, t0, b1, t1);
        pushLine(wallLines, wvp, t0, t1);
        pushLine(wallLines, wvp, b0, b1);
        if (i%2==0) pushLine(wallLines, wvp, t0, b0);
    }

    // ── Hole: bottom disc (dark grey) ────────────────────────────────────────
    std::vector<float> fvp;
    pushV(fvp, cx, yBot, cz, floorR, floorG, floorB); // centre: 0
    for (int i = 0; i < segs; i++)
    {
        float angle = 2.0f * PI * i / segs;
        pushV(fvp, cx + radius*std::cos(angle), yBot,
                   cz + radius*std::sin(angle), floorR, floorG, floorB);
    }
    for (int i = 0; i < segs; i++)
    {
        int a = 1+i, b2 = 1+(i+1)%segs;
        pushTri(wallTris, fvp, 0, a, b2);
        pushLine(wallLines, fvp, a, b2);
    }
}

// ─── Shape: Tree (cone on a cylinder) ────────────────────────────────────────
// Convenience: builds trunk (Y-axis cylinder) + foliage (cone) together into same buffers.
inline void buildTree(float cx, float cz,
                      float trunkBaseY, float trunkH, float trunkR,
                      float foliageH, float foliageR,
                      std::vector<float>& tris,
                      std::vector<float>& lines)
{
    // Trunk — dark brown cylinder, Y-axis
    buildYCylinder(cx, trunkBaseY + trunkH*0.5f, cz,
                      trunkR, trunkH*0.5f, 8,
                      0.36f, 0.20f, 0.09f,
                      tris, lines);
    // Foliage — dark green cone, Y-axis, base at top of trunk
    buildCone(cx, cz,
              trunkBaseY + trunkH, trunkBaseY + trunkH + foliageH,
              foliageR, 12,
              0.13f, 0.55f, 0.13f,
              tris, lines);
}

#endif // SHAPES_HPP
