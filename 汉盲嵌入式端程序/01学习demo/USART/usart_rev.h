/*
***************************************************************************************************
* 文件名  ：USART_REV.h
* 驱动目标：串口接收发送任务
* 测试状态：
* 测试平台：STM32F407  
* 备    注：未测试，仅供学习参考

* 使用说明：

* 建立时间：2019/10/08
* 修改时间：2019/10/08
* 版    本：V1.0
* 作    者：Osprey
* 公 众 号：鱼鹰谈单片机

* 修改时间           修改信息
* 2018/01/12        修改格式
******************************************************************************************************
*/
#ifndef __USART_REV_H
#define __USART_REV_H

#include "stm32f4xx_usart.h"


#define NULL    ((void*)0)
	

typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;


typedef enum {
    UART_TRANSFER_STATUS_IDLE,
    UART_TRANSFER_STATUS_PROCESSING,
}UartTransferState_Typedef;

typedef enum {
    UART_TRANSFER_ID_USART1,
    UART_TRANSFER_ID_USART2,
    UART_TRANSFER_ID_USART3,
    UART_TRANSFER_ID_USART4,
    UART_TRANSFER_ID_USART5,
    UART_TRANSFER_ID_MAX
}UartTransferID_Typedef;


typedef enum {
    UART_TRANSFER_ERROR_NO,
    UART_TRANSFER_ERROR_REV_NO_FINISHED,
    UART_TRANSFER_ERROR_NO_RESTART_DMA,
    UART_TRANSFER_ERROR_REV_OUT_SIZE,
    UART_TRANSFER_ERROR_DECODE_NULL,
    UART_TRANSFER_ERROR_BASEBUFF_NULL,
    UART_TRANSFER_ERROR_PROCESSING,
}UartTransferError_Typedef;


typedef BOOL (*UartFrameDecode)(void *pbuff,uint16_t length);
typedef void (*UartFrameRestartRev)(void);


typedef struct {
    uint32_t             TimeOut;
    uint16_t             MaxSize;
    uint8_t             *pBaseBuff;
    UartFrameDecode      Decode;
    UartFrameRestartRev  RestartRev;
}UartTransferInit_Typedef;


void UartTransfer_Init(uint32_t *p_currtime); // must be called
void UartTransfer_Regeist(UartTransferID_Typedef id, UartTransferInit_Typedef *p_uart_init);// must be called
void UartTransfer_RevFromRNE(UartTransferID_Typedef id,uint8_t byte);// must be called or UartTransfer_RevFromDMA
void UartTransfer_RevFromDMA(UartTransferID_Typedef id,uint16_t size);// must be called or UartTransfer_RevFromRNE
void UartTransfer_AgainRev(UartTransferID_Typedef id); // must be called

uint8_t UartTransfer_GetError(UartTransferID_Typedef id);
uint8_t *UartTransfer_GetBaseBuff(UartTransferID_Typedef id);
uint16_t UartTransfer_GetLength(UartTransferID_Typedef id);

UartTransferState_Typedef UartTransfer_GetState(UartTransferID_Typedef id);

#endif


