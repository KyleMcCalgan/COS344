#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "mat3.h"

// ============================================================
//  Geometry generators — all shapes centred at origin
// ============================================================

// Rectangle: two GL_TRIANGLES
std::vector<float> makeRect(float w, float h);

// Rectangle wireframe: 4 edges as GL_LINES
std::vector<float> makeRectWire(float w, float h);

// Circle: N triangles (triangle fan decomposed to GL_TRIANGLES)
std::vector<float> makeCircle(float r, int N);

// Circle wireframe: N perimeter edges as GL_LINES
std::vector<float> makeCircleWire(float r, int N);

// Triangle: 3 vertices (GL_TRIANGLES)
std::vector<float> makeTriangle(float base, float height);

// Triangle wireframe: 3 edges as GL_LINES
std::vector<float> makeTriangleWire(float base, float height);

// Hexagon: flat-top regular hexagon of radius r (GL_TRIANGLES)
std::vector<float> makeHexagon(float r);

// Hexagon wireframe: 6 edges as GL_LINES
std::vector<float> makeHexagonWire(float r);

// ============================================================
//  RenderShape — one drawable object
// ============================================================

struct RenderShape {
    GLuint VAO, VBO;
    GLuint wireVAO, wireVBO;
    int vertCount, wireVertCount;

    glm::vec3 colour;
    glm::vec3 pastelColour;
    bool selected;
    bool isSelectable;
    int  selectKey;     // 1-4, or 0 = not selectable

    // Cumulative transform state
    float tx, ty;
    float rotation;
    float sx, sy;

    void init(std::vector<float> verts, std::vector<float> wireVerts,
              glm::vec3 col, glm::vec3 pastel,
              bool selectable, int key,
              float initTx=0, float initTy=0,
              float initSx=1, float initSy=1);

    Matrix<3,3> modelMatrix();

    void draw(GLuint modelLoc, GLuint colourLoc, bool wireframeMode);
};

// ---- implementations ----

inline std::vector<float> makeRect(float w, float h) {
    float hw = w*0.5f, hh = h*0.5f;
    return { -hw,-hh,  hw,-hh,  hw, hh,
             -hw,-hh,  hw, hh, -hw, hh };
}

inline std::vector<float> makeRectWire(float w, float h) {
    float hw = w*0.5f, hh = h*0.5f;
    return { -hw,-hh,  hw,-hh,
              hw,-hh,  hw, hh,
              hw, hh, -hw, hh,
             -hw, hh, -hw,-hh };
}

inline std::vector<float> makeCircle(float r, int N) {
    std::vector<float> v;
    for (int i = 0; i < N; i++) {
        float a0 = 2.0f * M_PI * i / N;
        float a1 = 2.0f * M_PI * (i+1) / N;
        v.push_back(0);          v.push_back(0);
        v.push_back(r*cosf(a0)); v.push_back(r*sinf(a0));
        v.push_back(r*cosf(a1)); v.push_back(r*sinf(a1));
    }
    return v;
}

inline std::vector<float> makeCircleWire(float r, int N) {
    std::vector<float> v;
    for (int i = 0; i < N; i++) {
        float a0 = 2.0f * M_PI * i / N;
        float a1 = 2.0f * M_PI * (i+1) / N;
        v.push_back(r*cosf(a0)); v.push_back(r*sinf(a0));
        v.push_back(r*cosf(a1)); v.push_back(r*sinf(a1));
    }
    return v;
}

inline std::vector<float> makeTriangle(float base, float height) {
    float hb = base*0.5f, third = height/3.0f;
    return { -hb, -third,   hb, -third,   0, 2*third };
}

inline std::vector<float> makeTriangleWire(float base, float height) {
    float hb = base*0.5f, third = height/3.0f;
    return { -hb,-third,  hb,-third,
              hb,-third,  0, 2*third,
              0, 2*third, -hb,-third };
}

inline std::vector<float> makeHexagon(float r) {
    std::vector<float> v;
    for (int i = 0; i < 6; i++) {
        float a0 = 2.0f * M_PI * i / 6.0f;
        float a1 = 2.0f * M_PI * (i+1) / 6.0f;
        v.push_back(0);          v.push_back(0);
        v.push_back(r*cosf(a0)); v.push_back(r*sinf(a0));
        v.push_back(r*cosf(a1)); v.push_back(r*sinf(a1));
    }
    return v;
}

inline std::vector<float> makeHexagonWire(float r) {
    std::vector<float> v;
    for (int i = 0; i < 6; i++) {
        float a0 = 2.0f * M_PI * i / 6.0f;
        float a1 = 2.0f * M_PI * (i+1) / 6.0f;
        v.push_back(r*cosf(a0)); v.push_back(r*sinf(a0));
        v.push_back(r*cosf(a1)); v.push_back(r*sinf(a1));
    }
    return v;
}

inline void RenderShape::init(std::vector<float> verts, std::vector<float> wireVerts,
    glm::vec3 col, glm::vec3 pastel,
    bool selectable, int key,
    float initTx, float initTy, float initSx, float initSy)
{
    colour = col; pastelColour = pastel;
    selected = false; isSelectable = selectable; selectKey = key;
    tx = initTx; ty = initTy; rotation = 0; sx = initSx; sy = initSy;
    vertCount     = verts.size()     / 2;
    wireVertCount = wireVerts.size() / 2;

    auto upload = [](GLuint& vao, GLuint& vbo, std::vector<float>& data) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    };
    upload(VAO, VBO, verts);
    upload(wireVAO, wireVBO, wireVerts);
}

inline Matrix<3,3> RenderShape::modelMatrix() {
    return translate2D(tx, ty) * rotate2D(rotation) * scale2D(sx, sy);
}

inline void RenderShape::draw(GLuint modelLoc, GLuint colourLoc, bool wireframeMode) {
    float md[9];
    flattenMatrix(modelMatrix(), md);
    glUniformMatrix3fv(modelLoc, 1, GL_TRUE, md);

    glm::vec3 col = selected ? pastelColour : colour;
    glUniform3f(colourLoc, col.r, col.g, col.b);

    if (wireframeMode) {
        glBindVertexArray(wireVAO);
        glDrawArrays(GL_LINES, 0, wireVertCount);
    } else {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertCount);
    }
    glBindVertexArray(0);
}

#endif
