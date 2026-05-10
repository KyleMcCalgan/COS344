#include "Renderer.hpp"
#include "math3d.hpp"
#include <cmath>

void Renderer::init(GLuint shader)
{
    m_shader = shader;
    m_mvpLoc = glGetUniformLocation(shader, "MVP");
}

void Renderer::setMVP(const Mat4& mvp)
{
    float arr[16];
    mat4ToArray(mvp, arr);
    glUniformMatrix4fv(m_mvpLoc, 1, GL_FALSE, arr);
}

// ─── Main draw ────────────────────────────────────────────────────────────────

void Renderer::drawFrame(EditorState& state, int winW, int winH)
{
    glUseProgram(m_shader);
    glEnable(GL_SCISSOR_TEST);

    int lw = winW / 2;
    int rw = winW - lw;

    // Left: dark grey
    glViewport(0, 0, lw, winH);
    glScissor (0, 0, lw, winH);
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawLeftView(state, winW, winH);

    // Right: dark blue
    glViewport(lw, 0, rw, winH);
    glScissor (lw, 0, rw, winH);
    glClearColor(0.08f, 0.08f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawRightView(state, winW, winH);

    glDisable(GL_SCISSOR_TEST);
    drawDivider(winW, winH);
}

// ─── Left (2D orthographic) ───────────────────────────────────────────────────

void Renderer::drawLeftView(EditorState& state, int winW, int winH)
{
    float aspect = (winW / 2.0f) / (float)winH;
    float hw = WORLD_HALF_W, hh = hw / aspect;
    Mat4 proj2D = ortho2D(-hw, hw, -hh, hh, -10.0f, 10.0f);

    // Draw all faces: solid fill then wireframe overlay
    for (const Face& f : state.faces) {
        setMVP(proj2D);
        if (!state.wireframe) f.mesh.draw(false);
        f.mesh.draw(true);    // always show edges in 2D view
    }

    // In-progress contour
    if (!state.drawingVerts.empty()) {
        Vec2 cursor = screenToWorld2D(state.mouseX, state.mouseY, winW, winH);
        bool isCut = (state.mode == EditorMode::CUT_DRAWING);
        drawContourPreview(state.drawingVerts, cursor, isCut, proj2D);
        drawVertexDots(state.drawingVerts, proj2D);
    }
}

// ─── Right (3D perspective + orbit) ──────────────────────────────────────────

void Renderer::drawRightView(const EditorState& state, int winW, int winH)
{
    float aspect = (float)(winW - winW / 2) / (float)winH;
    Mat4 proj = perspective(0.785f, aspect, 0.1f, 100.0f);
    Mat4 view = state.camera.viewMatrix();
    Mat4 vp   = proj * view;

    for (const Face& f : state.faces) {
        setMVP(vp);
        if (state.wireframe) {
            f.mesh.draw(true);
        } else {
            f.mesh.draw(false);
        }
    }
}

// ─── In-progress contour preview ─────────────────────────────────────────────

void Renderer::drawContourPreview(const std::vector<Vec2>& verts,
                                   Vec2 cursorWorld, bool isCut,
                                   const Mat4& mvp)
{
    if (verts.empty()) return;

    float lr = isCut ? 1.0f : 1.0f;
    float lg = isCut ? 0.4f : 1.0f;
    float lb = isCut ? 0.0f : 0.0f;

    std::vector<float> lines;
    for (int i = 0; i + 1 < (int)verts.size(); i++) {
        pushV(lines, verts[i].x,   verts[i].y,   0, lr, lg, lb);
        pushV(lines, verts[i+1].x, verts[i+1].y, 0, lr, lg, lb);
    }
    // Cursor rubber-band line
    pushV(lines, verts.back().x, verts.back().y, 0, lr, lg, lb);
    pushV(lines, cursorWorld.x,  cursorWorld.y,  0, lr*0.6f, lg*0.6f, lb+0.4f);

    m_tempMesh.free();
    m_tempMesh.upload({}, lines);
    setMVP(mvp);
    m_tempMesh.draw(true);
}

void Renderer::drawVertexDots(const std::vector<Vec2>& verts,
                               const Mat4& mvp)
{
    static const float S = 0.05f;
    std::vector<float> tris;
    for (const Vec2& v : verts) {
        // Two triangles forming a small quad
        float x = v.x, y = v.y;
        pushV(tris, x-S, y-S, 0, 1,1,0);
        pushV(tris, x+S, y-S, 0, 1,1,0);
        pushV(tris, x+S, y+S, 0, 1,1,0);
        pushV(tris, x-S, y-S, 0, 1,1,0);
        pushV(tris, x+S, y+S, 0, 1,1,0);
        pushV(tris, x-S, y+S, 0, 1,1,0);
    }
    m_tempMesh.free();
    m_tempMesh.upload(tris, {});
    setMVP(mvp);
    m_tempMesh.draw(false);
}

// ─── Divider line ─────────────────────────────────────────────────────────────

void Renderer::drawDivider(int winW, int winH)
{
    // Draw a thin 2-pixel wide vertical line in the centre using a screen-space quad
    glViewport(0, 0, winW, winH);
    float px = 2.0f / winW;  // 2 pixels in NDC

    std::vector<float> tris;
    // Two triangles: a thin vertical rect from bottom to top at NDC x=0
    pushV(tris, -px, -1, 0, 0.5f, 0.5f, 0.5f);
    pushV(tris,  px, -1, 0, 0.5f, 0.5f, 0.5f);
    pushV(tris,  px,  1, 0, 0.5f, 0.5f, 0.5f);
    pushV(tris, -px, -1, 0, 0.5f, 0.5f, 0.5f);
    pushV(tris,  px,  1, 0, 0.5f, 0.5f, 0.5f);
    pushV(tris, -px,  1, 0, 0.5f, 0.5f, 0.5f);

    m_tempMesh.free();
    m_tempMesh.upload(tris, {});
    setMVP(identity4());
    m_tempMesh.draw(false);
}
