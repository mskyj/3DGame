#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
using namespace::std;
#include "Vector.h"

// マップデータ
extern float mapMinX;
extern float mapMaxX;
extern float mapMinZ;
extern float mapMaxZ;
extern float mapInterval;

bool LoadDepthData( const char* path );
float GetDepthData( float x, float z );
void DeleteDepthData();

void ResetDepth2();
void UpdateDepth2( float x, float z, float range, float height );
float GetDepthData2( float x, float z );
float GetAverageDepth( float x, float z, float range );

int LineCollisionDetect(Vector3f* save, Vector3f start, Vector3f end);
int CrossDetect(Vector2f a, Vector2f b, Vector2f c, Vector2f d);

#endif