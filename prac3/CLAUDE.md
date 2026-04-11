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
│   ├── REFERENCE.md        ← windmill reference photo notes & planning
│   └── LAYOUT.md           ← definitive scene layout: top-down + front view, all dimensions
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
| Rotor blades | At least **4 blades** on the rotor; each blade has a triangular prism tip |

> Reference photo chosen: `windmill.jpg`. See `CLAUDE/REFERENCE.md` for full shape mapping and `CLAUDE/LAYOUT.md` for all dimensions.

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

The `/CLAUDE/` folder is the persistent memory for this project across all Claude Code sessions.
It must be kept up to date. Stale memory is worse than no memory — if a file says something is
not started and it is actually done, the next session will waste time redoing it.

### File Index

| File | Purpose | Update trigger |
|---|---|---|
| `CLAUDE/DECISIONS.md` | Every significant architecture or design decision with rationale | Any time a non-trivial implementation choice is made |
| `CLAUDE/PROGRESS.md` | Session-by-session log: what was done, what works, what is broken, what is next | Every session, no exceptions |
| `CLAUDE/SHAPES.md` | Living catalogue of every 3D shape: name, vertex approach, colour, file, status | Any time a shape is added, modified, or its status changes |
| `CLAUDE/SHADERS.md` | Shader design: uniforms, GLSL logic, matrix pipeline, wireframe approach | Any time shaders or the uniform interface changes |
| `CLAUDE/REFERENCE.md` | Windmill reference photo, shape-to-primitive mapping, colour palette, rotor/gap notes | If any visual design decision changes |
| `CLAUDE/LAYOUT.md` | Definitive scene layout: top-down + front view, all world-unit dimensions, build order | If any spatial layout decision changes |

### Session Start Protocol — follow this every single session, in order

1. Read `CLAUDE.md` (this file) in full.
2. Read `CLAUDE/PROGRESS.md` — understand exactly where the last session ended and what is broken.
3. Read `CLAUDE/LAYOUT.md` — refresh the spatial layout before touching any geometry.
4. Read `CLAUDE/SHAPES.md` — know which shapes exist and which are pending.
5. Read any other CLAUDE docs relevant to the session's specific task (e.g. `SHADERS.md` if working on shaders).
6. Only then begin implementation.

### Session End Protocol — non-negotiable, every session

Before ending any session, update every file that was touched or affected:

1. **`CLAUDE/PROGRESS.md`** — always. Add a new session block with:
   - Date
   - What was implemented this session
   - Current status of every area (use the status table)
   - Any bugs found, even if not yet fixed
   - Exact next steps for the following session

2. **`CLAUDE/SHAPES.md`** — if any shape was started, completed, or changed:
   - Add a new entry using the template, or update the status of an existing one
   - Keep the shape count summary table accurate

3. **`CLAUDE/DECISIONS.md`** — if any non-trivial implementation choice was made:
   - Add a new DEC-XXX entry with the decision, rationale, and impact
   - Include things like: how a shape's gap was implemented, which axis the rotor spins on,
     how wireframe indices are stored, how the MVP is constructed

4. **`CLAUDE/SHADERS.md`** — if shaders or uniforms were modified:
   - Update the uniform table
   - Update the matrix pipeline description
   - Note any GLSL changes

5. **`CLAUDE/LAYOUT.md`** — if any spatial or dimensional decision was revised:
   - Update the affected dimension table or section
   - Note what changed and why at the bottom of the file

6. **`CLAUDE/REFERENCE.md`** — if any visual or colour decision was changed:
   - Update the shape mapping table or colour palette

### Rules for keeping memory accurate

- **Never mark something as done unless it compiles and renders correctly.**
- **Never leave a session without updating PROGRESS.md.** Even a 2-line note is better than nothing.
- **If a dimension in LAYOUT.md changes during implementation, update LAYOUT.md immediately** — do not let the document drift from the actual code.
- **SHAPES.md is the single source of truth for shape count.** The submission checklist requires 20+ shapes; SHAPES.md must reflect the real count at all times.
- **If a decision contradicts an earlier decision in DECISIONS.md, add a new entry** explaining the change — do not silently edit old entries.

