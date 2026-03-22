#ifndef WINDOW_H
#define WINDOW_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

////////////////////////////////////////////////////////////////////////////////////////
//getError - retrieve the last GLFW error string
////////////////////////////////////////////////////////////////////////////////////////
static const char* getError() {
    const char* msg; glfwGetError(&msg); return msg;
}

////////////////////////////////////////////////////////////////////////////////////////
//setUp - initialise GLFW + GLEW and create the 800x800 window
////////////////////////////////////////////////////////////////////////////////////////
static GLFWwindow* setUp() {
    glewExperimental = true;
    if (!glfwInit()) throw getError();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "u24648826", NULL, NULL);
    if (!window) { glfwTerminate(); throw "Failed to open GLFW window."; }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) { glfwTerminate(); throw getError(); }

    return window;
}

#endif // WINDOW_H
