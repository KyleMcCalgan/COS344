#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "shapes.h"

// Move/scale/rotate the selected shape based on held keys
void processInput(GLFWwindow* window, RenderShape* selected) {
    if (!selected) return;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) selected->ty += 0.003f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) selected->ty -= 0.003f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) selected->tx -= 0.003f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) selected->tx += 0.003f;
}

using namespace std;

// ============================================================
//  Window setup
// ============================================================

const char* getError() {
    const char* msg; glfwGetError(&msg); return msg;
}

GLFWwindow* setUp() {
    glewExperimental = true;
    if (!glfwInit()) throw getError();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "u24648826", NULL, NULL);
    if (!window) { glfwTerminate(); throw "Failed to open GLFW window."; }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) { glfwTerminate(); throw getError(); }

    return window;
}

// ============================================================
//  Main
// ============================================================

int main() {
    GLFWwindow* window = setUp();

    GLuint shaderID  = LoadShaders("vertex.glsl", "fragment.glsl");
    GLuint modelLoc  = glGetUniformLocation(shaderID, "model");
    GLuint projLoc   = glGetUniformLocation(shaderID, "projection");
    GLuint colourLoc = glGetUniformLocation(shaderID, "colour");

    // Identity projection — world space = NDC directly
    float projData[9];
    flattenMatrix(identity3(), projData);

    // --------------------------------------------------------
    //  Scene — playing area: tall rectangle, river with triangle
    //          banks, maroon start pad, pink hole, white ball
    //
    //  Layout (NDC, y-up):
    //    Grass:  1.1 wide x 1.7 tall, centred at origin
    //    Start:  maroon pad near bottom  (~y = -0.65)
    //    River:  thin blue strip at y = 0,  triangles pinch left/right
    //    Hole:   pink circle near top        (~y =  0.65)
    //    Ball:   white low-poly circle on start pad
    // --------------------------------------------------------

    RenderShape floor, grass, borderN, borderS, borderE, borderW;
    RenderShape startPad;
    RenderShape river, riverTriL, riverTriR;
    RenderShape rampBottom, rampTop;
    RenderShape hole, ball;

    // -- Floor --
    float floorW = 1.2f, floorH = 1.8f;
    floor.init(makeRect(floorW, floorH), makeRectWire(floorW, floorH),
        {0.55f, 0.55f, 0.55f}, {0.55f, 0.55f, 0.55f}, false, 0);

    // -- Grass --
    float grassW = 1.1f, grassH = 1.7f;
    float grassHW = grassW * 0.5f, grassHH = grassH * 0.5f;
    grass.init(makeRect(grassW, grassH), makeRectWire(grassW, grassH),
        {0.2f, 0.6f, 0.2f}, {0.2f, 0.6f, 0.2f}, false, 0);

    // -- Borders --
    glm::vec3 borderColour(0.3f, 0.15f, 0.05f);
    float borderThickness = 0.05f;
    float borderNY =  grassHH + borderThickness * 0.5f;
    float borderSY = -grassHH - borderThickness * 0.5f;
    float borderEX =  grassHW + borderThickness * 0.5f;
    float borderWX = -grassHW - borderThickness * 0.5f;
    borderN.init(makeRect(grassW, borderThickness), makeRectWire(grassW, borderThickness), borderColour, borderColour, false, 0,  0.0f, borderNY);
    borderS.init(makeRect(grassW, borderThickness), makeRectWire(grassW, borderThickness), borderColour, borderColour, false, 0,  0.0f, borderSY);
    borderE.init(makeRect(borderThickness, grassH), makeRectWire(borderThickness, grassH), borderColour, borderColour, false, 0, borderEX, 0.0f);
    borderW.init(makeRect(borderThickness, grassH), makeRectWire(borderThickness, grassH), borderColour, borderColour, false, 0, borderWX, 0.0f);

    // -- Starting pad --
    float startPadW = 0.3f, startPadH = 0.12f;
    float startPadX = 0.0f, startPadY = -0.65f;
    startPad.init(makeRect(startPadW, startPadH), makeRectWire(startPadW, startPadH),
        {0.5f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, false, 0, startPadX, startPadY);

    // -- River --
    float riverH = 0.12f;
    float riverY = 0.0f;
    river.init(makeRect(grassW, riverH), makeRectWire(grassW, riverH),
        {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, 0.0f, riverY);

    // -- River bank triangles --
    // Triangles sit on top of the river, tips pointing inward, bases flush with the walls
    float bankBase  = 0.4f;   // vertical span of the flat side against the wall
    float bankDepth = 0.35f;  // how far the tip reaches inward from the wall
    float bankLX = -(grassHW - bankDepth / 3.0f);
    float bankRX =  (grassHW - bankDepth / 3.0f);
    riverTriL.init(makeTriangle(bankBase, bankDepth), makeTriangleWire(bankBase, bankDepth),
        {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, bankLX, riverY);
    riverTriL.rotation = -M_PI / 2.0f;
    riverTriR.init(makeTriangle(bankBase, bankDepth), makeTriangleWire(bankBase, bankDepth),
        {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, bankRX, riverY);
    riverTriR.rotation =  M_PI / 2.0f;

    // -- Drawbridge ramps --
    float rampW     = 0.13f, rampH = 0.08f;
    float rampTilt  = 0.07f;             // radians lean toward each other
    float rampGap   = 0.1f;             // half-distance between the two ramps
    rampBottom.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
        {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f, -rampGap);
    rampBottom.rotation = M_PI / 2.0f + rampTilt;
    rampTop.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
        {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f,  rampGap);
    rampTop.rotation = M_PI / 2.0f - rampTilt;

    // -- Hole --
    float holeRadius = 0.07f;
    float holeX = 0.0f, holeY = 0.65f;
    hole.init(makeCircle(holeRadius, 60), makeCircleWire(holeRadius, 60),
        {1.0f, 0.5f, 0.7f}, {1.0f, 0.8f, 0.9f}, true, 4, holeX, holeY);

    // -- Ball --
    float ballRadius = 0.04f;
    float ballX = 0.0f, ballY = -0.65f;
    ball.init(makeCircle(ballRadius, 8), makeCircleWire(ballRadius, 8),
        {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.7f}, true, 1, ballX, ballY);

    // Draw order: back to front
    vector<RenderShape*> scene = {
        &floor, &grass,
        &startPad,
        &river, &riverTriL, &riverTriR,
        &rampBottom, &rampTop,
        &borderN, &borderS, &borderE, &borderW,
        &hole, &ball
    };

    // Currently selected shape (key 1 = ball, key 4 = hole, 0 = none)
    RenderShape* selected = nullptr;

    bool wireframeMode = false;
    double lastToggle  = 0.0;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            double now = glfwGetTime();
            if (now - lastToggle > 0.2) { wireframeMode = !wireframeMode; lastToggle = now; }
        }

        // Selection keys — deselect old, select new
        auto select = [&](RenderShape* next) {
            if (selected) selected->selected = false;
            selected = next;
            if (selected) selected->selected = true;
        };
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) select(nullptr);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) select(&ball);
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) select(&hole);

        processInput(window, selected);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderID);
        glUniformMatrix3fv(projLoc, 1, GL_TRUE, projData);

        for (RenderShape* s : scene)
            s->draw(modelLoc, colourLoc, wireframeMode);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
