#include "Mesh.hpp"

static void setupVAO(GLuint& vao, GLuint& vbo, const std::vector<float>& data)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(data.size() * sizeof(float)),
                 data.data(),
                 GL_STATIC_DRAW);

    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // colour (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::upload(const std::vector<float>& tris,
                  const std::vector<float>& lines)
{
    triCount  = (int)tris.size()  / 6;
    lineCount = (int)lines.size() / 6;

    setupVAO(triVAO,  triVBO,  tris);
    setupVAO(lineVAO, lineVBO, lines);
}

void Mesh::draw(bool wireframe) const
{
    if (wireframe)
    {
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINES, 0, lineCount);
    }
    else
    {
        glBindVertexArray(triVAO);
        glDrawArrays(GL_TRIANGLES, 0, triCount);
    }
    glBindVertexArray(0);
}

void Mesh::free()
{
    glDeleteVertexArrays(1, &triVAO);
    glDeleteBuffers(1, &triVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    triVAO = triVBO = lineVAO = lineVBO = 0;
}
