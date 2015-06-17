#ifndef _JULIUS_H_
#define _JULIUS_H_

#include <MyJuliusLib.h>

extern char voiceStr[128];
extern void* hMutex;
extern bool juliusFinishFlag;

bool JuliusInit();
void JuliusLoopFunc(LPVOID pParam);
void JuliusClose();

#endif