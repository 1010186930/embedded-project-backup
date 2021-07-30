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
#define DANZI_ITEM	32
FIL fil1;
FIL fil2;
u16 danzi=0;
char danzi_dot[DANZI_ITEM-2+1]="";
char buffer1[4*NAND_ECC_SECTOR_SIZE+1]="";
char danzi_src[]="0:/danzi.txt";
char danzi_dst[]="2:/danzitbl.txt";
__packed typedef struct 
{
	u8 fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 ugbkaddr; 			//unigbk的地址
	u32 ugbksize;			//unigbk的大小	 
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;			//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gbk16size;			//gbk16的大小		 
	u32 f24addr;			//gbk24地址
	u32 gbk24size;			//gbk24的大小 	 
	u32 f32addr;			//gbk32地址
	u32 gbk32size;			//gbk32的大小 
}_tbl_info; 
u8 Byte32WriteIntoNand(char *dst,char * src);
u8 TestDanziInNand(char *path);
int main(void)
{
	static u32 place=0;
	u16 unione=0x4e0a;
	u16 unitwo=0x4e0d;
	u16 unionedest=0;
	u16 unitwodest=0;
	u32 cnt=0;
 	u32 total,free;
	u8 key;
	u8 t=0;	
	static u8 i=0x31;
	int a=0x1234;
	u8 res=0;
	u32 fsize=0;
	u32 fplace=0;
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
//	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		
//	buf=mymalloc(SRAMIN,4*NAND_ECC_SECTOR_SIZE);		//申请一个扇区的缓存
//	
//	buf[4*NAND_ECC_SECTOR_SIZE]='\0';
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
////	Byte32WriteIntoNand(danzi_dst,danzi_src);
//	TestDanziInNand(danzi_dst);
	res=f_open(&fil2,"2:/danzitbl.txt",FA_READ);
	printf("danzi file open:%d\r\n",res);
	fsize=f_size(&fil2);
	printf("danzi file_size in nand:%d\r\n",fsize);
	f_read(&fil2,&danzi,sizeof(danzi),&br);
	fplace=f_tell(&fil2);
	printf("pointer place:%d\r\n",fplace);
	f_close(&fil2);
//	while(1)
//	{
//		key=KEY_Scan(0);
//		switch(key)
//		{
//			case KEY0_PRES://KEY0按下,读取sector
//				res=f_open(&fil1,"2:/text2.txt",FA_WRITE | FA_READ |FA_CREATE_ALWAYS);
//			printf("fopen res:%d\r\n",res);
//				if(res==0)//读取成功
//				{
//					printf("create over\r\n");
////					for(i=0;i<nand_dev.block_totalnum;i++)
////					{
////						printf("lut[%d]:%d\r\n",i,nand_dev.lut[i]);
////					}
//				}
//				f_close(&fil1);
//				break;
//			case KEY1_PRES://KEY1按下,写入sector
////				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//填充数据(随机的,根据t的值来确定) 
//			res=f_open(&fil1,"2:/text2.txt",FA_WRITE | FA_READ);
//			res=f_write(&fil1,&unione,sizeof(unione),&bw);
//			res=f_write(&fil1,&unitwo,sizeof(unitwo),&bw);
//			
////			f_lseek(&fil1,f_size(&fil1));
////			memset(buf,0x00+i,4*NAND_ECC_SECTOR_SIZE);
////			i++;
//////			buf[0]=(u8)a;
//////			buf[1]=(u8)(a>>8);
//////			buf[2]='\0';
////			if(!res)
////			{
////				do
////				{
////					res = f_write(&fil1,buf,strlen((char*)buf),&bw);
////					if(res)
////						break;
////				}
////				while (bw <strlen((char*)buf));
////			}		
//			f_close(&fil1);
//			printf("write res:%d\r\n",res);
//			break;
//			case KEY2_PRES://KEY2按下,恢复sector的数据
//				
//				res=f_open(&fil1,"2:/text2.txt",FA_WRITE | FA_READ);
//				res=f_read(&fil1,&unionedest,sizeof(unionedest),&br);
//				res=f_read(&fil1,&unitwodest,sizeof(unitwodest),&br);
//				if(res==0)
//				{
//					printf("unionedest:%x\r\n",unionedest);
//					printf("unitwodest:%x\r\n",unitwodest);
//				}
//					
//				
////				if(place!=0)
////				{
////					printf("place:%d\r\n",place);
////					f_lseek(&fil1,place);
////				}
////				
////				if(res==0)//读取成功
////				{
////					while(!res)
////					{
////						res=f_read(&fil1,buffer1,strlen((char*)buf),&br);
////						if (res || br < strlen((char*)buf)) 
////							break;
////					}
////				}
////				printf("buffer1:%s\r\n",buffer1);
////				place=f_tell(&fil1);
//				f_close(&fil1);
//				break;
//		}
//		t++;
//		delay_ms(10);
//		if(t==20)
//		{
//			LED0=!LED0;
//			t=0;
//		}
//	} 
}
u8 Byte32WriteIntoNand(char *dst,char * src)
{
	u8 res_sd=0xFF;
	u8 res_nand=0xFF;
	u8* pdstname;
	u8* psrcname;
	u32 dstfile_size=0;
	u32 srcfile_size=0;
	u32 dstfile_place=0;
	u32 srcfile_place=0;
	u32 write_times=0;
	u32 read_times=0;
	pdstname=mymalloc(SRAMIN,100);
	psrcname=mymalloc(SRAMIN,100);
	if(pdstname==NULL || psrcname==NULL)
	{
		myfree(SRAMIN,pdstname);
		myfree(SRAMIN,psrcname);
		return 5;		//内存申请失败
	}
	strcpy((char*)psrcname,(char*)src);
	res_sd=f_open(&fil1,(const TCHAR*)psrcname,FA_READ );
	printf("open %s in SD:%d\r\n",psrcname,res_sd);
	strcpy((char*)pdstname,(char*)dst);
	res_nand=f_open(&fil2,(const TCHAR*)pdstname,FA_WRITE | FA_READ | FA_CREATE_ALWAYS );
	printf("create %s:%d\r\n",pdstname,res_sd);
	srcfile_size=f_size(&fil1);
	printf("srcfile_size:%d\r\n",srcfile_size);
	if(res_nand==0 && res_sd==0)
	{
		while(srcfile_place!=srcfile_size)
		{
			res_sd=f_read(&fil1,&danzi,sizeof(danzi),&br);
			printf("read danzi :%d,danzi:%x\r\n",res_sd,danzi);
			if(res_sd!=0||br!=sizeof(danzi))
				break;
			res_sd=f_read(&fil1,danzi_dot,DANZI_ITEM-2,&br);
			printf("read danzi_dot :%d,danzi_dot:%s\r\n",res_sd,danzi_dot);
			if (res_sd || br !=DANZI_ITEM-2) 
				break;
			read_times++;
			printf("read times:%d\r\n",read_times);
			res_nand=f_write(&fil2,&danzi,sizeof(danzi),&br);
			printf("write danzi :%d\r\n",res_nand);
			if(res_nand!=0||br!=sizeof(danzi))
				break;
			res_nand=f_write(&fil2,danzi_dot,DANZI_ITEM-2,&br);
			printf("write danzi_dot :%d\r\n",res_nand);
			if (res_nand || br < DANZI_ITEM-2) 
				break;	
			write_times++;
			printf("write times:%d\r\n",write_times);
			srcfile_place=f_tell(&fil1);
		}
		f_close(&fil1);
		f_close(&fil2);
	}
	myfree(SRAMIN,pdstname);
	myfree(SRAMIN,psrcname);
}
u8 TestDanziInNand(char *path)
{
	u8 res_nand=0xFF;
	u32 file_size=0;
	u32 file_place=0;
	u32 read_times=0;
	char* pname;
	pname=mymalloc(SRAMIN,100);
	if(pname==NULL)
	{
		myfree(SRAMIN,pname);
		return 5;		//内存申请失败
	}
	strcpy(pname,path);
	res_nand=f_open(&fil2,(const TCHAR*)pname,FA_READ);
	printf("danzi file open:%d\r\n",res_nand);
	file_size=f_size(&fil2);
	printf("danzi file_size in nand:%d\r\n",file_size);
	if(res_nand==0)
	{
		while(file_place!=file_size)
		{
			res_nand=f_read(&fil2,&danzi,sizeof(danzi),&br);
			printf("read danzi :%d,danzi:%x\r\n",res_nand,danzi);
			if(res_nand!=0||br!=sizeof(danzi))
				break;
			res_nand=f_read(&fil2,danzi_dot,DANZI_ITEM-2,&br);
			printf("read danzi_dot :%d,danzi_dot:%s\r\n",res_nand,danzi_dot);
			if (res_nand || br !=DANZI_ITEM-2) 
				break;
			read_times++;
			printf("read times:%d\r\n",read_times);
			file_place=f_tell(&fil2);
		}
		f_close(&fil2);
	}
	myfree(SRAMIN,pname);
	printf("errtimes:%d\r\n",err_times);
}
