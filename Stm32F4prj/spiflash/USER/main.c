#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "w25qxx.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验27
 SPI实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//要写入到W25Q16的字符串数组
const u8 TEXT_Buffer[]={"Apollo STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 

int main(void)
{
    u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
    LCD_Init();                     //初始化LCD
    W25QXX_Init();				    //W25QXX初始化
  	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"SPI TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/1/16");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
	while(W25QXX_ReadID()!=W25Q256)								//检测不到W25Q256
	{
		LCD_ShowString(30,150,200,16,16,"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0=!LED0;		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q256 Ready!"); 
	FLASH_SIZE=32*1024*1024;	//FLASH 大小为32M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q128
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);					//显示读到的字符串
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}		   
	}		    
}
