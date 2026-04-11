# DECISIONS.md — Architecture & Design Decisions

This file logs every significant design or architecture decision made during the project,
with the rationale behind it. Update this at the end of any session where a decision was made.

---

## Decision Log

### DEC-001 — Matrix math from Practical 1
**Date:** Pre-session (setup)
**Decision:** Reuse and adapt the matrix arithmetic code from Practical 1 rather than writing from scratch.
**Rationale:** The spec explicitly forbids GLM math functions but allows first-principles matrix math. Practical 1 already has this implemented and tested.
**Impact:** Need to copy/adapt the relevant matrix files into this project's directory alongside main.cpp. Add them to the `files =` variable in the makefile.

---

### DEC-002 — Transformation architecture: single model matrix accumulation
**Date:** Pre-session (setup)
**Decision:** Maintain a single global model matrix for the entire scene (windmill + course), and a separate local rotation matrix for the rotor/blades. Apply these as uniforms in the vertex shader.
**Rationale:** WASD/QE/IJKL transformations all act on the whole scene. The rotor rotation is a child transform relative to the scene transform. Keeping two matrices (scene + rotor) is the simplest correct approach.
**Impact:** Vertex shader needs two uniform mat4s: `u_model` (scene) and `u_rotorModel` (rotor/blades only). Non-rotor geometry uses `u_model`. Rotor/blade geometry uses `u_model * u_rotorModel`.

---

### DEC-003 — Wireframe via GL_LINES, separate VAO/VBO per shape
**Date:** Pre-session (setup)
**Decision:** Each shape will store both a triangle index buffer (for solid rendering) and a line index buffer (for wireframe). Toggle is a bool; the render loop switches which index buffer to draw with.
**Rationale:** `glPolygonMode` is forbidden. The only compliant approach is explicit GL_LINES with manually defined edge indices. Storing both per shape avoids rebuilding geometry at toggle time.
**Impact:** Every shape-building function must generate both triangle indices and line (edge) indices.

---

### DEC-004 — Windmill reference photo
**Date:** TBD
**Decision:** PENDING — no reference photo chosen yet.
**Rationale:** N/A
**Impact:** Must be resolved before significant windmill geometry is built. See REFERENCE.md.

---

### DEC-005 — Per-vertex colour in VBO (revised Session 1)
**Date:** Session 1
**Decision:** Use per-vertex interleaved colour attributes (layout location 1, vec3) rather than a uniform `u_color`.
**Rationale:** Per-vertex colour allows different faces of the same shape to have different colours (e.g., each face of the test cuboid). It also means no extra uniform set per draw call. Works cleanly with both GL_TRIANGLES and GL_LINES (wireframe inherits colour automatically).
**Impact:** VBO layout is `[x,y,z,r,g,b]` per vertex (stride = 6 floats). VertexShader passes colour through as `out vec3 fragColor`. FragmentShader outputs `vec4(fragColor, 1.0)`.

---

### DEC-006 — Drop glad.c, use GLEW only
**Date:** Session 1
**Decision:** The provided makefile references `glad.c` but `glad/glad.h` is not installed on this system. Prac2 compiled successfully with GLEW only (`-lGLEW`). Removed `glad.c` from the build command.
**Rationale:** GLEW is already linked and covers all OpenGL extension loading needed. `glad.c` is redundant.
**Impact:** Makefile updated: `glad.c` removed from compile command. `glad.c` file kept in directory but not compiled.

### DEC-007 — math3d.hpp: helper layer on top of prac1 Matrix/Vector
**Date:** Session 1
**Decision:** Created `math3d.hpp` which wraps `Matrix<4,4>` as `Mat4` and provides: `identity4`, `translate`, `rotateX/Y/Z`, `rotateAxis`, `perspective`, `lookAt`, `mat4ToArray` (column-major for glUniformMatrix4fv).
**Rationale:** Keeps all 4×4 math in one place, satisfies the "no GLM math" rule, and builds on the prac1 template library without modifying it.
**Impact:** `main.cpp` includes `math3d.hpp`. No extra `.cpp` files needed (Matrix/Vector are header-only templates).

### DEC-008 — src/ folder structure
**Date:** Session 2
**Decision:** All logic moved into `src/`: Input.hpp/cpp, Mesh.hpp/cpp, Scene.hpp/cpp, math3d.hpp, shapes.hpp. main.cpp is entry point only (~65 lines).
**Rationale:** Keeps main.cpp readable and logic separated by concern.
**Impact:** Makefile `files =` lists src/Input.cpp, src/Mesh.cpp, src/Scene.cpp.

### DEC-009 — Windmill faces south (+Z)
**Date:** Session 2
**Decision:** Rotor axle protrudes from Z=+0.45 to Z=+0.80 (front face of upper tower toward the player). Hub at Z=+0.80. Spin axis = +Z unit vector.
**Rationale:** Channel runs north-south (+Z = toward player/south). Windmill should face the approaching player, not the hole.

### DEC-010 — Blade design matches reference photo
**Date:** Session 2
**Decision:** Each blade = white spine cuboid + 8 red cross-bars (progressively wider toward tip) + red triangular paddle at tip. Bars drawn after spine and offset +0.015 in Z to avoid z-fighting.
**Rationale:** windmill.jpg shows this exact ladder-with-growing-rungs pattern.

### DEC-011 — HUD text via ported linefont.h (separate 2D shader)
**Date:** Session 2 (planned)
**Decision:** Port `linefont.h` from prac2 into `src/`. Add a dedicated 2D GLSL shader pair (HUDVertex.glsl / HUDFragment.glsl) using mat3 model + projection uniforms, matching prac2's approach exactly. The HUD shader is loaded separately and used only for text drawing after the 3D scene is drawn.
**Rationale:** linefont.h is already written and tested. Its shader interface (mat3 uniforms, vec2 positions, vec3 colour uniform) is incompatible with the 3D MVP shader so a separate shader is required.
**Impact:** New files: src/HUD.hpp/cpp, HUDVertex.glsl, HUDFragment.glsl. Makefile gets src/HUD.cpp added to `files =`.

### DEC-012 — Course slope: channel slab tilted, starting mat lower
**Date:** Session 2 (planned)
**Decision:** The channel slab will be replaced with a tilted geometry so the south end (starting mat) is lower in Y than the windmill base. The wide putting area remains flat at Y=0. A retaining wall/support structure will be added around the elevated junction edge.
**Rationale:** Spec bonus mark for incline/decline. Matches real mini-golf where you putt uphill through the windmill.

### DEC-013 — Ground plane beneath trees and decorations
**Date:** Session 2 (planned)
**Decision:** A large flat green cuboid (ground plane) will be placed below the course, extending beyond the boundary walls, at a Y level that the trees and flowers sit on. The course itself will appear elevated above it once the slope is added.
**Rationale:** Trees currently sit at Y=0 with no ground reference. A ground plane gives context and avoids the floating appearance.

<!-- Add new decisions below this line -->
