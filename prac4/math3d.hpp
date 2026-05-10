#ifndef MATH3D_HPP
#define MATH3D_HPP

#include "Matrix.h"
#include "Vector.h"

#include <cmath>

typedef Matrix<4, 4> Mat4;
typedef Vector<4> Vec4;
typedef Vector<3> Vec3;

inline Mat4 identity4()
{
    Mat4 m;
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
    return m;
}

inline Mat4 translate(float tx, float ty, float tz)
{
    Mat4 m = identity4();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
    return m;
}

inline Mat4 rotateX(float angle)
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[1][1] = c;
    m[1][2] = -s;
    m[2][1] = s;
    m[2][2] = c;
    return m;
}

inline Mat4 rotateY(float angle)
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[0][0] = c;
    m[0][2] = s;
    m[2][0] = -s;
    m[2][2] = c;
    return m;
}

inline Mat4 rotateZ(float angle)
{
    Mat4 m = identity4();
    float c = std::cos(angle);
    float s = std::sin(angle);
    m[0][0] = c;
    m[0][1] = -s;
    m[1][0] = s;
    m[1][1] = c;
    return m;
}

inline Mat4 perspective(float fovY, float aspect, float nearPlane, float farPlane)
{
    float f = 1.0f / std::tan(fovY * 0.5f);
    Mat4 m;
    m[0][0] = f / aspect;
    m[1][1] = f;
    m[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
    m[2][3] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    m[3][2] = -1.0f;
    return m;
}

inline Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up)
{
    Vec3 forward = (center - eye).unitVector();
    Vec3 right = forward.crossProduct(up).unitVector();
    Vec3 trueUp = right.crossProduct(forward);

    Mat4 m = identity4();
    m[0][0] = right[0];
    m[0][1] = right[1];
    m[0][2] = right[2];
    m[0][3] = -(right[0] * eye[0] + right[1] * eye[1] + right[2] * eye[2]);

    m[1][0] = trueUp[0];
    m[1][1] = trueUp[1];
    m[1][2] = trueUp[2];
    m[1][3] = -(trueUp[0] * eye[0] + trueUp[1] * eye[1] + trueUp[2] * eye[2]);

    m[2][0] = -forward[0];
    m[2][1] = -forward[1];
    m[2][2] = -forward[2];
    m[2][3] = forward[0] * eye[0] + forward[1] * eye[1] + forward[2] * eye[2];

    m[3][3] = 1.0f;
    return m;
}

inline void mat4ToArray(const Mat4 &m, float out[16])
{
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            out[col * 4 + row] = m[row][col];
        }
    }
}

#endif
