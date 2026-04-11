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

    // ── Course: channel slab (green) ─────────────────────────────────────────
    // Width=1.4(hx=0.7), Depth=3.5(hz=1.75), Height=0.1(hy=0.05); centre Z=+1.75
    t.clear(); l.clear();
    buildCuboid(0.0f, -0.05f, 1.75f,  0.7f, 0.05f, 1.75f,
                0.18f, 0.65f, 0.18f, t, l);
    addMesh(m_entries, t, l);

    // ── Course: wide area slab (green) — 4 strips around square hole gap ────
    // buildGolfHole fills the circle-to-square transition with triangles, so
    // the strips just need to leave a square void of halfSq=0.22 around the hole.
    // Hole at X=0, Z=-2.2. Slab spans X=-2.0..+2.0, Z=0..-3.5.
    {
        float gR = 0.18f, gG = 0.65f, gB = 0.18f;
        float slabY = -0.05f, slabHY = 0.05f;
        float holeX = 0.0f, holeZ = -2.2f, halfSq = 0.22f;

        // Left strip:  X -2.0 .. -(halfSq), full Z
        {
            float hx = (2.0f - halfSq) * 0.5f;
            float cx = -halfSq - hx;
            t.clear(); l.clear();
            buildCuboid(cx, slabY, -1.75f,  hx, slabHY, 1.75f, gR, gG, gB, t, l);
            addMesh(m_entries, t, l);
        }

        // Right strip: X +(halfSq) .. +2.0, full Z
        {
            float hx = (2.0f - halfSq) * 0.5f;
            float cx = halfSq + hx;
            t.clear(); l.clear();
            buildCuboid(cx, slabY, -1.75f,  hx, slabHY, 1.75f, gR, gG, gB, t, l);
            addMesh(m_entries, t, l);
        }

        // South strip: X -(halfSq)..+(halfSq), Z 0 .. (holeZ+halfSq)
        {
            float z0 = 0.0f, z1 = holeZ + halfSq;
            t.clear(); l.clear();
            buildCuboid(holeX, slabY, (z0+z1)*0.5f,
                        halfSq, slabHY, (z0-z1)*0.5f, gR, gG, gB, t, l);
            addMesh(m_entries, t, l);
        }

        // North strip: X -(halfSq)..+(halfSq), Z (holeZ-halfSq) .. -3.5
        {
            float z0 = holeZ - halfSq, z1 = -3.5f;
            t.clear(); l.clear();
            buildCuboid(holeX, slabY, (z0+z1)*0.5f,
                        halfSq, slabHY, (z0-z1)*0.5f, gR, gG, gB, t, l);
            addMesh(m_entries, t, l);
        }
    }

    // ── Boundary walls (red cuboids, 0.2 thick, 0.4 tall) ────────────────────
    auto wall = [&](float cx, float cy, float cz,
                    float hx, float hy, float hz)
    {
        t.clear(); l.clear();
        buildCuboid(cx, cy, cz, hx, hy, hz, 0.78f, 0.15f, 0.15f, t, l);
        addMesh(m_entries, t, l);
    };

    // South cap wall
    wall(0.0f,   0.2f,  3.625f,  0.9f,  0.2f, 0.1f);
    // West channel wall
    wall(-0.8f,  0.2f,  1.75f,   0.1f,  0.2f, 1.75f);
    // East channel wall
    wall( 0.8f,  0.2f,  1.75f,   0.1f,  0.2f, 1.75f);
    // West junction step
    wall(-1.5f,  0.2f,  0.05f,   0.6f,  0.2f, 0.1f);
    // East junction step
    wall( 1.5f,  0.2f,  0.05f,   0.6f,  0.2f, 0.1f);
    // West wide wall
    wall(-2.1f,  0.2f, -1.75f,   0.1f,  0.2f, 1.75f);
    // East wide wall
    wall( 2.1f,  0.2f, -1.75f,   0.1f,  0.2f, 1.75f);
    // North cap wall
    wall( 0.0f,  0.2f, -3.6f,    2.2f,  0.2f, 0.1f);

    // ── Starting mat (yellow) ─────────────────────────────────────────────────
    t.clear(); l.clear();
    buildCuboid(0.0f, 0.02f, 3.375f,  0.45f, 0.02f, 0.25f,
                0.93f, 0.83f, 0.10f, t, l);
    addMesh(m_entries, t, l);

    // ── Golf hole — TODO: add hole geometry here once design is decided

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

    // ── Decorations: two trees outside boundary walls ─────────────────────────
    t.clear(); l.clear();
    buildTree(-3.2f, -1.0f, 0.0f, 0.6f, 0.08f, 1.0f, 0.4f, t, l);
    addMesh(m_entries, t, l);

    t.clear(); l.clear();
    buildTree( 3.2f, -1.0f, 0.0f, 0.6f, 0.08f, 1.0f, 0.4f, t, l);
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
