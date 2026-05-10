#ifndef FACE_HPP
#define FACE_HPP

#include <vector>
#include "Mesh.hpp"

struct Vec2 { float x, y; };
using Contour = std::vector<Vec2>;

struct Face {
    int     id       = 0;
    Contour outer;               // CCW winding
    std::vector<Contour> holes;  // CW winding
    bool    selected = false;
    float   r = 0.4f, g = 0.6f, b = 0.9f;
    Mesh    mesh;

    void rebuildMesh();
    Vec2 centroid() const;
    bool containsPoint(Vec2 p) const;   // point inside outer contour
    bool isPointOnFace(Vec2 p) const;   // inside outer but not inside any hole
    void freeGPU();

    Face() = default;
    Face(const Face&) = delete;
    Face& operator=(const Face&) = delete;
    Face(Face&&) = default;
    Face& operator=(Face&&) = default;
};

// Push a [x,y,z,r,g,b] vertex into a float buffer
inline void pushV(std::vector<float>& v,
                  float x, float y, float z,
                  float r, float g, float b)
{
    v.push_back(x); v.push_back(y); v.push_back(z);
    v.push_back(r); v.push_back(g); v.push_back(b);
}

#endif // FACE_HPP
