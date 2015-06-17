#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Player.h"

//#define _TEST_

#define TITLE_NUM 10

#define ENEMY_WARN_TIME 30
#define ENEMY_SPEED 1.0f
#define ENEMY_VIEW_RANGE 1.0f
#define ENEMY_STEP_RANGE 1.2f

enum ENEMY_STATUS{
	WALK, RUN, WARN, STOP, BATTLE
};

class Enemy
{
private:
	Model model;
	Sound alertSound;
	int type;
	float size;
	ENEMY_STATUS status;
	Vector3f pos;
	float angleXZ;
	Vector3f moveVec;
	float speed;

	int headIndex;
	int warnningTime;

	bool stepOnPlayerFlag;

	Vector3f titlePos[TITLE_NUM];
	float titleAngle[TITLE_NUM];

	Player* player;
public:
	Enemy();
	~Enemy();
	void Init();

	bool LoadModel( ALuint buffer, GLuint shaderProgram  );
	void SetPlayer( Player* p );
	int LoadAnim( int index );
	void Release();

	void PrepareAnim( int index );
	void StartAnim( int index, float speed );
	void StopAnim( int index );

	void SetSpeed( Vector3f vec );
	void SetSpeed( float speed );
	void AddSpeed( Vector3f vec );
	int Move( Vector3f vec );
	int Move();
	void RotateToAngle( float angle );
	void FixAngle( float angle );

	void Render( bool titleFlag = false );
	void Update();

	Vector3f GetPos(){ return pos; }
	float GetSize(){ return size; }
	ENEMY_STATUS GetStatus(){ return status; }
	bool IsStepOn(){ return stepOnPlayerFlag; }
	int GetType(){ return type; }
	void SetPos( Vector3f p ){ pos = p; }
	void SetAngleXZ( float angle ){ angleXZ = angle; }
};

#endif