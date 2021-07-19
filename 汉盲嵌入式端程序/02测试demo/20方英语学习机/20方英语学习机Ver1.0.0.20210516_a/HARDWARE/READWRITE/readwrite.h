#ifndef _READWRITE_H
#define _READWRITE_H
		
#include "diskio.h"		
#include "malloc.h" 
#include "exfuns.h"
FRESULT times_Init(const u8*path);
FRESULT times_Write(const u8*path);
FRESULT times_Read(u8* path);
u8 Scan_Directory(u8* path);
u8 Get_Parentdirectory(void);
u8 Confirm_Buttom(u8 index);
void Get_Nextpage(u8 num);
void Bytes_Get(u8 num);
#endif
