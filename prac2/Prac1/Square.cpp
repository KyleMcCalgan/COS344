#ifndef SQUARE_CPP
#define SQUARE_CPP

#include "Square.h"

template<int n>
Square<n>::Square(const Vector<n>& center, float height, float width) 
    : tl(center), tr(center), br(center), bl(center) 
{
    // Only the first two components (x, y) are affected
    // tl = top left:     x - width/2,  y + height/2
    // tr = top right:    x + width/2,  y + height/2
    // br = bottom right: x + width/2,  y - height/2
    // bl = bottom left:  x - width/2,  y - height/2
    tl[0] = center[0] - width  / 2.0f;
    tl[1] = center[1] + height / 2.0f;

    tr[0] = center[0] + width  / 2.0f;
    tr[1] = center[1] + height / 2.0f;

    br[0] = center[0] + width  / 2.0f;
    br[1] = center[1] - height / 2.0f;

    bl[0] = center[0] - width  / 2.0f;
    bl[1] = center[1] - height / 2.0f;
}

// Constructor from four points directly
template<int n>
Square<n>::Square(const Vector<n>& tl, const Vector<n>& tr, const Vector<n>& br, const Vector<n>& bl)
    : tl(tl), tr(tr), br(br), bl(bl) {}

// Copy constructor
template<int n>
Square<n>::Square(const Square<n>& other)
    : tl(other.tl), tr(other.tr), br(other.br), bl(other.bl) {}

// operator*= (apply matrix to each point in place)
template<int n>
Square<n>& Square<n>::operator*=(const Matrix<n,n>& mat) {
    tl = Vector<n>(mat * Matrix<n,1>(tl));
    tr = Vector<n>(mat * Matrix<n,1>(tr));
    br = Vector<n>(mat * Matrix<n,1>(br));
    bl = Vector<n>(mat * Matrix<n,1>(bl));
    return *this;
}

// operator* (apply matrix to a copy)
template<int n>
Square<n>* Square<n>::operator*(const Matrix<n,n>& mat) const {
    Square<n>* result = new Square<n>(*this);
    *result *= mat;
    return result;
}

// getPoints - returns flattened array: tl, tr, br, bl
template<int n>
float* Square<n>::getPoints() const {
    float* points = new float[n * 4];
    for (int i = 0; i < n; i++) points[i]       = tl[i];
    for (int i = 0; i < n; i++) points[n + i]   = tr[i];
    for (int i = 0; i < n; i++) points[2*n + i] = br[i];
    for (int i = 0; i < n; i++) points[3*n + i] = bl[i];
    return points;
}

// getNumPoints
template<int n>
int Square<n>::getNumPoints() const {
    return n * 4;
}

#endif /*SQUARE_CPP*/