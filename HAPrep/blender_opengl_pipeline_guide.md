# Blender → Assimp → OpenGL 3.3 Pipeline Guide
**COS Graphics — WSL + VSCode Step-by-Step**

---

## Stage 1: Export Your Model from Blender

### 1.1 Prepare Your Model

Before exporting, do the following inside Blender:

1. Select your model object(s) in the 3D viewport.
2. Apply all transforms: press **Ctrl + A → All Transforms**. This bakes position, rotation, and scale so OpenGL sees them correctly.
3. Make sure your model has **UV maps** if you plan to texture it. In the UV Editor, confirm UVs are unwrapped.
4. Confirm your model faces are **triangulated or quad-based** — Assimp will auto-triangulate, but it's cleaner to do it yourself: select all → **Mesh → Faces → Triangulate Faces**.

### 1.2 Export as glTF Binary (.glb)

1. Go to **File → Export → glTF 2.0 (.glb/.gltf)**
2. In the export panel on the right side, configure:

| Setting | Value | Why |
|---|---|---|
| Format | **glTF Binary (.glb)** | Single file — no loose textures or .bin files |
| +Y Up | ✅ Checked | OpenGL uses Y-up; Blender uses Z-up |
| Apply Transforms | ✅ Checked | Prevents unexpected rotations/scale |
| Normals | ✅ Checked | Required for lighting |
| UVs | ✅ Checked | Required for texturing |

3. Name the file `model.glb` and save it somewhere accessible (e.g., your Desktop or Downloads).

---

## Stage 2: Set Up Your WSL Project in VSCode

### 2.1 Open WSL in VSCode

1. Open VSCode.
2. Press **Ctrl + Shift + P** → type `WSL: Connect to WSL` → press Enter.
3. VSCode will reload connected to your WSL Linux environment.

### 2.2 Create the Project Directory Structure

Open the WSL terminal in VSCode (**Ctrl + `**) and run:

```bash
mkdir -p ~/graphics_project/src
mkdir -p ~/graphics_project/res
cd ~/graphics_project
```

Your final layout should look like this:

```
graphics_project/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── model.h
│   └── model.cpp
└── res/
    └── model.glb
```

### 2.3 Copy Your .glb File into the Project

From your Windows filesystem, copy `model.glb` into the WSL project. Windows drives are mounted at `/mnt/c/`, `/mnt/d/`, etc.

```bash
# Example: file is on your Windows Desktop
cp /mnt/c/Users/<YourWindowsUsername>/Desktop/model.glb ~/graphics_project/res/
```

Verify it is there:

```bash
ls ~/graphics_project/res/
# Should print: model.glb
```

### 2.4 Install Dependencies

Run these once in your WSL terminal:

```bash
sudo apt update
sudo apt install -y libassimp-dev libglfw3-dev libglew-dev cmake build-essential
```

Verify Assimp installed:

```bash
dpkg -l | grep assimp
# Should show: libassimp-dev
```

---

## Stage 3: Write the Project Files

### 3.1 CMakeLists.txt

Create this file at `~/graphics_project/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(GraphicsProject)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(main
    src/main.cpp
    src/model.cpp
)

target_link_libraries(main
    OpenGL::GL
    assimp::assimp
    glfw
    GLEW::GLEW
)
```

### 3.2 model.h

Create `~/graphics_project/src/model.h`:

```cpp
#pragma once
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

// Interleaved vertex layout: position + normal + UV
struct Vertex {
    float x, y, z;     // position
    float nx, ny, nz;  // normal
    float u, v;        // UV coordinates
};

// GPU handles for one mesh
struct Mesh {
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
};

class Model {
public:
    std::vector<Mesh> meshes;

    bool load(const std::string& path);
    void draw() const;

private:
    void processMesh(aiMesh* mesh);
};
```

### 3.3 model.cpp

Create `~/graphics_project/src/model.cpp`:

```cpp
#include "model.h"
#include <iostream>

bool Model::load(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |   // Convert quads/polygons to triangles
        aiProcess_FlipUVs     |   // Flip UV origin for OpenGL
        aiProcess_GenNormals      // Generate normals if missing
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // Process every mesh in the scene
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        processMesh(scene->mMeshes[i]);
    }

    return true;
}

void Model::processMesh(aiMesh* mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // --- Build interleaved vertex buffer ---
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;

        // Position
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;

        // Normal
        v.nx = mesh->mNormals[i].x;
        v.ny = mesh->mNormals[i].y;
        v.nz = mesh->mNormals[i].z;

        // UV (channel 0; default 0 if no UVs)
        if (mesh->mTextureCoords[0]) {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        } else {
            v.u = 0.0f;
            v.v = 0.0f;
        }

        vertices.push_back(v);
    }

    // --- Build index buffer ---
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // --- Upload to GPU ---
    Mesh gpuMesh;
    gpuMesh.indexCount = (unsigned int)indices.size();

    glGenVertexArrays(1, &gpuMesh.VAO);
    glGenBuffers(1, &gpuMesh.VBO);
    glGenBuffers(1, &gpuMesh.EBO);

    glBindVertexArray(gpuMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // layout 0: position (offset 0 bytes)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // layout 1: normals (offset 12 bytes = 3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // layout 2: UVs (offset 24 bytes = 6 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    meshes.push_back(gpuMesh);
}

void Model::draw() const {
    for (const Mesh& mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
```

### 3.4 main.cpp

Create `~/graphics_project/src/main.cpp`:

```cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "model.h"

// ---------------------------------------------------------------
// Minimal shaders: renders the model with a flat diffuse colour
// ---------------------------------------------------------------
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;

uniform mat4 uMVP;

out vec3 fragNormal;
out vec2 fragUV;

void main() {
    gl_Position = uMVP * vec4(in_Position, 1.0);
    fragNormal  = in_Normal;
    fragUV      = in_UV;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 fragNormal;
in vec2 fragUV;

out vec4 FragColor;

void main() {
    // Simple diffuse shading using the normal as colour
    vec3 colour = normalize(fragNormal) * 0.5 + 0.5;
    FragColor = vec4(colour, 1.0);
}
)";

// ---------------------------------------------------------------
// Shader compilation helper
// ---------------------------------------------------------------
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return shader;
}

int main() {
    // --- Init GLFW ---
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Model Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // --- Init GLEW ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW" << std::endl;
        return -1;
    }

    // --- OpenGL state ---
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    // --- Build shader program ---
    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertexShaderSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
    glDeleteShader(vert);
    glDeleteShader(frag);

    // --- Load model ---
    Model model;
    if (!model.load("../res/model.glb")) {
        std::cerr << "Failed to load model" << std::endl;
        return -1;
    }
    std::cout << "Model loaded: " << model.meshes.size() << " mesh(es)" << std::endl;

    // --- Render loop ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Build MVP matrix
        glm::mat4 model_m = glm::mat4(1.0f);
        glm::mat4 view    = glm::lookAt(
            glm::vec3(0.0f, 1.5f, 3.0f),  // camera position
            glm::vec3(0.0f, 0.0f, 0.0f),  // look at origin
            glm::vec3(0.0f, 1.0f, 0.0f)   // up vector
        );
        glm::mat4 proj    = glm::perspective(
            glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f
        );
        glm::mat4 MVP = proj * view * model_m;

        // Upload MVP uniform
        GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        // Draw
        model.draw();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

> **Note:** This requires GLM (OpenGL Mathematics). Install it: `sudo apt install libglm-dev`
> Then add to `CMakeLists.txt` under `target_link_libraries`: no extra link needed, GLM is header-only. Just ensure the include path is found. CMake will find it automatically after install.

---

## Stage 4: Build and Run

### 4.1 Build the Project

From your WSL terminal inside the project root:

```bash
cd ~/graphics_project
mkdir build && cd build
cmake ..
make
```

Watch for errors. Common ones:

| Error | Fix |
|---|---|
| `Could not find assimp` | Run `sudo apt install libassimp-dev` |
| `Could not find glfw3` | Run `sudo apt install libglfw3-dev` |
| `Could not find GLEW` | Run `sudo apt install libglew-dev` |
| `glm/glm.hpp not found` | Run `sudo apt install libglm-dev` |

### 4.2 Run the Executable

**You must run from inside the `build/` directory** so that `../res/model.glb` resolves correctly:

```bash
# Still inside build/
./main
```

You should see a window open with your model rendered using normal-based shading. If the terminal prints:

```
Model loaded: X mesh(es)
```

the pipeline is working.

### 4.3 Verify the Model Loaded Correctly

Check the terminal output:
- `Model loaded: 1 mesh(es)` — single-mesh model, all good.
- `Assimp error: ...` — the .glb file path is wrong or the export was malformed. Re-check step 2.3 and re-export from Blender.
- Window opens but nothing visible — your model may be too large or too small for the camera. Adjust the `glm::lookAt` position in `main.cpp` or scale your model in Blender before export.

---

## Quick Reference Checklist

```
[ ] Blender: Applied all transforms (Ctrl + A)
[ ] Blender: Exported as .glb with +Y Up and Apply Transforms checked
[ ] WSL: res/ directory exists at ~/graphics_project/res/
[ ] WSL: model.glb copied into res/
[ ] WSL: All libraries installed (libassimp-dev, libglfw3-dev, libglew-dev, libglm-dev)
[ ] WSL: All 4 files created (CMakeLists.txt, main.cpp, model.h, model.cpp)
[ ] WSL: cmake .. completed without errors
[ ] WSL: make completed without errors
[ ] WSL: ./main run from inside build/ directory
```

---

## Common Issues

**"No window appears / crashes immediately on WSL"**
WSL2 requires a display server for OpenGL windows. If you're on Windows, ensure you have **WSLg** enabled (Windows 11) or install **VcXsrv** / **X410** (Windows 10) and set `export DISPLAY=:0` in your terminal before running.

**"Model is invisible but no Assimp error"**
Your model's scale in Blender is likely huge or tiny. In Blender, before export, scale the model so it fits roughly within a 1–2 unit bounding box, apply transforms, then re-export.

**"Faces appear inside-out / model looks hollow"**
In Blender, select the mesh → Edit Mode → select all → **Mesh → Normals → Recalculate Outside**, then re-export.
