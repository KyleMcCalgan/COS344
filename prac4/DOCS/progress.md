# COS344 Prac 4 — Progress & Decisions Log

> Use this file to record implementation decisions, progress milestones, and anything that
> deviates from the plans in the architecture or feature docs.
> Update as you go.

---

## Implementation Progress

### Phase Plan

| Phase | Status | Scope | Manual test target |
|-------|--------|-------|--------------------|
| 0. Bootstrap and compile path | ✅ | Window creation, shader loading, basic draw loop, Prac 1 math imported/reused | Program opens, renders placeholder geometry, exits cleanly |
| 1. Scene state and input foundation | ⬜ | Centralise scene state, choose key bindings, add input routing for future features | Program still runs while state values can be changed via keys |
| 2. Mesh framework and sphere generation | ⬜ | Reusable mesh/buffer ownership and runtime-regenerating UV sphere | Sphere renders and subdivision changes are visible |
| 3. Plane generation and full scene render | ⬜ | Floor grid mesh, indexed drawing, camera/projection, draw sphere + plane together | Sphere rests on plane and both render correctly in 3D |
| 4. Transformations and reset | ⬜ | Scene rotations, local-space light translation state, reset behaviour | W/A/S/D/E/Q, arrows, `<>`, and `Space` visibly work |
| 5. Colour, alpha, and translucency | ⬜ | Floor/ball/light colour cycling, alpha control, blending order | Colours cycle and ball transparency updates correctly |
| 6. Floor lighting | ⬜ | Point light, floor-only Phong shading, effective light colour = light × glass | Light projection on floor responds to colour and movement |
| 7. Texture asset generation and loading | ⬜ | Create BMP maps from scratch and load/bind them in OpenGL | Textures load successfully and can be bound without rendering errors |
| 8. Texture feature integration | ⬜ | `B` colour map, `N` displacement map, `M` alpha map | Each toggle has the expected visible effect on the golf ball |
| 9. Wireframe mode | ⬜ | Edge buffer generation, `GL_LINES` rendering, Enter debounce | Enter toggles a coloured transformed wireframe view |
| 10. Final compliance and demo prep | ⬜ | Spec audit, defaults, makefile check, clean exit, docs and submission prep | Full manual walkthrough of all rubric items succeeds |

### Feature Checklist

| Phase | Status | Notes |
|-------|--------|-------|
| Project setup (Makefile, window, GL context) | ✅ | Basic compile/run path is present |
| Prac 1 math utilities ported/linked | ✅ | `math3d.hpp` and supporting matrix/vector code are in use |
| Sphere generation (UV sphere) | ⬜ | |
| Plane generation (grid) | ⬜ | |
| Basic render loop (clear + draw) | ✅ | Placeholder triangle loop works |
| Camera + projection matrix | ✅ | Present in current bootstrap scene |
| Scene rotation (W/A/S/D/E/Q) | ⬜ | |
| Light translation (arrows + <>) | ⬜ | |
| Reset command (Space) | ⬜ | |
| Colour cycling — floor | ⬜ | |
| Colour cycling — golf ball | ⬜ | |
| Colour cycling — light | ⬜ | |
| Alpha control (+/-) | ⬜ | |
| Blending enabled for sphere | ⬜ | |
| Phong point light on floor | ⬜ | |
| Glass colour × light colour on floor | ⬜ | |
| Texture BMP creation script | ⬜ | |
| Texture loading (BMP → OpenGL) | ⬜ | |
| Colour texture map (B toggle) | ⬜ | |
| Displacement texture map (N toggle) | ⬜ | |
| Alpha texture map (M toggle) | ⬜ | |
| Wireframe edge buffer generation | ⬜ | |
| Wireframe rendering (GL_LINES) | ⬜ | |
| Wireframe colour correct | ⬜ | |
| Wireframe transformations correct | ⬜ | |
| Enter debounce | ⬜ | |
| Subdivision runtime adjustment | ⬜ | |
| Plane resolution runtime adjustment | ⬜ | |
| Final compile + clean exit check | ⬜ | |
| Submission archive + makefile check | ⬜ | |

**Status key:** ⬜ Not started | 🔄 In progress | ✅ Done

---

## Decisions Log

Record decisions here as they are made during implementation.

### Key Bindings (confirm and fill in)

| Action | Key Chosen | Date Decided |
|--------|-----------|--------------|
| Sphere subdivision up | | |
| Sphere subdivision down | | |
| Plane resolution up | | |
| Plane resolution down | | |
| Floor colour next | | |
| Floor colour prev | | |
| Ball colour next | | |
| Ball colour prev | | |
| Light colour next | | |
| Light colour prev | | |

### Initial Key Plan

| Action | Key Chosen | Date Decided |
|--------|-----------|--------------|
| Sphere subdivision up | `I` | 2026-05-10 |
| Sphere subdivision down | `K` | 2026-05-10 |
| Plane resolution up | `O` | 2026-05-10 |
| Plane resolution down | `L` | 2026-05-10 |
| Floor colour next | `X` | 2026-05-10 |
| Floor colour prev | `Z` | 2026-05-10 |
| Ball colour next | `V` | 2026-05-10 |
| Ball colour prev | `C` | 2026-05-10 |
| Light colour next | `G` | 2026-05-10 |
| Light colour prev | `F` | 2026-05-10 |

> These bindings are chosen to avoid collisions with the fixed spec keys:
> `W/A/S/D/E/Q`, arrows, `<>`, `Space`, `Enter`, `B`, `N`, `M`, `+`, `-`.

### Geometry Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Sphere radius | | |
| Default sphere subdivision | | |
| Plane extent (±X and ±Z) | | |
| Default plane resolution | | |
| Plane Y position (= -radius?) | | |

### Texture Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Texture resolution | | |
| Dimple layout (regular / hex-offset) | | |
| Dimples per row | | |
| Max displacement amount | | |
| Smooth or hard dimple edges | | |
| BMP format (24-bit / 32-bit) | | |

### Lighting Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Lighting in vertex or fragment shader | | |
| Attenuation: constant | | |
| Attenuation: linear | | |
| Attenuation: quadratic | | |
| Include specular? | | |
| Ambient strength | | |
| One shader with flag or two shaders | | |

### Rendering Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Camera position | | |
| Projection FOV | | |
| Near clip plane | | |
| Far clip plane | | |
| Rotation step per key press (degrees) | | |
| Light translation step (units) | | |
| Alpha step per +/- press | | |
| Default alpha value | | |
| Enter debounce time (ms) | | |
| Wireframe: separate shader or flag | | |

---

## Issues & Blockers

Record any bugs, unexpected behaviour, or blockers here.

| Date | Issue | Resolution |
|------|-------|------------|
| | | |

---

## Deviations from Architecture Doc

Note any cases where the actual implementation differs from what was planned.

| Component | Planned | Actual | Reason |
|-----------|---------|--------|--------|
| | | | |
