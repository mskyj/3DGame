#include "Map.h"

// マップデータ
float mapMinX;
float mapMaxX;
float mapMinZ;
float mapMaxZ;
float mapInterval;
float* depthData = NULL;
float* depthData2 = NULL;

int xNum;
int zNum;

float GetFloat(char* src)
{
	float result;
	memcpy_s(&result, 4, src, 4);
	return result;
}

bool LoadDepthData( const char* path )
{
	FILE *fp;
	if( fopen_s(&fp, path, "rb")!=0 )
	{
		cout << "GameManager Error : LoadDepthData() can't open file : data\\depth.dat" << endl;
		return false;
	}

	// read file to buffer
	struct stat stat_buf;
	fstat( _fileno(fp), &stat_buf );
	int fileSize = stat_buf.st_size;
	char* src = new char[ fileSize ];
	fread_s( src, fileSize, fileSize, 1, fp );
	fclose(fp);
	char* pointer = src;

	mapMinX = GetFloat(pointer);
	pointer += 4;
	mapMaxX = GetFloat(pointer);
	pointer += 4;
	mapMinZ = GetFloat(pointer);
	pointer += 4;
	mapMaxZ = GetFloat(pointer);
	pointer += 4;
	mapInterval = GetFloat(pointer);
	pointer += 4;

	xNum = (int)((mapMaxX-mapMinX)/mapInterval);
	zNum = (int)((mapMaxZ-mapMinZ)/mapInterval);
	if( xNum<=0 || zNum <=0 )
	{
		cout << "GameManager Error : LoadDepthData() map data is not correct" << endl;
		delete[] src;
		return false;
	}

	depthData = new float[xNum*zNum];
	depthData2 = new float[xNum*zNum];
	for( int z=0; z<zNum; z++ )
		for( int x=0; x<xNum; x++ )
		{
			depthData[x+z*xNum] = GetFloat(pointer);
			depthData2[x+z*xNum] = depthData[x+z*xNum];
			pointer += 4;
		}
	// 修正
	for( float dz = -4.0f; dz <= 4.0f; dz += mapInterval )
		for( float dx = -4.0f; dx <= 4.0f; dx += mapInterval )
		{
			int i = (int)((72.0f-mapMinX+dx)/mapInterval);
			int j = (int)((-264.0f-mapMinZ+dz)/mapInterval);
			int index = i + j*xNum;
			depthData[index] = -10.0f;

			i = (int)((75.0f-mapMinX+dx)/mapInterval);
			j = (int)((-305.0f-mapMinZ+dz)/mapInterval);
			index = i + j*xNum;
			depthData[index] = -10.0f;

			i = (int)((185.0f-mapMinX+dx)/mapInterval);
			j = (int)((-260.0f-mapMinZ+dz)/mapInterval);
			index = i + j*xNum;
			depthData[index] = -10.0f;

			i = (int)((187.0f-mapMinX+dx)/mapInterval);
			j = (int)((-300.0f-mapMinZ+dz)/mapInterval);
			index = i + j*xNum;
			depthData[index] = -10.0f;
		}

	delete[] src;

	return true;
}

void DeleteDepthData()
{
	if( depthData )
		delete[] depthData;
	if( depthData2 )
		delete[] depthData2;
}

void ResetDepth2()
{
	for( int z=0; z<zNum; z++ )
		for( int x=0; x<xNum; x++ )
			depthData2[x+z*xNum] = depthData[x+z*xNum];
}

void UpdateDepth2( float x, float z, float range, float height )
{
	int i = (int)((x-mapMinX)/mapInterval);
	int j = (int)((z-mapMinZ)/mapInterval);
	int index = i + j*xNum;
	float dep = depthData[index];

	for( float dz = -range/2.0f; dz <= range/2.0f; dz += mapInterval )
		for( float dx = -range/2.0f; dx <= range/2.0f; dx += mapInterval )
		{
			i = (int)((x-mapMinX+dx)/mapInterval);
			j = (int)((z-mapMinZ+dz)/mapInterval);
			index = i + j*xNum;
			if( abs(depthData[index]-dep)<2.0f )
				depthData2[index] = depthData[index] + height;
		}
}

float GetDepthData( float x, float z )
{
	float depthMax = -1000.0f;

	if( mapMinX >= x - 0.5f || mapMaxX <= x + 0.5f
		|| mapMinZ >= z - 0.5f || mapMaxZ <= z + 0.5f )
		return -1000.0f;

	for( float dz = -0.5f; dz <= 0.5f; dz += 0.5f )
		for( float dx = -0.5f; dx <= 0.5f; dx += 0.5f )
		{
			int i = (int)((x-mapMinX+dx)/mapInterval);
			int j = (int)((z-mapMinZ+dz)/mapInterval);
			int index = i + j*xNum;
			if( depthMax < depthData[index] )
				depthMax = depthData[index];
		}

	return depthMax;
}

float GetDepthData2( float x, float z )
{
	float depthMax = -1000.0f;

	if( mapMinX >= x - 0.5f || mapMaxX <= x + 0.5f
		|| mapMinZ >= z - 0.5f || mapMaxZ <= z + 0.5f )
		return -1000.0f;

	for( float dz = -0.5f; dz <= 0.5f; dz += 0.5f )
		for( float dx = -0.5f; dx <= 0.5f; dx += 0.5f )
		{
			int i = (int)((x-mapMinX+dx)/mapInterval);
			int j = (int)((z-mapMinZ+dz)/mapInterval);
			int index = i + j*xNum;
			if( depthMax < depthData2[index] )
				depthMax = depthData2[index];
		}

	return depthMax;
}

float GetAverageDepth( float x, float z, float range )
{
	int num = 0;
	float sum = 0.0f;
	for( float dz = -range/2.0f; dz <= range/2.0f; dz += mapInterval )
		for( float dx = -range/2.0f; dx <= range/2.0f; dx += mapInterval )
		{
			int i = (int)((x-mapMinX+dx)/mapInterval);
			int j = (int)((z-mapMinZ+dz)/mapInterval);
			int index = i + j*xNum;
			sum += depthData2[index];
			num++;
		}
	return sum/(float)num;
}

int LineCollisionDetect(Vector3f* save, Vector3f start, Vector3f end)
{
	Vector3f current = start;
	int ex = (int)(floor(end.x/mapInterval));
	int ez = (int)(floor(end.z/mapInterval));
	if( (end-start).length() < 0.5f )
		return 0;
	Vector3f angleVec = (end-start).normalize();
	float angleXZ;
	if( abs(angleVec.z) < 0.01f )
	{
		if(angleVec.x>0.0f)
			angleXZ = 1.5708f;
		else
			angleXZ = 4.7124f;
	}
	else
	{
		angleXZ = atan2(angleVec.x,angleVec.z);
		if(angleXZ < 0.0f)
			angleXZ += 6.28318f;
	}

	int loopCount = 0;
	while( 1 )
	{
		float d;
		if( angleVec.z>0.0f && angleVec.x>0.0f )
			d = GetDepthData2(current.x, current.z);
		else if( angleVec.z>0.0f )
			d = GetDepthData2(current.x-0.01f, current.z);
		else if( angleVec.x>0.0f )
			d = GetDepthData2(current.x, current.z-0.01f);
		else
			d = GetDepthData2(current.x-0.01f, current.z-0.01f);

		if( d < -100.0f )
			return 0;
		else if( d >= current.y )
		{
			*save = current;
			return 1;
		}

		int sx = (int)(floor(current.x/mapInterval));
		int sz = (int)(floor(current.z/mapInterval));

		if( angleVec.z<0.0f )
			sz--;
		if( angleVec.x<0.0f )
			sx--;
			
		if( sx==ex && sz== ez )
			return 0;

		float x0,x1,z0,z1;
		z0 = (float)sz*mapInterval;
		z1 = (float)(sz+1)*mapInterval;
		x0 = (float)sx*mapInterval;
		x1 = (float)(sx+1)*mapInterval;

		Vector3f next;
		float angle;
		if( angleVec.z >0.0f && angleVec.x >0.0f )
		{
			if( z1-current.z < 0.01f )
				angle = 1.5708f;
			else
				angle = atan2(x1-current.x, z1-current.z);
			if( abs( angleXZ-angle )<0.01f )
			{
				next.x = x1;
				next.z = z1;
			}
			else if( angleXZ < angle )
			{
				next.x = current.x + (z1-current.z)*tan(angleXZ);
				next.z = z1;
			}
			else
			{
				next.x = x1;
				if( angleXZ == 1.5708f )
					next.z = current.z;
				else
					next.z = (x1-current.x)/tan(angleXZ) + current.z;
			}
		}
		else if( angleVec.z > 0.0f )
		{
			if( z1-current.z < 0.01f )
				angle = 4.7124f;
			else
				angle = atan2(x0-current.x, z1-current.z)+6.28318f;
			if( abs( angleXZ-angle )<0.01f )
			{
				next.x = x0;
				next.z = z1;
			}
			else if( angleXZ > angle )
			{
				next.x = current.x + (z1-current.z)*tan(angleXZ);
				next.z = z1;
			}
			else
			{
				next.x = x0;
				if( angleXZ == 4.7124f )
					next.z = current.z;
				else
					next.z = (x0-current.x)/tan(angleXZ) + current.z;
			}
		}
		else if( angleVec.x > 0.0f )
		{
			if( current.z-z0 < 0.01f )
				angle = 1.5708f;
			else
				angle = atan2(x1-current.x, z0-current.z);
			if( abs( angleXZ-angle )<0.01f )
			{
				next.x = x1;
				next.z = z0;
			}
			else if( angleXZ > angle )
			{
				next.x = current.x + (z0-current.z)*tan(angleXZ);
				next.z = z0;
			}
			else
			{
				next.x = x1;
				if( angleXZ == 1.5708f )
					next.z = current.z;
				else
					next.z = (x1-current.x)/tan(angleXZ) + current.z;
			}
		}
		else
		{
			if( current.z-z0 < 0.01f )
				angle = 4.7124f;
			else
				angle = atan2(x1-current.x, z0-current.z)+6.28318f;
			if( abs( angleXZ-angle )<0.01f )
			{
				next.x = x0;
				next.z = z0;
			}
			else if( angleXZ > angle )
			{
				next.x = current.x + (z0-current.z)*tan(angleXZ);
				next.z = z0;
			}
			else
			{
				next.x = x0;
				if( angleXZ == 4.7124f )
					next.z = current.z;
				else
					next.z = (x0-current.x)/tan(angleXZ) + current.z;
			}
		}

		float r;
		if( abs(end.x - current.x)< 0.01f )
			r = (next.z - current.z)/(end.z - current.z);
		else
			r = (next.x - current.x)/(end.x - current.x);
		next.y = current.y + (end.y - current.y)*r;

		if( r < 0.0f )
			return 0;

		if( next.y > d )
		{
			if( current == next )
				return 0;

			current = next;
		}
		else
		{
			float rate = (next.y - d)/(next.y - current.y);
			Vector3f new_pos;
			new_pos.x = (next.x - current.x)*rate + current.x;
			new_pos.y = d;
			new_pos.z = (next.z - current.z)*rate + current.z;
			*save = new_pos;
			return 1;
		}

		loopCount++;
		if( loopCount==100 )
		{
			return 0;
		}
	}
}

int CrossDetect(Vector2f a, Vector2f b, Vector2f c, Vector2f d)
{
	float acx = c.x - a.x;
	float acy = c.y - a.y;
	float bunbo = (b.x-a.x)*(d.y-c.y) - (b.y-a.y)*(d.x-c.y);
	if( abs(bunbo)<FLT_EPSILON )
		return 0;

	float r = ((d.y-c.y)*acx - (d.x-c.x)*acy) / bunbo;
	float s = ((b.y-a.y)*acx - (b.x-a.x)*acy) / bunbo;

	if( r>=0.0f && r<=1.0f && s>=0.0f && s<=1.0f )
		return 1;
	else
		return 0;
}