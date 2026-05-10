#ifndef VECTOR_CPP
#define VECTOR_CPP

#include "Vector.h"
#include <cmath>

template<int n>
Vector<n>::Vector()
{
    arr = new float[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = 0.0f;
    }
}

template<int n>
Vector<n>::Vector(std::initializer_list<float> list)
{
    arr = new float[n];
    int i = 0;
    for (float val : list)
    {
        if (i >= n) break;
        arr[i++] = val;
    }
    while (i < n)
    {
        arr[i++] = 0.0f;
    }
}

template<int n>
Vector<n>::Vector(float* ptr)
{
    arr = ptr;
}

template<int n>
Vector<n>::~Vector()
{
    delete[] arr;
}

template<int n>
Vector<n>::Vector(const Vector<n>& other)
{
    arr = new float[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = other.arr[i];
    }
}

template<int n>
Vector<n>::Vector(const Matrix<n, 1>& mat)
{
    arr = new float[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = mat[i][0];
    }
}

template<int n>
Vector<n>& Vector<n>::operator=(const Vector<n>& other)
{
    if (this != &other)
    {
        delete[] arr;
        arr = new float[n];
        for (int i = 0; i < n; i++)
        {
            arr[i] = other.arr[i];
        }
    }
    return *this;
}

template<int n>
Vector<n>::operator Matrix<n, 1>() const
{
    Matrix<n, 1> mat;
    for (int i = 0; i < n; i++)
    {
        mat[i][0] = arr[i];
    }
    return mat;
}

template<int n>
int Vector<n>::getN() const
{
    return n;
}

template<int n>
Vector<n> Vector<n>::operator+(const Vector<n> other) const
{
    Vector<n> result;
    for (int i = 0; i < n; i++)
    {
        result[i] = arr[i] + other[i];
    }
    return result;
}

template<int n>
Vector<n> Vector<n>::operator-(const Vector<n> other) const
{
    Vector<n> result;
    for (int i = 0; i < n; i++)
    {
        result[i] = arr[i] - other[i];
    }
    return result;
}

template<int n>
Vector<n> Vector<n>::operator*(const float scalar) const
{
    Vector<n> result;
    for (int i = 0; i < n; i++)
    {
        result[i] = arr[i] * scalar;
    }
    return result;
}

template<int n>
float Vector<n>::operator*(const Vector<n> other) const
{
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i] * other[i];
    }
    return sum;
}

template<int n>
float Vector<n>::magnitude() const
{
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i] * arr[i];
    }
    return std::sqrt(sum);
}

template<int n>
Vector<n> Vector<n>::unitVector() const
{
    float mag = magnitude();
    if (mag == 0.0f)
    {
        throw "Invalid unit vector";
    }
    return (*this) * (1.0f / mag);
}

template<int n>
Vector<3> Vector<n>::crossProduct(const Vector<3> other) const
{
    Vector<3> result;
    result[0] = arr[1] * other[2] - arr[2] * other[1];
    result[1] = arr[2] * other[0] - arr[0] * other[2];
    result[2] = arr[0] * other[1] - arr[1] * other[0];
    return result;
}

#endif /*VECTOR_CPP*/