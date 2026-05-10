# COS344 Practical 4 — Index

> **Translucency, Textures and Light**
> Release: 16-03-2026 | Start By: 14-04-2026 | **Due: 11-05-2026 at 10:00**
> Total Marks: 80 (+10 bonus available)

---

## Document Map

This file is the root of the planning documentation. It contains the full spec decomposition and
directs you to more specific planning files for each subsystem.

| File | Purpose |
|------|---------|
| `index.md` | This file. Full spec breakdown + file structure overview |
| `rubric.md` | Mark tracking, rubric table, and per-feature mark assignment |
| `progress.md` | Decisions log, progress tracker, and notes |
| `architecture.md` | File structure, module responsibilities, data flow |
| `features/shapes.md` | Sphere and plane generation details |
| `features/textures.md` | Texture map creation strategy and application |
| `features/lighting.md` | Point light, Phong model, floor projection |
| `features/transformations.md` | Rotation, translation, reset logic |
| `features/colour.md` | Colour cycling, alpha, RGBA pipeline |
| `features/wireframe.md` | Wireframe rendering with GL_LINES |

---

## 1. General Instructions (verbatim constraints)

- Individual assignment. Plagiarism detection software will be used.
- Late deadline: 1 hour after due date, **20% penalty**. No extensions.
- Code that does not compile → **mark of 0**.
- Program that does not exit cleanly → **mark of 0**.
- No additional libraries beyond those listed.
- Must use **C++11**.
- No AI-generated code.
- No pre-built objects or textures — everything created from scratch.
- **OpenGL 3.3** required.
- All functions implemented in `.cpp` files. No inline implementations in headers (except provided ones).
- No `glm` mathematical library functions (including matrix arithmetic). Use Prac 1 matrix code.
- No built-in OpenGL shape generation functions.
- No built-in OpenGL transformation functions.
- Window title must be your **student number**.
- All helper files in same directory as `main.cpp`.
- Must compile from terminal via makefile.

### Allowed Libraries

```
stdio.h       stdlib.h      iostream      iomanip
cmath         sstream
GL/glew.h     GLFW/glfw3.h  glm/glm.hpp
shader.hpp    glad.c
```

---

## 2. Overview

Render a **semi-translucent 3D glass golf ball** resting on a **flat plane (floor)**.

- Inside the golf ball there is a **point light** that shines through the glass and onto the floor.
- The number of vertices for both the golf ball and the plane must be **adjustable at runtime**.
- The scene is a single 3D render viewed from a camera perspective.

---

## 3. Shape Requirements

→ See `features/shapes.md` for full implementation details.

- **Sphere (Golf Ball)**
  - Vertex count adjustable at runtime (increase/decrease keys — your choice).
  - Generated from first principles (lat/lon or icosphere approach).

- **Flat Plane (Floor)**
  - Vertex count adjustable at runtime (increase/decrease keys — your choice).
  - Hint from spec: build as a grid of smaller connected planes.

---

## 4. Texture Requirements

→ See `features/textures.md` for full implementation details.

All textures must be **hand-created** (no downloads). Three texture maps required:

| Map | Key Toggle | Effect OFF | Effect ON |
|-----|-----------|-----------|----------|
| Colour texture | `B` | Uniform translucent colour | Dimples shaded darker |
| Displacement texture | `N` | Smooth surface | Actual surface displacement (not bump mapping) |
| Alpha texture | `M` | Entire ball semi-transparent (alpha from +/-) | Only dimples transparent, rest fully opaque |

- The alpha value (from `+`/`-` keys) applies to both the uniform mode and the dimple-only mode.
- Colour changes to the golf ball must be reflected correctly through all texture modes.

---

## 5. Colour Requirements

→ See `features/colour.md` for full implementation details.

### Floor
- Two keys to cycle through 10 colours: Red, Green, Blue, White, Black + 5 custom.

### Golf Ball
- Two keys to cycle through 10 colours: Red, Green, Blue, White, Black + 5 custom.
- `+` / `-` keys control the alpha value of the golf ball.

### Light
- Two keys to cycle through 9 colours: Red, Green, Blue, White + 5 custom.
- Light colour + glass colour + floor colour **combine** on the floor (Phong-aware blending).

---

## 6. Light Requirements

→ See `features/lighting.md` for full implementation details.

- **Point light**, initially at the **centre of the golf ball**.
- Lighting model: Chapter 4 of prescribed textbook (Phong point light pseudocode).
- Light projection displayed **only on the floor** (not the golf ball, unless bonus).
- Light, glass colour, and floor colour must all contribute to the floor's final colour.

---

## 7. Transformation Requirements

→ See `features/transformations.md` for full implementation details.

### Scene Rotations (entire scene rotates)

| Key | Action |
|-----|--------|
| `W` | Rotate scene about X-axis |
| `S` | Rotate scene about X-axis (opposite) |
| `A` | Rotate scene about Y-axis |
| `D` | Rotate scene about Y-axis (opposite) |
| `E` | Rotate scene about Z-axis |
| `Q` | Rotate scene about Z-axis (opposite) |

### Light Translation (light moves, scene stays still)

| Key | Action |
|-----|--------|
| Up arrow | Increase light local Y |
| Down arrow | Decrease light local Y |
| Left arrow | Decrease light local X |
| Right arrow | Increase light local X |
| `>` | Increase light local Z |
| `<` | Decrease light local Z |

> Note: Light moves in its **local** coordinate system — this may differ from global after scene rotation.

### Reset

- `Space` — resets vertex counts, positions, and rotations of the entire scene to initial state.

> **Penalty**: 1 mark deducted per transformation if the scene is moved back to centre before a new transformation is applied.

---

## 8. Wireframe

→ See `features/wireframe.md` for full implementation details.

- `Enter` toggles between normal render and wireframe.
- Wireframe must **maintain colour scheme** of the render.
- Wireframe must conform to all transformations.
- Must use **`GL_LINES`** — `glPolygonMode` is **forbidden**.
- Implement a time delay between `Enter` key presses to avoid double-toggling.

---

## 9. Texture Maps (creation)

→ See `features/textures.md` for creation strategy.

- Three maps: colour, displacement, alpha.
- Must be able to explain how each was created during the demo.
- Include texture files in the submission archive.
- Name files with logical, descriptive names.

---

## 10. Marking Rubric Summary

→ See `rubric.md` for the full mark breakdown and progress tracking.

| Section | Marks |
|---------|-------|
| Shape requirements | 8 |
| Texture requirements | 12 |
| Colour requirements | 14 |
| Light requirements | 12 |
| Transformation requirements | 8 |
| Reset command | 2 |
| Wireframe | 12 |
| **Total** | **80** |
| Bonus | +10 |

---

## 11. Bonus Marks (+10 available)

| Bonus Feature | Marks |
|--------------|-------|
| BSP algorithm to sort glass faces | 5 |
| 5 material finishes via texture map | 1 each (5 total) |
| Second non-ambient light | 2 |
| Edges on glass faces with glLines | 2 |
| Floor colour changes behind coloured glass | 2 |
| Spectral lighting effect on glass from light | 5 |
| Stained glass + ray tracing for floor projection | 2 per region |
| Light affects golf ball perception | 2 |

---

## 12. Submission Checklist

- [ ] Code compiles from terminal via makefile
- [ ] Window title is student number
- [ ] All texture maps included and named logically
- [ ] All helper files in same directory as `main.cpp`
- [ ] Uploaded to **ClickUP** before deadline
- [ ] Uploaded to **FitchFork** during demo in tutor's presence
- [ ] Prepared to explain texture creation during demo
