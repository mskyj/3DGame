#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include<math.h>

#include"Matrix.h"

class Quaternion
{
public:
	struct{
		float x, y, z, w;
	};

	//! constructors
	Quaternion(): x(0.0f), y(0.0f), z(0.0f),w(1.0f) {}
	Quaternion(float qx, float qy, float qz, float qw): x(qx), y(qy), z(qz),w(qw) {}

	Quaternion& normalize();

	Quaternion& operator-();
	Quaternion operator*( const float f ) const;
	Quaternion operator*(const Quaternion& q) const;
	Quaternion& operator*=(const Quaternion& q);
	Quaternion& operator=(const Quaternion& q);
	bool		operator==(const Quaternion& q) const;
	bool		operator!=(const Quaternion& q) const;

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& m);
};

inline Quaternion& Quaternion::normalize(){
	float xxyyzzww = x*x + y*y + z*z + w*w;
	x /= xxyyzzww; y /= xxyyzzww; z /= xxyyzzww; w /= xxyyzzww;
	return *this;
}

inline Quaternion& Quaternion::operator-(){
	x = -x; y = -y; z = -z; w = - w;
	return *this;
}

inline Quaternion Quaternion::operator *( const float f ) const{
	return Quaternion( x*f, y*f, z*f, w*f );
}

inline Quaternion Quaternion::operator*(const Quaternion& q) const{
	float pw, px, py, pz;
    float qw, qx, qy, qz;

    pw = w; px = x; py = y; pz = z;
    qw = q.w; qx = q.x; qy = q.y; qz = q.z;

	return Quaternion(
		pw * qx + px * qw + py * qz - pz * qy,
    	pw * qy - px * qz + py * qw + pz * qx,
		pw * qz + px * qy - py * qx + pz * qw,
		pw * qw - px * qx - py * qy - pz * qz);
}

inline Quaternion& Quaternion::operator *=(const Quaternion& q)		{			
	float pw, px, py, pz;
    float qw, qx, qy, qz;

    pw = w; px = x; py = y; pz = z;
    qw = q.w; qx = q.x; qy = q.y; qz = q.z;

	*this = Quaternion(
		pw * qx + px * qw + py * qz - pz * qy,
    	pw * qy - px * qz + py * qw + pz * qx,
		pw * qz + px * qy - py * qx + pz * qw,
		pw * qw - px * qx - py * qy - pz * qz);
	return *this;
}

inline Quaternion& Quaternion::operator=(const Quaternion& q){
	x = q.x; y = q.y; z = q.z; w = q.w;
	return *this;
}

inline bool Quaternion::operator==( const Quaternion& q ) const{
	return ( x == q.x ) && ( y == q.y ) && ( z == q.z ) && ( w == q.w );
}

inline bool Quaternion::operator!=( const Quaternion& q ) const{
	return ( x != q.x ) || ( y != q.y ) || ( z != q.z ) || ( w != q.w );
}

inline std::ostream& operator<<(std::ostream& os, const Quaternion& q){
	os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
	return os;
}

Quaternion QuaternionSlerp( Quaternion q1, Quaternion q2, float t );
Matrix4f QuatToMatrix( Quaternion quat );
Vector3f QuatToEuler( Quaternion quat );
Quaternion EulerToQuat( Vector3f euler );
Quaternion MatToQuat(Matrix4f mat);


#endif