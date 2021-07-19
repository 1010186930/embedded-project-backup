#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "timer.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验13
 SDRAM实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
u8 flag=0;
u16 testsram[250000] __attribute__((at(0XC0000000)));//测试用数组

//SDRAM内存测试	    
void fsmc_sdram_test(u16 x,u16 y)
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//在地址0读到的数据	  				   
//  	LCD_ShowString(x,y,180,y+16,16,"Ex Memory Test:    0KB "); 
	//每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//后面读出的数据一定要比第一次读到的数据大.	   		   
//		LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*16,5,16,0);	//显示内存容量  
		
 	}	
	printf("SDRAM Capacity:%dKB\r\n",(u16)(temp-sval+1)*16);//打印SDRAM容量	
}	

int main(void)
{
	u8 key;		 
	u8 i=0;	     
	u32 ts=0; 
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(180,4,2,4);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
//    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
	TIM3_Init(10000-1,9000-1);       //定时器3初始化，定时器时钟为90M，分频系数为9000-1，
                                    //所以定时器3的频率为90M/9000=10K，自动重装载为5000-1，那么定时器周期就是500ms
	
	for(ts=0;ts<20;ts++)
	{
		testsram[ts]=ts;//预存测试数据	 
	} 	
	
	while(1)
	{
//		key=KEY_Scan(0);//不支持连按	
//		if(key==KEY_ENTER_PRESS)fsmc_sdram_test(30,170);//测试SRAM容量
//		else if(key==KEY_RETURN_PRESS)//打印预存测试数据
//		{
//			for(ts=0;ts<20;ts++)
//			{
////				LCD_ShowxNum(30,190,testsram[ts],6,16,0);//显示测试数据	
//				printf("testsram[%d]:%d\r\n",ts,testsram[ts]);
//			}
//		}
//		else ;
		if(flag==1)
		{
			HAL_TIM_Base_Stop_IT(&TIM3_Handler);
			printf("123\r\n");
			flag=0;
			HAL_TIM_Base_Start_IT(&TIM3_Handler);
			
		}
		
		delay_ms(1000);   
		
	}
}
