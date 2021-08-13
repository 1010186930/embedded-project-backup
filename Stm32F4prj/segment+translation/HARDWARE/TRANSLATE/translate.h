#ifndef _TRANSLATE_H
#define _TRANSLATE_H
#include "sys.h"
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "delay.h"
#include "malloc.h"
#include "usart.h"

#define DANZI_ITEM	32
void TwoPloyphonicTranslate(unsigned short* uni, unsigned char len, char* dot, char* path);
void DanziTranslate(unsigned short* uni, unsigned char len, char* dot,char* path);
int AnalyzeTextType(const char* srcpath);
#endif