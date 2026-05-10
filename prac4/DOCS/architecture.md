# COS344 Prac 4 — Architecture

> Describes the actual file structure, module responsibilities, and runtime data flow for the
> current implementation.

---

## File Structure

```text
prac4/
├── DOCS/
│   ├── index.md
│   ├── spec.md
│   ├── rubric.md
│   ├── progress.md
│   ├── architecture.md
│   └── features/
│       ├── colour.md
│       ├── lighting.md
│       ├── shapes.md
│       ├── textures.md
│       ├── transformations.md
│       └── wireframe.md
├── textures/
│   ├── colour/
│   │   └── colour.bmp
│   ├── displacement/
│   │   └── displacement.bmp
│   ├── alpha/
│   │   └── alpha.bmp
│   └── generate_bitmaps.py
├── src/
│   ├── App.cpp / App.h
│   ├── HUD.cpp / HUD.hpp
│   ├── Input.cpp / Input.hpp
│   ├── Palette.hpp
│   ├── Scene.cpp / Scene.hpp
│   ├── Textures.cpp / Textures.hpp
│   └── linefont.h
├── Geometry.cpp / Geometry.h
├── Mesh.cpp / Mesh.h
├── Matrix.cpp / Matrix.h
├── Vector.cpp / Vector.h
├── math3d.hpp
├── shader.cpp / shader.hpp
├── vertex.glsl / fragment.glsl
├── HUDVertex.glsl / HUDFragment.glsl
├── main.cpp
└── makefile
```

---

## Module Responsibilities

### `main.cpp`
- Minimal entry point only.
- Calls `runApplication()` and keeps the root file simple.

### `src/App.cpp` / `src/App.h`
- Creates the GLFW window and OpenGL context.
- Sets the window title to the student number.
- Configures core OpenGL state such as depth testing and blending.
- Owns the high-level application loop:
  - poll input
  - update scene geometry if dirty
  - draw scene
  - draw HUD
  - swap buffers

### `src/Input.cpp` / `src/Input.hpp`
- Defines the persistent interactive state for the practical.
- Stores:
  - sphere subdivision
  - plane resolution
  - selected colour target
  - floor / ball / light colour indices
  - ball alpha
  - texture toggles
  - wireframe toggle
  - scene rotation angles
  - local light position
- Maps keyboard input to state changes.
- Handles reset behavior and `Enter` debounce.

### `src/Scene.cpp` / `src/Scene.hpp`
- Owns scene setup, geometry rebuilds, and rendering.
- Keeps the sphere, plane, and light marker meshes.
- Rebuilds sphere and plane meshes when subdivision values change.
- Loads and binds textures through `TextureSet`.
- Computes transforms, light world position, and lighting uniforms.
- Draws either:
  - solid scene with textures / lighting, or
  - wireframe scene using `GL_LINES`

### `src/HUD.cpp` / `src/HUD.hpp`
- Draws the 2D line-font overlay.
- Left panel shows the control table.
- Right panel shows live state:
  - solid / wireframe mode
  - selected colour target
  - alpha value
  - texture toggle states
  - current floor / ball / light colours

### `src/Textures.cpp` / `src/Textures.hpp`
- Loads the three uncompressed 24-bit BMP textures used by the sphere.
- Expected manual asset paths:
  - `textures/colour/colour.bmp`
  - `textures/displacement/displacement.bmp`
  - `textures/alpha/alpha.bmp`
- Uploads textures to OpenGL and binds them to texture units `0`, `1`, and `2`.

### `src/Palette.hpp`
- Central source of truth for the floor, ball, and light colour palettes.
- Exposes small helpers to fetch the current colour from the input state.

### `Geometry.cpp` / `Geometry.h`
- Generates CPU-side mesh data for:
  - UV sphere
  - grid plane
  - light marker mesh
- Also derives wireframe edge index buffers from triangle indices.

### `Mesh.cpp` / `Mesh.h`
- Uploads vertex / triangle / edge data to OpenGL buffers.
- Owns VAO, VBO, solid EBO, and wireframe EBO lifetime.
- Exposes solid draw and wireframe draw helpers.

### `Matrix.cpp`, `Vector.cpp`, `math3d.hpp`
- Reused / adapted math support from earlier practical work.
- Provide vectors, matrices, transforms, and helper operations without GLM.

### `shader.cpp` / `shader.hpp`
- Provided shader-compilation helper.
- Compiles and links shader programs from file paths.

### `textures/generate_bitmaps.py`
- Optional standalone helper to regenerate the three BMP texture assets.
- Not part of the build system.
- Uses only Python standard library code and writes manual BMPs directly to the texture folders.

---

## Shader Design

### `vertex.glsl`
- Receives sphere / plane vertex attributes:
  - position
  - normal
  - UV
- Applies model, view, and projection transforms.
- If displacement mapping is enabled, offsets sphere vertices along their normals using the
  displacement texture.
- Passes world position, world normal, and UV to the fragment shader.

### `fragment.glsl`
- Handles the main solid-render path.
- Supports:
  - uniform base object colour
  - colour texture modulation
  - alpha texture handling
  - floor-only point-light shading
- Uses the effective light colour `lightColour * glassColour` for the floor projection.
- Leaves the golf ball on a simpler translucent shading path.

### `HUDVertex.glsl` / `HUDFragment.glsl`
- Lightweight 2D HUD shader pair for the line-font overlay.
- Used only for screen-space text rendering.

---

## Data Flow

```text
Keyboard Input
    │
    ▼
InputState (src/Input.cpp)
    │
    ├──► Scene rebuild flags
    ├──► Colour / alpha / texture toggles
    ├──► Scene rotation angles
    └──► Local light position

InputState
    │
    ├──► src/Scene.cpp
    │      ├── rebuild meshes if dirty
    │      ├── compute transforms
    │      ├── compute light world position
    │      ├── bind textures
    │      └── draw solid or wireframe scene
    │
    └──► src/HUD.cpp
           └── draw control guide and current status
```

---

## Scene Layout

- The sphere has radius `1.0`.
- The plane sits at `y = -1.0`, so the sphere rests on it.
- The light starts at local position `(0, 0, 0)`, which is the sphere centre.
- The camera is fixed at `(0.0, 1.6, 4.6)` and looks toward `(0.0, -0.15, 0.0)`.
- Scene rotation is applied to the rendered scene, while the camera remains fixed.

---

## Render Order

### Solid mode
1. Draw plane.
2. Draw light marker.
3. Draw translucent sphere last.
4. Draw HUD.

### Wireframe mode
1. Draw plane wireframe with `GL_LINES`.
2. Draw sphere wireframe with `GL_LINES`.
3. Optionally keep the light marker visible for orientation.
4. Draw HUD.

Back-face culling is enabled for the sphere draw to reduce translucent shell artifacts.

---

## State Summary

The interactive state currently tracks:

- `sphereSubdivision`
- `planeResolution`
- `selectedColorTarget`
- `floorColorIndex`
- `ballColorIndex`
- `lightColorIndex`
- `ballAlpha`
- `colourTextureEnabled`
- `displacementTextureEnabled`
- `alphaTextureEnabled`
- `wireframeMode`
- `sceneRotXDegrees`
- `sceneRotYDegrees`
- `sceneRotZDegrees`
- `lightLocalPos`
- `sphereDirty`
- `planeDirty`

---

## Reset Behavior

Pressing `Space` restores:

- default sphere subdivision
- default plane resolution
- selected colour target
- default floor / ball / light colours
- default ball alpha
- all texture toggles off
- wireframe off
- all scene rotation angles to zero
- light local position to the sphere centre
