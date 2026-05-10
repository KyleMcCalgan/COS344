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
| 1. Scene state and input foundation | ✅ | Centralise scene state, choose key bindings, add input routing for future features | Program still runs while state values can be changed via keys |
| 2. Mesh framework and sphere generation | ✅ | Reusable mesh/buffer ownership and runtime-regenerating UV sphere | Sphere renders and subdivision changes are visible |
| 3. Plane generation and full scene render | ✅ | Floor grid mesh, indexed drawing, camera/projection, draw sphere + plane together | Sphere rests on plane and both render correctly in 3D |
| 4. Transformations and reset | ✅ | Scene rotations, local-space light translation state, reset behaviour | W/A/S/D/E/Q, arrows, `<>`, and `Space` visibly work |
| 5. Colour, alpha, and translucency | ✅ | Floor/ball/light colour cycling, alpha control, blending order | Colours cycle and ball transparency updates correctly |
| 6. Floor lighting | ✅ | Point light, floor-only Phong shading, effective light colour = light × glass | Light projection on floor responds to colour and movement |
| 7. Texture asset generation and loading | ✅ | Create BMP maps from scratch and load/bind them in OpenGL | Textures load successfully and can be bound without rendering errors |
| 8. Texture feature integration | ✅ | `B` colour map, `N` displacement map, `M` alpha map | Each toggle has the expected visible effect on the golf ball |
| 9. Wireframe mode | ✅ | Edge buffer generation, `GL_LINES` rendering, Enter debounce | Enter toggles a coloured transformed wireframe view |
| 10. Final compliance and demo prep | ⬜ | Spec audit, defaults, makefile check, clean exit, docs and submission prep | Full manual walkthrough of all rubric items succeeds |

### Feature Checklist

| Phase | Status | Notes |
|-------|--------|-------|
| Project setup (Makefile, window, GL context) | ✅ | Basic compile/run path is present |
| Prac 1 math utilities ported/linked | ✅ | `math3d.hpp` and supporting matrix/vector code are in use |
| Sphere generation (UV sphere) | ✅ | Runtime-regenerating UV sphere mesh is in place |
| Plane generation (grid) | ✅ | Runtime-regenerating plane mesh is in place |
| Basic render loop (clear + draw) | ✅ | Full scene now draws sphere + plane |
| Camera + projection matrix | ✅ | Perspective + lookAt camera now frame the scene |
| Scene rotation (W/A/S/D/E/Q) | ✅ | Accumulated scene rotation is applied to sphere, plane, and light marker |
| Light translation (arrows + <>) | ✅ | Local-space light marker movement is visible for manual testing |
| Reset command (Space) | ✅ | Restores mesh densities, rotations, and light position |
| Colour cycling — floor | ✅ | Select with `1`, then use `[` / `]` to cycle the 10-colour palette |
| Colour cycling — golf ball | ✅ | Select with `2`, then use `[` / `]` to cycle the 10-colour palette |
| Colour cycling — light | ✅ | Select with `3`, then use `[` / `]` to cycle the 9-colour palette and recolour the light marker |
| Alpha control (+/-) | ✅ | `+` / `-` adjust ball alpha with clamping |
| Blending enabled for sphere | ✅ | Sphere draws last with standard alpha blending |
| Phong point light on floor | ✅ | Fragment-shader point lighting is applied to the floor only |
| Glass colour × light colour on floor | ✅ | Effective light colour is computed from light colour × golf ball colour |
| Texture BMP creation script | ✅ | `tools/generate_textures.cpp` creates all three BMP maps |
| Texture loading (BMP → OpenGL) | ✅ | `TextureSet` loads and binds uncompressed 24-bit BMP textures |
| Colour texture map (B toggle) | ✅ | Sphere colour is modulated by the generated dimple shading map |
| Displacement texture map (N toggle) | ✅ | Vertex shader displaces the sphere surface using the generated height map |
| Alpha texture map (M toggle) | ✅ | Dimple-only transparency is driven by the generated alpha map |
| Wireframe edge buffer generation | ✅ | Edge pairs are derived from triangle indices for both sphere and plane |
| Wireframe rendering (GL_LINES) | ✅ | Wireframe uses dedicated edge index buffers and `GL_LINES` |
| Wireframe colour correct | ✅ | Plane and sphere wireframes use their current object colours |
| Wireframe transformations correct | ✅ | Wireframe uses the same transforms as the solid scene |
| Enter debounce | ✅ | `Enter` / keypad `Enter` toggles wireframe with a 0.20s debounce |
| Subdivision runtime adjustment | ✅ | `I` / `K` rebuild the sphere mesh |
| Plane resolution runtime adjustment | ✅ | `O` / `L` rebuild the plane mesh |
| Final compile + clean exit check | ⬜ | |
| Submission archive + makefile check | ⬜ | |

**Status key:** ⬜ Not started | 🔄 In progress | ✅ Done

---

## Decisions Log

Record decisions here as they are made during implementation.

### Key Bindings (confirm and fill in)

| Action | Key Chosen | Date Decided |
|--------|-----------|--------------|
| Sphere subdivision up | `I` | 2026-05-10 |
| Sphere subdivision down | `K` | 2026-05-10 |
| Plane resolution up | `O` | 2026-05-10 |
| Plane resolution down | `L` | 2026-05-10 |
| Floor colour next | `]` after selecting `1` | 2026-05-10 |
| Floor colour prev | `[` after selecting `1` | 2026-05-10 |
| Ball colour next | `]` after selecting `2` | 2026-05-10 |
| Ball colour prev | `[` after selecting `2` | 2026-05-10 |
| Light colour next | `]` after selecting `3` | 2026-05-10 |
| Light colour prev | `[` after selecting `3` | 2026-05-10 |

### Initial Key Plan

| Action | Key Chosen | Date Decided |
|--------|-----------|--------------|
| Sphere subdivision up | `I` | 2026-05-10 |
| Sphere subdivision down | `K` | 2026-05-10 |
| Plane resolution up | `O` | 2026-05-10 |
| Plane resolution down | `L` | 2026-05-10 |
| Floor colour next | `]` after selecting `1` | 2026-05-10 |
| Floor colour prev | `[` after selecting `1` | 2026-05-10 |
| Ball colour next | `]` after selecting `2` | 2026-05-10 |
| Ball colour prev | `[` after selecting `2` | 2026-05-10 |
| Light colour next | `]` after selecting `3` | 2026-05-10 |
| Light colour prev | `[` after selecting `3` | 2026-05-10 |

> These bindings are chosen to avoid collisions with the fixed spec keys:
> `W/A/S/D/E/Q`, arrows, `<>`, `Space`, `Enter`, `B`, `N`, `M`, `+`, `-`.
> `1/2/3` select the active colour target: floor / golf ball / light.

### Geometry Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Sphere radius | `1.0` | Clean unit sphere normals and simple plane placement |
| Default sphere subdivision | `12` | Enough detail for a clear first milestone while still showing low-res changes |
| Plane extent (±X and ±Z) | `4.5` | Large enough to show the future light projection clearly |
| Default plane resolution | `8` | Moderate detail for incremental testing |
| Plane Y position (= -radius?) | `-1.0` | Makes the sphere rest on the floor by construction |

### Texture Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Texture resolution | `256 x 256` | Lightweight enough for development while still giving a readable dimple pattern |
| Dimple layout (regular / hex-offset) | Hex-offset grid | Produces a more golf-ball-like staggered dimple packing |
| Dimples per row | `14 rows`, `16 columns` | Dense enough to read clearly on the sphere without becoming noisy |
| Max displacement amount | `0.08` | Makes the dimples visible at moderate subdivision levels |
| Smooth or hard dimple edges | Smooth | Gives cleaner displacement and alpha transitions |
| BMP format (24-bit / 32-bit) | 24-bit | Simpler loader and sufficient for all three maps |

### Lighting Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Lighting in vertex or fragment shader | Fragment shader | Smoother floor lighting and easier localised light projection |
| Attenuation: constant | `1.0` | Stable near-light baseline |
| Attenuation: linear | `0.14` | Visible falloff across the plane at current scale |
| Attenuation: quadratic | `0.20` | Tightens the hotspot enough to read the projection clearly |
| Include specular? | Yes | Adds a clearer glossy light response on the floor |
| Ambient strength | `0.18` | Keeps the floor visible outside the hotspot without washing the effect out |
| One shader with flag or two shaders | One shader with `applyLighting` flag | Keeps the pipeline simple while leaving sphere and marker unlit |

### Rendering Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Camera position | `(0.0, 1.6, 4.6)` looking toward `(0.0, -0.15, 0.0)` | Gives a readable three-quarter view of sphere and floor |
| Projection FOV | `45°` | Natural perspective without strong distortion |
| Near clip plane | `0.1` | Keeps the full scene visible without clipping nearby geometry |
| Far clip plane | `100.0` | Large enough for this scene while keeping depth precision reasonable |
| Rotation step per key press (degrees) | `5.0` | Gives clear but controllable scene rotation per key press |
| Light translation step (units) | `0.12` | Moves the light marker visibly without leaving the ball too quickly |
| Alpha step per +/- press | `0.05` | Small enough for control, large enough to inspect manually |
| Default alpha value | `0.5` | Clearly translucent without making the inner marker too hard to see |
| Enter debounce time (ms) | `200` | Prevents rapid repeat toggles while keeping the mode responsive |
| Wireframe: separate shader or flag | Shared shader with lighting/textures disabled | Keeps the pipeline simple while still using `GL_LINES` correctly |

### Colour Decisions

| Decision | Value Chosen | Reason |
|----------|-------------|--------|
| Default floor colour | White | Neutral starting point for later lighting tests |
| Default golf ball colour | White | Makes alpha/translucency easy to judge |
| Default light colour | White | Clean baseline before floor-lighting implementation |
| 5 custom colours | Yellow, Cyan, Magenta, Orange, Purple | Distinct and easy to demonstrate in the practical |
| Alpha step size and clamp range | `0.05`, clamped to `[0.05, 1.0]` | Gives visible change per key press without becoming fully invisible |

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
| Plane preview shading | Uniform floor colour | Subtle UV grid overlay while solid rendering | Makes plane resolution changes readable during manual testing before wireframe is implemented |
| Light preview before lighting phase | No explicit light visualisation needed beyond movement state | Light marker already uses the active light colour | Makes light-colour cycling and translation testable before floor lighting is implemented |
