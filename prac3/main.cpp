#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "src/Input.hpp"
#include "src/Scene.hpp"

using namespace std;

static const char *getError()
{
    const char *msg;
    glfwGetError(&msg);
    return msg;
}

static GLFWwindow *createWindow()
{
    glewExperimental = true;
    if (!glfwInit()) throw getError();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "u24648826", NULL, NULL);
    if (!window) { glfwTerminate(); throw getError(); }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) { glfwTerminate(); throw getError(); }

    return window;
}

int main()
{
    GLFWwindow *window;
    try { window = createWindow(); }
    catch (const char *e) { cout << e << endl; return 1; }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // sky blue

    GLuint shader = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

    InputState input;
    registerInput(window, input);

    Scene scene;
    scene.build();

    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - prevTime);
        prevTime   = now;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        scene.draw(shader, input, dt);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
