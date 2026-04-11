#ifndef MATRIX_CPP
#define MATRIX_CPP

#include "Matrix.h"

template<int n, int m>
Matrix<n-1, m-1> subMatrix(const Matrix<n,m>& mat, int row, int col)
{
    Matrix<n-1, m-1> result;
    int ri = 0;
    for (int i = 0; i < n; i++)
    {
        if (i == row) continue;
        int rj = 0;
        for (int j = 0; j < m; j++)
        {
            if (j == col) continue;
            result[ri][rj] = mat[i][j];
            rj++;
        }
        ri++;
    }
    return result;
}

template<>
inline float Matrix<1,1>::determinant() const
{
    return arr[0][0];
}

template<int n, int m>
Matrix<n,m>::Matrix()
{
    arr = new float*[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new float[m];
        for (int j = 0; j < m; j++)
        {
            arr[i][j] = 0.0f;
        }
    }
}

template<int n, int m>
Matrix<n,m>::Matrix(float** ptr)
{
    arr = ptr;
}

template<int n, int m>
Matrix<n,m>::Matrix(const Matrix<n,m>& other)
{
    arr = new float*[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new float[m];
        for (int j = 0; j < m; j++)
        {
            arr[i][j] = other[i][j];
        }
    }
}

template<int n, int m>
Matrix<n,m>::~Matrix()
{
    delete[] arr;
}

template<int n, int m>
Matrix<n,m>& Matrix<n,m>::operator=(const Matrix<n,m>& other)
{
    if (this != &other)
    {
        for (int i = 0; i < n; i++)
        {
            delete[] arr[i];
        }
        delete[] arr;
        arr = new float*[n];
        for (int i = 0; i < n; i++)
        {
            arr[i] = new float[m];
            for (int j = 0; j < m; j++)
            {
                arr[i][j] = other[i][j];
            }
        }
    }
    return *this;
}

template<int n, int m>
template<int a>
Matrix<n,a> Matrix<n,m>::operator*(const Matrix<m,a> other) const
{
    Matrix<n,a> result;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < a; j++)
        {
            float sum = 0.0f;
            for (int k = 0; k < m; k++)
            {
                sum += arr[i][k] * other[k][j];
            }
            result[i][j] = sum;
        }
    }
    return result;
}

template<int n, int m>
Matrix<n,m> Matrix<n,m>::operator*(const float scalar) const
{
    Matrix<n,m> result;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            result[i][j] = arr[i][j] * scalar;
        }
    }
    return result;
}

template<int n, int m>
Matrix<n,m> Matrix<n,m>::operator+(const Matrix<n,m> other) const
{
    Matrix<n,m> result;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            result[i][j] = arr[i][j] + other[i][j];
        }
    }
    return result;
}

template<int n, int m>
Matrix<m,n> Matrix<n,m>::operator~() const
{
    Matrix<m,n> result;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            result[j][i] = arr[i][j];
        }
    }
    return result;
}

template<int n, int m>
int Matrix<n,m>::getN() const
{
    return n;
}

template<int n, int m>
int Matrix<n,m>::getM() const
{
    return m;
}

template<int n, int m>
float Matrix<n,m>::determinant() const
{
    if (n != m)
    {
        throw "Matrix is not square";
    }
    float det = 0.0f;
    for (int j = 0; j < m; j++)
    {
        float sign = (j % 2 == 0) ? 1.0f : -1.0f;
        Matrix<n-1, m-1> sub = subMatrix(*this, 0, j);
        det += sign * arr[0][j] * sub.determinant();
    }
    return det;
}

#endif /*MATRIX_CPP*/