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





u8 s_files(u8*);
void highlight( u8 );
int mouse=0;
u8 k1_event(u8);
u8 k2_event();
void k3_event(u8);
//int n_files;
int curplace=0;	
	u8 path[200]={""};//全局变量，存放当前地址
	void k4_event();
	void k5_event();
	void k6_event();
	char buffer1[38];
	char buffer2[121]="";
	FIL fil1,fil2,fil3;
	u8*pathtxt=0;
	u8*pathdata=0;
	u8*pshu=0;
	int demo=1;
	void dis_txt();
	void play();
	void reco();
	DWORD P1,P2;
int main()
{
	
int key_v;
u8 menu=0;
int n_files;
delay_init();	    	
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
uart_init(9600);		  	
KEY_Init();				
LCD_Init();			
W25QXX_Init();		
my_mem_init(SRAMIN);	
exfuns_init();				
SUOCUN_init();
font_init();
f_mount(fs[0],"0:",1); 	
POINT_COLOR=RED;

	n_files=s_files("0:");
	strcpy((char*)path,"0:");
		while(1)
	{
	  key_v=KEY_Scan(0);
		if(key_v==1&&demo==1)
		{
			mouse=0;
		  n_files=k1_event(curplace);
		}
		 else if(key_v==3&&demo==1)
			 k3_event(n_files);
		else if(key_v==2)
		  n_files=k2_event();
	  else if(key_v==4&&demo==0)
		  k4_event();
    else if(key_v==5&&demo==0)
		    k5_event();
		else if(key_v==6&&demo==0)
		    k6_event();
	}
return 0;
}


u8 s_files(u8*path)
{
  u8 res=0;
	DIR*dir=0;
	FILINFO*finfo=0;
	u8*fn=0;
	u8*pathp=0;
	u8 va;
	u8*tetbl;
	int i=0;
		tetbl=mymalloc(SRAMIN,512);
	  dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	  finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	  finfo->lfsize=_MAX_LFN*2+1;          
	  finfo->lfname= mymalloc(SRAMIN,finfo->lfsize); 
 //注意，这个必须放在finfo.lfanme之前。先为finfo申请内存，再才会有为finfo.lfname申请内存
		//dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	//finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	pathp=mymalloc(SRAMIN,511);
	pathp[0]=0;
	mymemset(pathp,0,511);
	highlight(0);
	strcat((char*)pathp,(const char*)path);

	//res=f_opendir(dir,(const TCHAR*)pathp);//path或者pathp都可以
  res=f_opendir(dir,(const TCHAR*)pathp);
	if(res==0)
	{
	  while(1)
		{
		  va=dir->index;
      res=f_readdir(dir,finfo);
      fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
		  if(!finfo->fname[0]||res!=FR_OK) break;
			if(finfo->fattrib==AM_DIR)
			{
			 Show_Str(60,60+20*i,200,16,(u8*)fn,16,0);
		   tetbl[i]=va;
			 i++;
			}
		}
	}	
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	myfree(SRAMIN,dir);
  myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
	return i;
}


u8 k1_event(u8 curfiles)
{
  u8 res=0;
	DIR*dir=0;
	FILINFO*finfo=0;
	u8*fn=0;
	u8*tetbl;
	u8*pathp=0;
	u8 va;
	int i=0;
	int k=0;
	u8 red=0;
	u8 ret=0;
  LCD_Clear(WHITE);
	tetbl=mymalloc(SRAMIN,512);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	finfo->lfsize=_MAX_LFN*2+1;          
	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	pathp=mymalloc(SRAMIN,511);
  pathp[0]=0;
	strcat((char*)pathp,(char*)path);
	memset(path,0,200);
	res=f_opendir(dir,(const TCHAR*)pathp);
	if(res==0)
	{
	  while(1)
		{
		  va=dir->index;
			res=f_readdir(dir,finfo);
		  fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
		  if(!finfo->fname[0]||res!=FR_OK) break;
			if(finfo->fattrib==AM_DIR)
			{
		   tetbl[i]=va;
				if(i==curfiles) break;
				i++;
			}
		}
	}
  strcat((char*)pathp,"/");
	strcat((char*)pathp,(char*)fn);
	strcpy((char*)path,(char*)pathp);
	f_closedir(dir);
	myfree(SRAMIN,tetbl);
	i=0;
	res=f_opendir(dir,(const TCHAR*)pathp);
	if(res==0)
	{
	  while(1)
		{
		  res=f_readdir(dir,finfo);
		  fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
		  if(!finfo->fname[0]||res!=FR_OK) break;
			if(finfo->fattrib==AM_DIR)
			{
			   Show_Str(60,60+20*i,200,16,(u8*)fn,16,0);
				 i++;
			}
			else
			{
				demo=0;
				break;
			}
		}
	
	}
	//n_files=i;
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	myfree(SRAMIN,dir);
  myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
			curplace=0;
	if(demo==1)
	{
	  highlight(mouse);
	}
	else if(demo==0)
	{
	      pathtxt=mymalloc(SRAMIN,511);
				pathdata=mymalloc(SRAMIN,511);
				pathtxt[0]=pathdata[0]=0;
				mymemset(pathtxt,0,511);
				mymemset(pathdata,0,511);
		   
			  strcat((char*)pathtxt,(char*)path);
				strcat((char*)pathdata,(char*)path);
				strcat((char*)pathtxt,"/text.txt"); 
		    strcat((char*)pathdata,"/data.txt");

	      ret=f_open(&fil1,(char*)pathtxt,FA_READ);
        red=f_open(&fil2,(char*)pathdata,FA_READ);

		ret=f_open(&fil1,(char*)pathtxt,FA_READ);
    res=f_open(&fil2,(char*)pathdata,FA_READ);	  
	   if(res==FR_OK&&ret==FR_OK)
			   dis_txt();
	}
	return i;
}

u8 k2_event()
{
  u8 res=0;
	DIR*dir=0;
	FILINFO*finfo=0;
	u8*fn=0;
	u8*tetbl;
  u8*pathp=0;
	u8*patht=0;
	//u16 temp=0;
	char*p=0;
	u8*fn1=0;
	int i=0;
	int k;
	int j=0;
	u8 fnt[200]={""};
	//patht=path;
	//memset(path,0,200);
	LCD_Clear(WHITE);
	tetbl=mymalloc(SRAMIN,512);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	finfo->lfsize=_MAX_LFN*2+1;          
	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	pathp=mymalloc(SRAMIN,511);
  pathp[0]=0;
	strcat((char*)pathp,(char*)path);
	memset(fnt,0,200);
	if(demo==0)
	{
		demo=1;
		
		 pshu=mymalloc(SRAMIN,511);
				pshu[0]=0;
				mymemset(pshu,0,511);
			    	strcat((char*)pshu,(char*)path);
				strcat((char*)pshu,"/OSET.dat");
		f_unlink((char*)pshu);
		f_open(&fil3,(char*)pshu, FA_WRITE | FA_OPEN_ALWAYS);
		reco();
		f_close(&fil3);
		 myfree(SRAMIN,pshu);
	  f_close(&fil1);
		f_close(&fil2);
	 myfree(SRAMIN,pathtxt);
   myfree(SRAMIN,pathdata);
	}
	
//	strcpy((char*)fnt,(char*)path);

	
	
/*
	if(strcmp((char*)patht,(const TCHAR*)"0:"))
	{
	while(*patht!=0)
	{
	 patht++;
	 temp++;
	}
	strncat((u8*)fn1,(u8*)patht,temp);
	while(*patht!=0x5c)
		patht--;
	*patht='\0';
 }
 */
 /*
 p=strrchr((const char*)pathp,'/');
 for(j=0;path[(u8)p+j]!='\0';j++)
  fnt[j]=path[(u8)p+j];
	fnt[j]='\0';
	*/
	 p=strrchr((const char*)pathp,'/');
	 strcpy(fnt,p?p+1:pathp);
					if(p!=NULL)
					{
					  *p='\0';
						pathp[(u8)p]='\0';
						//strncpy((char*)pathp,(char*)pathp,(u8)p);
			    }
 
	//pathp=patht;//上级菜单
				//	Show_Str(300,20,200,16,patht,16,0);
		memset(path,0,200);
	strcpy((char*)path,(char*)pathp);
	res=f_opendir(dir,(const TCHAR*)pathp);
	if(res==0)
	{
	  while(1)
		{
		  res=f_readdir(dir,finfo);
		  fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
		  if(!finfo->fname[0]||res!=FR_OK) break;
			if(finfo->fattrib==AM_DIR)
			{
			   Show_Str(60,60+20*i,200,16,(u8*)fn,16,0);
				 if(!strcmp(fnt,fn))
					 k=i;
				 i++;
			}
		}
	curplace=k;
	mouse=curplace;
	highlight(mouse);
	}
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	myfree(SRAMIN,dir);
  myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
	myfree(SRAMIN,tetbl);
	return i;
}	


void k3_event(u8 n)
{
	curplace=(curplace+1)%n;
	mouse=curplace;
	highlight(mouse);
}
void k4_event()
{

			if(f_tell(&fil1)==f_size(&fil1))
			{	
				f_lseek(&fil2,0);
					f_lseek(&fil1,0);
			}
		P1=f_tell(&fil1);
	  P2=f_tell(&fil2);
			LCD_Clear(WHITE);
			f_gets((char*)buffer2,121,&fil2);
			f_gets((char*)buffer1,38,&fil1);
      play();
}

void k5_event()
{ 
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
	P1=f_tell(&fil1);
	P2=f_tell(&fil2);
			 LCD_Clear(WHITE);
			 f_gets(buffer2,121,&fil2);
			 f_gets(buffer1,38,&fil1);	
       play();
}

 void k6_event()
 {
	 FIL filo;
	 u8*pshuo=0;
	 DWORD P3,P4;
  pshuo=mymalloc(SRAMIN,511);
				pshuo[0]=0;
				mymemset(pshuo,0,511);
			    	strcat((char*)pshuo,(char*)path);
				strcat((char*)pshuo,"/OSET.dat");
	 	f_open(&filo,(char*)pshuo, FA_READ);
	     f_read(&filo, &P3, sizeof (P1), &br);
	   f_read(&filo, &P4, sizeof (P2), &br);
	 f_close(&filo);
	 myfree(SRAMIN,pshuo);
	 f_lseek(&fil1,P3);
    f_lseek(&fil2,P4);
	 	LCD_Clear(WHITE);
		
			 f_gets(buffer2,121,&fil2);
			 f_gets(buffer1,38,&fil1);
			
       play();
 }
void highlight(const u8 line)
{
	LCD_Fill(35,60,45,800,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,60+18*line,45,70+18*line,BLUE);
}

void dis_txt()
 {
	f_lseek(&fil1,0);
	f_lseek(&fil2,0);
	f_gets(buffer2,121,&fil2);
	f_gets(buffer1,38,&fil1);
        play();
 }


void play()
{int m;
	int l;
	char btmp[20][7]={""};
	LCD_Clear(WHITE);
  	Show_Str(18,80,800,32,(u8*)buffer1,32,1);
	SPeech(buffer1);
	delay_ms(1);
	for(l=0;l<strlen(buffer2);l++)
	buffer2[l]=buffer2[l];
	buffer2[l]='\0';
m=strlen(buffer2)/6;
	    for(l=0;l<m;l++)
	    memcpy(btmp[l],buffer2+6*l,6);
		LCD_WShowBraille(m,30,240,btmp);
	  DX_FCBraille(btmp);
	memset(btmp,'\0',sizeof(btmp));
}
void reco()
{
 f_write(&fil3,&P1, sizeof(P1), &bw);
 f_write(&fil3,&P2, sizeof(P2), &bw);
}






