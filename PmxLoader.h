#ifndef _PMX_LOADER_H_
#define _PMX_LOADER_H_

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include"TexLoader.h"
#include"Quaternion.h"
#include<stdlib.h>
#include<stdio.h>
#include<windows.h>
#include<sys/stat.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

struct PmxVertex
{
	Vector3f	pos ;					// ���W
	Vector3f	normal ;				// �@��
	UV2f		uv ;					// �W��UV�l
	Vector4f	addUV[ 4 ] ;			// �ǉ�UV�l
	byte		weightType ;			// �E�G�C�g�^�C�v( 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF )
	int			boneIndex[ 4 ] ;		// �{�[���C���f�b�N�X
	float		boneWeight[ 4 ] ;		// �{�[���E�F�C�g
	Vector3f	SDEF_C ;				// SDEF-C
	Vector3f	SDEF_R0 ;				// SDEF-R0
	Vector3f	SDEF_R1 ;				// SDEF-R1
	float		toonEdgeScale ;			// �g�D�[���G�b�W�̃X�P�[��
};

struct PmxFace
{
	int			vertexIndex[ 3 ] ;		// ���_�C���f�b�N�X
};

struct PmxTexture
{
	char		path[256] ;				// �e�N�X�`���̃t�@�C���p�X
	GLuint		texID;
};

struct PmxMaterial
{
	Color4f		diffuse ;				// �f�B�t���[�Y�J���[
	Color4f		specular ;				// �X�y�L�����J���[
	float		specularPower ;			// �X�y�L�����萔
	Color4f		ambient ;				// �A���r�G���g�J���[

	byte		cullingOff ;			// ���ʕ`��
	byte		groundShadow ;			// �n�ʉe
	byte		selfShadowMap ;			// �Z���t�V���h�E�}�b�v�ւ̕`��
	byte		selfShadowDraw ;		// �Z���t�V���h�E�̕`��
	byte		edgeDraw;				// �G�b�W�̕`��

	Color4f		edgeColor ;				// �G�b�W�J���[
	float		edgeSize ;				// �G�b�W�T�C�Y

	int			textureIndex ;			// �ʏ�e�N�X�`���C���f�b�N�X
	int			sphereTextureIndex ;	// �X�t�B�A�e�N�X�`���C���f�b�N�X
	byte		sphereMode ;			// �X�t�B�A���[�h( 0:���� 1:��Z 2:���Z 3:�T�u�e�N�X�`��(�ǉ�UV1��x,y��UV�Q�Ƃ��Ēʏ�e�N�X�`���`����s��)

	byte		toonFlag ;				// ���L�g�D�[���t���O( 0:��Toon 1:���LToon )
	int			toonTextureIndex ;		// �g�D�[���e�N�X�`���C���f�b�N�X

	int			materialFaceNum ;		// �}�e���A�����K������Ă���ʂ̐�

	GLuint		texID;					// �e�N�X�`��ID
	GLuint		sphereTexID;			// �e�N�X�`��ID
	GLuint		toonTexID;				
};

struct PmxIKLINK
{
	int			boneIndex ;			// �����N�{�[���̃C���f�b�N�X
	byte		rotLockFlag ;		// ��]����( 0:OFF 1:ON )
	Vector3f	rotLockMin ;		// ��]�����A����
	Vector3f	rotLockMax ;		// ��]�����A���
} ;

struct PmxIK
{
	int			targetBoneIndex ;			// IK�^�[�Q�b�g�̃{�[���C���f�b�N�X
	int			loopNum ;					// IK�v�Z�̃��[�v��
	float		rotLimit ;					// �v�Z���ӂ�̐����p�x

	int			linkNum ;					// �h�j�����N�̐�
	PmxIKLINK	link[ 64 ] ;				// �h�j�����N���
} ;

struct PmxBone
{
	char		name[32];						// ���O
	Vector3f	pos;							// ���W
	int			parentBoneIndex ;				// �e�{�[���C���f�b�N�X
	int			transformLayer ;				// �ό`�K�w
	
	byte		flag_LinkDest ;					// �ڑ���
	byte		flag_IK ;						// IK
	byte		flag_AddLocal ;					// ���[�J���t�^
	byte		flag_AddRot ;					// ��]�t�^
	byte		flag_AddMov ;					// �ړ��t�^
	byte		flag_LockAxis ;					// ���Œ�
	byte		flag_LocalAxis ;				// ���[�J����
	byte		flag_AfterPhysicsTransform ;	// ������ό`
	byte		flag_OutParentTransform ;		// �O���e�ό`

	Vector3f	offsetPos ;						// �I�t�Z�b�g���W
	int			linkBoneIndex ;					// �ڑ���{�[���C���f�b�N�X
	int			addParentBoneIndex ;			// �t�^�e�{�[���C���f�b�N�X
	float		addRatio ;						// �t�^��
	Vector3f	lockAxisVector ;				// ���Œ莞�̎��̕����x�N�g��
	Vector3f	localAxisXVector ;				// ���[�J�����̂w��
	Vector3f	localAxisZVector ;				// ���[�J�����̂y��
	int			outParentTransformKey ;			// �O���e�ό`���� Key�l

	PmxIK		ikInfo ;						// �h�j���
};

struct PmxMorphVertex
{
	int			index ;				// ���_�C���f�b�N�X
	Vector3f	offset ;			// ���_���W�I�t�Z�b�g
};

struct PmxMorphUV
{
	int			index ;				// ���_�C���f�b�N�X
	Vector4f	offset ;			// ���_�t�u�I�t�Z�b�g
};

struct PmxMorphBone
{
	int			index ;				// �{�[���C���f�b�N�X
	Vector3f	offset ;			// ���W�I�t�Z�b�g
	Quaternion	quat ;				// ��]�N�H�[�^�j�I��
};

struct PmxMorphMaterial
{
	int			index ;					// �}�e���A���C���f�b�N�X
	byte		calcType ;				// �v�Z�^�C�v( 0:��Z  1:���Z )
	Color4f		diffuse ;				// �f�B�t���[�Y�J���[
	Color3f		specular ;				// �X�y�L�����J���[
	float		specularPower ;			// �X�y�L�����W��
	Color3f		ambient ;				// �A���r�G���g�J���[
	Color4f		edgeColor ;				// �G�b�W�J���[
	float		edgeSize ;				// �G�b�W�T�C�Y
	Color4f		textureScale ;			// �e�N�X�`���W��
	Color4f		sphereTextureScale ;	// �X�t�B�A�e�N�X�`���W��
	Color4f		toonTextureScale ;		// �g�D�[���e�N�X�`���W��
};

struct PmxMorphGroup
{
	int			index ;		// ���[�t�C���f�b�N�X
	float		ratio ;		// ���[�t��
};

struct PmxMorph
{
	byte		controlPanel ;			// ����p�l��
	byte		type ;					// ���[�t�̎��  0:�O���[�v 1:���_ 2:�{�[�� 3:UV 4:�ǉ�UV1 5:�ǉ�UV2 6:�ǉ�UV3 7:�ǉ�UV4 8:�ގ�

	int			dataNum ;				// ���[�t���̐�

	PmxMorphVertex		*vertex ;		// ���_���[�t
	PmxMorphUV			*uv ;			// UV���[�t
	PmxMorphBone		*bone ;			// �{�[�����[�t
	PmxMorphMaterial	*material ;		// �}�e���A�����[�t
	PmxMorphGroup		*group ;		// �O���[�v���[�t
};

struct PmxRigidBody
{
	int			boneIndex ;				// �Ώۃ{�[���ԍ�

	byte		groupIndex ;			// ���̃O���[�v�ԍ�
	ushort		groupTarget ;			// ���̃O���[�v�Ώ�

	byte		shapeType ;				// �`��( 0:��  1:��  2:�J�v�Z�� )
	Vector3f	shapeSize ;				// �T�C�Y

	Vector3f	pos ;					// �ʒu
	Vector3f	rot ;					// ��]( ���W�A�� )

	float		weight ;				// ����
	float		posDim ;				// �ړ�����
	float		rotDim ;				// ��]����
	float		recoil ;				// ������
	float		friction ;				// ���C��

	byte		type ;					// ���̃^�C�v( 0:Bone�Ǐ]  1:�������Z  2:�������Z(Bone�ʒu���킹) )
};

struct PmxJoint
{
	byte		type ;						// ���( 0:�X�v�����O6DOF ( PMX2.0 �ł� 0 �̂� )

	int			rigidBodyAIndex ;			// �ڑ��捄�̂`
	int			rigidBodyBIndex ;			// �ڑ��捄�̂a

	Vector3f	pos ;						// �ʒu
	Vector3f	rot ;						// ��]( ���W�A�� )

	Vector3f	constrainPosMin ;			// �ړ�����-����
	Vector3f	constrainPosMax ;			// �ړ�����-���
	Vector3f	constrainRotMin ;			// ��]����-����
	Vector3f	constrainRotMax ;			// ��]����-���

	Vector3f	springPos ;					// �o�l�萔-�ړ�
	Vector3f	springRot ;					// �o�l�萔-��]
};

class PmxModel
{
private:
	// �ꎞ�ǂݍ��ݗp
	byte* src;
	byte* srcHead;

	byte ReadByte();
	short ReadShort();
	int ReadInt( int size );
	float ReadFloat();
	char* ReadString( int encodeType );
public:
	int vertexNum;
	int faceNum;
	int materialNum;
	int boneNum;
	int ikNum;
	int morphNum;
	int skinNum;
	int rigidBodyNum;
	int jointNum;
	PmxVertex* vertex;
	PmxFace* face;
	PmxMaterial* material;
	PmxBone* bone;
	PmxMorph* morph;
	PmxRigidBody* rigidBody;
	PmxJoint* joint;
	GLuint defaultTexID[10];

	PmxModel();
	~PmxModel();
	bool Load(const char* filename);
	void Release();
};

#endif