# SHADERS.md — Shader Design & Uniform Reference

This file documents the vertex and fragment shader design, all uniforms, and how
transformations are passed from C++ to GLSL. Update whenever shaders change.

---

## Shader Files

| File | Role |
|---|---|
| `VertexShader.glsl` | Processes vertex positions, applies transformation matrices |
| `FragmentShader.glsl` | Outputs solid colour per fragment |

Shaders are loaded via the provided `LoadShaders()` function in `shader.cpp`.

---

## Vertex Shader Design

```glsl
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 u_MVP;        // projection * view * model (scene transform)
uniform mat4 u_rotorExtra; // additional local rotation for rotor/blades only
                           // For non-rotor shapes, pass identity matrix

void main() {
    gl_Position = u_MVP * u_rotorExtra * vec4(vertexPosition_modelspace, 1.0);
}
```

**Notes:**
- `u_MVP` is rebuilt every frame from: `projection * view * sceneTranslation * sceneRotation`.
- `u_rotorExtra` is identity for all non-rotor geometry. For rotor/blade geometry it is the
  accumulated spin rotation around the axle axis.
- All matrix math (building projection, view, rotation, translation matrices) is done in C++
  from first principles using Practical 1 code. The result is passed as a flat float[16] array.

---

## Fragment Shader Design

```glsl
#version 330 core

uniform vec3 u_color;

out vec3 color;

void main() {
    color = u_color;
}
```

**Notes:**
- One colour per draw call. Set `u_color` uniform before each shape's draw call.
- No lighting, no textures — flat shading only (spec requires low-polygon style).

---

## Uniform Reference Table

| Uniform | Type | Set by | Purpose |
|---|---|---|---|
| `u_MVP` | mat4 | Every frame, per draw call | Full scene transform (proj * view * model) |
| `u_rotorExtra` | mat4 | Every frame, rotor draws only | Additional rotor spin rotation |
| `u_color` | vec3 | Every draw call | Solid colour for this shape |

---

## C++ Uniform Locations (cache these at startup)

```cpp
GLuint mvpLoc      = glGetUniformLocation(programID, "u_MVP");
GLuint rotorLoc    = glGetUniformLocation(programID, "u_rotorExtra");
GLuint colorLoc    = glGetUniformLocation(programID, "u_color");
```

---

## Matrix Pipeline (from first principles)

All matrices are 4×4 stored as `float[16]` in column-major order (OpenGL convention).

```
Projection matrix  — perspective, built manually (fov, aspect, near, far)
View matrix        — fixed camera position, built manually (lookAt equivalent)
Scene model matrix — accumulated from:
                       sceneTranslation (I/K/L/J/O/U keys)
                     * sceneRotationX   (W/S keys)
                     * sceneRotationY   (A/D keys)
                     * sceneRotationZ   (E/Q keys)
Rotor matrix       — accumulated spin angle around windmill axle direction

MVP (non-rotor)  = Projection * View * SceneModel
MVP (rotor)      = Projection * View * SceneModel * RotorLocal
```

> All of the above matrix construction functions come from Practical 1.

---

## Wireframe Mode

- A `bool wireframe` flag is toggled by the Enter key (with debounce).
- In wireframe mode, each shape is drawn with its `lineIndices` EBO using `GL_LINES`.
- In solid mode, each shape is drawn with its `triIndices` EBO using `GL_TRIANGLES`.
- `u_color` is set the same way in both modes so colours are preserved.
- The same shader program is used in both modes.

---

## Actual Shader Implementation (Session 1)

**VertexShader.glsl:**
- `layout(location=0) in vec3 aPos` — vertex position
- `layout(location=1) in vec3 aColor` — per-vertex colour
- `uniform mat4 MVP` — full transform (proj * view * model)
- Passes colour through as `out vec3 fragColor`

**FragmentShader.glsl:**
- `in vec3 fragColor` from vertex shader
- `out vec4 outColor = vec4(fragColor, 1.0)`

**VBO layout:** interleaved `[x,y,z,r,g,b]` per vertex, stride = 6 floats.

**Uniform:** `MVP` (mat4) — rebuilt every frame, passed column-major via `mat4ToArray()`.

## Shader Status

| Item | Status |
|---|---|
| VertexShader.glsl created | ✅ Done |
| FragmentShader.glsl created | ✅ Done |
| MVP uniform wired up in C++ | ✅ Done |
| Projection matrix implemented | ✅ Done (perspective in math3d.hpp) |
| View matrix implemented | ✅ Done (lookAt in math3d.hpp) |
| Scene model matrix (T*Rx*Ry*Rz) | ✅ Done |
| Rotor local matrix implemented | ⬜ Not started |
| Wireframe GL_LINES support | ⬜ Not started |
