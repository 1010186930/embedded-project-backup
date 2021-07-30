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
#include "translate.h"
#include "utf-8tounicode.h"
#include "ff.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验42
 汉字显示实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


//signtable test[] __attribute__((at(0XC01F4000))) ={0x1234,"1234"}; 


int main(void)
{	
	int x=10;
	int y=20;
	int z;
	int unibufsize;
	char *result[1000];
	u8 buffer1[100] ={0xe4,0xb9,0x8c};
	unsigned short buffer1s[100]={0xced2,0x8033};
	u16 unibuf[100] = { 0x0000 };
	unsigned short unicode;
	
	
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];                    //gbk码
	u8 key,t;
    int cnt;
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
  	f_mount(fs[1],"1:",1);          //挂载SPI FLASH.
	
//	strcpy(test,"n");
	PrintTest();

//printf("signtable test[0]:%x,%s\r\n",sign[1].unicode,sign[1].numdot);
		

//	while(1) 		        //检查字库
//	{
		
//  UPD:    
//		LCD_Clear(WHITE);		   	//清屏
//		POINT_COLOR=RED;			//设置字体为红色	   	   	  
//		LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
//		while(SD_Init())			//检测SD卡
//		{
//		  	LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
//		  	delay_ms(200);
//		  	LCD_Fill(30,70,200+30,70+16,WHITE);
//		   	delay_ms(200);		    
//		 }								 						    
//		LCD_ShowString(30,70,200,16,16,"SD Card OK");
//		LCD_ShowString(30,90,200,16,16,"Font Updating...");
//		key=update_font(20,110,16,"0:");//更新字库
//		while(key)//更新失败		
//		{		
//			 LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
//			 delay_ms(200);
//			 LCD_Fill(20,110,200+20,110+16,WHITE);
//			 delay_ms(200);		       
//		} 		  
//		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
//		delay_ms(1500);	
//		LCD_Clear(WHITE);//清屏	       
//	} 
//	POINT_COLOR=RED;       
//	Show_Str(30,30,200,16,"阿波罗STM32F4/F7开发板",16,0);				    	 
//	Show_Str(30,50,200,16,"GBK字库测试程序",16,0);				    	 
//	Show_Str(30,70,200,16,"正点原子@ALIENTEK",16,0);				    	 
//	Show_Str(30,90,200,16,"2016年1月16日",16,0);
//	Show_Str(30,110,200,16,"按KEY0,更新字库",16,0);
//	POINT_COLOR=BLUE;  
//	Show_Str(30,130,200,16,"内码高字节:",16,0);				    	 
//	Show_Str(30,150,200,16,"内码低字节:",16,0);				    	 
//	Show_Str(30,170,200,16,"汉字计数器:",16,0);

//	Show_Str(30,200,200,32,"对应汉字为:",32,0); 
//	Show_Str(30,232,200,24,"对应汉字为:",24,0); 
//	Show_Str(30,256,200,16,"对应汉字(16*16)为:",16,0);			 
//	Show_Str(30,272,200,12,"对应汉字(12*12)为:",12,0);			 
//	while(1)
//	{
//		delay_ms(1500);	
//	z=maxx(x,y);

//	//printf("输入的GBK编码：%d\n\r",buffer1s[0]);
//	//unicode=ff_convert(buffer1s[0],1);
//	//printf("GBK转换得到的Unicode码：%d\n\r",unicode);

//	StringToUni(buffer1, unibuf,strlen((const char*)buffer1));
//	unibufsize = sizeof(unibuf) /sizeof(unibuf[0]);
//	
//  GetDot(unibuf, unibufsize);
//	} 

	
}
