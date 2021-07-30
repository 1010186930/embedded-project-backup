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
 ALIENTEK ������STM32F429������ʵ��42
 ������ʾʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
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
	u8 fontx[2];                    //gbk��
	u8 key,t;
    int cnt;
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //SDRAM��ʼ��

    LCD_Init();                     //LCD��ʼ��
	W25QXX_Init();				    //��ʼ��W25Q256
    my_mem_init(SRAMIN);            //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);            //��ʼ���ⲿSDRAM�ڴ��
    my_mem_init(SRAMCCM);           //��ʼ���ڲ�CCM�ڴ��
    exfuns_init();		            //Ϊfatfs��ر��������ڴ�  
    f_mount(fs[0],"0:",1);          //����SD�� 
  	f_mount(fs[1],"1:",1);          //����SPI FLASH.
	
//	strcpy(test,"n");
	PrintTest();

//printf("signtable test[0]:%x,%s\r\n",sign[1].unicode,sign[1].numdot);
		

//	while(1) 		        //����ֿ�
//	{
		
//  UPD:    
//		LCD_Clear(WHITE);		   	//����
//		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
//		LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7");
//		while(SD_Init())			//���SD��
//		{
//		  	LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
//		  	delay_ms(200);
//		  	LCD_Fill(30,70,200+30,70+16,WHITE);
//		   	delay_ms(200);		    
//		 }								 						    
//		LCD_ShowString(30,70,200,16,16,"SD Card OK");
//		LCD_ShowString(30,90,200,16,16,"Font Updating...");
//		key=update_font(20,110,16,"0:");//�����ֿ�
//		while(key)//����ʧ��		
//		{		
//			 LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
//			 delay_ms(200);
//			 LCD_Fill(20,110,200+20,110+16,WHITE);
//			 delay_ms(200);		       
//		} 		  
//		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
//		delay_ms(1500);	
//		LCD_Clear(WHITE);//����	       
//	} 
//	POINT_COLOR=RED;       
//	Show_Str(30,30,200,16,"������STM32F4/F7������",16,0);				    	 
//	Show_Str(30,50,200,16,"GBK�ֿ���Գ���",16,0);				    	 
//	Show_Str(30,70,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
//	Show_Str(30,90,200,16,"2016��1��16��",16,0);
//	Show_Str(30,110,200,16,"��KEY0,�����ֿ�",16,0);
//	POINT_COLOR=BLUE;  
//	Show_Str(30,130,200,16,"������ֽ�:",16,0);				    	 
//	Show_Str(30,150,200,16,"������ֽ�:",16,0);				    	 
//	Show_Str(30,170,200,16,"���ּ�����:",16,0);

//	Show_Str(30,200,200,32,"��Ӧ����Ϊ:",32,0); 
//	Show_Str(30,232,200,24,"��Ӧ����Ϊ:",24,0); 
//	Show_Str(30,256,200,16,"��Ӧ����(16*16)Ϊ:",16,0);			 
//	Show_Str(30,272,200,12,"��Ӧ����(12*12)Ϊ:",12,0);			 
//	while(1)
//	{
//		delay_ms(1500);	
//	z=maxx(x,y);

//	//printf("�����GBK���룺%d\n\r",buffer1s[0]);
//	//unicode=ff_convert(buffer1s[0],1);
//	//printf("GBKת���õ���Unicode�룺%d\n\r",unicode);

//	StringToUni(buffer1, unibuf,strlen((const char*)buffer1));
//	unibufsize = sizeof(unibuf) /sizeof(unibuf[0]);
//	
//  GetDot(unibuf, unibufsize);
//	} 

	
}
