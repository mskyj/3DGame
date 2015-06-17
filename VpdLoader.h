#ifndef _VPD_LOADER_H_
#define _VPD_LOADER_H_

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<iostream>
using namespace::std;
#include "Quaternion.h"

struct VpdBone
{
	char name[32];
	Vector3f trans;
	Quaternion rot;
	int index;
};

class Vpd
{
private:
public:
	int boneNum;
	VpdBone* bone;
	bool useFlag;

	Vpd();
	~Vpd();
	bool Load( const char* path );
	void Release();
};
#endif