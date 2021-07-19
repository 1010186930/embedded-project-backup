#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "text.h"


int main(void)
{
	  volatile u32 j=0xFFC7FFAD;
    volatile u8 i=(u8)j;
	 
	
    Stm32_Clock_Init(9);		//系统时钟设置
    uart_init(72,115200);		//串口初始化为115200
    delay_init(72);	   	 		//延时初始化
    usmart_dev.init(72);		//初始化USMART
    LED_Init();		  			//初始化与LED连接的硬件接口
    KEY_Init();					//初始化按键
    LCD_Init();			   		//初始化LCD
    W25QXX_Init();				//初始化W25Q128
    font_init();
    LCD_Clear(YELLOW);		   	//清屏


    for(i=0; i<5; i++)
    {
        LCD_Draw_Ellipse(400,240,300+i,200+i,BLACK);
    }
    POINT_COLOR=PINK;
    for(i=0; i<10; i++)
    {
        LCD_DrawRectangle(320+i,270+i,480-i,350-i);
    }
    POINT_COLOR=BLUE;
		BACK_COLOR=RED;
		
    Show_Str(300,208,50,32,"我",32,1);
//    POINT_COLOR=BLACK;
//    Show_Str(348,224,50,16,"学习",16,1);


    while(1)
    {
        LED0=!LED0;
        delay_ms(100);
    }
}










