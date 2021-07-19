#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: usart.h

* 内容简述：英文学习机Ver1.0.0.20210516_a的串口2（TX:PA2,RX:PA3)，串口3(TX:PB10,RX:PB11)的定义

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-17 陈楷闻 创建该文件

*/	 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口3接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef UART2_Handler; //UART2句柄
extern UART_HandleTypeDef UART3_Handler; //UART3句柄

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);


#endif
