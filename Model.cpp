#include "Model.h"

/*      
	スキンメッシュアニメーション用クラス
	とりあえず動けばいいという考えで作ったのでいろいろひどい
	このソースを見て理解できたらすごいっていうぐらいひどい

	TODO
	・スキンメッシュアニメーションを行列計算で行う
	・メモリの無駄を減らす
*/

namespace detail 
{
float y(float t, float p1, float p2)
{
    return ((1 + 3 * p1 - 3 * p2) * t * t * t + (3 * p2 - 6 * p1) * t * t + 3 * p1 * t);
}
float dy(float t, float p1, float p2)
{
    return ((3 + 9 * p1 - 9 * p2) * t * t + (6 * p2 - 12 * p1) * t + 3 * p1);
}
}

float bezier( float x1, float y1, float x2, float y2, float x )
{
	float t = x;
	int count = 100;
    while (1) {
        float v = detail::y(t, x1, x2) - x;
        if (abs(v) < 0.0001) break;
        float tt = detail::dy(t, x1, x2);
        if ( tt == 0.0f) break;
        t -= v / tt;
		count--;
		if( count<=0 )
			return x;
    }
	return detail::y(t, y1, y2);
}

void Model::ComputeBoundingSphere()
{
	float tempRadius = 0.0f;
	Vector3f tempCenter;
	tempCenter.x = 0.0f;
	tempCenter.y = 0.0f;
	tempCenter.z = 0.0f;

	//　中心座標の算出
	for ( int i=0; i<pmx->vertexNum; i++ )
	{
		tempCenter.x += pmx->vertex[i].pos.x;
		tempCenter.y += pmx->vertex[i].pos.y;
		tempCenter.z += pmx->vertex[i].pos.z;
	}
	tempCenter.x /= (float)pmx->vertexNum;
	tempCenter.y /= (float)pmx->vertexNum;
	tempCenter.z /= (float)pmx->vertexNum;

	//　半径の算出
	for (int i=0; i<pmx->vertexNum; i++ )
	{
		float d = 0.0f;
		Vector3f temp;
		temp.x = pmx->vertex[i].pos.x - tempCenter.x;
		temp.y = pmx->vertex[i].pos.y - tempCenter.y;
		temp.z = pmx->vertex[i].pos.z - tempCenter.z;
		d = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
		tempRadius = (tempRadius < d ) ? d : tempRadius;
	}

	//　結果を格納
	sphere.center = tempCenter;
	sphere.radius = tempRadius;
}

Model::Model()
{
	pmx = NULL;
	for( int i=0; i<MAX_ANIM_NUM; i++ )
		vmd[i] = NULL;
	for( int i=0; i<MAX_POSE_NUM; i++ )
		vpd[i] = NULL;
	boneInfo = NULL;
	boneIndexList = NULL;
	kinectUseFlag = false;
	lookAtFlag = false;
}

Model::~Model()
{
	Release();
}

void Model::Release()
{
	if( !pmx )
	{
		delete pmx;
		pmx = NULL;
	}
	if( !boneInfo )
	{
		delete[] boneInfo;
		boneInfo = NULL;
	}
	if( !boneIndexList )
	{
		delete[] boneIndexList;
		boneIndexList = NULL;
	}
	for( int i=0; i<MAX_ANIM_NUM; i++ )
		ReleaseAnim(i);
	for( int i=0; i<MAX_POSE_NUM; i++ )
		ReleasePose(i);

	if( vbo[0] )
	{
		glDeleteBuffers(4, vbo);
		vbo[0] = 0;
	}
}

void Model::ReleaseAnim( int key )
{
	if( !vmd[ key ] )
	{
		delete vmd[ key ];
		vmd[ key ] = NULL;
	}
}

void Model::ReleasePose( int key )
{
	if( !vpd[ key ] )
	{
		delete vpd[ key ];
		vpd[ key ] = NULL;
	}
}

bool Model::Load( const char* filename, GLuint shaderProgram, float size )
{
	int i;
	pmx = new PmxModel();

	if( !pmx->Load( filename ) )
	{
		cout << "Model Error : 3Dモデルファイル" << filename << "の読み込みに失敗しました。" << endl;
		pmx = NULL;
		return false;
	}

	// バウンディングスフィアの計算
	ComputeBoundingSphere();

	scale = size/sphere.radius;

	//  頂点座標の正規化
	for( i=0; i<pmx->vertexNum; i++ )
		pmx->vertex[i].pos *= scale;

	// ボーン座標の正規化
	for( i=0; i<pmx->boneNum; i++ )
	{
		pmx->bone[i].pos *= scale;
		pmx->bone[i].offsetPos *= scale;
	}

	return Load(pmx, shaderProgram);
}


bool Model::Load( PmxModel* model, GLuint shaderProgram )
{
	int i,j;

	shaderProg = shaderProgram;

	pmx = model;

	// ボーンリストの作成
	if( pmx->boneNum>0 )
	{
		PmxBoneList* listHead = new PmxBoneList;
		listHead->prev = NULL;
		listHead->next = NULL;
		listHead->index = 0;
		listHead->layer = pmx->bone[0].transformLayer;
		listHead->flag_AfterPhysics = pmx->bone[0].flag_AfterPhysicsTransform;
		for( i=1; i<pmx->boneNum; i++ )
		{
			PmxBoneList* list = new PmxBoneList;

			list->index = i;
			list->layer = pmx->bone[i].transformLayer;
			list->flag_AfterPhysics = pmx->bone[i].flag_AfterPhysicsTransform;

			PmxBoneList* tempList1 = NULL;
			PmxBoneList* tempList2 = listHead;

			j = 0;
			if( list->flag_AfterPhysics )
			{
				for( j; j<i; j++ )
				{
					if( tempList2->flag_AfterPhysics )
						break;
					tempList1 = tempList2;
					tempList2 = tempList2->next;
				}
			}
			if( tempList2 != NULL
				&& tempList2->flag_AfterPhysics == list->flag_AfterPhysics
				&& list->layer > tempList2->layer )
			{
				for( j; j<i; j++ )
				{
					if( tempList2->flag_AfterPhysics > list->flag_AfterPhysics )
						break;
					if( tempList2->layer >= list->layer )
						break;
					tempList1 = tempList2;
					tempList2 = tempList2->next;
				}
			}
			if( tempList2 != NULL
				&& list->flag_AfterPhysics == tempList2->flag_AfterPhysics 
				&& list->layer == tempList2->layer
				&& list->index > tempList2->index )
			{
				for( j; j<i; j++ )
				{
					if( tempList2->flag_AfterPhysics > list->flag_AfterPhysics )
						break;
					if( tempList2->layer > list->layer )
						break;
					if( tempList2->index > list->index )
						break;
					tempList1 = tempList2;
					tempList2 = tempList2->next;
				}
			}

			if( j==0 )
			{
				list->next = listHead;
				list->prev = NULL;
				list->next->prev = list;
				listHead = list;
			}
			else if( j==i )
			{
				list->next = NULL;
				list->prev = tempList1;
				list->prev->next = list;
			}
			else
			{
				list->next = tempList2;
				list->prev = tempList2->prev;
				list->next->prev = list;
				list->prev->next = list;
			}
		}
		boneIndexList = new int[ pmx->boneNum ];

		PmxBoneList* list = listHead;
		for( i=0; i<pmx->boneNum; i++ )
		{
			boneIndexList[i] = list->index;
			if( i!=pmx->boneNum-1 )
			{
				list = list->next;
				delete list->prev;
			}
			else
				delete list;
		}
	}

	boneInfo = new BoneInfo[ pmx->boneNum ];
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		if( pmx->bone[index].parentBoneIndex>=0 && pmx->bone[index].parentBoneIndex<pmx->boneNum )
		{
			int parentIndex = pmx->bone[index].parentBoneIndex;
			boneInfo[index].localPos = pmx->bone[index].pos - pmx->bone[parentIndex].pos;
			boneInfo[index].worldPos = pmx->bone[index].pos;
			boneInfo[index].localRot = Quaternion();
			boneInfo[index].worldRot = Quaternion();
			boneInfo[index].rotMat = Matrix4f();
		}
		else
		{
			boneInfo[index].localPos = pmx->bone[index].pos;
			boneInfo[index].worldPos = pmx->bone[index].pos;
			boneInfo[index].localRot = Quaternion();
			boneInfo[index].worldRot = Quaternion();
			boneInfo[index].rotMat = Matrix4f();
		}
		if( !strcmp( pmx->bone[index].name, "センター" ) )
			boneInfo[index].isCenter = 1;
		else
			boneInfo[index].isCenter = 0;
	}

	// VBO用初期化
	float* vertexArr;
	float* texcoordArr;
	float* normalArr;
	uint* indexArr;
	vertexArr = new float[ pmx->vertexNum*3 ];
	normalArr = new float[ pmx->vertexNum*3 ];
	texcoordArr = new float[ pmx->vertexNum*2 ];
	indexArr = new uint[ pmx->faceNum*3 ];

	for( i=0; i<pmx->vertexNum; i++ )
	{
		vertexArr[i*3+0] = pmx->vertex[i].pos.x;
		vertexArr[i*3+1] = pmx->vertex[i].pos.y;
		vertexArr[i*3+2] = pmx->vertex[i].pos.z;
		normalArr[i*3+0] = pmx->vertex[i].normal.x;
		normalArr[i*3+1] = pmx->vertex[i].normal.y;
		normalArr[i*3+2] = pmx->vertex[i].normal.z;
		texcoordArr[i*2+0] = pmx->vertex[i].uv.u;
		texcoordArr[i*2+1] = pmx->vertex[i].uv.v;
	}
	for( i=0; i<pmx->faceNum; i++ )
	{
		indexArr[i*3+0] = pmx->face[i].vertexIndex[0];
		indexArr[i*3+1] = pmx->face[i].vertexIndex[1];
		indexArr[i*3+2] = pmx->face[i].vertexIndex[2];
	}

	int bufferSize = 0;

	glGenBuffers(4, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pmx->vertexNum*3*sizeof(float), vertexArr, GL_DYNAMIC_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if( pmx->vertexNum*3*sizeof(float) != bufferSize )
	{
		 glDeleteBuffers(1, &vbo[0]);
		 cout << "Model Error : VBOの作成に失敗しました。" << endl;
		 Release();
		 return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, pmx->vertexNum*3*sizeof(float), normalArr, GL_STATIC_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if( pmx->vertexNum*3*sizeof(float) != bufferSize )
	{
		 glDeleteBuffers(2, &vbo[0]);
		 cout << "Model Error : VBOの作成に失敗しました。" << endl;
		 Release();
		 return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, pmx->vertexNum*2*sizeof(float), texcoordArr, GL_STATIC_DRAW);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if( pmx->vertexNum*2*sizeof(float) != bufferSize )
	{
		 glDeleteBuffers(3, &vbo[0]);
		 cout << "Model Error : VBOの作成に失敗しました。" << endl;
		 Release();
		 return false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pmx->faceNum*3*sizeof(uint), indexArr, GL_STATIC_DRAW);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	if( pmx->faceNum*3*sizeof(uint) != bufferSize )
	{
		 glDeleteBuffers(4, &vbo[0]);
		 cout << "Model Error : VBOの作成に失敗しました。" << endl;
		 Release();
		 return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertexArr;
	delete[] normalArr;
	delete[] texcoordArr;
	delete[] indexArr;

	kinectBoneIndex[0] = GetBoneIndex("頭");
	kinectBoneIndex[1] = GetBoneIndex("首");
	kinectBoneIndex[2] = GetBoneIndex("左腕");
	kinectBoneIndex[3] = GetBoneIndex("右腕");
	kinectBoneIndex[4] = GetBoneIndex("左ひじ");
	kinectBoneIndex[5] = GetBoneIndex("右ひじ");
	kinectBoneIndex[6] = GetBoneIndex("左手首");
	kinectBoneIndex[7] = GetBoneIndex("右手首");
	kinectBoneIndex[8] = GetBoneIndex("胸");
	kinectBoneIndex[9] = GetBoneIndex("左脇腹");
	kinectBoneIndex[10] = GetBoneIndex("右脇腹");
	kinectBoneIndex[11] = GetBoneIndex("左ひざ");
	kinectBoneIndex[12] = GetBoneIndex("右ひざ");
	kinectBoneIndex[13] = GetBoneIndex("左足首");
	kinectBoneIndex[14] = GetBoneIndex("右足首");

	headBoneIndex = kinectBoneIndex[0];

	return true;
}

int Model::LoadAnim(const char* filename, byte loopFlag)
{
	int i,j;

	if( pmx==NULL )
	{
		cout << "Model Error : アニメーション読み込み時にモデルが読み込まれていません。" << endl;
		return -1;
	}

	int index;
	for( index=0; index<MAX_ANIM_NUM; index++ )
		if( !vmd[index] )
			break;
	if( index==MAX_ANIM_NUM )
	{
		cout << "Model Error : アタッチするアニメーションが多すぎます。" << endl;
		return -1;
	}

	vmd[index] = new Vmd();
	if( !vmd[index]->Load( filename ) )
	{
		cout << "Model Error : VMDファイル" << filename << "の読み込みに失敗しました。" << endl;
		delete vmd[index];
		return -1;
	}

	vmd[index]->loopFlag = loopFlag;
	vmd[index]->playFlag = 0;
	vmd[index]->playTime = 0;
	
	for( i=0; i<pmx->boneNum; i++ )
	{
		boneInfo[i].node[index] = NULL;
		boneInfo[i].nowKey[index] = NULL;
	}

	VmdBoneNode* node = vmd[index]->boneNode;
	for( i=0; i<(int)vmd[index]->boneNodeNum; i++,node++ )
	{
		for( j=0; j<pmx->boneNum; j++ )
			if( !strncmp( node->name, pmx->bone[j].name, 32 ) )
				break;
		if( j==pmx->boneNum )
		{
			//cout << "Model Warnning : VMDで使用されているボーン" << node->name << "が見つかりませんでした。" << endl;
		}
		else
		{
			boneInfo[j].node[index] = node;
			boneInfo[j].nowKey[index] = node->firstKey;
		}
	}

	return index;
}

void Model::startAnim( int key )
{
	if( key==-1 )
	{
		cout << "Model Error : アニメーションキーが不正なためアニメーションをスタートできませんでした。" << endl;
		return;
	}
	vmd[ key ]->playFlag = 1;
	vmd[ key ]->playTime = 0;
	for( int i=0; i<MAX_ANIM_NUM; i++ )
		if( vmd[i]!=NULL && i!=key )
			vmd[i]->playFlag = 0;

	VmdBoneNode* node = vmd[key]->boneNode;
	int i,j;
	for( i=0; i<(int)vmd[key]->boneNodeNum; i++,node++ )
	{
		for( j=0; j<pmx->boneNum; j++ )
			if( !strncmp( node->name, pmx->bone[j].name, 32 ) )
				break;
					
		if( j!=pmx->boneNum )
		{
			boneInfo[j].node[key] = node;
			boneInfo[j].nowKey[key] = node->firstKey;
		}
	}
}

void Model::stopAnim( int key )
{
	vmd[ key ]->playSpeed = 0.0f;
}

void Model::setAnimSpeed( int key, float speed )
{
	vmd[ key ]->playSpeed = speed;
}

int Model::LoadPose( const char* filename )
{
	if( pmx==NULL )
	{
		cout << "Model Error : ポーズデータ読み込み時にモデルが読み込まれていません。" << endl;
		return -1;
	}

	int index;
	for( index=0; index<MAX_POSE_NUM; index++ )
		if( !vpd[index] )
			break;
	if( index==MAX_POSE_NUM )
	{
		cout << "Model Error : アタッチするポーズデータが多すぎます。" << endl;
		return -1;
	}

	vpd[index] = new Vpd();
	if( !vpd[index]->Load( filename ) )
	{
		cout << "Model Error : VPDファイル" << filename << "の読み込みに失敗しました。" << endl;
		delete vpd[index];
		vpd[index] = NULL;
		return -1;
	}

	for( int i=0; i<vpd[index]->boneNum; i++ )
	{
		int j;
		for( j=0; j<pmx->boneNum; j++ )
			if( !strcmp(vpd[index]->bone[i].name, pmx->bone[j].name) )
				break;
		if( j<pmx->boneNum )
			vpd[index]->bone[i].index = j;
		else
		{
			vpd[index]->bone[i].index = -1;
			//cout << "Model Warnning : VPDで使用されているボーン" << vpd[index]->bone[i].name << "が見つかりませんでした。" << endl;
		}
	}

	return index;
}

void Model::startPose( int key )
{
	if( vpd[key] )
		vpd[key]->useFlag = true;
}

void Model::stopPose( int key )
{
	if( vpd[key] )
		vpd[key]->useFlag = false;
}

void Model::Render()
{
	int i;

	if( !pmx )
		return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glNormalPointer(GL_FLOAT, 0, 0); 
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0); 

	GLuint typeID = glGetUniformLocation(shaderProg,"drawType");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	uint materialCount = 0;
	int type = 0;
	for( i=0; i<pmx->materialNum; i++ )
	{
		type = 0;
		glActiveTexture(GL_TEXTURE0);
		if( pmx->material[i].texID != 0xFFFFFFFF )
		{
			glBindTexture(GL_TEXTURE_2D , pmx->material[i].texID);
			type += 1;
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		if( pmx->material[i].sphereMode==1 && pmx->material[i].sphereTexID != 0xFFFFFFFF )
		{
			glBindTexture(GL_TEXTURE_2D , pmx->material[i].sphereTexID);
			type += 2;
		}
		else if( pmx->material[i].sphereMode==2 && pmx->material[i].sphereTexID != 0xFFFFFFFF )
		{
			glBindTexture(GL_TEXTURE_2D , pmx->material[i].sphereTexID);
			type += 4;
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		if( pmx->material[i].toonTexID != 0xFFFFFFFF )
		{
			glBindTexture(GL_TEXTURE_2D , pmx->material[i].toonTexID);
			type += 8;
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0);

		glUniform1f(typeID, (float)type);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pmx->material[i].ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pmx->material[i].diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pmx->material[i].specular);

		glDrawElements(GL_TRIANGLES, pmx->material[i].materialFaceNum, 
				GL_UNSIGNED_INT, BUFFER_OFFSET(materialCount*sizeof(unsigned int)) );
		materialCount += pmx->material[i].materialFaceNum;
	}

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glActiveTexture (GL_TEXTURE0);
}

void Model::UpdateBoneRot()
{
	int i;
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];

		Quaternion rotate = Quaternion();
		
		if( pmx->bone[index].flag_AddRot )
		{
			if( pmx->bone[index].flag_AddLocal )
				rotate *= boneInfo[ pmx->bone[index].addParentBoneIndex ].localRot;
			else
			{
				if( pmx->bone[ pmx->bone[index].addParentBoneIndex ].flag_AddRot )
				{
					rotate *= QuaternionSlerp( Quaternion(), boneInfo[ pmx->bone[index].addParentBoneIndex ].localRot, pmx->bone[ pmx->bone[index].addParentBoneIndex ].addRatio );
				}
				else
				{
					rotate *= boneInfo[ pmx->bone[index].addParentBoneIndex ].localRot;
				}
			}
			if( pmx->bone[index].addRatio != 1.0f )
				rotate = QuaternionSlerp( Quaternion(), rotate, pmx->bone[index].addRatio );
		}
		rotate *= boneInfo[index].localRot;
		boneInfo[index].localRot = rotate;
	}
}

void Model::UpdateBonePos()
{
	for( int i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[i];
		int parentIndex = pmx->bone[index].parentBoneIndex;
		if( !boneInfo[index].isCenter && !pmx->bone[index].flag_IK  )
		{
			if( parentIndex>=0 && parentIndex<pmx->boneNum )
			{
				boneInfo[index].worldRot = boneInfo[parentIndex].worldRot * boneInfo[index].localRot;
				boneInfo[index].worldPos = boneInfo[parentIndex].worldPos + boneInfo[parentIndex].rotMat * boneInfo[index].localPos;
			}
			else
			{
				boneInfo[index].worldRot = boneInfo[index].localRot;
				boneInfo[index].worldPos = pmx->bone[index].pos + boneInfo[index].localPos;
			}
		}
		else
		{
			if( parentIndex>=0 && parentIndex<pmx->boneNum )
			{
				boneInfo[index].worldRot = boneInfo[parentIndex].worldRot * boneInfo[index].localRot;
				boneInfo[index].worldPos = boneInfo[parentIndex].worldPos + boneInfo[parentIndex].rotMat * (pmx->bone[index].pos-pmx->bone[parentIndex].pos) + boneInfo[index].localPos;
			}
			else
			{
				boneInfo[index].worldRot = boneInfo[index].localRot;
				boneInfo[index].worldPos = pmx->bone[index].pos + boneInfo[index].localPos;
			}
		}
		boneInfo[index].rotMat = QuatToMatrix( boneInfo[index].worldRot );
	}
}

void Model::UpdateIK( int index )
{
	UpdateBonePos();

	int targetIndex = pmx->bone[index].ikInfo.targetBoneIndex;
	int targetParentIndex = pmx->bone[targetIndex].parentBoneIndex;
	Vector3f targetPos = boneInfo[targetIndex].worldPos;
	Vector3f IKPos = boneInfo[index].worldPos;

	int loopNum = pmx->bone[index].ikInfo.loopNum;
	int boneIndex = 0;
	int i,j,k;
	for( i=0; i<loopNum; i++ )
	{
		for( j=0; j<pmx->bone[index].ikInfo.linkNum; j++ )
		{
			boneIndex = pmx->bone[index].ikInfo.link[j].boneIndex;

			Vector3f v1,v2,axis;
			float theta;
			Quaternion quat = Quaternion();

			v1 = IKPos - boneInfo[boneIndex].worldPos;
			v2 = targetPos - boneInfo[boneIndex].worldPos;

			if( v1.length() < FLT_EPSILON || v2.length() < FLT_EPSILON )
				theta = 0.0f;
			else
				theta = acosf( v1.dot( v2 ) / ( v1.length() * v2.length() ) );

			if( abs(theta)>FLT_EPSILON )
			{
				if( theta > pmx->bone[index].ikInfo.rotLimit )
					theta = pmx->bone[index].ikInfo.rotLimit;
				if( theta < -pmx->bone[index].ikInfo.rotLimit )
					theta = -pmx->bone[index].ikInfo.rotLimit;

				axis = v1.cross(v2).normalize();
				
				theta /= 2.0f;
				float s = sin(theta);
				quat.x = axis.x * s;
				quat.y = axis.y * s;
				quat.z = axis.z * s;
				quat.w = cos(theta);
				if( !strcmp(pmx->bone[boneIndex].name,"左ひざ") || !strcmp(pmx->bone[boneIndex].name,"右ひざ") )
				{
					// 角度制限
					Vector3f vec = QuatToEuler( quat );
					if( vec.x < -3.14159f )	vec.x = -3.14159f;
					if( -0.002f < vec.x )		vec.x = -0.002f;
					vec.y = 0.0f;
					vec.z = 0.0f;
					quat = EulerToQuat( vec );
				}
				boneInfo[boneIndex].localRot *= quat;
				boneInfo[boneIndex].IKQuat *= quat;
			}

			if( quat != Quaternion() )
			{
				for( k = pmx->bone[index].ikInfo.linkNum-1; k>=0; k-- )
				{
					boneIndex = pmx->bone[index].ikInfo.link[k].boneIndex;
					int parentIndex = pmx->bone[boneIndex].parentBoneIndex;
						
					if( parentIndex>=0 && parentIndex<pmx->boneNum )
					{
						boneInfo[boneIndex].worldRot = boneInfo[parentIndex].worldRot * boneInfo[boneIndex].localRot;
						boneInfo[boneIndex].worldPos = boneInfo[parentIndex].worldPos + boneInfo[parentIndex].rotMat * boneInfo[boneIndex].localPos;
					}
					else
					{
						boneInfo[boneIndex].worldRot = boneInfo[boneIndex].localRot;
						boneInfo[boneIndex].worldPos = pmx->bone[boneIndex].pos + boneInfo[boneIndex].localPos;
					}
					boneInfo[boneIndex].rotMat = QuatToMatrix( boneInfo[boneIndex].worldRot );
				}
				targetPos = boneInfo[targetParentIndex].worldPos + boneInfo[targetParentIndex].rotMat * boneInfo[targetIndex].localPos;
			}
			if( (targetPos-IKPos).length()<0.001f )
				break;
		}
	}
}

void Model:: UpdateBoneByVmd()
{
	int i,j;

	// VMDファイルの適応
	for( i=0; i<MAX_ANIM_NUM; i++ )
	{
		if( !vmd[i] )
			continue;

		if( vmd[i]->playTime >= (float)vmd[i]->playMaxTime && vmd[i]->playMaxTime!=0 )
		{
			if( !vmd[i]->loopFlag )
			{
				ReleaseAnim( i );
			}
			else
			{
				startAnim(i);
			}
			continue;
		}

		if( vmd[i]->playFlag != 1 )
			continue;

		for( j=0; j<pmx->boneNum; j++ )
		{
			if( boneInfo[j].node[i]!=NULL )
			{
				if( boneInfo[j].nowKey[i] != NULL && boneInfo[j].nowKey[i]->next != NULL )
				{
					Quaternion quat = Quaternion();
					Vector3f vec = Vector3f();

					uint t0, t1;
					Quaternion q0,q1;
					Vector3f v0,v1;
					t0 = boneInfo[j].nowKey[i]->frame;
					q0 = boneInfo[j].nowKey[i]->quat;
					v0 = boneInfo[j].nowKey[i]->pos;

					t1 = boneInfo[j].nowKey[i]->next->frame;
					q1 = boneInfo[j].nowKey[i]->next->quat;
					v1 = boneInfo[j].nowKey[i]->next->pos;

					float s = (vmd[i]->playTime - t0)/(t1 - t0);
					Vector2f pos1, pos2;
					float weightX, weightY, weightZ, weightRot;

					pos1 = boneInfo[j].nowKey[i]->bezierPosX[0];
					pos2 = boneInfo[j].nowKey[i]->bezierPosX[1];
					weightX = bezier( pos1.x, pos1.y, pos2.x, pos2.y, s );
					pos1 = boneInfo[j].nowKey[i]->bezierPosY[0];
					pos2 = boneInfo[j].nowKey[i]->bezierPosY[1];
					weightY = bezier( pos1.x, pos1.y, pos2.x, pos2.y, s );
					pos1 = boneInfo[j].nowKey[i]->bezierPosZ[0];
					pos2 = boneInfo[j].nowKey[i]->bezierPosZ[1];
					weightZ = bezier( pos1.x, pos1.y, pos2.x, pos2.y, s );
					pos1 = boneInfo[j].nowKey[i]->bezierRot[0];
					pos2 = boneInfo[j].nowKey[i]->bezierRot[1];
					weightRot = bezier( pos1.x, pos1.y, pos2.x, pos2.y, s );

					float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
					if( dot >= 0.0f )
						quat = QuaternionSlerp(q0, q1, weightRot);
					else
						quat = QuaternionSlerp(-q0, q1, weightRot);
					vec = Vector3f(
						v0.x + (v1.x-v0.x)*weightX,
						v0.y + (v1.y-v0.y)*weightY,
						v0.z + (v1.z-v0.z)*weightZ
					) * scale;

					if( vmd[i]->playTime >= t1-1.0f )
						boneInfo[j].nowKey[i] = boneInfo[j].nowKey[i]->next;

					boneInfo[j].localRot = quat;
					if( boneInfo[j].isCenter)
					{
						vec.z = 0.0f;
						boneInfo[j].localPos = vec;
					}
					else if( pmx->bone[j].flag_IK )
					{
						boneInfo[j].localPos = vec;
					}
				}
				else if( boneInfo[j].nowKey[i] != NULL )
				{
					boneInfo[j].localRot = boneInfo[j].nowKey[i]->quat;
					Vector3f vec = boneInfo[j].nowKey[i]->pos * scale;
					if( boneInfo[j].isCenter)
					{
						boneInfo[j].localPos = vec;
					}
					else if( pmx->bone[j].flag_IK )
					{
						boneInfo[j].localPos = vec;
					}
				}
			}
		}
		vmd[i]->playTime += vmd[i]->playSpeed;
	}

	UpdateBonePos();

	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[i];

		if( pmx->bone[index].flag_IK )
		{
			int parentIndex = pmx->bone[index].parentBoneIndex;

			if( boneInfo[index].localPos.length()>0.0f || boneInfo[parentIndex].localPos.length()>0.0f
				|| boneInfo[parentIndex].localRot != Quaternion())
				UpdateIK(index);
		}
	}
}

void Model::UpdateBoneByVpd()
{
	if( vpd == NULL )
		return;

	for( int i=0; i<MAX_POSE_NUM; i++ )
	{
		if( vpd[i]==NULL || !vpd[i]->useFlag )
			continue;

		for( int j=0; j<vpd[i]->boneNum; j++ )
		{
			int index = vpd[i]->bone[j].index;
			if( index == -1 )
				continue;

			boneInfo[index].localRot = vpd[i]->bone[j].rot;

			if( boneInfo[index].isCenter )
				boneInfo[index].localPos = vpd[i]->bone[j].trans * scale;

			if( pmx->bone[index].flag_IK )
				boneInfo[index].localPos = vpd[i]->bone[j].trans * scale;
		}
	}
	UpdateBonePos();

	for( int i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[i];

		if( pmx->bone[index].flag_IK )
		{
			int parentIndex = pmx->bone[index].parentBoneIndex;

			if( boneInfo[index].localPos.length()>0.0f || boneInfo[parentIndex].localPos.length()>0.0f )
				UpdateIK(index);
		}
	}
}

void Model::Update()
{
	int i,j;
	for( i=0; i<pmx->boneNum; i++ )
	{
		int index = boneIndexList[ i ];
		boneInfo[index].localRot = Quaternion();
		boneInfo[index].IKQuat = Quaternion();
		if( boneInfo[index].isCenter ||
			pmx->bone[index].parentBoneIndex<0 || pmx->bone[index].parentBoneIndex>=pmx->boneNum ||
			pmx->bone[index].flag_IK )
			boneInfo[index].localPos = Vector3f();
	}

	bool animFlag = false;
	for( i=0; i<MAX_ANIM_NUM; i++ )
		if( vmd[i] && vmd[i]->playFlag == 1 )
		{
			animFlag = true;
			break;
		}
	if( animFlag )
	{
		UpdateBoneByVmd();
		UpdateBoneRot();
	}
	else
		UpdateBoneByVpd();

	if( kinectUseFlag )
	{
		for( int i=1; i<2; i++ )
		{
			int index = kinectBoneIndex[i];
			if( index!=-1 )
				boneInfo[index].localRot = kinectBoneRot[i];
		}
		//boneInfo[kinectBoneIndex[1]].localRot = Quaternion();
	}

	if( lookAtFlag )
	{
		boneInfo[headBoneIndex].localRot = headRot;
	}

	UpdateBonePos();
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	float *ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	if( ptr )
	{
		for( i=0; i<pmx->vertexNum; i++ )
		{
			Vector3f pos = Vector3f();
			for( j=0; j<4; j++ )
				if( pmx->vertex[i].boneIndex[j]!=-1 )
				{
					int boneIndex = pmx->vertex[i].boneIndex[j];
					pos += ( ( boneInfo[ boneIndex ].worldPos
						+ boneInfo[ boneIndex ].rotMat
						* (pmx->vertex[i].pos - pmx->bone[ boneIndex ].pos) ) * pmx->vertex[i].boneWeight[j] );
				}
			ptr[i*3+0] = pos.x;
			ptr[i*3+1] = pos.y;
			ptr[i*3+2] = pos.z;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Model::UpdateKinectBone( Quaternion* quat )
{
	for( int i=0; i<15; i++ )
		kinectBoneRot[i] = quat[i];
}

void Model::UpdateHeadBone( Quaternion quat )
{
	headRot = quat;
}

int Model::GetBoneIndex( char* boneName )
{
	for( int i=0; i<pmx->boneNum; i++ )
		if( !strcmp( pmx->bone[i].name, boneName ) )
			return i;
	return -1;
}

int Model::GetBonePos( char* boneName, Vector3f* pos )
{
	for( int i=0; i<pmx->boneNum; i++ )
	{
		if( !strcmp( pmx->bone[i].name, boneName ) )
		{
			*pos = boneInfo[i].worldPos;
			return i;
		}
	}
	return -1;
}

void Model::GetBonePos( int index, Vector3f* pos )
{
	*pos = boneInfo[index].worldPos;
}

int Model::GetBoneRot( char* boneName, Quaternion* rot )
{
	for( int i=0; i<pmx->boneNum; i++ )
	{
		if( !strcmp( pmx->bone[i].name, boneName ) )
		{
			*rot = boneInfo[i].localRot;
			return i;
		}
	}
	return -1;
}

void Model::GetBoneRot( int index, Quaternion* rot )
{
	*rot = boneInfo[index].localRot;
}

void Model::GetBoneWRot( int index, Quaternion* rot )
{
	*rot = boneInfo[index].worldRot;
}