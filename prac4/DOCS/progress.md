# COS344 Prac 4 — Progress & Decisions Log

> Use this file to record implementation decisions, progress milestones, and anything that
> deviates from the plans in the architecture or feature docs.
> Update as you go.

---

## Implementation Progress

| Phase | Status | Notes |
|-------|--------|-------|
| Project setup (Makefile, window, GL context) | ⬜ | |
| Prac 1 math utilities ported/linked | ⬜ | |
| Sphere generation (UV sphere) | ⬜ | |
| Plane generation (grid) | ⬜ | |
| Basic render loop (clear + draw) | ⬜ | |
| Camera + projection matrix | ⬜ | |
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
