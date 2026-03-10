\# COS344 Practical 2: 2D Rendering — Mini Golf Floor Plan



\*\*Due:\*\* 23-03-2026 at 10:00 | \*\*Total Marks:\*\* 110 (+10 bonus)  

\*\*Window title must be your student number (u24648826).\*\*



---



\## Reusing Practical 1



The spec explicitly states: \*"You may use your Practical 1 in this practical."\*  

Your matrix math utilities from P1 (3x3 or 4x4 matrix structs, multiply, transpose, etc.) should be brought directly into this project. Place all helper files in the \*\*same directory as `main.cpp`\*\* — no subdirectories. The matrix arithmetic you built there is the backbone of your transformation system here.



---



\## Tech Stack \& Hard Constraints



\- \*\*OpenGL 3.3\*\* — do not use any higher-version features

\- \*\*C++11\*\*

\- \*\*Allowed libraries only\*\* — anything not on this list is forbidden:

&nbsp; - `stdio.h`, `stdlib.h`, `iostream`, `iomanip`, `cmath`, `sstream`, initializer lists

&nbsp; - `GL/glew.h`, `GLFW/glfw3.h`

&nbsp; - `glm/glm.hpp` — \*\*only for the data types\*\* (vec2, vec3, etc.), \*\*not\*\* for any math functions (no `glm::translate`, `glm::rotate`, `glm::scale`, `glm::perspective`, `glm::lookAt`, or any matrix operations)

&nbsp; - `shader.hpp` and `glad.c` — provided files for shader compilation/linking



\- \*\*FORBIDDEN:\*\*

&nbsp; - Any GLM matrix arithmetic — all matrix math must be hand-written (reuse P1)

&nbsp; - Built-in OpenGL shape generators — every shape is raw vertex data you compute

&nbsp; - Built-in OpenGL transform functions — no `glTranslate`, `glRotate`, `glScale`

&nbsp; - `glPolygonMode` for wireframe — you must use `GL\_LINES` manually

&nbsp; - Texture mapping of any kind

&nbsp; - Pre-built objects or textures



\- All `.h`, `.cpp`, `.hpp` helper files must live in the \*\*same directory\*\* as `main.cpp`

\- Must compile from the \*\*terminal via Makefile\*\* with no IDE dependency

\- No inline implementations in header files (except those already provided)

\- All functions implemented in corresponding `.cpp` files



---



\## Scene Overview



Render a \*\*2D top-down floor plan\*\* of a single hole in a miniature golf course. This is a static layout view — not a simulation. Think of it as a bird's-eye architectural diagram of the course. The spec hints that this layout will feed into your homework assignment (the actual golf simulation), so design it with a realistic course in mind.



---



\## Required Visual Elements



Every element below must be present. Colours listed are from the example figure — you may choose your own as long as they are distinct.



| Element | Visual representation | Notes |

|---|---|---|

| Concrete floor | Grey filled shape | Must \*\*not\*\* fill the entire window — leave a visible border gap between floor edges and window edges |

| Grass area | Green filled shape | Inside the course boundary |

| Border/barrier | Dark brown rectangles | Creates boundary between grass and concrete floor |

| Starting point | Maroon rectangle | Marks where the ball begins |

| River/stream | Blue rectangle | Runs through the course |

| Obstacle type 1 | Your choice of colour + shape | Must be a unique colour+shape combo |

| Obstacle type 2 | Your choice of colour + shape | Different colour AND shape from type 1 |

| Obstacle type 3 | Your choice of colour + shape | Different colour AND shape from types 1 and 2 |

| Golf hole | Circle — high-poly (≥50 verts) | Distinctive colour (pink in example) |

| Golf ball | Circle — low-poly (6–10 verts) | Must be white |



You may add more decorative details beyond the above if you wish.



---



\## Shape Requirements



Your layout must contain \*\*at least 2 of the following 3 distinct shape types\*\*:



\### Rectangle / Square

\- At least two distinct rectangles or squares present in the scene

\- Built from two triangles (GL\_TRIANGLES) — no `GL\_QUADS`



\### Triangle

\- At least two distinct triangles present in the scene

\- Defined by 3 manually specified vertices each



\### Circle (both variants required if you include circles)

\- \*\*High-polygon circle:\*\* at least \*\*50 vertices\*\* — renders as a smooth circle

\- \*\*Low-polygon circle:\*\* between \*\*6 and 10 vertices\*\* — renders as a recognisable polygon (hexagon to decagon)

\- Both are generated mathematically using `cos`/`sin` — no built-in circle functions

\- "Distinct" means the shape as a whole — not the internal triangles used to tessellate it



> Tip: The golf ball and golf hole are your two natural circles. Golf hole = high-poly (smooth, ≥50 verts), golf ball = low-poly (6–10 verts). This satisfies both circle requirements while also fulfilling layout requirements simultaneously.



---



\## Colour Requirements



Your scene must contain \*\*at least 9 distinct, solid colours\*\*. No gradients. No textures.



1\. \*\*Grey\*\* — concrete floor

2\. \*\*Green\*\* — grass area

3\. \*\*Dark brown\*\* — border/barrier rectangles

4\. \*\*Maroon\*\* — starting point

5\. \*\*Blue\*\* — river/stream (must be distinctly blue)

6\. \*\*Distinctive colour for golf hole\*\* (pink in example)

7\. \*\*White\*\* — golf ball (must be distinctly white)

8\. \*\*Distinctive background colour\*\* — set via `glClearColor`, must not match anything else in the scene

9\. \*\*3 distinct colours for the 3 obstacle types\*\* — each obstacle type gets its own colour



> Hint from spec: Use `glClearColor` for the background — this sets the colour outside your floor shape and counts as one of your 9 required colours.



---



\## Selection System



Only 4 objects are selectable. Each is bound to a number key. At most one object is selected at a time.



| Key | Object selected |

|---|---|

| `1` | Golf ball |

| `2` | Obstacle type 1 |

| `3` | Obstacle type 2 (must be a \*\*different type\*\* from key 2) |

| `4` | Golf hole |

| `0` | Deselect — clears current selection, no object highlighted |



\*\*Visual feedback for selection:\*\*  

When an object is selected, its render colour changes to a \*\*pastel version\*\* of its original colour. For example, a red obstacle becomes a light pink. When deselected (key `0`), it returns to its original colour.



\*\*Rubric note:\*\* Each key must map to exactly one object instance. Keys bound to the wrong object type, or keys that select but don't trigger a colour change, lose marks.



---



\## Transformation System



Transformations act on the \*\*currently selected object only\*\*. They are \*\*cumulative\*\* — pressing W three times moves the object up three increments from wherever it currently is. The object must \*\*never snap back\*\* to its original position when a new transform key is pressed.



All transformation matrices must be computed manually using your P1 matrix code. The recommended approach:

\- Store per-object state: `translation (x, y)`, `rotation (float, radians)`, `scale (sx, sy)`

\- Each frame, reconstruct the full model matrix as: `Translation \* Rotation \* Scale`

\- Keep base vertices centred at the origin in model space — this ensures rotation and scale naturally occur around the object's own centre

\- Upload the resulting matrix as a `uniform mat3` (or `mat4`) to the vertex shader each frame



| Key | Transformation | Details |

|---|---|---|

| `W` | Translate up | Increment Y translation |

| `S` | Translate down | Decrement Y translation |

| `A` | Translate left | Decrement X translation |

| `D` | Translate right | Increment X translation |

| `+` | Scale up | Multiply scale factor, centred on object |

| `-` | Scale down | Divide scale factor, centred on object |

| `E` | Rotate clockwise | Increment rotation angle |

| `Q` | Rotate counter-clockwise | Decrement rotation angle |



\*\*Step size:\*\* Keep increments small (e.g., 0.05 NDC units for translation, 5–10° for rotation, 1.1x for scale) so multiple key presses are needed to reach the screen edge. This is required for the demo to work correctly.



\*\*Rubric penalty:\*\* 1 mark deducted per transform criterion if the object visibly resets to its original/centre position before the next transform is applied.



---



\## Wireframe Mode



Toggle with the \*\*Enter key\*\*. The program starts in filled (normal) mode.



Requirements:

\- Must use \*\*`GL\_LINES`\*\* to draw the outlines of every shape — replacing the filled draw calls when active

\- Must \*\*maintain colour\*\* — each shape's wireframe lines drawn in that shape's colour so the layout is still identifiable by colour

\- Must \*\*respect all transformations\*\* — a moved/rotated/scaled object's wireframe must match its transformed state exactly

\- \*\*Debounce the Enter key\*\* — add a cooldown (e.g., 200–300ms) between toggles so one physical keypress doesn't toggle it multiple times



For each shape type, define a separate wireframe draw path:

\- Rectangle: 4 `GL\_LINES` calls connecting the 4 corner pairs

\- Triangle: 3 `GL\_LINES` calls connecting the 3 edge pairs

\- Circle: N `GL\_LINES` calls connecting consecutive perimeter vertices



The wireframe and filled modes share all the same transform logic — only the draw primitive changes.



---



\## Implementation Architecture (Suggested)



```

project/

├── main.cpp                    # GLFW setup, input handling, render loop

├── Makefile

├── shader.hpp                  # Provided

├── glad.c                      # Provided

├── mat3.h / mat3.cpp           # Your matrix math from Practical 1 — bring this in directly

├── Shape.h / Shape.cpp         # Renderable object struct/class

└── vertex.glsl / fragment.glsl # Your shaders

```



\### Shape struct (suggested fields)

```cpp

struct Shape {

&nbsp;   GLuint VAO, VBO;

&nbsp;   std::vector<float> vertices;  // base vertices in local/model space, centred at origin

&nbsp;   std::vector<float> wireframeVertices; // edge pairs for GL\_LINES



&nbsp;   glm::vec3 colour;             // original colour

&nbsp;   glm::vec3 pastelColour;       // highlight colour when selected

&nbsp;   bool selected;



&nbsp;   // Transform state — rebuilt into model matrix each frame using P1 matrix code

&nbsp;   float tx, ty;       // translation

&nbsp;   float rotation;     // radians

&nbsp;   float sx, sy;       // scale



&nbsp;   bool isSelectable;

&nbsp;   int selectKey;      // 1, 2, 3, or 4



&nbsp;   void draw(bool wireframe, bool useWireframeMode);

};

```



\### Vertex shader (suggested)

```glsl

\#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat3 model;       // hand-built transform matrix from P1 code

uniform mat3 projection;  // maps world coords to NDC



void main() {

&nbsp;   vec3 pos = projection \* model \* vec3(aPos, 1.0);

&nbsp;   gl\_Position = vec4(pos.xy, 0.0, 1.0);

}

```



\### Fragment shader (suggested)

```glsl

\#version 330 core

uniform vec3 colour;

out vec4 FragColor;



void main() {

&nbsp;   FragColor = vec4(colour, 1.0);

}

```



---



\## Marking Rubric Detail



\### Layout Requirements (30 marks)

\- Concrete floor shape present and not filling entire window

\- Starting point (maroon rectangle) present

\- River/stream (blue rectangle) present

\- At least 3 different types of obstacles present

\- Golf hole present and correct shape (circle)

\- Golf ball present and correct shape (circle)

\- Barrier/border between grass and floor present



\### Shape Requirements (12 marks)

\- Two distinct rectangles or squares present

\- Two distinct triangles present

\- Both a high-polygon circle (≥50 verts) AND a low-polygon circle (6–10 verts) present



\### Colour Requirements (18 marks)

\- 3 distinct colours across the 3 obstacle types

\- Distinct starting point colour

\- Distinct golf hole colour

\- Distinct water/blue colour

\- Distinct white for golf ball

\- Distinct grass colour

\- Distinct grey concrete colour

\- Distinct background colour (not matching anything else in the scene)



\### Selection Requirements (18 marks)

\- Golf ball selectable with key `1`, colour changes to pastel

\- Obstacle type 1 selectable with key `2`, colour changes to pastel

\- Obstacle type 2 selectable with key `3`, colour changes to pastel

\- Golf hole selectable with key `4`, colour changes to pastel

\- Deselection with key `0` works and reverts colour correctly



\### Transformation Requirements (20 marks)

\- Horizontal translation (A/D) works and is cumulative

\- Vertical translation (W/S) works and is cumulative

\- Scaling (+/-) works from object centre

\- Rotation (Q/E) works from object centre

\- All 4 selectable objects can perform all transforms



\### Wireframe (12 marks)

\- Wireframe renders correctly for all shapes

\- Wireframe maintains colour scheme per shape

\- Wireframe correctly reflects all active transformations



---



\## Bonus Marks (up to +10, worth 2 each)



\- Multi-object selection (transform multiple selected objects simultaneously)

\- Click-to-select (mouse click selects the object under cursor)

\- Off-screen bounds checking (objects cannot be moved outside the window)

\- Obstacle collision (golf ball cannot pass through obstacle shapes)

\- Golf ball bouncing off obstacles

\- Dynamic object add/remove at runtime

\- Extra visual details/patterns on objects (no texture mapping allowed)

\- Save layout state to a text file

\- Load layout state from a text file

\- Save screenshot to file using OpenGL pixel reading (`glReadPixels`)



---



\## Submission Checklist



\- \[ ] Code compiles cleanly from terminal with `make`

\- \[ ] Window title is student number `u24648826`

\- \[ ] All helper files (including P1 matrix code) in same directory as `main.cpp`

\- \[ ] Makefile included with compile instructions

\- \[ ] No inline implementations in header files

\- \[ ] Submitted as archive to \*\*ClickUP\*\* before 23-03-2026 at 10:00

\- \[ ] Late window: up to 1 hour after deadline with 20% penalty on achieved mark

\- \[ ] During demo: upload the downloaded ClickUP archive to \*\*FitchFork\*\* in tutor's presence

