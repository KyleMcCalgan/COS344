#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <GL/glew.h>

// A single drawable object. Holds two VAO/VBO pairs:
//   - triangle geometry (GL_TRIANGLES, solid mode)
//   - line geometry     (GL_LINES,     wireframe mode)
// Vertex layout in both buffers: interleaved [x, y, z, r, g, b] (stride = 6 floats).
struct Mesh
{
    GLuint triVAO  = 0, triVBO  = 0;
    GLuint lineVAO = 0, lineVBO = 0;
    int    triCount  = 0;
    int    lineCount = 0;

    // Upload geometry. tris / lines are flat [x,y,z,r,g,b,...] arrays.
    void upload(const std::vector<float>& tris,
                const std::vector<float>& lines);

    // Draw using GL_TRIANGLES or GL_LINES depending on wireframe flag.
    void draw(bool wireframe) const;

    void free();
};

#endif // MESH_HPP
