#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry.h"

extern const float PI=3.1415926;
extern const int DEFAULT_ALLOC=4;

template <> template <> Vec3<int>::Vec3<>(const Vec3<float> &v) : x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {
}

template <> template <> Vec3<float>::Vec3<>(const Vec3<int> &v) : x(v.x), y(v.y), z(v.z) {
}


Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), cols(c) { }

int Matrix::nrows() {
    return rows;
}

int Matrix::ncols() {
    return cols;
}

Matrix Matrix::identity(int dimensions) {
    Matrix E(dimensions, dimensions);
    for (int i=0; i<dimensions; i++) {
        for (int j=0; j<dimensions; j++) {
            E[i][j] = (i==j ? 1.f : 0.f);
        }
    }
    return E;
}

std::vector<float>& Matrix::operator[](const int i) {
    assert(i>=0 && i<rows);
    return m[i];
}

Matrix Matrix::operator*(const Matrix& a) {
    assert(cols == a.rows);
    Matrix result(rows, a.cols);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<a.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k=0; k<cols; k++) {
                result.m[i][j] += m[i][k]*a.m[k][j];
            }
        }
    }
    return result;
}

Vec4f Matrix::operator*(Vec4f a) {
    assert(cols == 4);
    Vec4f result;
    for (int i=0; i<rows; i++) {
        result[i] = 0.f;
        for (int j=0; j<4; j++) {
            result[i] += m[i][j] * a[j];
        }
    }
    return result;
}

Vec3f Matrix::operator*(Vec3f a) {
    assert(cols == 3);
    Vec3f result;
    for (int i=0; i<rows; i++) {
        result.raw[i] = 0.f;
        for (int j=0; j<3; j++) {
            result.raw[i] += m[i][j] * a[j];
        }
    }
    return result;
}

Matrix Matrix::transpose() {
    Matrix result(cols, rows);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix Matrix::inverse() {
    assert(rows==cols);
    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    Matrix result(rows, cols*2);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[i][j] = m[i][j];
    for(int i=0; i<rows; i++)
        result[i][i+cols] = 1;
    // first pass
    for (int i=0; i<rows-1; i++) {
        // normalize the first row
        for(int j=result.cols-1; j>=0; j--)
            result[i][j] /= result[i][i];
        for (int k=i+1; k<rows; k++) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // normalize the last row
    for(int j=result.cols-1; j>=rows-1; j--)
        result[rows-1][j] /= result[rows-1][rows-1];
    // second pass
    for (int i=rows-1; i>0; i--) {
        for (int k=i-1; k>=0; k--) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
}

void Matrix::SetCol(int index, const Vec4f& v)
{
    for(int i=0;i<4;i++) m[i][index] = v.raw[i];
}

void Matrix::SetCol(int index, const Vec3f& v)
{
    for(int i=0;i<3;i++) m[i][index] = v[i];
}
void Matrix::SetRow(int index, const Vec4f& v)
{
    for(int i=0;i<4;i++) m[index][i] = v.raw[i];
}

void Matrix::SetRow(int index, const Vec3f& v)
{
    for(int i=0;i<3;i++) m[index][i] = v[i];
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i=0; i<m.nrows(); i++)  {
        for (int j=0; j<m.ncols(); j++) {
            s << m[i][j];
            if (j<m.ncols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

Transform::Transform():position(0), scale(1), rotation(0) {}
Transform::Transform(Vec3f p):position(p), scale(1), rotation(0) {}
Transform::Transform(Vec3f p, Vec3f r):position(p), scale(1), rotation(r) {}
Transform::Transform(Vec3f p, Vec3f s, Vec3f r):position(p), scale(s), rotation(r) {}
