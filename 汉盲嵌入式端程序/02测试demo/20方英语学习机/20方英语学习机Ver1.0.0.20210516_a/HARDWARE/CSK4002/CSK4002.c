#include "string.h"
#include "usart.h"
#include "CSK4002.h"
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: CSK4002.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的CSK4002语音合成的功能实现

* 实现了两个函数：开始合成和状态查询

* 通过串口3将待合成的文本发送到CSK4002，串口2接收CSK4002的返回值并发送给上位机（在usart.c的串口3的中断服务函数实现）

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-17 陈楷闻 创建该文件

*/
u8 CSK4002_STA=UNKNOWN_STA;

/**
  * @name   CSK4002_Start
  *
  * @brief  CSK4002的开始合成函数
  *
  * @param  command：CSK4002的下传命令帧 [控制命令字]
  *	@param  page：CSK4002开始命令中的代码页 [PAGE]
  *	@param  Inputbuf：CSK4002待合成文本的输入缓冲，最多200-2字节

  * @retval 无
  */
void CSK4002_Start(CONTROL_COMMAND command,PAGE page,u8* Inputbuf)
{
    u8 TransmitText[USART_REC_LEN]= {0x00};
    u8 Inputbuf_length=strlen((const char*)Inputbuf);
    TransmitText[0]=FLAG;
    TransmitText[1]=0x00;
    TransmitText[2]=Inputbuf_length+2;
    TransmitText[3]=command;
    TransmitText[4]=page;
    memcpy(&TransmitText[5],Inputbuf,Inputbuf_length);
    HAL_UART_Transmit(&UART3_Handler,(uint8_t*)TransmitText,Inputbuf_length+2,1000);
    while(__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_TC)!=SET);
}



/**
  * @name   CSK4002_QueryState
  *
  * @brief  CSK4002的状态查询函数
  *
  * @param  无

  * @retval 无
  */
void CSK4002_QueryState()
{
    u8 TransmitText[COMMON_FRAME_LENGTH]= {0x00};
    TransmitText[0]=FLAG;
    TransmitText[1]=0x00;
    TransmitText[2]=0X01;
    TransmitText[3]=QUERY;
    HAL_UART_Transmit(&UART3_Handler,(uint8_t*)TransmitText,COMMON_FRAME_LENGTH,1000);
    while(__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_TC)!=SET);
}
