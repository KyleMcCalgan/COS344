#include "Scene.hpp"

#include <GL/glew.h>

#include "Geometry.h"
#include "Palette.hpp"
#include "math3d.hpp"

namespace
{
const float SPHERE_RADIUS = 1.0f;
const float PLANE_EXTENT = 4.5f;
const float LIGHT_MARKER_RADIUS = 0.12f;
const float LIGHT_CONSTANT = 1.0f;
const float LIGHT_LINEAR = 0.14f;
const float LIGHT_QUADRATIC = 0.20f;
const float AMBIENT_STRENGTH = 0.18f;
const float SPECULAR_STRENGTH = 0.30f;
const float SHININESS = 24.0f;
const float MAX_DISPLACEMENT = 0.03f;

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

void fillFloatUniform(GLuint programId, const char *uniformName, float value)
{
    GLint location = glGetUniformLocation(programId, uniformName);
    glUniform1f(location, value);
}

void fillBoolUniform(GLuint programId, const char *uniformName, bool value)
{
    GLint location = glGetUniformLocation(programId, uniformName);
    glUniform1i(location, value ? GL_TRUE : GL_FALSE);
}

void fillIntUniform(GLuint programId, const char *uniformName, int value)
{
    GLint location = glGetUniformLocation(programId, uniformName);
    glUniform1i(location, value);
}

Vec3f transformPoint(const Mat4 &matrix, const Vec3f &point)
{
    Vec4 localPoint = {point.x, point.y, point.z, 1.0f};
    Matrix<4, 1> transformedMatrix = matrix * static_cast<Matrix<4, 1> >(localPoint);
    Vec4 transformedPoint(transformedMatrix);

    Vec3f result;
    result.x = transformedPoint[0];
    result.y = transformedPoint[1];
    result.z = transformedPoint[2];
    return result;
}

Color3f multiplyColors(const Color3f &a, const Color3f &b)
{
    Color3f result;
    result.r = a.r * b.r;
    result.g = a.g * b.g;
    result.b = a.b * b.b;
    return result;
}

Mat4 buildSceneModelMatrix(const InputState &input)
{
    return rotateZ(degreesToRadians(input.sceneRotZDegrees)) *
           rotateY(degreesToRadians(input.sceneRotYDegrees)) *
           rotateX(degreesToRadians(input.sceneRotXDegrees));
}
}

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::build(const InputState &input)
{
    rebuildSphere(input.sphereSubdivision);
    rebuildPlane(input.planeResolution);
    rebuildLightMarker();
    m_textures.loadAll();
}

void Scene::syncGeometry(InputState &input)
{
    if (input.sphereDirty)
    {
        rebuildSphere(input.sphereSubdivision);
        input.sphereDirty = false;
    }

    if (input.planeDirty)
    {
        rebuildPlane(input.planeResolution);
        input.planeDirty = false;
    }
}

void Scene::draw(GLuint shaderProgram, const InputState &input, int framebufferWidth,
                 int framebufferHeight) const
{
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    float aspectRatio = static_cast<float>(framebufferWidth) /
                        static_cast<float>((framebufferHeight == 0) ? 1 : framebufferHeight);
    Mat4 baseViewModel = rotateY(degreesToRadians(20.0f)) * rotateX(degreesToRadians(-18.0f));
    Mat4 model = baseViewModel * buildSceneModelMatrix(input);
    Mat4 view = lookAt(Vec3{0.0f, 1.6f, 4.6f}, Vec3{0.0f, -0.15f, 0.0f},
                       Vec3{0.0f, 1.0f, 0.0f});
    Mat4 projection = perspective(degreesToRadians(45.0f), aspectRatio, 0.1f, 100.0f);
    Mat4 planeMvp = projection * view * model;
    Color3f floorColor = getFloorColor(input);
    Color3f ballColor = getBallColor(input);
    Color3f lightColor = getLightColor(input);
    Color3f effectiveLightColor = multiplyColors(lightColor, ballColor);
    Vec3f cameraPosition = {0.0f, 1.6f, 4.6f};
    Vec3f lightWorldPos = transformPoint(model, input.lightLocalPos);

    if (input.wireframeMode)
    {
        fillMatrixUniform(shaderProgram, "MVP", planeMvp);
        fillMatrixUniform(shaderProgram, "Model", model);
        fillBoolUniform(shaderProgram, "showGrid", false);
        fillBoolUniform(shaderProgram, "applyLighting", false);
        fillBoolUniform(shaderProgram, "applyTextureMaps", false);
        fillBoolUniform(shaderProgram, "colourTexEnabled", false);
        fillBoolUniform(shaderProgram, "displacementTexEnabled", false);
        fillBoolUniform(shaderProgram, "alphaTexEnabled", false);
        fillFloatUniform(shaderProgram, "gridDensity", 1.0f);
        fillFloatUniform(shaderProgram, "objectAlpha", 1.0f);
        fillFloatUniform(shaderProgram, "maxDisplacement", 0.0f);
        fillVec3Uniform(shaderProgram, "objectColor", floorColor.r, floorColor.g, floorColor.b);
        m_planeMesh.drawWireframe();

        fillMatrixUniform(shaderProgram, "MVP", planeMvp);
        fillMatrixUniform(shaderProgram, "Model", model);
        fillVec3Uniform(shaderProgram, "objectColor", ballColor.r, ballColor.g, ballColor.b);
        m_sphereMesh.drawWireframe();
        return;
    }

    fillMatrixUniform(shaderProgram, "MVP", planeMvp);
    fillMatrixUniform(shaderProgram, "Model", model);
    fillBoolUniform(shaderProgram, "showGrid", true);
    fillBoolUniform(shaderProgram, "applyLighting", true);
    fillBoolUniform(shaderProgram, "applyTextureMaps", false);
    fillBoolUniform(shaderProgram, "colourTexEnabled", false);
    fillBoolUniform(shaderProgram, "displacementTexEnabled", false);
    fillBoolUniform(shaderProgram, "alphaTexEnabled", false);
    fillFloatUniform(shaderProgram, "gridDensity", static_cast<float>(input.planeResolution));
    fillFloatUniform(shaderProgram, "objectAlpha", 1.0f);
    fillFloatUniform(shaderProgram, "maxDisplacement", MAX_DISPLACEMENT);
    fillVec3Uniform(shaderProgram, "lightPos", lightWorldPos.x, lightWorldPos.y, lightWorldPos.z);
    fillVec3Uniform(shaderProgram, "effectiveLightColor",
                    effectiveLightColor.r, effectiveLightColor.g, effectiveLightColor.b);
    fillVec3Uniform(shaderProgram, "cameraPos",
                    cameraPosition.x, cameraPosition.y, cameraPosition.z);
    fillFloatUniform(shaderProgram, "lightConstant", LIGHT_CONSTANT);
    fillFloatUniform(shaderProgram, "lightLinear", LIGHT_LINEAR);
    fillFloatUniform(shaderProgram, "lightQuadratic", LIGHT_QUADRATIC);
    fillFloatUniform(shaderProgram, "ambientStrength", AMBIENT_STRENGTH);
    fillFloatUniform(shaderProgram, "specularStrength", SPECULAR_STRENGTH);
    fillFloatUniform(shaderProgram, "shininess", SHININESS);
    fillVec3Uniform(shaderProgram, "objectColor", floorColor.r, floorColor.g, floorColor.b);
    m_planeMesh.drawSolid();

    Mat4 lightMarkerModel =
        model * translate(input.lightLocalPos.x, input.lightLocalPos.y, input.lightLocalPos.z);
    Mat4 lightMarkerMvp = projection * view * lightMarkerModel;
    fillMatrixUniform(shaderProgram, "MVP", lightMarkerMvp);
    fillMatrixUniform(shaderProgram, "Model", lightMarkerModel);
    fillBoolUniform(shaderProgram, "showGrid", false);
    fillBoolUniform(shaderProgram, "applyLighting", false);
    fillBoolUniform(shaderProgram, "applyTextureMaps", false);
    fillBoolUniform(shaderProgram, "colourTexEnabled", false);
    fillBoolUniform(shaderProgram, "displacementTexEnabled", false);
    fillBoolUniform(shaderProgram, "alphaTexEnabled", false);
    fillFloatUniform(shaderProgram, "gridDensity", 1.0f);
    fillFloatUniform(shaderProgram, "objectAlpha", 1.0f);
    fillFloatUniform(shaderProgram, "maxDisplacement", 0.0f);
    fillVec3Uniform(shaderProgram, "objectColor", lightColor.r, lightColor.g, lightColor.b);
    m_lightMarkerMesh.drawSolid();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    m_textures.bindAll(shaderProgram);
    fillMatrixUniform(shaderProgram, "MVP", planeMvp);
    fillMatrixUniform(shaderProgram, "Model", model);
    fillBoolUniform(shaderProgram, "showGrid", false);
    fillBoolUniform(shaderProgram, "applyLighting", false);
    fillBoolUniform(shaderProgram, "applyTextureMaps", true);
    fillBoolUniform(shaderProgram, "colourTexEnabled", input.colourTextureEnabled);
    fillBoolUniform(shaderProgram, "displacementTexEnabled", input.displacementTextureEnabled);
    fillBoolUniform(shaderProgram, "alphaTexEnabled", input.alphaTextureEnabled);
    fillFloatUniform(shaderProgram, "gridDensity", 1.0f);
    fillFloatUniform(shaderProgram, "objectAlpha", input.ballAlpha);
    fillFloatUniform(shaderProgram, "maxDisplacement", MAX_DISPLACEMENT);
    fillIntUniform(shaderProgram, "colourMap", 0);
    fillIntUniform(shaderProgram, "displacementMap", 1);
    fillIntUniform(shaderProgram, "alphaMap", 2);
    fillVec3Uniform(shaderProgram, "objectColor", ballColor.r, ballColor.g, ballColor.b);
    m_sphereMesh.drawSolid();
    glDisable(GL_CULL_FACE);
}

void Scene::rebuildSphere(int subdivisionLevel)
{
    MeshData sphereData = generateSphereMesh(subdivisionLevel, SPHERE_RADIUS);
    m_sphereMesh.upload(sphereData);
    destroyMeshData(sphereData);
}

void Scene::rebuildPlane(int resolution)
{
    MeshData planeData = generatePlaneMesh(resolution, -SPHERE_RADIUS, PLANE_EXTENT);
    m_planeMesh.upload(planeData);
    destroyMeshData(planeData);
}

void Scene::rebuildLightMarker()
{
    MeshData markerData = generateSphereMesh(8, LIGHT_MARKER_RADIUS);
    m_lightMarkerMesh.upload(markerData);
    destroyMeshData(markerData);
}
