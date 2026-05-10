#include "Editor.hpp"
#include "Tessellator.hpp"
#include <algorithm>
#include <cmath>

// ─── OrbitCamera ─────────────────────────────────────────────────────────────

Vec3 OrbitCamera::eye() const
{
    float sp = std::sin(phi),   cp = std::cos(phi);
    float st = std::sin(theta), ct = std::cos(theta);
    return Vec3{ radius * sp * ct, radius * cp, radius * sp * st };
}

Mat4 OrbitCamera::viewMatrix() const
{
    Vec3 e      = eye();
    Vec3 center = { 0.0f, 0.0f, 0.0f };
    Vec3 up     = { 0.0f, 1.0f, 0.0f };
    return lookAt(e, center, up);
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

Vec2 screenToWorld2D(double sx, double sy, int winW, int winH)
{
    float aspect = (winW / 2.0f) / (float)winH;
    float hw = WORLD_HALF_W, hh = hw / aspect;
    float ndcX =  (float)(sx / (winW / 2.0)) * 2.0f - 1.0f;
    float ndcY = -((float)(sy / (double)winH) * 2.0f - 1.0f);
    return { ndcX * hw, ndcY * hh };
}

static float kColors[][3] = {
    {0.35f, 0.55f, 0.90f},
    {0.90f, 0.45f, 0.30f},
    {0.35f, 0.80f, 0.45f},
    {0.85f, 0.75f, 0.20f},
    {0.75f, 0.35f, 0.85f},
    {0.25f, 0.80f, 0.85f},
};
static int kColorCount = 6;

void EditorState::assignColor(Face& f)
{
    int idx = nextId % kColorCount;
    f.r = kColors[idx][0];
    f.g = kColors[idx][1];
    f.b = kColors[idx][2];
}

void EditorState::deselectAll()
{
    for (Face& f : faces) {
        if (f.selected) {
            f.selected = false;
            f.rebuildMesh();
        }
    }
    selectedFaceId = -1;
}

int EditorState::hitTestFace(Vec2 p)
{
    for (int i = (int)faces.size() - 1; i >= 0; i--)
        if (faces[i].isPointOnFace(p)) return i;
    return -1;
}

void EditorState::moveFace(Face& f, Vec2 delta)
{
    for (Vec2& v : f.outer)  { v.x += delta.x; v.y += delta.y; }
    for (Contour& h : f.holes)
        for (Vec2& v : h)    { v.x += delta.x; v.y += delta.y; }
}

void EditorState::performCut(int outerIdx, const Contour& cutContour)
{
    // Inner face
    Face inner;
    inner.id    = nextId++;
    inner.outer = cutContour;
    if (signedArea(inner.outer) < 0)
        std::reverse(inner.outer.begin(), inner.outer.end());
    assignColor(inner);
    inner.rebuildMesh();
    faces.push_back(std::move(inner));

    // Add hole to outer face (must be CW)
    Contour hole = cutContour;
    if (signedArea(hole) > 0)
        std::reverse(hole.begin(), hole.end());
    faces[outerIdx].holes.push_back(hole);
    faces[outerIdx].rebuildMesh();
}

// ─── Main per-frame state machine ─────────────────────────────────────────────

void EditorState::processFrame(int winW, int winH)
{
    bool inLeft = (mouseX < winW / 2.0);
    Vec2 mouseWorld = screenToWorld2D(mouseX, mouseY, winW, winH);

    switch (mode) {

    case EditorMode::NORMAL:
        if (leftJustPressed && inLeft) {
            int hit = hitTestFace(mouseWorld);
            if (hit >= 0) {
                faces[hit].selected = true;
                faces[hit].rebuildMesh();
                selectedFaceId = hit;
                mode = EditorMode::SELECTED;
            } else {
                drawingVerts.push_back(mouseWorld);
                mode = EditorMode::DRAWING;
            }
        }
        break;

    case EditorMode::DRAWING:
        if (leftJustPressed && inLeft) {
            drawingVerts.push_back(mouseWorld);
        }
        if (rightJustPressed && inLeft && (int)drawingVerts.size() >= 3) {
            Face f;
            f.id    = nextId++;
            f.outer = drawingVerts;
            if (signedArea(f.outer) < 0)
                std::reverse(f.outer.begin(), f.outer.end());
            assignColor(f);
            f.rebuildMesh();
            faces.push_back(std::move(f));
            drawingVerts.clear();
            mode = EditorMode::NORMAL;
        }
        if (escapePressed) {
            drawingVerts.clear();
            mode = EditorMode::NORMAL;
        }
        break;

    case EditorMode::SELECTED:
        if (cutKeyPressed) {
            drawingVerts.clear();
            mode = EditorMode::CUT_DRAWING;
        }
        if (moveKeyPressed) {
            moveAnchor = mouseWorld;
            mode = EditorMode::MOVING;
        }
        if (deletePressed && selectedFaceId >= 0) {
            faces[selectedFaceId].freeGPU();
            faces.erase(faces.begin() + selectedFaceId);
            selectedFaceId = -1;
            mode = EditorMode::NORMAL;
        }
        if (escapePressed) {
            deselectAll();
            mode = EditorMode::NORMAL;
        }
        if (leftJustPressed && inLeft) {
            int hit = hitTestFace(mouseWorld);
            deselectAll();
            if (hit >= 0) {
                faces[hit].selected = true;
                faces[hit].rebuildMesh();
                selectedFaceId = hit;
                mode = EditorMode::SELECTED;
            } else {
                mode = EditorMode::NORMAL;
            }
        }
        break;

    case EditorMode::CUT_DRAWING:
        if (leftJustPressed && inLeft) {
            drawingVerts.push_back(mouseWorld);
        }
        if (rightJustPressed && inLeft && (int)drawingVerts.size() >= 3) {
            int outerIdx = selectedFaceId;
            deselectAll();
            performCut(outerIdx, drawingVerts);
            drawingVerts.clear();
            mode = EditorMode::NORMAL;
        }
        if (escapePressed) {
            drawingVerts.clear();
            mode = EditorMode::SELECTED;
        }
        break;

    case EditorMode::MOVING:
        if (inLeft) {
            Vec2 delta = { mouseWorld.x - moveAnchor.x,
                           mouseWorld.y - moveAnchor.y };
            moveFace(faces[selectedFaceId], delta);
            faces[selectedFaceId].rebuildMesh();
            moveAnchor = mouseWorld;
        }
        if (leftJustPressed) {
            mode = EditorMode::SELECTED;
        }
        if (escapePressed) {
            mode = EditorMode::SELECTED;
        }
        break;
    }
}
