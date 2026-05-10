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
    GLsizei indexCount;

    Mesh(const Mesh &);
    Mesh &operator=(const Mesh &);

public:
    Mesh();
    ~Mesh();

    void upload(const MeshData &meshData);
    void draw() const;
    void release();
};

#endif
