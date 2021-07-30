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
#include "usmart.h"
#include "sdio_sdcard.h"
#include "ftl.h" 
#include "nand.h"
#include "string.h"
/************************************************
 ALIENTEK ������STM32F429������ʵ��41
 FATFSʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
FIL fil1;
char buffer1[4*NAND_ECC_SECTOR_SIZE+2]="";
int main(void)
{
	u32 fsize=0;
	u32 place=0;
	u32 cnt=0;
 	u32 total,free;
	u8 key;
	u32 t=0;	
	static u8 i=0x31;
	int a=0x1234;
	u8 res=0;
	u8 *backbuf;
	u8 *buf;
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    usmart_dev.init(90);
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //SDRAM��ʼ��
    LCD_Init();                     //LCD��ʼ��
	W25QXX_Init();				    //��ʼ��W25Q256
 	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ�� 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FATFS TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/1/16");	 	 
	LCD_ShowString(30,130,200,16,16,"Use USMART for test");	      
 	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
    FTL_Init();
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	res=f_mount(fs[1],"1:",1); 				//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
		}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}	
	res=f_mount(fs[2],"2:",1); 				//����NAND FLASH.	
	if(res==0X0D)//NAND FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//��ʽ��NAND
		res=f_mkfs("2:",1,4096);//��ʽ��FLASH,2,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
//			f_setlabel((const TCHAR *)"2:NANDDISK");	//����Flash���̵�����Ϊ��NANDDISK
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//��ʽ�����
		}else LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}    
	LCD_Fill(30,150,240,150+16,WHITE);		//�����ʾ			  
	while(exf_getfree("0:",&total,&free))	//�õ�SD������������ʣ������
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//�����ʾ			  
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}	
//	for(i=0;i<nand_dev.block_totalnum;i++)
//	{
//		printf("lut[%d]:%d\r\n",i,nand_dev.lut[i]);
//	}
	
	
	
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
	LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+8*14,170,total>>10,5,16);	//��ʾSD�������� MB
 	LCD_ShowNum(30+8*14,190,free>>10,5,16);     //��ʾSD��ʣ������ MB	
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		
	buf=mymalloc(SRAMIN,4*NAND_ECC_SECTOR_SIZE);		//����һ�������Ļ���
	
	buf[4*NAND_ECC_SECTOR_SIZE]='\0';
//	for(cnt=0;cnt<nand_dev.block_totalnum;i++)
//	{
//		NAND_ReadSpare(cnt*nand_dev.block_pagenum,1,&res,1);//��ȡ�����������־
//        if(res==0XCC)	//�ÿ�
//		printf("FTL_UsedBlockMark[%d]:%d\r\n",cnt,res);
//	}
//	FTL_ReadSectors(backbuf,1,NAND_ECC_SECTOR_SIZE,1);//Ԥ�ȶ�ȡ����0����������,��ֹ��д�����ļ�ϵͳ��.
//	for(cnt=0;cnt<nand_dev.block_totalnum;cnt++)
//	{
//		printf("lut[%d]:%d\r\n",cnt,nand_dev.lut[cnt]);
//	}
	res=f_open(&fil1,"2:/text.txt",FA_WRITE | FA_READ);
	printf("res:%d\r\n",res);
	fsize=f_size(&fil1);
	printf("file size:%d\r\n",fsize);
	while (place!=fsize)
	{
		t++;
		f_gets(buffer1,2049,&fil1);
//		if(res==0)//��ȡ�ɹ�
//		{
//			while(!res)
//			{
//				res=f_read(&fil1,buffer1,strlen((char*)buf),&br);
//				if (res || br < strlen((char*)buf)) 
//					break;
//			}
//		}
		
		place=f_tell(&fil1);
		printf("%d:  buffer1:%s\r\n",t,buffer1);
		printf("place:%d\r\n",place);
		delay_ms(50);
	}
	f_close(&fil1);
	printf("read fin\r\n");
	printf("err_times:%d\r\n",err_times);
			
}
