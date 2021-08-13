#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "string.h"
#include "sdram.h"
#include "malloc.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "sdio_sdcard.h"
#include "fontupd.h"
#include "text.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验42
 汉字显示实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
    
int main(void)
{	
	u16 univalue[100]={0x4F60,0x597D,0x6211,0x53EB,0x6797,0x680B};
	u8 high;
	u8 low;
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];                    //gbk码
	u8 key,t;
    u16 tmptest=0;
	u16 gbktoken=0xc4cf;
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //SDRAM初始化
    LCD_Init();                     //LCD初始化
	W25QXX_Init();				    //初始化W25Q256
    my_mem_init(SRAMIN);            //初始化内部内存池
    my_mem_init(SRAMEX);            //初始化外部SDRAM内存池
    my_mem_init(SRAMCCM);           //初始化内部CCM内存池
    exfuns_init();		            //为fatfs相关变量申请内存  
    f_mount(fs[0],"0:",1);          //挂载SD卡 
  	//f_mount(fs[1],"1:",1);          //挂载SPI FLASH.  
	while(font_init()) 		        //检查字库
	{
  UPD:    
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,80,240,32,32,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,80,270,112,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,80,240,32,32,"SD Card OK");
		LCD_ShowString(30,112,256,32,32,"Font Updating...");
		key=update_font(30,144,32,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,112,200,32,32,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(30,112,230,110,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,112,200,32,32,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏       
	} 
	POINT_COLOR=RED;       
 	POINT_COLOR=BLUE;  
	Show_GBKStr(30,304,290,32,"对应汉字(32*32)为:",32,0); 	
	tmptest=ff_convert(gbktoken,1);
	printf("tmptest:%x",tmptest);	
	while(1)
	{
		
			Show_UNIStr(318,304,500,32,univalue,32,0);
			t=200;
			while(t--)//延时,同时扫描按键
			{
				delay_ms(1);
				key=KEY_Scan(0);
				if(key==KEY0_PRES)goto UPD;
			}
				LED0=!LED0;
	}
//		fontcnt=0;
//		for(i=0x81;i<0xff;i++)
//		{		
//			fontx[0]=i;
//			LCD_ShowNum(222,208,i,3,32);		//显示内码高字节    
//			for(j=0x40;j<0xfe;j++)
//			{
//				if(j==0x7f)continue;
//				fontcnt++;
//				LCD_ShowNum(222,240,j,3,32);	//显示内码低字节	 
//				LCD_ShowNum(222,272,fontcnt,5,32);//汉字计数显示	 
//			 	fontx[1]=j;
//				Show_GBKFont(318,304,fontx,32,0);	  
//				Show_GBKStr(318,340,100,32,fontx,32,0);
//				Show_GBKStr(318,380,100,32,test,32,0);
//				t=200;
//				while(t--)//延时,同时扫描按键
//				{
//					delay_ms(1);
//					key=KEY_Scan(0);
//					if(key==KEY0_PRES)goto UPD;
//				}
//				LED0=!LED0;
//			}   
//		}	
}
