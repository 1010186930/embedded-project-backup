#include "string.h"
#include "csk4002.h"

/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: CSK4002.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的CSK4002语音合成的功能实现

* 实现了两个函数：开始合成和状态查询

* 通过串口3将待合成的文本发送到CSK4002，串口2接收CSK4002的返回值并发送给上位机（在usart.c的串口3的中断服务函数实现）

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-17 陈楷闻 创建该文件

* 1.0.0.20210516_a 2021-08-20 陈楷闻 添加复位函数和初始化函数


*/	 


u8 CSK4002_STA=UNKNOWN_STA;



/** 
  * @name   CSK4002_DeInit
  *
  * @brief  CSK4002的复位函数，高电平复位，由PNP三极管控制
  *
  * @param  无
  *
  * @retval 无
  */
void CSK4002_DeInit()
{
	HAL_GPIO_WritePin(CSK4002_RST_PORT, CSK4002_RST_PIN, GPIO_PIN_RESET);
	delay_ms(10);
	HAL_GPIO_WritePin(CSK4002_RST_PORT, CSK4002_RST_PIN, GPIO_PIN_SET);
}

/** 
  * @name   CSK4002_Init
  *
  * @brief  CSK4002的初始化函数，配置串口3，波特率固定为115200；配置复位引脚PI4
  *
  * @param  无
  *
  * @retval 无
  */
void CSK4002_Init()
{
	GPIO_InitTypeDef GPIO_Initure;
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=BOUND_RATE;				    //波特率
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()会使能UART3
	CSK4002_RST_CLK_ENABLE;
	GPIO_Initure.Pin=CSK4002_RST_PIN;			//PI4
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
	HAL_GPIO_Init(CSK4002_RST_PORT,&GPIO_Initure);	   	//初始化PI4
	CSK4002_DeInit();
}
/** 
  * @name   CSK4002_Start
  *
  * @brief  CSK4002的开始合成函数
  *
  * @param  command：CSK4002的下传命令帧 [控制命令字]
  *	@param  page：CSK4002开始命令中的代码页 [PAGE]
  *	@param  Inputbuf：CSK4002待合成文本的输入缓冲，最多200-5字节
  
  * @retval 无
  */
void CSK4002_Start(CONTROL_COMMAND command, PAGE page,u8* Inputbuf)
{
	u8 TransmitText[USART_REC_LEN] = {0x00};
	u8 Inputbuf_length = strlen((const char*)Inputbuf);
	if(Inputbuf_length + 5 > USART_REC_LEN)
		return;
	TransmitText[0] = FLAG;
	TransmitText[1] = 0x00;
	TransmitText[2] = Inputbuf_length + 2;
	TransmitText[3] = command;
	TransmitText[4] = page;
	memcpy(&TransmitText[5], Inputbuf, Inputbuf_length);
	HAL_UART_Transmit(&UART3_Handler, (uint8_t*)TransmitText, Inputbuf_length + 5, 1000);
	while(__HAL_UART_GET_FLAG(&UART3_Handler, UART_FLAG_TC) != SET);
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
	u8 TransmitText[COMMON_FRAME_LENGTH]={0x00};
	TransmitText[0]=FLAG;
	TransmitText[1]=0x00;
	TransmitText[2]=0X01;
	TransmitText[3]=QUERY;
	HAL_UART_Transmit(&UART3_Handler,(uint8_t*)TransmitText,COMMON_FRAME_LENGTH,1000);
	while(__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_TC)!=SET);
}
