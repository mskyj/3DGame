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

// �{�[���L�[�t���[��
struct VmdBoneKeyFrame
{
	uint				frame;			// �t���[���ԍ�
	Vector3f			pos;			// �{�[���̈ʒu,�ʒu�f�[�^���Ȃ��ꍇ�͂��ꂼ��0
	Quaternion			quat;			// �{�[���̃N�H�[�^�j�I����],�f�[�^���Ȃ��ꍇ�͂��ꂼ��0
	Vector2f			bezierPosX[2];
	Vector2f			bezierPosY[2];
	Vector2f			bezierPosZ[2];
	Vector2f			bezierRot[2];
	char				bezier[64];		// ��ԃp�����[�^ 
	VmdBoneKeyFrame		*prev;			// �O�̃L�[�ւ̃A�h���X
	VmdBoneKeyFrame		*next;			// ���̃L�[�ւ̃A�h���X
};

// �{�[���L�[�t���[���p�m�[�h
struct VmdBoneNode
{
	uint				keyNum;			// �L�[�̐�
	uint				maxFrame;		// �ő�t���[���ԍ�
	uint				minFrame;		// �ŏ��t���[���ԍ�
	char				name[ 32 ];		// ���O
	VmdBoneKeyFrame		*firstKey;		// �L�[���X�g�̐擪
};

// �\��L�[�t���[��
struct VmdMorphKeyFrame
{
	uint				frame;			// �t���[���ԍ�
	float				value;			// �\��l(0�`1)
	VmdMorphKeyFrame	*prev;			// �O�̃L�[�ւ̃A�h���X
	VmdMorphKeyFrame	*next;			// ���̃L�[�ւ̃A�h���X
};

// �\��L�[�t���[���p�m�[�h
struct VmdMorphNode
{
	uint				keyNum;			// �L�[�̐�
	uint				maxFrame;		// �ő�t���[���ԍ�
	uint				minFrame;		// �ŏ��t���[���ԍ�
	char				name[ 32 ];		// ���O
	VmdMorphKeyFrame	*firstKey;		// �L�[���X�g�̐擪
};

// �ȉ��̃L�[�t���[���͍���g���܂���

// �J�����L�[�t���[��
struct VmdCameraKeyFrame
{
	uint				frame;			// �t���[���ԍ�
	float				distance;		// �ڕW�_�ƃJ�����̋���(�ڕW�_���J�����O�ʂŃ}�C�i�X)
	Vector3f			pos;			// �ڕW�_�̈ʒu
	Vector3f			rot;			// �J�����̉�](rad)(MMD���l���͂̃}�C�i�X�l)
	char				bezier[24];		// ��ԃp�����[�^
	uint				viewAngle;		// ����p(deg)
	char				persFlag;		// �p�[�X�y�N�e�B�u, 0:ON, 1:OFF
};

// �Ɩ��L�[�t���[��
struct VmdLightKeyFrame
{
	uint				frame;		// �t���[���ԍ�
	Color3f				color;		// �Ɩ��F(MMD���͒l��256�Ŋ������l)
	Vector3f			pos;		// �Ɩ��ʒu(MMD���͒l)
};

// �Z���t�e�L�[�t���[��
struct VmdSelfShadowKeyFrame
{
	uint				frame;		// �t���[���ԍ�
	char				type;		// �Z���t�V���h�E���, 0:OFF, 1:model1, 2:model2
	float				distance;	// �V���h�E����(MMD���͒lx��(10000-x)/100000�Ƃ����l)
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