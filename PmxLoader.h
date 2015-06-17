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
	Vector3f	pos ;					// 座標
	Vector3f	normal ;				// 法線
	UV2f		uv ;					// 標準UV値
	Vector4f	addUV[ 4 ] ;			// 追加UV値
	byte		weightType ;			// ウエイトタイプ( 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF )
	int			boneIndex[ 4 ] ;		// ボーンインデックス
	float		boneWeight[ 4 ] ;		// ボーンウェイト
	Vector3f	SDEF_C ;				// SDEF-C
	Vector3f	SDEF_R0 ;				// SDEF-R0
	Vector3f	SDEF_R1 ;				// SDEF-R1
	float		toonEdgeScale ;			// トゥーンエッジのスケール
};

struct PmxFace
{
	int			vertexIndex[ 3 ] ;		// 頂点インデックス
};

struct PmxTexture
{
	char		path[256] ;				// テクスチャのファイルパス
	GLuint		texID;
};

struct PmxMaterial
{
	Color4f		diffuse ;				// ディフューズカラー
	Color4f		specular ;				// スペキュラカラー
	float		specularPower ;			// スペキュラ定数
	Color4f		ambient ;				// アンビエントカラー

	byte		cullingOff ;			// 両面描画
	byte		groundShadow ;			// 地面影
	byte		selfShadowMap ;			// セルフシャドウマップへの描画
	byte		selfShadowDraw ;		// セルフシャドウの描画
	byte		edgeDraw;				// エッジの描画

	Color4f		edgeColor ;				// エッジカラー
	float		edgeSize ;				// エッジサイズ

	int			textureIndex ;			// 通常テクスチャインデックス
	int			sphereTextureIndex ;	// スフィアテクスチャインデックス
	byte		sphereMode ;			// スフィアモード( 0:無効 1:乗算 2:加算 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)

	byte		toonFlag ;				// 共有トゥーンフラグ( 0:個別Toon 1:共有Toon )
	int			toonTextureIndex ;		// トゥーンテクスチャインデックス

	int			materialFaceNum ;		// マテリアルが適応されている面の数

	GLuint		texID;					// テクスチャID
	GLuint		sphereTexID;			// テクスチャID
	GLuint		toonTexID;				
};

struct PmxIKLINK
{
	int			boneIndex ;			// リンクボーンのインデックス
	byte		rotLockFlag ;		// 回転制限( 0:OFF 1:ON )
	Vector3f	rotLockMin ;		// 回転制限、下限
	Vector3f	rotLockMax ;		// 回転制限、上限
} ;

struct PmxIK
{
	int			targetBoneIndex ;			// IKターゲットのボーンインデックス
	int			loopNum ;					// IK計算のループ回数
	float		rotLimit ;					// 計算一回辺りの制限角度

	int			linkNum ;					// ＩＫリンクの数
	PmxIKLINK	link[ 64 ] ;				// ＩＫリンク情報
} ;

struct PmxBone
{
	char		name[32];						// 名前
	Vector3f	pos;							// 座標
	int			parentBoneIndex ;				// 親ボーンインデックス
	int			transformLayer ;				// 変形階層
	
	byte		flag_LinkDest ;					// 接続先
	byte		flag_IK ;						// IK
	byte		flag_AddLocal ;					// ローカル付与
	byte		flag_AddRot ;					// 回転付与
	byte		flag_AddMov ;					// 移動付与
	byte		flag_LockAxis ;					// 軸固定
	byte		flag_LocalAxis ;				// ローカル軸
	byte		flag_AfterPhysicsTransform ;	// 物理後変形
	byte		flag_OutParentTransform ;		// 外部親変形

	Vector3f	offsetPos ;						// オフセット座標
	int			linkBoneIndex ;					// 接続先ボーンインデックス
	int			addParentBoneIndex ;			// 付与親ボーンインデックス
	float		addRatio ;						// 付与率
	Vector3f	lockAxisVector ;				// 軸固定時の軸の方向ベクトル
	Vector3f	localAxisXVector ;				// ローカル軸のＸ軸
	Vector3f	localAxisZVector ;				// ローカル軸のＺ軸
	int			outParentTransformKey ;			// 外部親変形時の Key値

	PmxIK		ikInfo ;						// ＩＫ情報
};

struct PmxMorphVertex
{
	int			index ;				// 頂点インデックス
	Vector3f	offset ;			// 頂点座標オフセット
};

struct PmxMorphUV
{
	int			index ;				// 頂点インデックス
	Vector4f	offset ;			// 頂点ＵＶオフセット
};

struct PmxMorphBone
{
	int			index ;				// ボーンインデックス
	Vector3f	offset ;			// 座標オフセット
	Quaternion	quat ;				// 回転クォータニオン
};

struct PmxMorphMaterial
{
	int			index ;					// マテリアルインデックス
	byte		calcType ;				// 計算タイプ( 0:乗算  1:加算 )
	Color4f		diffuse ;				// ディフューズカラー
	Color3f		specular ;				// スペキュラカラー
	float		specularPower ;			// スペキュラ係数
	Color3f		ambient ;				// アンビエントカラー
	Color4f		edgeColor ;				// エッジカラー
	float		edgeSize ;				// エッジサイズ
	Color4f		textureScale ;			// テクスチャ係数
	Color4f		sphereTextureScale ;	// スフィアテクスチャ係数
	Color4f		toonTextureScale ;		// トゥーンテクスチャ係数
};

struct PmxMorphGroup
{
	int			index ;		// モーフインデックス
	float		ratio ;		// モーフ率
};

struct PmxMorph
{
	byte		controlPanel ;			// 操作パネル
	byte		type ;					// モーフの種類  0:グループ 1:頂点 2:ボーン 3:UV 4:追加UV1 5:追加UV2 6:追加UV3 7:追加UV4 8:材質

	int			dataNum ;				// モーフ情報の数

	PmxMorphVertex		*vertex ;		// 頂点モーフ
	PmxMorphUV			*uv ;			// UVモーフ
	PmxMorphBone		*bone ;			// ボーンモーフ
	PmxMorphMaterial	*material ;		// マテリアルモーフ
	PmxMorphGroup		*group ;		// グループモーフ
};

struct PmxRigidBody
{
	int			boneIndex ;				// 対象ボーン番号

	byte		groupIndex ;			// 剛体グループ番号
	ushort		groupTarget ;			// 剛体グループ対象

	byte		shapeType ;				// 形状( 0:球  1:箱  2:カプセル )
	Vector3f	shapeSize ;				// サイズ

	Vector3f	pos ;					// 位置
	Vector3f	rot ;					// 回転( ラジアン )

	float		weight ;				// 質量
	float		posDim ;				// 移動減衰
	float		rotDim ;				// 回転減衰
	float		recoil ;				// 反発力
	float		friction ;				// 摩擦力

	byte		type ;					// 剛体タイプ( 0:Bone追従  1:物理演算  2:物理演算(Bone位置合わせ) )
};

struct PmxJoint
{
	byte		type ;						// 種類( 0:スプリング6DOF ( PMX2.0 では 0 のみ )

	int			rigidBodyAIndex ;			// 接続先剛体Ａ
	int			rigidBodyBIndex ;			// 接続先剛体Ｂ

	Vector3f	pos ;						// 位置
	Vector3f	rot ;						// 回転( ラジアン )

	Vector3f	constrainPosMin ;			// 移動制限-下限
	Vector3f	constrainPosMax ;			// 移動制限-上限
	Vector3f	constrainRotMin ;			// 回転制限-下限
	Vector3f	constrainRotMax ;			// 回転制限-上限

	Vector3f	springPos ;					// バネ定数-移動
	Vector3f	springRot ;					// バネ定数-回転
};

class PmxModel
{
private:
	// 一時読み込み用
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