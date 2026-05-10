#include "App.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "HUD.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "shader.hpp"

namespace
{
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const char *WINDOW_TITLE = "u24648826";

const char *getError()
{
    const char *errorDescription = "Unknown GLFW error";
    glfwGetError(&errorDescription);
    return errorDescription;
}

void startUpGLFW()
{
    glewExperimental = true;
    if (!glfwInit())
    {
        throw getError();
    }
}

void startUpGLEW()
{
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw "Failed to initialise GLEW";
    }
}

GLFWwindow *setUpWindow()
{
    startUpGLFW();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (window == NULL)
    {
        std::cout << getError() << std::endl;
        glfwTerminate();
        throw "Failed to create GLFW window";
    }

    glfwMakeContextCurrent(window);
    startUpGLEW();
    return window;
}

void configureOpenGLState()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

}

int runApplication()
{
    GLFWwindow *window = NULL;
    GLuint shaderProgramId = 0;
    InputState input = createDefaultInputState();
    Scene scene;
    HUD hud;

    try
    {
        window = setUpWindow();
        registerInput(window, input);

        shaderProgramId = LoadShaders("vertex.glsl", "fragment.glsl");
        if (shaderProgramId == 0)
        {
            throw "Failed to load shaders";
        }

        scene.build(input);
        hud.init();
        configureOpenGLState();

        while (glfwWindowShouldClose(window) == 0)
        {
            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            scene.syncGeometry(input);
            scene.draw(shaderProgramId, input, framebufferWidth, framebufferHeight);
            hud.draw(input, framebufferWidth, framebufferHeight);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (const char *error)
    {
        std::cout << error << std::endl;
    }

    if (shaderProgramId != 0)
    {
        glDeleteProgram(shaderProgramId);
    }

    if (window != NULL)
    {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
    return 0;
}
