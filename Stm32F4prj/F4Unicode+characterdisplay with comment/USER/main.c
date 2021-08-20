#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "exti.h"
#include "dot_driver.h"
#include "key.h"
#include "exfuns.h"
#include "string.h"
#include "timer.h"
#include "lcd.h"
#include "sdram.h"
#include "w25qxx.h"
#include "nand.h"    
#include "ftl.h"
#include "malloc.h"
#include "translate.h"
#include "segment.h"
#include "touni.h"
#include "usbh_usr.h"
#include "fontupd.h"
#include "text.h"
#include "csk4002.h"
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
u8 USH_User_App(void)
{  
//	FIL* fil1;
	u32 total,free;
	u8 t=0;
	u8 res=0;
	u8 key_v;
	u16 univalue[100]={0x4F60,0x597D,0x6211,0x53EB,0x6797,0x680B};
  	res=f_mount(fs[1],(const TCHAR*)"1:",1); 	//���¹���U��
	
	printf("usb f_mount res:%d\r\n",res);
//	fil1=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
//	res=f_open(fil1,"1:/test.txt",FA_READ);
//	printf("f_open res:%d\r\n",res);
//	f_close(fil1);
//	myfree(SRAMIN,fil1);
	res=exf_getfree("1:",&total,&free);
	if(res==0)
	{
		POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
		LCD_ShowString(30,190,250,48,32,"FATFS OK!");	
		LCD_ShowString(30,240,400,48,32,"U Disk Total Size:     MB");	 
		LCD_ShowString(30,290,400,48,32,"U Disk  Free Size:     MB"); 	    
		LCD_ShowNum(320,240,total>>10,5,32);//��ʾU�������� MB
		LCD_ShowNum(320,290,free>>10,5,32);	
		
	} 
	while(font_init()) 		        //����ֿ�
	{
  UPD:    
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		
				 						    
		LCD_ShowString(30,80,240,32,32,"UDISK OK");
		LCD_ShowString(30,112,256,32,32,"Font Updating...");
		key_v=update_font(30,144,32,"1:");//�����ֿ�
		printf("update_font res:%d\r\n",key_v);
		while(key_v)//����ʧ��		
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
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
	
		
		Show_UNIStr(318,304,500,32,univalue,32,0);
			t=200;
			while(t--)//��ʱ,ͬʱɨ�谴��
			{
				delay_ms(1);
				key_v=KEY_Scan(0);
				if(key_v==KEY_ENTER_PRESS)goto UPD;
			}
	}
	
 	f_mount(0,(const TCHAR*)"1:",1); 	//ж��U��
	printf("dis f_mount\r\n");
	POINT_COLOR=RED;//��������Ϊ��ɫ	   
	LCD_Fill(30,180,500,500,WHITE); 
	return res;
} 
 
int main(void)
{
	u16 i;
	
	u8 key_v;
	u8 res;    
	u8 buf[121]="011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110111111";
	u8 InputToIO[LATCH_NUM]={0x00};
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(168,4,2,7);   //����ʱ��,180Mhz
    delay_init(168);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
//	DOT_Init();
	CSK4002_Init();
	//EXTI_Init();
	KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //SDRAM��ʼ��
    LCD_Init();                     //LCD��ʼ��
	W25QXX_Init();				    //��ʼ��W25Q256
    my_mem_init(SRAMIN);            //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);            //��ʼ���ⲿSDRAM�ڴ��
    my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
   	POINT_COLOR=RED;
//	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
//	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2016/2/16");	 	 
//	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
//	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
//	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");   
	res=f_mount(fs[0],(const TCHAR*)"0:",1); 		//����NAND FLASH. 
	printf("nand res:%d\r\n",res);
	POINT_COLOR=RED;      
 	
				 		
	//��ʼ��USB����
  	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb); 
	
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
//		Key_Detect();
		delay_ms(1);
	}	
}




