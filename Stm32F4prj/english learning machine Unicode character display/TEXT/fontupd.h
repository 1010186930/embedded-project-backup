#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include "sys.h"
 	 


//������Ϣ�����ַ,ռ33���ֽ�,��1���ֽ����ڱ���ֿ��Ƿ����.����ÿ8���ֽ�һ��,�ֱ𱣴���ʼ��ַ���ļ���С														   
extern u32 FONTINFOADDR;	
//�ֿ���Ϣ�ṹ�嶨��
//���������ֿ������Ϣ����ַ����С��
__packed typedef struct 
{
	u8 fontok;				//�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
	u32 ugbkaddr; 			//unigbk�ĵ�ַ
	u32 ugbksize;			//unigbk�Ĵ�С	 
	u32 gbk_32_SONG_addr;			//gbk32�����ַ	
	u32 gbk_32_SONG_size;			//gbk32����Ĵ�С	 
	u32 uni_32_SONG_addr;			//uni32�����ַ
	u32 uni_32_SONG_size;			//uni32����Ĵ�С		 
}_font_info; 


extern _font_info ftinfo;	//�ֿ���Ϣ�ṹ��

u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos);	//��ʾ���½���
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx);	//����ָ���ֿ�
u8 update_font(u16 x,u16 y,u8 size,u8* src);			//����ȫ���ֿ�
u8 font_init(void);										//��ʼ���ֿ�
#endif




















