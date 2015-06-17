#ifndef MATH_MATRICES_H
#define MATH_MATRICES_H

#include "Vector.h"
#include<iostream>
using namespace::std;

///////////////////////////////////////////////////////////////////////////
// 3x3 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix3f
{
public:
    // constructors
    Matrix3f();  // init with identity
    Matrix3f(const float src[9]);
    Matrix3f(float xx, float xy, float xz,
            float yx, float yy, float yz,
            float zx, float zy, float zz);

    void        set(const float src[9]);
    void        set(float xx, float xy, float xz,
                    float yx, float yy, float yz,
                    float zx, float zy, float zz);
    void        setRow(int index, const float row[3]);
    void        setRow(int index, const Vector3f& v);
    void        setColumn(int index, const float col[3]);
    void        setColumn(int index, const Vector3f& v);

    const float* get() const;
    float       getDeterminant();

    Matrix3f&    identity();
    Matrix3f&    transpose();                            // transpose itself and return reference
    Matrix3f&    invert();

    // operators
    Matrix3f     operator+(const Matrix3f& rhs) const;    // add rhs
    Matrix3f     operator-(const Matrix3f& rhs) const;    // subtract rhs
    Matrix3f&    operator+=(const Matrix3f& rhs);         // add rhs and update this object
    Matrix3f&    operator-=(const Matrix3f& rhs);         // subtract rhs and update this object
    Vector3f     operator*(const Vector3f& rhs) const;    // multiplication: v' = M * v
    Matrix3f     operator*(const Matrix3f& rhs) const;    // multiplication: M3 = M1 * M2
    Matrix3f&    operator*=(const Matrix3f& rhs);         // multiplication: M1' = M1 * M2
    bool        operator==(const Matrix3f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Matrix3f& rhs) const;   // exact compare, no epsilon
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Matrix3f operator-(const Matrix3f& m);                     // unary operator (-)
    friend Matrix3f operator*(float scalar, const Matrix3f& m);       // pre-multiplication
    friend Vector3f operator*(const Vector3f& vec, const Matrix3f& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix3f& m);

protected:

private:
    float m[9];

};

///////////////////////////////////////////////////////////////////////////
// 4x4 matrix
///////////////////////////////////////////////////////////////////////////
class Matrix4f
{
public:
    // constructors
    Matrix4f();  // init with identity
    Matrix4f(const float src[16]);
    Matrix4f(float xx, float xy, float xz, float xw,
            float yx, float yy, float yz, float yw,
            float zx, float zy, float zz, float zw,
            float wx, float wy, float wz, float ww);

    void        set(const float src[16]);
    void        set(float xx, float xy, float xz, float xw,
                    float yx, float yy, float yz, float yw,
                    float zx, float zy, float zz, float zw,
                    float wx, float wy, float wz, float ww);
    void        setRow(int index, const float row[4]);
    void        setRow(int index, const Vector4f& v);
    void        setRow(int index, const Vector3f& v);
    void        setColumn(int index, const float col[4]);
    void        setColumn(int index, const Vector4f& v);
    void        setColumn(int index, const Vector3f& v);

    const float* get() const;
    const float* getTranspose();                        // return transposed matrix
    float        getDeterminant();

    Matrix4f&    identity();
    Matrix4f&    transpose();                            // transpose itself and return reference
	Matrix4f&	 inverse();

    // transform matrix
    Matrix4f&    translate(float x, float y, float z);   // translation by (x,y,z)
    Matrix4f&    translate(const Vector3f& v);            //
    Matrix4f&    rotate(float angle, const Vector3f& axis); // rotate angle(degree) along the given axis
    Matrix4f&    rotate(float angle, float x, float y, float z);
    Matrix4f&    rotateX(float angle);                   // rotate on X-axis with degree
    Matrix4f&    rotateY(float angle);                   // rotate on Y-axis with degree
    Matrix4f&    rotateZ(float angle);                   // rotate on Z-axis with degree
    Matrix4f&    scale(float scale);                     // uniform scale
    Matrix4f&    scale(float sx, float sy, float sz);    // scale by (sx, sy, sz) on each axis

    // operators
    Matrix4f     operator+(const Matrix4f& rhs) const;    // add rhs
    Matrix4f     operator-(const Matrix4f& rhs) const;    // subtract rhs
    Matrix4f&    operator+=(const Matrix4f& rhs);         // add rhs and update this object
    Matrix4f&    operator-=(const Matrix4f& rhs);         // subtract rhs and update this object
    Vector4f     operator*(const Vector4f& rhs) const;    // multiplication: v' = M * v
    Vector3f     operator*(const Vector3f& rhs) const;    // multiplication: v' = M * v
    Matrix4f     operator*(const Matrix4f& rhs) const;    // multiplication: M3 = M1 * M2
	Matrix4f	 operator*(const float f) const;
    Matrix4f&    operator*=(const Matrix4f& rhs);         // multiplication: M1' = M1 * M2
    bool        operator==(const Matrix4f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Matrix4f& rhs) const;   // exact compare, no epsilon
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]
	Matrix4f&	operator=(const Matrix4f& rhs);

    friend Matrix4f operator-(const Matrix4f& m);                     // unary operator (-)
    friend Matrix4f operator*(float scalar, const Matrix4f& m);       // pre-multiplication
    friend Vector3f operator*(const Vector3f& vec, const Matrix4f& m); // pre-multiplication
    friend Vector4f operator*(const Vector4f& vec, const Matrix4f& m); // pre-multiplication
    friend std::ostream& operator<<(std::ostream& os, const Matrix4f& m);

protected:

private:
    float       getCofactor(float m0, float m1, float m2,
                            float m3, float m4, float m5,
                            float m6, float m7, float m8);

    float m[16];
    float tm[16];                                       // transpose m

};

///////////////////////////////////////////////////////////////////////////
// inline functions for Matrix3f
///////////////////////////////////////////////////////////////////////////
inline Matrix3f::Matrix3f()
{
    // initially identity matrix
    identity();
}



inline Matrix3f::Matrix3f(const float src[9])
{
    set(src);
}



inline Matrix3f::Matrix3f(float xx, float xy, float xz,
                        float yx, float yy, float yz,
                        float zx, float zy, float zz)
{
    set(xx, xy, xz,  yx, yy, yz,  zx, zy, zz);
}



inline void Matrix3f::set(const float src[9])
{
    m[0] = src[0];  m[1] = src[1];  m[2] = src[2];
    m[3] = src[3];  m[4] = src[4];  m[5] = src[5];
    m[6] = src[6];  m[7] = src[7];  m[8] = src[8];
}



inline void Matrix3f::set(float xx, float xy, float xz,
                         float yx, float yy, float yz,
                         float zx, float zy, float zz)
{
    m[0] = xx;  m[1] = xy;  m[2] = xz;
    m[3] = yx;  m[4] = yy;  m[5] = yz;
    m[6] = zx;  m[7] = zy;  m[8] = zz;
}



inline void Matrix3f::setRow(int index, const float row[3])
{
    m[index*3] = row[0];  m[index*3 + 1] = row[1];  m[index*3 + 2] = row[2];
}



inline void Matrix3f::setRow(int index, const Vector3f& v)
{
    m[index*3] = v.x;  m[index*3 + 1] = v.y;  m[index*3 + 2] = v.z;
}



inline void Matrix3f::setColumn(int index, const float col[3])
{
    m[index] = col[0];  m[index + 3] = col[1];  m[index + 6] = col[2];
}



inline void Matrix3f::setColumn(int index, const Vector3f& v)
{
    m[index] = v.x;  m[index + 3] = v.y;  m[index + 6] = v.z;
}



inline const float* Matrix3f::get() const
{
    return m;
}



inline Matrix3f& Matrix3f::identity()
{
    m[0] = m[4] = m[8] = 1.0f;
    m[1] = m[2] = m[3] = m[5] = m[6] = m[7] = 0.0f;
    return *this;
}



inline Matrix3f Matrix3f::operator+(const Matrix3f& rhs) const
{
    return Matrix3f(m[0]+rhs[0], m[1]+rhs[1], m[2]+rhs[2],
                   m[3]+rhs[3], m[4]+rhs[4], m[5]+rhs[5],
                   m[6]+rhs[6], m[7]+rhs[7], m[8]+rhs[8]);
}



inline Matrix3f Matrix3f::operator-(const Matrix3f& rhs) const
{
    return Matrix3f(m[0]-rhs[0], m[1]-rhs[1], m[2]-rhs[2],
                   m[3]-rhs[3], m[4]-rhs[4], m[5]-rhs[5],
                   m[6]-rhs[6], m[7]-rhs[7], m[8]-rhs[8]);
}



inline Matrix3f& Matrix3f::operator+=(const Matrix3f& rhs)
{
    m[0] += rhs[0];  m[1] += rhs[1];  m[2] += rhs[2];
    m[3] += rhs[3];  m[4] += rhs[4];  m[5] += rhs[5];
    m[6] += rhs[6];  m[7] += rhs[7];  m[8] += rhs[8];
    return *this;
}



inline Matrix3f& Matrix3f::operator-=(const Matrix3f& rhs)
{
    m[0] -= rhs[0];  m[1] -= rhs[1];  m[2] -= rhs[2];
    m[3] -= rhs[3];  m[4] -= rhs[4];  m[5] -= rhs[5];
    m[6] -= rhs[6];  m[7] -= rhs[7];  m[8] -= rhs[8];
    return *this;
}



inline Vector3f Matrix3f::operator*(const Vector3f& rhs) const
{
    return Vector3f(m[0]*rhs.x + m[1]*rhs.y + m[2]*rhs.z,
                   m[3]*rhs.x + m[4]*rhs.y + m[5]*rhs.z,
                   m[6]*rhs.x + m[7]*rhs.y + m[8]*rhs.z);
}



inline Matrix3f Matrix3f::operator*(const Matrix3f& rhs) const
{
    return Matrix3f(m[0]*rhs[0] + m[1]*rhs[3] + m[2]*rhs[6],  m[0]*rhs[1] + m[1]*rhs[4] + m[2]*rhs[7],  m[0]*rhs[2] + m[1]*rhs[5] + m[2]*rhs[8],
                   m[3]*rhs[0] + m[4]*rhs[3] + m[5]*rhs[6],  m[3]*rhs[1] + m[4]*rhs[4] + m[5]*rhs[7],  m[3]*rhs[2] + m[4]*rhs[5] + m[5]*rhs[8],
                   m[6]*rhs[0] + m[7]*rhs[3] + m[8]*rhs[6],  m[6]*rhs[1] + m[7]*rhs[4] + m[8]*rhs[7],  m[6]*rhs[2] + m[7]*rhs[5] + m[8]*rhs[8]);
}



inline Matrix3f& Matrix3f::operator*=(const Matrix3f& rhs)
{
    *this = *this * rhs;
    return *this;
}



inline bool Matrix3f::operator==(const Matrix3f& rhs) const
{
    return (m[0] == rhs[0]) && (m[1] == rhs[1]) && (m[2] == rhs[2]) &&
           (m[3] == rhs[3]) && (m[4] == rhs[4]) && (m[5] == rhs[5]) &&
           (m[6] == rhs[6]) && (m[7] == rhs[7]) && (m[8] == rhs[8]);
}



inline bool Matrix3f::operator!=(const Matrix3f& rhs) const
{
    return (m[0] != rhs[0]) || (m[1] != rhs[1]) || (m[2] != rhs[2]) ||
           (m[3] != rhs[3]) || (m[4] != rhs[4]) || (m[5] != rhs[5]) ||
           (m[6] != rhs[6]) || (m[7] != rhs[7]) || (m[8] != rhs[8]);
}



inline float Matrix3f::operator[](int index) const
{
    return m[index];
}



inline float& Matrix3f::operator[](int index)
{
    return m[index];
}



inline Matrix3f operator-(const Matrix3f& rhs)
{
    return Matrix3f(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8]);
}



inline Matrix3f operator*(float s, const Matrix3f& rhs)
{
    return Matrix3f(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8]);
}



inline Vector3f operator*(const Vector3f& v, const Matrix3f& m)
{
    return Vector3f(v.x*m[0] + v.y*m[3] + v.z*m[6],  v.x*m[1] + v.y*m[4] + v.z*m[7],  v.x*m[2] + v.y*m[5] + v.z*m[8]);
}



inline std::ostream& operator<<(std::ostream& os, const Matrix3f& m)
{
    os << "(" << m[0] << ",\t" << m[1] << ",\t" << m[2] << ")\n"
       << "(" << m[3] << ",\t" << m[4] << ",\t" << m[5] << ")\n"
       << "(" << m[6] << ",\t" << m[7] << ",\t" << m[8] << ")\n";
    return os;
}
// END OF Matrix3f INLINE //////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// inline functions for Matrix4f
///////////////////////////////////////////////////////////////////////////
inline Matrix4f::Matrix4f()
{
    // initially identity matrix
    identity();
}



inline Matrix4f::Matrix4f(const float src[16])
{
    set(src);
}



inline Matrix4f::Matrix4f(float xx, float xy, float xz, float xw,
                        float yx, float yy, float yz, float yw,
                        float zx, float zy, float zz, float zw,
                        float wx, float wy, float wz, float ww)
{
    set(xx, xy, xz, xw,  yx, yy, yz, yw,  zx, zy, zz, zw,  wx, wy, wz, ww);
}



inline void Matrix4f::set(const float src[16])
{
    m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
    m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
    m[8] = src[8];  m[9] = src[9];  m[10]= src[10]; m[11]= src[11];
    m[12]= src[12]; m[13]= src[13]; m[14]= src[14]; m[15]= src[15];
}



inline void Matrix4f::set(float xx, float xy, float xz, float xw,
                         float yx, float yy, float yz, float yw,
                         float zx, float zy, float zz, float zw,
                         float wx, float wy, float wz, float ww)
{
    m[0] = xx;  m[1] = xy;  m[2] = xz;  m[3] = xw;
    m[4] = yx;  m[5] = yy;  m[6] = yz;  m[7] = yw;
    m[8] = zx;  m[9] = zy;  m[10]= zz;  m[11]= zw;
    m[12]= wx;  m[13]= wy;  m[14]= wz;  m[15]= ww;
}



inline void Matrix4f::setRow(int index, const float row[4])
{
    m[index*4] = row[0];  m[index*4 + 1] = row[1];  m[index*4 + 2] = row[2];  m[index*4 + 3] = row[3];
}



inline void Matrix4f::setRow(int index, const Vector4f& v)
{
    m[index*4] = v.x;  m[index*4 + 1] = v.y;  m[index*4 + 2] = v.z;  m[index*4 + 3] = v.w;
}



inline void Matrix4f::setRow(int index, const Vector3f& v)
{
    m[index*4] = v.x;  m[index*4 + 1] = v.y;  m[index*4 + 2] = v.z;
}



inline void Matrix4f::setColumn(int index, const float col[4])
{
    m[index] = col[0];  m[index + 4] = col[1];  m[index + 8] = col[2];  m[index + 12] = col[3];
}



inline void Matrix4f::setColumn(int index, const Vector4f& v)
{
    m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;  m[index + 12] = v.w;
}



inline void Matrix4f::setColumn(int index, const Vector3f& v)
{
    m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
}



inline const float* Matrix4f::get() const
{
    return m;
}



inline const float* Matrix4f::getTranspose()
{
    tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
    tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
    tm[8] = m[2];   tm[9] = m[6];   tm[10]= m[10];  tm[11]= m[14];
    tm[12]= m[3];   tm[13]= m[7];   tm[14]= m[11];  tm[15]= m[15];
    return tm;
}



inline Matrix4f& Matrix4f::identity()
{
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
    return *this;
}

inline Matrix4f Matrix4f::operator+(const Matrix4f& rhs) const
{
    return Matrix4f(m[0]+rhs[0],   m[1]+rhs[1],   m[2]+rhs[2],   m[3]+rhs[3],
                   m[4]+rhs[4],   m[5]+rhs[5],   m[6]+rhs[6],   m[7]+rhs[7],
                   m[8]+rhs[8],   m[9]+rhs[9],   m[10]+rhs[10], m[11]+rhs[11],
                   m[12]+rhs[12], m[13]+rhs[13], m[14]+rhs[14], m[15]+rhs[15]);
}



inline Matrix4f Matrix4f::operator-(const Matrix4f& rhs) const
{
    return Matrix4f(m[0]-rhs[0],   m[1]-rhs[1],   m[2]-rhs[2],   m[3]-rhs[3],
                   m[4]-rhs[4],   m[5]-rhs[5],   m[6]-rhs[6],   m[7]-rhs[7],
                   m[8]-rhs[8],   m[9]-rhs[9],   m[10]-rhs[10], m[11]-rhs[11],
                   m[12]-rhs[12], m[13]-rhs[13], m[14]-rhs[14], m[15]-rhs[15]);
}



inline Matrix4f& Matrix4f::operator+=(const Matrix4f& rhs)
{
    m[0] += rhs[0];    m[1] += rhs[1];    m[2] += rhs[2];    m[3] += rhs[3];
    m[4] += rhs[4];    m[5] += rhs[5];    m[6] += rhs[6];    m[7] += rhs[7];
    m[8] += rhs[8];    m[9] += rhs[9];    m[10] += rhs[10];  m[11] += rhs[11];
    m[12] += rhs[12];  m[13] += rhs[13];  m[14] += rhs[14];  m[15] += rhs[15];
    return *this;
}



inline Matrix4f& Matrix4f::operator-=(const Matrix4f& rhs)
{
    m[0] -= rhs[0];    m[1] -= rhs[1];    m[2] -= rhs[2];    m[3] -= rhs[3];
    m[4] -= rhs[4];    m[5] -= rhs[5];    m[6] -= rhs[6];    m[7] -= rhs[7];
    m[8] -= rhs[8];    m[9] -= rhs[9];    m[10] -= rhs[10];  m[11] -= rhs[11];
    m[12] -= rhs[12];  m[13] -= rhs[13];  m[14] -= rhs[14];  m[15] -= rhs[15];
    return *this;
}



inline Vector4f Matrix4f::operator*(const Vector4f& rhs) const
{
    return Vector4f(m[0]*rhs.x  + m[1]*rhs.y  + m[2]*rhs.z  + m[3]*rhs.w,
                   m[4]*rhs.x  + m[5]*rhs.y  + m[6]*rhs.z  + m[7]*rhs.w,
                   m[8]*rhs.x  + m[9]*rhs.y  + m[10]*rhs.z + m[11]*rhs.w,
                   m[12]*rhs.x + m[13]*rhs.y + m[14]*rhs.z + m[15]*rhs.w);
}



inline Vector3f Matrix4f::operator*(const Vector3f& rhs) const
{
    return Vector3f(m[0]*rhs.x + m[1]*rhs.y + m[2]*rhs.z,
                   m[4]*rhs.x + m[5]*rhs.y + m[6]*rhs.z,
                   m[8]*rhs.x + m[9]*rhs.y + m[10]*rhs.z);
}



inline Matrix4f Matrix4f::operator*(const Matrix4f& n) const
{
    return Matrix4f(m[0]*n[0]  + m[1]*n[4]  + m[2]*n[8]  + m[3]*n[12],   m[0]*n[1]  + m[1]*n[5]  + m[2]*n[9]  + m[3]*n[13],   m[0]*n[2]  + m[1]*n[6]  + m[2]*n[10]  + m[3]*n[14],   m[0]*n[3]  + m[1]*n[7]  + m[2]*n[11]  + m[3]*n[15],
                   m[4]*n[0]  + m[5]*n[4]  + m[6]*n[8]  + m[7]*n[12],   m[4]*n[1]  + m[5]*n[5]  + m[6]*n[9]  + m[7]*n[13],   m[4]*n[2]  + m[5]*n[6]  + m[6]*n[10]  + m[7]*n[14],   m[4]*n[3]  + m[5]*n[7]  + m[6]*n[11]  + m[7]*n[15],
                   m[8]*n[0]  + m[9]*n[4]  + m[10]*n[8] + m[11]*n[12],  m[8]*n[1]  + m[9]*n[5]  + m[10]*n[9] + m[11]*n[13],  m[8]*n[2]  + m[9]*n[6]  + m[10]*n[10] + m[11]*n[14],  m[8]*n[3]  + m[9]*n[7]  + m[10]*n[11] + m[11]*n[15],
                   m[12]*n[0] + m[13]*n[4] + m[14]*n[8] + m[15]*n[12],  m[12]*n[1] + m[13]*n[5] + m[14]*n[9] + m[15]*n[13],  m[12]*n[2] + m[13]*n[6] + m[14]*n[10] + m[15]*n[14],  m[12]*n[3] + m[13]*n[7] + m[14]*n[11] + m[15]*n[15]);
}

inline Matrix4f	Matrix4f::operator*(const float f) const
{
    return Matrix4f(m[0]*f,  m[1]*f,  m[2]*f,  m[3]*f,
                    m[4]*f,  m[5]*f,  m[6]*f,  m[7]*f,
                    m[8]*f,  m[9]*f,  m[10]*f, m[11]*f,
                    m[12]*f, m[13]*f, m[14]*f, m[15]*f);
}

inline Matrix4f& Matrix4f::operator*=(const Matrix4f& rhs)
{
    *this = *this * rhs;
    return *this;
}



inline bool Matrix4f::operator==(const Matrix4f& n) const
{
    return (m[0] == n[0])   && (m[1] == n[1])   && (m[2] == n[2])   && (m[3] == n[3]) &&
           (m[4] == n[4])   && (m[5] == n[5])   && (m[6] == n[6])   && (m[7] == n[7]) &&
           (m[8] == n[8])   && (m[9] == n[9])   && (m[10] == n[10]) && (m[11] == n[11]) &&
           (m[12] == n[12]) && (m[13] == n[13]) && (m[14] == n[14]) && (m[15] == n[15]);
}



inline bool Matrix4f::operator!=(const Matrix4f& n) const
{
    return (m[0] != n[0])   || (m[1] != n[1])   || (m[2] != n[2])   || (m[3] != n[3]) ||
           (m[4] != n[4])   || (m[5] != n[5])   || (m[6] != n[6])   || (m[7] != n[7]) ||
           (m[8] != n[8])   || (m[9] != n[9])   || (m[10] != n[10]) || (m[11] != n[11]) ||
           (m[12] != n[12]) || (m[13] != n[13]) || (m[14] != n[14]) || (m[15] != n[15]);
}



inline float Matrix4f::operator[](int index) const
{
    return m[index];
}



inline float& Matrix4f::operator[](int index)
{
    return m[index];
}

inline Matrix4f& Matrix4f::operator=(const Matrix4f& rhs)
{
	set( rhs.m );
    return *this;
}



inline Matrix4f operator-(const Matrix4f& rhs)
{
    return Matrix4f(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}



inline Matrix4f operator*(float s, const Matrix4f& rhs)
{
    return Matrix4f(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}



inline Vector4f operator*(const Vector4f& v, const Matrix4f& m)
{
    return Vector4f(v.x*m[0] + v.y*m[4] + v.z*m[8] + v.w*m[12],  v.x*m[1] + v.y*m[5] + v.z*m[9] + v.w*m[13],  v.x*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14], v.x*m[3] + v.y*m[7] + v.z*m[11] + v.w*m[15]);
}



inline Vector3f operator*(const Vector3f& v, const Matrix4f& m)
{
    return Vector3f(v.x*m[0] + v.y*m[4] + v.z*m[8],  v.x*m[1] + v.y*m[5] + v.z*m[9],  v.x*m[2] + v.y*m[6] + v.z*m[10]);
}



inline std::ostream& operator<<(std::ostream& os, const Matrix4f& m)
{
    os << "(" << m[0]  << ",\t" << m[1]  << ",\t" << m[2]  <<  ",\t" << m[3] << ")\n"
       << "(" << m[4]  << ",\t" << m[5]  << ",\t" << m[6]  <<  ",\t" << m[7] << ")\n"
       << "(" << m[8]  << ",\t" << m[9]  << ",\t" << m[10] <<  ",\t" << m[11] << ")\n"
       << "(" << m[12] << ",\t" << m[13] << ",\t" << m[14] <<  ",\t" << m[15] << ")\n";
    return os;
}
// END OF Matrix4f INLINE //////////////////////////////////////////////////////
#endif
