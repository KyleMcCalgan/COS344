#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "shapes.h"
#include "linefont.h"
#include "layout.h"

// ============================================================
//  Collision helpers
// ============================================================

// Push a circle out of an axis-aligned box. Returns true if overlap.
static bool circleVsAABB(float bx, float by, float br,
                          float rx, float ry, float rhw, float rhh,
                          float& pushX, float& pushY)
{
    float cx = fmaxf(rx - rhw, fminf(bx, rx + rhw));
    float cy = fmaxf(ry - rhh, fminf(by, ry + rhh));
    float dx = bx - cx, dy = by - cy;
    float d2 = dx*dx + dy*dy;
    if (d2 >= br*br) { pushX = pushY = 0; return false; }
    float d = sqrtf(d2);
    if (d < 1e-6f) { pushX = 0; pushY = br; return true; }
    float ov = br - d;
    pushX = dx/d * ov;
    pushY = dy/d * ov;
    return true;
}

// Push a circle out of another circle.
static void circleVsCircle(float& bx, float& by, float br,
                            float ox, float oy, float or_)
{
    float dx = bx - ox, dy = by - oy;
    float d2 = dx*dx + dy*dy;
    float minD = br + or_;
    if (d2 >= minD*minD || d2 < 1e-12f) return;
    float d = sqrtf(d2), ov = minD - d;
    bx += dx/d * ov;
    by += dy/d * ov;
}

// Push a circle out of a triangle defined by three vertices (in world space).
// Uses closest-point-on-each-edge and interior test.
static void circleVsTriangle(float& bx, float& by, float br,
                              float ax, float ay,
                              float bvx, float bvy,
                              float cvx, float cvy)
{
    // Helper: closest point on segment p0->p1 to point q
    auto closestOnSeg = [](float qx, float qy,
                           float p0x, float p0y, float p1x, float p1y,
                           float& cx, float& cy) {
        float ex = p1x-p0x, ey = p1y-p0y;
        float t = ((qx-p0x)*ex + (qy-p0y)*ey) / (ex*ex + ey*ey + 1e-12f);
        t = fmaxf(0.f, fminf(1.f, t));
        cx = p0x + t*ex; cy = p0y + t*ey;
    };

    // Find closest point on the triangle to ball centre
    float bestDist2 = 1e30f, bestCx = 0, bestCy = 0;
    float segs[3][4] = {
        {ax,ay,bvx,bvy}, {bvx,bvy,cvx,cvy}, {cvx,cvy,ax,ay}
    };
    for (auto& seg : segs) {
        float cx, cy;
        closestOnSeg(bx, by, seg[0],seg[1],seg[2],seg[3], cx, cy);
        float dx = bx-cx, dy = by-cy, d2 = dx*dx+dy*dy;
        if (d2 < bestDist2) { bestDist2 = d2; bestCx = cx; bestCy = cy; }
    }

    // Check if ball centre is inside the triangle (winding test)
    auto sign = [](float px,float py,float ax,float ay,float bx,float by){
        return (px-bx)*(ay-by) - (ax-bx)*(py-by);
    };
    bool inside = (sign(bx,by,ax,ay,bvx,bvy) <= 0) &&
                  (sign(bx,by,bvx,bvy,cvx,cvy) <= 0) &&
                  (sign(bx,by,cvx,cvy,ax,ay) <= 0);
    inside = inside || ((sign(bx,by,ax,ay,bvx,bvy) >= 0) &&
                        (sign(bx,by,bvx,bvy,cvx,cvy) >= 0) &&
                        (sign(bx,by,cvx,cvy,ax,ay) >= 0));

    if (!inside && bestDist2 >= br*br) return;  // no collision

    float dist = sqrtf(bestDist2);
    float dx = bx - bestCx, dy = by - bestCy;
    if (inside || dist < 1e-6f) {
        // Push out along the shortest edge normal — use closest edge
        // fallback: push straight up
        dx = 0; dy = 1; dist = 0;
    }
    float ov = br - dist;
    if (dist > 1e-6f) { bx += dx/dist * ov; by += dy/dist * ov; }
    else              { bx += 0; by += ov; }
}

// Move/scale/rotate all selected shapes based on held keys
void processInput(GLFWwindow* window, std::vector<RenderShape*>& sel) {
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
    }
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

int main(int argc, char* argv[]) {
    // Command-line import: ./main --import <file>
    std::string importFile = "";
    for (int i = 1; i < argc - 1; i++) {
        if (std::string(argv[i]) == "--import") {
            importFile = argv[i + 1];
        }
    }

    GLFWwindow* window = setUp();

    GLuint shaderID  = LoadShaders("vertex.glsl", "fragment.glsl");
    GLuint modelLoc  = glGetUniformLocation(shaderID, "model");
    GLuint projLoc   = glGetUniformLocation(shaderID, "projection");
    GLuint colourLoc = glGetUniformLocation(shaderID, "colour");

    // Identity projection — world space = NDC directly
    float projData[9];
    flattenMatrix(identity3(), projData);

    LineFont font;
    font.init(shaderID, colourLoc, modelLoc, projLoc);

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
    RenderShape obs1a, obs1b;   // orange triangles  (key 2 → obs1a)
    RenderShape obs2a, obs2b;   // purple rectangles (key 3 → obs2a)
    RenderShape hole, ball;
    // Ball soccer spots (5 small dark circles offset around centre)
    RenderShape spot1, spot2, spot3, spot4, spot5;

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
    float rampTilt  = 0.4f;              // radians lean toward each other
    float rampGap   = 0.1f;             // half-distance between the two ramps
    rampBottom.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
        {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f, -rampGap);
    rampBottom.rotation = M_PI / 2.0f + rampTilt;
    rampTop.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
        {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f,  rampGap);
    rampTop.rotation = M_PI / 2.0f - rampTilt;

    // -- Obstacle 1: orange triangles (between start and river) --
    glm::vec3 orangeCol(0.9f, 0.5f, 0.1f), orangePastel(1.0f, 0.85f, 0.7f);
    float obs1Base = 0.12f, obs1Height = 0.14f;
    obs1a.init(makeTriangle(obs1Base, obs1Height), makeTriangleWire(obs1Base, obs1Height),
        orangeCol, orangePastel, true, 2, -0.2f, -0.35f);
    obs1b.init(makeTriangle(obs1Base, obs1Height), makeTriangleWire(obs1Base, obs1Height),
        orangeCol, orangePastel, true, 2,  0.2f, -0.35f);

    // -- Obstacle 2: purple rectangles (between river and hole) --
    glm::vec3 purpleCol(0.5f, 0.1f, 0.8f), purplePastel(0.8f, 0.7f, 1.0f);
    float obs2W = 0.06f, obs2H = 0.22f;
    obs2a.init(makeRect(obs2W, obs2H), makeRectWire(obs2W, obs2H),
        purpleCol, purplePastel, true, 3, -0.25f, 0.35f);
    obs2b.init(makeRect(obs2W, obs2H), makeRectWire(obs2W, obs2H),
        purpleCol, purplePastel, true, 3,  0.25f, 0.35f);

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


    // -- Ball soccer spots: small dark circles offset from ball centre --
    glm::vec3 spotCol(0.05f, 0.05f, 0.05f);
    float spotR   = ballRadius * 0.28f;
    float spotDist = ballRadius * 0.52f;
    spot1.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
    spot2.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
    spot3.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
    spot4.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
    spot5.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);

    // Draw order: back to front
    vector<RenderShape*> scene = {
        &floor, &grass,
        &startPad,
        &obs1a, &obs1b,
        &river, &riverTriL, &riverTriR,
        &rampBottom, &rampTop,
        &obs2a, &obs2b,
        &borderN, &borderS, &borderE, &borderW,
        &hole,
        &ball, &spot1, &spot2, &spot3, &spot4, &spot5
    };

    // Multi-selection: ctrl+number adds/cycles, number alone sets single selection
    vector<RenderShape*> selectedShapes;
    int obs1Cycle = 0, obs2Cycle = 0;
    double lastKey1 = 0.0, lastKey2 = 0.0, lastKey3 = 0.0, lastKey4 = 0.0;

    bool gameWon       = false;
    bool wireframeMode = false;
    double lastToggle  = 0.0;

    // Bridge animation: open = tilted apart, closed = both at pi/2, edges meeting in a line
    bool   bridgeOpen       = true;
    double lastBridgeToggle = 0.0;

    // Apply imported layout if --import was given
    if (!importFile.empty()) {
        Layout layout;
        if (importLayout(importFile.c_str(), layout)) {
            applyLayout(layout, ball, hole, obs1a, obs1b, obs2a, obs2b, bridgeOpen);
            std::cout << "Layout imported from: " << importFile << "\n";
        } else {
            std::cerr << "Failed to import layout from: " << importFile << "\n";
        }
    }
    const float rotSpeed    = 0.5f;  // radians per second
    const float scaleSpeed  = 0.5f * (0.6f / rampTilt);  // scaled so both finish together

    const float openRotBottom  = M_PI / 2.0f + rampTilt;
    const float openRotTop     = M_PI / 2.0f - rampTilt;
    const float closedRot      = M_PI / 2.0f;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            if (now - lastToggle > 0.2) { wireframeMode = !wireframeMode; lastToggle = now; }
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (now - lastBridgeToggle > 0.3) { bridgeOpen = !bridgeOpen; lastBridgeToggle = now; }
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            // Reset ball to start
            ball.tx = ballX; ball.ty = ballY; ball.rotation = 0; ball.sx = 1; ball.sy = 1;
            // Reset obstacles to initial positions/transforms
            obs1a.tx = -0.2f; obs1a.ty = -0.35f; obs1a.rotation = 0; obs1a.sx = 1; obs1a.sy = 1;
            obs1b.tx =  0.2f; obs1b.ty = -0.35f; obs1b.rotation = 0; obs1b.sx = 1; obs1b.sy = 1;
            obs2a.tx = -0.25f; obs2a.ty = 0.35f; obs2a.rotation = 0; obs2a.sx = 1; obs2a.sy = 1;
            obs2b.tx =  0.25f; obs2b.ty = 0.35f; obs2b.rotation = 0; obs2b.sx = 1; obs2b.sy = 1;
            hole.tx = holeX; hole.ty = holeY; hole.rotation = 0; hole.sx = 1; hole.sy = 1;
            // Clear selection and win state
            for (RenderShape* r : selectedShapes) r->selected = false;
            selectedShapes.clear();
            gameWon = false;
            bridgeOpen = true;
            obs1Cycle = 0; obs2Cycle = 0;
        }

        // F5 — export current layout
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && now - lastToggle > 0.5) {
            if (exportLayout("layout.json", ball, hole, obs1a, obs1b, obs2a, obs2b, bridgeOpen))
                std::cout << "Layout exported to layout.json\n";
            else
                std::cerr << "Export failed\n";
            lastToggle = now;
        }

        {
            auto stepRot = [&](float cur, float target) -> float {
                float diff = target - cur;
                float s = rotSpeed * dt;
                if (fabsf(diff) <= s) return target;
                return cur + (diff > 0 ? s : -s);
            };
            auto stepScale = [&](float cur, float target) -> float {
                float diff = target - cur;
                float s = scaleSpeed * dt;
                if (fabsf(diff) <= s) return target;
                return cur + (diff > 0 ? s : -s);
            };
            float targetSx = bridgeOpen ? 1.0f : 1.6f;
            rampBottom.rotation = stepRot  (rampBottom.rotation, bridgeOpen ? openRotBottom : closedRot);
            rampTop.rotation    = stepRot  (rampTop.rotation,    bridgeOpen ? openRotTop    : closedRot);
            rampBottom.sx       = stepScale(rampBottom.sx,       targetSx);
            rampTop.sx          = stepScale(rampTop.sx,          targetSx);
            // Pin outer edges: bottom of rampBottom and top of rampTop stay fixed
            float outerEdge = rampGap + rampW * 0.5f;
            rampBottom.ty = -(outerEdge - rampW * rampBottom.sx * 0.5f);
            rampTop.ty    =  (outerEdge - rampW * rampTop.sx    * 0.5f);
        }

        // Selection helpers
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

        // Add shape to selection (or replace if ctrl not held)
        auto addToSel = [&](RenderShape* s) {
            if (!ctrl) {
                for (RenderShape* r : selectedShapes) r->selected = false;
                selectedShapes.clear();
            }
            // Toggle off if already selected
            for (int i = 0; i < (int)selectedShapes.size(); i++) {
                if (selectedShapes[i] == s) {
                    s->selected = false;
                    selectedShapes.erase(selectedShapes.begin() + i);
                    return;
                }
            }
            s->selected = true;
            selectedShapes.push_back(s);
        };

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            for (RenderShape* r : selectedShapes) r->selected = false;
            selectedShapes.clear();
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && now - lastKey1 > 0.2) {
            addToSel(&ball); lastKey1 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && now - lastKey2 > 0.2) {
            // Cycle between obs1a and obs1b each press of 2
            RenderShape* target = (obs1Cycle == 0) ? &obs1a : &obs1b;
            obs1Cycle = 1 - obs1Cycle;
            addToSel(target); lastKey2 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && now - lastKey3 > 0.2) {
            RenderShape* target = (obs2Cycle == 0) ? &obs2a : &obs2b;
            obs2Cycle = 1 - obs2Cycle;
            addToSel(target); lastKey3 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && now - lastKey4 > 0.2) {
            addToSel(&hole); lastKey4 = now;
        }

        if (!gameWon) processInput(window, selectedShapes);

        // ---- Collision detection (ball only) ----
        {
            const float gHW = grassW * 0.5f;
            const float gHH = grassH * 0.5f;

            // 1. Grass wall clamp
            ball.tx = fmaxf(-gHW + ballRadius, fminf(gHW - ballRadius, ball.tx));
            ball.ty = fmaxf(-gHH + ballRadius, fminf(gHH - ballRadius, ball.ty));

            // 2. River collision — exact triangle tests + centre gap rect
            //    World vertices derived from makeTriangle(bankBase=0.4, bankDepth=0.35)
            //    rotated and translated to match riverTriL / riverTriR.
            //    Left tri:  (-0.55,  0.2), (-0.55, -0.2), (-0.2, 0)
            //    Right tri: ( 0.55, -0.2), ( 0.55,  0.2), ( 0.2, 0)
            {
                const float tipX = bankDepth - grassHW;   // -0.2  (left tip x, right is +0.2)
                const float hb   = bankBase * 0.5f;        // 0.2

                // Left bank triangle
                circleVsTriangle(ball.tx, ball.ty, ballRadius,
                                 -grassHW,  hb,
                                 -grassHW, -hb,
                                  tipX,     0.0f);
                // Right bank triangle
                circleVsTriangle(ball.tx, ball.ty, ballRadius,
                                  grassHW, -hb,
                                  grassHW,  hb,
                                 -tipX,     0.0f);
                // Centre gap — only blocked when bridge is raised
                if (bridgeOpen) {
                    float px, py;
                    if (circleVsAABB(ball.tx, ball.ty, ballRadius,
                                      0.0f, riverY, -tipX, riverH * 0.5f, px, py)) {
                        ball.tx += px; ball.ty += py;
                    }
                }
            }

            // 3. Purple rect obstacles — use actual sx/sy in case player scaled them
            float r2hw = (obs2W * 0.5f), r2hh = (obs2H * 0.5f);
            for (RenderShape* obs : {&obs2a, &obs2b}) {
                float px, py;
                if (circleVsAABB(ball.tx, ball.ty, ballRadius,
                                  obs->tx, obs->ty,
                                  r2hw * obs->sx, r2hh * obs->sy,
                                  px, py)) {
                    ball.tx += px; ball.ty += py;
                }
            }

            // 4. Orange triangle obstacles — exact triangle collision
            //    Local vertices from makeTriangle(base=0.12, height=0.14):
            //    (-0.06, -0.0467), (0.06, -0.0467), (0, 0.0933)
            //    Rotated by shape's current rotation then translated to world position.
            auto obsTriCollide = [&](RenderShape& obs) {
                float ca = cosf(obs.rotation), sa = sinf(obs.rotation);
                auto rot = [&](float lx, float ly, float& wx, float& wy) {
                    wx = obs.tx + ca*lx - sa*ly;
                    wy = obs.ty + sa*lx + ca*ly;
                };
                float hb = obs1Base * 0.5f;
                float third = obs1Height / 3.0f;
                float ax, ay, bx, by, cx, cy;
                rot(-hb, -third, ax, ay);
                rot( hb, -third, bx, by);
                rot(0,  2*third, cx, cy);
                circleVsTriangle(ball.tx, ball.ty, ballRadius, ax, ay, bx, by, cx, cy);
            };
            obsTriCollide(obs1a);
            obsTriCollide(obs1b);

            // 5. Hole — win condition, no push-out
            float dx = ball.tx - hole.tx, dy = ball.ty - hole.ty;
            if (dx*dx + dy*dy <= holeRadius * holeRadius) {
                gameWon = true;
                ball.tx = hole.tx; ball.ty = hole.ty;  // snap ball to hole
            }
        }

        // Sync soccer spots to ball transform
        auto syncSpot = [](RenderShape& spot, const RenderShape& parent, float ox, float oy) {
            spot.tx = parent.tx + ox;
            spot.ty = parent.ty + oy;
            spot.sx = parent.sx;
            spot.sy = parent.sy;
        };
        syncSpot(spot1, ball,  0.0f,      spotDist);
        syncSpot(spot2, ball,  spotDist * 0.95f,  spotDist * 0.31f);
        syncSpot(spot3, ball,  spotDist * 0.59f, -spotDist * 0.81f);
        syncSpot(spot4, ball, -spotDist * 0.59f, -spotDist * 0.81f);
        syncSpot(spot5, ball, -spotDist * 0.95f,  spotDist * 0.31f);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderID);
        glUniformMatrix3fv(projLoc, 1, GL_TRUE, projData);

        for (RenderShape* s : scene)
            s->draw(modelLoc, colourLoc, wireframeMode);

        // Draw keybind help text on the left outside the play area
        // Play area left edge is at x ~ -0.6, so we start at x = -0.98
        glUseProgram(shaderID);
        float cw = 0.028f, ch = 0.036f;  // character cell size in NDC
        float tx = -0.99f, ty = 0.88f;
        float ls  = ch * 1.6f;           // line spacing
        float fg  = 0.85f;               // text brightness
        font.drawString("CONTROLS:", tx, ty,       cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("WASD  MOVE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("QE  ROTATE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("+-  SCALE",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("ENTER WIRE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("P  BRIDGE",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("R  RESTART",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("F5 EXPORT",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls * 1.4f;
        font.drawString("SELECT:",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("1  BALL",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("2  TRI",      tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("3  RECT",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("4  HOLE",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("0  DESEL",    tx, ty,    cw, ch, fg, fg, fg, projData);

        if (gameWon)
            font.drawString("YOU WIN", -0.25f, 0.0f, 0.07f, 0.09f, 1.0f, 0.9f, 0.2f, projData);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
