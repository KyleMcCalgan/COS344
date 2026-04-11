#include "Scene.hpp"
#include "shapes.hpp"
#include <cmath>

// ─── Internal helper: add a mesh entry ────────────────────────────────────────
static void addMesh(std::vector<MeshEntry>& entries,
                    const std::vector<float>& tris,
                    const std::vector<float>& lines,
                    bool isRotor = false)
{
    MeshEntry e;
    e.mesh.upload(tris, lines);
    e.isRotor = isRotor;
    entries.push_back(std::move(e));
}

// ─── Scene::build ─────────────────────────────────────────────────────────────
void Scene::build()
{
    std::vector<float> t, l;

    // ── Ground plane (large flat green slab, below everything) ───────────────
    // Sits at Y = -0.85 (below the lowest ramp point), extends well beyond
    // all boundary walls so trees and flowers have ground to stand on.
    t.clear(); l.clear();
    buildCuboid(0.0f, -0.88f, -0.5f,  6.0f, 0.03f, 5.5f,
                0.12f, 0.50f, 0.12f, t, l);
    addMesh(m_entries, t, l);

    // ── Course: channel ramp (green, smooth incline) ──────────────────────────
    // A single smooth quad surface sloping from Y=0 at the north junction
    // (Z=0) down to Y=-0.6 at the south end (Z=+3.5).
    // Width matches the channel interior: X = -0.7 .. +0.7.
    t.clear(); l.clear();
    buildRamp(-0.7f, 0.7f,
              0.0f,  0.0f,   // north edge: Y=0, Z=0
             -0.6f,  3.5f,   // south edge: Y=-0.6, Z=+3.5
              0.18f, 0.65f, 0.18f, t, l);
    addMesh(m_entries, t, l);

    // Side skirt walls under the ramp — thin cuboids closing the underside gap.
    // ── Course: wide area slab with circular hole ─────────────────────────────
    // The slab is built as 4 rectangles leaving a square gap, then the square
    // corners are carved away by corner fans that fill the corner material
    // outside the circle — leaving a circular opening.
    //
    // Hole centre: (hcx=0, hcz=-2.2), radius hR=0.18
    // Square gap half-size hSq = hR (rectangles stop at circle bounding box)
    //
    // Corner fans: each of the 4 square corners has N triangles.
    // Each triangle: apex = square corner point, two base vertices = adjacent
    // circle arc points. This REMOVES the corner material (the triangles cover
    // the region between the circle arc and the square corner), so the opening
    // becomes rounded.
    {
        float gR=0.18f, gG=0.65f, gB=0.18f;
        float topY=0.0f, botY=-0.1f;
        float slabHY=(topY-botY)*0.5f, slabCY=(topY+botY)*0.5f;
        float hcx=0.0f, hcz=-2.2f, hR=0.18f;
        float hSq = hR; // square gap = circle bounding box

        // ── 4 rectangles leaving square gap ──────────────────────────────────
        // West slab
        { float x0=-2.0f, x1=-hSq;
          t.clear(); l.clear();
          buildCuboid((x0+x1)*0.5f, slabCY, -1.75f, (x1-x0)*0.5f, slabHY, 1.75f, gR,gG,gB, t, l);
          addMesh(m_entries, t, l); }
        // East slab
        { float x0=hSq, x1=2.0f;
          t.clear(); l.clear();
          buildCuboid((x0+x1)*0.5f, slabCY, -1.75f, (x1-x0)*0.5f, slabHY, 1.75f, gR,gG,gB, t, l);
          addMesh(m_entries, t, l); }
        // South centre strip
        { float z0=0.0f, z1=hcz+hSq;
          t.clear(); l.clear();
          buildCuboid(hcx, slabCY, (z0+z1)*0.5f, hSq, slabHY, (z0-z1)*0.5f, gR,gG,gB, t, l);
          addMesh(m_entries, t, l); }
        // North centre strip
        { float z0=hcz-hSq, z1=-3.5f;
          t.clear(); l.clear();
          buildCuboid(hcx, slabCY, (z0+z1)*0.5f, hSq, slabHY, (z1-z0)*0.5f, gR,gG,gB, t, l);
          addMesh(m_entries, t, l); }

        // ── Corner fans: fill the 4 corner regions outside the circle ────────
        // Each fan covers a 90° arc. The triangles have:
        //   apex    = the square corner (outside the circle)
        //   base p0 = circle point at start of arc
        //   base p1 = circle point at end of arc
        // These triangles cover the gap between the circle arc and the square
        // corner, so only the circular opening remains.
        //
        // Angle convention: cos maps to X, sin maps to Z (in world coords).
        // SW corner (+X,+Z quadrant): arc 0° → 90°  (circle right → circle south)
        // SE corner (-X,+Z quadrant): arc 90° → 180°
        // NE corner (-X,-Z quadrant): arc 180° → 270°
        // NW corner (+X,-Z quadrant): arc 270° → 360°
        int N = 8; // triangles per corner

        struct Corner { float sx, sz, aStart, aEnd; };
        Corner corners[4] = {
            { hcx+hSq, hcz+hSq,  0.0f,      PI*0.5f  },
            { hcx-hSq, hcz+hSq,  PI*0.5f,   PI       },
            { hcx-hSq, hcz-hSq,  PI,         PI*1.5f  },
            { hcx+hSq, hcz-hSq,  PI*1.5f,   PI*2.0f  },
        };

        for (auto& c : corners)
        {
            t.clear(); l.clear();
            for (int i = 0; i < N; i++)
            {
                float a0 = c.aStart + (c.aEnd - c.aStart) * (float)i       / N;
                float a1 = c.aStart + (c.aEnd - c.aStart) * (float)(i + 1) / N;

                float p0x = hcx + hR * std::cos(a0), p0z = hcz + hR * std::sin(a0);
                float p1x = hcx + hR * std::cos(a1), p1z = hcz + hR * std::sin(a1);

                std::vector<float> vp;
                // Top face: apex → p0 → p1 (CCW from above)
                pushV(vp, c.sx, topY, c.sz,  gR,gG,gB); // 0 apex
                pushV(vp, p0x,  topY, p0z,   gR,gG,gB); // 1
                pushV(vp, p1x,  topY, p1z,   gR,gG,gB); // 2
                pushTri(t, vp, 0, 1, 2);

                // Bottom face (reversed winding)
                std::vector<float> bp;
                pushV(bp, c.sx, botY, c.sz,  gR,gG,gB);
                pushV(bp, p0x,  botY, p0z,   gR,gG,gB);
                pushV(bp, p1x,  botY, p1z,   gR,gG,gB);
                pushTri(t, bp, 0, 2, 1);

                // Wireframe: only the circle arc edge (inner boundary of fan)
                pushLine(l, vp, 1, 2);
            }
            addMesh(m_entries, t, l);
        }
    }

    // ── Golf hole cup: cylindrical walls + dark bottom ───────────────────────
    // N flat quad faces arranged in a circle form the cup wall (light grey).
    // A flat disc (dark grey) closes the bottom.
    // Radius matches the hole opening: hR=0.18. Top flush with slab bottom (Y=-0.1).
    {
        float hcx=0.0f, hcz=-2.2f, hR=0.18f;
        float yTop=-0.1f, yBot=-0.5f;
        int N = 16; // faces — same as hole circle segments

        float wallR = 0.75f, wallG = 0.75f, wallB = 0.75f; // light grey walls
        float botR  = 0.25f, botG  = 0.25f, botB  = 0.25f; // dark grey bottom

        t.clear(); l.clear();

        // Side walls: N quads, each spanning one arc segment top→bottom
        for (int i = 0; i < N; i++)
        {
            float a0 = 2.0f * PI * (float)i       / N;
            float a1 = 2.0f * PI * (float)(i + 1) / N;

            float x0 = hcx + hR * std::cos(a0), z0 = hcz + hR * std::sin(a0);
            float x1 = hcx + hR * std::cos(a1), z1 = hcz + hR * std::sin(a1);

            std::vector<float> vp;
            // 0: top-left  1: top-right  2: bot-right  3: bot-left
            pushV(vp, x0, yTop, z0,  wallR,wallG,wallB); // 0
            pushV(vp, x1, yTop, z1,  wallR,wallG,wallB); // 1
            pushV(vp, x1, yBot, z1,  wallR,wallG,wallB); // 2
            pushV(vp, x0, yBot, z0,  wallR,wallG,wallB); // 3

            // Inward-facing quad (normal points toward centre)
            pushTri(t, vp, 0, 2, 1);
            pushTri(t, vp, 0, 3, 2);

            // Wireframe: top edge + verticals
            pushLine(l, vp, 0, 1);
            pushLine(l, vp, 0, 3);
        }

        // Bottom disc: fan of N triangles from centre
        for (int i = 0; i < N; i++)
        {
            float a0 = 2.0f * PI * (float)i       / N;
            float a1 = 2.0f * PI * (float)(i + 1) / N;

            float x0 = hcx + hR * std::cos(a0), z0 = hcz + hR * std::sin(a0);
            float x1 = hcx + hR * std::cos(a1), z1 = hcz + hR * std::sin(a1);

            std::vector<float> vp;
            pushV(vp, hcx, yBot, hcz,  botR,botG,botB); // 0 centre
            pushV(vp, x0,  yBot, z0,   botR,botG,botB); // 1
            pushV(vp, x1,  yBot, z1,   botR,botG,botB); // 2

            pushTri(t, vp, 0, 1, 2);
            pushLine(l, vp, 1, 2);
        }

        addMesh(m_entries, t, l);
    }

    // ── Boundary walls (red cuboids, 0.2 thick, 0.4 tall) ────────────────────
    auto wall = [&](float cx, float cy, float cz,
                    float hx, float hy, float hz)
    {
        t.clear(); l.clear();
        buildCuboid(cx, cy, cz, hx, hy, hz, 0.78f, 0.15f, 0.15f, t, l);
        addMesh(m_entries, t, l);
    };

    // ── Boundary walls ────────────────────────────────────────────────────────
    // Ramp: Y=0 at Z=0 (north), Y=-0.6 at Z=3.5 (south).
    // Channel interior: X = -0.7 .. +0.7.
    // Boundary walls sit on top of (and flush with) the ramp edges.
    // wallTop = 0.4 above the ramp surface. Wall is 0.2 thick outboard of the ramp.
    //
    // Each sloped side wall is built as TWO ramp quads stacked:
    //   - inner face (facing the channel)
    //   - outer face (facing outside)
    // plus a top-face ramp quad. For simplicity we use buildRamp for each face.
    // The wall outer edge is at X = ±(0.7 + 0.2) = ±0.9.
    // Wall top runs from  Y=0+0.4=0.4 at Z=0  to  Y=-0.6+0.4=-0.2 at Z=3.5.
    // Wall bottom (ramp surface) runs from Y=0 at Z=0 to Y=-0.6 at Z=3.5.
    {
        float wR = 0.78f, wG = 0.15f, wB = 0.15f; // red
        float wallH = 0.4f;  // wall height above ramp

        // Ramp surface Y at north (Z=0) and south (Z=3.5)
        float yRN = 0.0f,  zRN = 0.0f;
        float yRS = -0.6f, zRS = 3.5f;

        // Wall top Y = ramp Y + wallH
        float yTN = yRN + wallH;   // +0.4 at north
        float yTS = yRS + wallH;   // -0.2 at south

        float gndY = -0.85f; // ground plane — walls extend down to here

        // ── West boundary wall — extends from ground up to ramp top ──────────
        t.clear(); l.clear();
        buildRampWall(-0.9f, -0.7f,
                      yTN,  yTS,    // top Y: sloped (ramp surface + wallH)
                      gndY, gndY,   // bottom Y: flat at ground plane
                      zRN,  zRS,
                      wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // ── East boundary wall — extends from ground up to ramp top ──────────
        t.clear(); l.clear();
        buildRampWall(0.7f, 0.9f,
                      yTN,  yTS,
                      gndY, gndY,
                      zRN,  zRS,
                      wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // All remaining walls extend from ground plane (Y=-0.85) up to
        // surface + wallH (Y=0.4). Half-height = (0.4-(-0.85))/2 = 0.625,
        // centre Y = (-0.85+0.4)/2 = -0.225.
        float fullHY = 0.625f;
        float fullCY = -0.225f;

        // South cap wall — top matches ramp surface at Z=3.5 (Y=-0.6) + wallH=0.4
        // Top Y = -0.2, bottom Y = -0.85, half-height = 0.325, centre Y = -0.525
        t.clear(); l.clear();
        buildCuboid(0.0f, -0.525f, 3.6f,  0.9f, 0.325f, 0.1f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // Junction steps
        t.clear(); l.clear();
        buildCuboid(-1.5f, fullCY, 0.05f,  0.6f, fullHY, 0.1f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);
        t.clear(); l.clear();
        buildCuboid( 1.5f, fullCY, 0.05f,  0.6f, fullHY, 0.1f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // West / East wide walls
        t.clear(); l.clear();
        buildCuboid(-2.1f, fullCY, -1.75f,  0.1f, fullHY, 1.75f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);
        t.clear(); l.clear();
        buildCuboid( 2.1f, fullCY, -1.75f,  0.1f, fullHY, 1.75f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // North cap wall
        t.clear(); l.clear();
        buildCuboid(0.0f, fullCY, -3.6f,   2.2f, fullHY, 0.1f, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);
    }


    // ── Starting mat (yellow) — sits on south end of ramp (Y = -0.6) ─────────
    t.clear(); l.clear();
    buildCuboid(0.0f, -0.62f, 3.375f,  0.45f, 0.02f, 0.25f,
                0.93f, 0.83f, 0.10f, t, l);
    addMesh(m_entries, t, l);

    // ── Windmill: lower tower (beige, split around ball gap) ──────────────────
    // Full tower: width=1.1(hx=0.55), height=1.8, depth=1.0(hz=0.5)
    // Gap: centred at X=0, Y=0..0.45, width=0.55(half=0.275)
    {
        float bw = 0.55f, gw = 0.275f, gh = 0.45f, th = 1.8f, td = 0.5f;
        float bR = 0.85f, bG = 0.82f, bB = 0.69f;

        // Left column  (x: -0.55 .. -0.275, full height)
        t.clear(); l.clear();
        buildCuboid(-(bw+gw)*0.5f, th*0.5f, 0.0f,
                    (bw-gw)*0.5f,  th*0.5f, td, bR, bG, bB, t, l);
        addMesh(m_entries, t, l);

        // Right column
        t.clear(); l.clear();
        buildCuboid( (bw+gw)*0.5f, th*0.5f, 0.0f,
                    (bw-gw)*0.5f,  th*0.5f, td, bR, bG, bB, t, l);
        addMesh(m_entries, t, l);

        // Upper bar (above gap, full width, from gh to th)
        t.clear(); l.clear();
        buildCuboid(0.0f, gh + (th-gh)*0.5f, 0.0f,
                    gw,   (th-gh)*0.5f,       td, bR, bG, bB, t, l);
        addMesh(m_entries, t, l);
    }

    // ── Windmill: upper tower body (beige) ────────────────────────────────────
    // Width=0.9(hx=0.45), Height=1.4(hy=0.7), Depth=0.9(hz=0.45); base Y=1.8 → cy=2.5
    t.clear(); l.clear();
    buildCuboid(0.0f, 2.5f, 0.0f,  0.45f, 0.7f, 0.45f,
                0.85f, 0.82f, 0.69f, t, l);
    addMesh(m_entries, t, l);

    // ── Windmill: blue trim strips (4 thin cuboids) ───────────────────────────
    {
        float tR = 0.23f, tG = 0.43f, tB = 0.71f;
        // Lower-left / lower-right: flush against lower tower (hx=0.55)
        float lbx = 0.575f; // 0.55 + 0.025 (half trim width)
        t.clear(); l.clear();
        buildCuboid(-lbx, 0.9f, 0.0f,  0.05f, 0.9f, 0.5f, tR, tG, tB, t, l);
        addMesh(m_entries, t, l);

        t.clear(); l.clear();
        buildCuboid( lbx, 0.9f, 0.0f,  0.05f, 0.9f, 0.5f, tR, tG, tB, t, l);
        addMesh(m_entries, t, l);

        // Upper-left / upper-right: flush against upper tower (hx=0.45)
        float ubx = 0.475f; // 0.45 + 0.025
        t.clear(); l.clear();
        buildCuboid(-ubx, 2.5f, 0.0f,  0.05f, 0.7f, 0.45f, tR, tG, tB, t, l);
        addMesh(m_entries, t, l);

        t.clear(); l.clear();
        buildCuboid( ubx, 2.5f, 0.0f,  0.05f, 0.7f, 0.45f, tR, tG, tB, t, l);
        addMesh(m_entries, t, l);
    }

    // ── Windmill: roof (triangular prism, dark grey) ──────────────────────────
    // Base width=0.9, height=0.8, depth=0.9; base at Y=3.2
    t.clear(); l.clear();
    buildTriPrism(0.0f, 3.2f, 0.0f,  0.9f, 0.8f, 0.9f,
                  0.38f, 0.38f, 0.38f, t, l);
    addMesh(m_entries, t, l);

    // ── Windmill: rotor axle (Z-axis cylinder, metallic grey) ─────────────────
    // Windmill faces south (+Z toward player). Front face of upper tower = Z=+0.45.
    // Axle protrudes from Z=+0.45 to Z=+0.80; centre at Z=+0.625, hl=0.175.
    t.clear(); l.clear();
    buildCylinder(0.0f, 2.5f, 0.625f,  0.08f, 0.175f, 12,
                  0.67f, 0.67f, 0.67f, t, l);
    addMesh(m_entries, t, l);

    // ── Windmill: rotor hub (red cuboid) ──────────────────────────────────────
    t.clear(); l.clear();
    buildCuboid(HUB_X, HUB_Y, HUB_Z,  0.14f, 0.14f, 0.06f,
                0.87f, 0.20f, 0.13f, t, l);
    addMesh(m_entries, t, l, /*isRotor=*/true);

    // ── Windmill: 4 blades ────────────────────────────────────────────────────
    // Each blade (per reference photo) has:
    //   - A narrow white spine (cuboid) running hub → paddle
    //   - 4 red cross-bars perpendicular to the spine, evenly spaced,
    //     starting at spine-width and growing progressively wider toward the tip
    //   - A wide red paddle (triangular prism) at the outer end
    //
    // Built in local space: hub at world position, blade spine extends along +Y.
    // Each blade is then rotated into its angular position around the hub.
    {
        float bladeAngles[4] = {45.0f, 135.0f, 225.0f, 315.0f};

        float bladeLen  = 1.2f;   // spine length hub→paddle base
        float spineHX   = 0.04f;  // half-width of white spine
        float spineHZ   = 0.025f; // half-depth of spine

        // Cross-bars: 8 bars evenly spaced along the spine
        // Bar half-width grows linearly from spineHX at bar 0 to barMaxHX at bar 7
        int   numBars   = 8;
        float barHZ     = 0.025f; // same depth as spine
        float barHY     = 0.025f; // bar thickness (half)
        float barMinHX  = spineHX;        // narrowest bar (at base) = spine width
        float barMaxHX  = spineHX * 3.5f; // widest bar (at tip)

        float paddleW   = spineHX * 5.0f; // paddle base width (half = paddleW)
        float paddleH   = 0.22f;          // paddle triangle height
        float paddleHZ  = 0.03f;

        float wR = 1.0f,  wG = 1.0f,  wB = 1.0f;  // white spine
        float rR = 0.87f, rG = 0.20f, rB = 0.13f;  // red bars + paddle

        for (int i = 0; i < 4; i++)
        {
            float angleRad = bladeAngles[i] * static_cast<float>(M_PI) / 180.0f;
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);

            auto rotateVerts = [&](std::vector<float>& v)
            {
                for (size_t idx = 0; idx < v.size(); idx += 6)
                {
                    float lx = v[idx]   - HUB_X;
                    float ly = v[idx+1] - HUB_Y;
                    v[idx]   = HUB_X + lx * cosA - ly * sinA;
                    v[idx+1] = HUB_Y + lx * sinA + ly * cosA;
                }
            };

            // White spine (sits behind bars in Z)
            {
                std::vector<float> st, sl;
                buildCuboid(HUB_X, HUB_Y + bladeLen * 0.5f, HUB_Z,
                            spineHX, bladeLen * 0.5f, spineHZ,
                            wR, wG, wB, st, sl);
                rotateVerts(st); rotateVerts(sl);
                addMesh(m_entries, st, sl, true);
            }

            // Red paddle (wide triangular prism) at the tip
            {
                std::vector<float> pt, pl;
                buildTriPrism(HUB_X, HUB_Y + bladeLen, HUB_Z,
                              paddleW * 2.0f, paddleH, paddleHZ * 2.0f,
                              rR, rG, rB, pt, pl);
                rotateVerts(pt); rotateVerts(pl);
                addMesh(m_entries, pt, pl, true);
            }

            // Red cross-bars — drawn last so they render in front of the spine.
            // Offset slightly toward viewer (+Z) to avoid z-fighting.
            for (int b = 0; b < numBars; b++)
            {
                float frac  = (float)b / (numBars - 1);
                float barY  = HUB_Y + bladeLen * (frac * 0.85f + 0.1f);
                float barHX = barMinHX + frac * (barMaxHX - barMinHX);

                std::vector<float> bt, bl;
                buildCuboid(HUB_X, barY, HUB_Z + 0.015f,
                            barHX, barHY, barHZ,
                            rR, rG, rB, bt, bl);
                rotateVerts(bt); rotateVerts(bl);
                addMesh(m_entries, bt, bl, true);
            }
        }
    }

    // ── Windmill: windows (flat dark blue cuboids on south tower face) ────────
    // Two small square windows on the upper tower south face (Z=+0.45)
    // Each window is a very thin cuboid sitting proud of the tower face by 0.01.
    {
        float winZ    = 0.46f;   // just in front of tower south face
        float winHX   = 0.08f;
        float winHY   = 0.08f;
        float winHZ   = 0.01f;
        float wR = 0.10f, wG = 0.20f, wB = 0.45f; // dark blue window

        // Left window
        t.clear(); l.clear();
        buildCuboid(-0.18f, 2.8f, winZ,  winHX, winHY, winHZ, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // Right window
        t.clear(); l.clear();
        buildCuboid( 0.18f, 2.8f, winZ,  winHX, winHY, winHZ, wR, wG, wB, t, l);
        addMesh(m_entries, t, l);

        // Small arched cap above each window — thin cuboid, lighter blue
        float capR = 0.30f, capG = 0.55f, capB = 0.85f;
        t.clear(); l.clear();
        buildCuboid(-0.18f, 2.90f, winZ,  winHX, 0.025f, winHZ, capR, capG, capB, t, l);
        addMesh(m_entries, t, l);

        t.clear(); l.clear();
        buildCuboid( 0.18f, 2.90f, winZ,  winHX, 0.025f, winHZ, capR, capG, capB, t, l);
        addMesh(m_entries, t, l);
    }

    // ── Golf ball (UV-sphere, white) — sits on starting mat ──────────────────
    // Mat top Y = -0.60. Ball radius = 0.10, so centre Y = -0.60 + 0.10 = -0.50.
    t.clear(); l.clear();
    buildSphere(0.0f, -0.50f, 3.375f,
                0.10f, 10, 16,
                0.97f, 0.97f, 0.97f, t, l);
    addMesh(m_entries, t, l);

    // ── Decorations: two trees sitting on the ground plane (Y = -0.85) ────────
    t.clear(); l.clear();
    buildTree(-3.2f, -1.0f, -0.85f, 1.6f, 0.18f, 2.2f, 0.9f, t, l);
    addMesh(m_entries, t, l);

    t.clear(); l.clear();
    buildTree( 3.2f, -1.0f, -0.85f, 1.6f, 0.18f, 2.2f, 0.9f, t, l);
    addMesh(m_entries, t, l);

    // ── Flowers — placed outside boundary walls, on ground plane ─────────────
    t.clear(); l.clear();
    buildFlower(-3.0f, 1.5f, -0.85f,
                0.4f, 0.025f,
                5, 0.07f, 0.12f, 0.10f,
                t, l);
    addMesh(m_entries, t, l);

    t.clear(); l.clear();
    buildFlower(-3.4f, -0.5f, -0.85f,
                0.35f, 0.025f,
                5, 0.06f, 0.10f, 0.09f,
                t, l);
    addMesh(m_entries, t, l);

    t.clear(); l.clear();
    buildFlower( 3.0f, 0.8f, -0.85f,
                0.4f, 0.025f,
                5, 0.07f, 0.12f, 0.10f,
                t, l);
    addMesh(m_entries, t, l);
}

// ─── Scene::draw ──────────────────────────────────────────────────────────────
void Scene::draw(GLuint shaderProgram, const InputState& input, float dt)
{
    m_rotorAngle += input.rotorSpeed * dt;

    // Scene model: translation then rotations (cumulative, never resets)
    Mat4 sceneModel = translate(input.transX, input.transY, input.transZ)
                    * rotateX(input.rotX)
                    * rotateY(input.rotY)
                    * rotateZ(input.rotZ);

    // Camera (fixed)
    Vec3 eye    = { 3.5f,  5.0f,  9.5f };
    Vec3 center = { 0.0f,  1.0f,  0.0f };
    Vec3 up     = { 0.0f,  1.0f,  0.0f };
    Mat4 proj   = perspective(0.7854f, 1.0f, 0.1f, 100.0f);
    Mat4 view   = lookAt(eye, center, up);

    Mat4 sceneMVP = proj * view * sceneModel;

    // Rotor: translate to hub → spin around Z (−Z = toward viewer) → translate back
    Mat4 toHub   = translate( HUB_X,  HUB_Y,  HUB_Z);
    Mat4 fromHub = translate(-HUB_X, -HUB_Y, -HUB_Z);
    Mat4 spin    = rotateAxis(m_rotorAngle, 0.0f, 0.0f, 1.0f);
    Mat4 rotorMVP = proj * view * sceneModel * (toHub * spin * fromHub);

    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");

    float arr[16];
    for (const MeshEntry& e : m_entries)
    {
        mat4ToArray(e.isRotor ? rotorMVP : sceneMVP, arr);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, arr);
        e.mesh.draw(input.wireframe);
    }
}

// ─── Scene constructor / destructor ──────────────────────────────────────────
Scene::Scene()  = default;

Scene::~Scene()
{
    for (MeshEntry& e : m_entries)
        e.mesh.free();
}
