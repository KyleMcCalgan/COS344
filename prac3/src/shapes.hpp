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


// ─── Shape: Inclined ramp (smooth quad surface) ──────────────────────────────
// A flat parallelogram-shaped surface for the approach channel slope.
// The four corners are given explicitly so any inclination is possible.
// Winding: viewed from above, corners go counter-clockwise so the top face is lit.
//
//   (x0, yN, zN) ─────── (x1, yN, zN)    ← north edge (high)
//        |                     |
//   (x0, yS, zS) ─────── (x1, yS, zS)    ← south edge (low)
//
// Also emits left/right/front/back edge lines for wireframe.
inline void buildRamp(float x0, float x1,          // west / east X
                      float yN, float zN,            // north-edge Y and Z
                      float yS, float zS,            // south-edge Y and Z
                      float r, float g, float b,
                      std::vector<float>& tris,
                      std::vector<float>& lines)
{
    std::vector<float> vp;
    // 0: SW (x0, yS, zS)   1: SE (x1, yS, zS)
    // 2: NE (x1, yN, zN)   3: NW (x0, yN, zN)
    pushV(vp, x0, yS, zS, r, g, b); // 0
    pushV(vp, x1, yS, zS, r, g, b); // 1
    pushV(vp, x1, yN, zN, r, g, b); // 2
    pushV(vp, x0, yN, zN, r, g, b); // 3

    // Two triangles, top face
    pushTri(tris, vp, 0, 1, 2);
    pushTri(tris, vp, 0, 2, 3);

    // Four edge lines only — no diagonal (the shared triangle edge is internal)
    pushLine(lines, vp, 0, 1); // south edge
    pushLine(lines, vp, 1, 2); // east edge
    pushLine(lines, vp, 2, 3); // north edge
    pushLine(lines, vp, 3, 0); // west edge
}

// ─── Shape: Sloped wall (solid, all faces) ───────────────────────────────────
// A solid wall whose top and bottom faces are inclined along Z.
// Think of it as a cuboid where the four Y corners are individually specified.
//
//  xInner, xOuter — X extents of the wall (e.g. -0.9 and -0.7 for west wall)
//  yTopN,  yTopS  — Y of the top face at the north (Z=zN) and south (Z=zS) ends
//  yBotN,  yBotS  — Y of the bottom face at the north and south ends
//  zN, zS         — Z of the north and south ends
//
// 8 vertices:
//   0: inner-bottom-south   1: outer-bottom-south
//   2: outer-bottom-north   3: inner-bottom-north
//   4: inner-top-south      5: outer-top-south
//   6: outer-top-north      7: inner-top-north
inline void buildRampWall(float xInner, float xOuter,
                          float yTopN,  float yTopS,
                          float yBotN,  float yBotS,
                          float zN,     float zS,
                          float r, float g, float b,
                          std::vector<float>& tris,
                          std::vector<float>& lines)
{
    std::vector<float> vp;
    pushV(vp, xInner, yBotS, zS, r, g, b); // 0
    pushV(vp, xOuter, yBotS, zS, r, g, b); // 1
    pushV(vp, xOuter, yBotN, zN, r, g, b); // 2
    pushV(vp, xInner, yBotN, zN, r, g, b); // 3
    pushV(vp, xInner, yTopS, zS, r, g, b); // 4
    pushV(vp, xOuter, yTopS, zS, r, g, b); // 5
    pushV(vp, xOuter, yTopN, zN, r, g, b); // 6
    pushV(vp, xInner, yTopN, zN, r, g, b); // 7

    // Top face (sloped)
    pushTri(tris, vp, 4, 7, 6);
    pushTri(tris, vp, 4, 6, 5);
    // Bottom face (sloped)
    pushTri(tris, vp, 0, 1, 2);
    pushTri(tris, vp, 0, 2, 3);
    // Inner face (toward channel)
    pushTri(tris, vp, 0, 3, 7);
    pushTri(tris, vp, 0, 7, 4);
    // Outer face (away from channel)
    pushTri(tris, vp, 1, 5, 6);
    pushTri(tris, vp, 1, 6, 2);
    // South end cap
    pushTri(tris, vp, 0, 4, 5);
    pushTri(tris, vp, 0, 5, 1);
    // North end cap
    pushTri(tris, vp, 3, 2, 6);
    pushTri(tris, vp, 3, 6, 7);

    // Wireframe edges
    // Bottom rect
    pushLine(lines, vp, 0, 1); pushLine(lines, vp, 1, 2);
    pushLine(lines, vp, 2, 3); pushLine(lines, vp, 3, 0);
    // Top rect
    pushLine(lines, vp, 4, 5); pushLine(lines, vp, 5, 6);
    pushLine(lines, vp, 6, 7); pushLine(lines, vp, 7, 4);
    // Verticals
    pushLine(lines, vp, 0, 4); pushLine(lines, vp, 1, 5);
    pushLine(lines, vp, 2, 6); pushLine(lines, vp, 3, 7);
}

// ─── Shape: UV-Sphere ────────────────────────────────────────────────────────
// Standard UV-sphere centred at (cx,cy,cz), radius r.
// stacks: latitudinal divisions, slices: longitudinal divisions (both >= 4).
inline void buildSphere(float cx, float cy, float cz,
                        float radius, int stacks, int slices,
                        float r, float g, float b,
                        std::vector<float>& tris,
                        std::vector<float>& lines)
{
    // Generate vertex pool: (stacks+1) rings × (slices+1) verts + shared poles
    // Indexed: row*( slices+1) + col
    std::vector<float> vp;
    int ringCount  = stacks + 1;
    int ringVerts  = slices + 1;

    for (int i = 0; i <= stacks; i++)
    {
        float phi = PI * i / stacks;          // 0 (top) .. PI (bottom)
        float sinP = std::sin(phi);
        float cosP = std::cos(phi);
        for (int j = 0; j <= slices; j++)
        {
            float theta = 2.0f * PI * j / slices;
            float dx = sinP * std::cos(theta);
            float dy = cosP;
            float dz = sinP * std::sin(theta);
            pushV(vp, cx + radius*dx, cy + radius*dy, cz + radius*dz, r, g, b);
        }
    }

    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            int v0 = i       * ringVerts + j;
            int v1 = i       * ringVerts + j + 1;
            int v2 = (i + 1) * ringVerts + j;
            int v3 = (i + 1) * ringVerts + j + 1;

            pushTri(tris, vp, v0, v2, v3);
            pushTri(tris, vp, v0, v3, v1);

            if (j % 2 == 0)
            {
                pushLine(lines, vp, v0, v1);
                pushLine(lines, vp, v0, v2);
            }
        }
    }
}

// ─── Shape: Flower (disc of petals on a cylinder stem) ───────────────────────
// Builds a simple flower: thin Y-cylinder stem + small cone petals arranged
// in a ring at the top of the stem.
// cx,cz       — XZ base position (stem bottom)
// baseY       — Y at ground level
// stemH       — height of stem
// stemR       — stem radius
// numPetals   — how many cone petals to arrange (>=4)
// petalR      — radius of each petal cone
// petalH      — height of each petal cone
// petalOffset — radial distance from stem centre to each petal centre
inline void buildFlower(float cx, float cz, float baseY,
                        float stemH, float stemR,
                        int numPetals, float petalR, float petalH,
                        float petalOffset,
                        std::vector<float>& tris,
                        std::vector<float>& lines)
{
    // Stem — light green thin cylinder, Y-axis
    buildYCylinder(cx, baseY + stemH * 0.5f, cz,
                   stemR, stemH * 0.5f, 8,
                   0.30f, 0.65f, 0.20f,
                   tris, lines);

    // Petals — small cones arranged in a ring at the top of the stem
    float topY = baseY + stemH;
    for (int i = 0; i < numPetals; i++)
    {
        float angle = 2.0f * PI * i / numPetals;
        float px = cx + petalOffset * std::cos(angle);
        float pz = cz + petalOffset * std::sin(angle);
        buildCone(px, pz,
                  topY, topY + petalH,
                  petalR, 8,
                  0.98f, 0.40f, 0.60f,   // pink petals
                  tris, lines);
    }

    // Yellow centre disc on top of stem — small flat cylinder
    buildYCylinder(cx, topY + 0.01f, cz,
                   petalOffset * 0.6f, 0.015f, 8,
                   1.0f, 0.85f, 0.10f,   // yellow
                   tris, lines);
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
