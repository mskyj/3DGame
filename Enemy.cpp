#include "Enemy.h"

const char* enemyModelName[] = {
	"model\\ao\\青鬼.pmx",
	"model\\boromir\\Boromir.pmx",
	"model\\homo\\ホモォ….pmx",
	"model\\abe\\阿部高和.pmx",
	"model\\mikudayo\\ミクダヨー.pmx",
	"model\\sento\\せんとさん.pmx",
	"model\\yodel\\ヨーデルキング.pmx"
};
const float enemySize[] = {
	6.0f,
	16.0f,
	8.0f,
	12.0f,
	7.0f,
	14.0f,
	18.0f
};

const char* enemyMotionName1[] = {
	"motion\\walk\\くねくね歩き.vmd",
	"motion\\walk\\とぼとぼ歩き.vmd",
	"motion\\walk\\ぴょこぴょこ歩き.vmd",
	"motion\\walk\\らいち歩き.vmd",
	"motion\\walk\\後ろ手歩き.vmd",
	"motion\\walk\\重量歩き.vmd",
	"motion\\walk\\男っぽい歩き.vmd",
	"motion\\walk\\頭手歩き.vmd",
	"motion\\walk\\日常歩き.vmd",
	"motion\\walk\\半スキップ.vmd"
};

const char* enemyMotionName2[] = {
	"motion\\run\\カイト走り.vmd",
	"motion\\run\\ガス欠走り.vmd",
	"motion\\run\\シェルティ走り.vmd",
	"motion\\run\\せんとさん走り.vmd",
	"motion\\run\\ふらつき走り.vmd",
	"motion\\run\\ボロミア全力走り.vmd",
	"motion\\run\\ヨーデル全力走り.vmd",
	"motion\\run\\女の子走り.vmd",
	"motion\\run\\男の子走り.vmd",
	"motion\\run\\必死逃げ走り.vmd"
};

PmxModel* pmx[7] = {NULL};
float pmxSize[7] = {0.0f};

static int GetRandom(int min,int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	pos.x = (float)GetRandom(-110, 210);
	pos.y = 0.0f;
	pos.z = (float)GetRandom(-190, -300);
	while( abs(pos.x)<10.0f && pos.z < -270.0f )
	{
		pos.x = (float)GetRandom(-110, 210);
		pos.z = (float)GetRandom(-190, -300);
	}
	angleXZ = (float)GetRandom(0, 62)/10.0f;

	stepOnPlayerFlag = false;
	status = STOP;
	StopAnim(0);
	StopAnim(1);
	StopAnim(2);

	for( int i=0; i<TITLE_NUM; i++ )
	{
		float dis;
		float dep;
		float ang;
		Vector3f tempPos;
		do
		{
			tempPos.x = (float)GetRandom(-350, 350);
			tempPos.y = 0.0f;
			tempPos.z = (float)GetRandom(-400, 250);
			dis = Vector3f(0.0f, 0.0f, -50.0f).distance(tempPos);
			ang = (float)GetRandom(0, 62)/10.0f;
			dep = GetDepthData(tempPos.x, tempPos.z);
			tempPos.y = dep;
		} while( dis < 180.0f || dis > 400.0f || dep > 10.0f || dep < -20.0f );
		titleAngle[i] = ang;
		titlePos[i] = tempPos;
	}
}

bool Enemy::LoadModel( ALuint buffer, GLuint shaderProgram  )
{
#ifndef _TEST_
	type = GetRandom(0, 6);
#else
	type = 0;
#endif
	if( pmx[type] == NULL )
	{
		size = (float)GetRandom(90, 120)/100.0f*enemySize[type];
		speed = (float)GetRandom(90, 120)/750.0f*sqrt(size)*ENEMY_SPEED;
		if( !model.Load(enemyModelName[type],shaderProgram,size) )
			return false;
		pmx[type] = model.GetPmx();
		pmxSize[type] = size;
	}
	else
	{
		size = pmxSize[type];
		speed = (float)GetRandom(90, 120)/750.0f*sqrt(size)*ENEMY_SPEED;
		if( !model.Load(pmx[type], shaderProgram) )
			return false;
	}

	headIndex = model.GetBoneIndex("頭");
	if( headIndex==-1 )
		return false;

	if( alertSound.LoadBuffer(buffer, 2.0f)==AL_FALSE )
		return false;

	return true;
}

void Enemy::SetPlayer( Player* p )
{
	player = p;
}

int Enemy::LoadAnim( int index )
{
	int animIndex = GetRandom(0, 9);
	if( index == 0 )
		return model.LoadAnim(enemyMotionName1[animIndex]);
	else if( index == 1 )
		return model.LoadAnim(enemyMotionName2[animIndex]);
	else
	{
		animIndex = GetRandom(1, 20);
		char buff[128] = "\0";
		sprintf_s(buff, "motion\\stand\\立%02d表情無し.vmd", animIndex);
		return model.LoadAnim(buff);
	}
}

void Enemy::Release()
{
	model.Release();
	alertSound.Release();
}

void Enemy::PrepareAnim( int index )
{
	model.startAnim(index);
	model.stopAnim(index);
}

void Enemy::StartAnim( int index, float speed )
{
	model.startAnim(index);
	model.setAnimSpeed(index, speed);
}

void Enemy::StopAnim( int index )
{
	model.stopAnim(index);
}

void Enemy::SetSpeed( Vector3f vec )
{
	moveVec = vec;
}

void Enemy::SetSpeed( float speed )
{
	moveVec = Vector3f( speed*sin(angleXZ), 0.0f, speed*cos(angleXZ) );
}

void Enemy::AddSpeed( Vector3f vec )
{
	moveVec += vec;
}

int Enemy::Move( Vector3f vec )
{
	float dep = GetDepthData(pos.x+vec.x, pos.z+vec.z);
	if( dep < -100.0f )
	{
		// 範囲外
		return -1;
	}
	else if( pos.y+vec.y >= dep )
	{
		pos.x += vec.x;
		pos.y += vec.y;
		pos.z += vec.z;

		return 0;
	}
	else if( pos.y+vec.y >= dep-size*0.7 )
	{
		pos.x += vec.x;
		pos.y = dep;
		pos.z += vec.z;

		return 0;
	}
	dep = GetDepthData(pos.x+vec.x, pos.z);
	if( pos.y+vec.y >= dep-1.0f )
		pos.x += vec.x;
	dep = GetDepthData(pos.x, pos.z+vec.z);
	if( pos.y+vec.y >= dep-1.0f )
		pos.z += vec.z;

	return 1;
}

int Enemy::Move()
{
	return Move(moveVec);
}

void Enemy::RotateToAngle( float angle )
{
	float gap = angle - angleXZ;
	if( gap < -3.1416f )
		gap += 6.2832f;
	else if( gap > 3.1416f )
		gap -= 6.2832f;

	if( abs(gap)<0.05f )
		angleXZ += gap;
	else if( gap > 0.0f )
		angleXZ += 0.05f;
	else
		angleXZ -= 0.05f;
	if( angleXZ > 6.2832f )
		angleXZ -= 6.28318f;
	else if( angleXZ < 0.0f )
		angleXZ += 6.28318f;
}

void Enemy::FixAngle( float angle )
{
	static float staticAngle = 0.0f;
	static Vector3f staticPos = Vector3f();
	static bool stopFlag = false;
	static int time = 0;
	
	if( staticPos.distance(pos)<0.1f && (staticAngle == angle || stopFlag) )
	{
		time++;
		stopFlag = true;
		angleXZ += 0.05f*time;
		if( angleXZ > 6.2832f )
			angleXZ -= 6.28318f;
		else if( angleXZ < 0.0f )
			angleXZ += 6.28318f;
		if( time == 30 )
			pos.y = GetDepthData(pos.x, pos.z);
	}
	else
	{
		stopFlag = false;
		time = 0;
	}

	staticAngle = angle;
	staticPos = pos;
}

void Enemy::Render( bool titleFlag )
{
	if( titleFlag )
	{
		model.Update();
		for( int i=0; i<TITLE_NUM; i++ )
		{
			glPushMatrix();
			glTranslatef(titlePos[i].x, titlePos[i].y, titlePos[i].z );
			glRotatef((titleAngle[i]/3.1416f+1.0f)*180.0f, 0.0f, 1.0f, 0.0f);
			model.Render();
			glPopMatrix();
		}
	}
	else
	{
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z );
		glRotatef(angleXZ/3.1416f*180.0f, 0.0f, 1.0f, 0.0f);
		model.Update();
		model.Render();
		glPopMatrix();
	}
}

void Enemy::Update()
{
	if( status==STOP )
	{
		StartAnim(0,speed/sqrt(size)*10.0f);
		status = WALK;
		SetSpeed( speed );
	}
	else if( status==WALK )
	{
		float aveDepMin = 1000.0f;
		int minX, minZ;
		for( int x=-1; x<=1; x++ )
			for( int z=-1; z<=1; z++ )
			{
				float dep = GetAverageDepth( pos.x+(size*0.15f+2.0f)*x, pos.z+(size*0.15f+2.0f)*z, 4.0f );
				if( aveDepMin > dep && !(x==0 && z==0) )
				{
					aveDepMin = dep;
					minX = x;
					minZ = z;
				}
			}
		float angle;
		if( minX==1 && minZ==0 )
			angle = 1.5708f;
		else if( minX==-1 && minZ==0 )
			angle = 4.7124f;
		else
			angle = atan2f((float)minX,(float)minZ);
		if( angle<0.0f )
			angle += 6.2832f;
		RotateToAngle(angle);
		FixAngle(angle);

		AddSpeed( Vector3f(0.0f, -0.5f, 0.0f) );
		if( Move()==1 )
		{
			SetSpeed( speed );
		}

		Vector3f pPos = player->GetPos();
		pPos.y = 0.0f;
		if( pPos.distance(Vector3f(pos.x,0.0f,pos.z)) < 10.0f*sqrt(size)*ENEMY_VIEW_RANGE )
		{
			if( abs(pPos.z - pos.z)<0.1f )
			{
				if( pPos.x - pos.x>0.0f )
					angle = 1.5708f;
				else
					angle = 4.7124f;
			}
			else
			{
				angle = atan2f(pPos.x-pos.x, pPos.z-pos.z);
				if( angle<0.0f )
					angle += 6.2832f;
			}

			float gap = abs(angleXZ-angle);
			if( gap<0.6f || gap>5.68f )
			{
				cout << angleXZ << endl;
				cout << angle << endl;
				cout << endl;

				status = WARN;
				StopAnim(0);
				StartAnim(2,0.0f);
				warnningTime = 0;
				model.EnableLookAt();
				model.UpdateHeadBone( Quaternion() );
				alertSound.SetPosition(pos+Vector3f(0.0f,size/2.0f, 0.0f));
				alertSound.Play();
			}
		}
	}
	else if( status==WARN )
	{

		SetSpeed( Vector3f() );

		float angle;
		Vector3f pPos = player->GetPos();
		float distance = Vector3f(pPos.x, 0.0f, pPos.z).distance(Vector3f(pos.x,0.0f,pos.z));
		if( distance < 12.0f*sqrt(size)*ENEMY_VIEW_RANGE )
		{
			if( abs(pPos.z - pos.z)<0.1f )
			{
				if( pPos.x - pos.x>0.0f )
					angle = 1.5708f;
				else
					angle = 4.7124f;
			}
			else
			{
				angle = atan2f(pPos.x-pos.x, pPos.z-pos.z);
				if( angle<0.0f )
					angle += 6.2832f;
			}

			if( abs(angleXZ-angle)>=0.72f && abs(angleXZ-angle)<=5.56f )
			{
				warnningTime--;
				if( warnningTime <= 0 )
				{
					status = STOP;
					StopAnim(2);
					model.DisableLookAt();
				}
			}
			else
			{
				float a = (angleXZ-angle)/2.0f;
				float b = atan2f(pPos.y + HUMAN_SIZE -pos.y-size, distance);
				Quaternion quat = Quaternion(0.0f, sin(a), 0.0f, cos(a)) * Quaternion(sin(b), 0.0f, 0.0f, cos(b));
				model.UpdateHeadBone( quat );
				warnningTime ++;
				if( warnningTime==ENEMY_WARN_TIME )
				{
					status = RUN;
					StopAnim(2);
					StartAnim(1, speed/sqrt(size)*10.0f);
					SetSpeed( speed*2.0f );
					model.DisableLookAt();
				}
			}
		}
		else
		{
			warnningTime--;
			if( warnningTime <= 0 )
			{
				status = STOP;
				StopAnim(2);
				model.DisableLookAt();
			}
		}
	}
	else if( status==RUN )
	{
		Vector3f pPos = player->GetPos();
		float distance = Vector3f(pPos.x, 0.0f, pPos.z).distance(Vector3f(pos.x,0.0f,pos.z));
		if( distance>15.0f*sqrt(size)*ENEMY_VIEW_RANGE )
		{
			status = STOP;
			StopAnim(1);
			SetSpeed( Vector3f() );
		}
		else if( distance<sqrt(size)*ENEMY_STEP_RANGE )
		{
			if( pPos.y < pos.y + 1.0f )
				stepOnPlayerFlag = true;
		}

		float angle;
		if( abs(pPos.z - pos.z)<0.1f )
		{
			if( pPos.z - pos.z>0.0f )
				angle = 1.5708f;
			else
				angle = 4.7124f;
		}
		else
		{
			angle = atan2f(pPos.x-pos.x, pPos.z-pos.z);
			if( angle<0.0f )
				angle += 6.2832f;
		}
		RotateToAngle(angle);
		FixAngle(angle);

		AddSpeed( Vector3f(0.0f, -0.5f, 0.0f) );
		if( Move()==1 )
		{
			SetSpeed( speed );
		}
	}

}