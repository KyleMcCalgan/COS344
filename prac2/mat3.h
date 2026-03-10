#ifndef MAT3_H
#define MAT3_H

#include <cmath>
#include "Matrix.h"

// Build a 3x3 identity matrix
Matrix<3,3> identity3();

// 2D transform matrices (homogeneous coordinates)
Matrix<3,3> translate2D(float tx, float ty);
Matrix<3,3> rotate2D(float radians);       // CCW positive
Matrix<3,3> scale2D(float sx, float sy);

// Flatten row-major Matrix<3,3> to float[9] for glUniformMatrix3fv
// Pass GL_TRUE for the transpose param when uploading
void flattenMatrix(const Matrix<3,3>& m, float out[9]);

// ---- implementations (header-only, like Matrix.h) ----

inline Matrix<3,3> identity3() {
    Matrix<3,3> m;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
    return m;
}

inline Matrix<3,3> translate2D(float tx, float ty) {
    Matrix<3,3> m = identity3();
    m[0][2] = tx; m[1][2] = ty;
    return m;
}

inline Matrix<3,3> rotate2D(float r) {
    Matrix<3,3> m = identity3();
    m[0][0] =  cosf(r); m[0][1] = -sinf(r);
    m[1][0] =  sinf(r); m[1][1] =  cosf(r);
    return m;
}

inline Matrix<3,3> scale2D(float sx, float sy) {
    Matrix<3,3> m = identity3();
    m[0][0] = sx; m[1][1] = sy;
    return m;
}

inline void flattenMatrix(const Matrix<3,3>& m, float out[9]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            out[i*3+j] = m[i][j];
}

#endif
