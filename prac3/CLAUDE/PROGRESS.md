# PROGRESS.md — Session Log

This file tracks what has been done each session, the current state of the project,
known issues, and what to tackle next. Update this at the end of every session.

---

## Current Status

**Overall:** 🔄 In progress — full scene built, controls working, blades complete.

| Area | Status |
|---|---|
| Project scaffold & build | ✅ Done — compiles with `make` |
| Matrix math (from P1) | ✅ Done — prac1/Matrix.h + Vector.h via math3d.hpp |
| Vertex / Fragment shaders | ✅ Done — MVP + per-vertex colour |
| Camera / projection setup | ✅ Done — perspective + lookAt |
| Course geometry | 🔄 In progress — green slabs, walls, mat done; hole is empty gap only |
| Windmill geometry | ✅ Done — lower/upper tower, blue trim, roof, axle, hub |
| Rotor & blades | ✅ Done — 4 blades with white spine, 8 red bars, red paddle |
| WASDQE rotations | ✅ Done — cumulative, correct axes/directions |
| IJKLOU translations | ✅ Done — cumulative |
| +/- rotor speed control | ✅ Done — never goes negative |
| Wireframe (GL_LINES) | ✅ Done — Enter toggles, debounced, colour-preserving |
| Background colour | ✅ Done — sky blue |
| HUD / on-screen text | ⬜ Not started |
| Golf hole (proper cutout) | ❌ Removed — empty square gap only, needs circular cutout |
| Slope / incline | ⬜ Not started |
| Golf ball | ⬜ Not started |
| Flowers | ⬜ Not started |
| Ground plane | ⬜ Not started |
| Course support structure | ⬜ Not started (needed once slope added) |
| Windmill windows | ⬜ Not started |
| Reference photo & planning | 🔄 Photo chosen (windmill.jpg), annotation not done |
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
- [x] Boundary walls — red, distinct from green
- [ ] Golf hole — currently just empty square gap, needs circular cutout with depth
- [x] Starting mat — yellow, distinct colour
- [x] 2+ decorations outside boundary — 2 trees (cone + cylinder each)
- [x] Green grass surface

### Shape Requirements (12 marks)
- [x] Cuboid (many)
- [x] Triangular Prism (roof + blade paddles)
- [x] Cone ≥8 vertices (tree foliage × 2, segs=12)
- [x] Cylinder ≥3, ≥8 circle verts (axle segs=12, tree trunks segs=8 × 2)
- [x] 20+ distinct 3D shapes (~34 currently)

### Colours (14 marks)
- [x] Grass — green
- [x] Boundary walls — red, distinct
- [x] Blades — white spine + red bars
- [x] Starting mat — yellow
- [x] Rotor axle — metallic grey (0.67, 0.67, 0.67)
- [x] Background — sky blue, not black
- [x] 6+ distinct colours — green, red, yellow, beige, blue, grey, white, dark grey = 8

### Transformations (12+12+8 marks)
- [x] W/S rotate X ±
- [x] A/D rotate Y ±
- [x] Q/E rotate Z ± (Q=+, E=-)
- [x] I/K translate Y ±
- [x] J/L translate X ±
- [x] U/O translate Z ±
- [x] All cumulative, never reset
- [x] +/- rotor speed, never negative
- [x] Rotor spins around arbitrary Z axis (rotateAxis)

### Wireframe (12 marks)
- [x] Enter toggles
- [x] GL_LINES only (glPolygonMode never used)
- [x] Colour preserved
- [x] Follows all transforms
- [x] Debounced (0.25s)

### Reference & Planning (6 marks)
- [x] Reference photo: windmill.jpg
- [ ] Annotated photo showing shape mapping — NOT DONE (0 marks if missing at demo)
- [ ] Both photos included in submission archive

### Imports — Clean?
- [x] main.cpp — only allowed headers
- [x] src/*.cpp/hpp — only allowed headers
- [x] shader.cpp — provided file (not our responsibility)
- [x] No GLM math functions used anywhere
- [x] No glPolygonMode anywhere

---

## Bonus Marks Available

| Bonus (+2 each) | Status | Notes |
|---|---|---|
| Doors/windows on windmill | ⬜ Planned | Small flat cuboids on tower face |
| Horizontal-axis rotor | ✅ Already done | Our rotor spins around Z = horizontal axis |
| Additional course details (flowers) | ⬜ Planned | Cone on cylinder stem |
| 3D spherical golf ball | ⬜ Planned | UV-sphere from triangles |
| Course incline/decline | ⬜ Planned | Channel slab tilted, starting mat lower |
| Internal tunnel (instead of gap) | ❌ Not planned | Complex, low priority |

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
- Removed unused prac1 files (Shape, Triangle, Square).
- Built complete scene: channel + wide area slabs, 8 boundary walls, starting mat,
  lower/upper tower with ball gap, 4 blue trim strips, roof, axle, hub, 4 blades.
- Two decorative trees outside boundary walls.
- Windmill flipped to face south (+Z toward player).
- Hole geometry removed pending design decision — currently empty square gap.
- Blade redesign: white spine + 8 progressive red cross-bars + red paddle tip.
- Key direction fixes: Q/E swapped on Z axis.

**Current issues:**
- Golf hole is just an empty square gap — needs proper circular cutout.
- Annotated reference photo not yet created.

**Next session — planned work:**
1. HUD text overlay (port linefont.h from prac2, add 2D shader)
2. Circular golf hole cutout (annular green ring + grey walls + dark floor)
3. Course slope — channel slab tilted so starting mat is lower than windmill
4. Course support structure — retaining walls around elevated section
5. Ground plane — flat green base for trees/flowers to sit on
6. Flowers (cone + cylinder) placed outside boundary walls
7. Golf ball (UV-sphere)
8. Windmill windows (flat cuboids on tower face)
9. Annotated reference photo
