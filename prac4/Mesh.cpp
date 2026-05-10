#include "Mesh.h"

namespace
{
const GLsizei FLOATS_PER_VERTEX = 8;
}

Mesh::Mesh()
    : vertexArrayId(0), vertexBufferId(0), elementBufferId(0), edgeElementBufferId(0),
      triangleIndexCount(0), edgeIndexCount(0)
{
}

Mesh::~Mesh()
{
    release();
}

void Mesh::upload(const MeshData &meshData)
{
    if (vertexArrayId == 0)
    {
        glGenVertexArrays(1, &vertexArrayId);
    }

    if (vertexBufferId == 0)
    {
        glGenBuffers(1, &vertexBufferId);
    }

    if (elementBufferId == 0)
    {
        glGenBuffers(1, &elementBufferId);
    }

    if (edgeElementBufferId == 0)
    {
        glGenBuffers(1, &edgeElementBufferId);
    }

    triangleIndexCount = static_cast<GLsizei>(meshData.indexCount);
    edgeIndexCount = static_cast<GLsizei>(meshData.edgeIndexCount);

    glBindVertexArray(vertexArrayId);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, meshData.floatCount * static_cast<int>(sizeof(float)),
                 meshData.vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indexCount * static_cast<int>(sizeof(unsigned int)),
                 meshData.indexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeElementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.edgeIndexCount * static_cast<int>(sizeof(unsigned int)),
                 meshData.edgeIndexData, GL_STATIC_DRAW);

    const GLsizei stride = FLOATS_PER_VERTEX * static_cast<GLsizei>(sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::drawSolid() const
{
    glBindVertexArray(vertexArrayId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    glDrawElements(GL_TRIANGLES, triangleIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::drawWireframe() const
{
    glBindVertexArray(vertexArrayId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeElementBufferId);
    glDrawElements(GL_LINES, edgeIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::release()
{
    if (edgeElementBufferId != 0)
    {
        glDeleteBuffers(1, &edgeElementBufferId);
        edgeElementBufferId = 0;
    }

    if (elementBufferId != 0)
    {
        glDeleteBuffers(1, &elementBufferId);
        elementBufferId = 0;
    }

    if (vertexBufferId != 0)
    {
        glDeleteBuffers(1, &vertexBufferId);
        vertexBufferId = 0;
    }

    if (vertexArrayId != 0)
    {
        glDeleteVertexArrays(1, &vertexArrayId);
        vertexArrayId = 0;
    }

    triangleIndexCount = 0;
    edgeIndexCount = 0;
}
