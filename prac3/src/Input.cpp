#include "Input.hpp"

static const float ROT_STEP   = 0.05f;   // radians per key press
static const float TRANS_STEP = 0.05f;   // world units per key press
static const float SPEED_STEP = 0.5f;    // rad/s per +/- press

static const double ENTER_DEBOUNCE = 0.25; // seconds

// Store a pointer to the active InputState so the static callback can reach it.
static InputState* g_state = nullptr;
static double      g_lastEnterTime = -1.0;

static void keyCallback(GLFWwindow* window, int key, int /*sc*/, int action, int /*mod*/)
{
    if (!g_state) return;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    InputState& s = *g_state;

    switch (key)
    {
        // Rotations (cumulative)
        case GLFW_KEY_W: s.rotX += ROT_STEP;  break;
        case GLFW_KEY_S: s.rotX -= ROT_STEP;  break;
        case GLFW_KEY_A: s.rotY += ROT_STEP;  break;
        case GLFW_KEY_D: s.rotY -= ROT_STEP;  break;
        case GLFW_KEY_E: s.rotZ -= ROT_STEP;  break;
        case GLFW_KEY_Q: s.rotZ += ROT_STEP;  break;

        // Translations (cumulative)
        case GLFW_KEY_I: s.transY += TRANS_STEP; break;
        case GLFW_KEY_K: s.transY -= TRANS_STEP; break;
        case GLFW_KEY_L: s.transX += TRANS_STEP; break;
        case GLFW_KEY_J: s.transX -= TRANS_STEP; break;
        case GLFW_KEY_O: s.transZ += TRANS_STEP; break;
        case GLFW_KEY_U: s.transZ -= TRANS_STEP; break;

        // Rotor speed (+/-)
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            s.rotorSpeed += SPEED_STEP; break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            s.rotorSpeed -= SPEED_STEP;
            if (s.rotorSpeed < 0.0f) s.rotorSpeed = 0.0f;
            break;

        // Wireframe toggle (debounced)
        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
        {
            double now = glfwGetTime();
            if (now - g_lastEnterTime > ENTER_DEBOUNCE)
            {
                s.wireframe    = !s.wireframe;
                g_lastEnterTime = now;
            }
            break;
        }

        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE); break;

        default: break;
    }
}

void registerInput(GLFWwindow* window, InputState& state)
{
    g_state = &state;
    glfwSetKeyCallback(window, keyCallback);
}
