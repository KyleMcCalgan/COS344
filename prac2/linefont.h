#ifndef LINEFONT_H
#define LINEFONT_H

#include <vector>
#include <string>
#include <GL/glew.h>

// ============================================================
//  Minimal line-segment font (GL_LINES)
//  Each character is defined on a 4x6 grid (x: 0-4, y: 0-6)
//  Origin is bottom-left of the character cell.
// ============================================================

// A segment is two endpoints: x0,y0, x1,y1
struct Seg { float x0,y0,x1,y1; };

static std::vector<Seg> getChar(char c) {
    switch(c) {
    case 'A': return {{0,0,2,6},{2,6,4,0},{1,3,3,3}};
    case 'B': return {{0,0,0,6},{0,6,3,6},{3,6,4,5},{4,5,3,3},{3,3,0,3},{3,3,4,2},{4,2,4,1},{4,1,3,0},{3,0,0,0}};
    case 'C': return {{4,1,3,0},{3,0,1,0},{1,0,0,1},{0,1,0,5},{0,5,1,6},{1,6,3,6},{3,6,4,5}};
    case 'D': return {{0,0,0,6},{0,6,2,6},{2,6,4,4},{4,4,4,2},{4,2,2,0},{2,0,0,0}};
    case 'E': return {{4,0,0,0},{0,0,0,6},{0,6,4,6},{0,3,3,3}};
    case 'F': return {{0,0,0,6},{0,6,4,6},{0,3,3,3}};
    case 'G': return {{4,5,3,6},{3,6,1,6},{1,6,0,5},{0,5,0,1},{0,1,1,0},{1,0,3,0},{3,0,4,1},{4,1,4,3},{4,3,2,3}};
    case 'H': return {{0,0,0,6},{4,0,4,6},{0,3,4,3}};
    case 'I': return {{0,0,4,0},{0,6,4,6},{2,0,2,6}};
    case 'J': return {{0,1,1,0},{1,0,3,0},{3,0,4,1},{4,1,4,6}};
    case 'K': return {{0,0,0,6},{4,6,0,3},{0,3,4,0}};
    case 'L': return {{0,6,0,0},{0,0,4,0}};
    case 'M': return {{0,0,0,6},{0,6,2,3},{2,3,4,6},{4,6,4,0}};
    case 'N': return {{0,0,0,6},{0,6,4,0},{4,0,4,6}};
    case 'O': return {{1,0,0,1},{0,1,0,5},{0,5,1,6},{1,6,3,6},{3,6,4,5},{4,5,4,1},{4,1,3,0},{3,0,1,0}};
    case 'P': return {{0,0,0,6},{0,6,3,6},{3,6,4,5},{4,5,4,4},{4,4,3,3},{3,3,0,3}};
    case 'Q': return {{1,0,0,1},{0,1,0,5},{0,5,1,6},{1,6,3,6},{3,6,4,5},{4,5,4,1},{4,1,3,0},{3,0,1,0},{2,2,4,0}};
    case 'R': return {{0,0,0,6},{0,6,3,6},{3,6,4,5},{4,5,4,4},{4,4,3,3},{3,3,0,3},{2,3,4,0}};
    case 'S': return {{4,5,3,6},{3,6,1,6},{1,6,0,5},{0,5,0,4},{0,4,1,3},{1,3,3,3},{3,3,4,2},{4,2,4,1},{4,1,3,0},{3,0,1,0},{1,0,0,1}};
    case 'T': return {{0,6,4,6},{2,6,2,0}};
    case 'U': return {{0,6,0,1},{0,1,1,0},{1,0,3,0},{3,0,4,1},{4,1,4,6}};
    case 'V': return {{0,6,2,0},{2,0,4,6}};
    case 'W': return {{0,6,1,0},{1,0,2,3},{2,3,3,0},{3,0,4,6}};
    case 'X': return {{0,0,4,6},{0,6,4,0}};
    case 'Y': return {{0,6,2,3},{4,6,2,3},{2,3,2,0}};
    case 'Z': return {{0,6,4,6},{4,6,0,0},{0,0,4,0}};
    case '0': return {{1,0,0,1},{0,1,0,5},{0,5,1,6},{1,6,3,6},{3,6,4,5},{4,5,4,1},{4,1,3,0},{3,0,1,0},{1,1,3,5}};
    case '1': return {{1,5,2,6},{2,6,2,0},{0,0,4,0}};
    case '2': return {{0,5,1,6},{1,6,3,6},{3,6,4,5},{4,5,4,4},{4,4,0,1},{0,1,0,0},{0,0,4,0}};
    case '3': return {{0,5,1,6},{1,6,3,6},{3,6,4,5},{4,5,4,4},{4,4,3,3},{3,3,4,2},{4,2,4,1},{4,1,3,0},{3,0,1,0},{1,0,0,1},{1,3,3,3}};
    case '4': return {{0,6,0,3},{0,3,4,3},{4,6,4,0}};
    case '+': return {{2,1,2,5},{0,3,4,3}};
    case '-': return {{0,3,4,3}};
    case '/': return {{4,6,0,0}};
    case ':': return {{1,4,2,4},{2,4,2,3},{2,3,1,3},{1,3,1,4},{1,1,2,1},{2,1,2,0},{2,0,1,0},{1,0,1,1}};
    case ' ': return {};
    default:  return {};
    }
}

// ============================================================
//  LineFont renderer
// ============================================================

struct LineFont {
    GLuint VAO, VBO;
    GLuint shaderID;
    GLuint colourLoc, modelLoc, projLoc;

    void init(GLuint shader, GLuint col, GLuint model, GLuint proj) {
        shaderID  = shader;
        colourLoc = col;
        modelLoc  = model;
        projLoc   = proj;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    // Draw a string at (x,y) in NDC. charW/charH = size of one character cell in NDC.
    void drawString(const std::string& text, float x, float y,
                    float charW, float charH, float r, float g, float b,
                    const float proj[9])
    {
        float cellW  = charW;
        float scaleX = cellW  / 4.0f;   // glyph grid is 0-4
        float scaleY = charH  / 6.0f;   // glyph grid is 0-6

        float curX = x;
        for (char c : text) {
            auto segs = getChar(c);
            std::vector<float> verts;
            verts.reserve(segs.size() * 4);
            for (auto& s : segs) {
                verts.push_back(s.x0 * scaleX);
                verts.push_back(s.y0 * scaleY);
                verts.push_back(s.x1 * scaleX);
                verts.push_back(s.y1 * scaleY);
            }

            if (!verts.empty()) {
                // model = translate to (curX, y)
                float md[9] = {
                    1,0,curX,
                    0,1,y,
                    0,0,1
                };

                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER,
                    verts.size()*sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

                glUniformMatrix3fv(modelLoc, 1, GL_TRUE, md);
                glUniformMatrix3fv(projLoc,  1, GL_TRUE, proj);
                glUniform3f(colourLoc, r, g, b);

                glDrawArrays(GL_LINES, 0, (GLsizei)(verts.size()/2));
                glBindVertexArray(0);
            }
            curX += cellW * 1.2f;  // advance + small gap
        }
    }
};

#endif
