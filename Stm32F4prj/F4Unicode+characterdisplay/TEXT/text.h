#ifndef __TEXT_H__
#define __TEXT_H__	  
#include "fontupd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//������ʾ ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			//�õ����ֵĵ�����
void Show_GBKFont(u16 x,u16 y,u8 *font,u8 size,u8 mode);					//��ָ��λ����ʾһ������
void Show_GBKStr(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	//��ָ��λ����ʾһ���ַ��� 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void Get_UNIMat(u16 *code,unsigned char *mat,u8 size);
void Show_UNIFont(u16 x,u16 y,u16 *font,u8 size,u8 mode);
void Show_UNIStr(u16 x,u16 y,u16 width,u16 height,u16*str,u8 size,u8 mode);
void Show_UTF8Str(u16 x,u16 y,u16 width,u16 height,u8* utfsrc, u16* unidst,u8 size,u8 len,u8 mode);
#endif
