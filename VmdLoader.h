#ifndef _VMD_LOADER_H_
#define _VMD_LOADER_H_

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#include <sys/stat.h>
#include<iostream>
using namespace std;
#include<string.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include "Quaternion.h"

// ボーンキーフレーム
struct VmdBoneKeyFrame
{
	uint				frame;			// フレーム番号
	Vector3f			pos;			// ボーンの位置,位置データがない場合はそれぞれ0
	Quaternion			quat;			// ボーンのクォータニオン回転,データがない場合はそれぞれ0
	Vector2f			bezierPosX[2];
	Vector2f			bezierPosY[2];
	Vector2f			bezierPosZ[2];
	Vector2f			bezierRot[2];
	char				bezier[64];		// 補間パラメータ 
	VmdBoneKeyFrame		*prev;			// 前のキーへのアドレス
	VmdBoneKeyFrame		*next;			// 次のキーへのアドレス
};

// ボーンキーフレーム用ノード
struct VmdBoneNode
{
	uint				keyNum;			// キーの数
	uint				maxFrame;		// 最大フレーム番号
	uint				minFrame;		// 最小フレーム番号
	char				name[ 32 ];		// 名前
	VmdBoneKeyFrame		*firstKey;		// キーリストの先頭
};

// 表情キーフレーム
struct VmdMorphKeyFrame
{
	uint				frame;			// フレーム番号
	float				value;			// 表情値(0～1)
	VmdMorphKeyFrame	*prev;			// 前のキーへのアドレス
	VmdMorphKeyFrame	*next;			// 次のキーへのアドレス
};

// 表情キーフレーム用ノード
struct VmdMorphNode
{
	uint				keyNum;			// キーの数
	uint				maxFrame;		// 最大フレーム番号
	uint				minFrame;		// 最小フレーム番号
	char				name[ 32 ];		// 名前
	VmdMorphKeyFrame	*firstKey;		// キーリストの先頭
};

// 以下のキーフレームは今回使いません

// カメラキーフレーム
struct VmdCameraKeyFrame
{
	uint				frame;			// フレーム番号
	float				distance;		// 目標点とカメラの距離(目標点がカメラ前面でマイナス)
	Vector3f			pos;			// 目標点の位置
	Vector3f			rot;			// カメラの回転(rad)(MMD数値入力のマイナス値)
	char				bezier[24];		// 補間パラメータ
	uint				viewAngle;		// 視野角(deg)
	char				persFlag;		// パースペクティブ, 0:ON, 1:OFF
};

// 照明キーフレーム
struct VmdLightKeyFrame
{
	uint				frame;		// フレーム番号
	Color3f				color;		// 照明色(MMD入力値を256で割った値)
	Vector3f			pos;		// 照明位置(MMD入力値)
};

// セルフ影キーフレーム
struct VmdSelfShadowKeyFrame
{
	uint				frame;		// フレーム番号
	char				type;		// セルフシャドウ種類, 0:OFF, 1:model1, 2:model2
	float				distance;	// シャドウ距離(MMD入力値xを(10000-x)/100000とした値)
};

class Vmd
{
private:
	byte *srcHead;
	byte *src;
	char ReadChar();
	int ReadInt();
	float ReadFloat();
public:
	uint boneKeyNum;
	uint boneNodeNum;
	uint morphKeyNum;
	uint morphNodeNum;
	VmdBoneKeyFrame*		boneKey;
	VmdBoneNode*			boneNode;
	VmdMorphKeyFrame*		morphKey;
	VmdMorphNode*			morphNode;

	byte  playFlag;
	byte  loopFlag;
	float playTime;
	uint  playMaxTime;
	float playSpeed;
	float ratio;
	
	Vmd();
	~Vmd();
	bool Load( const char* path );
	void Release();

};

#endif