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

### DEC-005 — Shader uniform for colour (per-draw-call)
**Date:** Pre-session (setup)
**Decision:** Pass colour as a `uniform vec3 u_color` to the fragment shader rather than using per-vertex colour attributes.
**Rationale:** Simpler to manage for a low-polygon scene. Each shape is a single solid colour, so a per-draw-call uniform is sufficient and avoids a colour VBO per shape.
**Impact:** Fragment shader is trivial (`gl_FragColor = vec4(u_color, 1.0)`). Each shape's draw call sets the uniform before drawing.

---

<!-- Add new decisions below this line -->
