#include "BrailleProtocol.h"
#include "BrailleUSART.h"
#include "string.h"
#include "usart.h"

void SendText(u8* InputText, u8 len)
{
	u8 i=0;
	USART2_TX_CNT=0;
	__set_PRIMASK(1);
	memset(USART2_TX_BUFF,'\0',TX_MAXSIZE);
	USART2_TX_BUFF[0]=0x55;
	USART2_TX_BUFF[1]=0x02;
	USART2_TX_BUFF[2]=len+4;
	for(i = 0; i < len; ++i)
	{
		USART2_TX_BUFF[3+i]=*InputText;
		InputText++;
		USART2_TX_CNT++;
		if(USART2_TX_CNT > TX_MAXSIZE-1)
			return;
	}
	USART2_TX_BUFF[3+i] = 0xFE;

	for(i = 0; i < len+4; ++i)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
		USART_SendData(USART2,USART2_TX_BUFF[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
	__set_PRIMASK(0);
}

void SendACK(void)
{
	u8 i=0;
	USART2_TX_BUFF[0]=0x55;
	USART2_TX_BUFF[1]=0x03;
	USART2_TX_BUFF[2]=0x04;
	USART2_TX_BUFF[3]=0xFE;
	for(i = 0; i < 4; ++i)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
		USART_SendData(USART2,USART2_TX_BUFF[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}

void SendNCK(void)
{
	u8 i=0;
	USART2_TX_BUFF[0]=0x55;
	USART2_TX_BUFF[1]=0x04;
	USART2_TX_BUFF[2]=0x04;
	USART2_TX_BUFF[3]=0xFE;
	for(i = 0; i < 4; ++i)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
		USART_SendData(USART2,USART2_TX_BUFF[i]);
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}

int32_t UnpackedDot(u8* dotbuf, u8* cnt)
{
	u8 len,num,i;
	
    if(USART2_FrameFlag==1)
    {
		
		USART2_FrameFlag=0;
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
		len = USART2_RX_BUFF[2];
//		USART_SendData(USART2,len);
		
		for(i = 0; i < len ; ++i)
		{
			printf("%x\r\n",USART2_RX_BUFF[i]);
		}
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
		
        if(USART2_RX_BUFF[0]==0x55 && USART2_RX_BUFF[1]==0x01 && USART2_RX_BUFF[len - 1]==0xFE)//帧头正确功能码和帧尾正确
        {
			num = USART2_RX_BUFF[3];
			if(len > 0 && len <= 0x7D)//长度符号
			{
				*cnt=len-5;
				for(i=0; i<len-5; ++i)
				{
					*dotbuf++ = USART2_RX_BUFF[4 + i];			
				}
				*dotbuf = '\0';
				SendACK();
			}
		}
		else
			SendNCK();
	
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
		memset(USART2_RX_BUFF,'\0',RX_MAXSIZE);
		USART2_RX_CNT=0;
		return num;
	}
	return -1; 
}
//int32_t UnpackedDot(u8* dotbuf)
//{
//	u8 len,num,i,j;
//    
//		
//		

////		for(i = 0; i < len-5; ++i)
////		{
////			while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
////			USART_SendData(USART2,USART2_RX_BUFF[4 + i]);
////		}
////		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
//		
//      
//			if(USART2_FrameFlag==1)
//    {
//			
//				for(i=0; i<6; ++i)
//				{
//					*dotbuf++ = 0x45;
//					
//				}
//				*dotbuf = '\0';
//				
//			
//			}
//		
//		return num;
//	
//}
