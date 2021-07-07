#ifndef _DOT_PROCESSOR_H
#define _DOT_PROCESSOR_H
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: dot_processor.h

* 内容简述：英文学习机Ver1.0.0.20210516_a的20方二进制点阵数据的处理函数定义

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-19 陈楷闻 创建该文件

*/
#define BRAILLE_CELL_NUM		20
#define CELL_DOT_NUM			6
#define LATCH_NUM				15

/*盲文二进制点序处理函数*/

void Process_BrailleBinary(unsigned char* InputBraille,unsigned char InputBraille_Length,unsigned char InputToIO[LATCH_NUM]);
void Inverse_BrailleCell(unsigned char InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM+1]);
void Change_DotSequence(unsigned char InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM+1],unsigned char IO_Input[LATCH_NUM]);
unsigned char Binary_To_Dec(unsigned char* InputBinary);
unsigned char Dec_To_Bit(unsigned char dec,unsigned char bit);
#endif
