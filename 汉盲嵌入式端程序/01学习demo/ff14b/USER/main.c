#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "lcd.h" 
#include "ltdc.h"   
#include "sdram.h"    
#include "malloc.h" 
#include "usmart.h"  
#include "w25qxx.h"  
#include "pcf8574.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"  
#include "text.h"
#include "usbh_usr.h"
#include "key.h"
#include "readwrite.h"
#include "text.h"
#include "exti.h"
#include "timer.h"
#include "fattester.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验57
 USB U盘(Host)实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
u8 path[100]="3:";

int key_v;
//用户测试主程序
//返回值:0,正常
//       1,有问题
u8 USH_User_App(void)
{  
	u32 total,free;
	u8 res=0;
	Show_Str(30,140,250,48,"设备连接成功!.",32,0);	 
  	res=f_mount(fs[3],(const TCHAR*)"3:",1); 	//重新挂载U盘
	
	printf("f_mount res:%d\r\n",res);
	res=exf_getfree("3:",&total,&free);
	mf_getlabel("3:");
	res=mf_scan_files("3:");
	printf("mf_scan_files res:%d\r\n",res);
	if(res==0)
	{
		POINT_COLOR=BLUE;//设置字体为蓝色	   
		LCD_ShowString(30,190,250,48,32,"FATFS OK!");	
		LCD_ShowString(30,240,400,48,32,"U Disk Total Size:     MB");	 
		LCD_ShowString(30,290,400,48,32,"U Disk  Free Size:     MB"); 	    
		LCD_ShowNum(320,240,total>>10,5,32);//显示U盘总容量 MB
		LCD_ShowNum(320,290,free>>10,5,32);	
		
	} 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
	{	
		LED1=!LED1;
		delay_ms(500);
	}
	LED1=1;				//关闭LED1 
 	f_mount(0,(const TCHAR*)"3:",1); 	//卸载U盘
	printf("dis f_mount\r\n");
	POINT_COLOR=RED;//设置字体为红色	   
	Show_Str(30,140,250,48,"设备连接中...",32,0);
	LCD_Fill(30,180,500,500,WHITE); 
	return res;
} 
int main(void)
{
    Stm32_Clock_Init(384,25,2,8);   //设置时钟,192Mhz   
    delay_init(192);                //初始化延时函数
    uart_init(115200);              //初始化USART
//    usmart_dev.init(96);
	KEY_Init();
   	LED_Init();					//初始化与LED连接的硬件接口
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD 
	W25QXX_Init();				//初始化W25Q256 
	PCF8574_Init();				//初始化PCF8574
 	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
	EXTI_Init();                //外部中断初始化
//	TIM3_Init(100-1,9600-1);
 	f_mount(fs[0],(const TCHAR*)"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],(const TCHAR*)"1:",1); 		//挂载SPI FLASH. 
 	f_mount(fs[2],(const TCHAR*)"2:",1); 		//挂载NAND FLASH. 
	POINT_COLOR=RED;      
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,32,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}
	Show_Str(30,140,250,48,"设备连接中...",32,0);			 		
	//初始化USB主机
  	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb); 
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
//		Key_Detect();
		delay_ms(1);
	}	
}

