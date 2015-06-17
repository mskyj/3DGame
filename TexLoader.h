#ifndef _TEX_LOADER_H_
#define _TEX_LOADER_H_

#include"lodepng.h"
#include<MyFreeglutLib.h>
#include<MyOpencvLib.h>
#include<Windows.h>
#include<iostream>
using namespace std;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

class Texture
{
private:
	int width,height,nChannels;
	GLubyte *image;
	char filepath[128];
public:
	Texture();
	~Texture();
	bool Load( char* path );
	bool CreateTexture( GLuint* imageTex );
	void Release();
};

#endif