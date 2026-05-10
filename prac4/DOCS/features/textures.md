# Feature: Textures

> Three manually created texture maps for the golf ball: colour, displacement, and alpha.

---

## Overview

All three textures are square BMP images that wrap around the sphere using its UV coordinates
(U = longitude fraction, V = latitude fraction). The golf ball dimple pattern is what all three
maps describe — each map just encodes a different property of the dimples.

A **golf ball dimple** in UV space appears as a small dark circle on the colour map,
a small indented circle on the displacement map, and a transparent circle on the alpha map.

---

## Creation Strategy: Programmatic BMP Generation

The simplest approach that keeps things explainable during the demo is to write a small standalone
C++ or Python script (outside the main project) that generates the BMP files by computing pixel
values mathematically. This means:

- No external image editor required.
- Fully explainable: "I wrote a script that places circles at regular grid positions on the UV map."
- Produces clean, consistent textures.
- All three maps share the same circle layout — only the pixel values differ.

### Dimple Layout Algorithm

1. Define a texture resolution, e.g. 512 × 512 pixels.
2. Define a grid of dimple centres in UV space — e.g. a hex-offset grid to simulate real golf ball
   dimple packing. A simple regular grid (e.g. every 1/8 of the texture width/height) is sufficient
   for marking purposes.
3. For each pixel `(px, py)`, check if it falls within the radius of any dimple centre.
   - If yes → the pixel is "inside a dimple".
   - If no → the pixel is "outside a dimple" (normal surface).
4. Write different pixel values depending on which map is being generated (see below).

### Hex-Offset Grid (recommended for realism)

Alternate rows are offset by half a cell width to give the staggered packing that real golf balls use.
This is simple to compute and looks convincing.

---

## Colour Texture Map (`golf_colour.bmp`)

**Purpose:** Makes dimples appear slightly darker than the surrounding surface.

**Pixel rules:**
- Inside a dimple → dark grey, e.g. `RGB(80, 80, 80)`.
- Outside a dimple → light grey / near-white, e.g. `RGB(220, 220, 220)`.

**Usage in shader:**
- When enabled (`B` key), the sphere's base colour is multiplied (modulated) by this texture sample.
- Dimple pixels darken the base colour; surface pixels leave it nearly unchanged.
- When disabled, the sphere renders with its uniform base colour.

---

## Displacement Texture Map (`golf_displacement.bmp`)

**Purpose:** Actually moves vertices inward at dimple locations to create real surface geometry.

**Pixel rules:**
- Inside a dimple → black `RGB(0, 0, 0)` = maximum inward displacement.
- Outside a dimple → white `RGB(255, 255, 255)` = no displacement (surface stays put).
- Optionally, a smooth gradient at the dimple edge (using distance from centre) to avoid hard edges.

**Usage in vertex shader:**
- When enabled (`N` key), the vertex shader samples this texture at the vertex's UV coordinate.
- The sampled value (0.0 to 1.0) is used to scale a displacement amount (e.g. max 0.05 units).
- The displacement is applied along the vertex normal direction: `pos += normal * (1.0 - sample) * maxDisplacement`.
- Since displacement only affects actual vertex positions, increasing sphere subdivision makes
  the dimples appear smoother and more accurate — this directly demonstrates why vertex count matters.
- **Bump mapping is explicitly forbidden** — this must be actual vertex displacement.

**Note on demo explanation:**
- Be ready to explain that displacement mapping requires sufficient vertex count to look good,
  and that at low subdivision the dimples will appear faceted or not visible at all.

---

## Alpha Texture Map (`golf_alpha.bmp`)

**Purpose:** Makes only the dimple regions transparent while the rest of the ball is fully opaque.

**Pixel rules:**
- Inside a dimple → black `RGB(0, 0, 0)` = transparent (alpha = 0 when applied).
- Outside a dimple → white `RGB(255, 255, 255)` = fully opaque (alpha = 1 when applied).

**Usage in fragment shader:**
- When disabled (`M` key off), entire ball uses the uniform alpha value from `+`/`-`.
- When enabled (`M` key on), the sampled alpha texture value gates per-fragment transparency:
  - Fragment alpha = `alphaTexSample * uniformAlpha` if inside dimple region.
  - Fragment alpha = `1.0` (fully opaque) if outside dimple region.
  - Or more precisely: `finalAlpha = mix(uniformAlpha, 1.0, alphaTexSample)`.
    - Where `alphaTexSample` = 1.0 on surface (opaque) and 0.0 in dimple (uses uniformAlpha).

---

## Texture Loading (`texture.cpp`)

- Read BMP files from the `textures/` directory tree at startup.
- Upload to GPU as `GL_TEXTURE_2D` objects.
- Use `GL_REPEAT` wrapping and `GL_LINEAR` filtering (or `GL_NEAREST` for crisp dimple edges).
- Bind to texture units 0, 1, 2 for colour, displacement, and alpha respectively.
- In the shader, declare `uniform sampler2D colourMap`, `displacementMap`, `alphaMap`.

---

## Shader Uniforms Related to Textures

```
uniform sampler2D colourMap;          // unit 0
uniform sampler2D displacementMap;    // unit 1 (sampled in vertex shader)
uniform sampler2D alphaMap;           // unit 2

uniform bool colourTexEnabled;
uniform bool displacementTexEnabled;  // read in vertex shader
uniform bool alphaTexEnabled;
```

---

## File Naming

| File | Description |
|------|-------------|
| `textures/colour/golf_colour.bmp` | Colour/dimple shading map |
| `textures/displacement/golf_displacement.bmp` | Surface displacement map |
| `textures/alpha/golf_alpha.bmp` | Per-dimple alpha/transparency map |

---

## Key Decisions to Log in `progress.md`

- [ ] Texture resolution chosen (e.g. 512×512).
- [ ] Dimple grid layout (regular vs hex-offset).
- [ ] Number of dimples across the texture.
- [ ] Max displacement amount.
- [ ] Whether to use smooth gradient edges on dimples.
- [ ] Whether to write a generation script or manually create BMP files.
