#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "BrailleUSART.h"
#include "timer.h"
#include "BrailleProtocol.h"
#include "exti.h"
/************************************************
 ALIENTEK精英STM32开发板实验4
 串口 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
u8 buffer1[121]="";
u8 dot_cnt=0;
u8 text_cnt=0;
u8 testbuf[65]={0xe4,0xb9,0x8b,0xe6,0xb1,0x9f,0xe5,0xae,0x9e,0xe9,0xaa,0x8c,0xe5,0xae,0xa4};
 int main(void)
 {		
 	  
	 u8 i=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	BraillieUSART_Init(115200);
 	LED_Init();			     //LED端口初始化
	EXTIX_Init();
	TIM3_Init(40-1,7200-1);
	 for(i=0; i<sizeof(testbuf)/sizeof(u8); i++)
		{
			if(testbuf[i]!=0x00)
				text_cnt++;
		}
 	while(1)
	{
		UnpackedDot(buffer1,&dot_cnt);
//		printf("123\r\n");
		
		delay_ms(50);
		
	}	 
 }

