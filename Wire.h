#ifndef _WIRE_H_
#define _WIRE_H_

#include "Model.h"
#include "Player.h"
#include "Kinect.h"
#include "Sound.h"

enum WIRE_STATUS{
	NO_TARGET=0, TARGET, ON_SHOT, WIRE_FIXED, TAKE_UP, CIRCLE
};

class WireModel : public Model
{
private:
	Model human;
	Vector3f lPos, rPos;
	int* humanBoneIndex;

public:
	WireModel();
	~WireModel();
	void SetHumanModel(Model* humanModel);
	void Update();
	void GetPos(Vector3f* new_lPos, Vector3f* new_rPos);
};

class Wire
{
private:
	WireModel model;
	Sound shotSound;
	Sound pullSound;
	Vector3f worldPos[2];
	float len[2];
	float speed[2];
	WIRE_STATUS status[2];
	Vector3f predictedPos[2];
	GLuint texture;

	Vector3f centerPos;
	float circleMaxTheta;
	float circleTheta;
	Vector3f rotateAxis;
	float circleSpeed;

	Player* player;
	Kinect* kinect;
public:
	Wire();
	~Wire();

	void Reset(){ status[0]=NO_TARGET; status[1]=NO_TARGET; }
	bool LoadModel( const char* path, GLuint shaderProgram );
	void Release();
	void SetHuman( Player* human );
	void SetKinect( Kinect* k );

	void Shot();
	void Update();

	void RenderModel();
	void RenderWire();

	WIRE_STATUS GetStatus( int index ){ return status[index]; }
	void SetStatus( int index, WIRE_STATUS new_status ){ status[index] = new_status; }
};

#endif