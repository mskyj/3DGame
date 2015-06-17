#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Model.h"
#include "Map.h"
#include "Sound.h"

#define HUMAN_SIZE 1.6f

enum DIRECT{
	AHEAD, LEFT, RIGHT, BEHIND, UP, HIGHUP
};

class Player
{
private:
	Model model;
	Sound stepSound;
	Vector3f pos;
	float angleXZ;
	Vector3f moveVec;
	int eyeIndex[2];
	int headIndex;
	int armIndex[2];
	int ankleIndex[2];
	Vector3f defAnklePos[2];
	float* defDepth;
	float* depthData;
public:
	Player();
	~Player();
	void Init();
	void Reset();

	bool LoadModel( const char* path, GLuint shaderProgram, bool useKinect );
	int LoadAnim( const char* path );
	int LoadPose( const char* path );
	void Release();
	void SetDepthData( float* data ){ depthData = data; }

	void PrepareAnim( int index );
	void StartAnim( int index );
	void StopAnim( int index );
	void SetAnimSpeed( int index, float spped );

	void SetSpeed( Vector3f vec );
	void SetSpeed( float speed );
	void AddSpeed( Vector3f vec );

	int Move( Vector3f vec );
	int Move();
	void Move( DIRECT direct );
	void Jump();

	void Render();

	bool IsStep();
	void PlayStepSound();

	void UpdateByKinect( Quaternion* quat );
	void RotateXZ( float angle );
	
	bool IsInAir();
	Model* GetModel();
	Vector3f GetPos();
	float GetAngleXZ();
	Vector3f GetSpeed();
	Vector3f GetEyePos();
	Vector3f GetEyeVec();
	Vector3f GetArmPos( int index );
};


#endif