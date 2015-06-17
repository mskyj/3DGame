#include "cvCustom.h"

//-----------------------------------------------------------------------------------------------------------------------------
//
// 関数：cvInitJPN(const char* name)
// 引数：	const char*	name	… 日本語表示用レイヤードウィンドウを作成するウィンドウの名前
// 機能：指定されたウィンドウの上に日本語表示用レイヤードウィンドウを作成する．
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvInitJPN(const char* name)
{
	// 指定名称ウィンドウ存在確認
	if(!cvWndIsExist(name)){ return false; }

	// ハンドル取得
	HWND hWnd    = (HWND)cvGetWindowHandle(name);
	HWND hParent = GetParent(hWnd);

	// 既存レイヤードウィンドウ存在確認
	if(GetWindow(hParent, GW_ENABLEDPOPUP)){ return false; }

	static int checkWC = 0; // ウィンドウクラス登録フラグ
	
	if(!checkWC){
		// ウィンドウクラス定義
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

		// ウィンドウクラス登録
		if(!RegisterClass(&wc)){ return false; }

		checkWC = 1;
	}

	// レイヤードウィンドウ作成
	RECT wrect, crect;
	GetWindowRect(hWnd, &wrect);
	GetClientRect(hParent, &crect);
	HWND hWndLayered = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, _T("LayeredWindow class"), _T(""), WS_VISIBLE | WS_POPUP, wrect.left, wrect.top, crect.right, crect.bottom, hParent, NULL, NULL, NULL);
	SetLayeredWindowAttributes(hWndLayered, TRANSPARENT_COLOR, 0, LWA_COLORKEY);

	// プロシージャ差し替え 
	SetProp(hParent, _T("CV_DEF_PROC"), (HANDLE)GetWindowLong(hParent, GWL_WNDPROC)); 
	SetWindowLong(hParent, GWL_WNDPROC, (LONG)CV_JPNProc);

	return true; 
}

//-----------------------------------------------------------------------------------------------------------------------------
//
// 関数：CV_JPNProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
// 引数：通常のウィンドウプロシージャと同じ
// 機能：OpenCVのウィンドウプロシージャの前にCV_JPNProcプロシージャでメッセージを処理する．
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
// 関数：cvInitFontJPN(CvFontJPN* font, const char* font_name, int hscale, int vscale, int shear, int thickness, int flag)
// 引数：	CvFontJP*	font		… フォント構造体(CvFontJPの実態はHFONT型)
// 			const char*	font_name	… フォント名(指定無しでデフォルト)
// 			int			hscale		… 高さ(0指定でデフォルト)
// 			int			vscale		… 幅(0指定で高さに合った幅に自動設定される)
// 			double		shear		… 文字列の角度(shear°の角度で文字列が表示される)
// 			int			thickness	… 文字の太さ(TEXT_DEFO指定でデフォルト,TEXT_BOLD指定で太字)
// 			int			flag		… 拡張テキストスタイル(TEXT_ITALICTYPEでイタリック体,TEXT_UNDERBARで下線,TEXT_STRIKEBARで打ち消し線) 
// 機能：表示に利用するフォントを設定する．
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvInitFontJPN(CvFontJPN* font, const char* font_name, int hscale = 0, int vscale = 0, int shear = 0, int thickness = 0, int flag = 0)
{
	// フォント構造体存在確認
	if(!font){ return false; }

	// 角度算出
	//shear = shear * 10;
	shear = 0; /* 角度設定未実装 */

	// 拡張テキストスタイル判定
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

	// 論理フォント作成
	*font = CreateFont(-hscale, -vscale, shear, shear, thickness, italictype, underbar, strikebar, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, font_name);

	return true;
}


//-----------------------------------------------------------------------------------------------------------------------------
//
// 関数：cvPutTextJPN(const char* name, const char* text, CvPoint org, CvFontJPN* font, CvScalar color)
// 引数：	const char*	name	… 文字列を表示するウィンドウ名
// 			const char*	text	… 表示する文字列
// 			CvPoint		org		… 文字列の表示位置(CvPoint(x, y)で指定)
// 			CvFontJPN*	font	… フォントハンドル
// 			CvScalar	color	… 文字列の色(CV_RGB(r, g, b)で指定)
// 機能：指定された文字列を表示する．
//
//-----------------------------------------------------------------------------------------------------------------------------
bool cvPutTextJPN(const char* name,const char* text, CvPoint org, CvFontJPN* font, CvScalar color)
{
	// 指定名称ウィンドウ存在確認
	if(!cvWndIsExist(name)){ return false; }

	// ハンドル取得
	HWND hWndLayered = GetWindow(GetParent((HWND)cvGetWindowHandle(name)), GW_ENABLEDPOPUP);
	HDC hdc = GetDC(hWndLayered);

	// 各種設定
	SelectObject(hdc, *font);
	SetTextColor(hdc, RGB(color.val[2], color.val[1], color.val[0]));
	SetBkColor(hdc, TRANSPARENT_COLOR);
	
	// 文字列表示
	RECT trect = {org.x + GetSystemMetrics(SM_CXFRAME), org.y + GetSystemMetrics(SM_CYFRAME), 0, 0};
	DrawText(hdc, text, -1, &trect, DT_CALCRECT);
	DrawText(hdc, text, -1, &trect, DT_WORDBREAK);

	// 文字列表示領域再描画
	ReleaseDC(hWndLayered, hdc);
	ValidateRect(hWndLayered, &trect);

	return true;
}


//-----------------------------------------------------------------------------------------------------------------------------
// 
// 関数：cvDestroyJPN(const char* name)
// 引数：	const char*	name	… 日本語表示用レイヤードウィンドウを削除するウィンドウの名前
// 機能：指定されたウィンドウの上にあるレイヤードウィンドウを削除する．
// 
//-----------------------------------------------------------------------------------------------------------------------------
bool cvDestroyJPN(const char* name)
{
	// 指定名称ウィンドウ存在確認
	if(!cvWndIsExist(name)){ return false; }

	// レイヤードウィンドウ削除
	if(!DestroyWindow(GetWindow(GetParent((HWND)cvGetWindowHandle(name)), GW_ENABLEDPOPUP))){ return false; }

	return true;
}