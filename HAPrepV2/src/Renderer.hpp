#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include "Editor.hpp"
#include "Mesh.hpp"

class Renderer {
public:
    void init(GLuint shaderProgram);
    void drawFrame(EditorState& state, int winW, int winH);

private:
    GLuint m_shader  = 0;
    GLint  m_mvpLoc  = -1;
    Mesh   m_tempMesh;

    void setMVP(const Mat4& mvp);
    void drawLeftView (EditorState& state, int winW, int winH);
    void drawRightView(const EditorState& state, int winW, int winH);
    void drawContourPreview(const std::vector<Vec2>& verts,
                            Vec2 cursorWorld, bool isCut, const Mat4& mvp);
    void drawVertexDots(const std::vector<Vec2>& verts,
                        const Mat4& mvp);
    void drawDivider(int winW, int winH);
};

#endif // RENDERER_HPP
