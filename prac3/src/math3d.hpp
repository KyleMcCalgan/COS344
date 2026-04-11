#ifndef MATH3D_HPP
#define MATH3D_HPP

// math3d.hpp — 4x4 matrix math built on prac1 Matrix/Vector, no GLM math used.
// Provides: identity, translate, rotateX/Y/Z, perspective, lookAt, mat4ToArray.

#include "../prac1/Matrix.h"
#include "../prac1/Vector.h"
#include <cmath>

// Convenience alias
typedef Matrix<4,4> Mat4;
typedef Vector<4>   Vec4;
typedef Vector<3>   Vec3;

// ─── Identity ────────────────────────────────────────────────────────────────
inline Mat4 identity4()
{
    Mat4 m;
    m[0][0] = 1; m[1][1] = 1; m[2][2] = 1; m[3][3] = 1;
    return m;
}

// ─── Translation ─────────────────────────────────────────────────────────────
inline Mat4 translate(float tx, float ty, float tz)
{
    Mat4 m = identity4();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
    return m;
}

// ─── Rotations ───────────────────────────────────────────────────────────────
inline Mat4 rotateX(float angle) // angle in radians
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[1][1] =  c;  m[1][2] = -s;
    m[2][1] =  s;  m[2][2] =  c;
    return m;
}

inline Mat4 rotateY(float angle)
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[0][0] =  c;  m[0][2] =  s;
    m[2][0] = -s;  m[2][2] =  c;
    return m;
}

inline Mat4 rotateZ(float angle)
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[0][0] =  c;  m[0][1] = -s;
    m[1][0] =  s;  m[1][1] =  c;
    return m;
}

// Rodrigues rotation around an arbitrary unit axis
inline Mat4 rotateAxis(float angle, float ax, float ay, float az)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;
    Mat4 m = identity4();
    m[0][0] = t*ax*ax + c;    m[0][1] = t*ax*ay - s*az; m[0][2] = t*ax*az + s*ay;
    m[1][0] = t*ax*ay + s*az; m[1][1] = t*ay*ay + c;    m[1][2] = t*ay*az - s*ax;
    m[2][0] = t*ax*az - s*ay; m[2][1] = t*ay*az + s*ax; m[2][2] = t*az*az + c;
    return m;
}

// ─── Perspective projection ───────────────────────────────────────────────────
// fovY in radians, aspect = w/h, near/far clip planes
inline Mat4 perspective(float fovY, float aspect, float near, float far)
{
    float f = 1.0f / std::tan(fovY * 0.5f);
    Mat4 m;
    m[0][0] = f / aspect;
    m[1][1] = f;
    m[2][2] = (far + near) / (near - far);
    m[2][3] = (2.0f * far * near) / (near - far);
    m[3][2] = -1.0f;
    return m;
}

// ─── LookAt (view matrix) ─────────────────────────────────────────────────────
inline Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up)
{
    // forward = normalize(center - eye)
    Vec3 f = (center - eye).unitVector();
    // right  = normalize(f x up)
    Vec3 r = f.crossProduct(up).unitVector();
    // true up = r x f
    Vec3 u = r.crossProduct(f);

    Mat4 m = identity4();
    m[0][0] = r[0];  m[0][1] = r[1];  m[0][2] = r[2];  m[0][3] = -(r[0]*eye[0] + r[1]*eye[1] + r[2]*eye[2]);
    m[1][0] = u[0];  m[1][1] = u[1];  m[1][2] = u[2];  m[1][3] = -(u[0]*eye[0] + u[1]*eye[1] + u[2]*eye[2]);
    m[2][0] = -f[0]; m[2][1] = -f[1]; m[2][2] = -f[2]; m[2][3] =  (f[0]*eye[0] + f[1]*eye[1] + f[2]*eye[2]);
    m[3][3] = 1.0f;
    return m;
}

// ─── Flatten Mat4 to column-major float[16] for glUniformMatrix4fv ────────────
inline void mat4ToArray(const Mat4& m, float out[16])
{
    // OpenGL expects column-major: out[col*4 + row]
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            out[col * 4 + row] = m[row][col];
}

#endif // MATH3D_HPP
