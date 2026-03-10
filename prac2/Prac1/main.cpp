#include "Matrix.h"
#include "Vector.h"
#include "Shape.h"
#include "Triangle.h"
#include "Square.h"

#include <iostream>
#include <sstream>

using namespace std;

// int main(int argc, char const *argv[])
// {
    
// }


// ==================== VECTOR TESTS ====================

void testVectorConstructors() {
    std::cout << "=== Vector Constructors ===" << std::endl;

    // Default
    Vector<3> v1;
    std::cout << "Default (expect 0 0 0):" << std::endl;
    v1.print();

    // Initializer list - exact
    Vector<3> v2 = {1.0f, 2.0f, 3.0f};
    std::cout << "Initializer list exact (expect 1 2 3):" << std::endl;
    v2.print();

    // Initializer list - too few
    Vector<3> v3 = {1.0f, 2.0f};
    std::cout << "Initializer list too few (expect 1 2 0):" << std::endl;
    v3.print();

    // Initializer list - too many
    Vector<3> v4 = {1.0f, 2.0f, 3.0f, 4.0f};
    std::cout << "Initializer list too many (expect 1 2 3):" << std::endl;
    v4.print();

    // Float pointer (shallow)
    float* raw = new float[3]{7.0f, 8.0f, 9.0f};
    Vector<3> v5(raw);
    std::cout << "Float pointer (expect 7 8 9):" << std::endl;
    v5.print();

    // Copy constructor
    Vector<3> v6(v2);
    std::cout << "Copy of v2 (expect 1 2 3):" << std::endl;
    v6.print();
    v6[0] = 99.0f;
    std::cout << "v2 after modifying copy (expect 1 2 3 - unchanged):" << std::endl;
    v2.print();
}

void testVectorAssignment() {
    std::cout << "=== Vector Assignment ===" << std::endl;

    Vector<3> v1 = {1.0f, 2.0f, 3.0f};
    Vector<3> v2;
    v2 = v1;
    std::cout << "Assigned (expect 1 2 3):" << std::endl;
    v2.print();

    v2[0] = 99.0f;
    std::cout << "v1 after modifying v2 (expect 1 2 3 - unchanged):" << std::endl;
    v1.print();

    std::cout << "Self assignment (expect 1 2 3 - no crash):" << std::endl;
    v1 = v1;
    v1.print();
}

void testVectorArithmetic() {
    std::cout << "=== Vector Arithmetic ===" << std::endl;

    Vector<3> v1 = {1.0f, 2.0f, 3.0f};
    Vector<3> v2 = {4.0f, 5.0f, 6.0f};

    std::cout << "Addition (expect 5 7 9):" << std::endl;
    (v1 + v2).print();

    std::cout << "Subtraction (expect -3 -3 -3):" << std::endl;
    (v1 - v2).print();

    std::cout << "Scalar multiply by 2 (expect 2 4 6):" << std::endl;
    (v1 * 2.0f).print();

    std::cout << "Dot product (expect 32):" << std::endl;
    std::cout << (v1 * v2) << std::endl;
}

void testVectorMagnitudeAndUnit() {
    std::cout << "=== Magnitude and Unit Vector ===" << std::endl;

    Vector<3> v1 = {3.0f, 4.0f, 0.0f};
    std::cout << "Magnitude of (3,4,0) (expect 5):" << std::endl;
    std::cout << v1.magnitude() << std::endl;

    std::cout << "Unit vector of (3,4,0) (expect 0.6 0.8 0):" << std::endl;
    v1.unitVector().print();

    std::cout << "Magnitude of unit vector (expect 1):" << std::endl;
    std::cout << v1.unitVector().magnitude() << std::endl;

    try {
        Vector<3> zero = {0.0f, 0.0f, 0.0f};
        zero.unitVector();
    } catch (const char* e) {   // <-- this line changed
        std::cout << "Zero vector exception (expect 'Invalid unit vector'): " << e << std::endl;
    }
}

void testVectorCrossProduct() {
    std::cout << "=== Cross Product ===" << std::endl;

    Vector<3> x = {1.0f, 0.0f, 0.0f};
    Vector<3> y = {0.0f, 1.0f, 0.0f};

    std::cout << "x cross y (expect 0 0 1):" << std::endl;
    x.crossProduct(y).print();

    std::cout << "y cross x (expect 0 0 -1):" << std::endl;
    y.crossProduct(x).print();

    Vector<3> a = {1.0f, 2.0f, 3.0f};
    Vector<3> b = {4.0f, 5.0f, 6.0f};
    std::cout << "a cross b (expect -3 6 -3):" << std::endl;
    a.crossProduct(b).print();
}

void testVectorMatrixConversion() {
    std::cout << "=== Vector <-> Matrix Conversion ===" << std::endl;

    Vector<3> v = {1.0f, 2.0f, 3.0f};

    // Vector to Matrix
    Matrix<3,1> mat = v;
    std::cout << "Vector to Matrix<3,1> (expect 1 / 2 / 3):" << std::endl;
    mat.print();

    // Matrix to Vector
    Vector<3> v2(mat);
    std::cout << "Matrix<3,1> back to Vector (expect 1 2 3):" << std::endl;
    v2.print();
}

// ==================== MATRIX TESTS ====================

void testMatrixConstructors() {
    std::cout << "=== Matrix Constructors ===" << std::endl;

    // Default
    Matrix<2,2> m1;
    std::cout << "Default 2x2 (expect 0s):" << std::endl;
    m1.print();

    // Float** (shallow)
    float** raw = new float*[2];
    raw[0] = new float[2]{1.0f, 2.0f};
    raw[1] = new float[2]{3.0f, 4.0f};
    Matrix<2,2> m2(raw);
    std::cout << "Float** constructor (expect 1 2 / 3 4):" << std::endl;
    m2.print();

    // Copy constructor
    Matrix<2,2> m3(m2);
    std::cout << "Copy constructor (expect 1 2 / 3 4):" << std::endl;
    m3.print();
    m3[0][0] = 99.0f;
    std::cout << "m2 after modifying copy (expect 1 2 / 3 4 - unchanged):" << std::endl;
    m2.print();
}

void testMatrixAssignment() {
    std::cout << "=== Matrix Assignment ===" << std::endl;

    Matrix<2,2> m1;
    m1[0][0] = 1.0f; m1[0][1] = 2.0f;
    m1[1][0] = 3.0f; m1[1][1] = 4.0f;

    Matrix<2,2> m2;
    m2 = m1;
    std::cout << "Assigned (expect 1 2 / 3 4):" << std::endl;
    m2.print();

    m2[0][0] = 99.0f;
    std::cout << "m1 after modifying m2 (expect 1 2 / 3 4 - unchanged):" << std::endl;
    m1.print();

    std::cout << "Self assignment (expect 1 2 / 3 4 - no crash):" << std::endl;
    m1 = m1;
    m1.print();
}

void testMatrixArithmetic() {
    std::cout << "=== Matrix Arithmetic ===" << std::endl;

    Matrix<2,2> m1;
    m1[0][0]=1; m1[0][1]=2;
    m1[1][0]=3; m1[1][1]=4;

    Matrix<2,2> m2;
    m2[0][0]=5; m2[0][1]=6;
    m2[1][0]=7; m2[1][1]=8;

    std::cout << "Addition (expect 6 8 / 10 12):" << std::endl;
    (m1 + m2).print();

    std::cout << "Scalar multiply by 2 (expect 2 4 / 6 8):" << std::endl;
    (m1 * 2.0f).print();

    // 2x2 * 2x2 = 2x2
    // [1 2] * [5 6] = [1*5+2*7  1*6+2*8] = [19 22]
    // [3 4]   [7 8]   [3*5+4*7  3*6+4*8]   [43 50]
    std::cout << "Matrix multiply 2x2 * 2x2 (expect 19 22 / 43 50):" << std::endl;
    (m1 * m2).print();

    // Non-square multiply: 2x3 * 3x2 = 2x2
    Matrix<2,3> a;
    a[0][0]=1; a[0][1]=2; a[0][2]=3;
    a[1][0]=4; a[1][1]=5; a[1][2]=6;
    Matrix<3,2> b;
    b[0][0]=7;  b[0][1]=8;
    b[1][0]=9;  b[1][1]=10;
    b[2][0]=11; b[2][1]=12;
    std::cout << "Matrix multiply 2x3 * 3x2 (expect 58 64 / 139 154):" << std::endl;
    (a * b).print();
}

void testMatrixTranspose() {
    std::cout << "=== Transpose ===" << std::endl;

    Matrix<2,3> m;
    m[0][0]=1; m[0][1]=2; m[0][2]=3;
    m[1][0]=4; m[1][1]=5; m[1][2]=6;

    std::cout << "Original 2x3:" << std::endl;
    m.print();
    std::cout << "Transposed 3x2 (expect 1 4 / 2 5 / 3 6):" << std::endl;
    (~m).print();
}

void testMatrixDeterminant() {
    std::cout << "=== Determinant ===" << std::endl;

    // 1x1
    Matrix<1,1> m1;
    m1[0][0] = 5.0f;
    std::cout << "1x1 det (expect 5):" << std::endl;
    std::cout << m1.determinant() << std::endl;

    // 2x2: det([1 2 / 3 4]) = 1*4 - 2*3 = -2
    Matrix<2,2> m2;
    m2[0][0]=1; m2[0][1]=2;
    m2[1][0]=3; m2[1][1]=4;
    std::cout << "2x2 det (expect -2):" << std::endl;
    std::cout << m2.determinant() << std::endl;

    // 3x3
    // [6 1 1]
    // [4 -2 5]  det = 6*((-2)*7 - 5*3) - 1*(4*7 - 5*2) + 1*(4*3 - (-2)*2) = -306
    // [2 8 7]
    Matrix<3,3> m3;
    m3[0][0]=6;  m3[0][1]=1;  m3[0][2]=1;
    m3[1][0]=4;  m3[1][1]=-2; m3[1][2]=5;
    m3[2][0]=2;  m3[2][1]=8;  m3[2][2]=7;
    std::cout << "3x3 det (expect -306):" << std::endl;
    std::cout << m3.determinant() << std::endl;
}

void testGetters() {
    std::cout << "=== Getters ===" << std::endl;

    Vector<4> v;
    std::cout << "Vector getN (expect 4): " << v.getN() << std::endl;

    Matrix<3,5> m;
    std::cout << "Matrix getN (expect 3): " << m.getN() << std::endl;
    std::cout << "Matrix getM (expect 5): " << m.getM() << std::endl;
}

// ==================== SHAPE TESTS ====================

void testTriangleConstructors() {
    std::cout << "=== Triangle Constructors ===" << std::endl;

    Vector<2> p1 = {0.0f, 0.0f};
    Vector<2> p2 = {1.0f, 0.0f};
    Vector<2> p3 = {0.0f, 1.0f};

    // Regular constructor
    Triangle<2> t1(p1, p2, p3);
    std::cout << "Triangle constructor (expect 0,0 / 1,0 / 0,1):" << std::endl;
    t1.print();

    // Copy constructor
    Triangle<2> t2(t1);
    std::cout << "Copy constructor (expect 0,0 / 1,0 / 0,1):" << std::endl;
    t2.print();
}

void testTriangleGetPoints() {
    std::cout << "=== Triangle getPoints ===" << std::endl;

    Vector<2> p1 = {1.0f, 2.0f};
    Vector<2> p2 = {3.0f, 4.0f};
    Vector<2> p3 = {5.0f, 6.0f};
    Triangle<2> t(p1, p2, p3);

    std::cout << "getNumPoints (expect 6): " << t.getNumPoints() << std::endl;

    float* pts = t.getPoints();
    std::cout << "getPoints (expect 1 2 3 4 5 6): ";
    for (int i = 0; i < t.getNumPoints(); i++) {
        std::cout << pts[i] << " ";
    }
    std::cout << std::endl;
    delete[] pts;
}

void testTriangleMatrixMultiply() {
    std::cout << "=== Triangle Matrix Multiply ===" << std::endl;

    Vector<2> p1 = {1.0f, 0.0f};
    Vector<2> p2 = {0.0f, 1.0f};
    Vector<2> p3 = {1.0f, 1.0f};
    Triangle<2> t(p1, p2, p3);

    Matrix<2,2> scale;
    scale[0][0] = 2.0f; scale[0][1] = 0.0f;
    scale[1][0] = 0.0f; scale[1][1] = 2.0f;

    t *= scale;
    std::cout << "After *= scale by 2 (expect 2,0 / 0,2 / 2,2):" << std::endl;
    t.print();

    {
        Triangle<2> t2(t);
        t2 *= scale;
        std::cout << "After * scale by 2 again (expect 4,0 / 0,4 / 4,4):" << std::endl;
        t2.print();
    } // t2 destroyed here cleanly

    std::cout << "Original unchanged (expect 2,0 / 0,2 / 2,2):" << std::endl;
    t.print();
}

void testSquareConstructors() {
    std::cout << "=== Square Constructors ===" << std::endl;

    // Centre constructor
    Vector<2> centre = {0.0f, 0.0f};
    Square<2> s1(centre, 4.0f, 6.0f); // height=4, width=6
    std::cout << "Centre (0,0), height=4, width=6:" << std::endl;
    std::cout << "(expect tl:-3,2 / tr:3,2 / br:3,-2 / bl:-3,-2):" << std::endl;
    s1.print();

    // Four points constructor
    Vector<2> tl = {-1.0f,  1.0f};
    Vector<2> tr = { 1.0f,  1.0f};
    Vector<2> br = { 1.0f, -1.0f};
    Vector<2> bl = {-1.0f, -1.0f};
    Square<2> s2(tl, tr, br, bl);
    std::cout << "Four points constructor (expect -1,1 / 1,1 / 1,-1 / -1,-1):" << std::endl;
    s2.print();

    // Copy constructor
    Square<2> s3(s2);
    std::cout << "Copy constructor (expect -1,1 / 1,1 / 1,-1 / -1,-1):" << std::endl;
    s3.print();
}

void testSquareGetPoints() {
    std::cout << "=== Square getPoints ===" << std::endl;

    Vector<2> tl = {1.0f, 2.0f};
    Vector<2> tr = {3.0f, 4.0f};
    Vector<2> br = {5.0f, 6.0f};
    Vector<2> bl = {7.0f, 8.0f};
    Square<2> s(tl, tr, br, bl);

    std::cout << "getNumPoints (expect 8): " << s.getNumPoints() << std::endl;

    float* pts = s.getPoints();
    std::cout << "getPoints (expect 1 2 3 4 5 6 7 8): ";
    for (int i = 0; i < s.getNumPoints(); i++) {
        std::cout << pts[i] << " ";
    }
    std::cout << std::endl;
    delete[] pts;
}

void testSquareMatrixMultiply() {
    std::cout << "=== Square Matrix Multiply ===" << std::endl;

    Vector<2> centre = {0.0f, 0.0f};
    Square<2> s(centre, 2.0f, 2.0f);
    std::cout << "Original (expect tl:-1,1 / tr:1,1 / br:1,-1 / bl:-1,-1):" << std::endl;
    s.print();

    Matrix<2,2> scale;
    scale[0][0] = 3.0f; scale[0][1] = 0.0f;
    scale[1][0] = 0.0f; scale[1][1] = 3.0f;

    s *= scale;
    std::cout << "After *= scale by 3 (expect tl:-3,3 / tr:3,3 / br:3,-3 / bl:-3,-3):" << std::endl;
    s.print();

    {
        Square<2> s2(s);
        s2 *= scale;
        std::cout << "After * scale by 3 again (expect tl:-9,9 / tr:9,9 / br:9,-9 / bl:-9,-9):" << std::endl;
        s2.print();
    } // s2 destroyed here cleanly

    std::cout << "Original unchanged (expect tl:-3,3 / tr:3,3 / br:3,-3 / bl:-3,-3):" << std::endl;
    s.print();
}

void testSquareCentreWithHigherDimension() {
    std::cout << "=== Square Centre Constructor Higher Dimension ===" << std::endl;

    // Only x,y should change - z should stay as centre's z (5.0)
    Vector<3> centre = {0.0f, 0.0f, 5.0f};
    Square<3> s(centre, 2.0f, 2.0f);
    std::cout << "3D Square, centre z=5 (expect all points to have z=5):" << std::endl;
    s.print();
}

// ==================== MAIN ====================

int main() {
    testVectorConstructors();
    testVectorAssignment();
    testVectorArithmetic();
    testVectorMagnitudeAndUnit();
    testVectorCrossProduct();
    testVectorMatrixConversion();

    testMatrixConstructors();
    testMatrixAssignment();
    testMatrixArithmetic();
    testMatrixTranspose();
    testMatrixDeterminant();
    testGetters();

    testTriangleConstructors();
    testTriangleGetPoints();
    testTriangleMatrixMultiply();

    testSquareConstructors();
    testSquareGetPoints();
    testSquareMatrixMultiply();
    testSquareCentreWithHigherDimension();

    return 0;
}