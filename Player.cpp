#include "Player.h"

Player::Player()
{
	Init();
}

Player::~Player()
{
}

void Player::Init()
{
	pos.set(0.0f, -9.8f, -310.0f);
	angleXZ=0.0f;
}

void Player::Reset()
{
	Init();
	StopAnim(0);
}

bool Player::LoadModel( const char* path, GLuint shaderProgram, bool useKinect )
{
	if( !model.Load(path,shaderProgram,HUMAN_SIZE) )
		return false;

	if( useKinect )
		model.EnableKinect();

	eyeIndex[0] = model.GetBoneIndex("ç∂ñ⁄");
	eyeIndex[1] = model.GetBoneIndex("âEñ⁄");
	headIndex = model.GetBoneIndex("ì™");
	armIndex[0] = model.GetBoneIndex("ç∂òr");
	armIndex[1] = model.GetBoneIndex("âEòr");
	ankleIndex[0] = model.GetBoneIndex("ç∂ë´éÒ");
	ankleIndex[1] = model.GetBoneIndex("âEë´éÒ");
	if( eyeIndex[0]==-1 || eyeIndex[1]==-1 || headIndex==-1 || armIndex[0]==-1 || armIndex[1]==-1 || ankleIndex[0]==-1 || ankleIndex[1]==-1 )
		return false;
	model.GetBonePos(ankleIndex[0], &defAnklePos[0]);
	model.GetBonePos(ankleIndex[1], &defAnklePos[1]);

	if( stepSound.LoadWave("sound\\step.wav") == AL_FALSE )
		return false;

	return true;
}

int Player::LoadAnim( const char* path )
{
	return model.LoadAnim(path);
}

int Player::LoadPose( const char* path )
{
	return model.LoadPose(path);
}

void Player::Release()
{
	model.Release();
	stepSound.Release();
}

void Player::PrepareAnim( int index )
{
	model.startAnim(index);
	model.stopAnim(index);
}

void Player::StartAnim( int index )
{
	model.startAnim(index);
}

void Player::StopAnim( int index )
{
	model.stopAnim(index);
}

void Player::SetAnimSpeed( int index, float speed )
{
	model.setAnimSpeed(index, speed);
}

void Player::SetSpeed( Vector3f vec )
{
	moveVec = vec;
}

void Player::SetSpeed( float speed )
{
	moveVec = Vector3f( speed*sin(angleXZ), 0.0f, speed*cos(angleXZ) );
}

void Player::AddSpeed( Vector3f vec )
{
	moveVec += vec;
}

int Player::Move( Vector3f vec )
{
	float dep = GetDepthData2(pos.x+vec.x, pos.z+vec.z);
	if( dep < -100.0f )
	{
		// îÕàÕäO
		return -1;
	}
	else if( pos.y+vec.y >= dep )
	{
		pos.x += vec.x;
		pos.y += vec.y;
		pos.z += vec.z;

		return 0;
	}
	else if( pos.y+vec.y >= dep-1.0f )
	{
		pos.x += vec.x;
		pos.y = dep;
		pos.z += vec.z;

		return 0;
	}

	dep = GetDepthData2(pos.x+vec.x, pos.z);
	if( pos.y+vec.y >= dep-1.0f )
		pos.x += vec.x;
	dep = GetDepthData2(pos.x, pos.z+vec.z);
	if( pos.y+vec.y >= dep-1.0f )
		pos.z += vec.z;

	return 1;
}

int Player::Move()
{
	return Move(moveVec);
}

void Player::Move( DIRECT direct )
{
	switch(direct)
	{
	case AHEAD:
		Move( Vector3f(0.5f*sin(angleXZ), 0.0f, 0.5f*cos(angleXZ)) );
		break;
	case LEFT:
		Move( Vector3f(0.5f*cos(angleXZ), 0.0f, -0.5f*sin(angleXZ)) );
		break;
	case RIGHT:
		Move( Vector3f(-0.5f*cos(angleXZ), 0.0f, 0.5f*sin(angleXZ)) );
		break;
	case BEHIND:
		Move( Vector3f(-0.5f*sin(angleXZ), 0.0f, -0.5f*cos(angleXZ)) );
		break;
	case UP:
		break;
	case HIGHUP:
		SetSpeed( Vector3f(0.5f*sin(angleXZ), 1.5f, 0.5f*cos(angleXZ)) );
		break;
	}
}


void Player::RotateXZ( float angle )
{
	angleXZ += angle;
	if( angleXZ < 0.0f )
		angleXZ += 6.28318f;
	else if( angleXZ > 6.28318f )
		angleXZ -= 6.28318f;
}

void Player::Jump()
{
	AddSpeed( Vector3f(0.0f, 0.5f, 0.0f) );
}

void Player::Render()
{
	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(angleXZ/3.1416f*180.0f, 0.0f, 1.0f, 0.0f);

	model.Update();
	model.Render();

	glPopMatrix();
}

void Player::PlayStepSound()
{
	if( IsStep() )
	{
		stepSound.SetPosition(pos);
		stepSound.Play();
	}
}

bool Player::IsStep()
{
	static Vector3f staticPos[2] = {defAnklePos[0],defAnklePos[1]};
	static bool staticFlag[2] = {false, false};
	bool flag[2] = { false, false };
	Vector3f anklePos[2];
	bool isStep = false;

	model.GetBonePos(ankleIndex[0], &anklePos[0]);
	model.GetBonePos(ankleIndex[1], &anklePos[1]);
	for( int i=0; i<2; i++ )
	{
		if( anklePos[i].z - staticPos[i].z > 0.0f )
			flag[i] = true;
		else
			flag[i] = false;
		if( !flag[i] && staticFlag[i] )
			isStep = true;
		staticPos[i] = anklePos[i];
		staticFlag[i] = flag[i];
	}

	return isStep;
}

bool Player::IsInAir()
{
	if( pos.y > GetDepthData2(pos.x, pos.z)+0.3f )
		return true;
	else
		return false;
}

void Player::UpdateByKinect( Quaternion* quat )
{
	model.UpdateKinectBone(quat);
}

Model* Player::GetModel()
{
	return &model;
}

Vector3f Player::GetPos()
{
	return pos;
}

Vector3f Player::GetSpeed()
{
	return moveVec;
}

float Player::GetAngleXZ()
{
	return angleXZ;
}

Vector3f Player::GetEyePos()
{
	Vector3f lpos,rpos;
	model.GetBonePos(eyeIndex[0], &lpos);
	model.GetBonePos(eyeIndex[1], &rpos);

	Quaternion rot;
	model.GetBoneWRot(headIndex, &rot);
	Vector3f vec = QuatToMatrix( rot ) * Vector3f(0.0f,0.0f,1.0f);

	Vector3f eyePos = (lpos+rpos)/2.0f;
	float dis = sqrt(eyePos.x*eyePos.x + eyePos.z*eyePos.z);
	eyePos.x = 1.25f*dis* sin(angleXZ);
	eyePos.z = 1.25f*dis* cos(angleXZ);
	eyePos.y += vec.y*0.2f;
	eyePos += pos;

	return eyePos;
}

Vector3f Player::GetEyeVec()
{
	Quaternion rot;
	model.GetBoneWRot(headIndex, &rot);
	Vector3f vec = QuatToMatrix( rot ) * Vector3f(0.0f,0.0f,1.0f);
	return Vector3f(sin(angleXZ), vec.y, cos(angleXZ))*5.0f;
}

Vector3f Player::GetArmPos( int index )
{
	Vector3f lArmPos, wArmPos;
	model.GetBonePos(armIndex[index], &lArmPos);
	wArmPos.x = lArmPos.z*sin(angleXZ) + lArmPos.x*cos(angleXZ);
	wArmPos.y = lArmPos.y;
	wArmPos.z = lArmPos.z*cos(angleXZ) - lArmPos.x*sin(angleXZ);
	wArmPos += pos;
	return wArmPos;
}