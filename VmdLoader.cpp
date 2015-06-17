#include "VmdLoader.h"

Vmd::Vmd()
{
	boneKeyNum		= 0;
	boneNodeNum		= 0;
	morphKeyNum		= 0;
	morphNodeNum	= 0;
	boneKey			= NULL;
	boneNode		= NULL;
	morphKey		= NULL;
	morphNode		= NULL;

	playFlag = 0;
	playTime = 0.0;
	playMaxTime = 0;
	playSpeed = 1.0;
	ratio = 0.0;

	srcHead = NULL;
	src = NULL;
}

Vmd::~Vmd()
{
	Release();
}

char Vmd::ReadChar()
{
	char c = (char)*src;
	src++;
	return c;
}

int Vmd::ReadInt()
{
	int buff[4] = {0};
	for( int i=0; i<4; i++ )
		buff[i] = *(src+i);
	src += 4;
	return buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);
}

float Vmd::ReadFloat()
{
	float result;
	memcpy_s( &result, 4, src, 4 );
	src += 4;
	return result;
}

bool Vmd::Load( const char* path )
{
	FILE *fp;
	uint i,j;
	byte version = 0;
	struct stat stat_buf;
	int fileSize;

	/*uint cameraKeyNum;
	uint lightKeyNum;
	uint selfShadowKeyNum;
	VmdCameraKeyFrame*		cameraKey		= NULL;
	VmdLightKeyFrame*		lightKey		= NULL;
	VmdSelfShadowKeyFrame*	selfShadowKey	= NULL;
	*/

	if( srcHead )
		Release();

	if( fopen_s( &fp, path, "rb" ) )
	{
		cout << "VMD Loader Error : can't open file: " << path << endl;
		return false;
	}
	fstat( _fileno(fp), &stat_buf );
	fileSize = stat_buf.st_size;
	src = new byte[ fileSize ];
	fread_s( src, fileSize, fileSize, 1, fp );
	fclose(fp);

	srcHead = src;

	// ヘッダチェック
	if( strncmp( (char*)src, "Vocaloid Motion Data ", 21 ) )
	{
		cout << "VMD Loader Error : file: " << path << " is not VMD file" << endl;
		return false;
	}
	else
	{
		// Versionチェック
		if( !strncmp( (char*)src+21, "file", 4 ) )
			version = 1;
		else if( !strncmp( (char*)src+21, "0002", 4 ) )
			version = 2;
		else
		{
			cout << "VMD Loader Error : file: " << path << "'s VMD version is not correct" << endl;
			return false;
		}
	}
	src += 30;

	// モデル名読み飛ばし
	if( version==1 )
		src += 10;
	else if( version==2 )
		src += 20;

	// ボーンキーフレームの読み込み
	boneKeyNum = ReadInt();
	boneNodeNum = 0;
	boneNode = new VmdBoneNode[ 1024 ];
	if( boneKeyNum )
	{
		boneKey = new VmdBoneKeyFrame[ boneKeyNum ];

		VmdBoneKeyFrame* key = boneKey;
		for( i=0; i<boneKeyNum; i++,key++ )
		{
			VmdBoneNode* node = boneNode;

			// ノードが既に登録されているか調べる
			for( j=0; j<boneNodeNum; j++,node++ )
				if( !strncmp( (char*)src, node->name, 15 ) )
					break;

			// 無かったら登録する
			if( j==boneNodeNum )
			{
				node->keyNum	= 0;
				node->maxFrame	= 0;
				node->minFrame	= 0xFFFFFFFF;
				memcpy_s( node->name, 16, src, 15 );
				node->name[15]	= '\0';
				node->firstKey	= NULL;
				boneNodeNum++;
			}
			src += 15;

			// キーの情報を格納する
			key->frame = ReadInt();
			key->pos.x = ReadFloat();
			key->pos.y = ReadFloat();
			key->pos.z = -ReadFloat();
			key->quat.x = -ReadFloat();
			key->quat.y = -ReadFloat();
			key->quat.z = ReadFloat();
			key->quat.w = -ReadFloat();
			
			key->bezierPosX[0].x = (float)src[0]/127.0f;
			key->bezierPosX[0].y = (float)src[4]/127.0f;
			key->bezierPosX[1].x = (float)src[8]/127.0f;
			key->bezierPosX[1].y = (float)src[12]/127.0f;
			key->bezierPosY[0].x = (float)src[1]/127.0f;
			key->bezierPosY[0].y = (float)src[5]/127.0f;
			key->bezierPosY[1].x = (float)src[9]/127.0f;
			key->bezierPosY[1].y = (float)src[13]/127.0f;
			key->bezierPosZ[0].x = (float)src[2]/127.0f;
			key->bezierPosZ[0].y = (float)src[6]/127.0f;
			key->bezierPosZ[1].x = (float)src[10]/127.0f;
			key->bezierPosZ[1].y = (float)src[14]/127.0f;
			key->bezierRot[0].x = (float)src[3]/127.0f;
			key->bezierRot[0].y = (float)src[7]/127.0f;
			key->bezierRot[1].x = (float)src[11]/127.0f;
			key->bezierRot[1].y = (float)src[15]/127.0f;

			src += 64;

			// ノードにキーを格納する
			if( node->firstKey == NULL )
			{
				node->firstKey = key;
				key->prev = NULL;
				key->next = NULL;
			}
			else
			{
				// 格納場所を探す
				VmdBoneKeyFrame* tempKey1 = NULL;
				VmdBoneKeyFrame* tempKey2 = node->firstKey;
				for( j=0; j<node->keyNum; j++ )
				{
					if( tempKey2->frame > key->frame )
						break;
					tempKey1 = tempKey2;
					tempKey2 = tempKey2->next;
				}
				// 末尾の場合
				if( j == node->keyNum )
				{
					key->prev		= tempKey1;
					tempKey1->next	= key;
					key->next		= NULL;
				}
				else
				{
					key->prev		= tempKey1;
					key->next		= tempKey2;
					tempKey2->prev	= key;
					if( key->prev == NULL )
						node->firstKey = key;
					else
						key->prev->next = key;
				}
			}

			// キーの数を加算する
			node->keyNum++;

			// 最大フレーム番号と最小フレーム番号を更新
			if( node->maxFrame < key->frame )
			{
				node->maxFrame = key->frame;
				if( node->maxFrame > playMaxTime )
					playMaxTime = node->maxFrame;
			}
			if( node->minFrame > key->frame )
				node->minFrame = key->frame;
		}
	}

	// モーフキーフレームの読み込み
	morphKeyNum = ReadInt();
	morphNodeNum = 0;
	morphNode = new VmdMorphNode[ 512 ];
	if( morphKeyNum )
	{
		morphKey = new VmdMorphKeyFrame[ morphKeyNum ];

		VmdMorphKeyFrame* key = morphKey;
		for( i=0; i<morphKeyNum; i++,key++ )
		{
			VmdMorphNode* node = morphNode;

			// ノードが既に登録されているか調べる
			for( j=0; j<morphNodeNum; j++,node++ )
				if( !strncmp( (char*)src, node->name, 15 ) )
					break;

			// 無かったら登録する
			if( j==morphNodeNum )
			{
				node->keyNum	= 0;
				node->maxFrame	= 0;
				node->minFrame	= 0xFFFFFFFF;
				memcpy_s( node->name, 16, src, 15 );
				node->name[15]	= '\0';
				node->firstKey	= NULL;
				morphNodeNum++;
			}
			src += 15;

			key->frame = ReadInt();
			key->value = ReadFloat();

			// ノードにキーを格納する
			if( node->firstKey == NULL )
			{
				node->firstKey = key;
				key->prev = NULL;
				key->next = NULL;
			}
			else
			{
				// 格納場所を探す
				VmdMorphKeyFrame* tempKey1 = NULL;
				VmdMorphKeyFrame* tempKey2 = node->firstKey;
				for( j=0; j<node->keyNum; j++ )
				{
					if( tempKey2->frame > key->frame )
						break;
					tempKey1 = tempKey2;
					tempKey2 = tempKey2->next;
				}
				// 末尾の場合
				if( j == node->keyNum )
				{
					key->prev		= tempKey1;
					tempKey1->next	= key;
					key->next		= NULL;
				}
				else
				{
					key->prev		= tempKey1;
					key->next		= tempKey2;
					tempKey2->prev	= key;
					if( key->prev == NULL )
						node->firstKey = key;
					else
						key->prev->next = key;
				}
			}

			// キーの数を加算する
			node->keyNum++;

			// 最大フレーム番号と最小フレーム番号を更新
			if( node->maxFrame < key->frame )
				node->maxFrame = key->frame;
			if( node->minFrame > key->frame )
				node->minFrame = key->frame;
		}
	}
	/*
	// カメラキーフレームの読み込み
	cameraKeyNum = ReadInt();
	if( cameraKeyNum )
	{
		cameraKey = new VmdCameraKeyFrame[ cameraKeyNum ];

		for( i=0; i<cameraKeyNum; i++ )
		{
			cameraKey[i].frame = ReadInt();
			cameraKey[i].distance = ReadFloat();
			cameraKey[i].pos.x = ReadFloat();
			cameraKey[i].pos.y = ReadFloat();
			cameraKey[i].pos.z = -ReadFloat();
			cameraKey[i].rot.x = -ReadFloat();
			cameraKey[i].rot.y = -ReadFloat();
			cameraKey[i].rot.z = ReadFloat();
			memcpy_s( cameraKey[i].bezier, 24, src, 24 );
			src += 24;
			cameraKey[i].viewAngle = ReadInt();
			cameraKey[i].persFlag = ReadChar(); 
		}

		// 使わないので削除
		delete[] cameraKey;
		cameraKey = NULL;
	}

	// ライトキーフレームの読み込み
	lightKeyNum = ReadInt();
	if( lightKeyNum )
	{
		lightKey = new VmdLightKeyFrame[ lightKeyNum ];

		for( i=0; i<lightKeyNum; i++ )
		{
			lightKey[i].frame = ReadInt();
			lightKey[i].color.r = ReadFloat();
			lightKey[i].color.g = ReadFloat();
			lightKey[i].color.b = ReadFloat();
			lightKey[i].pos.x = ReadFloat();
			lightKey[i].pos.y = ReadFloat();
			lightKey[i].pos.z = -ReadFloat();
		}

		// 使わないので削除
		delete[] lightKey;
		lightKey = NULL;
	}

	// ファイルの終了判定
	if( src - srcHead == fileSize )
		return true;

	// セルフシャドウキーフレームの読み込み
	selfShadowKeyNum = ReadInt();
	if( selfShadowKeyNum )
	{
		selfShadowKey = new VmdSelfShadowKeyFrame[ selfShadowKeyNum ];

		for( i=0; i<selfShadowKeyNum; i++ )
		{
			selfShadowKey[i].frame = ReadInt();
			selfShadowKey[i].type = ReadChar();
			selfShadowKey[i].distance = ReadFloat();
		}
		// 使わないので削除
		delete[] selfShadowKey;
		selfShadowKey = NULL;
	}*/

	delete[] srcHead;
	srcHead = NULL;
	src = NULL;

	return true;
}

void Vmd::Release()
{
	boneKeyNum		= 0;
	boneNodeNum		= 0;
	morphKeyNum		= 0;
	morphNodeNum	= 0;

	if( boneKey )
	{
		delete[] boneKey;
		boneKey = NULL;
	}
	if( morphKey )
	{
		delete[] morphKey;
		morphKey = NULL;
	}
	if( boneNode )
	{
		delete[] boneNode;
		boneNode = NULL;
	}
	if( morphNode )
	{
		delete[] morphNode;
		morphNode = NULL;
	}

	if( srcHead )
	{
		delete[] srcHead;
		srcHead = NULL;
		src = NULL;
	}
}