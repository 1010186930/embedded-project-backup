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
#include "ftl.h" 
#include "nand.h"
#include "translate.h"
#include "segment.h"
#include "touni.h"
#include "timer.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验42
 汉字显示实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
#define MAX_READBYTES	100
FIL *fil1=NULL;
FIL *fil2=NULL;
FIL *pSeg=NULL;
u8 str[100]="";
char dot_res[200] = "";
unsigned short dst_res[100] = { 0x00 };
unsigned short seg_res[100] = { 0x00 };
char segpath[100] = "0:/FenciTable.txt";
char polyphonic_two_dst[]="2:/polyphonictwo.txt";
char danzi_dst[]="2:/danzitbl.txt";
u8 demo = 1;
int fil1size;
int fil2size;
int res_key1=1;
int res_wk=1;
int texttype=-2;
int press_wkup(void);
int press_key1(void);
void ExitCurrentText(void);
int ShowNextText(int strtype);
int main(void)
{	
	
	DIR *dir=NULL;
	u8 key_v;
	u8 res;                    
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
	TIM3_Init(5000-1,9000-1);
    SDRAM_Init();                   //SDRAM初始化
    LCD_Init();                     //LCD初始化
	W25QXX_Init();				    //初始化W25Q256
    my_mem_init(SRAMIN);            //初始化内部内存池
    my_mem_init(SRAMEX);            //初始化外部SDRAM内存池
    my_mem_init(SRAMCCM);           //初始化内部CCM内存池
	exfuns_init();		            //为fatfs相关变量申请内存  
	FTL_Init();
	font_init();
    res=f_mount(fs[0],"0:",1);          //挂载SD卡 
	if(res!=0)
	{
		printf("SD CARD ERROR!\r\n");
		return -1;
	}
	res=f_mount(fs[2],"2:",1); 				//挂载NAND FLASH.	
	if(res==0X0D)//NAND FLASH磁盘,FAT文件系统错误,重新格式化NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//格式化NAND
		res=f_mkfs("2:",1,4096);//格式化FLASH,2,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//格式化完成
		else 
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}    
	pSeg=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	LCD_Fill(30,150,240,150+16,WHITE);		//清除显示	
 	POINT_COLOR=BLUE;  
	res=f_opendir(dir,"0:");
	if(res!=0)
	{
		printf("OPEN ROOT DIRECTORY ERROR!\r\n");
		return -2;
	}
	res=f_open(pSeg,segpath, FA_READ);
	if (res!=0)
	{
		printf("open dict file error:%d\r\n",res);
		return -2;
	}
	Show_GBKStr(30,40,800,32,"PRESS WK_UP OPEN UTF_8 TEXT",32,0);
	Show_GBKStr(30,80,800,32,"PRESS KEY1 OPEN GBK TEXT",32,0);
	while(1)
	{
		key_v=KEY_Scan(0);
		if(key_v == WKUP_PRES && demo == 1)
		{
			texttype=press_wkup();
			res_wk=0;
		}
		else if(key_v == KEY1_PRES && demo == 1)
		{
			texttype=press_key1();
			res_key1=0;
		}
		else if(key_v == KEY2_PRES && demo == 0)
		{
			ExitCurrentText();
		}
		else if(key_v == KEY0_PRES && demo == 0)
		{
			memset(str,'\0',sizeof(str)/sizeof(unsigned char));
			memset(dot_res,'\0',sizeof(dot_res)/sizeof(char));
			Init_Uni(dst_res,sizeof(dst_res)/sizeof(unsigned short));
			Init_Uni(seg_res,sizeof(seg_res)/sizeof(unsigned short));
			if(res_key1 == 0)
			{
				if(f_tell(fil2)==f_size(fil2))
					f_lseek(fil2,0);
				f_gets((char*)str,MAX_READBYTES,fil2);
				ShowNextText(texttype);
				
			}
			else if(res_wk == 0)
			{
				if(f_tell(fil1)==f_size(fil1))
					f_lseek(fil1,0);
				f_gets((char*)str,MAX_READBYTES,fil1);
				ShowNextText(texttype);
			}
		}
		LED0=!LED0;                 //LED0翻转
        delay_ms(200);              //延时200ms
	}
}
int press_wkup()
{
	u8 res;
	int cnt=0;
	int addcnt=0;
	int strtype=Unknown;
	memset(str,'\0',sizeof(str)/sizeof(unsigned char));
	memset(dot_res,'\0',sizeof(dot_res)/sizeof(char));
	Init_Uni(dst_res,sizeof(dst_res)/sizeof(unsigned short));
	Init_Uni(seg_res,sizeof(seg_res)/sizeof(unsigned short));
	strtype=AnalyzeTextType("0:/text1.txt");
	printf("strtype:%d\r\n",strtype);
	LCD_Clear(WHITE);
	if (strtype == 1)
		Show_GBKStr(30,40,800,32,"This Text is GBK",32,0);
	else if (strtype == -1)
		Show_GBKStr(30,40,800,32,"This Text is Unknown",32,0);
	else if (strtype == 2)
		Show_GBKStr(30,40,800,32,"This Text is UTF-8",32,0);
	fil1=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_open(fil1,"0:/text1.txt", FA_READ);
	if (res!=0)
	{
		printf("open text1 file error:%d\r\n",res);
		return -2;
	}
	fil1size=f_size(fil1);
	f_gets((char*)str,MAX_READBYTES,fil1);
	printf("str:%s\r\n",str);
	ShowNextText(strtype);
	demo = 0;
	return strtype;
}
int press_key1()
{
	u8 res;
	int cnt=0;
	int addcnt=0;
	int strtype=Unknown;
	memset(str,'\0',sizeof(str)/sizeof(unsigned char));
	memset(dot_res,'\0',sizeof(dot_res)/sizeof(char));
	Init_Uni(dst_res,sizeof(dst_res)/sizeof(unsigned short));
	Init_Uni(seg_res,sizeof(seg_res)/sizeof(unsigned short));
	strtype=AnalyzeTextType("0:/text2.txt");
	printf("strtype:%d\r\n",strtype);
	LCD_Clear(WHITE);
	if (strtype == 1)
		Show_GBKStr(30,40,800,32,"This Text is GBK",32,0);
	else if (strtype == -1)
		Show_GBKStr(30,40,800,32,"This Text is Unknown",32,0);
	else if (strtype == 2)
		Show_GBKStr(30,40,800,32,"This Text is UTF-8",32,0);
	fil2=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_open(fil2,"0:/text2.txt", FA_READ);
	if (res!=0)
	{
		printf("open text2 file error:%d\r\n",res);
		return -2;
	}
	fil2size=f_size(fil2);
	f_gets((char*)str,MAX_READBYTES,fil2);
	printf("str:%s\r\n",str);
	ShowNextText(strtype);
	demo = 0;
	return strtype;
}
void ExitCurrentText()
{
	if(res_key1 == 0)
	{
		f_close(fil1);
		myfree(SRAMIN,fil1);
		res_key1 = 1;
		demo = 1;
	}
	else if(res_wk == 0)
	{
		f_close(fil2);
		myfree(SRAMIN,fil2);
		res_wk = 1;
		demo = 1;
	}
	LCD_Clear(WHITE);
	Show_GBKStr(30,40,800,32,"PRESS WK_UP OPEN UTF_8 TEXT",32,0);
	Show_GBKStr(30,80,800,32,"PRESS KEY1 OPEN GBK TEXT",32,0);
}

int ShowNextText(int strtype)
{
	int cnt,addcnt;
	StrToUni(str, dst_res, strtype, strlen((char*)str));
	cnt = Unicnt(dst_res,sizeof(dst_res)/sizeof(unsigned short));
	Unidisplay(dst_res, cnt);
	addcnt= DevideString(seg_res, dst_res, cnt, pSeg);
	printf("addcnt:%d\r\n",addcnt);
	cnt = Unicnt(seg_res,sizeof(seg_res)/sizeof(unsigned short));
	Unidisplay(seg_res, cnt);
	LCD_Clear(WHITE);
	Show_UNIStr(30,80,800,32,seg_res,32,0);
	TwoPloyphonicTranslate(seg_res, cnt, dot_res, polyphonic_two_dst);
//	printf("dot text2 res:%s\r\n", dot_res);
	DrawBraillePattern(dot_res, strlen(dot_res));
	return addcnt;
}