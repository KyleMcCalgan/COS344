#ifndef GAME_H
#define GAME_H
#include <vector>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../shapes.h"
#include "../linefont.h"
#include "../layout.h"
#include "scene.h"
#include "collision.h"
#include "input.h"

////////////////////////////////////////////////////////////////////////////////////////
//GameState - all runtime flags, timers, selection state, and per-frame update/render logic
////////////////////////////////////////////////////////////////////////////////////////
struct GameState {

////////////////////////////////////////////////////////////////////////////////////////
//runtime flags
////////////////////////////////////////////////////////////////////////////////////////
    bool gameWon       = false;
    bool wireframeMode = false;
    bool bridgeOpen    = true;

////////////////////////////////////////////////////////////////////////////////////////
//selection state - tracks which shapes are currently selected and cycling indices
////////////////////////////////////////////////////////////////////////////////////////
    std::vector<RenderShape*> selectedShapes;
    int obs1Cycle = 0, obs2Cycle = 0;

////////////////////////////////////////////////////////////////////////////////////////
//debounce timers - prevent keys from firing multiple times per press
////////////////////////////////////////////////////////////////////////////////////////
    double lastKey1 = 0, lastKey2 = 0, lastKey3 = 0, lastKey4 = 0;
    double lastSpawn = 0, lastDelete = 0, lastToggle = 0;
    double lastBridgeToggle = 0;
    float  logTime = 0.0f;          // accumulates real time for log animation

////////////////////////////////////////////////////////////////////////////////////////
//bridge animation constants - derived from scene rampTilt on startup
////////////////////////////////////////////////////////////////////////////////////////
    float rotSpeed, scaleSpeed;
    float openRotBottom, openRotTop, closedRot;

    void initBridgeConstants(float rampTilt) {
        rotSpeed      = 0.5f;
        scaleSpeed    = 0.5f * (0.6f / rampTilt);
        openRotBottom = M_PI / 2.0f + rampTilt;
        openRotTop    = M_PI / 2.0f - rampTilt;
        closedRot     = M_PI / 2.0f;
    }

////////////////////////////////////////////////////////////////////////////////////////
//addToSel - add a shape to the selection
//with Ctrl held, toggles it in/out of a multi-selection
////////////////////////////////////////////////////////////////////////////////////////
    void addToSel(GLFWwindow* window, RenderShape* s) {
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)  == GLFW_PRESS ||
                    glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        if (!ctrl) {
            for (RenderShape* r : selectedShapes) r->selected = false;
            selectedShapes.clear();
        }
        for (int i = 0; i < (int)selectedShapes.size(); i++) {
            if (selectedShapes[i] == s) {
                s->selected = false;
                selectedShapes.erase(selectedShapes.begin() + i);
                return;
            }
        }
        s->selected = true;
        selectedShapes.push_back(s);
    }

    void clearSelection() {
        for (RenderShape* r : selectedShapes) r->selected = false;
        selectedShapes.clear();
    }

////////////////////////////////////////////////////////////////////////////////////////
//handleKeys - process all keyboard input for the current frame
////////////////////////////////////////////////////////////////////////////////////////
    void handleKeys(GLFWwindow* window, Scene& sc, double now, float dt) {

        // Global toggles
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && now - lastToggle > 0.2) {
            wireframeMode = !wireframeMode;
            lastToggle = now;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && now - lastBridgeToggle > 0.3) {
            bridgeOpen = !bridgeOpen;
            lastBridgeToggle = now;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            sc.resetBallAndHole();
            sc.clearObstacles();
            sc.spawnDefaultObstacles();
            clearSelection();
            gameWon = false;
            bridgeOpen = true;
            obs1Cycle = 0; obs2Cycle = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && now - lastToggle > 0.5) {
            if (exportLayout("layout.json", sc.ball, sc.hole, sc.triObs, sc.rectObs, bridgeOpen))
                std::cout << "Layout exported to layout.json\n";
            else
                std::cerr << "Export failed\n";
            lastToggle = now;
        }

        // Selection keys
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            clearSelection();

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && now - lastKey1 > 0.2) {
            addToSel(window, &sc.ball);  lastKey1 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && now - lastKey2 > 0.2) {
            if (!sc.triObs.empty()) {
                obs1Cycle = obs1Cycle % (int)sc.triObs.size();
                addToSel(window, sc.triObs[obs1Cycle]);
                obs1Cycle = (obs1Cycle + 1) % (int)sc.triObs.size();
            }
            lastKey2 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && now - lastKey3 > 0.2) {
            if (!sc.rectObs.empty()) {
                obs2Cycle = obs2Cycle % (int)sc.rectObs.size();
                addToSel(window, sc.rectObs[obs2Cycle]);
                obs2Cycle = (obs2Cycle + 1) % (int)sc.rectObs.size();
            }
            lastKey3 = now;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && now - lastKey4 > 0.2) {
            addToSel(window, &sc.hole);  lastKey4 = now;
        }

        // Obstacle spawn / delete
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && now - lastSpawn > 0.3) {
            addToSel(window, sc.spawnTri(0.0f, 0.0f));
            lastSpawn = now;
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && now - lastSpawn > 0.3) {
            addToSel(window, sc.spawnRect(0.0f, 0.0f));
            lastSpawn = now;
        }
        if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && now - lastDelete > 0.3) {
            for (RenderShape* sel : selectedShapes) {
                sel->selected = false;
                for (int i = (int)sc.triObs.size()-1;  i >= 0; i--)
                    if (sc.triObs[i]  == sel) { delete sc.triObs[i];  sc.triObs.erase(sc.triObs.begin()+i); }
                for (int i = (int)sc.rectObs.size()-1; i >= 0; i--)
                    if (sc.rectObs[i] == sel) { delete sc.rectObs[i]; sc.rectObs.erase(sc.rectObs.begin()+i); }
            }
            selectedShapes.clear();
            obs1Cycle = 0; obs2Cycle = 0;
            lastDelete = now;
        }

        // Shape movement (blocked when game is won)
        if (!gameWon)
            processInput(window, selectedShapes, sc.grassHW, sc.grassHH);
    }

////////////////////////////////////////////////////////////////////////////////////////
//updateBridge - animate drawbridge ramps toward their open/closed target transforms
////////////////////////////////////////////////////////////////////////////////////////
    void updateBridge(Scene& sc, float dt) {
        auto stepRot = [&](float cur, float target) -> float {
            float diff = target - cur, s = rotSpeed * dt;
            return (fabsf(diff) <= s) ? target : cur + (diff > 0 ? s : -s);
        };
        auto stepScale = [&](float cur, float target) -> float {
            float diff = target - cur, s = scaleSpeed * dt;
            return (fabsf(diff) <= s) ? target : cur + (diff > 0 ? s : -s);
        };

        float targetSx = bridgeOpen ? 1.0f : 1.6f;
        sc.rampBottom.rotation = stepRot  (sc.rampBottom.rotation, bridgeOpen ? openRotBottom : closedRot);
        sc.rampTop.rotation    = stepRot  (sc.rampTop.rotation,    bridgeOpen ? openRotTop    : closedRot);
        sc.rampBottom.sx       = stepScale(sc.rampBottom.sx, targetSx);
        sc.rampTop.sx          = stepScale(sc.rampTop.sx,   targetSx);

        float outerEdge = sc.rampGap + sc.rampW * 0.5f;
        sc.rampBottom.ty = -(outerEdge - sc.rampW * sc.rampBottom.sx * 0.5f);
        sc.rampTop.ty    =  (outerEdge - sc.rampW * sc.rampTop.sx    * 0.5f);
    }

////////////////////////////////////////////////////////////////////////////////////////
//updateLogs - animate the two floating hexagonal log stumps in the river
//  - drift rightward and wrap around to the left edge of the grass
//  - bob vertically with a sine wave (phase-offset between the two logs)
////////////////////////////////////////////////////////////////////////////////////////
    void updateLogs(Scene& sc, float dt) {
        const float driftSpeed = 0.04f;          // NDC units per second
        const float bobAmp     = 0.006f;        // bob amplitude in NDC
        const float bobFreq    = 0.4f;          // bobs per second
        // wrap triggers once the log has fully cleared the right edge
        const float exitRight  =  sc.grassHW + sc.logRadius;
        const float spawnLeft  = -sc.grassHW - sc.logRadius;
        const float width      = exitRight - spawnLeft;

        logTime += dt;

        sc.log1.tx += driftSpeed * dt;
        if (sc.log1.tx > exitRight) sc.log1.tx -= width;
        sc.log1.ty = sc.logBaseY + bobAmp * sinf(bobFreq * 2.0f * M_PI * logTime);

        sc.log2.tx += driftSpeed * dt;
        if (sc.log2.tx > exitRight) sc.log2.tx -= width;
        sc.log2.ty = sc.logBaseY + bobAmp * sinf(bobFreq * 2.0f * M_PI * logTime + M_PI);
    }

////////////////////////////////////////////////////////////////////////////////////////
//updateCollision - run all collision responses for the ball each frame
////////////////////////////////////////////////////////////////////////////////////////
    void updateCollision(Scene& sc) {
        RenderShape& ball = sc.ball;

        // 1. Grass wall clamp
        ball.tx = fmaxf(-sc.grassHW + sc.ballRadius, fminf(sc.grassHW - sc.ballRadius, ball.tx));
        ball.ty = fmaxf(-sc.grassHH + sc.ballRadius, fminf(sc.grassHH - sc.ballRadius, ball.ty));

        // 2. River — triangle banks + centre gap (only when bridge is raised)
        {
            const float tipX = sc.bankDepth - sc.grassHW;
            const float hb   = sc.bankBase * 0.5f;

            circleVsTriangle(ball.tx, ball.ty, sc.ballRadius,
                             -sc.grassHW,  hb,  -sc.grassHW, -hb,  tipX,  0.0f);
            circleVsTriangle(ball.tx, ball.ty, sc.ballRadius,
                              sc.grassHW, -hb,   sc.grassHW,  hb, -tipX,  0.0f);

            // Ball can cross river if it is standing on a floating log
            auto onLog = [&](const RenderShape& log) {
                float dx = ball.tx - log.tx, dy = ball.ty - log.ty;
                float threshold = sc.ballRadius + sc.logRadius;
                return dx*dx + dy*dy < threshold * threshold;
            };
            bool ridingLog = onLog(sc.log1) || onLog(sc.log2);

            if (bridgeOpen && !ridingLog) {
                float px, py;
                if (circleVsAABB(ball.tx, ball.ty, sc.ballRadius,
                                 0.0f, sc.riverY, -tipX, sc.riverH * 0.5f, px, py))
                    ball.tx += px, ball.ty += py;
            }
        }

        // 3. Purple rect obstacles
        for (RenderShape* obs : sc.rectObs) {
            float px, py;
            if (circleVsAABB(ball.tx, ball.ty, sc.ballRadius,
                              obs->tx, obs->ty,
                              sc.obs2W * 0.5f * obs->sx, sc.obs2H * 0.5f * obs->sy,
                              px, py))
                ball.tx += px, ball.ty += py;
        }

        // 4. Orange triangle obstacles
        for (RenderShape* obs : sc.triObs) {
            float ca = cosf(obs->rotation), sa = sinf(obs->rotation);
            auto rot = [&](float lx, float ly, float& wx, float& wy) {
                wx = obs->tx + ca*lx - sa*ly;
                wy = obs->ty + sa*lx + ca*ly;
            };
            float hb = sc.obs1Base * 0.5f, third = sc.obs1Height / 3.0f;
            float ax, ay, bx, by, cx, cy;
            rot(-hb, -third, ax, ay);
            rot( hb, -third, bx, by);
            rot( 0,  2*third, cx, cy);
            circleVsTriangle(ball.tx, ball.ty, sc.ballRadius, ax, ay, bx, by, cx, cy);
        }

        // 5. Hole — win condition (snap ball to centre, no push-out)
        float dx = ball.tx - sc.hole.tx, dy = ball.ty - sc.hole.ty;
        if (dx*dx + dy*dy <= sc.holeRadius * sc.holeRadius) {
            gameWon = true;
            ball.tx = sc.hole.tx; ball.ty = sc.hole.ty;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////
//syncSpots - keep the five soccer-ball markings locked to the ball position each frame
////////////////////////////////////////////////////////////////////////////////////////
    void syncSpots(Scene& sc) {
        auto sync = [](RenderShape& spot, const RenderShape& parent, float ox, float oy) {
            spot.tx = parent.tx + ox;
            spot.ty = parent.ty + oy;
            spot.sx = parent.sx;
            spot.sy = parent.sy;
        };
        const float d = sc.spotDist;
        sync(sc.spot1, sc.ball,  0.0f,       d);
        sync(sc.spot2, sc.ball,  d * 0.95f,  d * 0.31f);
        sync(sc.spot3, sc.ball,  d * 0.59f, -d * 0.81f);
        sync(sc.spot4, sc.ball, -d * 0.59f, -d * 0.81f);
        sync(sc.spot5, sc.ball, -d * 0.95f,  d * 0.31f);
    }

////////////////////////////////////////////////////////////////////////////////////////
//drawHUD - render the keybind reference panel and win message
////////////////////////////////////////////////////////////////////////////////////////
    void drawHUD(LineFont& font, float* projData) {
        const float cw = 0.028f, ch = 0.036f;
        const float ls = ch * 1.6f;
        const float fg = 0.85f;
        float tx = -0.99f, ty = 0.88f;

        font.drawString("CONTROLS:", tx, ty,      cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("WASD  MOVE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("QE  ROTATE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("+-  SCALE",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("ENTER WIRE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("P  BRIDGE",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("R  RESTART",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("F5 EXPORT",   tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("T  NEW TRI",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("Y  NEW RECT", tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("DEL REMOVE",  tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls * 1.4f;
        font.drawString("SELECT:",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("1  BALL",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("2  TRI",      tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("3  RECT",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("4  HOLE",     tx, ty,    cw, ch, fg, fg, fg, projData); ty -= ls;
        font.drawString("0  DESEL",    tx, ty,    cw, ch, fg, fg, fg, projData);

        if (gameWon)
            font.drawString("YOU WIN", -0.25f, 0.0f, 0.07f, 0.09f, 1.0f, 0.9f, 0.2f, projData);
    }
};

#endif // GAME_H
