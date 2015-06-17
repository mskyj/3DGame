#include "GamaManager.h"

bool loadFinishFlag = false;
char FpsString[50] = {0};

HANDLE juliusThread[1] = {NULL};

Vector2f windowPos;
Vector2f windowSize;

Kinect kinect;

bool useKinect = false;
bool useJulius = false;


void glutRenderText(void* bitmapfont, char*text);
int drawText(char *text, HFONT hfont, GLfloat xmove=0);
int drawText(int x, int y, char *text, HFONT hfont, Color4f mainColor, Color4f edgeColor, int edgeSize, GLfloat xmove=0);
void DrawSmallCircle(float radius,int x,int y);		// 半径32.0まで 品質：良
void DrawLargeCircle(float radius,int x,int y);		// 半径制限なし 品質：悪

GameManager::GameManager()
{
	mutex = NULL;
	thread[0] = NULL;
	thread[1] = NULL;

	int viewType = 0;
	effectIndex = -1;

	status = TITLE;
	statusTime = 0;

	battleStatus = 0;

	//　カウンターの初期化
	CurrentCount = LastCount = glutGet(GLUT_ELAPSED_TIME);
	FrameCount = 0;
	CurrentTime = 0.0;
	LastTime = 0.0;
	LastCount = 0.0;
	Fps = 0.0f;

	windowSize.x = (float)WINDOW_DEFAULT_W;
	windowSize.y = (float)WINDOW_DEFAULT_H;
	windowPos.x = (float)WINDOW_DEFAULT_POS_X;
	windowPos.y = (float)WINDOW_DEFAULT_POS_Y;
}

GameManager::~GameManager()
{
}

bool GameManager::Initialize()
{
	if( !LoadConfig() )
	{
		cout << "GameManager Error : Initialize() failed to initialize config" << endl;
		MessageBox(NULL, TEXT("設定ファイルの読み込みに失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return false;
	}
	if( !FontInit() )
	{
		cout << "GameManager Error : Initialize() failed to initialize font" << endl;
		MessageBox(NULL, TEXT("フォントの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return false;
	}
	if( !effect.Init() )
	{
		cout << "GameManager Error : Initialize() failed to initialize effekseer" << endl;
		MessageBox(NULL, TEXT("Effekseerの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return false;
	}
	if( useKinect )
	{
		if( !kinect.init() )
		{
			cout << "GameManager Error : Initialize() failed to initialize Kinect" << endl;
			MessageBox(NULL, TEXT("Kinectの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
			return false;
		}
	}
	if( useJulius )
	{
		if( !JuliusInit() )
		{
			cout << "GameManager Error : Initialize() failed to initialize Julius" << endl;
			MessageBox(NULL, TEXT("Juliusの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
			return false;
		}
	}
	if( !LoadShader(&shaderProgram) )
	{
		cout << "GameManager Error : Initialize() failed to load shader program" << endl;
		MessageBox(NULL, TEXT("シェーダーの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return false;
	}
	if( !LoadDepthData( "data\\depth.dat" ) )
	{
		cout << "GameManager Error : Initialize() failed to load map data" << endl;
		MessageBox(NULL, TEXT("深度情報の初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return false;
	}
	if( !LoadModel() )
	{
		cout << "GameManager Error : Initialize() failed to load 3D model data" << endl;
		return false;
	}

	if( useJulius )
	{
		hMutex = CreateMutex(NULL,FALSE,NULL);
		juliusThread[0] = (HANDLE)_beginthread(JuliusLoopFunc,0,NULL);
	}

	return true;
}

void GameManager::Release()
{
	juliusFinishFlag = true;

	castleGround.Release();
	castleGarden.Release();
	castleDining.Release();
	castleKingBench.Release();
	ground.Release();
	royal.Release();
	resident.Release();
	port.Release();
	business.Release();
	skydome.Release();
	player.Release();
	wire.Release();
	effect.Release();

	for( int i=0; i<7; i++ )
		bgm[i].Release();
	for( int i=0; i<6; i++ )
		se[i].Release();

	for( int i=0; i<ENEMY_NUM; i++ )
		enemy[i].Release();

	FontRelease();

	if( useJulius )
	{
		if( juliusThread[0] )
		{
			if( WaitForMultipleObjects(1,juliusThread,TRUE,1000)==WAIT_TIMEOUT )
				TerminateThread(juliusThread[0], -1);
			CloseHandle(juliusThread[0]);
			CloseHandle(hMutex);
			JuliusClose();
		}
	}

	DeleteDepthData();
	item.Release();
}

bool GameManager::LoadModel()
{
	try{
		DrawLoadScreen();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if( !player.LoadModel("model\\eren\\お掃除エレン.pmx",shaderProgram,useKinect) )
			throw "model\\eren\\お掃除エレン.pmx";
		if( loadFinishFlag )
			throw "";
#if 0
		if( player.LoadPose("motion\\test.vpd")==-1 )
			throw "motion\\test.vpd";
		if( player.LoadPose("motion\\rhand.vpd")==-1 )
			throw "motion\\rhand.vpd";
		if( player.LoadPose("motion\\lhand.vpd")==-1 )
			throw "motion\\lhand.vpd";
#endif
		if( player.LoadAnim("motion\\run.vmd")==-1 )
			throw "motion\\run.vmd";
		if( loadFinishFlag )
			throw "";
		if( !wire.LoadModel("model\\wire\\wire2.pmx",shaderProgram) )
			throw "model\\wire\\wire2.pmx";
		if( loadFinishFlag )
			throw "";

		Sound sound;
		if( sound.LoadWave("sound\\alert.wav")==AL_FALSE )
			throw "sound\\alert.wav";
		ALuint buffer = sound.GetBuffer();

		srand((unsigned int)time(NULL));
		for( int i=0; i<ENEMY_NUM; i++ )
		{	
			if( !enemy[i].LoadModel(buffer, shaderProgram) )
				throw "敵モデル";
			if( enemy[i].LoadAnim(0)==-1 )
				throw "敵アニメーション";
			if( enemy[i].LoadAnim(1)==-1 )
				throw "敵アニメーション";
			if( enemy[i].LoadAnim(2)==-1 )
				throw "敵アニメーション";

			enemy[i].Init();
			enemy[i].SetPlayer(&player);
			enemy[i].PrepareAnim(0);

			if( loadFinishFlag )
				throw "";
		}

		player.PrepareAnim(0);
		wire.SetHuman( &player );
		wire.SetKinect( &kinect );

		if( !castleGround.Load("stage\\リベルニア王室\\王城内地面.x") )
			throw "stage\\リベルニア王室\\王城内地面.x";
		if( loadFinishFlag )
			throw "";
		if( !castleGarden.Load("stage\\リベルニア王室\\庭園.x") )
			throw "stage\\リベルニア王室\\庭園.x";
		if( loadFinishFlag )
			throw "";
		if( !castleDining.Load("stage\\リベルニア王室\\食堂.x") )
			throw "stage\\リベルニア王室\\食堂.x";
		if( loadFinishFlag )
			throw "";
		if( !castleKingBench.Load("stage\\リベルニア王室\\玉座の間.x") )
			throw "stage\\リベルニア王室\\玉座の間.x";
		if( loadFinishFlag )
			throw "";
		if( !ground.Load("stage\\リベルニア地面\\地面.x") )
			throw "stage\\リベルニア地面\\地面.x";
		if( loadFinishFlag )
			throw "";
		if( !royal.Load("stage\\貴族街\\貴族街.x") )
			throw "stage\\貴族街\\貴族街.x";
		if( loadFinishFlag )
			throw "";
		if( !resident.Load("stage\\居住地\\居住地.x") )
			throw "stage\\居住地\\居住地.x";
		if( loadFinishFlag )
			throw "";
		if( !port.Load("stage\\港\\港.x") )
			throw "stage\\港\\港.x";
		if( loadFinishFlag )
			throw "";
		if( !business.Load("stage\\商業地区\\商業地区.x") )
			throw "stage\\商業地区\\商業地区.x";
		if( loadFinishFlag )
			throw "";
		if( !skydome.Load("stage\\スカイドーム\\青空.x", 10.0f) )
			throw "stage\\スカイドーム\\青空.x";
		if( loadFinishFlag )
			throw "";

		if( !effect.Load( (const EFK_CHAR*)L"effect\\laser.efk" ) )
			throw "effect\\laser.efk";
		if( loadFinishFlag )
			throw "";
		if( !effect.Load( (const EFK_CHAR*)L"effect\\laser2.efk" ) )
			throw "effect\\laser2.efk";
		if( loadFinishFlag )
			throw "";
		if( !effect.Load( (const EFK_CHAR*)L"effect\\sprite2.efk" ) )
			throw "effect\\sprite2.efk";
		if( loadFinishFlag )
			throw "";
		if( !effect.Load( (const EFK_CHAR*)L"effect\\billboard.efk" ) )
			throw "effect\\billboard.efk";
		if( loadFinishFlag )
			throw "";

		if( !item.Load() )
			throw "アイテムモデル";
		if( loadFinishFlag )
			throw "";

		if( se[0].LoadWave("sound\\ban.wav")==AL_FALSE )
			throw "sound\\ban.wav";
		if( se[1].LoadWave("sound\\enter.wav")==AL_FALSE )
			throw "sound\\enter.wav";
		if( se[2].LoadWave("sound\\appear.wav")==AL_FALSE )
			throw "sound\\appear.wav";
		if( se[3].LoadWave("sound\\oreha.wav")==AL_FALSE )
			throw "sound\\oreha.wav";
		if( se[4].LoadWave("sound\\a-.wav")==AL_FALSE )
			throw "sound\\a-.wav";
		if( se[5].LoadWave("sound\\yaranaika.wav")==AL_FALSE )
			throw "sound\\yaranaika.wav";
		if( loadFinishFlag )
			throw "";
		if( bgm[0].LoadWave("sound\\hyouka.wav")==AL_FALSE )
			throw "sound\\hyouka.wav";
		if( bgm[1].LoadWave("sound\\gameover.wav")==AL_FALSE )
			throw "sound\\gameover.wav";
		if( bgm[2].LoadWave("sound\\continue.wav")==AL_FALSE )
			throw "sound\\continue.wav";
		if( bgm[3].LoadWave("sound\\normal.wav", 1.0f, AL_TRUE)==AL_FALSE )
			throw "sound\\normal.wav";
		if( bgm[4].LoadWave("sound\\battle.wav", 1.0f, AL_TRUE)==AL_FALSE )
			throw "sound\\battle.wav";
		if( bgm[5].LoadWave("sound\\title.wav", 1.0f, AL_TRUE)==AL_FALSE )
			throw "sound\\battle.wav";
		if( bgm[6].LoadWave("sound\\bararaika.wav", 1.0f, AL_TRUE)==AL_FALSE )
			throw "sound\\bararaika.wav";
		if( loadFinishFlag )
			throw "";

		WaitForSingleObject(mutex,INFINITE);
		loadFinishFlag = true;
		ReleaseMutex(mutex);

		WaitForMultipleObjects(1,thread,TRUE,INFINITE);

		CloseHandle(mutex);
		CloseHandle(thread[0]);

		return true;
	}
	catch( const char* str )
	{
		if( strlen(str)>5 )
		{
			char buff[128] = "/0";
			strcat_s(buff, str);
			strcat_s(buff, " の読み込みに失敗しました。");
			MessageBox(NULL, TEXT(buff), TEXT("読み込みエラー"), MB_OK);
		}

		WaitForSingleObject(mutex,INFINITE);
		loadFinishFlag = true;
		ReleaseMutex(mutex);

		WaitForMultipleObjects(1,thread,TRUE,INFINITE);

		CloseHandle(mutex);
		CloseHandle(thread[0]);

		return false;
	}
}

void GameManager::KeyFunc(KEY key)
{
	if( status == MAIN )
	{
		switch(key)
		{
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_LEFT:
			player.RotateXZ(0.05f);
			break;
		case KEY_RIGHT:
			player.RotateXZ(-0.05f);
			break;
		case KEY_W:
			player.Move( AHEAD );
			break;
		case KEY_A:
			player.Move( LEFT );
			break;
		case KEY_S:
			player.Move( BEHIND );
			break;
		case KEY_D:
			player.Move( RIGHT );
			break;
		case KEY_V:
			viewType = 1 - viewType;
			break;
		case KEY_SPACE:
			player.Move( HIGHUP );
			break;
		case KEY_ENTER:
			break;
		case KEY_BACKSPACE:
			break;
		case KEY_1:
		case KEY_2:
		case KEY_3:
			effectViewType = viewType;
			if( effectIndex!=-1 )
				effect.Stop(effectIndex);
			effect.Play(key - KEY_1, player.GetPos());
			effectIndex = key - KEY_1;
			break;
		}
	}
	else if( status == TITLE )
	{
		if( key == KEY_ENTER )
			strcpy_s(voiceStr, "スタート");
	}
	else if( status == CONTINUE )
	{
		if( key == KEY_ENTER )
			strcpy_s(voiceStr, "はい");
	}
}

double cameraAngle = 0.0;

void GameManager::SetView()
{
	gluPerspective(90.0, (double)windowSize.x/(double)windowSize.y*1.2, 0.1, 2000.0);

	if( status == TITLE )
	{
		gluLookAt(300.0*sin(cameraAngle), 200.0, -300.0*cos(cameraAngle), 0.0, 0.0, 0.0,
					  0.0, 1.0, 0.0);
		cameraAngle += 0.002;
	}
	else
	{
		if( viewType == 0 )
		{
			Vector3f eyePos = player.GetEyePos();
			Vector3f angle = player.GetEyeVec();
			gluLookAt(eyePos.x, eyePos.y, eyePos.z,
					  eyePos.x + angle.x,
					  eyePos.y + angle.y,
					  eyePos.z + angle.z,
					  0.0, 1.0, 0.0);
			effect.SetCamera(eyePos.x, eyePos.y, eyePos.z,
							 eyePos.x + angle.x,
							 eyePos.y + angle.y,
							 eyePos.z + angle.z);
		}
		else if( viewType == 1 )
		{
			Vector3f pos = player.GetPos();
			gluLookAt(pos.x, pos.y + 1.5f, pos.z + 3.0f,
					  pos.x,
					  pos.y + 2.0f,
					  pos.z,
					  0.0, 1.0, 0.0);
			effect.SetCamera(pos.x, pos.y + 2.5f, pos.z + 5.0f,
							 pos.x,
							 pos.y + 2.0f,
							 pos.z);
		}
	}


}

void GameManager::Render2D()
{
	char PosXString[50] = {0};
	char PosYString[50] = {0};
	char PosZString[50] = {0};

	//　時間計測とFPS算出
	CurrentCount = glutGet(GLUT_ELAPSED_TIME);
	CurrentTime = (CurrentCount - LastCount)/1000.0;
	FrameCount++;
	if ( (CurrentTime - LastTime )>= 1.0 )
	{
		Fps = FrameCount/(CurrentTime - LastTime);
		sprintf_s(FpsString, "%.3f FPS", Fps);
		FrameCount = 0;
		LastTime = CurrentTime;
	}

	if( status == TITLE )
	{
		if( statusTime < 3 )
		{
			glColor4d(0.0, 0.0, 0.0, 1.0); 
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(windowSize.x, 0.0f);
			glVertex2f(windowSize.x, windowSize.y);
			glVertex2f(0.0f, windowSize.y);
			glEnd();
		}
		else
		{
			drawText((int)(windowSize.x/2.0f)-192, (int)(windowSize.y/2.0f)-120,
				"凸】♂巨人】",largeFont2, Color4f(0.5f, 0.3f,0.3f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);

			drawText((int)(windowSize.x/2.0f)-120, (int)(windowSize.y/2.0f)+120,
				"Please say \"START\"",smallFont, Color4f(1.0f, 1.0f, 1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		}
	}
	else if( status == START)
	{
		if( statusTime < 60 )
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glColor4d(0.0, 0.0, 0.0, 0.75 - (double)statusTime/80.0); 
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(windowSize.x, 0.0f);
			glVertex2f(windowSize.x, windowSize.y);
			glVertex2f(0.0f, windowSize.y);
			glEnd();
			glDisable(GL_BLEND);
		}
		else if( statusTime < 150 )
		{
			int remainTime = 3-(statusTime-60)/30;
			char str[4];
			sprintf_s(str, "%d", remainTime);

			drawText((int)(windowSize.x/2.0f)-210, (int)(windowSize.y/2.0f)-80,
				"GAME START",largeFont, Color4f(1.0f, 0.3f, 0.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
			drawText((int)(windowSize.x/2.0f)-48, (int)(windowSize.y/2.0f)+40,
				str,largeFont, Color4f(1.0f, 0.3f, 0.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		}
	}
	else if( status == MAIN )
	{
		Vector3f pos = player.GetPos();
		sprintf_s(PosXString, "X : %.1f", pos.x);
		sprintf_s(PosYString, "Y : %.1f", pos.y);
		sprintf_s(PosZString, "Z : %.1f", pos.z);
		char timeStr[16];
		sprintf_s(timeStr, "TIME：%d", statusTime/30);
		char itemStr[3][16];
		sprintf_s(itemStr[0], "芋：%d", item.GetNum(POTATO));
		sprintf_s(itemStr[1], "肉：%d", item.GetNum(MEAT));
		sprintf_s(itemStr[2], "本：%d", item.GetNum(BOOK));

		// 座標とFPSの描画
		glColor4f(1.0, 1.0, 1.0, 1.0); 
		glRasterPos2i(15, 15);
		glutRenderText(GLUT_BITMAP_HELVETICA_12, FpsString);
		glRasterPos2i(15, 30);
		glutRenderText(GLUT_BITMAP_HELVETICA_12, PosXString);
		glRasterPos2i(15, 45);
		glutRenderText(GLUT_BITMAP_HELVETICA_12, PosYString);
		glRasterPos2i(15, 60);
		glutRenderText(GLUT_BITMAP_HELVETICA_12, PosZString);

		// 各アイテムの取得数描画
		drawText(10, 100, timeStr,smallFont, Color4f(0.0f, 1.0f, 1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		drawText(10, 135, itemStr[0],smallFont, Color4f(0.0f, 1.0f, 1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		drawText(10, 170, itemStr[1],smallFont, Color4f(0.0f, 1.0f, 1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		drawText(10, 205, itemStr[2],smallFont, Color4f(0.0f, 1.0f, 1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);

#if 0
		glColor4d(1.0, 1.0, 1.0, 1.0); 
		glBegin(GL_LINES); 
			glVertex2i( (int)windowSize.x-30, 30 );     
			glVertex2i( (int)windowSize.x-15, 30 );
			glVertex2i( (int)windowSize.x-15, 30 );
			glVertex2i( (int)windowSize.x-23, 15 );
			glVertex2i( (int)windowSize.x-23, 15 );
			glVertex2i( (int)windowSize.x-23, 45 );
		glEnd();
#endif

		// ここからMAP描画

		// プレイヤー位置描画
		glColor4d(0.0, 1.0, 0.0, 1.0);
		DrawSmallCircle(6.0f, (int)windowSize.x-100, 100 );
		glColor4d(0.0, 0.0, 0.0, 1.0);
		DrawSmallCircle(7.0f, (int)windowSize.x-100, 100 );

		// アイテム位置描画
		Vector3f pPos = player.GetPos();
		for( int i=0; i<ITEM_NUM; i++ )
		{
			Vector3f iPos = item.GetPos(i);
			float dis = iPos.distance(pPos);
			if( dis<66.0f )
			{
				if( item.GetType(i) == POTATO )
					glColor4d(1.0, 0.9, 0.7, 1.0);
				else if( item.GetType(i) == MEAT )
					glColor4d(1.0, 0.5, 0.15, 1.0);
				else
					glColor4d(1.0, 0.0, 1.0, 1.0);
				float angle = player.GetAngleXZ();
				Vector2f before, after;
				before = Vector2f(iPos.z-pPos.z, iPos.x-pPos.x);
				after.x = cos(-angle)*before.x - sin(-angle)*before.y;
				after.y = sin(-angle)*before.x + cos(-angle)*before.y;
				DrawSmallCircle(6.0f, (int)windowSize.x-100-(int)after.y, 100-(int)after.x );
				glColor4d(0.0, 0.0, 0.0, 1.0);
				DrawSmallCircle(7.0f, (int)windowSize.x-100-(int)after.y, 100-(int)after.x );
			}
		}
		// 敵位置描画
		for( int i=0; i<enemyNum; i++ )
		{
			Vector3f ePos = enemy[i].GetPos();
			float dis = ePos.distance(pPos);
			if( dis<66.0f )
			{
				if( enemy[i].GetStatus() == WARN )
					glColor4d(1.0, 1.0, 0.0, 1.0);
				else if( enemy[i].GetStatus() == RUN )
					glColor4d(1.0, 0.0, 0.0, 1.0);
				else
					glColor4d(0.0, 0.0, 1.0, 1.0);
				float angle = player.GetAngleXZ();
				Vector2f before, after;
				before = Vector2f(ePos.z-pPos.z, ePos.x-pPos.x);
				after.x = cos(-angle)*before.x - sin(-angle)*before.y;
				after.y = sin(-angle)*before.x + cos(-angle)*before.y;
				DrawSmallCircle(6.0f, (int)windowSize.x-100-(int)after.y, 100-(int)after.x );
				glColor4d(0.0, 0.0, 0.0, 1.0);
				DrawSmallCircle(7.0f, (int)windowSize.x-100-(int)after.y, 100-(int)after.x );
			}
		}

		// 枠描画
		glColor4d(1.0, 1.0, 1.0, 1.0);
		DrawLargeCircle(72.0f, (int)windowSize.x-100, 100 );
		glColor4d(0.5, 0.5, 0.5, 1.0);
		DrawLargeCircle(75.0f, (int)windowSize.x-100, 100 );

		// タイムアップ残り時間表示
		if( statusTime > (TIMEUP - 3)*30 )
		{
			int remainTime = 3-(statusTime-(TIMEUP - 3)*30)/30;
			char str[4];
			sprintf_s(str, "%d", remainTime);

			drawText((int)(windowSize.x/2.0f)-48, (int)(windowSize.y/2.0f)+40,
				str,largeFont, Color4f(1.0f, 0.3f, 0.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		}
	}
	else if( status == CONTINUE )
	{
		int remainTime = 10-statusTime/30;

		char str[4];
		sprintf_s(str, "%d", remainTime);
		glColor4d(0.9, 0.9, 0.9, 1.0); 
		glRasterPos2i((int)(windowSize.x/2.0f)-210, (int)(windowSize.y/2.0f)-80);
		drawText("CONTINUE?",onryouFont96,4);
		glRasterPos2i((int)(windowSize.x/2.0f)-48, (int)(windowSize.y/2.0f)+40);
		drawText(str,onryouFont96,4);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4d(0.0, 0.0, 0.0, 0.5); 
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(windowSize.x, 0.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glVertex2f(0.0f, windowSize.y);
		glEnd();
		glDisable(GL_BLEND);
	}
	else if( status == ABE )
	{
		glColor4d(0.9, 0.9, 0.9, 1.0);
		glRasterPos2i((int)(windowSize.x/2.0f)-350, (int)(windowSize.y/2.0f)-40);
		drawText("あなたはノンケですか？",largeFont,4);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4d(0.0, 0.0, 0.0, 0.5); 
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(windowSize.x, 0.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glVertex2f(0.0f, windowSize.y);
		glEnd();
		glDisable(GL_BLEND);
	}
	else if( status == GAMEOVER)
	{
		double color;
		if( statusTime<60 )
			color = (double)statusTime/60.0;
		else if( statusTime < 90 )
			color = 1.0;
		else
			color = 1.0 - (double)(statusTime-90)/150.0;
		glColor4d(color, 0.0, 0.0, 1.0); 
		glRasterPos2i((int)(windowSize.x/2.0f)-210, (int)(windowSize.y/2.0f)-70);
		drawText("GAME OVER",onryouFont96,4);

		glColor4d(0.0, 0.0, 0.0, 1.0); 
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(windowSize.x, 0.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glVertex2f(0.0f, windowSize.y);
		glEnd();
	}
	else if( status == ABE_END )
	{
		char str1[] = "俺はノンケだってかまわないで　";
		char str2[] = "食っちまう人間なんだぜ　";
		if( statusTime < 10 )
			;
		else if( statusTime < 120 )
		{
			int strLen = (statusTime-10)/4+1;
			if( strLen <= 14 )
			{
				str1[strLen*2] = '\0';
				str2[0] = '\0';
			}
			else if( strLen <= 25 )
				str2[(strLen-14)*2] = '\0';
			glColor4d(1.0, 1.0, 1.0, 1.0); 
			glRasterPos2i((int)(windowSize.x/2.0f)-300, (int)(windowSize.y/2.0f)-50);
			drawText(str1,defFont,4);
			glRasterPos2i((int)(windowSize.x/2.0f)-260, (int)(windowSize.y/2.0f)+50);
			drawText(str2,defFont,4);
		}
		else
		{
			double color;
			if( statusTime<150 )
				color = 1.0;
			else
				color = 1.0 - (double)(statusTime-150)/120.0;
			glColor4d(color, 0.0, 0.0, 1.0); 
			glRasterPos2i((int)(windowSize.x/2.0f)-230, (int)(windowSize.y/2.0f)-70);
			drawText("GAME OVER♂",onryouFont96,4);
		}
		glColor4d(0.0, 0.0, 0.0, 1.0); 
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(windowSize.x, 0.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glVertex2f(0.0f, windowSize.y);
		glEnd();
	}
	else if( status == ABE_END2 )
	{
		char str[] = "やらないか　";
		if( statusTime < 30 )
			;
		else if( statusTime < 80 )
		{
			int strLen = (statusTime-30)/6+1;
			if( strLen <= 5 )
				str[strLen*2] = '\0';

			glColor4d(1.0, 1.0, 1.0, 1.0); 
			glRasterPos2i((int)(windowSize.x/2.0f)-200, (int)(windowSize.y/2.0f)-30);
			drawText(str,largeFont,4);
		}
		else
		{
			double color;
			if( statusTime<110 )
				color = 1.0;
			else
				color = 1.0 - (double)(statusTime-110)/120.0;
			glColor4d(color, 0.0, 0.0, 1.0); 
			glRasterPos2i((int)(windowSize.x/2.0f)-230, (int)(windowSize.y/2.0f)-70);
			drawText("GAME OVER♂",onryouFont96,4);
		}
		glColor4d(0.0, 0.0, 0.0, 1.0); 
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(windowSize.x, 0.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glVertex2f(0.0f, windowSize.y);
		glEnd();
	}
	else if( status == GAMECLEAR )
	{
		if( statusTime < 60 )
		{
			drawText((int)(windowSize.x/2.0f)-150, (int)(windowSize.y/2.0f)-80,
				"TIMEUP",largeFont, Color4f(1.0f, 0.3f, 0.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 1.0f), 2, 4);
		}
		else if( statusTime < 120 )
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glColor4d(0.0, 0.0, 0.0, (double)(statusTime-60)/60.0); 
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(windowSize.x, 0.0f);
			glVertex2f(windowSize.x, windowSize.y);
			glVertex2f(0.0f, windowSize.y);
			glEnd();
			glDisable(GL_BLEND);
		}
		else if( statusTime < 270 )
		{
			double color;
			if( statusTime<150 )
				color = 1.0;
			else
				color = 1.0 - (double)(statusTime-150)/120.0;
			glColor4d(color, color, 0.0, 1.0); 
			glRasterPos2i((int)(windowSize.x/2.0f)-210, (int)(windowSize.y/2.0f)-70);
			drawText("GAME CLEAR",largeFont,4);
			glColor4d(0.0, 0.0, 0.0, 1.0); 
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(windowSize.x, 0.0f);
			glVertex2f(windowSize.x, windowSize.y);
			glVertex2f(0.0f, windowSize.y);
			glEnd();
		}
		else
		{
			int point = item.GetNum( POTATO ) + item.GetNum( MEAT )*10;
			float color;
			if( statusTime>480 )
				color = 1.0f - (float)(statusTime-480)/80.0f;
			else
				color = 1.0f ;

			drawText(50, 120,
				"成果",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
			if( statusTime >= 315 )
			{
				if( point < 10 )
				{
					drawText(70, 240,
						"何の成果も得られませんでしたぁぁ！！",defFont, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				}
				else
				{
					char itemStr[2][16];
					sprintf_s(itemStr[0], "芋：%d", item.GetNum(POTATO));
					sprintf_s(itemStr[1], "肉：%d", item.GetNum(MEAT));
					drawText(70, 210, itemStr[0],defFont, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
					drawText(70, 280, itemStr[1],defFont, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);

				}
			}
			if( statusTime >= 375 )
			{
				drawText(50, 400,
					"評価",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
			}
			if( statusTime >= 420 )
			{
				if( point < 10 )
					drawText(70, 500,
						"orz",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				else if( point < 25 )
					drawText(70, 500,
						"訓練兵団級",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				else if( point < 40 )
					drawText(70, 500,
						"憲兵団級",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				else if( point < 55 )
					drawText(70, 500,
						"駐屯兵団級",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				else if( point < 70 )
					drawText(70, 500,
						"調査兵団級",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
				else
					drawText(70, 500,
						"リヴァイ班級",largeFont2, Color4f(color, color, color, 1.0f), Color4f(0.3f*color, 0.3f*color, 0.3f*color, 1.0f), 2, 4);
			}
			glColor4d(0.0, 0.0, 0.0, 1.0); 
			glBegin(GL_QUADS);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(windowSize.x, 0.0f);
			glVertex2f(windowSize.x, windowSize.y);
			glVertex2f(0.0f, windowSize.y);
			glEnd();
		}
	}

#if 1
	if( status == MAIN )
	{
		cv::Mat img = kinect.DrawUser();
		UCHAR* imageData = &img.data[0];
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.cols, img.rows, GL_RGBA, GL_UNSIGNED_BYTE, imageData );
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(windowSize.x*0.75f, windowSize.y*0.75f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(windowSize.x, windowSize.y*0.75f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(windowSize.x, windowSize.y);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(windowSize.x*0.75f, windowSize.y);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
#endif
}

void GameManager::Render3D()
{
	if( status != GAMEOVER && status != ABE_END && !(status == GAMECLEAR && statusTime>=120) )
	{
		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
		glUniform1i(glGetUniformLocation(shaderProgram, "toontex"), 2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		player.Render();
		wire.RenderModel();

		if( status == TITLE )
		{
			for(int i=0; i<ENEMY_NUM; i++)
				enemy[i].Render( true );
		}
		else
		{
			for(int i=0; i<enemyNum; i++)
				enemy[i].Render();
		}

		glDisable(GL_BLEND);
		glUseProgram(0);

		glAlphaFunc(GL_GREATER, 0.5);
		glEnable(GL_ALPHA_TEST);
		castleGround.Render();
		castleGarden.Render();
		castleDining.Render();
		castleKingBench.Render();
		ground.Render();
		royal.Render();
		resident.Render();
		port.Render();
		business.Render();
		skydome.Render();
		glDisable(GL_ALPHA_TEST);

		item.Render();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		if( effectIndex != -1 )
		{
			glPushMatrix();
			{
				Vector3f pos = player.GetPos();
				float angleXZ = player.GetAngleXZ();

				if( viewType == 0 )
					effect.SetCamera(pos.x, pos.y, pos.z-2.0f,
									 pos.x, pos.y, pos.z+5.0f);
				else
					effect.SetCamera(pos.x+2.0f*sin(-angleXZ), pos.y, pos.z+2.0f*cos(-angleXZ)-1.0f,
									 pos.x-5.0f*sin(-angleXZ), pos.y, pos.z-5.0f*cos(-angleXZ));
				if( effectIndex == 3 )
					effect.Translate(effectIndex, effectPos);
				else
					effect.Translate(effectIndex, pos);
				effect.Render();
			}
			glPopMatrix();
		}
		glDisable(GL_BLEND);

		wire.RenderWire();
	}
}

void GameManager::Update()
{
	char buff[128] = "\0";

	if( useJulius )
	{
		WaitForSingleObject(hMutex,INFINITE);
		strcpy_s(buff, voiceStr);
		voiceStr[0]='\0';
		ReleaseMutex(hMutex);
	}
	else
	{
		strcpy_s(buff, voiceStr);
		voiceStr[0]='\0';
	}

	alListener3f( AL_POSITION, 0.0f, 0.0f, 0.0f );

	if( !strcmp( buff, "終了" ) )
		glutLeaveMainLoop();

	if( status == TITLE )
	{
		if( statusTime == 1 )
		{
			player.Reset();
			wire.Reset();
			item.Reset();
			for( int i=0; i<ENEMY_NUM; i++ )
				enemy[i].Init();
#ifndef _TEST_
			enemyNum = 5;
#else
			enemyNum = 1;
#endif
			bgm[5].Play();
		}

		if( !strcmp( buff, "スタート" ) )
		{
			bgm[5].Stop();
			se[1].Play();
			status = START;
			statusTime = 0;
		}
	}
	else if( status == START  )
	{
		if( statusTime == 150 )
		{
			status = MAIN;
			statusTime = 0;
		}
	}
	else if( status == MAIN )
	{
		if( !strcmp( buff, "ジャンプ" ) )
		{
			if( !player.IsInAir() )
			{
				player.Move( Vector3f(0.0f, 0.5f, 0.0f) );
				player.Jump();
			}
		}
		else if( !strcmp( buff, "ワイヤーショット" ) )
		{
			wire.Shot();
		}
		else if( !strcmp( buff, "リスポーン" ) )
		{
			player.Init();
		}
		else if( !strcmp( buff, "レーザー" ) )
		{
			effectViewType = viewType;
			if( effectIndex!=-1 )
				effect.Stop(effectIndex);
			effect.Play(0, player.GetPos());
			effectIndex = 0;
		}
		else if( !strcmp( buff, "サンダー" ) )
		{
			effectViewType = viewType;
			if( effectIndex!=-1 )
				effect.Stop(effectIndex);
			effect.Play(1, player.GetPos());
			effectIndex = 1;
		}

		if( statusTime == 1 )
			bgm[3].Play();

		if( useKinect )
		{
			kinect.Update();
			player.UpdateByKinect( kinect.GetBoneRot() );
			wire.Update();
		}

		if( wire.GetStatus(0)==TAKE_UP  )
		{
			if( player.Move()==1 )
			{
				cout << "wire finished." << endl;
				player.SetSpeed( Vector3f() );
				wire.SetStatus(0, NO_TARGET);
				wire.SetStatus(1, NO_TARGET);
			}
			else
			{
				cout << "wire move." << endl;
				player.AddSpeed( Vector3f(0.0f, -0.05f, 0.0f) );
				if( Vector2f( player.GetSpeed().x, player.GetSpeed().z ).length() > 0.1f )
				{
					Vector2f vec = Vector2f( player.GetSpeed().x, player.GetSpeed().z ).normalize()*0.01f;
					player.AddSpeed( Vector3f(-vec.x, 0.0f, -vec.y) );
				}
			}

			player.StopAnim( 0 );
		}
		else if( wire.GetStatus(0)==CIRCLE  )
		{
			if( player.Move()==1 )
			{
				player.SetSpeed( Vector3f() );
				wire.SetStatus(0, NO_TARGET);
				wire.SetStatus(1, NO_TARGET);
			}

			player.StopAnim( 0 );
		}
		else
		{
			if( player.IsInAir() )
			{
				if( player.Move()==1 )
					player.SetSpeed( Vector3f() );
				else
					player.AddSpeed( Vector3f(0.0f, -0.05f, 0.0f) );
		
				player.StopAnim( 0 );
			}
			else
			{
				float speed = 0.0f;
				if( useKinect )
				{
					player.RotateXZ(kinect.GetBodyAngleXZ());
					speed = kinect.GetBodyAngleY();
				}
				if( speed > 0.0f )
				{
					player.SetSpeed( speed );
					player.Move();
					player.SetAnimSpeed( 0, speed*3.0f );
					player.PlayStepSound();
				}
				else
					player.StopAnim( 0 );
			}
		}
		Vector3f pPos = player.GetPos();
		float pAngle = player.GetAngleXZ();
		ALfloat orient[] = {sin(pAngle), 0.0f, cos(pAngle), 0.0f, 1.0f, 0.0f};
		alListener3f( AL_POSITION, pPos.x, pPos.y+3.0f, pPos.z );
		alListenerfv( AL_ORIENTATION, orient );

		int normalCount = 0;
		int warnCount = 0;
		int runCount = 0;
		for(int i=0; i<enemyNum; i++)
		{
			enemy[i].Update();
			if( enemy[i].IsStepOn() )
			{
				status = CONTINUE;
				statusTime = 0;
				bgm[3].Stop();
				bgm[4].Stop();
				break;
			}
			else if( enemy[i].GetStatus() == WARN )
				warnCount++;
			else if( enemy[i].GetStatus() == RUN )
				runCount++;
			else
				normalCount++;
		}
		if( normalCount == enemyNum && battleStatus != 0 )
		{
			bgm[4].Stop();
			bgm[3].Play();
			battleStatus = 0;
		}
		else if( runCount > 0 && battleStatus != 2 )
		{
			bgm[3].Stop();
			bgm[4].Play();
			battleStatus = 2;
		}
		else if( warnCount > 0 && battleStatus == 0 )
		{
			bgm[3].Pause();
			bgm[4].Pause();
			battleStatus = 1;
		}

		for(int i=0; i<ITEM_NUM; i++)
		{
			Vector3f iPos = item.GetPos(i);
			if( abs( pPos.y-iPos.y ) < 4.0f )
			{
				if( Vector2f(pPos.x,pPos.z).distance(Vector2f(iPos.x,iPos.z)) < 2.0f )
				{
					item.Gain(i);
					item.Repop(i);
				}
			}
		}

		ResetDepth2();
		for(int i=0; i<enemyNum; i++)
			UpdateDepth2( enemy[i].GetPos().x, enemy[i].GetPos().z, enemy[i].GetSize()/4.0f, enemy[i].GetSize()*2.0f );

		if( !strcmp( buff, "やらないか" ) )
		{
			int count = 0;
			Vector3f pPos = player.GetPos();
			float pAngle = player.GetAngleXZ();

			for(int i=0; i<enemyNum; i++)
				if( enemy[i].GetType()==3 )
				{
					count++;
					float d = 0.0f;
					if( count == 2 )
						d = 0.6f;
					else if( count == 3 )
						d = -0.6f;
					else if( count > 3 )
						break;

					Vector3f pos;
					pos.x = pPos.x + 5.0f * sin(pAngle+d);
					pos.z = pPos.z + 5.0f * cos(pAngle+d);
					pos.y = GetDepthData( pos.x, pos.z );
					enemy[i].Init();
					enemy[i].SetPos(pos);
					float angle = pAngle + d + 3.1416f;
					if( angle > 6.283184f )
						angle -= 6.283184f;
					enemy[i].SetAngleXZ(angle);
				}
			if( count > 0 )
			{
				status = ABE;
				statusTime = 0;
				bgm[3].Stop();
				bgm[4].Stop();
				bgm[6].Play();
			}
		}
#ifndef _TEST_
		if( statusTime != 0 && ENEMY_NUM > 5 && (statusTime %(TIMEUP*30/(ENEMY_NUM-5))) == 0 && enemyNum < ENEMY_NUM )
		{
			enemyNum++;
			effectViewType = viewType;
			if( effectIndex!=-1 )
				effect.Stop(effectIndex);
			effectIndex = 3;
			effectPos = enemy[enemyNum-1].GetPos();
			effect.Play(3, effectPos);
			se[2].SetPosition(effectPos);
			se[2].Play();
		}
#endif
		if( statusTime == TIMEUP*30 )
		{
			bgm[3].Stop();
			bgm[4].Stop();
			status = GAMECLEAR;
			statusTime = 0;
		}
	}
	else if( status == CONTINUE )
	{
		if( statusTime == 1 )
			bgm[2].Play();

		player.StopAnim( 0 );

		if( !strcmp( buff, "はい" ) )
		{
			player.Reset();
			wire.Reset();
			item.Reset();
			for( int i=0; i<enemyNum; i++ )
				enemy[i].Init();
#ifndef _TEST_
			enemyNum = 5;
#else
			enemyNum = 1;
#endif
			status = START;
			statusTime = 0;

			bgm[2].Stop();
		}
		else if( !strcmp( buff, "いいえ" ) || statusTime == 300 )
		{
			status = GAMEOVER;
			statusTime = 0;

			bgm[2].Stop();
		}
	}
	else if( status == GAMEOVER )
	{
		if( statusTime == 300 )
		{
			status = TITLE;
			statusTime = 0;
		}
		if( statusTime == 1 )
			bgm[1].Play();
	}
	else if( status == ABE_END )
	{
		if( statusTime == 300 )
		{
			status = TITLE;
			statusTime = 0;
		}
		if( statusTime == 15 )
			se[3].Play();
		if( statusTime == 135 )
			se[4].Play();
	}
	else if( status == ABE_END2 )
	{
		if( statusTime == 230 )
		{
			status = TITLE;
			statusTime = 0;
		}
		if( statusTime == 15 )
			se[5].Play();
		if( statusTime == 95 )
			se[4].Play();
	}
	else if( status == GAMECLEAR )
	{
		if( statusTime == 560 )
		{
			status = TITLE;
			statusTime = 0;
		}
		if( statusTime == 270 )
			bgm[0].Play();
		else if( statusTime == 315 || statusTime == 420 )
			se[0].Play();
	}
	else if( status == ABE )
	{
		if( !strcmp( buff, "はい" ) )
		{
			status = ABE_END;
			statusTime = 0;
			bgm[6].Stop();
		}
		else if( !strcmp( buff, "いいえ" ) )
		{
			status = ABE_END2;
			statusTime = 0;
			bgm[6].Stop();
		}
		if( useKinect )
		{
			kinect.Update();
			player.UpdateByKinect( kinect.GetBoneRot() );
		}
	}
	statusTime++;
}

bool GameManager::LoadConfig()
{
	FILE *fp;
	char str[128];
	char *p = NULL;
	char *name = NULL;
	char *value = NULL;
	int val;

	if( fopen_s( &fp, "data\\config.ini", "r" ) )
	{
		cout << "Load Config Error : can't open file data\\config.ini" << endl;
		return false;
	}
	for(;;) {
		if( fgets(str, 128, fp) == NULL )
		{
            fclose(fp);
            break;
        }
		if( (p = strchr(str, '=')) == NULL )
		{
			fclose(fp);
			return false;
		}
		name = str;
		value = p+1;
		while( *name == ' ' )
			name++;
		if( name == p )
			return false;
		while( *value == ' ' )
			value++;
		if( *value == '\n' )
			return false;
		if( *(value+1)!=' ' && *(value+1)!='\n' || !isdigit(*value) )
			return false;
		val = *value - '0';
		if( val != 0 && val != 1 )
			return false;
		if( strncmp(name, "useKinect", 9) == 0 )
		{
			if( val == 1 )
				useKinect = true;
			else
				useKinect = false;
		}
		else if( strncmp(name, "useMic", 6) == 0 )
		{
			if( val == 1 )
				useJulius = true;
			else
				useJulius = false;
		}
		else
			return false;
    }

	return true;
}

void GameManager::SetWindowSize( int x, int y )
{
	//　ウィンドウサイズを保存
	windowSize.x = (float)x;
	windowSize.y = (float)y;

	//　サイズチェック
	if ( windowSize.x < 10.0f ) windowSize.x = 10.0f;
	if ( windowSize.y < 10.0f ) windowSize.y = 10.0f;

	//　ビューポートの設定
	glViewport(0, 0, (int)windowSize.x, (int)windowSize.y);
}

unsigned __stdcall LoadingScreen( void* pArguments )
{
	cv::Mat img((int)windowSize.y, (int)windowSize.x, CV_8UC3);

	const char* text[4] = {"Loading   ", "Loading.  ", "Loading.. ", "Loading..."};
	const string windowTitle( "Loading" );
	cv::namedWindow(windowTitle, CV_WINDOW_AUTOSIZE);
	cvInitJPN(windowTitle.c_str());
	CvFontJPN jpFont;
	cvInitFontJPN(&jpFont, _T("ＭＳ Ｐ明朝"), 36, NULL, NULL, TEXT_BOLD, NULL);

	int i=0;

	while (1) {
		DWORD start, end;
		start = timeGetTime();

		cvMoveWindow(windowTitle.c_str(), (int)windowPos.x, (int)windowPos.y);

		if( useKinect )
		{
			kinect.Update();
			cv::resize(kinect.DrawUser(), img, cv::Size(), 3.0, 3.0);

			if( !kinect.IsReady() )
				cvPutTextJPN(windowTitle.c_str(), _T("Kinectの前に真っ直ぐ立ってください。　　　　　"),cvPoint(150, 250), &jpFont, CV_RGB(0,0,0));
			else if( kinect.GetBodyAngleY()>0.0f )
				cvPutTextJPN(windowTitle.c_str(), _T("体が傾いています。真っ直ぐ立ってください。"),cvPoint(150, 250), &jpFont, CV_RGB(0,0,0));
			else
				cvPutTextJPN(windowTitle.c_str(), _T("そのままの姿勢でしばらくお待ちください。　　"),cvPoint(150, 250), &jpFont, CV_RGB(0,0,0));
		}
		else
		{
			img = cv::Scalar(255,255,255);
			cv::putText(img, text[i/10], cv::Point((int)windowSize.x-325,(int)windowSize.y-75), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 2.0, cv::Scalar(0,0,0), 2, CV_AA);
		}

		cv::imshow(windowTitle, img);

		end = timeGetTime();
		if( end-start<66 )
			cvWaitKey(66-(end-start));

		void* handle = cvGetWindowHandle( windowTitle.c_str() );
		if( loadFinishFlag || handle == NULL)
		{
			loadFinishFlag = true;
			break;
		}
		else
		{
			// このままではなぜか上手くいかないので、要改良
			LPRECT rect = NULL;
			if( GetWindowRect( (HWND)handle, rect ) )
			{
				windowPos.x = (float)rect->left;
				windowPos.y = (float)rect->top;
			}
		}

		i++;
		if( i==40 )
			i=0;
	}
	cv::destroyWindow(windowTitle);

	return 0;
}

void GameManager::DrawLoadScreen()
{
	loadFinishFlag = false;
	mutex = CreateMutex(NULL,FALSE,NULL);
	thread[0] = (HANDLE)_beginthreadex( NULL,0,&LoadingScreen, NULL,0,NULL);
}

float GameManager::GetWindowWidth()
{
	return windowSize.x;
}

float GameManager::GetWindowHeight()
{
	return windowSize.y;
}

float GameManager::GetWindowPosX()
{
	return windowPos.x;
}

float GameManager::GetWindowPosY()
{
	return windowPos.y;
}

void glutRenderText(void* bitmapfont, char*text)
{
	for ( int i=0; i<(int)strlen(text); i++ )
		glutBitmapCharacter(bitmapfont, (int)text[i]);
}

int drawText(char *text, HFONT hfont, GLfloat xmove)
{
	unsigned int textLength;	//引数で受け取ったテキストの長さ
	WCHAR * unicodeText;		//textをUNICODEに変換した文字列を格納する
	GLuint listbaseIdx;		//ディスプレイリストの最初のインデックス


	//日本語の文字列として扱うよう設定
	setlocale(LC_CTYPE, "jpn");

	//textの文字数を取得
	textLength = (unsigned int)_mbstrlen(text);
	if(textLength == -1)
		return -1;

	//textの文字数分のワイド文字列の領域を作成
	unicodeText = new WCHAR[textLength + 1];
	if(unicodeText == NULL)
	{
		return -2;
	}

	//取得したジョイントIDをUNICODEに変換する
	if(MultiByteToWideChar(CP_ACP,	0, text, -1, unicodeText, (sizeof(WCHAR) * textLength) + 1) == 0)
		return -3;

	HDC hdc = wglGetCurrentDC();
	SelectObject(hdc, hfont);

	//文字数分のディスプレイリストを確保し、ディスプレイリストの最初のインデックスを取得
	listbaseIdx = glGenLists(textLength);

	for(unsigned int textCnt = 0; textCnt < textLength; ++textCnt)
	{
		if(wglUseFontBitmapsW(hdc, unicodeText[textCnt], 1, listbaseIdx + textCnt) == FALSE)
		{
			//MessageBox(hwnd, "wglUseFontBitmaps() Error!!", "wgl Error", MB_OK);
		}
	}

	//1文字描画したら文字を何bitずらすか
	glBitmap( 0, 0, 0, 0, xmove/*x move*/, 0/*y move*/, NULL );

	//ディスプレイリストを実行する
	for(unsigned int textCnt = 0; textCnt < textLength; textCnt++)
	{
		glCallList(listbaseIdx + (GLuint)textCnt);
	}

	delete[] unicodeText;
	glDeleteLists(listbaseIdx, textLength);

	return 1;
}

int drawText(int x, int y, char *text, HFONT hfont, Color4f mainColor, Color4f edgeColor, int edgeSize, GLfloat xmove)
{
	// 文字列描画
	glColor4f(mainColor.r, mainColor.g, mainColor.b, mainColor.a); 
	glRasterPos2i(x, y);
	drawText(text,hfont,xmove);

	// 縁取り
	if( edgeSize > 0 )
	{
		glColor4f(edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a); 
		for( int i=-edgeSize+1; i<edgeSize; i++ )
		{
			glRasterPos2i(x+i, y+(edgeSize-i));
			drawText(text,hfont,xmove);
			glRasterPos2i(x+i, y-(edgeSize-i));
			drawText(text,hfont,xmove);
		}
		glRasterPos2i(x+edgeSize, y);
		drawText(text,hfont,xmove);
		glRasterPos2i(x-edgeSize, y);
		drawText(text,hfont,xmove);
	}

	return 1;
}

void DrawSmallCircle(float radius,int x,int y)
{
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(radius*2.0f);
	glBegin(GL_POINTS);
		glVertex2f( (float)x, (float)y );
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
}

void DrawLargeCircle(float radius,int x,int y)
{
	for (float th1 = 0.0f;  th1 <= 360.0f;  th1 = th1 + 2.0f)
	{             
		float th2 = th1 + 10.0f;
		float th1_rad = th1 * 0.0174533f; 
		float th2_rad = th2 * 0.0174533f;

		float x1 = radius * cos(th1_rad);
		float y1 = radius * sin(th1_rad);
		float x2 = radius * cos(th2_rad);
		float y2 = radius * sin(th2_rad);

		glBegin(GL_TRIANGLES); 
			glVertex2f( (float)x, (float)y );
			glVertex2f( x1+x, y1+y );     
			glVertex2f( x2+x, y2+y );
		glEnd();
	} 
}