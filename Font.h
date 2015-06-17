#ifndef _FONT_H_
#define _FONT_H_

#include<Windows.h>
#include<iostream>
using namespace::std;

extern HFONT smallFont;
extern HFONT defFont;
extern HFONT largeFont;
extern HFONT largeFont2;
extern HFONT onryouFont96;

bool FontInit();
void FontRelease();

#endif