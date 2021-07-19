#ifndef __SUOCUN_H
#define __SUOCUN_H
#include "stm32f10x_gpio.h"
#include "sys.h"
extern  u16  emptmp[15],fultmp[15];
void SUOCUN_init(void);
void DX_ShowBraille(char tbtmp[][13]);
void DXdis(u16 dx[15]);
void dis(char b);
#endif
