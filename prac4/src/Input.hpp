#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>

enum ColorSelectionTarget
{
    COLOR_TARGET_FLOOR = 0,
    COLOR_TARGET_BALL = 1,
    COLOR_TARGET_LIGHT = 2
};

struct Vec3f
{
    float x;
    float y;
    float z;
};

struct InputState
{
    int sphereSubdivision = 12;
    int planeResolution = 8;
    int selectedColorTarget = COLOR_TARGET_FLOOR;
    int floorColorIndex = 3;
    int ballColorIndex = 3;
    int lightColorIndex = 0;
    float ballAlpha = 0.5f;
    bool colourTextureEnabled = false;
    bool displacementTextureEnabled = false;
    bool alphaTextureEnabled = false;
    bool wireframeMode = false;
    float sceneRotXDegrees = 0.0f;
    float sceneRotYDegrees = 0.0f;
    float sceneRotZDegrees = 0.0f;
    Vec3f lightLocalPos = {0.0f, 0.0f, 0.0f};
    bool sphereDirty = true;
    bool planeDirty = true;
};

InputState createDefaultInputState();
const char *getSelectedColorTargetLabel(const InputState &input);
void registerInput(GLFWwindow *window, InputState &state);

#endif
