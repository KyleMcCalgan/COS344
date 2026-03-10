#ifndef TRIANGLE_CPP
#define TRIANGLE_CPP

#include "Triangle.h"

// Constructor
template<int n>
Triangle<n>::Triangle(const Vector<n>& p1, const Vector<n>& p2, const Vector<n>& p3)
    : p1(p1), p2(p2), p3(p3) {}

// Copy constructor
template<int n>
Triangle<n>::Triangle(const Triangle<n>& other)
    : p1(other.p1), p2(other.p2), p3(other.p3) {}

// operator*= (apply matrix to each point in place)
template<int n>
Triangle<n>& Triangle<n>::operator*=(const Matrix<n,n>& mat) {
    p1 = Vector<n>(mat * Matrix<n,1>(p1));
    p2 = Vector<n>(mat * Matrix<n,1>(p2));
    p3 = Vector<n>(mat * Matrix<n,1>(p3));
    return *this;
}

// operator* (apply matrix to a copy)
template<int n>
Triangle<n>* Triangle<n>::operator*(const Matrix<n,n>& mat) const {
    Triangle<n>* result = new Triangle<n>(*this);
    *result *= mat;
    return result;
}

// getPoints - returns flattened array of all point components: p1, p2, p3
template<int n>
float* Triangle<n>::getPoints() const {
    float* points = new float[n * 3];
    for (int i = 0; i < n; i++) points[i]       = p1[i];
    for (int i = 0; i < n; i++) points[n + i]   = p2[i];
    for (int i = 0; i < n; i++) points[2*n + i] = p3[i];
    return points;
}

// getNumPoints - total number of floats in getPoints array
template<int n>
int Triangle<n>::getNumPoints() const {
    return n * 3;
}

#endif /*TRIANGLE_CPP*/