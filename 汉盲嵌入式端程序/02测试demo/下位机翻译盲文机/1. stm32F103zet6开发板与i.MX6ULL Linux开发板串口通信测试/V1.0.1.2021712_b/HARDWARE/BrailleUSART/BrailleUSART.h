#ifndef __BRAILLEUSART_H
#define __BRAILLEUSART_H
#include "sys.h"

#endif

#define RX_MAXSIZE 126
#define TX_MAXSIZE 69
	  		  	
extern u8 USART2_RX_BUFF[RX_MAXSIZE]; 		//接收缓冲,最大64个字节
extern u8 USART2_TX_BUFF[TX_MAXSIZE]; 		//发送缓冲,最大64个字节
extern u8 USART2_RX_CNT;   			//接收到的数据长度
extern u8 USART2_TX_CNT;   			//发送的数据长度
extern u8 USART2_FrameFlag;			//帧结束标记
void BraillieUSART_Init(u32 bound);
