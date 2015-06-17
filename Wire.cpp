#include "Wire.h"

WireModel::WireModel()
{
}

WireModel::~WireModel()
{
}

void WireModel::SetHumanModel(Model* humanModel)
{
	human = *humanModel;
	humanBoneIndex = new int[pmx->boneNum];

	int i;
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		int parentIndex = pmx->bone[index].parentBoneIndex;
		Vector3f pos;
		humanBoneIndex[i] = human.GetBonePos(pmx->bone[index].name,&pos);
		if( humanBoneIndex[i] != -1 )
		{
			pmx->bone[index].pos = pos/human.sphere.radius*13.0f;
		}

		if( pmx->bone[index].parentBoneIndex>=0 && pmx->bone[index].parentBoneIndex<pmx->boneNum )
		{
			int parentIndex = pmx->bone[index].parentBoneIndex;
			boneInfo[index].localPos = pmx->bone[index].pos - pmx->bone[parentIndex].pos;
			boneInfo[index].worldPos = pmx->bone[index].pos;
			boneInfo[index].localRot = Quaternion();
			boneInfo[index].worldRot = Quaternion();
		}
		else
		{
			boneInfo[index].localPos = pmx->bone[index].pos;
			boneInfo[index].worldPos = pmx->bone[index].pos;
			boneInfo[index].localRot = Quaternion();
			boneInfo[index].worldRot = Quaternion();
		}
		if( !strcmp( pmx->bone[index].name, "センター" ) )
			boneInfo[index].isCenter = 1;
		else
			boneInfo[index].isCenter = 0;
	}
}

void WireModel::GetPos(Vector3f* new_lPos, Vector3f* new_rPos)
{
	*new_lPos = lPos/13.0f;
	*new_rPos = rPos/13.0f;
}

void WireModel::Update()
{
	int i,j;
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		boneInfo[index].localRot = Quaternion();
		boneInfo[index].IKQuat = Quaternion();
		if( boneInfo[index].isCenter || pmx->bone[index].parentBoneIndex<0 || pmx->bone[index].parentBoneIndex>=pmx->boneNum )
			boneInfo[index].localPos = Vector3f();
	}

	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		Quaternion rot;
		if( humanBoneIndex[index]!=-1 )
		{
			human.GetBoneRot(humanBoneIndex[index], &rot);
			boneInfo[index].localRot = rot;
		}
	}
	Vector3f pos;
	if( !human.GetBonePos("センター",&pos) )
	{
			boneInfo[1].localPos = pos-pmx->bone[1].pos;
	}
	UpdateBonePos();

#if 0
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		Vector3f pos;
		if( humanBoneIndex[index]!=-1 )
		{
			human.GetBonePos(humanBoneIndex[index], &pos);
			boneInfo[index].worldPos = pos/human.sphere.radius*13.0f;
		}
	}
#endif

	lPos = boneInfo[5].worldPos;
	rPos = boneInfo[8].worldPos;

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	float *ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	if( ptr )
	{
		for( i=0; i<pmx->vertexNum; i++ )
		{
			Vector3f pos = Vector3f();
			for( j=0; j<4; j++ )
				if( pmx->vertex[i].boneIndex[j]!=-1 )
					pos += ( ( boneInfo[ pmx->vertex[i].boneIndex[j] ].worldPos
						+ QuatToMatrix( boneInfo[ pmx->vertex[i].boneIndex[j] ].worldRot )
						* (pmx->vertex[i].pos - pmx->bone[ pmx->vertex[i].boneIndex[j] ].pos) ) * pmx->vertex[i].boneWeight[j] );

			ptr[i*3+0] = pos.x/13.0f;
			ptr[i*3+1] = pos.y/13.0f;
			ptr[i*3+2] = pos.z/13.0f;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

/*
*	ここからWireクラス
*/

Wire::Wire()
{
	status[0] = NO_TARGET; 
	status[1] = NO_TARGET;
}

Wire::~Wire()
{
}

bool Wire::LoadModel( const char* path, GLuint shaderProgram )
{
	Texture* tex = new Texture();
	if( !tex->Load("data\\target.png") )
		return false;
	tex->CreateTexture(&texture);
	delete tex;

	if( shotSound.LoadWave("sound\\wire.wav") == AL_FALSE )
		return false;
	if( pullSound.LoadWave("sound\\wire2.wav") == AL_FALSE )
		return false;

	return model.Load(path, shaderProgram);
}

void Wire::Release()
{
	model.Release();
	shotSound.Release();
	pullSound.Release();
}

void Wire::SetHuman( Player* human )
{
	player = human;
	model.SetHumanModel( player->GetModel() );
}

void Wire::SetKinect( Kinect* k )
{
	kinect = k;
}

void Wire::Shot()
{
	if( status[0]!=TARGET || status[1]!=TARGET )
		return;

	Vector3f playerPos = player->GetPos();
	float angleXZ = player->GetAngleXZ();

	Vector3f pos[2];
	model.GetPos(&pos[0], &pos[1]);
	for(int i=0; i<2; i++)
	{
		status[i] = ON_SHOT;
		Vector3f temp = pos[i];
		worldPos[i].x = temp.z*sin(angleXZ) + temp.x*cos(angleXZ);
		worldPos[i].y = temp.y;
		worldPos[i].z = temp.z*cos(angleXZ) - temp.x*sin(angleXZ);
		worldPos[i] += playerPos;
		len[i] = (predictedPos[i]-worldPos[i]).length();
	}
	shotSound.SetPosition((worldPos[0]+worldPos[1])/2.0f);
	shotSound.Play();
}

void Wire::Update()
{
	if( status[0]==TARGET )
		status[0] = NO_TARGET;
	if( status[1]==TARGET )
		status[1] = NO_TARGET;

	float angleXZ = player->GetAngleXZ();

	Vector3f vec[2];
	int result[2];
	result[0] = kinect->GetLArmVec(&vec[0]);
	result[1] = kinect->GetRArmVec(&vec[1]);

	for( int i=0; i<2; i++ )
	{
		if( status[i]==NO_TARGET && result[i] && vec[i].z >0.2f )
		{
			vec[i] = Vector3f(vec[i].z*sin(angleXZ)+vec[i].x*cos(angleXZ),
							  vec[i].y,
						 	  vec[i].z*cos(angleXZ)-vec[i].x*sin(angleXZ));
			Vector3f start = player->GetArmPos(i);
			status[i] = (WIRE_STATUS)LineCollisionDetect(&predictedPos[i], start, start+vec[i]*30.0f);
		}
		else if( status[i]==ON_SHOT )
		{
			Vector3f vec = predictedPos[i] - worldPos[i];
			if( vec.length()<3.0f )
			{
				worldPos[i] = predictedPos[i];
				status[i] = WIRE_FIXED;
			}
			else
			{
				worldPos[i] += vec.normalize()*3.0f;
			}
		}
	}

	// ワイヤーが左右両方とも固定されたら
	if( status[0]==WIRE_FIXED && status[1]==WIRE_FIXED )
	{
		status[0]=TAKE_UP;
		status[1]=TAKE_UP;

		// 左右のワイヤーの長さを考慮してベクトル計算
		// →ベクトルをプレイヤーの速度に加える

		Vector3f pos[2],start[2];
		model.GetPos(&pos[0], &pos[1]);
		for(int i=0; i<2; i++)
		{
			Vector3f temp = pos[i]*HUMAN_SIZE;
			start[i].x = temp.z*sin(angleXZ) + temp.x*cos(angleXZ);
			start[i].y = temp.y;
			start[i].z = temp.z*cos(angleXZ) - temp.x*sin(angleXZ);
			start[i] += player->GetPos();
			len[i] = (worldPos[i]-start[i]).length();
		}
		float rate[2];
		if( len[0] < len[1] )
		{
			rate[0] = len[0] / len[1];
			rate[1] = 1.0f;
		}
		else
		{
			rate[0] = 1.0f;
			rate[1] = len[1] / len[0];
		}
		Vector3f vec = Vector3f();
		for(int i=0; i<2; i++)
			vec += (worldPos[i]-start[i]).normalize()*rate[i];
		player->AddSpeed(vec*0.8f);
		player->AddSpeed( Vector3f(0.0f, 0.5f, 0.0f) );

		pullSound.SetPosition((start[0]+start[1])/2.0f);
		pullSound.Play();
	}
	else if( status[0]==TAKE_UP )
	{
#if 0
		if( player->GetSpeed().y < 0.0f )
		{
			status[0] = CIRCLE;
			status[1] = CIRCLE;

			Vector3f pos[2],start[2];
			model.GetPos(&pos[0], &pos[1]);
			Vector3f vec = Vector3f();
			for(int i=0; i<2; i++)
			{
				Vector3f temp = pos[i]*HUMAN_SIZE;
				start[i].x = temp.z*sin(angleXZ) + temp.x*cos(angleXZ);
				start[i].y = temp.y;
				start[i].z = temp.z*cos(angleXZ) - temp.x*sin(angleXZ);
				start[i] += player->GetPos();
				len[i] = (worldPos[i]-start[i]).length();
				vec += (worldPos[i]-start[i]);
				if( (worldPos[i]-start[i]).y < 0.0f )
				{
					status[0]=TAKE_UP;
					status[1]=TAKE_UP;
				}
			}
			if( status[0]==CIRCLE )
			{
				Vector3f v1 = -vec.normalize();
				Vector3f v2 = Vector3f( 0.0f, -1.0f, 0.0f );
				circleMaxTheta = acosf(abs( v1.dot(v2) ));
				circleTheta = circleMaxTheta;
				if( abs(circleMaxTheta)<0.1f )
				{
					status[0]=TAKE_UP;
					status[1]=TAKE_UP;
				}
				else
				{
					rotateAxis = v1.cross(v2).normalize();
					circleSpeed = player->GetSpeed().length()/10.0f;
					cout << "circleMaxTheta" << circleMaxTheta << endl;
					cout << "circleSpeed" << circleSpeed << endl;
				}
			}
		}
#endif

		// ワイヤーが伸びたら、状態を戻す
		Vector3f pos[2],start[2];
		model.GetPos(&pos[0], &pos[1]);
		for(int i=0; i<2; i++)
		{
			Vector3f temp = pos[i]*HUMAN_SIZE;
			start[i].x = temp.z*sin(angleXZ) + temp.x*cos(angleXZ);
			start[i].y = temp.y;
			start[i].z = temp.z*cos(angleXZ) - temp.x*sin(angleXZ);
			start[i] += player->GetPos();
			float l = (worldPos[i]-start[i]).length();;
			if( len[i] < l )
			{
				status[0] = NO_TARGET;
				status[1] = NO_TARGET;
			}
			len[i] = l;
		}

#if 0
		Vector2f a = Vector2f( worldPos[0].z, worldPos[0].x );
		Vector2f b = Vector2f( worldPos[1].z, worldPos[1].x );
		Vector2f c = Vector2f( player->GetPos().z, player->GetPos().x );
		Vector2f d = Vector2f( player->GetPos().z+player->GetSpeed().z, player->GetPos().x+player->GetSpeed().x );
		if( CrossDetect(a, b, c, d  ) )
		{
			status[0] = NO_TARGET;
			status[1] = NO_TARGET;
		}
#endif
	}
	else if( status[0]==CIRCLE )
	{
#if 0
		Vector3f pos[2],start[2],pPos=Vector3f();
		model.GetPos(&pos[0], &pos[1]);
		for(int i=0; i<2; i++)
		{
			Vector3f temp = pos[i]*HUMAN_SIZE;
			start[i].x = temp.z*sin(angleXZ) + temp.x*cos(angleXZ);
			start[i].y = temp.y;
			start[i].z = temp.z*cos(angleXZ) - temp.x*sin(angleXZ);
			start[i] += player->GetPos();
			pPos += start[i];
		}

		pPos /= 2.0f;
		float change;
		if( circleMaxTheta>0.0f )
			change = -0.05f*circleSpeed;
		else
			change = 0.05f*circleSpeed;

		circleTheta += change;
		if( abs(circleTheta) > abs(circleMaxTheta) )
		{
			status[0] = NO_TARGET;
			status[1] = NO_TARGET;
		}
		else
		{
			Quaternion quat;
			change /= -2.0f;
			float s = sin(change);
			quat.x = rotateAxis.x * s;
			quat.y = rotateAxis.y * s;
			quat.z = rotateAxis.z * s;
			quat.w = cos(change);
			Vector3f new_pPos = centerPos + QuatToMatrix(quat)*(pPos-centerPos);
			player->SetSpeed(new_pPos-pPos);
		}
#endif
	}
}

void Wire::RenderModel()
{
	glPushMatrix();

	Vector3f pos = player->GetPos();
	float angleXZ = player->GetAngleXZ();

	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(angleXZ/3.1416f*180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(HUMAN_SIZE,HUMAN_SIZE,HUMAN_SIZE);

	model.Update();
	model.Render();

	glPopMatrix();
}

void Wire::RenderWire()
{
	Vector3f playerPos = player->GetPos();
	float angleXZ = player->GetAngleXZ();

	for( int i=0; i<2; i++ )
	{
		if( status[i]==TARGET )
		{
#if 1
			glPushMatrix();
			Vector3f pos = (predictedPos[i]*0.9f+playerPos*0.1f);
			pos.y += 1.0f;
			glTranslatef(pos.x, pos.y, pos.z);
			glRotatef(angleXZ*57.3f, 0.0f, 1.0f, 0.0f);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex3d(-1.0, -1.0, 0.0);
			glTexCoord2f(0.0, 1.0);
			glVertex3d(-1.0, 1.0, 0.0);
			glTexCoord2f(1.0, 1.0);
			glVertex3d(1.0, 1.0, 0.0);
			glTexCoord2f(1.0, 0.0);
			glVertex3d(1.0, -1.0, 0.0);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glPopMatrix();
#endif
		}
		else if( status[i] == ON_SHOT || status[i] == WIRE_FIXED )
		{
			glDisable(GL_TEXTURE_2D);
			glPushMatrix();
			Vector3f begin,end;
			Vector3f pos[2];
			model.GetPos(&pos[0], &pos[1]);
			pos[i] = pos[i] * HUMAN_SIZE;
			begin.x = pos[i].z*sin(angleXZ) + pos[i].x*cos(angleXZ);
			begin.y = pos[i].y;
			begin.z = pos[i].z*cos(angleXZ) - pos[i].x*sin(angleXZ);
			begin += playerPos;
			end = worldPos[i];

			glLineWidth(1.5f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex3f(begin.x, begin.y, begin.z);
			glVertex3f(end.x, end.y, end.z);
			glEnd();
			glPopMatrix();
		}
	}
}