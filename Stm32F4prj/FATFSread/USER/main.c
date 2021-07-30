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
 ALIENTEK 阿波罗STM32F429开发板实验41
 FATFS实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
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
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90);
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //SDRAM初始化
    LCD_Init();                     //LCD初始化
	W25QXX_Init();				    //初始化W25Q256
 	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"FATFS TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/1/16");	 	 
	LCD_ShowString(30,130,200,16,16,"Use USMART for test");	      
 	while(SD_Init())//检测不到SD卡
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
    FTL_Init();
 	exfuns_init();							//为fatfs相关变量申请内存				 
  	f_mount(fs[0],"0:",1); 					//挂载SD卡 
 	res=f_mount(fs[1],"1:",1); 				//挂载FLASH.	
	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//格式化完成
		}else LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}	
	res=f_mount(fs[2],"2:",1); 				//挂载NAND FLASH.	
	if(res==0X0D)//NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//格式化NAND
		res=f_mkfs("2:",1,4096);//格式化FLASH,2,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
//			f_setlabel((const TCHAR *)"2:NANDDISK");	//设置Flash磁盘的名字为：NANDDISK
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//格式化完成
		}else LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}    
	LCD_Fill(30,150,240,150+16,WHITE);		//清除显示			  
	while(exf_getfree("0:",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,150,240,150+16,WHITE);	//清除显示			  
		delay_ms(200);
		LED0=!LED0;//DS0闪烁
	}	
//	for(i=0;i<nand_dev.block_totalnum;i++)
//	{
//		printf("lut[%d]:%d\r\n",i,nand_dev.lut[i]);
//	}
	
	
	
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	LCD_ShowString(30,150,200,16,16,"FATFS OK!");	 
	LCD_ShowString(30,170,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(30,190,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+8*14,170,total>>10,5,16);	//显示SD卡总容量 MB
 	LCD_ShowNum(30+8*14,190,free>>10,5,16);     //显示SD卡剩余容量 MB	
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		
	buf=mymalloc(SRAMIN,4*NAND_ECC_SECTOR_SIZE);		//申请一个扇区的缓存
	
	buf[4*NAND_ECC_SECTOR_SIZE]='\0';
//	for(cnt=0;cnt<nand_dev.block_totalnum;i++)
//	{
//		NAND_ReadSpare(cnt*nand_dev.block_pagenum,1,&res,1);//读取备份区坏块标志
//        if(res==0XCC)	//好块
//		printf("FTL_UsedBlockMark[%d]:%d\r\n",cnt,res);
//	}
//	FTL_ReadSectors(backbuf,1,NAND_ECC_SECTOR_SIZE,1);//预先读取扇区0到备份区域,防止乱写导致文件系统损坏.
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
//		if(res==0)//读取成功
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
