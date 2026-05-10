# COS344 Prac 4 — Rubric & Mark Tracking

> Total: **80 marks** + 10 bonus
> This file is now used as a final audit sheet against the implemented project.

---

## Mark Tracking Table

| Feature | Max Marks | Status | Notes |
|---------|-----------|--------|-------|
| Sphere — configurable vertices | 4 | ✅ Done | `I / K` rebuild the UV sphere at runtime |
| Plane — configurable vertices | 4 | ✅ Done | `O / L` rebuild the floor grid at runtime |
| Colour texture map | 4 | ✅ Done | `B` toggles darker dimple shading that respects ball colour |
| Displacement texture map | 4 | ✅ Done | `N` toggles real vertex displacement on the sphere |
| Alpha texture map | 4 | ✅ Done | `M` makes only the dimples transparent using the same alpha value |
| Floor colour cycling (10 colours) | 4 | ✅ Done | 10-colour palette is present; current control scheme is `1` then `[` / `]` |
| Golf ball colour cycling (10 colours) | 4 | ✅ Done | 10-colour palette is present; current control scheme is `2` then `[` / `]` |
| Golf ball alpha value control | 2 | ✅ Done | `+ / -` adjust visible translucency with clamping |
| Light colour cycling | 4 | ✅ Done | 9-colour palette is present; current control scheme is `3` then `[` / `]` |
| Light effect on floor colour | 4 | ✅ Done | Floor combines floor colour, light colour, and glass colour |
| Lighting type (point light, Phong) | 12 | ✅ Done | Point light with floor-only Phong shading |
| Scene rotations (all 3 axes) | 4 | ✅ Done | `W / A / S / D / E / Q` accumulate rotation around X, Y, Z |
| Light translation (all 3 axes) | 4 | ✅ Done | Arrows and `< >` move the light in scene-local space |
| Reset command | 2 | ✅ Done | `Space` restores geometry, transforms, colours, alpha, textures, and wireframe |
| Wireframe render | 4 | ✅ Done | `Enter` toggles `GL_LINES` wireframe using edge buffers |
| Wireframe colour | 4 | ✅ Done | Sphere and plane wireframes keep their current object colours |
| Wireframe transformations | 4 | ✅ Done | Wireframe follows the same scene transforms as solid mode |
| **TOTAL** | **80** | | |

**Status key:** ⬜ Not started | 🔄 In progress | ✅ Done | ❌ Skipped

---

## Bonus Mark Tracking

| Bonus Feature | Marks | Status | Notes |
|--------------|-------|--------|-------|
| BSP sort for glass faces | 5 | ⬜ Not started | Complex — do last |
| 5 material finishes via texture | 5 (1 each) | ⬜ Not started | |
| Second non-ambient light | 2 | ⬜ Not started | |
| Edges on glass faces (glLines) | 2 | ⬜ Not started | |
| Floor colour changes behind glass | 2 | ⬜ Not started | |
| Spectral lighting on glass | 5 | ⬜ Not started | |
| Stained glass + ray tracing | 2/region | ⬜ Not started | |
| Light affects golf ball | 2 | ⬜ Not started | |

---

## Final Audit Notes

Use this section as the last sanity check before demo or submission.

| Check | Status | Notes |
|------|--------|-------|
| Terminal build works | ✅ Done | Verified with `make clean main` and `make main` |
| Window title matches student number | ✅ Done | Title is `u24648826` |
| Texture files included with logical names | ✅ Done | `colour.bmp`, `displacement.bmp`, `alpha.bmp` in their respective folders |
| Texture creation can be explained in demo | ✅ Done | Optional helper: `textures/generate_bitmaps.py` |
| Manual visual check of clean exit | ⬜ Pending | Needs final local run confirmation |
| Manual full-controls walkthrough | ⬜ Pending | Useful before demo to confirm every key in one pass |
| Strict keybinding interpretation risk reviewed | 🔄 In progress | Current colour controls use `1/2/3` selection plus `[` / `]` cycling instead of three separate dedicated key-pairs |

---

## Full Rubric (verbatim from spec)

### Shape Requirements — 8 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Golf Ball (Sphere) | Not configurable | Configurable but rendered incorrectly | ✓ Full marks |
| Floor (Plane) | Not configurable | Configurable but rendered incorrectly | ✓ Full marks |

**What is needed for full marks:**
- Sphere vertex count increases/decreases at runtime with two keys.
- Plane vertex count increases/decreases at runtime with two keys.
- Both render correctly at any vertex count.

**Current project status:**
- Implemented and working with `I / K` for sphere detail and `O / L` for plane detail.

---

### Texture Requirements — 12 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Colour Texture Map | None | Partially correct / toggleable | Fully correct + toggleable |
| Displacement Texture Map | None | Partially correct / toggleable | Fully correct + toggleable |
| Alpha Texture Map | None | Partially correct / toggleable | Fully correct + toggleable |

**What is needed for full marks:**
- Each map toggleable with its designated key (B, N, M).
- Colour map: dimples visibly darker than surrounding surface.
- Displacement map: actual geometry displaced (not bump mapped — vertices must move).
- Alpha map: dimples transparent at alpha value, rest of ball fully opaque.
- All maps respect the current golf ball colour.
- Textures created manually — must be explainable during demo.

**Current project status:**
- Implemented with manual BMP assets in `textures/`.
- `textures/generate_bitmaps.py` can regenerate the current three maps, but the runtime path is manual-only.

> **Note:** 4 marks per texture map (not 2 as implied by the 0/1/2 table — the rubric table is condensed). The spec states "for each texture map that you correctly created, you will receive 4 marks."

---

### Colour Requirements — 14 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Floor colour | Single non-changeable | At least 5 correct + controllable | All 10 correct + controllable |
| Golf Ball colours | Single non-changeable | At least 5 correct + controllable | All 10 correct + controllable |
| Golf Ball alpha | Not changeable | Changeable with visible effect | — |
| Light colours | Single non-changeable | At least 5 correct + controllable | — (implied full) |
| Light effect on floor | No effect | Affects floor but ignores glass colour | Correctly combines all three |

**Colour lists:**
- Floor & Golf Ball: Red, Green, Blue, White, Black + 5 custom = 10 total
- Light: Red, Green, Blue, White + 5 custom = 9 total

**Floor final colour formula:**
```
floor_colour = f(floor_base_colour, glass_colour, light_colour, light_attenuation_at_vertex)
```
All three must contribute. Light intensity at each vertex drives how much colour is applied.

**Current project status:**
- Required colour counts are present.
- Floor and ball include red, green, blue, white, black, plus five custom colours.
- Light includes red, green, blue, white, plus five custom colours.
- Current control layout uses a selector model:
  - `1` select floor, then `[` / `]`
  - `2` select ball, then `[` / `]`
  - `3` select light, then `[` / `]`
- This is functionally complete, but it is the main remaining “strict spec wording” caveat if the marker expects separate dedicated key pairs per category.

---

### Light Requirements — 12 marks

| Criteria | 0 | 3 | 12 |
|----------|---|---|---|
| Lighting type | No lighting | Wrong type of light | Correct point light with Phong |

**What is needed for full marks:**
- Point light, initially at centre of sphere.
- Phong point light model (Chapter 4 of textbook) applied to floor vertices.
- Light projection visible on floor only (not on golf ball, unless bonus).
- Light + glass + floor colours combine on floor.

**Current project status:**
- Implemented.

---

### Transformation Requirements — 8 marks

| Criteria | 0 | 1 | 2 | 3 | 4 |
|----------|---|---|---|---|---|
| Rotations | None | Rotates around ≥1 axis | Rotates around ≥2 axes | — | All 3 axes |
| Light Translation | None | Translates but wrong direction | Correct along 1 axis | Correct along 2 axes | Correct along all 3 |

**Penalty:** 1 mark deducted per transformation if scene is reset to centre before applying another transformation.

**Current project status:**
- Implemented without resetting between successive transforms.

---

### Reset Command — 2 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Reset | None | Partially resets | Fully resets (vertices, positions, rotations) |

**Current project status:**
- Implemented. `Space` resets scene rotations, light position, geometry settings, colours, alpha, textures, and wireframe mode.

---

### Wireframe — 12 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Render | No wireframe | Partial wireframe | Full wireframe |
| Colour | No wireframe | Partially coloured | Correct colours matching scene |
| Transformations | No wireframe | Partial transformations | All transformations apply to wireframe |

> **Must use `GL_LINES`** — using `glPolygonMode` forfeits wireframe marks entirely.

**Current project status:**
- Implemented with edge buffers and `GL_LINES`.
- `Enter` debounce is in place.

---

## Key Bindings Summary (for quick reference during demo)

| Key | Action |
|-----|--------|
| `I` | Sphere vertex count up |
| `K` | Sphere vertex count down |
| `O` | Plane vertex count up |
| `L` | Plane vertex count down |
| `1` then `]` | Floor colour cycle forward |
| `1` then `[` | Floor colour cycle backward |
| `2` then `]` | Golf ball colour cycle forward |
| `2` then `[` | Golf ball colour cycle backward |
| `+` | Increase golf ball alpha |
| `-` | Decrease golf ball alpha |
| `3` then `]` | Light colour cycle forward |
| `3` then `[` | Light colour cycle backward |
| `B` | Toggle colour texture map |
| `N` | Toggle displacement texture map |
| `M` | Toggle alpha texture map |
| `W` | Rotate scene around X-axis |
| `S` | Rotate scene around X-axis (reverse) |
| `A` | Rotate scene around Y-axis |
| `D` | Rotate scene around Y-axis (reverse) |
| `E` | Rotate scene around Z-axis |
| `Q` | Rotate scene around Z-axis (reverse) |
| Up arrow | Light Y+ |
| Down arrow | Light Y- |
| Left arrow | Light X- |
| Right arrow | Light X+ |
| `>` | Light Z+ |
| `<` | Light Z- |
| `Space` | Reset entire scene |
| `Enter` | Toggle wireframe |
