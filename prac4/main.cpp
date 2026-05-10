#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "math3d.hpp"
#include "shader.hpp"

namespace
{
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const char *WINDOW_TITLE = "u24681012";

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

void fillMatrixUniform(GLuint programId, const char *uniformName, const Mat4 &matrix)
{
    float matrixValues[16];
    mat4ToArray(matrix, matrixValues);

    GLint location = glGetUniformLocation(programId, uniformName);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrixValues);
}
}

int main()
{
    GLFWwindow *window = NULL;
    GLuint vertexArrayId = 0;
    GLuint vertexBufferId = 0;
    GLuint shaderProgramId = 0;

    try
    {
        window = setUpWindow();

        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);

        const GLfloat triangleVertices[] = {
             0.0f,  0.6f,  0.0f, 1.0f, 0.2f, 0.2f,
            -0.6f, -0.6f,  0.0f, 0.2f, 1.0f, 0.3f,
             0.6f, -0.6f,  0.0f, 0.2f, 0.4f, 1.0f
        };

        glGenBuffers(1, &vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

        shaderProgramId = LoadShaders("vertex.glsl", "fragment.glsl");
        if (shaderProgramId == 0)
        {
            throw "Failed to load shaders";
        }

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              (void *)(3 * sizeof(GLfloat)));

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
               glfwWindowShouldClose(window) == 0)
        {
            glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(shaderProgramId);

            float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
            Mat4 model = rotateY(static_cast<float>(glfwGetTime()) * 0.6f);
            Mat4 view = lookAt(Vec3{0.0f, 0.0f, 2.4f}, Vec3{0.0f, 0.0f, 0.0f}, Vec3{0.0f, 1.0f, 0.0f});
            Mat4 projection = perspective(0.78539816339f, aspectRatio, 0.1f, 100.0f);
            Mat4 mvp = projection * view * model;

            fillMatrixUniform(shaderProgramId, "MVP", mvp);

            glBindVertexArray(vertexArrayId);
            glDrawArrays(GL_TRIANGLES, 0, 3);

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

    if (vertexBufferId != 0)
    {
        glDeleteBuffers(1, &vertexBufferId);
    }

    if (vertexArrayId != 0)
    {
        glDeleteVertexArrays(1, &vertexArrayId);
    }

    if (window != NULL)
    {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
    return 0;
}
