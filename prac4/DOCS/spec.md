# COS344 - Computer Graphics
## Practical 4 Specification: Translucency, Textures and Light

Department of Computer Science  
Faculty of Engineering, Built Environment & IT  
University of Pretoria

Copyright ©2026 by FJ Redelinghuys. All rights reserved.

Release Date: 16-03-2026 at 06:00  
Start By Date: 14-04-2026  
Due Date: 11-05-2026 at 10:00  
Total Marks: 80

---

## Contents

1. General Instructions
2. Overview
3. Your Task
   - 3.1 Shape requirements
   - 3.2 Texture requirements
   - 3.3 Colour requirements
   - 3.4 Light requirements
   - 3.5 Transformation requirements
     - 3.5.1 Rotations
     - 3.5.2 Translation
     - 3.5.3 Reset Command
   - 3.6 Wireframe
   - 3.7 Texture Maps
4. Marking rubric
5. Bonus marks
6. Implementation Details
7. Submission
8. Demo Instructions

---

## 1 General Instructions

- Read the entire assignment thoroughly before you begin coding.
- This assignment should be completed individually.
- Every submission will be inspected with the help of dedicated plagiarism detection software.
- Be ready to upload your assignment well before the deadline. There is a late deadline which is 1 hour after the initial deadline which has a penalty of 20% of your achieved mark. No extensions will be granted.
- If your code does not compile, you will be awarded a mark of 0. The output of your program will be primarily considered for marks, although internal structure may also be tested (eg. the presence/absence of certain functions or structure).
- Failure of your program to successfully exit will result in a mark of 0.
- Note that plagiarism is considered a very serious offence. Plagiarism will not be tolerated, and disciplinary action will be taken against offending students. Please refer to the University of Pretoria's plagiarism page at https://portal.cs.up.ac.za/files/departmental-guide/.
- Unless otherwise stated, the usage of additional libraries outside of those indicated in the assignment, will not be allowed. Some of the appropriate files that you have submit will be overwritten during marking to ensure compliance to these requirements. Please ensure you use C++11.
- All functions should be implemented in the corresponding cpp file. No inline implementation in the header file apart from the provided functions.
- The usage of ChatGPT and other AI-Related software to generate submitted code is strictly forbidden and will be considered as plagiarism.
- The use of this practical, in any form, by any company/business/organisation outside the University of Pretoria is strictly forbidden. This includes, but is not limited to, the use of the practical for discussion sessions, review sessions, marketing tools, or providing certain aspects as a free service.
- No pre-build objects and textures may be used. All objects and textures that you need to use must be created by yourself.
- You must use OpenGL version 3.3 for this practical.

---

## 2 Overview

For this practical, you will need to render a semi-translucent 3D glass golf ball resting on a flat plane. Inside the golf ball, there is a light which shines through the glass and onto the plane below. The number of vertices that the golf ball and the plane need to be adjustable, such that the effect of the number of vertices on the overall quality of the render can be observed.

---

## 3 Your Task:

For this practical, you will need to render a scene similar to the sketch depicted in Figures 1 and 2.

In Figures 1 and 2, the white box represents the flat plane (floor) that the golf ball will rest on, and the grey circle/sphere represents the golf ball. The yellow dot represents the initial position of the light that is inside the golf ball, and the orange lines/circle represent the light that is shining onto the floor. Remember, although Figures 1 and 2 are 2D, your render needs to be a single 3D scene.

In the sections below, the different requirements are laid out.

### 3.1 Shape requirements

Your rendering should contain the following shapes:

- A sphere whose number of vertices can be increased and decreased during runtime.
  - Assign two keys on your keyboard (of your choosing) to control the number of vertices assigned to the sphere.
- A flat plane whose number of vertices can be increased and decreased during runtime.
  - Assign two keys on your keyboard (of your choosing) to control the number of vertices assigned to the floor.
  - Hint: This may require you to create a series of smaller planes that are connected to form a bigger flat plane.

### 3.2 Texture requirements

To make the golf ball appear more realistic, texture mapping needs to be applied. You will be required to create your own texture maps for this practical. The downloading or use of a pre-existing texture map will result in forfeiting all marks for the texture requirement. The following two texture maps are required:

- A colour texture map that will shade the golf ball such that it is the correct shaded regions (dimples).
- A displacement texture map that will be able to morph the surface of the golf ball such that the surface has accurate golf ball dimples.
- An alpha texture map that will make the dimples transparent while the normal surface is non-transparent.

The different texture maps should be enabled and disabled using the following buttons:

- The **B Key** should be used to toggle the colour texture map on and off. When the colour texture map is disabled, the sphere should be a uniform translucent colour. When the colour texture map is enabled, the sphere should have dimples that is shaded a bit darker than the rest of the golf ball.
- The **N Key** should be used to toggle the displacement texture map on and off. When the displacement texture map is disabled, the surface of the golf ball should be smooth, but when the displacement texture map is enabled, the surface should have visible dimples (in other words, should not be smooth). Note that the actual surface should be changed. The use of bump texture mapping is not allowed.
- The **M Key** should be used to toggle the alpha texture map on and off. When disabled, the entire golf ball should be semi-transparent according to an alpha value, which is changed with the + and - keys. When the alpha texture map is enabled, only the dimples should be transparent, according to the same alpha value previously mentioned, and the rest of the golf ball should be fully opaque.

The colour of the golf ball can change (discussed in the following section) and this should be taken into account when applying the texture mappings as discussed above.

### 3.3 Colour requirements

You will need to comply with the following colour requirements:

**Floor**
- Assign two keys on your keyboard (of your choosing) to control the colour of the floor.
- Using these keys, you will need to be able to cycle through the following colours:
  - Red
  - Green
  - Blue
  - White
  - Black
  - 5 other distinct colours of your choosing.

**Glass Golf Ball**
- Assign two different keys on your keyboard (of your choosing) to control the colour of the glass golf ball.
- Using these keys, you will need to be able to cycle through the following colours:
  - Red
  - Green
  - Blue
  - White
  - Black
  - 5 other distinct colours of your choosing.
- Assign two further keys (the **+** and **-** keys) to control the alpha value of the glass golf ball.
- Hint: Make use of RGBA and assign the colour buffer to take in RGBA colours (a vec4 value) instead of the standard RGB (a vec3 value) colours.

**Light**
- Assign two different keys on your keyboard (of your choosing) to control the colour of the light.
- Using these keys, you will need to be able to cycle through the following colours:
  - Red
  - Green
  - Blue
  - White
  - 5 other distinct colours of your choosing.
- Note that the light and glass colour should also affect the floor's colour. In other words, if the light is yellow, the glass is green, and the floor is purple, the resulting colour of the floor will be a combination of the three colours (remember to take into account how much light shines onto each specific vertex of the floor).

### 3.4 Light requirements

Your light should have the following requirements:

- The light should be a point light, initially positioned in the centre of the golf ball.
- You should at least make use of the pseudo code described in Chapter 4 of the prescribed textbook to calculate the lighting for a point source.
- The light's projection will only be displayed on the floor.
- The light's projection should not affect the golf ball's perception, but this can be added in as bonus marks.

### 3.5 Transformation requirements

In this section, the transformation or animation requirements for your rendering are discussed. Keep the animation rates small such that it allows you to press the key multiple times before going out of camera view.

#### 3.5.1 Rotations

- When the **W key** is pressed, the scene needs to rotate about the x-axis.
- When the **S key** is pressed, the scene needs to rotate about the x-axis in the opposite direction to the W key.
- When the **A key** is pressed, the scene needs to rotate about the y-axis.
- When the **D key** is pressed, the scene needs to rotate about the y-axis in the opposite direction to the S key.
- When the **E key** is pressed, the scene needs to rotate about the z-axis.
- When the **Q key** is pressed, the scene needs to rotate about the z-axis in the opposite direction to the E key.

#### 3.5.2 Translation

- When the **up arrow key** is pressed, the light's local y-axis position needs to increase.
- When the **down arrow key** is pressed, the light's local y-axis position needs decrease.
- When the **left arrow key** is pressed, the light's local x-axis position needs decrease.
- When the **right arrow key** is pressed, the light's local x-axis position needs increase.
- When the **> key** is pressed, the light's local z-axis position needs to increase.
- When the **< key** is pressed, the light's local z-axis position needs to decrease.

Note, through all the transformations of the light, the basic scene setup should remain still, apart from the position of the light. Remember that the local coordinate system of the light can be different from the global coordinate system.

#### 3.5.3 Reset Command

In order to reset the render to its initial state, the **Space Key** should be used. This should reset the number of vertices, positions and rotations of the entire scene to the initial state of the render.

### 3.6 Wireframe

You are also required to implement a wireframe for your render. The wireframe should maintain the colour scheme of your render, such that the colours of the different shapes can be identified. The wireframe should also conform to all the transformations described earlier.

Your program should toggle between the wireframe and the normal render when the **Enter key** is pressed. Hint: you may need to implement a time delay between key presses for the wireframe toggling, so that the expected behaviour is achieved.

Please note that you must use the **GL_LINES** to implement your wireframe. Using the glPolygonMode function will result in the forfeiting of your wireframe marks.

### 3.7 Texture Maps

You will need to create three texture maps as described earlier. Note, you may not simply download a texture map; you will need to create your own. For each texture map that you correctly created, you will receive 4 marks. During the demo, you will need to explain how you created the texture map. Remember to include the texture maps in your submission.

---

## 4 Marking rubric

The following rubric will be used to mark your submitted assignment. Note you will be demoing the practical during the practical sessions on your own computer or a lab computer. Please see Table 1 for the rubric. Note: 1 mark will be subtracted for each transformation if the render is moved back to the centre before a new transformation is applied.

| Assessment Criteria | 0 | 1 | 2 |
|---------------------|---|---|---|
| **Shape requirements (8 marks)** | | | |
| Golf Ball (Sphere) | Not configurable number of vertices | Configurable number of vertices but rendered incorrectly | (full marks) |
| Floor (Plane) | Not configurable number of vertices | Configurable number of vertices but rendered incorrectly | (full marks) |
| **Texture requirements (12 marks)** | | | |
| Colour Texture Map | No colour texture map | Partially correct texture mapping. Correct toggle | (full marks) |
| Displacement Texture Map | No displacement texture map | Partially correct texture mapping. Correct toggle | (full marks) |
| Alpha Texture Map | No alpha texture map | Partially correct texture mapping. Correct toggle | (full marks) |
| **Colour requirements (14 marks)** | | | |
| Floor | Single non changeable colour | At least 5 of the specified colours and controllable | All 10 specified colours and controllable |
| Golf Ball: Colours | Single non changeable colour | At least 5 of the specified colours and controllable | All 10 specified colours and controllable |
| Golf Ball: Alpha value | Alpha values not changeable or present | Alpha value is changeable and has visible effect | — |
| Light: Colours | Single non changeable colour | At least 5 of the specified colours and controllable | — |
| Light: Effect on floor | Light has no effect on floor colour | Light has effect on floor colour but ignores the glass colour | (full marks — combines all three) |
| **Light requirements (12 marks)** | | | |
| Lighting type | There is no lighting effect | There is a light but it is the wrong type (3 marks) | (full marks — correct point light) |
| **Transformation requirements (8 marks)** | | | |
| Rotations | No rotation | Entire scene rotates around at least one axis | Entire scene rotates around two axes |
| Translation (light effect) | No light translations | Light translates but not in the correct directions relative to the scene | Light translates correctly along only one axis relative to the scene |
| **Reset command (2 marks)** | | | |
| Reset Command | There is no reset command | Reset command partially resets the render | Reset command works correctly |
| **Wireframe (12 marks)** | | | |
| Render | No wireframe | Partial wireframe | (full marks) |
| Colour | No wireframe | Partially coloured | (full marks) |
| Transformations | No wireframe | Partial transformations | (full marks) |

*Table 1: Marking rubric*

---

## 5 Bonus marks

There are 10 bonus marks available. Things that can be done for bonus marks are:

- To sort your glass faces, use the Binary Search Tree Partitioning algorithm (5 marks).
- Add 5 different types of material finishes to the golf ball using another texture map (1 mark for each material finish).
- Adding a second, which is not an ambient light (2 marks).
- Adding edges (with glLines) to the glass such that each face can be easily distinguished (2 marks).
- Allow the colour of the floor to change as the floor moves behind the coloured golf ball (2 marks).
- Allowing the light to also affect the glass causes a spectral lighting effect on the glass (5 marks).
- Adding stained glass regions to the glass. For you to obtain your marks, you will need to add ray tracing so that the light that intersects the different regions affects the light's projection on the floor. For each new region you correctly add, you will get 2 marks.
- Allow the light to affect the golf ball's perception (2 marks).

---

## 6 Implementation Details

- You need to use OpenGL version 3.3 for this practical.
- You may not use any of the build-in mathematical libraries within the glm package. This included matrix arithmetic. Hint: You may use your practical 1 in this practical.
- You may not use any of the built-in OpenGL functions to generate the shapes for you. You need to create each shape from first principles.
- You may not use any of the built-in OpenGL functions to perform the transformations of the shapes. You need to transform each shape from first principles either explicitly or by using the matrix arithmetic techniques discussed in class.
- You may only use the following C++ and OpenGL libraries:
  - stdio.h
  - stdlib.h
  - iostream
  - iomanip
  - cmath
  - sstream
  - GL/glew.h
  - GLFW/glfw3.h
  - glm/glm.hpp
- You may also use the shader.hpp and glad.c files that assist with compiling and linking of shaders. Your code should be able to be compiled without the assistance of an IDE, i.e. the project needs to be able to be compiled from terminal.
- All your helper classes and files needs to be in the same directory of the main.cpp.
- Ensure that the title of the window of your program is your correct student number.
- Ensure you correctly name your texture maps to logical names such that they can be easily inspected offline.

---

## 7 Submission

You are required to submit on ClickUp under the appropriate submission link. In the archive that you submit, include a makefile and compiling instructions such that the program can be compiled and executed by the markers if needed. Failure to upload to ClickUP will result in you forfeiting all marks for this practical. No exceptions will be made on this matter.

---

## 8 Demo Instructions

1. You will first be required to download your submission from ClickUP.
2. You will then demo your practical to the tutor.
3. In the presence of the tutor, you will be required to upload the archive you downloaded from ClickUP to FitchFork. Failure to upload to FitchFork will result in you forfeiting all marks for this practical. No exceptions will be made on this matter.
