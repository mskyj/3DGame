#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "Vector.h"
#include <MyEffekseerLib.h>
#include <iostream>
using namespace::std;

#define EFFECT_MAX_NUM 32

class Effect
{
private:
	::Effekseer::Manager* manager;
	::EffekseerRenderer::Renderer* renderer;
	::Effekseer::Effect* effect[EFFECT_MAX_NUM];
	int num;
	int handle[EFFECT_MAX_NUM];
	int time[EFFECT_MAX_NUM];
	Vector3f position;

public:
	Effect();
	~Effect();
	bool Init();
	bool Load( const EFK_CHAR* path );
	void Release();
	void Play( int index, Vector3f pos );
	void Stop( int index );
	void Translate( int index, Vector3f pos );
	void Render();
	void SetCamera(float x, float y, float z, float cx, float cy, float cz);
};



#endif