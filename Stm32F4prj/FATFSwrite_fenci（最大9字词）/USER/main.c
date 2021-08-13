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
#define DANZI_ITEM	32
FIL fil1;
FIL fil2;
FIL fil3;
u16 danzi=0;
u32 two_ployphonic=0;
char danzi_dot[DANZI_ITEM-2+1]="";
char two_ployphonic_dot[DANZI_ITEM-4+1]="";
u16 segdictionary[DANZI_ITEM]={0x00};
char buffer1[4*NAND_ECC_SECTOR_SIZE+1]="";
char danzi_src[]="0:/danzi.txt";
char danzi_dst[]="2:/danzitbl.txt";
char polyphonic_two_src[]="0:/polyphonictwo.txt";
char polyphonic_two_dst[]="2:/polyphonictwo.txt";
char segdictionary_src[]="0:/FenciTable.txt";
char segdictionary_dst[]="2:/FenciTable.txt";
__packed typedef struct 
{
	u8 fontok;				//�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
	u32 ugbkaddr; 			//unigbk�ĵ�ַ
	u32 ugbksize;			//unigbk�Ĵ�С	 
	u32 f12addr;			//gbk12��ַ	
	u32 gbk12size;			//gbk12�Ĵ�С	 
	u32 f16addr;			//gbk16��ַ
	u32 gbk16size;			//gbk16�Ĵ�С		 
	u32 f24addr;			//gbk24��ַ
	u32 gbk24size;			//gbk24�Ĵ�С 	 
	u32 f32addr;			//gbk32��ַ
	u32 gbk32size;			//gbk32�Ĵ�С 
}_tbl_info; 
u8 WriteSegDictionaryIntoNand(char *dst,char * src);
u8 WriteTwoPolyIntoNand(char *dst,char * src);
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
//	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		
//	buf=mymalloc(SRAMIN,4*NAND_ECC_SECTOR_SIZE);		//����һ�������Ļ���
//	
//	buf[4*NAND_ECC_SECTOR_SIZE]='\0';
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
////	Byte32WriteIntoNand(danzi_dst,danzi_src);
//	WriteTwoPolyIntoNand(polyphonic_two_dst,polyphonic_two_src);
WriteSegDictionaryIntoNand(segdictionary_dst,segdictionary_src);
//	TestDanziInNand(danzi_dst);
//	res=f_open(&fil2,"2:/danzitbl.txt",FA_READ);
//	printf("danzi file open:%d\r\n",res);
//	fsize=f_size(&fil2);
//	printf("danzi file_size in nand:%d\r\n",fsize);
//	f_read(&fil2,&danzi,sizeof(danzi),&br);
//	fplace=f_tell(&fil2);
//	printf("pointer place:%d\r\n",fplace);
//	f_close(&fil2);
//	while(1)
//	{
//		key=KEY_Scan(0);
//		switch(key)
//		{
//			case KEY0_PRES://KEY0����,��ȡsector
//				res=f_open(&fil1,"2:/text2.txt",FA_WRITE | FA_READ |FA_CREATE_ALWAYS);
//			printf("fopen res:%d\r\n",res);
//				if(res==0)//��ȡ�ɹ�
//				{
//					printf("create over\r\n");
////					for(i=0;i<nand_dev.block_totalnum;i++)
////					{
////						printf("lut[%d]:%d\r\n",i,nand_dev.lut[i]);
////					}
//				}
//				f_close(&fil1);
//				break;
//			case KEY1_PRES://KEY1����,д��sector
////				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//�������(�����,����t��ֵ��ȷ��) 
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
//			case KEY2_PRES://KEY2����,�ָ�sector������
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
////				if(res==0)//��ȡ�ɹ�
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
u8 WriteSignIntoNand(char *dst,char * src)
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
		return 5;		//�ڴ�����ʧ��
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
		return 5;		//�ڴ�����ʧ��
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
u8 WriteTwoPolyIntoNand(char *dst,char * src)
{
	u8 res_sd=0xFF;
	u8 res_nand=0xFF;
	u8* pdstname;
	u8* psrcname;
	u16 unitmph=0x00;
	u16 unitmpl=0x00;
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
		return 5;		//�ڴ�����ʧ��
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
			res_sd=f_read(&fil1,&unitmph,sizeof(unitmph),&br);
			printf("read unitmph :%d,unitmph:%x\r\n",res_sd,unitmph);
			if(res_sd!=0||br!=sizeof(unitmph))
				break;
			res_sd=f_read(&fil1,&unitmpl,sizeof(unitmpl),&br);
			printf("read unitmph :%d,unitmph:%x\r\n",res_sd,unitmpl);
			if(res_sd!=0||br!=sizeof(unitmpl))
				break;
			res_sd=f_read(&fil1,two_ployphonic_dot,DANZI_ITEM-4,&br);
			printf("read two polyphonic_dot :%d,two polyphonic_dot:%s\r\n",res_sd,two_ployphonic_dot);
			if (res_sd || br !=DANZI_ITEM-4) 
				break;
			read_times++;
			printf("read times:%d\r\n",read_times);
			res_nand=f_write(&fil2,&unitmph,sizeof(unitmph),&br);
			printf("write unitmph :%d\r\n",res_nand);
			if(res_nand!=0||br!=sizeof(unitmph))
				break;
			res_nand=f_write(&fil2,&unitmpl,sizeof(unitmpl),&br);
			printf("write unitmpl :%d\r\n",res_nand);
			if(res_nand!=0||br!=sizeof(unitmpl))
				break;
			res_nand=f_write(&fil2,two_ployphonic_dot,DANZI_ITEM-4,&br);
			printf("write two_ployphonic_dot :%d\r\n",res_nand);
			if (res_nand || br < DANZI_ITEM-4) 
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
int Unicnt(unsigned short* src)// if src have no 0x00,fail
{
	int cnt = 0;
	unsigned short* psrc = src;
	while (*(psrc++) != 0x00)
		cnt++;
	psrc = NULL;
	return cnt;
}
void Unidisplay(unsigned short* src, int unilen)
{
	int i;
	for (i = 0; i < unilen; i++)
		printf("src[%d]:%x\r\n", i, *(src + i));
}
u8 WriteSegDictionaryIntoNand(char *dst,char * src)
{
	u8 res_sd=0xFF;
	u8 res_nand=0xFF;
	u8 seg_cnt=0;
	u8 seg_cntlast=0;
	int current_itemcnt=0;
	u8* pdstname;
	u8* psrcname;
	u32 srcfile_size=0;
	u32 srcfile_place=0;
	u32 write_times=0;
	u32 read_times=0;
	pdstname=mymalloc(SRAMIN,100);
	psrcname=mymalloc(SRAMIN,100);
	if(pdstname==NULL || psrcname==NULL)
	{
		myfree(SRAMIN,pdstname);
		myfree(SRAMIN,psrcname);
		return 5;		//�ڴ�����ʧ��
	}
	strcpy((char*)psrcname,(char*)src);
	res_sd=f_open(&fil1,(const TCHAR*)psrcname,FA_READ );
	printf("open %s in SD:%d\r\n",psrcname,res_sd);
	strcpy((char*)pdstname,(char*)dst);
	res_nand=f_open(&fil3,(const TCHAR*)pdstname,FA_WRITE | FA_READ | FA_CREATE_ALWAYS );
	printf("create %s:%d\r\n",pdstname,res_sd);
	srcfile_size=f_size(&fil1);
	printf("srcfile_size:%d\r\n",srcfile_size);
	if(res_nand==0 && res_sd==0)
	{
		while(srcfile_place!=srcfile_size)
		{
			res_sd=f_read(&fil1,segdictionary,DANZI_ITEM,&br);
			seg_cnt=Unicnt(segdictionary);
			if(res_sd==0)
				current_itemcnt++;
			if((seg_cntlast != seg_cnt) && seg_cnt != 1)
			{
				printf("**********READ %d LENGTH ITEM**********\r\n",seg_cntlast);
				printf("read segfile state :%d,seg_cnt:%d\r\n",res_sd,current_itemcnt - 1);
				current_itemcnt=1;
			}
			Unidisplay(segdictionary, seg_cnt);
			seg_cntlast=seg_cnt;
			if(res_sd!=0||br!=DANZI_ITEM)
				break;
			read_times++;
			printf("read times:%d\r\n",read_times);
			res_nand=f_write(&fil3,segdictionary,DANZI_ITEM,&bw);
			if (res_nand || bw < DANZI_ITEM) 
				break;	
			write_times++;
			printf("write times:%d\r\n",write_times);
			srcfile_place=f_tell(&fil1);
		}
		f_close(&fil1);
		f_close(&fil3);
	}
	myfree(SRAMIN,pdstname);
	myfree(SRAMIN,psrcname);
}