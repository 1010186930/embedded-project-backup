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
#include "timer.h"
#include "touni.h"
#include "stdio.h"
#include "string.h"
/************************************************
 ALIENTEK战舰STM32开发板实验40
 汉字显示 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


 int main(void)
 {	
	u16 univalue;
	u8 high;
	u8 low;
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2]="";//gbk码
	u8 key,t;   	    
//	u16 unibuf[64]={0x0000};
	//u8 utfscr[] = {0x20,0xe6,0xa0,0x87,0xe9,0xa2,0x98,0x20,0x3c,0x3c,0xe6,0x97,0xa7,0xe9,0x9b,0xa8,0xe6,0xa5,0xbc,0xc2,0xb7,0xe5,0x8f,0xa4,0xe9,0xbe,0x99,0xe3,0x80,0x8a,0xe7,0x99,0xbd,0xe7,0x8e,0x89,0xe9,0x9b,0x95,0xe9,0xbe,0x99,0xe3,0x80,0x8b,0x3e,0x3e };
//	u8 utfscr[]="浣犲ソ鎴戝彨鏋楁爧";
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
	TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数到5000为500ms  
//	 LCD_ShowString(30,70,200,16,32,"Erase Start");
//		W25QXX_Erase_Chip();
//	while(1)
//	{
//		
//		LCD_ShowString(30,70,200,16,32,"Erase OK");
//	}
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
// 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	while(font_init()) 			//检查字库
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
//	POINT_COLOR=RED;       
//	Show_GBKStr(30,176,200,32,"按KEY0,更新字库",32,0);
// 	POINT_COLOR=BLUE;  
//	Show_GBKStr(30,208,200,32,"内码高字节:",32,0);				    	 
//	Show_GBKStr(30,240,200,32,"内码低字节:",32,0);				    	 
//	Show_GBKStr(30,272,200,32,"汉字计数器:",32,0);
//	Show_GBKStr(30,304,290,32,"对应汉字(32*32)为:",32,0); 
	POINT_COLOR=RED;       
//	Show_UTF8Str(0,176,500,32,utfscr,unibuf,32,sizeof(utfscr)/sizeof(u8),0);
 	POINT_COLOR=BLUE;  
	Show_GBKStr(30,208,200,32,"内码高字节:",32,0);				    	 
	Show_GBKStr(30,240,200,32,"内码低字节:",32,0);				    	 
	Show_GBKStr(30,272,200,32,"汉字计数器:",32,0);
	Show_GBKStr(30,304,290,32,"对应汉字(32*32)为:",32,0); 		 
	while(1)
	{
		fontcnt=0;
		for(univalue=0x4e00;univalue<=0x9FFF;univalue++,fontcnt++)
		{
			high=(univalue>>8);
			low=univalue&0xFF;
			sprintf((char*)fontx,"%x",high);
			LCD_ShowString(222,208,100,32,32,fontx);
			sprintf((char*)fontx,"%x",low);
//			LCD_ShowNum(222,208,univalue>>8,3,32);		//显示内码高字节
			LCD_ShowString(222,240,100,32,32,fontx);
//			LCD_ShowNum(222,240,univalue&0xFF,3,32);	//显示内码低字节	 
			LCD_ShowNum(222,272,fontcnt,5,32);//汉字计数显示	
			Show_UNIStr(318,304,100,32,&univalue,32,0);
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
}

















