#ifndef __TOUNI_H__
#define __TOUNI_H__
#include "sys.h"
#define Unknown -1
#define GBK 1
#define UTF_8 2
#define ASCII 3
unsigned int BytesOneUTF_Required(unsigned char str);
unsigned int UTF8toUni(const unsigned char* pInput, unsigned short* Unic, unsigned int utfbytes);
int StringToUni(unsigned char* Inputbuf, unsigned short* Outputbuf, u8 len);
int UTF8_to_Unicode(unsigned char* utf_buf, unsigned short* uni_buf, unsigned int len);
#endif
