# COS344 Prac 4 — Architecture

> Describes the file structure, module responsibilities, and how data flows through the program.
> No code here — descriptions only.

---

## File Structure

```
prac4/
├── DOCS/
│   ├── index.md
│   ├── rubric.md
│   ├── progress.md
│   ├── architecture.md
│   └── features/
│       ├── shapes.md
│       ├── textures.md
│       ├── lighting.md
│       ├── transformations.md
│       ├── colour.md
│       └── wireframe.md
├── shaders/
│   ├── vertex.glsl
│   ├── fragment.glsl
│   ├── vertex_wireframe.glsl   (may reuse main vertex shader)
│   └── fragment_wireframe.glsl (may reuse main fragment shader)
├── textures/
│   ├── colour/
│   │   └── colour.bmp              (colour/dimple shading map)
│   ├── displacement/
│   │   └── displacement.bmp        (displacement map)
│   └── alpha/
│       └── alpha.bmp               (alpha/transparency map)
├── src/
│   ├── main.cpp
│   ├── sphere.h / sphere.cpp
│   ├── plane.h / plane.cpp
│   ├── texture.h / texture.cpp
│   ├── light.h / light.cpp
│   ├── math_utils.h / math_utils.cpp   (matrix/vec math from Prac 1)
│   └── shader.hpp                      (provided)
├── glad.c                              (provided)
└── Makefile
```

> All source files sit at the same level or within `src/`. If the marker requires everything flat in
> root alongside `main.cpp`, the `src/` folder can be collapsed — decide before final submission.

---

## Module Responsibilities

### `main.cpp`
- Entry point and OpenGL context setup (GLFW window, GLEW init).
- Window title set to student number.
- Owns the **render loop**: clear → draw plane → draw sphere → swap buffers.
- Owns the **keyboard callback** which routes key presses to the correct module functions.
- Holds top-level scene state: rotation angles, wireframe toggle, which colour indices are active.
- Calls into sphere, plane, light, and texture modules — does not implement their logic.

### `sphere.h / sphere.cpp`
- Generates sphere vertex/index data based on a configurable subdivision parameter (stacks × slices).
- Exposes a function to regenerate the mesh when the subdivision level changes.
- Stores its own VAO and VBO.
- Exposes a draw function that accepts the current shader program, transformation matrix, and state flags (wireframe mode, which textures are active, current colour, current alpha).
- Also generates and stores the wireframe index buffer (edge pairs derived from the triangle mesh).

### `plane.h / plane.cpp`
- Generates a flat grid mesh based on a configurable resolution (N × N quads).
- Same VAO/VBO/regenerate/draw pattern as the sphere.
- Exposes current floor colour to be set externally.
- Also generates wireframe edge indices.

### `texture.h / texture.cpp`
- Loads BMP texture files from disk into OpenGL texture objects.
- Exposes three texture IDs: colour map, displacement map, alpha map.
- Exposes bind/unbind helpers used before draw calls.
- Does NOT handle the toggle logic — that lives in `main.cpp` state flags passed to the shader.

### `light.h / light.cpp`
- Holds the light's position in local space (starts at sphere centre).
- Holds the light's current colour.
- Exposes functions to translate the light along local X, Y, Z.
- Exposes a function that returns the light position transformed into world space (accounting for scene rotation) — needed for the fragment shader uniform.

### `math_utils.h / math_utils.cpp`
- Ported/reused from Prac 1.
- Provides: 4×4 matrix struct, matrix multiply, rotation matrices (X/Y/Z), translation matrix, identity matrix, matrix-vector multiply.
- Provides: vec3/vec4 structs, dot product, cross product, normalise.
- No GLM math functions used anywhere in the project.

### `shader.hpp` (provided)
- Compiles and links vertex + fragment shaders from file paths.
- Returns a shader program ID.

---

## Shader Design

### Main Vertex Shader (`vertex.glsl`)
Responsibilities:
- Receives position, normal, and UV coordinates per vertex.
- Applies the combined model-view-projection matrix (passed as a uniform).
- If displacement is enabled (uniform bool), samples the displacement texture at the UV
  coordinate and offsets the vertex position along the normal by the displacement amount.
- Passes UV, world-space position, and world-space normal to the fragment shader as varyings.

### Main Fragment Shader (`fragment.glsl`)
Responsibilities:
- Receives UV, world-space position, world-space normal.
- Receives uniforms: light position (world space), light colour, object base colour, alpha value.
- Receives uniform booleans: colourTexEnabled, alphaTexEnabled (displacement is handled in vertex).
- Samples colour texture if enabled — modulates base colour with texture colour.
- Computes Phong point light contribution (for floor fragments) using the light position and colour.
- Computes final alpha: if alphaTexEnabled, uses alpha texture to decide per-fragment transparency;
  otherwise uses the uniform alpha value.
- Outputs `vec4(finalColour, finalAlpha)`.

### Wireframe Shaders (`vertex_wireframe.glsl`, `fragment_wireframe.glsl`)
- Simpler — just apply the same MVP transformation.
- Fragment shader outputs the object's current colour (no lighting, no texture sampling).
- Can potentially be the same shaders with a uniform flag if keeping file count low.

---

## Data Flow

```
Keyboard Input
    │
    ▼
main.cpp (scene state)
    │
    ├──► sphere.cpp ──► VAO/VBO ──► vertex shader (displacement) ──► fragment shader (colour/alpha/light)
    │
    ├──► plane.cpp  ──► VAO/VBO ──► vertex shader ──► fragment shader (light projection + combined colour)
    │
    ├──► light.cpp  ──► light position uniform ──► fragment shader
    │
    └──► texture.cpp ──► texture units 0/1/2 ──► fragment shader samplers
```

---

## Scene Coordinate System

- **World origin**: centre of the scene, initially where the sphere sits.
- **Sphere**: centred at world origin, resting on the plane (so sphere bottom touches plane top).
- **Plane**: flat, extends in XZ, Y = -(sphere radius).
- **Light**: starts at world origin (sphere centre). Translated in local coordinates.
- **Scene rotation**: entire scene (sphere + plane + light) rotated by accumulating rotation matrices
  applied to the model matrix before uploading to the shader.
- **Camera**: fixed, looking at the origin from some default position (e.g. slightly elevated and back).

---

## Rendering Order (for translucency correctness)

1. Enable depth testing.
2. Draw **floor (plane)** first — opaque, writes to depth buffer.
3. Draw **golf ball (sphere)** second — translucent, enable blending, disable depth write (or sort faces).
4. Draw **wireframe** on top if wireframe mode is active — use same geometry but GL_LINES index buffer.

> If the BSP bonus is attempted, face sorting replaces the naive back-to-front assumption.

---

## State Flags (held in main.cpp)

```
bool wireframeMode          — toggled by Enter
bool colourTexEnabled       — toggled by B
bool displacementTexEnabled — toggled by N
bool alphaTexEnabled        — toggled by M

int  sphereSubdivision      — increased/decreased by chosen keys
int  planeResolution        — increased/decreased by chosen keys

int  floorColourIndex       — cycles through 10 colours
int  ballColourIndex        — cycles through 10 colours
int  lightColourIndex       — cycles through 9 colours

float ballAlpha             — modified by + and -

float sceneRotX, sceneRotY, sceneRotZ   — accumulate rotation angles
vec3  lightLocalPos                     — light position in local space
```

---

## Reset Behaviour (Space key)

Resets all state flags to their initial values:
- `sphereSubdivision` → default (e.g. 16)
- `planeResolution` → default (e.g. 8)
- `floorColourIndex` → 0 (White or chosen default)
- `ballColourIndex` → 0
- `lightColourIndex` → 0 (White)
- `ballAlpha` → 0.5 (or chosen default)
- `sceneRotX/Y/Z` → 0.0
- `lightLocalPos` → (0, 0, 0) (sphere centre)
- All texture toggles → false (or chosen defaults)
- `wireframeMode` → false
