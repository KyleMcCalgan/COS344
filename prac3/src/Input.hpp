#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>

// All scene-level mutable state driven by keyboard input.
struct InputState
{
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
    float transX = 0.0f, transY = 0.0f, transZ = 0.0f;
    float rotorSpeed = 0.0f;   // radians per second
    bool  wireframe  = false;
};

// Register the GLFW key callback on window. state must remain valid for the
// lifetime of the window.
void registerInput(GLFWwindow* window, InputState& state);

#endif // INPUT_HPP
