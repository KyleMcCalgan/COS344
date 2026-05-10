#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "model.h"

// ---------------------------------------------------------------
// Shaders
// ---------------------------------------------------------------
const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uMVP;
uniform mat4 uModel;

out vec3 fragNormal;
out vec3 fragPos;

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    fragPos    = vec3(uModel * vec4(aPos, 1.0));
    fragNormal = mat3(transpose(inverse(uModel))) * aNormal;
}
)";

const char* fragSrc = R"(
#version 330 core
in vec3 fragNormal;
in vec3 fragPos;

uniform vec3 uColor;
uniform vec3 uLightDir;

out vec4 FragColor;

void main() {
    vec3 n = normalize(fragNormal);
    float diff = max(dot(n, normalize(-uLightDir)), 0.0);
    vec3 ambient = uColor * 0.3;
    vec3 diffuse = uColor * diff * 0.7;
    FragColor = vec4(ambient + diffuse, 1.0);
}
)";

// ---------------------------------------------------------------
// Procedural geometry helpers
// ---------------------------------------------------------------
struct SimpleVertex { float x, y, z, nx, ny, nz; };

struct SimpleMesh {
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
};

SimpleMesh buildMesh(const std::vector<SimpleVertex>& verts,
                     const std::vector<unsigned int>& idx) {
    SimpleMesh m;
    m.indexCount = (unsigned int)idx.size();

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.EBO);

    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(SimpleVertex), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return m;
}

// Flat ground plane: half-size s, at y=0, normal pointing up
SimpleMesh makePlane(float s) {
    std::vector<SimpleVertex> v = {
        {-s, 0, -s,  0,1,0},
        { s, 0, -s,  0,1,0},
        { s, 0,  s,  0,1,0},
        {-s, 0,  s,  0,1,0},
    };
    std::vector<unsigned int> idx = {0,1,2, 0,2,3};
    return buildMesh(v, idx);
}

// Axis-aligned box centred at origin, half-extents hx,hy,hz
SimpleMesh makeBox(float hx, float hy, float hz) {
    std::vector<SimpleVertex> v = {
        // +Y face
        {-hx, hy,-hz,  0,1,0}, { hx, hy,-hz,  0,1,0},
        { hx, hy, hz,  0,1,0}, {-hx, hy, hz,  0,1,0},
        // -Y face
        {-hx,-hy,-hz,  0,-1,0}, { hx,-hy,-hz,  0,-1,0},
        { hx,-hy, hz,  0,-1,0}, {-hx,-hy, hz,  0,-1,0},
        // +X face
        { hx,-hy,-hz,  1,0,0}, { hx, hy,-hz,  1,0,0},
        { hx, hy, hz,  1,0,0}, { hx,-hy, hz,  1,0,0},
        // -X face
        {-hx,-hy,-hz, -1,0,0}, {-hx, hy,-hz, -1,0,0},
        {-hx, hy, hz, -1,0,0}, {-hx,-hy, hz, -1,0,0},
        // +Z face
        {-hx,-hy, hz,  0,0,1}, { hx,-hy, hz,  0,0,1},
        { hx, hy, hz,  0,0,1}, {-hx, hy, hz,  0,0,1},
        // -Z face
        {-hx,-hy,-hz,  0,0,-1}, { hx,-hy,-hz,  0,0,-1},
        { hx, hy,-hz,  0,0,-1}, {-hx, hy,-hz,  0,0,-1},
    };
    std::vector<unsigned int> idx;
    for (unsigned int f = 0; f < 6; f++) {
        unsigned int b = f * 4;
        idx.insert(idx.end(), {b,b+1,b+2, b,b+2,b+3});
    }
    return buildMesh(v, idx);
}

// ---------------------------------------------------------------
// Shader compile / link
// ---------------------------------------------------------------
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader error: " << log << std::endl;
    }
    return s;
}

GLuint buildProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

// ---------------------------------------------------------------
// Camera state
// ---------------------------------------------------------------
glm::vec3 camPos(0.0f, 1.5f, 5.0f);
float yaw   = -90.0f; // looking toward -Z
float pitch =   0.0f; // degrees, clamped to avoid flipping

glm::vec3 camForward() {
    float y = glm::radians(yaw);
    float p = glm::radians(pitch);
    return glm::normalize(glm::vec3(
        cos(p) * cos(y),
        sin(p),
        cos(p) * sin(y)
    ));
}

// ---------------------------------------------------------------
// main
// ---------------------------------------------------------------
int main() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "Basic Scene", nullptr, nullptr);
    if (!win) { std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);

    // No mouse input at all — disable cursor capture
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    GLuint prog = buildProgram();

    SimpleMesh plane = makePlane(10.0f);
    SimpleMesh box   = makeBox(0.25f, 0.25f, 0.25f);

    Model drone;
    if (!drone.load("../res/Drone.glb"))
        return -1;
    std::cout << "Drone loaded: " << drone.meshes.size() << " mesh(es)\n";

    // Auto-scale: fit drone inside a 1-unit box on load
    float droneScale = drone.autoScale(1.0f);
    std::cout << "Auto scale: " << droneScale << "\n";

    // Propeller blade mesh indices (2 blades per motor, 4 motors)
    // Identified from the centre positions printed on load
    const int bladeMeshes[8] = {5, 6,  // motor FL  (Cylinder)
                                 7, 8,  // motor BR  (Cylinder.003)
                                 9, 10, // motor FR  (Cylinder.002... wait — mapped below)
                                11,12}; // motor BL  (Cylinder.001)
    // Motor pivot centres in model space (from cylinder mesh centres)
    const glm::vec3 motorCentres[4] = {
        {-1.87823f, 0.584797f,  1.85346f},  // Cylinder   → blades 5,6
        { 2.96907f, 0.541226f, -1.93149f},  // Cylinder.003 → blades 7,8
        { 2.98794f, 0.536698f,  1.91531f},  // Cylinder.002 → blades 9,10
        {-1.91842f, 0.573252f, -1.87849f},  // Cylinder.001 → blades 11,12
    };

    bool  propsSpinning = false;
    float propAngle     = 0.0f;

    glm::vec3 lightDir(0.4f, -1.0f, 0.5f);

    float lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        float now  = (float)glfwGetTime();
        float dt   = now - lastTime;
        lastTime   = now;
        float speed = 3.0f * dt;

        // Arrow keys: look direction
        float turnSpeed = 90.0f * dt;
        if (glfwGetKey(win, GLFW_KEY_LEFT)  == GLFW_PRESS) yaw   -= turnSpeed;
        if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw   += turnSpeed;
        if (glfwGetKey(win, GLFW_KEY_UP)    == GLFW_PRESS) pitch += turnSpeed;
        if (glfwGetKey(win, GLFW_KEY_DOWN)  == GLFW_PRESS) pitch -= turnSpeed;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // WASD: move horizontally, Space/Shift: fly up/down
        glm::vec3 fwd      = camForward();
        glm::vec3 flatFwd  = glm::normalize(glm::vec3(fwd.x, 0.0f, fwd.z));
        glm::vec3 right    = glm::normalize(glm::cross(flatFwd, glm::vec3(0,1,0)));

        if (glfwGetKey(win, GLFW_KEY_W)            == GLFW_PRESS) camPos += flatFwd * speed;
        if (glfwGetKey(win, GLFW_KEY_S)            == GLFW_PRESS) camPos -= flatFwd * speed;
        if (glfwGetKey(win, GLFW_KEY_A)            == GLFW_PRESS) camPos -= right   * speed;
        if (glfwGetKey(win, GLFW_KEY_D)            == GLFW_PRESS) camPos += right   * speed;
        if (glfwGetKey(win, GLFW_KEY_SPACE)        == GLFW_PRESS) camPos.y += speed;
        if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) camPos.y -= speed;

        // +/- to scale drone
        if (glfwGetKey(win, GLFW_KEY_EQUAL) == GLFW_PRESS) droneScale *= (1.0f + 2.0f * dt);
        if (glfwGetKey(win, GLFW_KEY_MINUS) == GLFW_PRESS) droneScale *= (1.0f - 2.0f * dt);
        droneScale = glm::clamp(droneScale, 0.01f, 100.0f);

        // Enter toggles propeller spinning (single press)
        static bool enterWasDown = false;
        bool enterDown = glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS;
        if (enterDown && !enterWasDown) propsSpinning = !propsSpinning;
        enterWasDown = enterDown;

        if (propsSpinning)
            propAngle += 720.0f * dt; // 2 full rotations per second

        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, true);

        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);

        glm::vec3 target = camPos + camForward();
        glm::mat4 view = glm::lookAt(camPos, target, glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 200.0f);

        glUniform3fv(glGetUniformLocation(prog, "uLightDir"), 1, glm::value_ptr(lightDir));

        // --- Draw plane (grey) ---
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 MVP   = proj * view * model;
            glUniformMatrix4fv(glGetUniformLocation(prog, "uMVP"),   1, GL_FALSE, glm::value_ptr(MVP));
            glUniformMatrix4fv(glGetUniformLocation(prog, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(prog, "uColor"), 0.55f, 0.55f, 0.55f);

            glBindVertexArray(plane.VAO);
            glDrawElements(GL_TRIANGLES, plane.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // --- Draw box (blue) at world centre, sitting on the plane ---
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.25f, 0.0f));
            glm::mat4 MVP   = proj * view * model;
            glUniformMatrix4fv(glGetUniformLocation(prog, "uMVP"),   1, GL_FALSE, glm::value_ptr(MVP));
            glUniformMatrix4fv(glGetUniformLocation(prog, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(prog, "uColor"), 0.2f, 0.4f, 0.9f);

            glBindVertexArray(box.VAO);
            glDrawElements(GL_TRIANGLES, box.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // --- Draw drone (per-mesh so blades can spin) ---
        {
            glm::mat4 droneBase = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            droneBase = glm::scale(droneBase, glm::vec3(droneScale));

            glUniform3f(glGetUniformLocation(prog, "uColor"), 0.8f, 0.8f, 0.8f);

            for (int i = 0; i < (int)drone.meshes.size(); i++) {
                const Mesh& m = drone.meshes[i];

                // Check if this mesh is a blade
                int bladeGroup = -1;
                for (int b = 0; b < 8; b++) {
                    if (bladeMeshes[b] == i) { bladeGroup = b / 2; break; }
                }

                glm::mat4 meshMatrix = droneBase;

                if (bladeGroup >= 0) {
                    // Rotate around the motor's Y axis in model space:
                    // translate to pivot → rotate → translate back
                    glm::vec3 pivot = motorCentres[bladeGroup];
                    meshMatrix = meshMatrix
                        * glm::translate(glm::mat4(1.0f),  pivot)
                        * glm::rotate(glm::mat4(1.0f), glm::radians(propAngle), glm::vec3(0,1,0))
                        * glm::translate(glm::mat4(1.0f), -pivot);
                }

                glm::mat4 MVP = proj * view * meshMatrix;
                glUniformMatrix4fv(glGetUniformLocation(prog, "uMVP"),   1, GL_FALSE, glm::value_ptr(MVP));
                glUniformMatrix4fv(glGetUniformLocation(prog, "uModel"), 1, GL_FALSE, glm::value_ptr(meshMatrix));

                glBindVertexArray(m.VAO);
                glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
