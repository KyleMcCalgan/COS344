#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Geometry.h"
#include "Mesh.h"
#include "math3d.hpp"
#include "shader.hpp"

namespace
{
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const char *WINDOW_TITLE = "u24681012";
const float SPHERE_RADIUS = 1.0f;
const float PLANE_EXTENT = 4.5f;
const float LIGHT_MARKER_RADIUS = 0.12f;
const int MIN_SPHERE_SUBDIVISION = 3;
const int MAX_SPHERE_SUBDIVISION = 64;
const int MIN_PLANE_RESOLUTION = 1;
const int MAX_PLANE_RESOLUTION = 64;
const float ROTATION_STEP_DEGREES = 5.0f;
const float LIGHT_TRANSLATION_STEP = 0.12f;

struct Vec3f
{
    float x;
    float y;
    float z;
};

struct SceneState
{
    int defaultSphereSubdivision;
    int defaultPlaneResolution;
    int sphereSubdivision;
    int planeResolution;
    float sceneRotXDegrees;
    float sceneRotYDegrees;
    float sceneRotZDegrees;
    Vec3f lightLocalPos;
    Mesh sphereMesh;
    Mesh planeMesh;
    Mesh lightMarkerMesh;
};

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

float degreesToRadians(float degrees)
{
    return degrees * 3.14159265359f / 180.0f;
}

void fillMatrixUniform(GLuint programId, const char *uniformName, const Mat4 &matrix)
{
    float matrixValues[16];
    mat4ToArray(matrix, matrixValues);

    GLint location = glGetUniformLocation(programId, uniformName);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrixValues);
}

void fillVec3Uniform(GLuint programId, const char *uniformName, float x, float y, float z)
{
    GLint location = glGetUniformLocation(programId, uniformName);
    glUniform3f(location, x, y, z);
}

void rebuildSphereMesh(SceneState &sceneState)
{
    MeshData sphereData = generateSphereMesh(sceneState.sphereSubdivision, SPHERE_RADIUS);
    sceneState.sphereMesh.upload(sphereData);
    destroyMeshData(sphereData);
}

void rebuildPlaneMesh(SceneState &sceneState)
{
    MeshData planeData = generatePlaneMesh(sceneState.planeResolution, -SPHERE_RADIUS, PLANE_EXTENT);
    sceneState.planeMesh.upload(planeData);
    destroyMeshData(planeData);
}

void rebuildLightMarkerMesh(SceneState &sceneState)
{
    MeshData markerData = generateSphereMesh(8, LIGHT_MARKER_RADIUS);
    sceneState.lightMarkerMesh.upload(markerData);
    destroyMeshData(markerData);
}

void resetSceneState(SceneState &sceneState)
{
    sceneState.sphereSubdivision = sceneState.defaultSphereSubdivision;
    sceneState.planeResolution = sceneState.defaultPlaneResolution;
    sceneState.sceneRotXDegrees = 0.0f;
    sceneState.sceneRotYDegrees = 0.0f;
    sceneState.sceneRotZDegrees = 0.0f;
    sceneState.lightLocalPos.x = 0.0f;
    sceneState.lightLocalPos.y = 0.0f;
    sceneState.lightLocalPos.z = 0.0f;

    rebuildSphereMesh(sceneState);
    rebuildPlaneMesh(sceneState);
}

Mat4 buildSceneModelMatrix(const SceneState &sceneState)
{
    return rotateZ(degreesToRadians(sceneState.sceneRotZDegrees)) *
           rotateY(degreesToRadians(sceneState.sceneRotYDegrees)) *
           rotateX(degreesToRadians(sceneState.sceneRotXDegrees));
}

void handleKeyPress(SceneState &sceneState, int key)
{
    switch (key)
    {
        case GLFW_KEY_I:
            if (sceneState.sphereSubdivision < MAX_SPHERE_SUBDIVISION)
            {
                sceneState.sphereSubdivision++;
                rebuildSphereMesh(sceneState);
            }
            break;

        case GLFW_KEY_K:
            if (sceneState.sphereSubdivision > MIN_SPHERE_SUBDIVISION)
            {
                sceneState.sphereSubdivision--;
                rebuildSphereMesh(sceneState);
            }
            break;

        case GLFW_KEY_O:
            if (sceneState.planeResolution < MAX_PLANE_RESOLUTION)
            {
                sceneState.planeResolution++;
                rebuildPlaneMesh(sceneState);
            }
            break;

        case GLFW_KEY_L:
            if (sceneState.planeResolution > MIN_PLANE_RESOLUTION)
            {
                sceneState.planeResolution--;
                rebuildPlaneMesh(sceneState);
            }
            break;

        case GLFW_KEY_W:
            sceneState.sceneRotXDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_S:
            sceneState.sceneRotXDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_A:
            sceneState.sceneRotYDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_D:
            sceneState.sceneRotYDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_E:
            sceneState.sceneRotZDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_Q:
            sceneState.sceneRotZDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_UP:
            sceneState.lightLocalPos.y += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_DOWN:
            sceneState.lightLocalPos.y -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_LEFT:
            sceneState.lightLocalPos.x -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_RIGHT:
            sceneState.lightLocalPos.x += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_PERIOD:
            sceneState.lightLocalPos.z += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_COMMA:
            sceneState.lightLocalPos.z -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_SPACE:
            resetSceneState(sceneState);
            break;

        default:
            break;
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }

    SceneState *sceneState = static_cast<SceneState *>(glfwGetWindowUserPointer(window));
    if (sceneState == NULL)
    {
        return;
    }

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    handleKeyPress(*sceneState, key);
}
}

int main()
{
    GLFWwindow *window = NULL;
    GLuint shaderProgramId = 0;
    SceneState sceneState;
    sceneState.defaultSphereSubdivision = 12;
    sceneState.defaultPlaneResolution = 8;
    sceneState.sphereSubdivision = sceneState.defaultSphereSubdivision;
    sceneState.planeResolution = sceneState.defaultPlaneResolution;
    sceneState.sceneRotXDegrees = 0.0f;
    sceneState.sceneRotYDegrees = 0.0f;
    sceneState.sceneRotZDegrees = 0.0f;
    sceneState.lightLocalPos.x = 0.0f;
    sceneState.lightLocalPos.y = 0.0f;
    sceneState.lightLocalPos.z = 0.0f;

    try
    {
        window = setUpWindow();
        glfwSetWindowUserPointer(window, &sceneState);
        glfwSetKeyCallback(window, keyCallback);

        shaderProgramId = LoadShaders("vertex.glsl", "fragment.glsl");
        if (shaderProgramId == 0)
        {
            throw "Failed to load shaders";
        }

        rebuildSphereMesh(sceneState);
        rebuildPlaneMesh(sceneState);
        rebuildLightMarkerMesh(sceneState);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        while (glfwWindowShouldClose(window) == 0)
        {
            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            glViewport(0, 0, framebufferWidth, framebufferHeight);

            glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(shaderProgramId);

            float aspectRatio = static_cast<float>(framebufferWidth) /
                                static_cast<float>((framebufferHeight == 0) ? 1 : framebufferHeight);
            Mat4 baseViewModel = rotateY(degreesToRadians(20.0f)) * rotateX(degreesToRadians(-18.0f));
            Mat4 model = baseViewModel * buildSceneModelMatrix(sceneState);
            Mat4 view = lookAt(Vec3{0.0f, 1.6f, 4.6f}, Vec3{0.0f, -0.15f, 0.0f},
                               Vec3{0.0f, 1.0f, 0.0f});
            Mat4 projection = perspective(degreesToRadians(45.0f), aspectRatio, 0.1f, 100.0f);
            Mat4 planeMvp = projection * view * model;

            fillMatrixUniform(shaderProgramId, "MVP", planeMvp);
            glUniform1i(glGetUniformLocation(shaderProgramId, "showGrid"), GL_TRUE);
            glUniform1f(glGetUniformLocation(shaderProgramId, "gridDensity"),
                        static_cast<float>(sceneState.planeResolution));
            fillVec3Uniform(shaderProgramId, "objectColor", 0.78f, 0.80f, 0.84f);
            sceneState.planeMesh.draw();

            fillMatrixUniform(shaderProgramId, "MVP", planeMvp);
            glUniform1i(glGetUniformLocation(shaderProgramId, "showGrid"), GL_FALSE);
            glUniform1f(glGetUniformLocation(shaderProgramId, "gridDensity"), 1.0f);
            fillVec3Uniform(shaderProgramId, "objectColor", 0.85f, 0.87f, 0.90f);
            sceneState.sphereMesh.draw();

            Mat4 lightMarkerModel =
                model * translate(sceneState.lightLocalPos.x,
                                  sceneState.lightLocalPos.y,
                                  sceneState.lightLocalPos.z);
            Mat4 lightMarkerMvp = projection * view * lightMarkerModel;
            fillMatrixUniform(shaderProgramId, "MVP", lightMarkerMvp);
            fillVec3Uniform(shaderProgramId, "objectColor", 1.0f, 0.85f, 0.25f);
            sceneState.lightMarkerMesh.draw();


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
