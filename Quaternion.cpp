#include"Quaternion.h"

Quaternion QuaternionSlerp( Quaternion q1, Quaternion q2, float t )
{
	float qr = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	float ss = 1.0f - qr * qr,sp;
 
	if ( ss <= 0.0 || (sp = sqrt(ss)) == 0.0 ) 
		return Quaternion( q1.x, q1.y, q1.z, q1.w );
	else
	{
		float ph = acos(qr);
		float pt = ph * t;
		float t1 = sin(pt) / sp;
		float t0 = sin(ph - pt) / sp;
   
		return Quaternion(
			q1.x * t0 + q2.x * t1,
			q1.y * t0 + q2.y * t1,
			q1.z * t0 + q2.z * t1,
			q1.w * t0 + q2.w * t1
			);
	}
}

Matrix4f QuatToMatrix( Quaternion quat )
{
    float qw, qx, qy, qz;
    float xx, yy, zz;
    float xy, yz, zx;
    float wx, wy, wz;

    qw = quat.w; qx = quat.x; qy = quat.y; qz = quat.z;

    xx = 2.0f * qx * qx;
    yy = 2.0f * qy * qy;
    zz = 2.0f * qz * qz;

    xy = 2.0f * qx * qy;
    yz = 2.0f * qy * qz;
    zx = 2.0f * qz * qx;
        
    wx = 2.0f * qw * qx;
    wy = 2.0f * qw * qy;
    wz = 2.0f * qw * qz;

	return Matrix4f(
		1.0f - yy - zz, xy + wz, zx - wy, 0.0f,
		xy - wz, 1.0f - zz - xx, yz + wx, 0.0f,
		zx + wy, yz - wx, 1.0f - xx - yy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );
}

Quaternion MatToQuat(Matrix4f mat){
	Quaternion q;

	float s;
	float m[3][3];
	m[0][0] = mat.get()[0];
	m[0][1] = mat.get()[1];
	m[0][2] = mat.get()[2];
	m[1][0] = mat.get()[4];
	m[1][1] = mat.get()[5];
	m[1][2] = mat.get()[6];
	m[2][0] = mat.get()[8];
	m[2][1] = mat.get()[9];
	m[2][2] = mat.get()[10];

	float tr = m[0][0] + m[1][1] + m[2][2] + 1.0f;
	if (tr >= 1.0f) {
		s = 0.5f / sqrt(tr);
		q.w= 0.25f / s;
		q.x= (m[1][2] - m[2][1]) * s;
		q.y= (m[2][0] - m[0][2]) * s;
		q.z= (m[0][1] - m[1][0]) * s;
		return q;
	}else{
		float max;
		if(m[1][1] > m[2][2]){
			max = m[1][1];
		}else{
			max = m[2][2];
		}
       
		if (max < m[0][0]) {
			s = sqrt(m[0][0] - (m[1][1] + m[2][2]) + 1.0f);
			float x = s * 0.5f;
			s = 0.5f / s;
			q.x= x;
			q.y= (m[0][1] + m[1][0]) * s;
			q.z= (m[2][0] + m[0][2]) * s;
			q.w= (m[1][2] - m[2][1]) * s;
			return q;
		}else if (max == m[1][1]) {
            s = sqrt(m[1][1] - (m[2][2] + m[0][0]) + 1.0f);
            float y = s * 0.5f;
            s = 0.5f / s;
			q.x= (m[0][1] + m[1][0]) * s;
			q.y= y;
			q.z= (m[1][2] + m[2][1]) * s;
			q.w= (m[2][0] - m[0][2]) * s;
			return q;
		}else{
			s = sqrt(m[2][2] - (m[0][0] + m[1][1]) + 1.0f);
			float z = s * 0.5f;
			s = 0.5f / s;
			q.x= (m[2][0] + m[0][2]) * s;
			q.y= (m[1][2] + m[2][1]) * s;
			q.z= z;
			q.w= (m[0][1] - m[1][0]) * s;
			return q;
		}
	}
}

Quaternion EulerToQuat( Vector3f euler )
{
	float	xRadian = euler.x * 0.5f;
	float	yRadian = euler.y * 0.5f;
	float	zRadian = euler.z * 0.5f;
	float	sinX = sinf( xRadian );
	float	cosX = cosf( xRadian );
	float	sinY = sinf( yRadian );
	float	cosY = cosf( yRadian );
	float	sinZ = sinf( zRadian );
	float	cosZ = cosf( zRadian );

	// XYZ
	return Quaternion(  sinX * cosY * cosZ - cosX * sinY * sinZ,
						cosX * sinY * cosZ + sinX * cosY * sinZ,
						cosX * cosY * sinZ - sinX * sinY * cosZ,
						cosX * cosY * cosZ + sinX * sinY * sinZ );
}

Vector3f QuatToEuler( Quaternion quat )
{
	// XYZŽ²‰ñ“]‚ÌŽæ“¾
	// Y‰ñ“]‚ð‹‚ß‚é
	float	x2 = quat.x + quat.x;
	float	y2 = quat.y + quat.y;
	float	z2 = quat.z + quat.z;
	float	xz2 = quat.x * z2;
	float	wy2 = quat.w * y2;
	float	temp = -(xz2 - wy2);

	// Œë·‘Îô
	if( temp >= 1.f ){ temp = 1.f; }
	else if( temp <= -1.f ){ temp = -1.f; }

	float	yRadian = asinf(temp);

	// ‘¼‚Ì‰ñ“]‚ð‹‚ß‚é
	float	xx2 = quat.x * x2;
	float	xy2 = quat.x * y2;
	float	zz2 = quat.z * z2;
	float	wz2 = quat.w * z2;

	Vector3f angle;

	if( yRadian < 3.1415926f * 0.5f )
	{
		if( yRadian > -3.1415926f * 0.5f )
		{
			float	yz2 = quat.y * z2;
			float	wx2 = quat.w * x2;
			float	yy2 = quat.y * y2;
			angle.x = atan2f( (yz2 + wx2), (1.f - (xx2 + yy2)) );
			angle.y = yRadian;
			angle.z = atan2f( (xy2 + wz2), (1.f - (yy2 + zz2)) );
		}
		else
		{
			angle.x = -atan2f( (xy2 - wz2), (1.f - (xx2 + zz2)) );
			angle.y = yRadian;
			angle.z = 0.f;
		}
	}
	else
	{
		angle.x = atan2f( (xy2 - wz2), (1.f - (xx2 + zz2)) );
		angle.y = yRadian;
		angle.z = 0.f;
	}

	return angle;
}