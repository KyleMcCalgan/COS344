#ifndef INPUT_H
#define INPUT_H
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../shapes.h"

////////////////////////////////////////////////////////////////////////////////////////
//processInput - translate/rotate/scale all selected shapes based on held keys
//clamps position to the grass playing area bounds
////////////////////////////////////////////////////////////////////////////////////////
static void processInput(GLFWwindow* window, std::vector<RenderShape*>& sel,
                         float grassHW, float grassHH)
{
    if (sel.empty()) return;
    for (RenderShape* s : sel) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) s->ty += 0.003f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) s->ty -= 0.003f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) s->tx -= 0.003f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) s->tx += 0.003f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) s->rotation += 0.02f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) s->rotation -= 0.02f;
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) { s->sx *= 1.01f; s->sy *= 1.01f; }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) { s->sx /= 1.01f; s->sy /= 1.01f; }

        s->tx = fmaxf(-grassHW, fminf(grassHW, s->tx));
        s->ty = fmaxf(-grassHH, fminf(grassHH, s->ty));
    }
}

#endif // INPUT_H
