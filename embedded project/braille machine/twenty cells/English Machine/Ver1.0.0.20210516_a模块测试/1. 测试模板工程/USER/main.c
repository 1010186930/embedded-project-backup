#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "exti.h"
#include "dot_driver.h"
#include "key.h"
#include "string.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板 实验1
 跑马灯实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/




/*下面主函数是使用HAL库函数实现控制IO口输出*/
 
int main(void)
{
	int i=0;
	unsigned char InputBraille[121]="101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110";
	unsigned char InputToIO[15]={0x00};
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
	DOT_Init();
	EXTI_Init();
//	KEY_Init();
	Dot_Control(ALLDOWN);
//	Braille_Display(InputBraille,120,InputToIO);


	
    while(1)
    {
//        key=KEY_Scan(0);
//		if(key==KEY_NEXT_PRESS)
//			Dot_Control(ALLDOWN);
//		else if(key==KEY_PREVIOUS_PRESS)
			Dot_Control(ALLUP);
    }
}
 

 
/*下面主函数使用位带操作实现：*/
/*
int main(void)
{ 
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED    while(1)
	{
         LED0=0;			     //LED0亮
	     LED1=1;				 //LED1灭
		 delay_ms(500);
		 LED0=1;				//LED0灭
		 LED1=0;				//LED1亮
		 delay_ms(500);
	 }
}*/




/*
下面主函数使用直接操作结存器方式实现跑马灯
*/
/*
int main(void)
{ 
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED  
	while(1)
	{
      GPIOB->BSRR=GPIO_PIN_1;     //LED0亮
	  GPIOB->BSRR=GPIO_PIN_0<<16; //LED1灭
	  delay_ms(500);
      GPIOB->BSRR=GPIO_PIN_1<<16; //LED0灭
	  GPIOB->BSRR=GPIO_PIN_0;     //LED1亮
	  delay_ms(500);
	 }
 }	
*/




