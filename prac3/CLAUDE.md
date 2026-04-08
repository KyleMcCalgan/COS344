# CLAUDE.md — COS344 Practical 3: 3D Rendering (Mini-Golf Windmill)

This file is the root guide for Claude Code when working on this project. It contains the full
specification digest, project rules, architecture notes, and pointers to the memory documents
stored in the `/CLAUDE/` folder. Read this file at the start of every session before touching
any code.

---

## 1. Project Identity

| Field | Value |
|---|---|
| Module | COS344 — Computer Graphics |
| Student | u24648826 |
| Window title | Must be set to student number: `"u24648826"` |
| Due date | 13 April 2026 at 10:00 |
| Total marks | 100 (+ 10 bonus available) |
| Language | C++11 |
| OpenGL version | 3.3 Core Profile |

---

## 2. Repository Layout

```
project/
├── CLAUDE.md               ← this file (root guide)
├── CLAUDE/                 ← session memory folder
│   ├── DECISIONS.md        ← architecture & design decisions log
│   ├── PROGRESS.md         ← session-by-session progress log
│   ├── SHAPES.md           ← catalogue of every shape built, its vertices, colours
│   ├── SHADERS.md          ← vertex & fragment shader design notes
│   └── REFERENCE.md        ← windmill reference photo notes & planning
├── main.cpp                ← entry point (provided template)
├── shader.cpp              ← shader loader (provided, do not modify)
├── shader.hpp              ← shader header (provided, do not modify)
├── glad.c                  ← OpenGL loader (provided by course)
├── makefile                ← provided build file (extend `files =` variable)
├── VertexShader.glsl       ← to be created
├── FragmentShader.glsl     ← to be created
└── [helper .cpp/.hpp]      ← any helper classes go in same directory as main.cpp
```

---

## 3. Build System

The provided makefile compiles with:
```
g++ -g Shader.cpp $(files) main.cpp glad.c -lglfw3 -pthread -lGLEW -ldl -lGL -o main
```

- Add any new `.cpp` files to the `files =` variable in the makefile (space-separated).
- Compile target: `make` or `make all`
- Run: `make run` or `./main`
- Clean: `make clean`
- The project must compile and run **from terminal without an IDE**.

---

## 4. Full Specification Digest

### 4.1 Windmill Requirements (10 marks)

| Requirement | Detail |
|---|---|
| Realistic | Must resemble the chosen real-world mini-golf windmill reference photo |
| Ball gap | There must be a gap through which the golf ball can pass |
| Attached to course | The windmill must sit on / be attached to the course geometry |
| Rotor blades | At least **4 blades** on the rotor |

> **TODO:** Choose a real-world mini-golf windmill reference photo. Document it in `CLAUDE/REFERENCE.md`. The render must look like it. You will need to show this photo at the demo.

### 4.2 Course Requirements (14 marks)

| Requirement | Detail |
|---|---|
| Boundary wall | Encloses the course; must be a **different colour** from the course surface |
| Golf hole cutout | A visible hole/cutout where the ball would go in |
| Starting mat | A designated starting square/mat in a **distinct colour** |
| Plants / decorations | At least **2** plants or decorations **outside** the boundary wall |
| Grass surface | Green grass-like surface for play |

### 4.3 Shape Requirements (12 marks)

All shapes must be built **from first principles** — no GLM shape generators, no OBJ loaders.

| Shape | Constraint |
|---|---|
| Cuboid | At least one |
| Triangular Prism | At least one |
| Cone | At least one; must have correct vertex count (≥8 around base) |
| Cylinder | At least **three**; each circle top/bottom must have **≥8 vertices**; must be used for the **axle** and the **golf hole** |
| Total distinct 3D shapes | At least **20** visible 3D shapes in the scene |

Notes:
- "Distinct" = the shape as a whole, not its internal triangles.
- 2D decorative shapes on faces (windows, doors) are allowed but **do not count** toward the 20.
- Some faces will be hidden by other geometry — that is fine.

### 4.4 Colour Requirements (14 marks)

| Element | Colour requirement |
|---|---|
| Grass | Green |
| Boundary walls | Different colour from the rest of the course |
| Blades | Distinct colour |
| Starting mat | Distinct colour |
| Rotor axle | Metallic colour (grey / silver) |
| Background | Distinctive (not black); use `glClearColor` |
| Total | At least **6 different solid distinct colours** |

Colours should be inspired by the chosen real-world windmill reference photo.

### 4.5 Transformation Requirements

#### Rotations (12 marks) — WASDQE keys

| Key | Action |
|---|---|
| `W` | Rotate entire scene about **x-axis** (positive direction) |
| `S` | Rotate entire scene about **x-axis** (negative direction) |
| `A` | Rotate entire scene about **y-axis** (positive direction) |
| `D` | Rotate entire scene about **y-axis** (negative direction) |
| `E` | Rotate entire scene about **z-axis** (positive direction) |
| `Q` | Rotate entire scene about **z-axis** (negative direction) |

#### Translations (12 marks) — IJKLOU keys

| Key | Action |
|---|---|
| `I` | Move entire scene **+y** |
| `K` | Move entire scene **-y** |
| `L` | Move entire scene **+x** |
| `J` | Move entire scene **-x** |
| `O` | Move entire scene **+z** |
| `U` | Move entire scene **-z** |

#### Rotor & Blades Rotation (8 marks) — +/- keys

| Key | Action |
|---|---|
| `+` | Increase rotor/blade spin speed |
| `-` | Decrease rotor/blade spin speed (must not go negative / spin backwards) |

- Initial speed: **0** (stationary).
- Rotation must be around an **arbitrary axis** (the axle axis of the windmill rotor).
- Speed is continuous — it accumulates with each key press.
- Rotation must look **realistic** (smooth, continuous in render loop using delta time or frame accumulation).

#### Important transformation rules
- Transformations must **not** reset between key presses; they must be **cumulative**.
- 1 mark deducted per transformation if the scene resets to centre before applying.
- Keep animation rates **low** so multiple presses are possible before the scene leaves view.

### 4.6 Wireframe (12 marks)

| Requirement | Detail |
|---|---|
| Toggle | `Enter` key toggles between solid and wireframe mode |
| Implementation | Must use **`GL_LINES`** — `glPolygonMode` is **strictly forbidden** |
| Colour | Wireframe must maintain the colour scheme (each shape retains its colour) |
| Transformations | Wireframe must follow all the same transformations as solid mode |
| Time delay | Implement a debounce/time delay on Enter so it doesn't flicker |

### 4.7 Reference & Planning (6 marks)

- Choose a real-world mini-golf windmill photo.
- Annotate / augment the photo showing how the 3D shapes map to the real windmill (like Figure 17 in the spec).
- Submit the photo(s) with the project archive.
- Show the photos at the demo.
- 0 marks if no photo is included.

---

## 5. Bonus Marks (up to +10)

2 marks each for extras such as:
- Doors, windows, or text on the windmill
- A rotor-style (horizontal axis) windmill
- Internal tunnel instead of straight gap for the ball
- Additional course details: sand pits, flowers, rivers
- A 3D spherical golf ball
- Course incline and decline

---

## 6. Hard Implementation Rules

These are non-negotiable per the spec. Violating them loses marks or zeroes the submission.

| Rule | Detail |
|---|---|
| No GLM math | Cannot use any GLM matrix/vector arithmetic functions. Must implement matrix math from first principles (reuse Practical 1 code). |
| No built-in shape generators | Every shape must be defined manually from vertices. |
| No built-in transformation functions | All transformations must be done manually via matrix math. |
| No glPolygonMode | Wireframe must use GL_LINES only. |
| No pre-built objects or textures | Everything created from scratch. |
| No extra libraries | Only the allowed list below. |
| Must compile from terminal | No IDE dependency. |
| Window title = student number | `"u24648826"` |
| Program must exit cleanly | Failure to exit = 0 marks. |

### Allowed Libraries

```
stdio.h          stdlib.h         iostream         iomanip
initialisation list               cmath            sstream
GL/glew.h        GLFW/glfw3.h     glm/glm.hpp      (data types only)
shader.hpp       glad.c
```

> `glm/glm.hpp` may be used for **data types** (vec3, vec4, mat4 as storage) but **not for math operations** (no glm::rotate, glm::translate, glm::perspective, etc.).

---

## 7. Provided Starter Files

### `main.cpp`
Sets up GLFW (OpenGL 3.3 Core, 4xAA, 1000×1000 window), initialises GLEW, creates the window. The `//Add code here` comment in `main()` is where all implementation goes.

### `shader.cpp` / `shader.hpp`
`LoadShaders(vertexPath, fragmentPath)` — reads two GLSL files from disk, compiles them, links a program, returns the `GLuint` program ID. Do not modify these files.

### `makefile`
Extend the `files =` variable with any new `.cpp` files. Do not change the compiler flags.

---

## 8. Submission Checklist

- [ ] Code compiles with `make` from terminal with no errors
- [ ] Window title is `u24648826`
- [ ] Program exits cleanly (Escape key or window close)
- [ ] All 6 key rotation directions work cumulatively
- [ ] All 6 key translation directions work cumulatively
- [ ] Rotor speed increases with `+`, decreases with `-`, never goes negative
- [ ] Enter toggles wireframe (GL_LINES, debounced, colour-preserving)
- [ ] At least 20 distinct 3D shapes visible
- [ ] Cuboid, triangular prism, cone, 3× cylinder all present
- [ ] At least 6 distinct colours
- [ ] Grass is green, walls distinct, blades distinct, mat distinct, axle metallic, background distinctive
- [ ] Windmill has 4+ blades, ball gap, is attached to course
- [ ] Course has boundary wall, hole cutout, starting mat, 2+ decorations
- [ ] Reference / planning photo(s) included in submission archive
- [ ] Makefile included with compile instructions
- [ ] Submitted to ClickUP before deadline
- [ ] Uploaded to FitchFork during demo in presence of tutor

---

## 9. CLAUDE Folder — Session Memory System

The `/CLAUDE/` folder acts as persistent memory across Claude Code sessions. At the **end of every session**, the relevant documents must be updated.

| File | Purpose |
|---|---|
| `CLAUDE/DECISIONS.md` | Log of every significant architecture or design decision with rationale |
| `CLAUDE/PROGRESS.md` | Session-by-session log: what was done, what works, what is broken, what is next |
| `CLAUDE/SHAPES.md` | Living catalogue of every 3D shape implemented: name, vertex data approach, colour, which file it lives in |
| `CLAUDE/SHADERS.md` | Shader design: what uniforms exist, what the vertex/fragment shaders do, how transformations are passed in |
| `CLAUDE/REFERENCE.md` | Windmill reference photo details, planning notes, shape-to-real-world mapping |

### Session Start Protocol
1. Read `CLAUDE.md` (this file).
2. Read `CLAUDE/PROGRESS.md` to understand where things were left off.
3. Read any other CLAUDE docs relevant to the session's task.
4. Proceed with implementation.

### Session End Protocol
1. Update `CLAUDE/PROGRESS.md` with what was done this session, current status, and what to do next.
2. Update `CLAUDE/SHAPES.md` if any shapes were added or changed.
3. Update `CLAUDE/DECISIONS.md` if any architectural decisions were made.
4. Update `CLAUDE/SHADERS.md` if shaders were modified.
5. Update `CLAUDE/REFERENCE.md` if the windmill reference was chosen or planning was done.
