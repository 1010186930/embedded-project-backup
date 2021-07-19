#ifndef __StringProcess_H
#define __StringProcess_H 
#include "sys.h"
#include "string.h"
#include "tts.h"
char* BufDisplay(char* buffer,char *bufdisplay);
char* ReplaceSubStr(const char* str, const char* srcSubStr, const char* dstSubStr, char* out);
void SpecialSynthesis(char *str);
void SpecialSynthesisInBk(char * synbuf, char * srcbuf);
#endif