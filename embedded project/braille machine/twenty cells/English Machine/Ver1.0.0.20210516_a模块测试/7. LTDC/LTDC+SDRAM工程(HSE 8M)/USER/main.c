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



int main(void)
{
	u8 x=0;
  	u8 lcd_id[12];
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
	LCD_Init();                     //LCD初始化
	TIM3_Init(10000-1,9000-1);       //定时器3初始化，定时器时钟为90M，分频系数为9000-1，
                                    //所以定时器3的频率为90M/9000=10K，自动重装载为5000-1，那么定时器周期就是500ms
	POINT_COLOR=RED; 
	BACK_COLOR=YELLOW;
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。	
	while(1)
	{
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(10,40,260,32,32,"Apollo STM32F4/F7"); 	
		LCD_ShowString(10,80,240,24,24,"LTDC TEST");
		LCD_ShowString(10,110,240,16,16,"ATOM@ALIENTEK");
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2016/1/6");	
		x++;
		if(x==12)x=0;  
		delay_ms(1000);			
//		if(flag==1)
//		{
//			HAL_TIM_Base_Stop_IT(&TIM3_Handler);
//			printf("123\r\n");
//			flag=0;
//			HAL_TIM_Base_Start_IT(&TIM3_Handler);
//			
//		}
//		
//		delay_ms(1000);   
		
	}
}
