# COS344 Prac 4 — Rubric & Mark Tracking

> Total: **80 marks** + 10 bonus
> Update the Status column as features are completed.

---

## Mark Tracking Table

| Feature | Max Marks | Status | Notes |
|---------|-----------|--------|-------|
| Sphere — configurable vertices | 4 | ⬜ Not started | |
| Plane — configurable vertices | 4 | ⬜ Not started | |
| Colour texture map | 4 | ⬜ Not started | |
| Displacement texture map | 4 | ⬜ Not started | |
| Alpha texture map | 4 | ⬜ Not started | |
| Floor colour cycling (10 colours) | 4 | ⬜ Not started | |
| Golf ball colour cycling (10 colours) | 4 | ⬜ Not started | |
| Golf ball alpha value control | 2 | ⬜ Not started | |
| Light colour cycling | 4 | ⬜ Not started | |
| Light effect on floor colour | 4 | ⬜ Not started | |
| Lighting type (point light, Phong) | 12 | ⬜ Not started | Worth 12 marks on its own |
| Scene rotations (all 3 axes) | 4 | ⬜ Not started | |
| Light translation (all 3 axes) | 4 | ⬜ Not started | |
| Reset command | 2 | ⬜ Not started | |
| Wireframe render | 4 | ⬜ Not started | |
| Wireframe colour | 4 | ⬜ Not started | |
| Wireframe transformations | 4 | ⬜ Not started | |
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

---

### Transformation Requirements — 8 marks

| Criteria | 0 | 1 | 2 | 3 | 4 |
|----------|---|---|---|---|---|
| Rotations | None | Rotates around ≥1 axis | Rotates around ≥2 axes | — | All 3 axes |
| Light Translation | None | Translates but wrong direction | Correct along 1 axis | Correct along 2 axes | Correct along all 3 |

**Penalty:** 1 mark deducted per transformation if scene is reset to centre before applying another transformation.

---

### Reset Command — 2 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Reset | None | Partially resets | Fully resets (vertices, positions, rotations) |

---

### Wireframe — 12 marks

| Criteria | 0 | 1 | 2 |
|----------|---|---|---|
| Render | No wireframe | Partial wireframe | Full wireframe |
| Colour | No wireframe | Partially coloured | Correct colours matching scene |
| Transformations | No wireframe | Partial transformations | All transformations apply to wireframe |

> **Must use `GL_LINES`** — using `glPolygonMode` forfeits wireframe marks entirely.

---

## Key Bindings Summary (for quick reference during demo)

| Key | Action |
|-----|--------|
| *(your choice)* | Sphere vertex count up |
| *(your choice)* | Sphere vertex count down |
| *(your choice)* | Plane vertex count up |
| *(your choice)* | Plane vertex count down |
| *(your choice)* | Floor colour cycle forward |
| *(your choice)* | Floor colour cycle backward |
| *(your choice)* | Golf ball colour cycle forward |
| *(your choice)* | Golf ball colour cycle backward |
| `+` | Increase golf ball alpha |
| `-` | Decrease golf ball alpha |
| *(your choice)* | Light colour cycle forward |
| *(your choice)* | Light colour cycle backward |
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
