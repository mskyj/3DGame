////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// OpenCV日本語表示機能 cvCustom.h
//
// ■バージョン
//     1.0
//
// ■ライセンス
//     本ソフトウェアはBSDライセンスのもと配布されます．
//     以下ライセンス表記です．
//
//     Copyright (c) 2010 UnaNancyOwen All rights reserved.
//
//     ソースコード形式かバイナリ形式か、変更するかしないかを問わず、以下の条件を満たす場合に限り、再
//     頒布および使用が許可されます。
//
//      •ソースコードを再頒布する場合、上記の著作権表示、本条件一覧、および下記免責条項を含めること。 
//      •バイナリ形式で再頒布する場合、頒布物に付属のドキュメント等の資料に、上記の著作権表示、本条件
//       一覧、および下記免責条項を含めること。
//      •書面による特別の許可なしに、本ソフトウェアから派生した製品の宣伝または販売促進に、UnaNancyO
//       wenの名前またはコントリビューターの名前を使用してはならない。
//
//     本ソフトウェアは、著作権者およびコントリビューターによって「現状のまま」提供されており、明示黙
//     示を問わず、商業的な使用可能性、および特定の目的に対する適合性に関する暗黙の保証も含め、またそ
//     れに限定されない、いかなる保証もありません。
//     著作権者もコントリビューターも、事由のいかんを問わず、損害発生の原因いかんを問わず、かつ責任の
//     根拠が契約であるか厳格責任であるか（過失その他の）不法行為であるかを問わず、仮にそのような損害
//     が発生する可能性を知らされていたとしても、本ソフトウェアの使用によって発生した（代替品または代
//     用サービスの調達、使用の喪失、データの喪失、利益の喪失、業務の中断も含め、またそれに限定されな
//     い）直接損害、間接損害、偶発的な損害、特別損害、懲罰的損害、または結果損害について、一切責任を
//     負わないものとします。
//
// ■動作環境(確認環境)
//     OS	 ：Microsoft Windows XP Professional(SP3) / Vista Business(SP2) / 7 Professional [各OSは32bit版]
//     IDE	 ：Microsoft Visual Studio 2008 Professional Edition SP1
//     OpenCV：OpenCV 1.0 / 1.1pre
//
// ■作者
//     UnaNancyOwen：http://kgxpx834.blog58.fc2.com/
//                   https://twitter.com/UnaNancyOwen
// 
// ■協力
//     ochaloid
//
// ■バグ報告，利用報告
//     バグ報告は，メールまたはTwitterでお願します．
//     Twitterの方がレスポンスが早いと思います．
//
//     宛先：kgxpx834@yahoo.co.jp
//     件名：OpenCV日本語表示機能のバグ報告
//     本文：・OpenCV日本語表示機能のバージョン
//           ・利用環境(OS,IDE,Compiler,OpenCVなど)
//           ・バグ内容
//                         など
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CV_CUSTOM_H_
#define _CV_CUSTOM_H_

#include <MyOpencvLib.h>
#include <Windows.h>
#include <tchar.h>

// 各種設定
#define TRANSPARENT_COLOR	RGB(1, 1, 1)				// レイヤードウィンドウ透過色
#define DELETE_COLOR		CV_RGB(1, 1, 1)				// 文字表示削除用指定色
#define CvFontJPN			HFONT						// フォント構造体
#define TEXT_DEFO			0							// 普通
#define TEXT_BOLD			700							// 太字
#define BIT(num)			((unsigned int)1 << (num))	// 拡張テキストスタイル判定ビット
#define TEXT_ITALICTYPE		BIT(0)						// イタリック体
#define TEXT_UNDERBAR		BIT(1)						// 下線
#define TEXT_STRIKEBAR		BIT(2)						// 打ち消し線


// プロトタイプ宣言
bool		cvInitJPN(const char* name);
bool		cvInitFontJPN(CvFontJPN* font, const char* font_name, int hscale, int vscale, int shear, int thickness, int flag);
bool		cvPutTextJPN(const char* name, const char* text, CvPoint org, CvFontJPN* font, CvScalar color);
bool		cvDestroyJPN(const char* name);
inline bool cvWndIsExist(const char* name);
LRESULT CALLBACK CV_JPNProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------------------------------------------------------------------------------------
//
// 関数：cvWndIsExist(const char* name)
// 引数：	const char*	name	… 存在確認するウィンドウの名前
// 機能：指定されたウィンドウが存在するかを確認する．存在する：true，存在しない：false
//
//-----------------------------------------------------------------------------------------------------------------------------
inline bool cvWndIsExist(const char* name)
{
	return (cvGetWindowHandle(name) != 0);
}

#endif // CVJPN