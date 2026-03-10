#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "Matrix.h"

using namespace std;

// ============================================================
//  Matrix helpers (hand-built, uses P1 Matrix<3,3>)
// ============================================================

Matrix<3,3> identity3() {
    Matrix<3,3> m;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
    return m;
}

Matrix<3,3> translate2D(float tx, float ty) {
    Matrix<3,3> m = identity3();
    m[0][2] = tx;
    m[1][2] = ty;
    return m;
}

Matrix<3,3> rotate2D(float r) {
    Matrix<3,3> m = identity3();
    m[0][0] =  cosf(r); m[0][1] = -sinf(r);
    m[1][0] =  sinf(r); m[1][1] =  cosf(r);
    return m;
}

Matrix<3,3> scale2D(float sx, float sy) {
    Matrix<3,3> m = identity3();
    m[0][0] = sx;
    m[1][1] = sy;
    return m;
}

// Flatten row-major Matrix<3,3> to float[9] for OpenGL
void flattenMatrix(const Matrix<3,3>& m, float out[9]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            out[i*3+j] = m[i][j];
}

// ============================================================
//  Window setup
// ============================================================

const char *getError() {
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW() {
    glewExperimental = true;
    if (!glfwInit()) throw getError();
}

inline void startUpGLEW() {
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp() {
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "u24648826", NULL, NULL);
    if (window == NULL) {
        cout << getError() << endl;
        glfwTerminate();
        throw "Failed to open GLFW window.";
    }
    glfwMakeContextCurrent(window);
    startUpGLEW();
    return window;
}

// ============================================================
//  Main
// ============================================================

int main() {
    GLFWwindow *window;
    try {
        window = setUp();
    } catch (const char *e) {
        cout << e << endl;
        throw;
    }

    GLuint shaderID = LoadShaders("vertex.glsl", "fragment.glsl");

    // Uniform locations
    GLuint modelLoc  = glGetUniformLocation(shaderID, "model");
    GLuint projLoc   = glGetUniformLocation(shaderID, "projection");
    GLuint colourLoc = glGetUniformLocation(shaderID, "colour");

    // Upload identity projection once (world space = NDC)
    Matrix<3,3> proj = identity3();
    float projData[9];
    flattenMatrix(proj, projData);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Stage 1: just clear to charcoal background
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);
        glUniformMatrix3fv(projLoc, 1, GL_TRUE, projData);

        // (shapes will be drawn here in later stages)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
