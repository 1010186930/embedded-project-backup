#ifndef __TOUNI_H__
#define __TOUNI_H__
#include "sys.h"
#include "malloc.h"
#include "ff.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
#include "exfuns.h"
#define Unknown -1
#define GBK 1
#define UTF_8 2
#define ASCII 3
int Init_Uni(unsigned short* src, int unilen);
int EncodeType(unsigned char* Inputbuf);
int RequireBytes(unsigned char str);
int is_str_utf8(unsigned char* str);
int is_str_gbk(unsigned char* str);
int RequireBytes(unsigned char str);
int StrToUni(unsigned char* src, unsigned short* dst, int strtype,unsigned int len);
int GBKConvertUni(u8* gbkstr, u16* unistr,unsigned int len);
unsigned int BytesOneUTF_Required(unsigned char str);
unsigned int UTF8toUni(const unsigned char* pInput, unsigned short* Unic, unsigned int utfbytes);
int StringToUni(unsigned char* Inputbuf, unsigned short* Outputbuf, unsigned int len);
int UTF8_to_Unicode( unsigned char* utf_buf, unsigned short* uni_buf, unsigned int len);
#endif
