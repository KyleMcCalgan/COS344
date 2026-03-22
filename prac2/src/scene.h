#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../shapes.h"

////////////////////////////////////////////////////////////////////////////////////////
//Scene - all geometry dimensions, RenderShape objects, spawn helpers,
//and render-order groups for the mini-golf playing area
//
//Layout (NDC, y-up):
//  Grass:  1.1 wide x 1.7 tall, centred at origin
//  Start:  maroon pad near bottom  (~y = -0.65)
//  River:  thin blue strip at y = 0, triangles pinch left/right
//  Hole:   pink circle near top    (~y =  0.65)
//  Ball:   white low-poly circle on start pad
////////////////////////////////////////////////////////////////////////////////////////
struct Scene {

////////////////////////////////////////////////////////////////////////////////////////
//dimensions - all measurements are in NDC space
////////////////////////////////////////////////////////////////////////////////////////
    const float floorW = 1.2f, floorH = 1.8f;
    const float grassW = 1.1f, grassH = 1.7f;
    const float grassHW = grassW * 0.5f, grassHH = grassH * 0.5f;

    const float borderThickness = 0.05f;
    const float borderNY =  grassHH + borderThickness * 0.5f;
    const float borderSY = -grassHH - borderThickness * 0.5f;
    const float borderEX =  grassHW + borderThickness * 0.5f;
    const float borderWX = -grassHW - borderThickness * 0.5f;

    const float riverH = 0.12f, riverY = 0.0f;
    const float bankBase = 0.4f, bankDepth = 0.35f;
    const float bankLX = -(grassHW - bankDepth / 3.0f);
    const float bankRX =  (grassHW - bankDepth / 3.0f);

    const float rampW = 0.13f, rampH = 0.08f;
    const float rampTilt = 0.4f;
    const float rampGap  = 0.1f;

    const float startPadW = 0.3f,  startPadH = 0.12f;
    const float startPadX = 0.0f,  startPadY = -0.65f;

    const float holeRadius = 0.07f;
    const float holeX = 0.0f, holeY = 0.65f;

    const float ballRadius = 0.04f;
    const float ballX = 0.0f, ballY = -0.65f;

    const float obs1Base = 0.12f, obs1Height = 0.14f;
    const float obs2W = 0.06f,    obs2H = 0.22f;
    const float spotR    = ballRadius * 0.28f;
    const float spotDist = ballRadius * 0.52f;

////////////////////////////////////////////////////////////////////////////////////////
//shape objects - static geometry plus dynamic obstacle pools
////////////////////////////////////////////////////////////////////////////////////////
    RenderShape floor, grass;
    RenderShape borderN, borderS, borderE, borderW;
    RenderShape startPad;
    RenderShape river, riverTriL, riverTriR;
    RenderShape rampBottom, rampTop;
    RenderShape hole, ball;
    RenderShape spot1, spot2, spot3, spot4, spot5;

    // Floating log stumps (grey hexagons in river)
    RenderShape log1, log2;
    const float logRadius = 0.07f;
    const float logBaseY  = riverY;        // centre of river
    const float log1StartX = -0.275f;
    const float log2StartX =  0.275f;

    // River-edge masks: charcoal rectangles that hide logs outside the grass area
    RenderShape riverMaskL, riverMaskR;
    const float maskW = 0.5f;   // wide enough to cover from grass edge to screen edge
    const float maskH = 0.3f;   // tall enough to fully cover a bobbing log

    std::vector<RenderShape*> triObs;   // orange triangle obstacles (dynamic)
    std::vector<RenderShape*> rectObs;  // purple rect obstacles     (dynamic)

////////////////////////////////////////////////////////////////////////////////////////
//render-order groups - obstacles inserted dynamically each frame via buildDrawList
////////////////////////////////////////////////////////////////////////////////////////
    std::vector<RenderShape*> sceneStatic;
    std::vector<RenderShape*> sceneRiver;
    std::vector<RenderShape*> sceneFg;

////////////////////////////////////////////////////////////////////////////////////////
//init - initialise all shapes and populate render groups
////////////////////////////////////////////////////////////////////////////////////////
    void init() {
        floor.init(makeRect(floorW, floorH), makeRectWire(floorW, floorH),
            {0.55f, 0.55f, 0.55f}, {0.55f, 0.55f, 0.55f}, false, 0);

        grass.init(makeRect(grassW, grassH), makeRectWire(grassW, grassH),
            {0.2f, 0.6f, 0.2f}, {0.2f, 0.6f, 0.2f}, false, 0);

        glm::vec3 borderCol(0.3f, 0.15f, 0.05f);
        borderN.init(makeRect(grassW, borderThickness), makeRectWire(grassW, borderThickness), borderCol, borderCol, false, 0,  0.0f, borderNY);
        borderS.init(makeRect(grassW, borderThickness), makeRectWire(grassW, borderThickness), borderCol, borderCol, false, 0,  0.0f, borderSY);
        borderE.init(makeRect(borderThickness, grassH), makeRectWire(borderThickness, grassH), borderCol, borderCol, false, 0, borderEX, 0.0f);
        borderW.init(makeRect(borderThickness, grassH), makeRectWire(borderThickness, grassH), borderCol, borderCol, false, 0, borderWX, 0.0f);

        startPad.init(makeRect(startPadW, startPadH), makeRectWire(startPadW, startPadH),
            {0.5f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, false, 0, startPadX, startPadY);

        river.init(makeRect(grassW, riverH), makeRectWire(grassW, riverH),
            {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, 0.0f, riverY);

        riverTriL.init(makeTriangle(bankBase, bankDepth), makeTriangleWire(bankBase, bankDepth),
            {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, bankLX, riverY);
        riverTriL.rotation = -M_PI / 2.0f;
        riverTriR.init(makeTriangle(bankBase, bankDepth), makeTriangleWire(bankBase, bankDepth),
            {0.2f, 0.4f, 0.9f}, {0.2f, 0.4f, 0.9f}, false, 0, bankRX, riverY);
        riverTriR.rotation =  M_PI / 2.0f;

        rampBottom.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
            {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f, -rampGap);
        rampBottom.rotation = M_PI / 2.0f + rampTilt;
        rampTop.init(makeRect(rampW, rampH), makeRectWire(rampW, rampH),
            {0.4f, 0.2f, 0.05f}, {0.4f, 0.2f, 0.05f}, false, 0, 0.0f,  rampGap);
        rampTop.rotation = M_PI / 2.0f - rampTilt;

        hole.init(makeCircle(holeRadius, 60), makeCircleWire(holeRadius, 60),
            {1.0f, 0.5f, 0.7f}, {1.0f, 0.8f, 0.9f}, true, 4, holeX, holeY);

        ball.init(makeCircle(ballRadius, 8), makeCircleWire(ballRadius, 8),
            {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.7f}, true, 1, ballX, ballY);

        glm::vec3 spotCol(0.05f, 0.05f, 0.05f);
        spot1.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
        spot2.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
        spot3.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
        spot4.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);
        spot5.init(makeCircle(spotR, 6), makeCircleWire(spotR, 6), spotCol, spotCol, false, 0);

        // Floating hexagonal log stumps (grey, non-selectable)
        glm::vec3 logCol(0.6f, 0.6f, 0.6f);
        log1.init(makeHexagon(logRadius), makeHexagonWire(logRadius),
                  logCol, logCol, false, 0, log1StartX, logBaseY);
        log2.init(makeHexagon(logRadius), makeHexagonWire(logRadius),
                  logCol, logCol, false, 0, log2StartX, logBaseY);

        // River-edge mask rectangles — same colour as background, hide logs outside grass
        glm::vec3 maskCol(0.15f, 0.15f, 0.15f);
        std::vector<float> maskVerts = makeRect(maskW, maskH);
        std::vector<float> maskWire;  // no wireframe needed
        float maskLX = -(grassHW + maskW * 0.5f);
        float maskRX =  (grassHW + maskW * 0.5f);
        riverMaskL.init(maskVerts, maskWire, maskCol, maskCol, false, 0, maskLX, riverY);
        riverMaskR.init(maskVerts, maskWire, maskCol, maskCol, false, 0, maskRX, riverY);

        spawnDefaultObstacles();

        sceneStatic = { &floor, &grass, &startPad };
        sceneRiver  = { &river, &riverTriL, &riverTriR,
                        &log1, &log2,
                        &rampBottom, &rampTop };
        sceneFg     = { &riverMaskL, &riverMaskR,
                        &borderN, &borderS, &borderE, &borderW,
                        &hole,
                        &ball, &spot1, &spot2, &spot3, &spot4, &spot5 };
    }

////////////////////////////////////////////////////////////////////////////////////////
//spawn helpers - allocate a new obstacle and add it to the appropriate pool
////////////////////////////////////////////////////////////////////////////////////////
    RenderShape* spawnTri(float x, float y) {
        glm::vec3 col(0.9f, 0.5f, 0.1f), pastel(1.0f, 0.85f, 0.7f);
        RenderShape* s = new RenderShape();
        s->init(makeTriangle(obs1Base, obs1Height), makeTriangleWire(obs1Base, obs1Height),
                col, pastel, true, 2, x, y);
        triObs.push_back(s);
        return s;
    }

    RenderShape* spawnRect(float x, float y) {
        glm::vec3 col(0.5f, 0.1f, 0.8f), pastel(0.8f, 0.7f, 1.0f);
        RenderShape* s = new RenderShape();
        s->init(makeRect(obs2W, obs2H), makeRectWire(obs2W, obs2H),
                col, pastel, true, 3, x, y);
        rectObs.push_back(s);
        return s;
    }

////////////////////////////////////////////////////////////////////////////////////////
//reset helpers - restore ball, hole, and obstacles to their default state
////////////////////////////////////////////////////////////////////////////////////////
    void resetBallAndHole() {
        ball.tx = ballX; ball.ty = ballY; ball.rotation = 0; ball.sx = 1; ball.sy = 1;
        hole.tx = holeX; hole.ty = holeY; hole.rotation = 0; hole.sx = 1; hole.sy = 1;
    }

    void clearObstacles() {
        for (RenderShape* s : triObs)  delete s;
        for (RenderShape* s : rectObs) delete s;
        triObs.clear();
        rectObs.clear();
    }

    void spawnDefaultObstacles() {
        spawnTri (-0.2f, -0.35f);
        spawnTri ( 0.2f, -0.35f);
        spawnRect(-0.25f, 0.35f);
        spawnRect( 0.25f, 0.35f);
    }

////////////////////////////////////////////////////////////////////////////////////////
//buildDrawList - assemble the full ordered draw list for this frame
//order: static bg -> tri obstacles -> river -> rect obstacles -> foreground
////////////////////////////////////////////////////////////////////////////////////////
    std::vector<RenderShape*> buildDrawList() const {
        std::vector<RenderShape*> list;
        list.insert(list.end(), sceneStatic.begin(), sceneStatic.end());
        for (RenderShape* s : triObs)  list.push_back(s);
        list.insert(list.end(), sceneRiver.begin(), sceneRiver.end());
        for (RenderShape* s : rectObs) list.push_back(s);
        list.insert(list.end(), sceneFg.begin(), sceneFg.end());
        return list;
    }
};

#endif // SCENE_H
