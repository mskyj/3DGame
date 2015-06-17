#include"Font.h"

HFONT smallFont;
HFONT defFont;
HFONT largeFont;
HFONT largeFont2;
HFONT onryouFont96;

bool FontInit()
{
	if( AddFontResourceEx("data\\bokutachi.otf", FR_PRIVATE, NULL) <= 0 )
	{
		cout << "Font Error : Init() failed to read font\\bokutachi.otf" << endl;
		return false;
	}
	smallFont = CreateFont(32,0,0,0,
				FW_REGULAR,FALSE,FALSE,FALSE,
				SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY|PROOF_QUALITY,DEFAULT_PITCH|OUT_DEFAULT_PRECIS,
				"ぼくたちのゴシック");
	defFont = CreateFont(64,0,0,0,
				FW_REGULAR,FALSE,FALSE,FALSE,
				SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY|PROOF_QUALITY,DEFAULT_PITCH|OUT_DEFAULT_PRECIS,
				"ぼくたちのゴシック");
	largeFont = CreateFont(96,0,0,0,
				FW_BOLD,TRUE,FALSE,FALSE,
				SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY|PROOF_QUALITY,DEFAULT_PITCH|OUT_DEFAULT_PRECIS,
				"ぼくたちのゴシック");
	largeFont2 = CreateFont(96,0,0,0,
				FW_REGULAR,FALSE,FALSE,FALSE,
				SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY|PROOF_QUALITY,DEFAULT_PITCH|OUT_DEFAULT_PRECIS,
				"ぼくたちのゴシック");
	if( AddFontResourceEx("data\\onryou.ttf", FR_PRIVATE, NULL) <= 0 )
	{
		cout << "Font Error : Init() failed to read font\\onryou.ttf" << endl;
		return false;
	}
	onryouFont96 = CreateFont(96,0,0,0,
				FW_BOLD,FALSE,FALSE,FALSE,
				SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY|PROOF_QUALITY,DEFAULT_PITCH|OUT_DEFAULT_PRECIS,
				"怨霊");

	return true;
}

void FontRelease()
{
	DeleteObject(smallFont);
	DeleteObject(defFont);
	DeleteObject(largeFont);
	DeleteObject(largeFont2);
	DeleteObject(onryouFont96);
	RemoveFontResourceEx("data\\bokutachi.otf", FR_PRIVATE, NULL);
	RemoveFontResourceEx("data\\onryou.ttf", FR_PRIVATE, NULL);
}