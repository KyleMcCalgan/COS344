# COS344 Practical 2 — Implementation Plan
**Due:** 23-03-2026 at 10:00 | **Student:** u24648826

---

## 1. What Needs to Be Done (Summary)

Render a 2D top-down mini golf floor plan in OpenGL 3.3 with:
- A static scene with specific visual elements (floor, grass, borders, obstacles, river, ball, hole)
- A selection system (keys 1–4, 0 to deselect) with pastel colour highlight
- Cumulative transformations on the selected object (W/S/A/D translate, +/- scale, Q/E rotate)
- Wireframe toggle (Enter key, GL_LINES, colour preserved)
- All matrix math hand-written using Prac1 Matrix code

---

## 2. File Setup — What to Copy and Where

All files must live **flat** in `/home/student/COS344/prac2/` (same directory as `main.cpp`).

### Copy from `prac2/Prac1/` to `prac2/`:
```
Matrix.h        (template Matrix<n,m> class)
Matrix.cpp      (implementation — #included by Matrix.h, not compiled separately)
Vector.h        (template Vector<n> class)
Vector.cpp      (implementation — #included by Vector.h, not compiled separately)
```

> **Do NOT copy** `Shape.h`, `Triangle.h/cpp`, `Square.cpp/h`, `main.cpp` from Prac1 — those are P1-specific. We will write a new OpenGL-specific RenderShape struct.

### Copy from `344Example1/` to `prac2/`:
```
glad.c          (OpenGL loader — required for glewInit replacement)
```
> Check if the existing `prac2/` already has `glad.c` — if not, copy it.

### Already in `prac2/`:
```
main.cpp        (starter — GLFW/GLEW setup already done)
shader.cpp      (provided shader loader)
shader.hpp      (provided shader header)
makefile        (needs updating)
```

### Create new files in `prac2/`:
```
vertex.glsl     (vertex shader using uniform mat3 model + mat3 projection)
fragment.glsl   (fragment shader using uniform vec3 colour)
```

---

## 3. How Prac1 Matrix Code Is Used Here

The `Matrix<n,m>` template from P1 supports:
- `Matrix<n,m> * Matrix<m,a>` → matrix multiplication
- `Matrix<n,m> * float` → scalar multiply
- `Matrix<n,m> + Matrix<n,m>` → addition
- `~Matrix<n,m>` → transpose
- Copy constructor and assignment

**For Prac 2 we use `Matrix<3,3>`** (3×3 homogeneous coordinates for 2D transforms).

Helper functions to write (in `main.cpp` or a `mat3utils.h`):

```cpp
Matrix<3,3> identity3();
Matrix<3,3> translate2D(float tx, float ty);
Matrix<3,3> rotate2D(float radians);     // CCW positive
Matrix<3,3> scale2D(float sx, float sy);
// Combine: modelMatrix = translate2D(tx,ty) * rotate2D(rot) * scale2D(sx,sy)
```

These produce the `uniform mat3 model` uploaded to the vertex shader each frame.

**Important:** Matrix<n,m> stores data as `float**` (row-major). When uploading to OpenGL via `glUniformMatrix3fv`, OpenGL expects column-major — so we must transpose before uploading, or pass `GL_TRUE` for the transpose parameter in `glUniformMatrix3fv`.

---

## 4. Shader Files

### `vertex.glsl`
```glsl
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 model;
uniform mat3 projection;

void main() {
    vec3 pos = projection * model * vec3(aPos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
```

### `fragment.glsl`
```glsl
#version 330 core
uniform vec3 colour;
out vec4 FragColor;

void main() {
    FragColor = vec4(colour, 1.0);
}
```

**Projection matrix:** A simple identity (if we use NDC coordinates directly) or an orthographic mapping. Using an identity `mat3` projection and NDC coordinates for vertices is the simplest approach — world space IS NDC space.

---

## 5. RenderShape Struct (new, for OpenGL)

```cpp
struct RenderShape {
    GLuint VAO, VBO;            // filled draw
    GLuint wireVAO, wireVBO;    // wireframe draw (GL_LINES edge pairs)
    int vertCount;              // filled: number of vertices (triangles)
    int wireVertCount;          // wireframe: number of line-pair vertices

    glm::vec3 colour;           // original colour
    glm::vec3 pastelColour;     // pastel version when selected

    bool selected;
    bool isSelectable;
    int selectKey;              // 1, 2, 3, 4 (or 0 = not selectable)

    // Transform state (cumulative, rebuilt each frame)
    float tx, ty;       // translation
    float rotation;     // radians
    float sx, sy;       // scale

    void init(std::vector<float>& verts, std::vector<float>& wireVerts);
    void draw(GLuint shaderID, bool wireframeMode);
    Matrix<3,3> modelMatrix();  // builds T * R * S from P1 Matrix code
};
```

---

## 6. Scene Layout

### Coordinate System
Use NDC (-1 to +1) directly. World space = NDC space. Base vertices centered at origin in model space.

### Objects to Create

| # | Object | Shape | Colour (RGB approx) | Selectable |
|---|--------|-------|---------------------|------------|
| 1 | Background | `glClearColor` | Dark charcoal (0.15, 0.15, 0.15) | No |
| 2 | Concrete floor | Large rectangle (2 triangles) | Grey (0.55, 0.55, 0.55) | No |
| 3 | Grass area | Rectangle (2 triangles) | Green (0.2, 0.6, 0.2) | No |
| 4 | Border N | Thin rectangle | Dark brown (0.3, 0.15, 0.05) | No |
| 5 | Border S | Thin rectangle | Dark brown | No |
| 6 | Border E | Thin rectangle | Dark brown | No |
| 7 | Border W | Thin rectangle | Dark brown | No |
| 8 | Starting point | Rectangle | Maroon (0.5, 0.0, 0.0) | No |
| 9 | River/stream | Rectangle | Blue (0.2, 0.4, 0.9) | No |
| 10 | Obstacle 1 (inst 1) | Triangle | Orange (0.9, 0.5, 0.1) | Key 2 |
| 11 | Obstacle 1 (inst 2) | Triangle | Orange | No |
| 12 | Obstacle 2 (inst 1) | Rectangle | Purple (0.5, 0.1, 0.8) | Key 3 |
| 13 | Obstacle 2 (inst 2) | Rectangle | Purple | No |
| 14 | Obstacle 3 (inst 1) | Pentagon/shape | Teal (0.1, 0.7, 0.7) | No |
| 15 | Obstacle 3 (inst 2) | Pentagon/shape | Teal | No |
| 16 | Golf hole | High-poly circle (≥50 verts) | Pink (1.0, 0.5, 0.7) | Key 4 |
| 17 | Golf ball | Low-poly circle (6–8 verts) | White (1.0, 1.0, 1.0) | Key 1 |

> Total distinct colours: charcoal bg, grey floor, green grass, dark brown barriers, maroon start, blue river, orange obstacle1, purple obstacle2, teal obstacle3, pink hole, white ball = **11 colours** ✓ (need ≥9)

### Shape Requirements Coverage
- **Rectangles:** concrete floor, grass, borders, starting point, river, obstacle2 ✓ (many)
- **Triangles:** obstacle1 instances ✓ (≥2)
- **High-poly circle:** golf hole (≥50 verts) ✓
- **Low-poly circle:** golf ball (6–8 verts) ✓

### Pastel Colours (for selection highlight)
| Object | Original | Pastel |
|--------|----------|--------|
| Golf ball (key 1) | White (1,1,1) | Light yellow (1.0, 1.0, 0.7) |
| Obstacle 1 (key 2) | Orange (0.9,0.5,0.1) | Light peach (1.0, 0.85, 0.7) |
| Obstacle 2 (key 3) | Purple (0.5,0.1,0.8) | Lavender (0.8, 0.7, 1.0) |
| Golf hole (key 4) | Pink (1.0,0.5,0.7) | Light pink (1.0, 0.8, 0.9) |

---

## 7. Circle Generation

```cpp
// High-poly circle (golf hole): N=60 vertices
// Low-poly circle (golf ball): N=8 vertices
// Both use GL_TRIANGLE_FAN decomposed to GL_TRIANGLES:
//   centre, v[i], v[i+1] for i in 0..N-1 (wrapping)

std::vector<float> makeCircle(float cx, float cy, float r, int N) {
    std::vector<float> verts;
    for (int i = 0; i < N; i++) {
        float a0 = 2.0f * M_PI * i / N;
        float a1 = 2.0f * M_PI * (i+1) / N;
        // centre
        verts.push_back(0.0f); verts.push_back(0.0f);
        // v[i]
        verts.push_back(r * cosf(a0)); verts.push_back(r * sinf(a0));
        // v[i+1]
        verts.push_back(r * cosf(a1)); verts.push_back(r * sinf(a1));
    }
    return verts;
}
// Wireframe version: consecutive perimeter edges only (N pairs)
```

**Vertices are centered at (0,0) in model space** — translations set via `tx`, `ty` in the transform state.

---

## 8. Rectangle/Triangle Generation

All vertices **centred at (0,0)** in model space. Use `tx`/`ty` to position in world space.

```cpp
// Rectangle: width w, height h, centred at origin
// Two triangles (GL_TRIANGLES):
// (-w/2,-h/2), (w/2,-h/2), (w/2,h/2)   <- tri 1
// (-w/2,-h/2), (w/2,h/2), (-w/2,h/2)   <- tri 2
// Wireframe: 4 edges (8 vertices for GL_LINES)

// Triangle: define 3 points around centroid
// Wireframe: 3 edges (6 vertices for GL_LINES)
```

---

## 9. Input Handling

```cpp
// In key_callback or processInput():
// Selection (single select, deselects others):
//   key 0 → deselect all
//   key 1 → select golf ball
//   key 2 → select obstacle1
//   key 3 → select obstacle2
//   key 4 → select golf hole

// Transforms (act on currently selected object):
//   W → selected.ty += 0.05f
//   S → selected.ty -= 0.05f
//   A → selected.tx -= 0.05f
//   D → selected.tx += 0.05f
//   + (GLFW_KEY_EQUAL + shift, or KP_ADD) → selected.sx *= 1.1f; selected.sy *= 1.1f
//   - (GLFW_KEY_MINUS or KP_SUBTRACT) → selected.sx /= 1.1f; selected.sy /= 1.1f
//   E → selected.rotation -= 0.0873f  (5° CW = -5° in CCW convention)
//   Q → selected.rotation += 0.0873f  (5° CCW)

// Wireframe toggle (Enter key, debounced 200ms):
//   bool wireframeMode = false;
//   double lastToggleTime = 0.0;
//   if (key==ENTER && glfwGetTime()-lastToggleTime > 0.2) {
//       wireframeMode = !wireframeMode;
//       lastToggleTime = glfwGetTime();
//   }
```

---

## 10. Render Loop Structure

```cpp
while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderID);

    // Upload identity projection (world = NDC)
    Matrix<3,3> proj = identity3();
    // glUniformMatrix3fv(projLoc, 1, GL_TRUE, flattenMatrix(proj));

    // Draw all shapes in order (back to front):
    for (auto& shape : allShapes) {
        Matrix<3,3> model = shape.modelMatrix();
        // Upload model matrix (transposed for OpenGL column-major)
        // Upload colour (pastel if selected, else normal)
        shape.draw(shaderID, wireframeMode);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

---

## 11. Matrix Upload to OpenGL

`Matrix<n,m>` stores `float**` (row-major). OpenGL `glUniformMatrix3fv` expects column-major.
Two options:
1. Pass `GL_TRUE` for the transpose flag: `glUniformMatrix3fv(loc, 1, GL_TRUE, data)`
2. Or transpose the matrix first using the `~` operator from P1

Need a helper to flatten `Matrix<3,3>` to `float[9]`:
```cpp
void flattenMatrix(const Matrix<3,3>& m, float out[9]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            out[i*3+j] = m[i][j];
}
// Then: glUniformMatrix3fv(loc, 1, GL_TRUE, out);  // GL_TRUE = transpose row→col-major
```

---

## 12. Makefile Update

```makefile
files = Matrix.cpp Vector.cpp   # NOT compiled separately — they're header-included
                                 # but keep empty if using header-only templates

main: main.cpp glad.c shader.cpp
    g++ -std=c++11 -g shader.cpp main.cpp glad.c -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
    rm -f *.o main

run:
    ./main

all:
    make clean
    make
    make run
```

> Note: `Matrix.cpp` and `Vector.cpp` are `#include`d directly from their `.h` files (template pattern), so they do NOT need to be in the compiler command. Same for `Triangle.cpp`, `Square.cpp`. Only `shader.cpp` and `glad.c` need explicit compilation.

---

## 13. Step-by-Step Implementation Order

1. **Copy files:** Matrix.h, Matrix.cpp, Vector.h, Vector.cpp from `Prac1/` to `prac2/`. Copy `glad.c` from `344Example1/` if not present.
2. **Write shaders:** `vertex.glsl` and `fragment.glsl`
3. **Update makefile:** Ensure shader.cpp and glad.c are included, add `-std=c++11`
4. **Write matrix helpers:** `identity3()`, `translate2D()`, `rotate2D()`, `scale2D()`, `flattenMatrix()`
5. **Write shape generators:** `makeRect()`, `makeTriangle()`, `makeCircle()`, `makeRectWire()`, `makeTriangleWire()`, `makeCircleWire()`
6. **Write RenderShape struct:** constructor, `init()`, `modelMatrix()`, `draw()`
7. **Build the scene:** Instantiate all shapes with correct positions, colors, transform initial states
8. **Wire up input:** Key callbacks for 0/1/2/3/4, W/A/S/D, +/-, Q/E, Enter
9. **Render loop:** Clear, draw all shapes, swap buffers
10. **Test wireframe toggle**
11. **Test all transforms on each selectable object**
12. **Verify all 9+ colours, all shape types, all visual elements**
13. **Set window title to `u24648826`**
14. **Final build check:** `make clean && make`

---

## 14. Submission Checklist

- [ ] Window title is `u24648826`
- [ ] Compiles with `make` from terminal
- [ ] All helper files flat in same dir as `main.cpp`
- [ ] No inline implementations in NEW header files
- [ ] At least 9 distinct solid colours visible
- [ ] All required visual elements present
- [ ] 2+ rectangles (GL_TRIANGLES), 2+ triangles, high-poly circle, low-poly circle
- [ ] Keys 1–4 select correct objects, 0 deselects, colours change to pastel
- [ ] W/S/A/D translate, +/- scale, Q/E rotate work cumulatively
- [ ] Enter toggles wireframe (GL_LINES, colours preserved, transforms respected)
- [ ] Submitted as archive to ClickUP before 23-03-2026 10:00

---

## 15. Rubric Assessment (as of 22-03-2026)

### Gaps / Risks

#### Layout [30 marks]
- **Three obstacle types** — only 2 exist (orange triangles + purple rects). A **third distinct obstacle type** is needed for full marks.
- **Golf hole / ball shape** — both are circles, should be correct visually.

#### Shape Requirements [12 marks]
- Two distinct circles: 8-segment ball (low poly) + 60-segment hole (high poly) ✓
- Two distinct triangles: river bank triangles + obstacle triangles ✓
- Two distinct rectangles: many exist ✓

#### Colour [18 marks]
- **Three distinct obstacle colours** — only 2 (orange, purple). Adding a third obstacle type fixes this simultaneously.
- All other colours present and distinct ✓

#### Selection [18 marks]
- Keys 1–4 correct, pastel highlight works, 0 deselects ✓
- Should be full marks.

#### Transformation [20 marks]
- All transforms work on all selectable shapes ✓
- No centre-reset between transforms ✓

#### Wireframe [12 marks]
- ENTER toggles wireframe, applies to all shapes including obstacles ✓

### What to Fix

| Priority | Issue | Marks at stake |
|---|---|---|
| **High** | Add a **third obstacle type** | Fixes Layout + Colour = up to **6 marks** |

### Bonus Status

| Bonus | Status |
|---|---|
| Multi-select (Ctrl) | Done |
| Bounds checking (ball off-screen) | Done |
| Ball collision with obstacles | Done |
| Dynamic add/remove (T/Y/Delete) | Done |
| Extra detail on ball (soccer spots) | Done |
| Save layout (F5) | Done |
| Load layout (--import) | Done |
| Click to select | Not done |
| Screenshot via OpenGL | Not done |
| Ball bouncing off obstacles | Not done (push-out only, no velocity) |

7 of 10 bonus features implemented — already at the 10-mark cap if all are accepted.
