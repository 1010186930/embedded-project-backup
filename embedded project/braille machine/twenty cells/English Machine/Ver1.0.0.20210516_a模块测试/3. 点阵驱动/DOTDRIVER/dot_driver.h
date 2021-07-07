#ifndef _DOT_DRIVER_H
#define _DOT_DRIVER_H
#include "sys.h"
#include "dot_processor.h"
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: dot_driver.h

* 内容简述：英文学习机Ver1.0.0.20210516_a的CSK4002语音合成芯片点阵驱动中锁存器74HC573D0~D7，LE0~LE14的宏定义

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-18 陈楷闻 创建该文件

*/

/* 锁存器74HC573的数据输入D0~D7 */
 
#define D0		PIout(2)
#define D1		PIout(1)
#define D2		PIout(0)
#define D3		PHout(15)
#define D4		PHout(14)
#define D5		PHout(13)
#define D6		PCout(9)
#define D7		PCout(8)

/* 锁存器74HC573的Latch LE0~LE14 */

#define LE0		PIout(7)
#define LE1		PIout(5)
#define LE2		PJout(14)
#define LE3		PJout(12)
#define LE4		PJout(13)
#define LE5		PDout(7)
#define LE6		PDout(3)
#define LE7		PGout(7)
#define LE8		PGout(3)
#define LE9		PJout(7)
#define LE10	PBout(13)
#define LE11	PBout(15)
#define LE12	PHout(12)
#define LE13	PHout(10)
#define LE14	PHout(6)

/* Control Command */
typedef enum {
	ALLDOWN=	0X00,
	ALLUP=		0X01
} Control_Command;

/* 点阵控制函数 */
extern unsigned char InputBraille_IO[LATCH_NUM];
void DOT_Init(void);
void Disable_Latch(void);
void Enable_Latch(void);
void Reset_DataPort(void);
void Set_DataPort(void);
void Dot_Control(Control_Command command);
void Braille_Display(unsigned char* InputBraille,unsigned char InputBraille_Length,unsigned char InputToIO[LATCH_NUM]);
void Cell_Control(unsigned char num);
#endif
