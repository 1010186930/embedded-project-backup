#include "string.h"
#include "csk4002.h"

/****************************************************************************

* Copyright (C), 2021-2022,֮��ʵ����

* �ļ���: CSK4002.c

* ���ݼ�����Ӣ��ѧϰ��Ver1.0.0.20210516_a��CSK4002�����ϳɵĹ���ʵ��

* ʵ����������������ʼ�ϳɺ�״̬��ѯ

* ͨ������3�����ϳɵ��ı����͵�CSK4002������2����CSK4002�ķ���ֵ�����͸���λ������usart.c�Ĵ���3���жϷ�����ʵ�֣�

* �ļ���ʷ��

* �汾�� ���� ���� ˵��

* 1.0.0.20210516_a 2021-05-17 �¿��� �������ļ�

* 1.0.0.20210516_a 2021-08-20 �¿��� ��Ӹ�λ�����ͳ�ʼ������


*/	 


u8 CSK4002_STA=UNKNOWN_STA;



/** 
  * @name   CSK4002_DeInit
  *
  * @brief  CSK4002�ĸ�λ�������ߵ�ƽ��λ����PNP�����ܿ���
  *
  * @param  ��
  *
  * @retval ��
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
  * @brief  CSK4002�ĳ�ʼ�����������ô���3�������ʹ̶�Ϊ115200�����ø�λ����PI4
  *
  * @param  ��
  *
  * @retval ��
  */
void CSK4002_Init()
{
	GPIO_InitTypeDef GPIO_Initure;
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=BOUND_RATE;				    //������
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()��ʹ��UART3
	CSK4002_RST_CLK_ENABLE;
	GPIO_Initure.Pin=CSK4002_RST_PIN;			//PI4
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;		//�����������
	GPIO_Initure.Pull=GPIO_PULLUP;			//����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
	HAL_GPIO_Init(CSK4002_RST_PORT,&GPIO_Initure);	   	//��ʼ��PI4
	CSK4002_DeInit();
}
/** 
  * @name   CSK4002_Start
  *
  * @brief  CSK4002�Ŀ�ʼ�ϳɺ���
  *
  * @param  command��CSK4002���´�����֡ [����������]
  *	@param  page��CSK4002��ʼ�����еĴ���ҳ [PAGE]
  *	@param  Inputbuf��CSK4002���ϳ��ı������뻺�壬���200-5�ֽ�
  
  * @retval ��
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
  * @brief  CSK4002��״̬��ѯ����
  *
  * @param  ��

  * @retval ��
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
