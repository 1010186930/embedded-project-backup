#ifndef __TEXT_H__
#define __TEXT_H__	  
#include "fontupd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板 
//汉字显示 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
void DrawBraillePattern(char* dot, int len);
void LCD_WShowBraille(int n,u16 x,u16 y,char braille[][7]);
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			//得到汉字的点阵码
void Show_GBKFont(u16 x,u16 y,u8 *font,u8 size,u8 mode);					//在指定位置显示一个汉字
void Show_GBKStr(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	//在指定位置显示一个字符串 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void Get_UNIMat(u16 *code,unsigned char *mat,u8 size);
void Show_UNIFont(u16 x,u16 y,u16 *font,u8 size,u8 mode);
void Show_UNIStr(u16 x,u16 y,u16 width,u16 height,u16*str,u8 size,u8 mode);
void Show_UTF8Str(u16 x,u16 y,u16 width,u16 height,u8* utfsrc, u16* unidst,u8 size,u8 len,u8 mode);
#endif
