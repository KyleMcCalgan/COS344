#include "Input.hpp"

namespace
{
const int MIN_SPHERE_SUBDIVISION = 3;
const int MAX_SPHERE_SUBDIVISION = 64;
const int MIN_PLANE_RESOLUTION = 1;
const int MAX_PLANE_RESOLUTION = 64;
const int FLOOR_COLOR_COUNT = 10;
const int BALL_COLOR_COUNT = 10;
const int LIGHT_COLOR_COUNT = 9;
const float ROTATION_STEP_DEGREES = 5.0f;
const float LIGHT_TRANSLATION_STEP = 0.12f;
const float ALPHA_STEP = 0.05f;
const float MIN_BALL_ALPHA = 0.05f;
const float MAX_BALL_ALPHA = 1.0f;
const double ENTER_DEBOUNCE_SECONDS = 0.20;

InputState *g_state = 0;
double g_lastEnterToggleTime = -1.0;

int wrapIndex(int index, int count)
{
    int wrapped = index % count;
    return (wrapped < 0) ? (wrapped + count) : wrapped;
}

float clampValue(float value, float minValue, float maxValue)
{
    if (value < minValue)
    {
        return minValue;
    }

    if (value > maxValue)
    {
        return maxValue;
    }

    return value;
}

void cycleSelectedColor(InputState &state, int direction)
{
    switch (state.selectedColorTarget)
    {
        case COLOR_TARGET_FLOOR:
            state.floorColorIndex = wrapIndex(state.floorColorIndex + direction, FLOOR_COLOR_COUNT);
            break;

        case COLOR_TARGET_BALL:
            state.ballColorIndex = wrapIndex(state.ballColorIndex + direction, BALL_COLOR_COUNT);
            break;

        case COLOR_TARGET_LIGHT:
            state.lightColorIndex = wrapIndex(state.lightColorIndex + direction, LIGHT_COLOR_COUNT);
            break;

        default:
            break;
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (g_state == 0)
    {
        return;
    }

    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }

    InputState &state = *g_state;

    switch (key)
    {
        case GLFW_KEY_I:
            if (state.sphereSubdivision < MAX_SPHERE_SUBDIVISION)
            {
                state.sphereSubdivision++;
                state.sphereDirty = true;
            }
            break;

        case GLFW_KEY_K:
            if (state.sphereSubdivision > MIN_SPHERE_SUBDIVISION)
            {
                state.sphereSubdivision--;
                state.sphereDirty = true;
            }
            break;

        case GLFW_KEY_O:
            if (state.planeResolution < MAX_PLANE_RESOLUTION)
            {
                state.planeResolution++;
                state.planeDirty = true;
            }
            break;

        case GLFW_KEY_L:
            if (state.planeResolution > MIN_PLANE_RESOLUTION)
            {
                state.planeResolution--;
                state.planeDirty = true;
            }
            break;

        case GLFW_KEY_W:
            state.sceneRotXDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_S:
            state.sceneRotXDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_A:
            state.sceneRotYDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_D:
            state.sceneRotYDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_E:
            state.sceneRotZDegrees += ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_Q:
            state.sceneRotZDegrees -= ROTATION_STEP_DEGREES;
            break;

        case GLFW_KEY_UP:
            state.lightLocalPos.y += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_DOWN:
            state.lightLocalPos.y -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_LEFT:
            state.lightLocalPos.x -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_RIGHT:
            state.lightLocalPos.x += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_PERIOD:
            state.lightLocalPos.z += LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_COMMA:
            state.lightLocalPos.z -= LIGHT_TRANSLATION_STEP;
            break;

        case GLFW_KEY_1:
            state.selectedColorTarget = COLOR_TARGET_FLOOR;
            break;

        case GLFW_KEY_2:
            state.selectedColorTarget = COLOR_TARGET_BALL;
            break;

        case GLFW_KEY_3:
            state.selectedColorTarget = COLOR_TARGET_LIGHT;
            break;

        case GLFW_KEY_LEFT_BRACKET:
            cycleSelectedColor(state, -1);
            break;

        case GLFW_KEY_RIGHT_BRACKET:
            cycleSelectedColor(state, 1);
            break;

        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            state.ballAlpha = clampValue(state.ballAlpha + ALPHA_STEP,
                                         MIN_BALL_ALPHA, MAX_BALL_ALPHA);
            break;

        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            state.ballAlpha = clampValue(state.ballAlpha - ALPHA_STEP,
                                         MIN_BALL_ALPHA, MAX_BALL_ALPHA);
            break;

        case GLFW_KEY_SPACE:
            state = createDefaultInputState();
            break;

        case GLFW_KEY_B:
            state.colourTextureEnabled = !state.colourTextureEnabled;
            break;

        case GLFW_KEY_N:
            state.displacementTextureEnabled = !state.displacementTextureEnabled;
            break;

        case GLFW_KEY_M:
            state.alphaTextureEnabled = !state.alphaTextureEnabled;
            break;

        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
        {
            const double now = glfwGetTime();
            if (now - g_lastEnterToggleTime > ENTER_DEBOUNCE_SECONDS)
            {
                state.wireframeMode = !state.wireframeMode;
                g_lastEnterToggleTime = now;
            }
            break;
        }

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;

        default:
            break;
    }
}
}

InputState createDefaultInputState()
{
    InputState state;
    return state;
}

const char *getSelectedColorTargetLabel(const InputState &input)
{
    switch (input.selectedColorTarget)
    {
        case COLOR_TARGET_FLOOR:
            return "FLOOR";

        case COLOR_TARGET_BALL:
            return "BALL";

        case COLOR_TARGET_LIGHT:
            return "LIGHT";

        default:
            return "UNKNOWN";
    }
}

void registerInput(GLFWwindow *window, InputState &state)
{
    g_state = &state;
    glfwSetKeyCallback(window, keyCallback);
}
