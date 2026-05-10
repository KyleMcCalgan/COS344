#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <vector>
#include "Face.hpp"
#include "math3d.hpp"

static constexpr float WORLD_HALF_W = 5.0f;

enum class EditorMode {
    NORMAL,
    DRAWING,
    SELECTED,
    CUT_DRAWING,
    MOVING
};

struct OrbitCamera {
    float theta  = 0.5f;
    float phi    = 0.9f;
    float radius = 8.0f;

    Vec3 eye() const;
    Mat4 viewMatrix() const;
};

struct EditorState {
    EditorMode mode = EditorMode::NORMAL;

    std::vector<Face> faces;
    int selectedFaceId = -1;
    int nextId = 0;

    std::vector<Vec2> drawingVerts;
    Vec2 moveAnchor{0, 0};

    OrbitCamera camera;

    // Set by GLFW callbacks each frame
    double mouseX = 0, mouseY = 0;
    bool leftJustPressed  = false;
    bool rightJustPressed = false;
    bool cutKeyPressed    = false;
    bool moveKeyPressed   = false;
    bool escapePressed    = false;
    bool deletePressed    = false;
    bool wireframe        = false;

    // Orbit drag state (right viewport, right mouse button)
    bool   orbitDragging = false;
    double orbitLastX = 0, orbitLastY = 0;

    void processFrame(int winW, int winH);

    int  hitTestFace(Vec2 p);
    void performCut(int outerIdx, const Contour& cutContour);
    void moveFace(Face& f, Vec2 delta);
    void deselectAll();
    void assignColor(Face& f);
};

Vec2 screenToWorld2D(double sx, double sy, int winW, int winH);

#endif // EDITOR_HPP
