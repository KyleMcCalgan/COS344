# Feature: Wireframe

> Toggle between solid render and wireframe using GL_LINES. glPolygonMode is forbidden.

---

## Toggle Behaviour

- `Enter` key toggles `wireframeMode` boolean.
- When `wireframeMode = false`: normal solid render.
- When `wireframeMode = true`: wireframe render using edge index buffers.
- A **time delay** (debounce) is required so that holding Enter or rapid presses don't flicker.
  - Suggested approach: record the timestamp of the last toggle, ignore subsequent presses within
    ~200ms (use `glfwGetTime()` to measure elapsed time).

---

## Implementation: GL_LINES Edge Buffers

The wireframe is drawn using a **separate EBO (Element Buffer Object)** containing edge pairs.

### Edge Buffer Generation

When the sphere or plane mesh is generated, simultaneously generate its edge index buffer:

1. Iterate over every triangle in the triangle index list.
2. For each triangle `(A, B, C)`, extract three edges: `(A,B)`, `(B,C)`, `(A,C)`.
3. Normalize each edge so the smaller index comes first (for deduplication).
4. Insert into a set (or sort + unique) to remove duplicates.
5. Flatten into an array of index pairs: `[A0, B0, A1, B1, ...]`.
6. Upload to a separate EBO.

### Drawing Wireframe

In the render loop, when `wireframeMode = true`:
- Bind the object's VAO.
- Bind the **wireframe EBO** instead of the triangle EBO.
- Call `glDrawElements(GL_LINES, edgeCount * 2, GL_UNSIGNED_INT, 0)`.

This reuses the same VAO/VBO (same vertex positions) but draws edges instead of triangles.

---

## Colour in Wireframe Mode

The wireframe must **maintain the colour scheme** of the render:
- Sphere wireframe → drawn in the golf ball's current colour.
- Plane wireframe → drawn in the floor's current colour.

The wireframe fragment shader should output the object's base colour directly (no texture sampling,
no lighting computation). Pass the current object colour as a uniform.

This means either:
- Use the same shared shader with a `uniform bool wireframeMode` flag that bypasses texture/lighting.
- Or use a separate, simpler shader just for wireframe passes.

The simpler shader approach is cleaner and avoids branching in the main shader.

---

## Transformations in Wireframe Mode

The wireframe must respond to all the same transformations as the solid render:
- Scene rotations (W/A/S/D/E/Q) apply — use the same `MVP` matrix uniform.
- Light translation changes the light position but does not change the wireframe appearance directly.
- Since the MVP matrix is the same for both solid and wireframe, this is automatically correct
  as long as the wireframe draw call uses the same matrix.

---

## Render Order with Wireframe

When `wireframeMode = true`:
1. Clear the screen.
2. Draw **floor wireframe** with `GL_LINES` and floor colour.
3. Draw **sphere wireframe** with `GL_LINES` and ball colour.
4. (Optionally) draw the light position as a small point or marker — not required by spec.

No blending is needed for the wireframe — all edges are fully opaque.

---

## Key Decisions to Log in `progress.md`

- [ ] Debounce delay chosen (e.g. 200ms).
- [ ] Whether to use a separate wireframe shader or a flag in the main shader.
- [ ] Whether the light position is visualized in wireframe mode.
