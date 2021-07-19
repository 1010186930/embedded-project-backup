#include "sys.h"
#include "usart.h"	
#include "BrailleUSART.h"
//接收缓存区
u8 USART2_RX_BUFF[RX_MAXSIZE]="";  	//接收缓冲,最大126个字节.
//发送缓存区
u8 USART2_TX_BUFF[TX_MAXSIZE]="";	//发送缓冲,最大69个字节.
//接收到的数据长度
u8 USART2_RX_CNT=0;
//发送的数据长度
u8 USART2_TX_CNT=0;

void BraillieUSART_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//PA2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位

    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(USART2, &USART_InitStructure);  //初始化串口
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART2, ENABLE);                    //使能串口2
}

void USART2_IRQHandler(void)
{
    u8 res;
	u8 err;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{
		if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//检测到噪音、帧错误或校验错误
		else err=0;
		res=USART_ReceiveData(USART2); //读接收到的字节，同时相关标志自动清除

		if((USART2_RX_CNT<RX_MAXSIZE)&&(err==0))
		{
				USART2_RX_BUFF[USART2_RX_CNT]=res;
				USART2_RX_CNT++;

				TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除定时器溢出中断
				TIM_SetCounter(TIM3,0);//当接收到一个新的字节，将定时器3复位为0，重新计时（相当于喂狗）
				TIM_Cmd(TIM3,ENABLE);//开始计时
		}
	}
}
