#include "cvCustom.h"

//-----------------------------------------------------------------------------------------------------------------------------
//
// �֐��FcvInitJPN(const char* name)
// �����F	const char*	name	�c ���{��\���p���C���[�h�E�B���h�E���쐬����E�B���h�E�̖��O
// �@�\�F�w�肳�ꂽ�E�B���h�E�̏�ɓ��{��\���p���C���[�h�E�B���h�E���쐬����D
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvInitJPN(const char* name)
{
	// �w�薼�̃E�B���h�E���݊m�F
	if(!cvWndIsExist(name)){ return false; }

	// �n���h���擾
	HWND hWnd    = (HWND)cvGetWindowHandle(name);
	HWND hParent = GetParent(hWnd);

	// �������C���[�h�E�B���h�E���݊m�F
	if(GetWindow(hParent, GW_ENABLEDPOPUP)){ return false; }

	static int checkWC = 0; // �E�B���h�E�N���X�o�^�t���O
	
	if(!checkWC){
		// �E�B���h�E�N���X��`
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = DefWindowProc;
		wc.cbClsExtra    = wc.cbWndExtra = 0;
		wc.hInstance     = GetModuleHandle(NULL);
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = CreateSolidBrush(TRANSPARENT_COLOR);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = _T("LayeredWindow class");

		// �E�B���h�E�N���X�o�^
		if(!RegisterClass(&wc)){ return false; }

		checkWC = 1;
	}

	// ���C���[�h�E�B���h�E�쐬
	RECT wrect, crect;
	GetWindowRect(hWnd, &wrect);
	GetClientRect(hParent, &crect);
	HWND hWndLayered = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, _T("LayeredWindow class"), _T(""), WS_VISIBLE | WS_POPUP, wrect.left, wrect.top, crect.right, crect.bottom, hParent, NULL, NULL, NULL);
	SetLayeredWindowAttributes(hWndLayered, TRANSPARENT_COLOR, 0, LWA_COLORKEY);

	// �v���V�[�W�������ւ� 
	SetProp(hParent, _T("CV_DEF_PROC"), (HANDLE)GetWindowLong(hParent, GWL_WNDPROC)); 
	SetWindowLong(hParent, GWL_WNDPROC, (LONG)CV_JPNProc);

	return true; 
}

//-----------------------------------------------------------------------------------------------------------------------------
//
// �֐��FCV_JPNProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
// �����F�ʏ�̃E�B���h�E�v���V�[�W���Ɠ���
// �@�\�FOpenCV�̃E�B���h�E�v���V�[�W���̑O��CV_JPNProc�v���V�[�W���Ń��b�Z�[�W����������D
//
//-----------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK CV_JPNProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	switch(msg){
		case WM_MOVE:
			RECT crect;
			GetClientRect(hWnd, &crect);
			MoveWindow(GetWindow(hWnd, GW_ENABLEDPOPUP), (lParam & 0xFFFF), ((lParam >> 16) & 0xFFFF), crect.right, crect.bottom, true);

		default:
			WNDPROC DefWP = (WNDPROC)GetProp(hWnd, _T("CV_DEF_PROC"));
			return CallWindowProc(DefWP, hWnd, msg, wParam, lParam);
	}
}


//-----------------------------------------------------------------------------------------------------------------------------
//
// �֐��FcvInitFontJPN(CvFontJPN* font, const char* font_name, int hscale, int vscale, int shear, int thickness, int flag)
// �����F	CvFontJP*	font		�c �t�H���g�\����(CvFontJP�̎��Ԃ�HFONT�^)
// 			const char*	font_name	�c �t�H���g��(�w�薳���Ńf�t�H���g)
// 			int			hscale		�c ����(0�w��Ńf�t�H���g)
// 			int			vscale		�c ��(0�w��ō����ɍ��������Ɏ����ݒ肳���)
// 			double		shear		�c ������̊p�x(shear���̊p�x�ŕ����񂪕\�������)
// 			int			thickness	�c �����̑���(TEXT_DEFO�w��Ńf�t�H���g,TEXT_BOLD�w��ő���)
// 			int			flag		�c �g���e�L�X�g�X�^�C��(TEXT_ITALICTYPE�ŃC�^���b�N��,TEXT_UNDERBAR�ŉ���,TEXT_STRIKEBAR�őł�������) 
// �@�\�F�\���ɗ��p����t�H���g��ݒ肷��D
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvInitFontJPN(CvFontJPN* font, const char* font_name, int hscale = 0, int vscale = 0, int shear = 0, int thickness = 0, int flag = 0)
{
	// �t�H���g�\���̑��݊m�F
	if(!font){ return false; }

	// �p�x�Z�o
	//shear = shear * 10;
	shear = 0; /* �p�x�ݒ薢���� */

	// �g���e�L�X�g�X�^�C������
	bool italictype	= false;
	bool underbar	= false;
	bool strikebar  = false;
	
	if(flag){
		if(flag & TEXT_ITALICTYPE){
			italictype = true;
		}
		if(flag & TEXT_UNDERBAR){
			underbar = true;
		}
		if(flag & TEXT_STRIKEBAR){
			strikebar = true;
		}
	}

	// �_���t�H���g�쐬
	*font = CreateFont(-hscale, -vscale, shear, shear, thickness, italictype, underbar, strikebar, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, font_name);

	return true;
}


//-----------------------------------------------------------------------------------------------------------------------------
//
// �֐��FcvPutTextJPN(const char* name, const char* text, CvPoint org, CvFontJPN* font, CvScalar color)
// �����F	const char*	name	�c �������\������E�B���h�E��
// 			const char*	text	�c �\�����镶����
// 			CvPoint		org		�c ������̕\���ʒu(CvPoint(x, y)�Ŏw��)
// 			CvFontJPN*	font	�c �t�H���g�n���h��
// 			CvScalar	color	�c ������̐F(CV_RGB(r, g, b)�Ŏw��)
// �@�\�F�w�肳�ꂽ�������\������D
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvPutTextJPN(const char* name,const char* text, CvPoint org, CvFontJPN* font, CvScalar color)
{
	// �w�薼�̃E�B���h�E���݊m�F
	if(!cvWndIsExist(name)){ return false; }

	// �n���h���擾
	HWND hWndLayered = GetWindow(GetParent((HWND)cvGetWindowHandle(name)), GW_ENABLEDPOPUP);
	HDC hdc = GetDC(hWndLayered);

	// �e��ݒ�
	SelectObject(hdc, *font);
	SetTextColor(hdc, RGB(color.val[2], color.val[1], color.val[0]));
	SetBkColor(hdc, TRANSPARENT_COLOR);
	
	// ������\��
	RECT trect = {org.x + GetSystemMetrics(SM_CXFRAME), org.y + GetSystemMetrics(SM_CYFRAME), 0, 0};
	DrawText(hdc, text, -1, &trect, DT_CALCRECT);
	DrawText(hdc, text, -1, &trect, DT_WORDBREAK);

	// ������\���̈�ĕ`��
	ReleaseDC(hWndLayered, hdc);
	ValidateRect(hWndLayered, &trect);

	return true;
}


//-----------------------------------------------------------------------------------------------------------------------------
// 
// �֐��FcvDestroyJPN(const char* name)
// �����F	const char*	name	�c ���{��\���p���C���[�h�E�B���h�E���폜����E�B���h�E�̖��O
// �@�\�F�w�肳�ꂽ�E�B���h�E�̏�ɂ��郌�C���[�h�E�B���h�E���폜����D
// 
//-----------------------------------------------------------------------------------------------------------------------------
bool cvDestroyJPN(const char* name)
{
	// �w�薼�̃E�B���h�E���݊m�F
	if(!cvWndIsExist(name)){ return false; }

	// ���C���[�h�E�B���h�E�폜
	if(!DestroyWindow(GetWindow(GetParent((HWND)cvGetWindowHandle(name)), GW_ENABLEDPOPUP))){ return false; }

	return true;
}