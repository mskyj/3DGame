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
// ���O : main()
// ���e : �A�v���P�[�V�����̃G���g���[�|�C���g
//----------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
	cout.setf(ios::showpoint);

	//�@GLUT�E�B���h�E�̍쐬
	glutInit(&argc, argv);
	glutInitWindowPosition(WINDOW_DEFAULT_POS_X, WINDOW_DEFAULT_POS_Y);
	glutInitWindowSize(WINDOW_DEFAULT_W, WINDOW_DEFAULT_H);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(WINDOW_TITLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutHideWindow();

	// GLEW�̏�����
	if (glewInit() != GLEW_OK)
	{
		cout << "main : failed to initialize glew" << endl;
		MessageBox(NULL, TEXT("GLEW�̏������Ɏ��s���܂����B"), TEXT("�������G���["), MB_OK);
		return -1;
	}

	// Alut�̏�����
	alutInit(NULL,0);
	if(alGetError() != AL_NO_ERROR)
	{
		cout << "main : failed to initialize alut" << endl;
		MessageBox(NULL, TEXT("ALUT�̏������Ɏ��s���܂����B"), TEXT("�������G���["), MB_OK);
		return -1;
	}

	// �Q�[���}�l�[�W���[�̏�����
	if( !gm.Initialize() )
	{
		gm.Release();
		return -1;
	}

	//GL�̃f�o�C�X�R���e�L�X�g�n���h���擾
    HDC glDc = wglGetCurrentDC();
    //�E�B���h�E�n���h���擾
    HWND hWnd = WindowFromDC(glDc);
	//�E�B���h�E�̑����ƈʒu�ύX
	SetWindowPos(hWnd, HWND_TOP,
		(int)gm.GetWindowPosX(),(int)gm.GetWindowPosY(),
		(int)gm.GetWindowWidth(),(int)gm.GetWindowHeight(),
		SWP_SHOWWINDOW);

	// GLUT�R�[���o�b�N�֐��̃Z�b�g
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc( 100, Time, 0 );
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	// GL�̏�����
	Initialize();

	// ���C�����[�v
	glutMainLoop();

	// ��Еt��
	Shutdown();

	return 1;
}

//----------------------------------------------------------------------------------------------------
// ���O : Initialize()
// ���e : GL��AL�̏�����
//----------------------------------------------------------------------------------------------------
void Initialize()
{
	//�@�o�b�N�o�b�t�@���N���A����F�̎w��
	glClearColor(0.3f, 0.3f, 1.0f, 1.0f);

	//�@�[�x�e�X�gON
	glEnable(GL_DEPTH_TEST);

	//�@���C�e�B���O
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

	//�@�E�B���h�E��\��
	glutShowWindow();
}

//----------------------------------------------------------------------------------------------------
// ���O : Time()
// ���e : 33ms���ɍĕ`�悳���悤�ɂ��邽�߂̃R�[���o�b�N�֐�
//----------------------------------------------------------------------------------------------------
void Time(int)
{
	//�@33ms���ɕ`��
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
// ���O : Reshape()
// ���e : �E�B���h�E�̃T�C�Y���ύX���ꂽ���ɌĂ΂��R�[���o�b�N�֐�
//----------------------------------------------------------------------------------------------------
void Reshape(int x, int y)
{
	gm.SetWindowSize(x,y);
}

//----------------------------------------------------------------------------------------------------
// ���O : Display()
// ���e : �`��p�R�[���o�b�N�֐�
//----------------------------------------------------------------------------------------------------
void Display()
{
	gm.Update();

	//�@�o�b�N�o�b�t�@���N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gm.SetView();

	//�@���f���r���[�s��̐ݒ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	
	//�@3D�V�[���̕`��
	gm.Render3D();	

	bool isLighting = false;
	//�@3D�@���@2D
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

	//�@2D�V�[���̕`��
	gm.Render2D();

	if ( !glIsEnabled(GL_LIGHTING) )
	{
		if ( isLighting )
			glEnable(GL_LIGHTING);
	}

	//�@2D�@���@3D
	glPopMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//
	glPopMatrix();

	//�@�_�u���o�b�t�@
	glutSwapBuffers();
}

//----------------------------------------------------------------------------------------------------
// ���O : Keyboard()
// ���e : �L�[�{�[�h�����p�R�[���o�b�N�֐�
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
// ���O : Special()
// ���e : ����L�[�����p�R�[���o�b�N�֐�
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
// ���O : Shutdown()
// ���e : ��Еt���p
//----------------------------------------------------------------------------------------------------
void Shutdown()
{
	gm.Release();
}
