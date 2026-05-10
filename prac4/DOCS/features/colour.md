# Feature: Colour

> Floor colour, golf ball colour and alpha, light colour, and how they combine on the floor.

---

## Colour Lists

### Floor & Golf Ball (10 colours each)

| Index | Colour | RGB |
|-------|--------|-----|
| 0 | Red | (1.0, 0.0, 0.0) |
| 1 | Green | (0.0, 1.0, 0.0) |
| 2 | Blue | (0.0, 0.0, 1.0) |
| 3 | White | (1.0, 1.0, 1.0) |
| 4 | Black | (0.0, 0.0, 0.0) |
| 5 | Yellow | (1.0, 1.0, 0.0) |
| 6 | Cyan | (0.0, 1.0, 1.0) |
| 7 | Magenta | (1.0, 0.0, 1.0) |
| 8 | Orange | (1.0, 0.5, 0.0) |
| 9 | Purple | (0.5, 0.0, 0.5) |

### Light (9 colours)

| Index | Colour | RGB |
|-------|--------|-----|
| 0 | White | (1.0, 1.0, 1.0) |
| 1 | Red | (1.0, 0.0, 0.0) |
| 2 | Green | (0.0, 1.0, 0.0) |
| 3 | Blue | (0.0, 0.0, 1.0) |
| 4 | Yellow | (1.0, 1.0, 0.0) |
| 5 | Cyan | (0.0, 1.0, 1.0) |
| 6 | Magenta | (1.0, 0.0, 1.0) |
| 7 | Orange | (1.0, 0.5, 0.0) |
| 8 | Purple | (0.5, 0.0, 0.5) |

> Colours 5–9 are the "5 other distinct colours of your choosing" for each object.

---

## Key Bindings

| Keys | Target | Direction |
|------|--------|-----------|
| `Z` / `X` | Floor colour | Prev / Next |
| `C` / `V` | Golf ball colour | Prev / Next |
| `F` / `G` | Light colour | Prev / Next |
| `+` / `-` | Golf ball alpha | Increase / Decrease |

> Key choices above are suggestions. Confirm and log in `progress.md`. Avoid collisions with
> transformation keys (W/A/S/D/E/Q) and texture toggles (B/N/M).

---

## Golf Ball Alpha

- Controlled by `+` and `-` keys.
- Clamped between 0.05 (nearly invisible) and 1.0 (fully opaque).
- Step size: 0.05 per key press.
- Stored as `float ballAlpha` in main scene state.
- Passed to the fragment shader as `uniform float alpha`.

### RGBA Pipeline

- The golf ball colour buffer must use RGBA (`vec4`), not RGB (`vec3`).
- OpenGL blending must be enabled: `glEnable(GL_BLEND)` with `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`.
- The floor is drawn first (opaque, no blending), then the sphere (blended on top).

---

## Floor Colour: Combined Effect

The floor's final colour is a combination of three sources:

1. **Floor base colour** — the floor's own selected colour.
2. **Light colour** — the colour of the point light.
3. **Glass colour** — the current colour of the golf ball (acts as a colour filter for the light).

### Effective Light Colour

Before passing to the shader, compute on the CPU:
```
effectiveLightColour = lightColour * glassColour   (component-wise multiply)
```

Pass this as a single `vec3` uniform. The fragment shader then applies:
```
floorFinalColour = floorBaseColour * (ambient + diffuseAttenuation * effectiveLightColour)
```

This means:
- Where the light is bright and close → floor colour is strongly tinted by both light and glass colour.
- Where the light is dim / far → floor colour approaches the ambient-lit floor base colour.
- A black glass ball blocks all light (effectiveLightColour → 0).
- A white glass ball passes light unchanged (effectiveLightColour = lightColour).

---

## Colour Cycling Implementation

- Each object (floor, ball, light) has an integer index into its colour array.
- Pressing the "next" key: `index = (index + 1) % arraySize`.
- Pressing the "prev" key: `index = (index - 1 + arraySize) % arraySize`.
- The active colour is looked up from the array each frame and passed as a shader uniform.

---

## Key Decisions to Log in `progress.md`

- [ ] Key bindings for each colour cycle.
- [ ] Default starting colour for each object.
- [ ] Default alpha value.
- [ ] Final choice of 5 custom colours for each object.
- [ ] Alpha step size and clamp range.
