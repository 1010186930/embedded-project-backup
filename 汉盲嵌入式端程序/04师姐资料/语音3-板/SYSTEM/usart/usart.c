#include "sys.h"
#include "usart.h"	  

#include "delay.h"

#include <string.h>	


////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃucos,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼ş¼´¿É.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos Ê¹ÓÃ	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//±ê×¼¿âĞèÒªµÄÖ§³Öº¯Êı                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
_sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êı 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


 
#if EN_USART1_RX   //Èç¹ûÊ¹ÄÜÁË½ÓÊÕ
//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
//×¢Òâ,¶ÁÈ¡USARTx->SRÄÜ±ÜÃâÄªÃûÆäÃîµÄ´íÎó   	
u8 USART_RX_BUF[USART_REC_LEN];     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
//½ÓÊÕ×´Ì¬
//bit15£¬	½ÓÊÕÍê³É±êÖ¾
//bit14£¬	½ÓÊÕµ½0x0d
//bit13~0£¬	½ÓÊÕµ½µÄÓĞĞ§×Ö½ÚÊıÄ¿
u16 USART_RX_STA=0;       //½ÓÊÕ×´Ì¬±ê¼Ç	  
  
void uart_init(u32 bound){
  //GPIO¶Ë¿ÚÉèÖÃ
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
		USART_DeInit(USART1);  //¸´Î»´®¿Ú1
	USART_DeInit(USART2);  //¸´Î»´®¿Ú2
	USART_DeInit(USART3);  //¸´Î»´®¿Ú2
	
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
  GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.9
   
  //USART1_RX	  GPIOA.10³õÊ¼»¯
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
  GPIO_Init(GPIOA, &GPIO_InitStructure);//³õÊ¼»¯GPIOA.10  
	
	      //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

    //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOB, &GPIO_InitStructure); //³õÊ¼»¯PB10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //³õÊ¼»¯PB11
	

  //Usart1 NVIC ÅäÖÃ
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯VIC¼Ä´æÆ÷
	
	    //Usart2 NVIC ÅäÖÃ

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾İNVIC_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷USART1
   //Usart3 NVIC ÅäÖÃ
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯VIC¼Ä´æÆ
  
   //USART ³õÊ¼»¯ÉèÖÃ

	USART_InitStructure.USART_BaudRate = bound;//´®¿Ú²¨ÌØÂÊ£¬Ò»°ãÉèÖÃÎª9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½

  USART_Init(USART1, &USART_InitStructure); //³õÊ¼»¯´®¿Ú1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//¿ªÆô´®¿Ú½ÓÊÜÖĞ¶Ï
  USART_Cmd(USART1, ENABLE);                    //Ê¹ÄÜ´®¿Ú1 

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//¿ªÆôÖĞ¶Ï2   
    USART_Cmd(USART2, ENABLE);                    //Ê¹ÄÜ´®¿Ú 

	USART_Init(USART3, &USART_InitStructure); //³õÊ¼»¯´®¿Ú3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//¿ªÆôÖĞ¶Ï3
    USART_Cmd(USART3, ENABLE);  

}
 
 //´®¿Ú1·¢ËÍµ¥¸ö×Ö½Ú 
void UART1_SendByte(u16 Data)
{ 
   EXTI->IMR = 0x00000000;
   while (!(USART1->SR & USART_FLAG_TXE));
   USART1->DR = (Data & (uint16_t)0x01FF);	 
   EXTI->IMR = 0xfffff;
}
//´®¿Ú2·¢ËÍµ¥¸ö×Ö½Ú 
void UART2_SendByte(u16 Data)
{ 
   EXTI->IMR = 0x00000000;
   while (!(USART2->SR & USART_FLAG_TXE));
   USART2->DR = (Data & (uint16_t)0x01FF);	 
   EXTI->IMR = 0xfffff;   
}
//´®¿Ú3·¢ËÍµ¥¸ö×Ö½Ú 
void UART3_SendByte(u16 Data)
{ 
   EXTI->IMR = 0x00000000;
   while (!(USART3->SR & USART_FLAG_TXE));
   USART3->DR = (Data & (uint16_t)0x01FF);	 
   EXTI->IMR = 0xfffff;   
}
//´®¿Ú1·¢ËÍ×Ö·û´® 
void UART1Write(char* data)
{
	u16 i,len;
	len=strlen((char*)data);
//	DIR485_H ;
	for (i=0; i<len; i++){
		UART1_SendByte(data[i]);
	}		
}
//´®¿Ú2·¢ËÍ×Ö·û´®
void UART2Write(char* data)
{
	u16 i,len;
	len=strlen((char*)data);

	for (i=0; i<len; i++){
		UART2_SendByte(data[i]);
	}	
}
//´®¿Ú3·¢ËÍ×Ö·û´®
void UART3Write(char* data)
{
	u16 i,len;
	len=strlen((char*)data);

	for (i=0; i<len; i++){
		UART3_SendByte(data[i]);
	}	
}	
//´®¿Ú1·¢ËÍHEX
void UART1Write_Hex(char* data, int len)
{
	u16 i,len1;
	len1=len;

	for (i=0; i<len1; i++){
		UART1_SendByte(data[i]);
	}	
}
//´®¿Ú2·¢ËÍHEX
void UART2Write_Hex(char* data, int len)
{
	u16 i,len1;
	len1=len;

	for (i=0; i<len1; i++){
		UART2_SendByte(data[i]);
	}	
}


void USART1_IRQHandler(void)                	//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//Èç¹ûSYSTEM_SUPPORT_OSÎªÕæ£¬ÔòĞèÒªÖ§³ÖOS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //½ÓÊÕÖĞ¶Ï(½ÓÊÕµ½µÄÊı¾İ±ØĞëÊÇ0x0d 0x0a½áÎ²)
		{
		Res =USART_ReceiveData(USART1);	//¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ
		
		if((USART_RX_STA&0x8000)==0)//½ÓÊÕÎ´Íê³É
			{
			if(USART_RX_STA&0x4000)//½ÓÊÕµ½ÁË0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//½ÓÊÕ´íÎó,ÖØĞÂ¿ªÊ¼
				else USART_RX_STA|=0x8000;	//½ÓÊÕÍê³ÉÁË 
				}
			else //»¹Ã»ÊÕµ½0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//½ÓÊÕÊı¾İ´íÎó,ÖØĞÂ¿ªÊ¼½ÓÊÕ	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//Èç¹ûSYSTEM_SUPPORT_OSÎªÕæ£¬ÔòĞèÒªÖ§³ÖOS.
	OSIntExit();  											 
#endif
} 
	
unsigned int tmpdata;
void USART3_IRQHandler(void)                	//´®¿Ú3ÖĞ¶Ï·şÎñ³ÌĞò¡¢
//void USART2_IRQHandler(void)  
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //½ÓÊÕÖĞ¶Ï
		{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		tmpdata=USART_ReceiveData(USART3);//(USART3->DR);	//¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ
		}
}//       1£¬´®¿ÚÖĞ¶Ï³ÌĞò¸ÄÒ»ÏÂ    2£¬´®¿Ú·¢ËÍ½áÊøºóÒªÓĞÒ»¸öÅĞ¶Ï·¢ËÍ½áÊøµÄ±êÖ¾·û   
// 3£¬Ğ¾Æ¬ÖĞÓĞÃ»ÓĞ×Ô´øµÄÅĞ¶Ï·¢ËÍÍê³ÉµÄÖ¡ÃüÁî£¬ÔõÃ´¼Ó½øÈ¥£»»òÕßÊÇÔÚSpeechÖĞ¼ÓÈë´®¿Ú·¢ËÍÍê³ÉµÄ±êÖ¾
//¼´ÊÇ¿¼ÂÇÒ»Ö¡·¢ËÍÍê³Éºó²ÅÄÜ·¢ËÍÏÂÒ»Ö¡

#endif	

