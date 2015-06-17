#if 1
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif

#include "GamaManager.h"

#define WINDOW_TITLE "G A M E"

GameManager gm;

void Initialize();
void Shutdown();
void Display();
void Time(int);
void Reshape(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Special(int key, int x, int y);

//----------------------------------------------------------------------------------------------------
// 名前 : main()
// 内容 : アプリケーションのエントリーポイント
//----------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
	cout.setf(ios::showpoint);

	//　GLUTウィンドウの作成
	glutInit(&argc, argv);
	glutInitWindowPosition(WINDOW_DEFAULT_POS_X, WINDOW_DEFAULT_POS_Y);
	glutInitWindowSize(WINDOW_DEFAULT_W, WINDOW_DEFAULT_H);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(WINDOW_TITLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutHideWindow();

	// GLEWの初期化
	if (glewInit() != GLEW_OK)
	{
		cout << "main : failed to initialize glew" << endl;
		MessageBox(NULL, TEXT("GLEWの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return -1;
	}

	// Alutの初期化
	alutInit(NULL,0);
	if(alGetError() != AL_NO_ERROR)
	{
		cout << "main : failed to initialize alut" << endl;
		MessageBox(NULL, TEXT("ALUTの初期化に失敗しました。"), TEXT("初期化エラー"), MB_OK);
		return -1;
	}

	// ゲームマネージャーの初期化
	if( !gm.Initialize() )
	{
		gm.Release();
		return -1;
	}

	//GLのデバイスコンテキストハンドル取得
    HDC glDc = wglGetCurrentDC();
    //ウィンドウハンドル取得
    HWND hWnd = WindowFromDC(glDc);
	//ウィンドウの属性と位置変更
	SetWindowPos(hWnd, HWND_TOP,
		(int)gm.GetWindowPosX(),(int)gm.GetWindowPosY(),
		(int)gm.GetWindowWidth(),(int)gm.GetWindowHeight(),
		SWP_SHOWWINDOW);

	// GLUTコールバック関数のセット
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc( 100, Time, 0 );
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	// GLの初期化
	Initialize();

	// メインループ
	glutMainLoop();

	// 後片付け
	Shutdown();

	return 1;
}

//----------------------------------------------------------------------------------------------------
// 名前 : Initialize()
// 内容 : GLとALの初期化
//----------------------------------------------------------------------------------------------------
void Initialize()
{
	//　バックバッファをクリアする色の指定
	glClearColor(0.3f, 0.3f, 1.0f, 1.0f);

	//　深度テストON
	glEnable(GL_DEPTH_TEST);

	//　ライティング
	float pos[4] = { 1.0f, 4.0f, 1.0f, 0.0f };
	float amb[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float dif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spe[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

	alDistanceModel( AL_INVERSE_DISTANCE );

	//　ウィンドウを表示
	glutShowWindow();
}

//----------------------------------------------------------------------------------------------------
// 名前 : Time()
// 内容 : 33ms毎に再描画されるようにするためのコールバック関数
//----------------------------------------------------------------------------------------------------
void Time(int)
{
	//　33ms毎に描画
	glutPostRedisplay();
#if 0
	if( gm.GetStatus() != TITLE )
		glutTimerFunc( 33, Time, 0 );
	else
		glutTimerFunc( 66, Time, 0 );
#else
	glutTimerFunc( 33, Time, 0 );
#endif
}

//----------------------------------------------------------------------------------------------------
// 名前 : Reshape()
// 内容 : ウィンドウのサイズが変更された時に呼ばれるコールバック関数
//----------------------------------------------------------------------------------------------------
void Reshape(int x, int y)
{
	gm.SetWindowSize(x,y);
}

//----------------------------------------------------------------------------------------------------
// 名前 : Display()
// 内容 : 描画用コールバック関数
//----------------------------------------------------------------------------------------------------
void Display()
{
	gm.Update();

	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gm.SetView();

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	
	//　3Dシーンの描画
	gm.Render3D();	

	bool isLighting = false;
	//　3D　→　2D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, (double)gm.GetWindowWidth(), (double)gm.GetWindowHeight(), 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if ( glIsEnabled(GL_LIGHTING) )
	{
		isLighting = true;
		glDisable(GL_LIGHTING);
	}

	//　2Dシーンの描画
	gm.Render2D();

	if ( !glIsEnabled(GL_LIGHTING) )
	{
		if ( isLighting )
			glEnable(GL_LIGHTING);
	}

	//　2D　→　3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//
	glPopMatrix();

	//　ダブルバッファ
	glutSwapBuffers();
}

//----------------------------------------------------------------------------------------------------
// 名前 : Keyboard()
// 内容 : キーボード処理用コールバック関数
//----------------------------------------------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch ( (int)key )
	{
	case 243:	// Esc
		exit(0);
		break;
	case 119:	// W
		gm.KeyFunc( KEY_W );
		break;
	case 97:	// A
		gm.KeyFunc( KEY_A );
		break;
	case 115:	// S
		gm.KeyFunc( KEY_S );
		break;
	case 100:	// D
		gm.KeyFunc( KEY_D );
		break;
	case 118:	// V
		gm.KeyFunc( KEY_V );
		break;
	case 32:	// space
		gm.KeyFunc( KEY_SPACE );
		break;
	case 13:	// Enter
		gm.KeyFunc( KEY_ENTER );
		break;
	case 8:	// Enter
		gm.KeyFunc( KEY_BACKSPACE );
		break;
	case 49:	// 1
		gm.KeyFunc( KEY_1 );
		break;
	case 50:	// 2
		gm.KeyFunc( KEY_2 );
		break;
	case 51:	// 3
		gm.KeyFunc( KEY_3 );
		break;
	case 52:	// 3
		gm.KeyFunc( KEY_4 );
		break;
	case 53:	// 3
		gm.KeyFunc( KEY_5 );
		break;

	default:
		cout << (int)key << endl;
		break;
	}
}

//----------------------------------------------------------------------------------------------------
// 名前 : Special()
// 内容 : 特殊キー処理用コールバック関数
//----------------------------------------------------------------------------------------------------
void Special(int key, int x, int y)
{
	switch ( key )
	{
	case GLUT_KEY_LEFT:
		gm.KeyFunc( KEY_LEFT );
		break;
	case GLUT_KEY_RIGHT:
		gm.KeyFunc( KEY_RIGHT );
		break;
	case GLUT_KEY_UP:
		gm.KeyFunc( KEY_UP );
		break;
	case GLUT_KEY_DOWN:
		gm.KeyFunc( KEY_DOWN );
		break;
	}
}

//----------------------------------------------------------------------------------------------------
// 名前 : Shutdown()
// 内容 : 後片付け用
//----------------------------------------------------------------------------------------------------
void Shutdown()
{
	gm.Release();
}
