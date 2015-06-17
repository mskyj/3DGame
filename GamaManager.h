#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#define WIN32_LEAN_AND_MEAN
#include "XLoader.h"
#include "Wire.h"
#include "cvCustom.h"
#include "Enemy.h"
#include "Item.h"
#include "Julius.h"
#include "ShaderLoader.h"
#include "Effect.h"
#include "Font.h"
#include <windows.h>
#include <process.h>
#include <time.h>
#include <mmsystem.h>

#define ENEMY_NUM 30
#define TIMEUP 180

#define WINDOW_DEFAULT_W 1040
#define WINDOW_DEFAULT_H 806
#define WINDOW_DEFAULT_POS_X 0
#define WINDOW_DEFAULT_POS_Y 0

enum KEY {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W, KEY_A, KEY_S, KEY_D, KEY_V,
	KEY_SPACE, KEY_ENTER, KEY_BACKSPACE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5
};

enum GAME_STATUS {
	START, MAIN, CONTINUE, GAMECLEAR, GAMEOVER, ABE_END, ABE_END2, ABE, TITLE
};

extern char voiceStr[128];
extern void* hMutex;
extern bool juliusFinishFlag;

class GameManager
{
private:

	// 関数
	bool LoadConfig();
	bool LoadModel();

	// FPS用
	double CurrentTime;
	double LastTime;
	double CurrentCount;
	double LastCount;
	int FrameCount;
	double Fps;

	// シェーダー
	GLuint shaderProgram;

	// プレイヤー
	Player player;

	// ワイヤー
	Wire wire;

	// エネミー
	Enemy enemy[ENEMY_NUM];
	int enemyNum;

	// アイテム
	Item item;

	// 状況
	GAME_STATUS status;
	unsigned long statusTime;

	int viewType;

	// 街データ
	XModel castleGround;
	XModel castleGarden;
	XModel castleDining;
	XModel castleKingBench;
	XModel ground;
	XModel royal;
	XModel resident;
	XModel port;
	XModel business;
	XModel skydome;

	// エフェクト
	Effect effect;
	int effectIndex;
	int effectViewType;
	Vector3f effectPos;

	// 音
	Sound bgm[7];
	Sound se[6];
	int battleStatus;

	HANDLE thread[2];
	HANDLE mutex;

public:
	GameManager();
	~GameManager();
	bool Initialize();
	void Release();
	void Render2D();
	void Render3D();
	void KeyFunc(KEY key);
	void SetView();
	void Update();
	void DrawLoadScreen();
	void SetWindowSize( int x, int y );
	float GetWindowWidth();
	float GetWindowHeight();
	float GetWindowPosX();
	float GetWindowPosY();
	GAME_STATUS GetStatus(){ return status; };
};


#endif