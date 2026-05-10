#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

#include "Geometry.h"

class Mesh
{
private:
    GLuint vertexArrayId;
    GLuint vertexBufferId;
    GLuint elementBufferId;
    GLuint edgeElementBufferId;
    GLsizei triangleIndexCount;
    GLsizei edgeIndexCount;

    Mesh(const Mesh &);
    Mesh &operator=(const Mesh &);

public:
    Mesh();
    ~Mesh();

    void upload(const MeshData &meshData);
    void drawSolid() const;
    void drawWireframe() const;
    void release();
};

#endif
