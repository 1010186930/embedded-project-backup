#include "sys.h"
#include "delay.h"
#include "usart.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "suocun.h"
#include "tts.h"
#include<stdlib.h>
#include<string.h>
#define MAXDIR 20
void display(const u8*,int,int);
u8 menu;
u8 key0=0,key1=0,key2=0,key3=0,key4=0,key5=0;
int n0,n1,n2,n3,n4,n5;
FIL fil1,fil2,fil3;
void UIfun(u8);
void dis_txt();
char buffer1[38];
char buffer2[121]="";
void play();
void reco();
DWORD P1,P2;
char midname[50];
char name[100][50];
u8* pathtur;//unsigned char相当于整型变量
u16 n;//short int

void highlight(const u8 line)
{
	LCD_Fill(35,40,45,1000,WHITE);//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)  清除原先位置，白色填充	
	LCD_Fill(35,45+32*line,45,55+32*line,BLUE);//32位字号大小
}
u8 get_tnum(const u8 *path)//指向的内容不可变
{
	u16 temp;
  DIR dirs;
	FILINFO finfo;
	u16 i=0;
	u8* fn;
		//highlight(0);	
if(f_opendir(&dirs,(const TCHAR*)path)==FR_OK)
  	finfo.lfsize=_MAX_LFN*2+1;					
	finfo.lfname=mymalloc(SRAMIN,finfo.lfsize);	
{
   while(f_readdir(&dirs,&finfo)==FR_OK)
	 {
		 fn=(u8*)(*finfo.lfname?finfo.lfname:finfo.fname);
	   if(!finfo.fname[0]) break;//这一句一定要加，否则都不出来，跳不出循环
		 if(finfo.fattrib==AM_DIR)
		 {
			 strcpy((u8*)name[i],(u8*)fn);
			 Show_Str(60,32+32*i,400,32,(u8*)fn,32,0);
			 i++;
	   }
  }
}
     myfree(SRAMIN,finfo.lfname);
		return i;
}
void e1_event(u8*state)//ok
{
  u8 menu;
	menu=*state;
	switch(menu)
	{
	  case 0: 
		*state=key0+1;
		key1=0;
		key2=0;
		key3=0;
		key4=0;
		key5=0;
		highlight(0);
		SPeech("[i1][n1][y1]");
		delay_ms(1);
		UIfun(*state);
		break;
		case 1:
				LCD_Clear(WHITE);
			*state=6;
		UIfun(*state);
		break;
		case 2:
				LCD_Clear(WHITE);
			*state=7;
			UIfun(*state);
		break;
			case 3:
				LCD_Clear(WHITE);
			*state=8;
			UIfun(*state);
		break;
			case 4:
				LCD_Clear(WHITE);
			*state=9;
			UIfun(*state);
		break;
			case 5:
				LCD_Clear(WHITE);
			*state=10;
			UIfun(*state);
		break;
	}
}
void e4_event(u8*state)//下一页
{
	u8 res;
  u8 menu;
	menu=*state;
	switch(menu)
	{
	  case 0:
			key0=(key0+1)%n0;
		highlight(key0);	
						strncpy(midname,name[key0]+2,strlen(name[key0])-2);
		SPeech(midname);
		memset(midname,'\0',sizeof(midname));
			//			SPeech(name[key0]);
		   delay_ms(1);
		break;
		case 1:
			key1=(key1+1)%n1;
		highlight(key1);
				strncpy(midname,name[key1]+2,strlen(name[key1])-2);
		SPeech(midname);
		memset(midname,'\0',sizeof(midname));
		//				SPeech(name[key1]);
		   delay_ms(1);
		break;
		case 2:
			key2=(key2+1)%n2;
		highlight(key2);	
				strncpy(midname,name[key2]+2,strlen(name[key2])-2);
		SPeech(midname);	
		memset(midname,'\0',sizeof(midname));
				//		SPeech(name[key2]);
		   delay_ms(1);
		break;
				case 3:
			key3=(key3+1)%n3;
		highlight(key3);
							strncpy(midname,name[key3]+2,strlen(name[key3])-2);			
		SPeech(midname);					
				memset(midname,'\0',sizeof(midname));
			//			SPeech(name[key3]);
		   delay_ms(1);
		break;
						case 4:
			key4=(key4+1)%n4;
		highlight(key4);	
									strncpy(midname,name[key4]+2,strlen(name[key4])-2);
		SPeech(midname);			
						memset(midname,'\0',sizeof(midname));
				//		SPeech(name[key4]);
		   delay_ms(1);
		break;
		case 5:
			key5=(key5+1)%n5;
		highlight(key5);	
									strncpy(midname,name[key5]+2,strlen(name[key5])-2);
		SPeech(midname);			
						memset(midname,'\0',sizeof(midname));
				//		SPeech(name[key4]);
		   delay_ms(1);
		break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			reco();
			if(f_tell(&fil1)==f_size(&fil1))
			{	
				f_lseek(&fil2,0);
					f_lseek(&fil1,0);
			}
			LCD_Clear(WHITE);
			f_gets((char*)buffer2,121,&fil2);
			f_gets((char*)buffer1,38,&fil1);
      play();
			break;			
	}
}
void e6_event(u8*state)
{
	u8 res;
  u8 menu;
	menu=*state;
	switch(menu)
	{
	  case 0:
			key0=(key0+1)%n0;
		  highlight(key0);
				strncpy(midname,name[key0]+2,strlen(name[key0])-2);
		SPeech(midname);
		memset(midname,'\0',sizeof(midname));
		//	SPeech(name[key0]);
		  delay_ms(1);
		break;
		case 1:
			key1=(key1+1)%n1;
		highlight(key1);	
					strncpy(midname,name[key1]+2,strlen(name[key1])-2);
		SPeech(midname);	
		delay_ms(1);
		memset(midname,'\0',sizeof(midname));
		//			SPeech(name[key1]);
		break;
		case 2:
			key2=(key2+1)%n2;
		highlight(key2);	
				strncpy(midname,name[key2]+2,strlen(name[key2])-2);
		SPeech(midname);
		delay_ms(1);
		memset(midname,'\0',sizeof(midname));
			//			SPeech(name[key2]);
		break;
				case 3:
			key3=(key3+1)%n3;
		highlight(key3);	
		strncpy(midname,name[key3]+2,strlen(name[key3])-2);			
		SPeech(midname);
delay_ms(1);				
		memset(midname,'\0',sizeof(midname));		
			//			SPeech(name[key3]);
		break;
						case 4:
			key4=(key4+1)%n4;
		highlight(key4);	memset(midname,'\0',sizeof(midname));
		strncpy(midname,name[key4]+2,strlen(name[key4])-2);
		SPeech(midname);					
						
		//				SPeech(name[key4]);
		   delay_ms(1);
		break;
		case 5:
			key5=(key5+1)%n5;
		highlight(key5);	memset(midname,'\0',sizeof(midname));
		strncpy(midname,name[key5]+2,strlen(name[key5])-2);
		SPeech(midname);					
						
		//				SPeech(name[key4]);
		   delay_ms(1);
		break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			reco();
			if(f_tell(&fil1)==f_size(&fil1))
			{	
				f_lseek(&fil2,0);
					f_lseek(&fil1,0);
			}
			LCD_Clear(WHITE);
			f_gets((char*)buffer2,121,&fil2);
			f_gets((char*)buffer1,38,&fil1);
      play();
			break;			
	}
}

void e5_event(u8*state)//上一页
{
	u8 res;
  u8 menu;
	u8 t1;
	menu=*state;
	switch(menu)
	{
	  case 0:
			if(key0==0) key0=n0-1;
		  else key0--;
		highlight(key0);	
				strncpy(midname,name[key0]+2,strlen(name[key0])-2);
		SPeech(midname);
		memset(midname,'\0',sizeof(midname));
				//SPeech(name[key0]);
		   delay_ms(1);
		break;
		case 1:
			if(key1==0) key1=n1-1;
		  else key1--;
		highlight(key1);	
				strncpy(midname,name[key1]+2,strlen(name[key1])-2);
		SPeech(midname);		
		memset(midname,'\0',sizeof(midname));
	//					SPeech(name[key1]);
		   delay_ms(1);
		break;
		case 2:
			if(key2==0) key2=n2-1;
		  else key2--;
		highlight(key2);
			
			strncpy(midname,name[key2]+2,strlen(name[key2])-2);
		SPeech(midname);		
		memset(midname,'\0',sizeof(midname));
		//		SPeech(name[key2]);
		   delay_ms(1);
		break;
				case 3:
			if(key3==0) key3=n3-1;
		  else key3--;
		highlight(key3);	
						strncpy(midname,name[key3]+2,strlen(name[key3])-2);			
		SPeech(midname);		
		memset(midname,'\0',sizeof(midname));		
			//			SPeech(name[key3]);
		   delay_ms(1);
		break;
		case 4:
			if(key4==0) key4=n4-1;
		  else key4--;
		highlight(key4);
	
			strncpy(midname,name[key4]+2,strlen(name[key4])-2);
		SPeech(midname);
			memset(midname,'\0',sizeof(midname));
		//		SPeech(name[key4]);
		   delay_ms(1);
		break;
	  case 5:
				if(key5==0) key5=n5-1;
		  else key5--;
		highlight(key5);
	
			strncpy(midname,name[key5]+2,strlen(name[key5])-2);
		SPeech(midname);
			memset(midname,'\0',sizeof(midname));
		//		SPeech(name[key4]);
		   delay_ms(1);
		break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			reco();
		  if(f_tell(&fil1)==f_size(&fil1))
		 {
				 
		  if(f_size(&fil1)%37==0)
			 {
			 f_lseek(&fil1,f_size(&fil1)-74);	
			 f_lseek(&fil2,f_size(&fil2)-240);
			 }
			 else
			 {
		   f_lseek(&fil1,f_size(&fil1)-f_size(&fil1)%37-37);	
			 f_lseek(&fil2,f_size(&fil2)-f_size(&fil2)%120-120);
			 }		
		 }			 
			else if(f_tell(&fil1)==37)
			{
					 if(f_size(&fil1)%37==0)
					 {
						 f_lseek(&fil1,f_size(&fil1)-37);	
				     f_lseek(&fil2,f_size(&fil2)-120);	
					 }
					 else
					 {
				   f_lseek(&fil1,f_size(&fil1)-f_size(&fil1)%37);	
				   f_lseek(&fil2,f_size(&fil2)-f_size(&fil2)%120);	
					 }
			}		

			else	
			{				
			   f_lseek(&fil1,f_tell(&fil1)-74);	
				 f_lseek(&fil2,f_tell(&fil2)-240);	
			}
			 LCD_Clear(WHITE);
			 f_gets(buffer2,121,&fil2);
			 f_gets(buffer1,38,&fil1);	
       play();
		break;
	}
}

void e2_event(u8*state)//上一级
{
  u8 menu;
	menu=*state;
	switch(menu)
	{
		case 0:
			dis(0);
			break;
	  case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			dis(0);
			*state=0;
		  UIfun(*state);
		break;
		case 6:
			*state=1;
		UIfun(*state);
		f_close(&fil1);
		f_close(&fil2);
		f_close(&fil3);
		break;
		case 7:
			*state=2;
		UIfun(*state);
		f_close(&fil1);
		f_close(&fil2);
		f_close(&fil3);
		break;
		case 8:
			*state=3;
		UIfun(*state);
		f_close(&fil1);
		f_close(&fil2);
		f_close(&fil3);
		break;
		case 9:
			*state=4;
		UIfun(*state);
		f_close(&fil1);
		f_close(&fil2);
		f_close(&fil3);
		break;
		case 10:
			*state=5;
		UIfun(*state);
		f_close(&fil1);
		f_close(&fil2);
		f_close(&fil3);
		break;
	}
}

void e3_event(u8*state)//书签
{
  u8 menu;
	menu=*state;
	switch(menu)
	{
		case 5:
		case 6:
		case 7:
		case 8:
	   f_read(&fil3, &P1, sizeof (P1), &br);
	   f_read(&fil3, &P2, sizeof (P2), &br);	
		f_close(&fil3);
		f_lseek(&fil1,P1);
    f_lseek(&fil2,P2);
				LCD_Clear(WHITE);
		
			 f_gets(buffer2,121,&fil2);
			 f_gets(buffer1,38,&fil1);
			
       play();
		break;
	}
}


int main()
{
int key_v;
	u8 menu=0;
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		      //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为115200	
 	//LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					  //初始化按键
	LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
	//TTS_Init();
 	my_mem_init(SRAMIN);	//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
	SUOCUN_init();//点显器初始化
		font_init();
 	f_mount(fs[0],"0:",1); 		//挂载SD卡	，0;无错误	
	POINT_COLOR=RED;
	
dis(0);

UIfun(0);

while(1)
{
  key_v=KEY_Scan(0);
/*if(key_v==4) e3_event(&menu);//书签
	else if(key_v==1) e1_event(&menu);//OK
	else if(key_v==5) e2_event(&menu);//返回
	else if(key_v==3)  e5_event(&menu);//上
	else if(key_v==2)   e4_event(&menu);//下*/
		if(key_v==3) e5_event(&menu);//上一页，Key1
	else if(key_v==1) e1_event(&menu);//OK，Key0
	else if(key_v==2) e4_event(&menu);//下一页，Key2
	else if(key_v==5)  e2_event(&menu);//上一级，Key4
	else if(key_v==4)   e6_event(&menu);//下一页目录端，Key3
}


return 0;
}

void UIfun(u8 state)
{
	LCD_Clear(WHITE);//清屏。
	POINT_COLOR=BLUE;
				SPeech("[i1][n1][y1]");
		delay_ms(1);
	switch(state)
	{

	  case 0:
			Show_Str(10,0,320,32,"欢迎使用盲文阅读系统",32,0);
			n0=get_tnum("0:");
		highlight(key0);
			
		strncpy(midname,name[key0]+2,strlen(name[key0])-2);
		
		SPeech(midname);
		memset(midname,'\0',sizeof(midname));
	//	SPeech(name[key0]);
		   delay_ms(1);
		break;
		case 1:
			n1=get_tnum("0:/01盲文基础学习");
		highlight(key1);
		
		strncpy(midname,name[key1]+2,strlen(name[key1])-2);
		SPeech(midname);	
		delay_ms(1);
				//SPeech(name[key1]);

			memset(midname,'\0',sizeof(midname));
		break;
		case 2:
			n2=get_tnum("0:/02一年级课文（不带音调）");
	 highlight(key2);
			
		strncpy(midname,name[key2]+2,strlen(name[key2])-2);
		SPeech(midname);	
		 delay_ms(1);
		memset(midname,'\0',sizeof(midname));
	//			SPeech(name[key2]);

		break;
				case 3:
			n3=get_tnum("0:/03一年级课文（带音调）");
		highlight(key3);
					
						strncpy(midname,name[key3]+2,strlen(name[key3])-2);
		SPeech(midname);
		 delay_ms(1);
				memset(midname,'\0',sizeof(midname));
			//	SPeech(name[key3]);
		  
		break;
				case 4:
			n4=get_tnum("0:/04古诗（不带音调）");
		highlight(key4);
					
						strncpy(midname,name[key4]+2,strlen(name[key4])-2);
		SPeech(midname);
				delay_ms(1);
				memset(midname,'\0',sizeof(midname));
		//		SPeech(name[key4]);
		   
		break;
		   case 5:
				 n5=get_tnum("0:/05古诗（带音调）");
		highlight(key5);
					
						strncpy(midname,name[key5]+2,strlen(name[key5])-2);
		SPeech(midname);
				delay_ms(1);
				memset(midname,'\0',sizeof(midname));
		//		SPeech(name[key4]);
		   
		break;
			 case 6:
				 SPeech("[i1][n1][y1]");
			//if(key1==5)
				
				 //SPeech("[i0][n1][y1]");
			
			display("0:/01盲文基础学习",key1,n1);
		break;
		case 7:
			display("0:/02一年级课文（不带音调）",key2,n2);
		break;
		case 8:
			display("0:/03一年级课文（带音调）",key3,n3);
		break;
		case 9:
			display("0:/04古诗（不带音调）",key4,n4);
		break;
		case 10:
			display("0:/05古诗（带音调）",key5,n5);
		break;
	}
}




void display(const u8*path,int key,int n)
{ int i;
	u16 res,ret;
  DIR dira;
	u8* pname;//
	u8* pdata;
	u8 *pshu;
	FILINFO finfo;
	u16 *tetbl;	//
	u8 temen; // 
	int curindex;
	int tuindex;
  u16 va;//索引值
	u8*fn;
	u8 key_v;
	int t=0;
	BYTE buffer[38];
	temen=n;	
  finfo.lfsize=_MAX_LFN*2+1;						
	finfo.lfname=mymalloc(SRAMIN,finfo.lfsize);
	pname=mymalloc(SRAMIN,finfo.lfsize);
	pdata=mymalloc(SRAMIN,finfo.lfsize);
	pshu=mymalloc(SRAMIN,finfo.lfsize);
	tetbl=mymalloc(SRAMIN,2*temen);
	POINT_COLOR=BLUE;	
	if(f_opendir(&dira,(const TCHAR*)path)==FR_OK)
	{
	  curindex=0;
		while(1)
		{
		  va=dira.index;//
			res=f_readdir(&dira,&finfo);
		
			if(res!=FR_OK||!finfo.fname[0] ) break;
			
	       if(finfo.fattrib==AM_DIR)
		      {
		        tetbl[curindex]=va;
            curindex++;							
	        }
		}	
	}


res=f_opendir(&dira,(const TCHAR*)path);

if(res==FR_OK)
{  
  dir_sdi(&dira,tetbl[key]);
	res=f_readdir(&dira,&finfo);
	if(res==FR_OK)
	{
   fn=(u8*)(*finfo.lfname?finfo.lfname:finfo.fname);
   strcpy((char*)pname,(char*)path);	
		strcat((char*)pname,"/");
	 strcat((char*)pname,(char*)fn); 
		
		strcpy((char*)pdata,(char*)pname); 
		strcpy((char*)pshu,(char*)pname); 
		
		strcat((char*)pname,"/text.txt"); 
		strcat((char*)pdata,"/data.txt");
		strcat((char*)pshu,"/OSET.dat");
		ret=f_open(&fil1,(char*)pname,FA_READ);
    res=f_open(&fil2,(char*)pdata,FA_READ);	  
		 f_open(&fil3,(char*)pshu, FA_WRITE | FA_OPEN_ALWAYS);
	if(res==FR_OK&&ret==FR_OK)
		{
			   dis_txt();
		}
}
}
myfree(SRAMIN,finfo.lfname);	
myfree(SRAMIN,pname);
myfree(SRAMIN,pdata);
myfree(SRAMIN,pshu);
myfree(SRAMIN,tetbl);		
}
void dis_txt()
{

	f_lseek(&fil1,0);
	f_lseek(&fil2,0);
	//f_read(&fil1,buffer1,40,&br);
	//f_read(&fil2,buffer2,120,&br);			 
	 
			 f_gets(buffer2,121,&fil2);
	    f_gets(buffer1,38,&fil1);
	//f_gets((char*)buffer1,20,&fil1);
        play();
	
	}
void play()
{int n;
	int k;
	char btmp[20][7]={""};
  	Show_Str(18,80,800,32,(u8*)buffer1,32,1);
	SPeech(buffer1);
	delay_ms(1);
	for(k=0;k<strlen(buffer2);k++)
	buffer2[k]=buffer2[k];
	buffer2[k]='\0';
n=strlen(buffer2)/6;
	    for(k=0;k<n;k++)
	    memcpy(btmp[k],buffer2+6*k,6);
		LCD_WShowBraille(n,30,240,btmp);
	  DX_FCBraille(btmp);
	memset(btmp,'\0',sizeof(btmp));
}

void reco()
{

	P1=f_tell(&fil1);
	P2=f_tell(&fil2);

		 f_write(&fil3,&P2, sizeof(P2), &bw);
		 f_write(&fil3,&P1, sizeof(P1), &bw);
}