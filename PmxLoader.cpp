#include "PmxLoader.h"

char* ConvUtf16toSjis( char* utf16 )
{
	char *sjis;
	const int nSizeSJis = ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)utf16, -1, NULL, 0, NULL, NULL );
	sjis = new char[ nSizeSJis ];
	ZeroMemory( sjis, nSizeSJis );
	::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)utf16, -1, (LPSTR)sjis, nSizeSJis, NULL, NULL );
	sjis[ nSizeSJis-1 ] = '\0';
	return sjis;
}

char* ConvUtf8toSjis( char* utf8 )
{
	char *utf16, *sjis;
	const int nSize = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)utf8, -1, NULL, 0 );
	utf16 = new char[ nSize*2 + 2 ];
	::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)utf8, -1, (LPWSTR)utf16, nSize );
	sjis = ConvUtf16toSjis( utf16 );
	delete[] utf16;
	return sjis;
}

byte PmxModel::ReadByte()
{
	byte result = *src;
	src++;
	return result;
}

short PmxModel::ReadShort()
{
	byte buff[2] = {0};
	buff[0] = *src;
	buff[1] = *(src+1);
	src += 2;
	return (short)(buff[0]*256 + buff[1]);
}

int PmxModel::ReadInt( int size=4 )
{
	int buff[4] = {0};
	for( int i=0; i<size; i++ )
		buff[i] = *(src+i);
	src += size;
	return buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);
}

float PmxModel::ReadFloat()
{
	float result;
	memcpy_s( &result, 4, src, 4 );
	src += 4;
	return result;
}

char* PmxModel::ReadString( int encodeType=-1 )
{

	byte lenBuff[4] = {0};
	for( int i=0; i<4; i++ )
		lenBuff[i] = *(src+i);
	src += 4;
	int len = lenBuff[0] + lenBuff[1]*256 + lenBuff[2]*65536 + lenBuff[3]*16777216;

	if( len!=0 ){
		char* buff = new char[len+2];
		char* result = NULL;
		memcpy_s( buff, len+2, src, len );
		src += len;
		
		buff[len] = '\0';
		buff[len+1] = '\0';

		switch( encodeType )
		{
		case 0:
			result = ConvUtf16toSjis( buff );
			delete[] buff;
			break;
		case 1:
			result = ConvUtf8toSjis( buff );
			delete[] buff;
			break;
		default:
			return buff;
		}

		return result;
	}
	else
		return NULL;
}

PmxModel::PmxModel()
{
	src				= NULL;
	srcHead			= NULL;

	vertexNum		= 0;
	faceNum			= 0;
	materialNum		= 0;
	boneNum			= 0;
	ikNum			= 0;
	morphNum		= 0;
	skinNum			= 0;
	rigidBodyNum	= 0;
	jointNum		= 0;
	vertex			= NULL;
	face			= NULL;
	material		= NULL;
	bone			= NULL;
	morph			= NULL;
	rigidBody		= NULL;
	joint			= NULL;
}

PmxModel::~PmxModel()
{
	Release();
}

void PmxModel::Release()
{
	if( srcHead ){
		delete[] srcHead;
		src		= NULL;
		srcHead	= NULL;
	}

	vertexNum		= 0;
	faceNum			= 0;
	materialNum		= 0;
	boneNum			= 0;
	ikNum			= 0;
	morphNum		= 0;
	skinNum			= 0;
	rigidBodyNum	= 0;
	jointNum		= 0;

	if( vertex )
	{
		delete[] vertex;
		vertex = NULL;
	}
	if( face )
	{
		delete[] face;
		face = NULL;
	}
	if( material )
	{
		delete[] material;
		material = NULL;
	}
	if( bone )
	{
		delete[] bone;
		bone = NULL;
	}
	if( morph )
	{
		delete[] morph;
		morph = NULL;
	}
	if( rigidBody )
	{
		delete[] rigidBody;
		rigidBody = NULL;
	}
	if( joint )
	{
		delete[] joint;
		joint = NULL;
	}
}

bool PmxModel::Load( const char* filename )
{
	int i,j;
	int fileSize;
	char* dirName;
	int version;
	byte encodeType;
	byte addUVNum;
	byte vertexIndexSize;
	byte textureIndexSize;
	byte materialIndexSize;
	byte boneIndexSize;
	byte morphIndexSize;
	byte rigidIndexSize;

	int textureNum = 0;
	PmxTexture* texture = NULL;

	// open file
	FILE *fp;
	if( fopen_s(&fp, filename, "rb")!=0 )
	{
		cout << "PMX Loader Error : ファイル" << filename << "が開けません。" << endl;
		return false;
	}

	// read file to buffer
	struct stat stat_buf;
	fstat( _fileno(fp), &stat_buf );
	fileSize = stat_buf.st_size;
	src = new byte[ fileSize ];
	fread_s( src, fileSize, fileSize, 1, fp );
	fclose(fp);
	srcHead = src;

	// get relative directory path
	char buff[1024];
	strcpy_s( buff, filename );
	char* p = strrchr( buff, '\\' );
	if( p )
	{
		while( strrchr( p+1, '\\' ) )
			p = strrchr( buff, '\\' );
		int dirNameSize = p - buff + 1;
		dirName = new char[ dirNameSize+1 ];
		strncpy_s( dirName, dirNameSize+1, buff, dirNameSize );
		dirName[ dirNameSize ] = '\0';
	}
	else{
		dirName = new char[1];
		*dirName = '\0';
	}

	// check header
	if( src[0] != 'P' || src[1] != 'M' || src[2] != 'X' || src[3] != ' ' )
	{
		cout << "PMX Loader Error : ファイル" << filename << "はPMXファイルではありません。" << endl;
		Release();
		return false;
	}
	if( src[4] != 0x00 || src[5] != 0x00 || src[6] != 0x00 || src[7] != 0x40 )
	{
		cout << "PMX Loader Error : PMXのversionが2.0ではありません。" << endl;
		Release();
		return false;
	}
	else
	{
		version = 20;
	}
	src += 8;

	// read add header
	byte addHeadDataSize = *src;
	src++;

	encodeType			= src[0];
	addUVNum			= src[1];
	vertexIndexSize		= src[2];
	textureIndexSize	= src[3];
	materialIndexSize	= src[4];
	boneIndexSize		= src[5];
	morphIndexSize		= src[6];
	rigidIndexSize		= src[7];

	src += addHeadDataSize;

	// read modelname and comment
	char* modelJaName = ReadString();
	char* modelEnName = ReadString();
	char* jaComment = ReadString();
	char* enComment = ReadString();
	delete[] modelJaName;
	delete[] modelEnName;
	delete[] jaComment;
	delete[] enComment;

	// read vertex info
	vertexNum = ReadInt();
	if( vertexNum>0 )
	{
		vertex = new PmxVertex[vertexNum];

		for( i=0; i < vertexNum; i++ )
		{
			vertex[i].pos.x = ReadFloat();
			vertex[i].pos.y = ReadFloat();
			vertex[i].pos.z = -ReadFloat();
			vertex[i].normal.x = ReadFloat();
			vertex[i].normal.y = ReadFloat();
			vertex[i].normal.z = -ReadFloat();
			vertex[i].uv.u = ReadFloat();
			vertex[i].uv.v = ReadFloat();

			for( j=0; j<addUVNum; j++ )
			{
				vertex[i].addUV[j].x = ReadFloat();
				vertex[i].addUV[j].y = ReadFloat();
				vertex[i].addUV[j].z = ReadFloat();
				vertex[i].addUV[j].w = ReadFloat();
			}
			vertex[i].weightType = ReadByte();
			vertex[i].boneIndex[1] = -1;
			vertex[i].boneIndex[2] = -1;
			vertex[i].boneIndex[3] = -1;
			vertex[i].boneWeight[2] = 0.0f;
			vertex[i].boneWeight[3] = 0.0f;

			switch( vertex[i].weightType )
			{
			case 0:
				vertex[i].boneIndex[0]  = ReadInt(boneIndexSize);
				vertex[i].boneWeight[0] = 1.0f;
				vertex[i].boneWeight[1] = 0.0f;
				break;

			case 1:
				vertex[i].boneIndex[0] = ReadInt(boneIndexSize);
				vertex[i].boneIndex[1] = ReadInt(boneIndexSize);
				vertex[i].boneWeight[0] = ReadFloat();
				vertex[i].boneWeight[1] = 1.0f - vertex[i].boneWeight[0];
				break;

			case 2:
				vertex[i].boneIndex[0] = ReadInt(boneIndexSize);
				vertex[i].boneIndex[1] = ReadInt(boneIndexSize);
				vertex[i].boneIndex[2] = ReadInt(boneIndexSize);
				vertex[i].boneIndex[3] = ReadInt(boneIndexSize);
				vertex[i].boneWeight[0] = ReadFloat();
				vertex[i].boneWeight[1] = ReadFloat();
				vertex[i].boneWeight[2] = ReadFloat();
				vertex[i].boneWeight[3] = ReadFloat();
				break;

			case 3:
				vertex[i].boneIndex[0] = ReadInt(boneIndexSize);
				vertex[i].boneIndex[1] = ReadInt(boneIndexSize);
				vertex[i].boneWeight[0] = ReadFloat();
				vertex[i].boneWeight[1] = 1.0f - vertex[i].boneWeight[0];
				vertex[i].SDEF_C.x = ReadFloat();
				vertex[i].SDEF_C.y = ReadFloat();
				vertex[i].SDEF_C.z = -ReadFloat();
				vertex[i].SDEF_R0.x = ReadFloat();
				vertex[i].SDEF_R0.y = ReadFloat();
				vertex[i].SDEF_R0.z = -ReadFloat();
				vertex[i].SDEF_R1.x = ReadFloat();
				vertex[i].SDEF_R1.y = ReadFloat();
				vertex[i].SDEF_R1.z = -ReadFloat();
				break;
			}
			vertex[i].toonEdgeScale = ReadFloat();
		}
	}

	// read face info
	faceNum = ReadInt() / 3;
	if( faceNum>0 )
	{
		face = new PmxFace[ faceNum ];

		for( i=0; i<faceNum; i++ )
		{
			face[i].vertexIndex[2] = ReadInt(vertexIndexSize);
			face[i].vertexIndex[1] = ReadInt(vertexIndexSize);
			face[i].vertexIndex[0] = ReadInt(vertexIndexSize);
		}
	}

	// read texture info
	textureNum = ReadInt();
	if( textureNum>0 )
	{
		texture = new PmxTexture[ textureNum ];

		for( i=0; i<textureNum; i++ )
		{
			char* path = ReadString(encodeType);
			strcpy_s(texture[i].path, dirName);
			strcat_s(texture[i].path, path);
			delete[] path;
		}
	}

	// read material info
	materialNum = ReadInt();
	if( materialNum>0 )
	{
		material = new PmxMaterial[materialNum];

		for( i=0; i<materialNum; i++ )
		{
			char* jaName = ReadString();
			char* enName = ReadString();
			if( jaName ) delete[] jaName;
			if( enName ) delete[] enName;

			material[i].diffuse.r = ReadFloat();
			material[i].diffuse.g = ReadFloat();
			material[i].diffuse.b = ReadFloat();
			material[i].diffuse.a = ReadFloat();

			material[i].specular.r = ReadFloat();
			material[i].specular.g = ReadFloat();
			material[i].specular.b = ReadFloat();
			material[i].specular.a = 1.0f;

			material[i].specularPower = ReadFloat();

			material[i].ambient.r = ReadFloat();
			material[i].ambient.g = ReadFloat();
			material[i].ambient.b = ReadFloat();
			material[i].ambient.a = 1.0f;

			byte b = ReadByte();
			material[i].cullingOff		= ( b & 0x01 ) ? 1 : 0 ;
			material[i].groundShadow	= ( b & 0x02 ) ? 1 : 0 ;
			material[i].selfShadowMap	= ( b & 0x04 ) ? 1 : 0 ;
			material[i].selfShadowDraw	= ( b & 0x08 ) ? 1 : 0 ;
			material[i].edgeDraw		= ( b & 0x10 ) ? 1 : 0 ;

			material[i].edgeColor.r = ReadFloat();
			material[i].edgeColor.g = ReadFloat();
			material[i].edgeColor.b = ReadFloat();
			material[i].edgeColor.a = ReadFloat();

			material[i].edgeSize = ReadFloat();

			material[i].textureIndex = ReadInt(textureIndexSize);
			material[i].sphereTextureIndex = ReadInt(textureIndexSize);

			material[i].sphereMode = ReadByte();
			material[i].toonFlag = ReadByte();

			if( material[i].toonFlag )
				material[i].toonTextureIndex = ReadByte();
			else
				material[i].toonTextureIndex = ReadInt(textureIndexSize);

			char *memo = ReadString();
			delete[] memo;

			material[i].materialFaceNum = ReadInt();
		}
	}

	// read bone info
	boneNum = ReadInt();
	ikNum = 0;
	if( boneNum>0 )
	{
		bone = new PmxBone[ boneNum ];

		for( i=0; i<boneNum; i++ )
		{
			char* jaName = ReadString(encodeType);
			char* enName = ReadString();
			if( jaName )
			{
				strcpy_s( bone[i].name, jaName );
				delete[] jaName;
			}
			if( enName ) delete[] enName;

			bone[i].pos.x = ReadFloat();
			bone[i].pos.y = ReadFloat();
			bone[i].pos.z = -ReadFloat();
			bone[i].parentBoneIndex = ReadInt(boneIndexSize);
			bone[i].transformLayer = ReadInt();

			short flag;
			memcpy_s( &flag, 2, src, 2 );
			src += 2;
			bone[i].flag_LinkDest		= ( flag & 0x0001 )!= 0 ? 1 : 0 ;
			bone[i].flag_IK				= ( flag & 0x0020 )!= 0 ? 1 : 0 ;
			bone[i].flag_AddLocal		= ( flag & 0x0080 )!= 0 ? 1 : 0 ;
			bone[i].flag_AddRot			= ( flag & 0x0100 )!= 0 ? 1 : 0 ;
			bone[i].flag_AddMov			= ( flag & 0x0200 )!= 0 ? 1 : 0 ;
			bone[i].flag_LockAxis		= ( flag & 0x0400 )!= 0 ? 1 : 0 ;
			bone[i].flag_LocalAxis		= ( flag & 0x0800 )!= 0 ? 1 : 0 ;
			bone[i].flag_AfterPhysicsTransform = ( flag & 0x1000 )!= 0 ? 1 : 0 ;
			bone[i].flag_OutParentTransform = ( flag & 0x2000 )!= 0 ? 1 : 0 ;

			if( bone[i].flag_LinkDest )
			{
				bone[i].linkBoneIndex = ReadInt(boneIndexSize);
			}
			else
			{
				bone[i].offsetPos.x = ReadFloat();
				bone[i].offsetPos.y = ReadFloat();
				bone[i].offsetPos.z = -ReadFloat();
			}

			if( bone[i].flag_AddRot || bone[i].flag_AddMov )
			{
				bone[i].addParentBoneIndex = ReadInt(boneIndexSize);
				bone[i].addRatio = ReadFloat();
			}

			if( bone[i].flag_LockAxis )
			{
				bone[i].lockAxisVector.x = ReadFloat();
				bone[i].lockAxisVector.y = ReadFloat();
				bone[i].lockAxisVector.z = -ReadFloat();
			}

			if( bone[i].flag_LocalAxis )
			{
				bone[i].localAxisXVector.x = ReadFloat();
				bone[i].localAxisXVector.y = ReadFloat();
				bone[i].localAxisXVector.z = -ReadFloat();

				bone[i].localAxisZVector.x = ReadFloat();
				bone[i].localAxisZVector.y = ReadFloat();
				bone[i].localAxisZVector.z = -ReadFloat();
			}

			if( bone[i].flag_OutParentTransform )
			{
				bone[i].outParentTransformKey = ReadInt();
			}

			if( bone[i].flag_IK )
			{
				ikNum++;

				bone[i].ikInfo.targetBoneIndex = ReadInt(boneIndexSize);
				bone[i].ikInfo.loopNum = ReadInt();
				bone[i].ikInfo.rotLimit = ReadFloat();
				bone[i].ikInfo.linkNum = ReadInt();
				if( bone[i].ikInfo.linkNum >= 64 )
				{
					cout << "PMX Loader Error : IKリンクの数が対応数を超えています。" << endl;
					Release();
					return false;
				}

				for( j=0; j<bone[i].ikInfo.linkNum; j++ )
				{
					bone[i].ikInfo.link[j].boneIndex = ReadInt(boneIndexSize);
					bone[i].ikInfo.link[j].rotLockFlag = ReadByte();

					if( bone[i].ikInfo.link[j].rotLockFlag )
					{
						bone[i].ikInfo.link[j].rotLockMin.x = -ReadFloat();
						bone[i].ikInfo.link[j].rotLockMin.y = -ReadFloat();
						bone[i].ikInfo.link[j].rotLockMin.z = ReadFloat();

						bone[i].ikInfo.link[j].rotLockMax.x = -ReadFloat();
						bone[i].ikInfo.link[j].rotLockMax.y = -ReadFloat();
						bone[i].ikInfo.link[j].rotLockMax.z = ReadFloat();
					}
				}
			}
		}
	}

	// read morph info
	morphNum = ReadInt();
	skinNum = 0;

	if( morphNum>0 )
	{
		morph = new PmxMorph[ morphNum ];

		for( i=0; i<morphNum; i++ )
		{
			char* jaName = ReadString();
			char* enName = ReadString();
			if( jaName ) delete[] jaName;
			if( enName ) delete[] enName;

			morph[i].controlPanel = ReadByte();
			morph[i].type = ReadByte();
			morph[i].dataNum = ReadInt();

			int dataNum = morph[i].dataNum;
			switch( morph[i].type )
			{
			case 0:
				morph[i].group = new PmxMorphGroup[ dataNum ];

				for( j=0; j<dataNum; j++ )
				{
					morph[i].group[j].index = ReadInt(morphIndexSize);
					morph[i].group[j].ratio = ReadFloat();
				}
				break;

			case 1:
				morph[i].vertex = new PmxMorphVertex[ dataNum ];

				for( j=0; j<dataNum; j++ )
				{
					morph[i].vertex[j].index = ReadInt(vertexIndexSize);
					morph[i].vertex[j].offset.x = ReadFloat();
					morph[i].vertex[j].offset.y = ReadFloat();
					morph[i].vertex[j].offset.z = -ReadFloat();
				}
				break;

			case 2:
				morph[i].bone = new PmxMorphBone[ dataNum ];

				for( j=0; j<dataNum; j++ )
				{
					morph[i].bone[j].index = ReadInt(boneIndexSize);
					morph[i].bone[j].offset.x = ReadFloat();
					morph[i].bone[j].offset.y = ReadFloat();
					morph[i].bone[j].offset.z = -ReadFloat();
					morph[i].bone[j].quat.x = -ReadFloat();
					morph[i].bone[j].quat.y = -ReadFloat();
					morph[i].bone[j].quat.z = ReadFloat();
					morph[i].bone[j].quat.w = ReadFloat();
				}
				break;

			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				morph[i].uv = new PmxMorphUV[ dataNum ];

				for( j=0; j<dataNum; j++ )
				{
					morph[i].uv[j].index = ReadInt(vertexIndexSize);
					morph[i].uv[j].offset.x = ReadFloat();
					morph[i].uv[j].offset.y = ReadFloat();
					morph[i].uv[j].offset.z = ReadFloat();
					morph[i].uv[j].offset.w = ReadFloat();
				}
				break;

			case 8:
				morph[i].material = new PmxMorphMaterial[ dataNum ];

				for( j=0; j<dataNum; j++ )
				{
					morph[i].material[j].index = ReadInt(materialIndexSize);
					morph[i].material[j].calcType = ReadByte();
					morph[i].material[j].diffuse.r = ReadFloat();
					morph[i].material[j].diffuse.g = ReadFloat();
					morph[i].material[j].diffuse.b = ReadFloat();
					morph[i].material[j].diffuse.a = ReadFloat();
					morph[i].material[j].specular.r = ReadFloat();
					morph[i].material[j].specular.g = ReadFloat();
					morph[i].material[j].specular.b = ReadFloat();
					morph[i].material[j].specularPower = ReadFloat();
					morph[i].material[j].ambient.r = ReadFloat();
					morph[i].material[j].ambient.g = ReadFloat();
					morph[i].material[j].ambient.b = ReadFloat();
					morph[i].material[j].edgeColor.r = ReadFloat();
					morph[i].material[j].edgeColor.g = ReadFloat();
					morph[i].material[j].edgeColor.b = ReadFloat();
					morph[i].material[j].edgeColor.a = ReadFloat();
					morph[i].material[j].edgeSize = ReadFloat();
					morph[i].material[j].textureScale.r = ReadFloat();
					morph[i].material[j].textureScale.g = ReadFloat();
					morph[i].material[j].textureScale.b = ReadFloat();
					morph[i].material[j].textureScale.a = ReadFloat();
					morph[i].material[j].sphereTextureScale.r = ReadFloat();
					morph[i].material[j].sphereTextureScale.g = ReadFloat();
					morph[i].material[j].sphereTextureScale.b = ReadFloat();
					morph[i].material[j].sphereTextureScale.a = ReadFloat();
					morph[i].material[j].toonTextureScale.r = ReadFloat();
					morph[i].material[j].toonTextureScale.g = ReadFloat();
					morph[i].material[j].toonTextureScale.b = ReadFloat();
					morph[i].material[j].toonTextureScale.a = ReadFloat();
				}
				break;

			default:
				cout << "PMX Loader Error : モーフタイプが不正です。" << endl;
				Release();
				return false;
			}
		}
	}

	// read display frame info
	int dispFrameNum = ReadInt();

	if( dispFrameNum>0 )
	{
		for( i=0; i<dispFrameNum; i++ )
		{
			char* jaName = ReadString();
			char* enName = ReadString();
			if( jaName ) delete[] jaName;
			if( enName ) delete[] enName;

			byte specialFrameFlag = ReadByte();

			int elementNum = ReadInt();

			for( j=0; j<elementNum; j++ )
			{
				byte target = ReadByte();

				int boneIndex,morphIndex;
				switch( target )
				{
				case 0:
					boneIndex = ReadInt(boneIndexSize);
					break;
				case 1:
					morphIndex = ReadInt(morphIndexSize);
					break;
				default:
					cout << "PMX Loader Error : 枠内要素対象が不正です。" << endl;
					Release();
					return false;
				}
			}
		}
	}

	// read rigid body info
	rigidBodyNum = ReadInt();

	if( rigidBodyNum>0 )
	{
		rigidBody = new PmxRigidBody[ rigidBodyNum ];

		for( i=0; i<rigidBodyNum; i++ )
		{
			char* jaName = ReadString();
			char* enName = ReadString();
			if( jaName ) delete[] jaName;
			if( enName ) delete[] enName;

			rigidBody[i].boneIndex = ReadInt(boneIndexSize);

			rigidBody[i].groupIndex = ReadByte();
			rigidBody[i].groupTarget = (ushort)ReadShort();

			rigidBody[i].shapeType = ReadByte();
			rigidBody[i].shapeSize.x = ReadFloat();
			rigidBody[i].shapeSize.y = ReadFloat();
			rigidBody[i].shapeSize.z = ReadFloat();

			rigidBody[i].pos.x = ReadFloat();
			rigidBody[i].pos.y = ReadFloat();
			rigidBody[i].pos.z = -ReadFloat();

			rigidBody[i].rot.x = -ReadFloat();
			rigidBody[i].rot.y = -ReadFloat();
			rigidBody[i].rot.z = ReadFloat();

			rigidBody[i].weight = ReadFloat();
			rigidBody[i].posDim = ReadFloat();
			rigidBody[i].rotDim = ReadFloat();
			rigidBody[i].recoil = ReadFloat();
			rigidBody[i].friction = ReadFloat();

			rigidBody[i].type = ReadByte();
		}
	}

	// read joint info
	jointNum = ReadInt();

	if( jointNum>0 )
	{
		joint = new PmxJoint[ jointNum ];

		for( i=0; i<jointNum; i++ )
		{
			char* jaName = ReadString();
			char* enName = ReadString();
			if( jaName ) delete[] jaName;
			if( enName ) delete[] enName;

			joint[i].type = ReadByte();

			joint[i].rigidBodyAIndex = ReadInt(rigidIndexSize);
			joint[i].rigidBodyBIndex = ReadInt(rigidIndexSize);

			joint[i].pos.x = ReadFloat();
			joint[i].pos.y = ReadFloat();
			joint[i].pos.z = -ReadFloat();

			joint[i].rot.x = -ReadFloat();
			joint[i].rot.y = -ReadFloat();
			joint[i].rot.z = ReadFloat();

			joint[i].constrainPosMin.x = ReadFloat();
			joint[i].constrainPosMin.y = ReadFloat();
			joint[i].constrainPosMin.z = -ReadFloat();

			joint[i].constrainPosMax.x = ReadFloat();
			joint[i].constrainPosMax.y = ReadFloat();
			joint[i].constrainPosMax.z = -ReadFloat();

			joint[i].constrainRotMin.x = -ReadFloat();
			joint[i].constrainRotMin.y = -ReadFloat();
			joint[i].constrainRotMin.z = ReadFloat();

			joint[i].constrainRotMax.x = -ReadFloat();
			joint[i].constrainRotMax.y = -ReadFloat();
			joint[i].constrainRotMax.z = ReadFloat();

			joint[i].springPos.x = ReadFloat();
			joint[i].springPos.y = ReadFloat();
			joint[i].springPos.z = ReadFloat();

			joint[i].springRot.x = ReadFloat();
			joint[i].springRot.y = ReadFloat();
			joint[i].springRot.z = ReadFloat();
		}
	}
	// load default toon texture
	for( i=0; i<10; i++ )
	{
		char path[32];
		sprintf_s( path, 32, "data\\toon\\toon%02d.bmp", i+1 );

		Texture* tex = new Texture();
		if( tex->Load(path) )
			tex->CreateTexture( &defaultTexID[i] );
		else
		{
			cout << "Pmx Loader Error : failed to load tex file: " << path << endl;
			defaultTexID[i] = 0xFFFFFFFF;
		}
		delete tex;
	}

	for( i=0; i<textureNum; i++ )
	{
		Texture* tex = new Texture();
		if( tex->Load( texture[i].path ) )
			tex->CreateTexture( &texture[i].texID );
		else
		{
			cout << "Pmx Loader Error : failed to load tex file: " << texture[i].path << endl;
			texture[i].texID = 0xFFFFFFFF;
		}
		delete tex;
	}

	// create texture
	for( i=0; i<materialNum; i++ )
	{
		// case : exist texture file
		if( material[i].textureIndex >=  0 && material[i].textureIndex < textureNum )
			material[i].texID = texture[material[i].textureIndex].texID;
		else
			material[i].texID = 0xFFFFFFFF;

		if( material[i].sphereTextureIndex >=  0 && material[i].sphereTextureIndex < textureNum )
			material[i].sphereTexID = texture[material[i].sphereTextureIndex].texID;
		else
			material[i].sphereTexID = 0xFFFFFFFF;

		if( material[i].toonFlag )
			material[i].toonTexID = defaultTexID[material[i].toonTextureIndex];
		else if( material[i].toonTextureIndex >= 0 && material[i].toonTextureIndex < textureNum )
			material[i].toonTexID = texture[material[i].toonTextureIndex].texID;
		else
			material[i].toonTexID = 0xFFFFFFFF;
	}

	delete[] srcHead;
	srcHead = NULL;
	src = NULL;

	// 今回使わないので
	if( morph )
	{
		delete[] morph;
		morph = NULL;
	}
	if( rigidBody )
	{
		delete[] rigidBody;
		rigidBody = NULL;
	}
	if( joint )
	{
		delete[] joint;
		joint = NULL;
	}

	return true;
}
