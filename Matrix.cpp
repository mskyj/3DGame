#include <cmath>
#include <algorithm>
#include "Matrix.h"

const float DEG2RAD = 3.141593f / 180;


Matrix4f& Matrix4f::inverse()
{
	Matrix4f inv = Matrix4f();
	float buf;
	int i,j,k;

	//‘|‚«o‚µ–@
	for(i=0;i<4;i++){
		buf=1/m[i*5];
		for(j=0;j<4;j++){
			m[i*4+j] *= buf;
			inv.m[i*4+j] *= buf;
		}
		for(j=0;j<4;j++){
			if(i!=j){
				buf = m[j*4+i];
				for(k=0;k<4;k++){
					m[j*4+k] -= m[i*4+k]*buf;
					inv.m[j*4+k] -= inv.m[i*4+k]*buf;
				}
			}
		}
	}
	*this = inv;

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// transpose 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Matrix4f& Matrix4f::transpose()
{
    std::swap(m[1],  m[4]);
    std::swap(m[2],  m[8]);
    std::swap(m[3],  m[12]);
    std::swap(m[6],  m[9]);
    std::swap(m[7],  m[13]);
    std::swap(m[11], m[14]);

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// return determinant of 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
float Matrix4f::getDeterminant()
{
    return m[0] * getCofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]) -
           m[1] * getCofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]) +
           m[2] * getCofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]) -
           m[3] * getCofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);
}



///////////////////////////////////////////////////////////////////////////////
// compute cofactor of 3x3 minor matrix without sign
// input params are 9 elements of the minor matrix
// NOTE: The caller must know its sign.
///////////////////////////////////////////////////////////////////////////////
float Matrix4f::getCofactor(float m0, float m1, float m2,
                           float m3, float m4, float m5,
                           float m6, float m7, float m8)
{
    return m0 * (m4 * m8 - m5 * m7) -
           m1 * (m3 * m8 - m5 * m6) +
           m2 * (m3 * m7 - m4 * m6);
}



///////////////////////////////////////////////////////////////////////////////
// translate this matrix by (x, y, z)
///////////////////////////////////////////////////////////////////////////////
Matrix4f& Matrix4f::translate(const Vector3f& v)
{
    return translate(v.x, v.y, v.z);
}

Matrix4f& Matrix4f::translate(float x, float y, float z)
{
    m[0] += m[12]*x;   m[1] += m[13]*x;   m[2] += m[14]*x;   m[3] += m[15]*x;
    m[4] += m[12]*y;   m[5] += m[13]*y;   m[6] += m[14]*y;   m[7] += m[15]*y;
    m[8] += m[12]*z;   m[9] += m[13]*z;   m[10]+= m[14]*z;   m[11]+= m[15]*z;
    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// uniform scale
///////////////////////////////////////////////////////////////////////////////
Matrix4f& Matrix4f::scale(float s)
{
    return scale(s, s, s);
}

Matrix4f& Matrix4f::scale(float x, float y, float z)
{
    m[0] = m[0]*x;   m[1] = m[1]*x;   m[2] = m[2]*x;   m[3] = m[3]*x;
    m[4] = m[4]*y;   m[5] = m[5]*y;   m[6] = m[6]*y;   m[7] = m[7]*y;
    m[8] = m[8]*z;   m[9] = m[9]*z;   m[10]= m[10]*z;  m[11]= m[11]*z;
    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// build a rotation matrix with given angle(degree) and rotation axis, then
// multiply it with this object
///////////////////////////////////////////////////////////////////////////////
Matrix4f& Matrix4f::rotate(float angle, const Vector3f& axis)
{
    return rotate(angle, axis.x, axis.y, axis.z);
}

Matrix4f& Matrix4f::rotate(float angle, float x, float y, float z)
{
    float c = cosf(angle * DEG2RAD);    // cosine
    float s = sinf(angle * DEG2RAD);    // sine
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float yy = y * y;
    float yz = y * z;
    float zz = z * z;

    // build rotation matrix
    Matrix4f m;
    m[0] = xx * (1 - c) + c;
    m[1] = xy * (1 - c) - z * s;
    m[2] = xz * (1 - c) + y * s;
    m[3] = 0;
    m[4] = xy * (1 - c) + z * s;
    m[5] = yy * (1 - c) + c;
    m[6] = yz * (1 - c) - x * s;
    m[7] = 0;
    m[8] = xz * (1 - c) - y * s;
    m[9] = yz * (1 - c) + x * s;
    m[10]= zz * (1 - c) + c;
    m[11]= 0;
    m[12]= 0;
    m[13]= 0;
    m[14]= 0;
    m[15]= 1;

    // multiply it
    *this = m * (*this);

    return *this;
}

Matrix4f& Matrix4f::rotateX(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7],
          m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];

    m[4] = m4 * c + m8 *-s;
    m[5] = m5 * c + m9 *-s;
    m[6] = m6 * c + m10*-s;
    m[7] = m7 * c + m11*-s;
    m[8] = m4 * s + m8 * c;
    m[9] = m5 * s + m9 * c;
    m[10]= m6 * s + m10* c;
    m[11]= m7 * s + m11* c;

    return *this;
}

Matrix4f& Matrix4f::rotateY(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
          m8 = m[8], m9 = m[9], m10= m[10], m11= m[11];

    m[0] = m0 * c + m8 * s;
    m[1] = m1 * c + m9 * s;
    m[2] = m2 * c + m10* s;
    m[3] = m3 * c + m11* s;
    m[8] = m0 *-s + m8 * c;
    m[9] = m1 *-s + m9 * c;
    m[10]= m2 *-s + m10* c;
    m[11]= m3 *-s + m11* c;

    return *this;
}

Matrix4f& Matrix4f::rotateZ(float angle)
{
    float c = cosf(angle * DEG2RAD);
    float s = sinf(angle * DEG2RAD);
    float m0 = m[0], m1 = m[1], m2 = m[2],  m3 = m[3],
          m4 = m[4], m5 = m[5], m6 = m[6],  m7 = m[7];

    m[0] = m0 * c + m4 *-s;
    m[1] = m1 * c + m5 *-s;
    m[2] = m2 * c + m6 *-s;
    m[3] = m3 * c + m7 *-s;
    m[4] = m0 * s + m4 * c;
    m[5] = m1 * s + m5 * c;
    m[6] = m2 * s + m6 * c;
    m[7] = m3 * s + m7 * c;

    return *this;
}
