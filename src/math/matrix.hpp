#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include <iostream>
#include <cassert>
#include <stdexcept>

#include "constant.h"
#include "vector.hpp"

// Matrix 4x4
// Since we are building matrix for rendering
class Matrix
{
public:
    static const Matrix Identity;

    union
    {
        double m_[4][4];
        double m_array_[16];
    }; 

    Matrix() : m_{  {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0} } {}

    Matrix(double m00, double m01, double m02, double m03,
        double m10, double m11, double m12, double m13,
        double m20, double m21, double m22, double m23,
        double m30, double m31, double m32, double m33)
    {
        m_[0][0] = m00, m_[0][1] = m01, m_[0][2] = m02, m_[0][3] = m03;
        m_[1][0] = m10, m_[1][1] = m11, m_[1][2] = m12, m_[1][3] = m13;
        m_[2][0] = m20, m_[2][1] = m21, m_[2][2] = m22, m_[2][3] = m23;
        m_[3][0] = m30, m_[3][1] = m31, m_[3][2] = m32, m_[3][3] = m33;
    }

    Matrix(const std::initializer_list<double>& init)
    {
        assert(init.size() == 16 && "Matrix initializer must have exactly 16 values");
        std::copy(init.begin(), init.end(), &m_[0][0]);
    }

    Matrix(const Matrix& m) { memcpy(m_array_, m.m_array_, 16 * sizeof(double)); }

    Matrix& operator = (const Matrix& m)
    {
        if(this == &m) return *this;
        memcpy(m_array_, m.m_array_, 16 * sizeof(double));
        return *this;
    }
};

inline
const Matrix Matrix::Identity = Matrix(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0);

inline
std::ostream& operator << (std::ostream& os, const Matrix& mat)
{
    os << "{" << std::endl;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            os << mat.m_[i][j];
            if(!(i == 3 && j == 3)) os << ", ";
        }
        if(i != 3) os << std::endl;
    }
    os << std::endl << "}" << std::endl;
    return os;
}

template <typename T>
inline 
Vector4<T> operator * (const Matrix& mat, const Vector4<T>& vec)
{
    Vector4<T> result_vec;
    result_vec.x_ = (mat.m_[0][0] * vec.x_ + mat.m_[0][1] * vec.y_ + mat.m_[0][2] * vec.z_ + mat.m_[0][3] * vec.w_);
    result_vec.y_ = (mat.m_[1][0] * vec.x_ + mat.m_[1][1] * vec.y_ + mat.m_[1][2] * vec.z_ + mat.m_[1][3] * vec.w_);
    result_vec.z_ = (mat.m_[2][0] * vec.x_ + mat.m_[2][1] * vec.y_ + mat.m_[2][2] * vec.z_ + mat.m_[2][3] * vec.w_);
    result_vec.w_ = (mat.m_[3][0] * vec.x_ + mat.m_[3][1] * vec.y_ + mat.m_[3][2] * vec.z_ + mat.m_[3][3] * vec.w_);
    return result_vec;
}

inline 
Matrix operator * (const Matrix& mat1, const Matrix& mat2)
{
    Matrix result_mat;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            for(int k = 0; k < 4; ++k)
            {
                result_mat.m_[i][j] += mat1.m_[i][k] * mat2.m_[k][j];
            }
        }
    }
    return result_mat;
}

// LH -> RH
template <typename T>
inline 
Matrix lookAt(const Vector3<T>& eye, const Vector3<T>& center, const Vector3<T>& up)
{
    Vector3<T> f = normalize(center - eye);

    Vector3<T> s = normalize(cross(up, f));
    Vector3<T> u = cross(f, s);
    
    Matrix result_mat(
        s.x_, s.y_, s.z_, -dot(s, eye),
        u.x_, u.y_, u.z_, -dot(u, eye),
        -f.x_, -f.y_, -f.z_, dot(f, eye),
        0, 0, 0, 1
    );

    return result_mat;
}

inline
Matrix transpose(const Matrix& mat)
{
    Matrix result_mat;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            result_mat.m_[i][j] = mat.m_[j][i]; 
        }
    }
    return result_mat;
}

inline 
Matrix scale(const Matrix& mat, const double scale_x, const double scale_y, const double scale_z)
{
    Matrix result_mat(mat);
    result_mat.m_[0][0] *= scale_x;
    result_mat.m_[1][1] *= scale_y;
    result_mat.m_[2][2] *= scale_z;
    return result_mat;
}

inline 
Matrix translate(const Matrix& mat, const Vector3<double>& v)
{
    Matrix result_mat(mat);
    result_mat.m_[0][3] += v.x_;
    result_mat.m_[1][3] += v.y_;
    result_mat.m_[2][3] += v.z_;
    return result_mat;
}

inline 
Matrix translate(const Matrix& mat, const Vector3<float>& v)
{
    Matrix result_mat(mat);
    result_mat.m_[0][3] += static_cast<double>(v.x_);
    result_mat.m_[1][3] += static_cast<double>(v.y_);
    result_mat.m_[2][3] += static_cast<double>(v.z_);
    return result_mat;
}

// parameter near and far are coordinates
// since we look at -z, 0 > near > far
inline 
Matrix ortho(float width, float height, float near, float far)
{
    // frustum is always centered : r = -l = width/2, t = -b = height/2s
    Matrix ortho_translate_mat
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(near + far)/2,
        0, 0, 0, 1
    };
    Matrix ortho_scale_mat
    {
        2/ width, 0, 0, 0,
        0, 2 / height, 0, 0,
        0, 0, 2 / (near - far), 0,
        0, 0, 0, 1
    };
    return ortho_scale_mat * ortho_translate_mat;
}

// parameter near and far are coordinates
// negative since we look at -z
inline
Matrix perspective(float fovy, float aspect, float near, float far)
{
    Matrix perspective_to_ortho_mat
    {
        near, 0, 0, 0,
        0, near, 0, 0,
        0, 0, near + far, -near * far,
        0, 0, 1, 0
    };

    float radian_fovy = fovy * PI / 180.0;
    float height = -near * tan(radian_fovy / 2) * 2;

    Matrix ortho_mat = ortho(height * aspect, height, near, far);

    return ortho_mat * perspective_to_ortho_mat;
}

typedef Matrix Mat4;


#endif