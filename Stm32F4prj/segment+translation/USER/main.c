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
 ALIENTEK ������STM32F429������ʵ��42
 ������ʾʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
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
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
	TIM3_Init(5000-1,9000-1);
    SDRAM_Init();                   //SDRAM��ʼ��
    LCD_Init();                     //LCD��ʼ��
	W25QXX_Init();				    //��ʼ��W25Q256
    my_mem_init(SRAMIN);            //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);            //��ʼ���ⲿSDRAM�ڴ��
    my_mem_init(SRAMCCM);           //��ʼ���ڲ�CCM�ڴ��
	exfuns_init();		            //Ϊfatfs��ر��������ڴ�  
	FTL_Init();
	font_init();
    res=f_mount(fs[0],"0:",1);          //����SD�� 
	if(res!=0)
	{
		printf("SD CARD ERROR!\r\n");
		return -1;
	}
	res=f_mount(fs[2],"2:",1); 				//����NAND FLASH.	
	if(res==0X0D)//NAND FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��NAND FLASH
	{
		LCD_ShowString(30,150,200,16,16,"NAND Disk Formatting...");//��ʽ��NAND
		res=f_mkfs("2:",1,4096);//��ʽ��FLASH,2,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Finish");		//��ʽ�����
		else 
			LCD_ShowString(30,150,200,16,16,"NAND Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}    
	pSeg=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	LCD_Fill(30,150,240,150+16,WHITE);		//�����ʾ	
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
		LED0=!LED0;                 //LED0��ת
        delay_ms(200);              //��ʱ200ms
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