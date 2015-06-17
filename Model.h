#ifndef _MODEL_H_
#define _MODEL_H_

#define MAX_ANIM_NUM 10
#define MAX_POSE_NUM 10
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

#include <MyGlewLib.h>
#include "PmxLoader.h"
#include "VmdLoader.h"
#include "VpdLoader.h"

struct PmxBoneList
{
	PmxBoneList*	prev;
	PmxBoneList*	next;
	byte			flag_AfterPhysics;
	int				layer;
	int				index;
};

struct BoneInfo
{
	Vector3f		localPos;				// 現在のローカル座標
	Vector3f		worldPos;				// 現在のワールド座標
	Quaternion		localRot;
	Quaternion		worldRot;

	Matrix4f		rotMat;

	Quaternion		IKQuat;					// IK処理で使用するクォータニオン

	byte			isCenter;
	
	VmdBoneNode		*node[MAX_ANIM_NUM] ;	// ボーンと関連付けられているキーフレームのノード
	VmdBoneKeyFrame	*nowKey[MAX_ANIM_NUM] ;	// 現在再生しているキー
};

struct BoundingSphere
{
	Vector3f center;
	float radius;
};

class Model
{
protected:
	PmxModel*	pmx;
	Vmd*		vmd[MAX_ANIM_NUM];
	Vpd*		vpd[MAX_POSE_NUM];
	BoneInfo*	boneInfo;
	int*		boneIndexList;
	void		UpdateBoneRot();
	void		UpdateBonePos();
	void		UpdateIK( int index );
	void		UpdateBoneByVmd();
	void		UpdateBoneByVpd();
	GLuint		vbo[4];
	GLuint		shaderProg;

	bool		kinectUseFlag;
	int			kinectBoneIndex[15];
	Quaternion	kinectBoneRot[15];

	bool		lookAtFlag;
	int			headBoneIndex;
	Quaternion	headRot;

	void ComputeBoundingSphere();

	float		scale;
public:
	BoundingSphere sphere;

	Model();
	~Model();
	int  LoadAnim(const char* filename, byte loopFlag=1);
	void startAnim( int key );
	void stopAnim( int key );
	void setAnimSpeed( int key, float speed );
	void ReleaseAnim( int key );

	int LoadPose(const char* filename);
	void startPose( int key );
	void stopPose( int key );
	void ReleasePose( int key );

	void Render();
	virtual void Update();

	bool Load( PmxModel* model, GLuint shaderProgram );
	bool Load( const char* filename, GLuint shaderProgram, float size = 1.0f );
	void Release();

	void EnableKinect(){ kinectUseFlag = true; }
	void DisableKinect(){ kinectUseFlag = false; }

	void UpdateKinectBone( Quaternion* quat );

	void EnableLookAt(){ lookAtFlag = true; }
	void DisableLookAt(){ lookAtFlag = false; }

	void UpdateHeadBone( Quaternion quat );

	int  GetBoneIndex( char* boneName );
	int  GetBonePos( char* boneName, Vector3f* pos  );
	void GetBonePos( int index, Vector3f* pos );
	int  GetBoneRot( char* boneName, Quaternion* rot );
	void GetBoneRot( int index, Quaternion* rot );
	void GetBoneWRot( int index, Quaternion* rot );
	PmxModel* GetPmx(){ return pmx; };
};

#endif