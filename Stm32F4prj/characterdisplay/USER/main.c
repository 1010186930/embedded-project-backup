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
 ALIENTEK ������STM32F429������ʵ��42
 ������ʾʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
    
int main(void)
{	
	u16 univalue[100]={0x4F60,0x597D,0x6211,0x53EB,0x6797,0x680B};
	u8 high;
	u8 low;
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];                    //gbk��
	u8 key,t;
    u16 tmptest=0;
	u16 gbktoken=0xc4cf;
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
  	//f_mount(fs[1],"1:",1);          //����SPI FLASH.  
	while(font_init()) 		        //����ֿ�
	{
  UPD:    
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,80,240,32,32,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,80,270,112,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,80,240,32,32,"SD Card OK");
		LCD_ShowString(30,112,256,32,32,"Font Updating...");
		key=update_font(30,144,32,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,112,200,32,32,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(30,112,230,110,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,112,200,32,32,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����       
	} 
	POINT_COLOR=RED;       
 	POINT_COLOR=BLUE;  
	Show_GBKStr(30,304,290,32,"��Ӧ����(32*32)Ϊ:",32,0); 	
	tmptest=ff_convert(gbktoken,1);
	printf("tmptest:%x",tmptest);	
	while(1)
	{
		
			Show_UNIStr(318,304,500,32,univalue,32,0);
			t=200;
			while(t--)//��ʱ,ͬʱɨ�谴��
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
//			LCD_ShowNum(222,208,i,3,32);		//��ʾ������ֽ�    
//			for(j=0x40;j<0xfe;j++)
//			{
//				if(j==0x7f)continue;
//				fontcnt++;
//				LCD_ShowNum(222,240,j,3,32);	//��ʾ������ֽ�	 
//				LCD_ShowNum(222,272,fontcnt,5,32);//���ּ�����ʾ	 
//			 	fontx[1]=j;
//				Show_GBKFont(318,304,fontx,32,0);	  
//				Show_GBKStr(318,340,100,32,fontx,32,0);
//				Show_GBKStr(318,380,100,32,test,32,0);
//				t=200;
//				while(t--)//��ʱ,ͬʱɨ�谴��
//				{
//					delay_ms(1);
//					key=KEY_Scan(0);
//					if(key==KEY0_PRES)goto UPD;
//				}
//				LED0=!LED0;
//			}   
//		}	
}
