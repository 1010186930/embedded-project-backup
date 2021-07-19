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
#include "timer.h"
#include "StringProcess.h"
#include "touni.h"
#include "BrailleUSART.h"
#include "BrailleProtocol.h"

#include<stdlib.h>
#include<string.h>
#define MAXSIZE 10//数组大小基数
#define MAXBK 10//书签数目
#define MAXDR 10//单页显示最多的目录数目
#define MAX_ITEM 10
#define MAX_BYTE 50
const char yue[]="2 3 4 5 6点  韵母 üē               ";
const char yue3[]="üē  üě  üè                        ";
const char yuan[]="1 2 3 4 6点 韵母 üān               ";
const char yuan3[]="韵母  üān   ǖn   iōng            ";
const char yue6[]="韵母 uā  uāi  uī  uō  üē  ér  ";
const char guo[]="字字标调 guó                        ";
const char jia[]="字字标调 jiā                        ";
const char guojia[]="字字标调 guó jiā                   ";
const char quzi[]="轻声不标调 qǔ zi                    ";
const char ext[]=".c";//书签的扩展名
void bookmarkRename(char *str);
void NoBlankSpace(char* pStr);//删去字符串中的空格
void selectSort_ascending(char(*pt)[100], int n);
void Rename_Bookmark (const u8* path);
void k3_last(u8 n);//目录端和文本端的上一个
void move_Bookmarkcursor_Last(u16 n);//选择下一个书签
void selectSort(char(*pt)[100], int n);//书签排序函数
void select_Bookmark(u16 index);//选择要打开的书签，和确认键的功能类似
void move_Bookmarkcursor_Next(u16 n);//选择下一个书签
FRESULT bookmark_Read(const u8*path,u8*buf_Read);
int display_Bookmark();//遍历出所有的.c文件并且按照书签1~10的顺序排序
u32 path_Change(u16 bknums);
FRESULT create_New(const u8*path,const u8*path2,UINT nums);//path是文件的路径，path2是写入的字符数组,先写text再写data最后写文本存在的路径
FRESULT times_Init(const u8*path);//初始化 创建次数.txt(nums.txt)，初始值为0，写入成功后返回值为0
void Int_To_Str(int x, char* Str);//十进制转换成字符串
u32 times_Read(const u8*path);//读 创建次数.txt(nums.txt)获取创建书签的次数
FRESULT times_Write(const u8*path);//写 创建次数.txt(nums.txt)，增加创建书签的次数，写入成功后返回值为0
u16 scan_Bookmark (const u8* path,int num);//获取书签的数目并且将要替换的书签名赋值给nameReplaced
void findExt( char* file_name, char* name, char* ext);//得出文件名的扩展名和文件名
u8 s_files(u8*);
void highlight( u8 );
int mouse=0;
int countdown;
int flag=0;
int isTransRight=0;
u8 k1_event(u8);
u8 k2_event();
void k3_event(u8);
int curplace=0;
u16 bkplace=0;//书签的位置信息
u8 buf_Read[20*MAXSIZE]="";//存放书签读到的地址
u8 path[200]={""};//全局变量，存放当前地址
u8 nameReplaced[100]="";//要替换的书签名
u8*pshu=0;
void k4_event();
void k5_event();
void k6_event();
char buffer1[19]="";
char buffer2[121]="";
FIL fil1,fil2;//fil3删了
u8*pathtxt=0;
u8*pathdata=0;
int demo=1;
void dis_txt();
void play();
void reco();
int utf8_play();
DWORD P1=0,P2=0;//P1是text的位置，P2是data的位置
DWORD P3=0,P4=0;//P3是读到的text的位置，P4是读到的data的位置
u8 path_c[10*MAXSIZE]="";//创建的地方	

FRESULT res;
BYTE buffer[20*MAXSIZE]="";//按照gb2312编码，一个汉字两个字节
char filename[50][50]={{""}};//存放fn的二维数组
char midname[50]="";//需要发声的数组
char bookmarkname[10][100]={{""}};//存放书签名的二维数组
char bkmidname[100]="";//修改后需要发声的数组
int main()
{
	FRESULT res1;
	int key_v;
	u8 menu=0;
	int n_files;
	int bk_nums;
	delay_init();	    	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
	uart_init(115200);	
	BraillieUSART_Init(115200);  	
	KEY_Init();				
	LCD_Init();//5510的width480 height800	
	W25QXX_Init();		
	my_mem_init(SRAMIN);	
	exfuns_init();				
	SUOCUN_init();
	font_init();
	f_mount(fs[0],"0:",1); 	
	POINT_COLOR=RED;
	TIM3_Int_Init(99,7199);//0.01s	
	TIM4_Init(40-1,7200-1);
	TIM2_Init(40-1,7200-1);
	times_Init("0:/nums.h");
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
		else if(key_v==2&&demo==1)
			 k3_event(n_files);//目录端下一个
		else if(key_v==3&&demo==1)
			k3_last(n_files);//目录端上一个
		else if(key_v==3&&demo==0)
			k5_event();//文本端上一页
		else if(key_v==2&&demo==0)
			 k4_event();//文本端下一页
		else if(key_v==5)
			n_files=k2_event();//上一级
		else if(key_v==6)
			bk_nums=display_Bookmark();//显示书签
		else if(key_v==1&&demo==2)
			select_Bookmark(bkplace);//书签端确认
		else if(key_v==3&&demo==2)
			move_Bookmarkcursor_Last(bk_nums);//书签端上一个
		else if(key_v==2&&demo==2)
			move_Bookmarkcursor_Next(bk_nums);//书签端下一个
		if(countdown==30000)
		{
			dis(0);
			LCD_LED=0;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,DISABLE);
		}
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
	int i=0;
	int j=0;//屏幕显示位置，行数变量
	char bufdisplay[50]="";
	SPeech("[d]");
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
	memset(&filename[0][0],0,sizeof(char)*50*50);
	highlight(0);
	strcat((char*)pathp,(const char*)path);
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
				strcpy((u8*)filename[i],(u8*)fn);
				i++;
			}
		}
		if(i>MAXDR)
		{
			for(j=0;j<MAXDR;j++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*j,800,42,(u8*)bufdisplay,32,0);
			}
			
		}
		else
		{
			for(j=0;j<i;j++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*j,800,42,(u8*)bufdisplay,32,0);
			}
		}
		memset(midname,'\0',sizeof(midname));
		strncpy(midname,filename[0]+2,strlen(filename[0])-2);
		
		SPeech(midname);
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
	u8*pathp=0;
	int i=0;
	int j=0;
	u8 red=0;
	u8 ret=0;
	char bufdisplay[50]="";
	LCD_Clear(WHITE);
	SPeech("[i1][n2][s3]");
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	finfo->lfsize=_MAX_LFN*2+1;          
	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	pathp=mymalloc(SRAMIN,511);
	pathp[0]=0;
	strcat((char*)pathp,(char*)path);
	memset(path,0,200);
	memset(&filename[0][0],0,sizeof(char)*50*50);
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
				if(i==curfiles) break;
				i++;
			}
		}
	}
	strcat((char*)pathp,"/");
	strcat((char*)pathp,(char*)fn);
	strcpy((char*)path,(char*)pathp);
	f_closedir(dir);
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
				strcpy((u8*)filename[i],(u8*)fn);
				i++;
			}
			else
			{
				demo=0;
				break;
			}
		}
		if(demo==1)
		{
			if(i>MAXDR)
			{
				for(j=0;j<MAXDR;j++)
				{
					BufDisplay(filename[j]+2,bufdisplay);
					Show_GBKStr(60,32+42*j,800,42,(u8*)bufdisplay,32,0);
				}
			}
			else
			{
				for(j=0;j<MAXDR;j++)
				{
					BufDisplay(filename[j]+2,bufdisplay);
					Show_GBKStr(60,32+42*j,800,42,(u8*)bufdisplay,32,0);
				}
			}
			memset(midname,'\0',sizeof(midname));
			strncpy(midname,filename[0]+2,strlen(filename[0])-2);
			if(!strcmp(midname,"第一课 标调基本规则"))
				SPeech("第一课 标掉基本规则");
			else
				SPeech(midname);
		}
	}
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	fn=NULL;
	myfree(SRAMIN,dir);
	dir=NULL;
	myfree(SRAMIN,finfo);
	finfo=NULL;
	myfree(SRAMIN,pathp);
	pathp=NULL;
	curplace=0;
	if(demo==1)
	{
	  highlight(mouse);
	}
	else if(demo==0)
	{
		pathtxt=mymalloc(SRAMIN,511);
		mymemset(pathtxt,0,511);
		strcat((char*)pathtxt,(char*)path);
		strcat((char*)pathtxt,"/text.txt"); 
		ret=f_open(&fil1,(char*)pathtxt,FA_READ);
		P1=0;
		if(ret==FR_OK)
			dis_txt();
	}
	return i;
}

u8 k2_event()
{
	FRESULT res=0;
	DIR*dir=0;
	FILINFO*finfo=0;
	u8*fn=0;
	u8*pathp=0;
	u8*patht=0;
	char idx[3]="";
	char bk[10]="书签";
	char bufdisplay[50]="";
	char*p=0;
	char*q=0;
	int i=0;
	int l=0;//显示行数
	int j=0;//文件名数组的起始和终止位置
	int k=0;//父目录标志
	u8 fnt[2*MAX_BYTE]={""};
	u8 bkname[2*MAX_BYTE]={""};
	u32 cnt1;//创建书签的次数
	u16 cnt2;//已有书签的个数
	u32 cnt3;//要写入文本地址的长度
	int index;//要新建的书签编号
	char processedname[MAX_BYTE]="";//文件名不带扩展名
	char extension[3]="";
	memset(processedname,0,MAX_BYTE);
	SPeech("[i1][n1][s3]");
	LCD_Clear(WHITE);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	finfo->lfsize=_MAX_LFN*2+1;          
	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	memset(&filename[0][0],0,sizeof(char)*MAX_BYTE*MAX_BYTE);
	pathp=mymalloc(SRAMIN,511);
	pathp[0]=0;
	strcat((char*)pathp,(char*)path);
	memset(fnt,0,2*MAX_BYTE);
	memset(bkname,0,2*MAX_BYTE);//文档所属的名称
	memset(path_c,0,10*MAXSIZE);
	memset(buffer,0,20*MAXSIZE);//要写入地址的路径
	if(demo==0)
	{
		f_close(&fil1);
		myfree(SRAMIN,pathtxt);
		strcpy((char*)buffer,(const char*)path);
		cnt3=strlen((const char*)buffer);
		cnt2=scan_Bookmark("0:",-1);//已有书签的个数
		if(cnt2==MAXBK)
		{
			strcpy(path_c,"书签10 ");
			scan_Bookmark("0:",cnt2);
			if(*nameReplaced!=NULL)
			{
				res=f_unlink((char*)nameReplaced);
			}
			Rename_Bookmark("0:");
		}
		else if(cnt2>=0&&cnt2<=MAXBK-1)
		{
			sprintf(idx, "%d", cnt2+1);
			strcat(bk, idx);
			strcat(bk," ");
			strcat(path_c, bk);
		}
		q=strrchr((const char*)pathp,'/');
		strcpy((char*)bkname,q?q+3:pathp);//把数字和'/'去了
		strcat(path_c,bkname);
		strcat(path_c,ext);//把书签文件换成.c扩展名
		res=create_New((const u8*)path_c,buffer,cnt3);
		if(res==FR_OK)
			times_Write("0:/nums.h");
		q=NULL;
	}
	demo=1;	
	p=strrchr((const char*)pathp,'/');
	strcpy(fnt,p?p+1:pathp);
	if(p!=NULL)
	{
		*p='\0';
		//pathp[(u8)p]='\0';//?
	}
	p=NULL;
	if(!strcmp(fnt,pathp))
		k=0;
	memset(path,0,200);
	strcpy((char*)path,(char*)pathp);
	res=f_opendir(dir,(const TCHAR*)pathp);//打开修改过的路径
	if(res==0)
	{
	  while(1)
		{
		  res=f_readdir(dir,finfo);
		  fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
		  if(!finfo->fname[0]||res!=FR_OK) break;
			if(finfo->fattrib==AM_DIR)
			{
				strcpy((u8*)filename[i],(u8*)fn);
				if(!strcmp(fnt,fn))
					k=i;
				i++;
			}
		}
		if(i>MAXDR)
		{
			for(j=(k/MAXDR)*MAXDR;j<(k/MAXDR)*MAXDR+MAXDR;j++,l++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*l,800,42,(u8*)bufdisplay,32,0);
			}
		}
		else
		{
			for(j=0;j<i;j++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*j,800,42,(u8*)bufdisplay,32,0);
			}
		}
		memset(midname,'\0',sizeof(midname));
		strncpy(midname,filename[k]+2,strlen(filename[k])-2);
		SpecialSynthesis(midname);
		curplace=k;
		if(curplace>=MAXDR)
		{
			mouse=curplace%MAXDR;
			highlight(mouse);
		}
		else
		{
			mouse=curplace;
			highlight(mouse);
		}
	}
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	finfo=NULL;
	myfree(SRAMIN,dir);
	dir=NULL;
	myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
	pathp=NULL;
	return i;
}	


void k3_event(u8 n)  
{
	char bufdisplay[50]="";
	int j=0;
	int l=0;
	int curplace2;
	curplace=(curplace+1)%n;
	if(n>MAXDR)
	{
		if(curplace%MAXDR==0&&n>(curplace/MAXDR)*MAXDR+MAXDR)
		{
			LCD_Clear(WHITE);
			for(j=(curplace/MAXDR)*MAXDR;j<(curplace/MAXDR)*MAXDR+MAXDR;j++,l++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*l,800,42,(u8*)bufdisplay,32,0);
			}
			
		}
		else if(curplace%MAXDR==0&&n<=(curplace/MAXDR)*MAXDR+MAXDR)
		{
			LCD_Clear(WHITE);
			for(j=(curplace/MAXDR)*MAXDR;j<n;j++,l++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*l,800,42,(u8*)bufdisplay,32,0);
			}
		}	
	}
	curplace2=curplace%MAXDR;
	mouse=curplace2;
	highlight(mouse);
	memset(midname,'\0',sizeof(midname));
	delay_ms(1);
	strncpy(midname,filename[curplace]+2,strlen(filename[curplace])-2);
	SpecialSynthesis(midname);
}
void k3_last(u8 n)
{
	char bufdisplay[50]="";
	int j=0;
	int l=0;
	int curplace2;
	curplace=(curplace-1)%n;
	if(n>MAXDR)
	{
		if(curplace<0)
		{
			LCD_Clear(WHITE);
			curplace=n-1;
			for(j=(curplace/MAXDR)*MAXDR;j<n;j++,l++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*l,800,42,(u8*)bufdisplay,32,0);
			}
		}
		else if((curplace+1)%MAXDR==0&&(curplace+1)!=0)
		{
			LCD_Clear(WHITE);
			for(j=(curplace/MAXDR)*MAXDR;j<(curplace/MAXDR)*MAXDR+MAXDR;j++,l++)
			{
				BufDisplay(filename[j]+2,bufdisplay);
				Show_GBKStr(60,32+42*l,800,42,(u8*)bufdisplay,32,0);
			}
		}
	}
	else 
	{
		if(curplace<0)
			curplace=n-1;
	}
	curplace2=curplace%MAXDR;
	mouse=curplace2;
	highlight(mouse);
	memset(midname,'\0',sizeof(midname));
	delay_ms(1);
	strncpy(midname,filename[curplace]+2,strlen(filename[curplace])-2);
	SpecialSynthesis(midname);
}
void k4_event()
{
	if(isTransRight!=1)
	{
		P1=f_tell(&fil1)-18;
		isTransRight=utf8_play();
	}
	else
	{
		if(f_tell(&fil1)==f_size(&fil1))
		{	
			f_lseek(&fil2,0);
			f_lseek(&fil1,0);
		}
		P1=f_tell(&fil1);
		LCD_Clear(WHITE);
		f_gets((char*)buffer1,19,&fil1);
		isTransRight=utf8_play();
	}
}

void k5_event()
{ 
	if(isTransRight!=1)
	{
		P1=f_tell(&fil1)-18;
		isTransRight=utf8_play();
	}
	else
	{
		if(f_tell(&fil1)==f_size(&fil1))
		{
			if(f_size(&fil1)%18==0)
			{
				f_lseek(&fil1,f_size(&fil1)-36);	
			}
			else
			{
			  f_lseek(&fil1,f_size(&fil1)-f_size(&fil1)%18-18);	
			}		
		}			 
		else if(f_tell(&fil1)==18)
		{
			if(f_size(&fil1)%18==0)
			{
				f_lseek(&fil1,f_size(&fil1)-18);	
			}
			else
			{
				f_lseek(&fil1,f_size(&fil1)-f_size(&fil1)%18);	
			}
		}		
		else
		{				
			 f_lseek(&fil1,f_tell(&fil1)-36);	
		}
		P1=f_tell(&fil1);
		LCD_Clear(WHITE);
		f_gets(buffer1,19,&fil1);	
		isTransRight=utf8_play();
	}
  
}
void highlight(const u8 line)
{
	LCD_Fill(35,45,45,800,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,45+42*line,45,55+42*line,BLUE);
}

void dis_txt()
 {
	f_lseek(&fil1,0);
	f_gets(buffer1,19,&fil1);
	isTransRight=utf8_play();
 }


void play()
{
	int m;
	int l;
	char btmp[20][7]={""};
	char bufdisplay[38]="";
	LCD_Clear(WHITE);
	SPeech("[i1][n1]");
	delay_ms(10);
	BufDisplay(buffer1,bufdisplay);
	Show_GBKStr(18,80,800,32,(u8*)bufdisplay,32,1);
	if(strstr((const char*)path,"üē"))
	{
		if(!strcmp(buffer1,yue6))
		{
			SPeech("韵母 uā uāi uī uō vē ér");
		}	
		else if(!strcmp(buffer1,yue))
		{
			SPeech("2 3 4 5 6点  韵母 vē");
		}
		else if(strstr((const char*)buffer1,"üè"))
		{
			SPeech("vē vě vè");
		}
		else		
		SPeech(buffer1);
	}
	else if(strstr((const char*)path,"üān"))
	{
		if(!strcmp(buffer1,yuan))
		{
			SPeech("1 2 3 4 6点 韵母 vān");
		}	
		else if(strstr((const char*)buffer1,"üán"))
		{
			SPeech("vān ván van3 vàn");
		}
		else if(!strcmp(buffer1,yuan3))
		{
			SPeech("韵母 vān ǖn iōng");
		}
		else		
		SPeech(buffer1);
	}
	else if(strstr((const char*)path,"标调基本规则"))
	{
		if(!strcmp(buffer1,guo))
		{
			SPeech("字字标掉 guó");
		}	
		else if(!strcmp(buffer1,jia))
		{
			SPeech("字字标掉 jiā");
		}
		else if(!strcmp(buffer1,guojia))
		{
			SPeech("字字标掉 guó jiā");
		}
		else if(!strcmp(buffer1,quzi))
		{
			SPeech("轻声不标掉 qǔ zi");
		}
		else		
		SPeech(buffer1);
	}
	else
	{
		SPeech(buffer1);
	}	
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

u16 scan_Bookmark (const u8* path,int num)//获取书签的数目并且将要替换的书签名赋值给nameReplaced
{
	u16 cnt=0;
	FRESULT res;
	DIR *dir;
	FILINFO *fno;
	u8*fn;
	char buf[10];//存储fn
	memset(nameReplaced,0,100);
//	memset(nameReplaced,0,50);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fno =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	fno->lfsize=_MAX_LFN*2+1;          
	fno->lfname= mymalloc(SRAMIN,fno->lfsize); 
	res = f_opendir(dir, (const TCHAR*)path);
	if(res==FR_OK&&fno->lfname!=NULL)
		{
			while(1)//查询总的有效文件数
			{
				res=f_readdir(dir,fno);       		//读取目录下的一个文件
				fn=(u8*)(*fno->lfname?fno->lfname:fno->fname);				
				if(res!=FR_OK||fno->fname[0]==0)
					break;	//错误了/到末尾了,退出		   
				res=f_typetell(fn);	
				if((res&0XFF)==0X31)//取高四位,看看是不是.C书签文件
					{
						cnt++;//有效文件数增加1
						if(num==MAXBK)
						{
							memset(buf,'\0',sizeof(buf));
							strncpy(buf,(const char*)fn,6);
							if(strstr(buf,"1")!=NULL&&strstr(buf,"0")==NULL)
							{
								strcpy(nameReplaced,"0:/");
								strcat(nameReplaced,fn);
//								printf("要删除的书签名是%s\r\n",nameReplaced);
							}	
						}						
					}								
			 }  
		} 
	f_closedir(dir);
	myfree(SRAMIN,fno->lfname);
	fn=NULL;
	myfree(SRAMIN,dir);
	dir=NULL;
	myfree(SRAMIN,fno);
	fno=NULL;
	return cnt;
}
void findExt( char* file_name, char* name, char* ext)
{
		char* end;
		char* ptr = file_name;
		while (*ptr != '\0')
		{
			ptr++;//移到文件名末尾
		}
		end = ptr;//存一下
		while(*ptr!='.')
		{
			ptr--;//从后往前移动到.的位置
		}
		memcpy(name, file_name,  (ptr-file_name)*sizeof(char));
		memcpy(ext, ptr+1, (end - ptr ) * sizeof(char));
		end=NULL;
		ptr=NULL;
}
u32 times_Read(const u8*path)//读 创建次数.txt(nums.txt)获取创建书签的次数
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	u32 cnt=0;
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"0:");
	res=f_open(fil,(const TCHAR*)path,FA_READ);
	if(!res)
	{
		while(!res)
		{
			f_read(fil,&cnt,4,&br);
			if (res || br < 4) 
				break;
		}				
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,dir);
	myfree(SRAMIN,fil);
	return cnt;	
}
FRESULT times_Write(const u8*path)//写 创建次数.txt(nums.txt)，增加创建书签的次数，写入成功后返回值为0
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	u32 cnt=times_Read(path);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"0:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_READ|FA_WRITE);
		if(!res)
		{
			cnt=cnt+1;
			do
			{
				res = f_write(fil,&cnt,4,&bw);
				if(res)
					break;
			}
			while (bw <sizeof(cnt));
		}		
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,fil);
	myfree(SRAMIN,dir);
	return res;	
}
FRESULT times_Init(const u8*path)//初始化 创建次数.txt(nums.txt)，初始值为0，写入成功后返回值为0
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	u32 cnt=0;
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"0:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_CREATE_NEW|FA_WRITE);
			if(!res)
				res=f_write(fil,&cnt,4,&bw);
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,fil);
	myfree(SRAMIN,dir);
	return res;	
}
void Int_To_Str(int x, char* Str)//整数转成字符串
{
	int t;
	char* Ptr, Buf[5];
	int i = 0;
	Ptr = Str;
	if (x < 10)  // 当整数小于10，转换为0x格式 
	{
		*Ptr++ = x + 0x30;
	}
	else
	{
		while (x > 0)
		{
			t = x % 10;
			x = x / 10;
			Buf[i++] = t + 0x30; // 通过计算把数字编成ASCII码形式  
		}
		i--;
		for (; i >= 0; i--)   // 将得到的字符串倒序  
		{
			*(Ptr++) = Buf[i];
		}
	}
	*Ptr = '\0';
}
FRESULT create_New(const u8*path,const u8*path2,UINT nums)//path是文件的路径，path2是写入的字符数组,先写text再写data最后写文本存在的路径
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"0:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_READ|FA_WRITE|FA_CREATE_NEW);//可以新建写入，也可以已有文件覆盖写入
		res = f_write(fil,&P1,sizeof(P1),&bw);
		if(!res)
		{
			do
			{
				res = f_write(fil,path2,nums,&bw);
				if(res)
					break;
			}
			while (bw <nums);
		}		
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,fil);
	myfree(SRAMIN,dir);
	fil=NULL;
	dir=NULL;
	return res;	
}
u32 path_Change(u16 bknums)
/*cnt1是创建书签的次数，cnt2是已有书签个数 如果cnt2小于10个，把cnt2+1然后转换成字符串。
如果cnt2=10，此时若cnt1+1能整除10，就替换第10个书签。否则就替代第（cnt1+1）%10个书签
原理：如果有10个书签，cnt1必然为10.创建第11个书签相当于替换第（cnt1+1）%10个书签，如果cnt1+1为整除10的数，例如cnt1=19.创造次数为20
则替换的是第10个书签*/
{
	char idx[5]="";
	char bk[10]="书签";
//	char *str=idx;
	memset(path_c,0,10*MAXSIZE);
	strcpy((char*)path_c,"0:/");
	if(bknums<=MAXBK-1)
	{
		bknums++;
		sprintf(idx, "%d", bknums);
//		Int_To_Str((int)bknums, str);
		strcat(bk,idx);
		strcat(bk," ");
		strcat(path_c,bk);
		return 0;
	}
	else if(bknums==MAXBK)
	{
			sprintf(idx, "%d", bknums);
			strcat(bk, idx);
			strcat(bk," ");
			strcat(path_c, bk);
			return MAXBK;
	}
}
int display_Bookmark()//遍历出所有的.c文件并且按照书签1~10的顺序排序
{
	FRESULT res;
	int i=0;
	int line=0;
	int num=0;//书签个数
	DIR *bkdir;
	FILINFO *bkfileinfo;//文件信息
	u8 *fn;   			//长文件名
	char bufdisplay[100]="";
	char speechname[100]="";
	char processedname[100]="";//文件名不带扩展名
	char extension[3]="";
	memset(processedname,0,100);
	memset(speechname,0,100);
	memset(&bookmarkname[0][0],0,sizeof(char)*10*100);
	LCD_Clear(WHITE);
	SPeech("[i1][n2][s3]");
	bkdir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
  bkfileinfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	bkfileinfo->lfsize=_MAX_LFN*2+1;          
	bkfileinfo->lfname= mymalloc(SRAMIN,bkfileinfo->lfsize);
	demo=2;
	bkplace=0;//记录读到书签所处目录的位置
	highlight(0);
	memset(path,0,200);
	strcpy((char*)path,"0:");
	res=f_opendir(bkdir,(const char*)path);
	if(res==FR_OK)
	{
		while(1)//全部查询一遍
		{
			res=f_readdir(bkdir,bkfileinfo);//读取目录下的一个文件
			if(res!=FR_OK||bkfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出		  
			fn=(u8*)(*bkfileinfo->lfname?bkfileinfo->lfname:bkfileinfo->fname);			 
			res=f_typetell(fn);	
			if((res&0XFF)==0X31)//是否为书签文件	
			{
				findExt( (char*)fn, processedname, extension);
				strcpy((u8*)bookmarkname[line],processedname);
				line++;
				memset(processedname,0,100);
			}	    
		}
		selectSort(bookmarkname, line); 
		for(i=0;i<line;i++)
		{
			BufDisplay(bookmarkname[i],bufdisplay);
			Show_GBKStr(60,32+42*i,800,42,(u8*)bufdisplay,32,0);
		}
		
		memset(bkmidname,'\0',sizeof(bkmidname));
		strcpy(bkmidname,bookmarkname[0]);
		SpecialSynthesisInBk(speechname, bkmidname);
	}
	num=line;
	f_closedir(bkdir);
	myfree(SRAMIN,bkfileinfo->lfname);
	myfree(SRAMIN,bkdir);
	myfree(SRAMIN,bkfileinfo);
	return num;
}
FRESULT bookmark_Read(const u8*path,u8*buf_Read)//读书签内容，分为text的offset，data的offset，和书签记录的地址
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	memset(buf_Read,0,20*MAXSIZE);
	P3=0;
	P4=0;
	res=f_opendir(dir,"0:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_READ);//可以新建写入，也可以已有文件覆盖写入
		res = f_read(fil,&P3,sizeof(P3),&br);
		for(;;)
		{
			res = f_read(fil,buf_Read,20*MAXSIZE,&br);
			//printf("%s\r\n",buf_Read);//显示书签路径
			if(res||br<100)break;
		}
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,fil);
	myfree(SRAMIN,dir);
	return res;	
}
void select_Bookmark(u16 index)
/*i从0开始，和数组元素下标的起始位置相同，找到下标为index的元素，打开并且调用打开书签的函数，
将路径存到path中，方便打开path中的text.txt和data.txt*/
{
//	FRESULT res=0;
//	DIR*dir=0;
//	FILINFO*finfo=0;
//	u8*fn=0;
	u8*pathp=0;
	int i=0;
	u8 ret=0;
  LCD_Clear(WHITE);
	
//	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
//	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
//	finfo->lfsize=_MAX_LFN*2+1;          
//	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	pathp=mymalloc(SRAMIN,200);
  pathp[0]=0;
	strcat((char*)pathp,"0:");
	memset(path,0,200);
//	res=f_opendir(dir,(const TCHAR*)pathp);
//	if(res==0)
//	{
//	  while(1)
//		{
//			res=f_readdir(dir,finfo);
//		  fn=(u8*)(*finfo->lfname?finfo->lfname:finfo->fname);		
//		  if(!finfo->fname[0]||res!=FR_OK) break;
//		  res=f_typetell(fn);	
//		  if((res&0XFF)==0X31)//是否为书签文件	
//			{
//				if(i==index) break;
//				i++;
//			}
//		}
//	}
////	for (i = 0; i < 10; i++)
////		printf("%s\r\n", bookmarkname[i]);
	
	i=index;
	strcat((char*)pathp,"/");
	strcat((char*)pathp,(char*)bookmarkname[i]);
	strcat((char*)pathp,".c");
//	f_closedir(dir);
////printf("%s\r\n",pathp);
	i=0;
	res=bookmark_Read(pathp,buf_Read);
	if(res==0)
		strcpy((char*)path,(char*)buf_Read);
//	myfree(SRAMIN,finfo->lfname);
//	myfree(SRAMIN,dir);
//	myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
	 if(demo==2)
	{
		demo=0;
		pathtxt=mymalloc(SRAMIN,511);
		mymemset(pathtxt,0,511);
		strcat((char*)pathtxt,(char*)path);
		strcat((char*)pathtxt,"/text.txt"); 
		ret=f_open(&fil1,(char*)pathtxt,FA_READ);
	  if(ret==FR_OK)
		{
		  f_lseek(&fil1,P3);
		  f_gets(buffer1,19,&fil1);
		  utf8_play();
	 	  P3=0;
		  P4=0;
		}
	}
}
void move_Bookmarkcursor_Next(u16 n)//选择下一个书签
{
	u16 cursor;
	char speechname[100]="";
	memset(speechname,0,100);
	SPeech("[i1][n2]");
	bkplace=(bkplace+1)%n;
	cursor=bkplace;
	highlight(cursor);
	memset(bkmidname,'\0',sizeof(bkmidname));
	delay_ms(1);
	strcpy(bkmidname,bookmarkname[bkplace]);
	SpecialSynthesisInBk(speechname, bkmidname);
}
void move_Bookmarkcursor_Last(u16 n)//选择上一个书签
{
	u16 cursor;
	char speechname[100]="";
	memset(speechname,0,100);
	SPeech("[i1][n2]");
	if((bkplace-1)<0)
		bkplace=n-1;
	else
	bkplace=(bkplace-1)%n;
	cursor=bkplace;
	highlight(cursor);
	memset(bkmidname,'\0',sizeof(bkmidname));
	delay_ms(1);
	strcpy(bkmidname,bookmarkname[bkplace]);
	SpecialSynthesisInBk(speechname, bkmidname);
}
void selectSort(char(*pt)[100], int n) 
/*对书签1~10排序，书签名格式：书签X XXXXX,形参pt为指向二维字符数组的指针。书签1 XXX比书签10 XXX小，因为空格的ASCII码是20,0的ASCII码是30.
但汉字的ASCII比数字和符号的ASCII大。如果有书签10 XXX，最终结果为书签1 XXX,书签10 XXX，书签2 XXX.....*/
{
	int i, j, k;
	char tmp[100];  //临时字符数组
	for (i = 0; i < n - 1; i++)  //遍历所有未排序的字符串的趟数
	{
		k = i;  //变量k存放当前最小字符串的下标
		for (j = i + 1; j < n; j++)               //遍历下标从(i+1)~(n-1)的所有字符串
			if (strcmp(pt[k], pt[j]) < 0)  //字符串进行两两大小比较,如果>0,则执行if语句体
				k = j;                     //将当前最小的字符串的数组下标赋值给变量k
		if (k != i)                       //如果k不等于i,则执行if语句体
		{   //两个字符串进行交换操作
			strcpy(tmp, pt[k]);
			strcpy(pt[k], pt[i]);
			strcpy(pt[i], tmp);
		}
	}
	if (n == 10)//如果书签数目是10个，就进行如下交换：先交换10和1的顺序，然后依次交换1后面相邻的两个序号，把10交换到最后一位
	{
		/*strcpy(tmp, pt[0]);
		strcpy(pt[0], pt[1]);
		strcpy(pt[1], tmp);*/
		for (i = 9; i > 1; i--)
		{
			strcpy(tmp, pt[i-2]);
			strcpy(pt[i-2], pt[i - 1]);
			strcpy(pt[i - 1], tmp);
		}
	}
}
void Rename_Bookmark (const u8* path)//将书签文件的标号在磁盘中-1 例如书签10 XXX.c->书签9 XXX.c
{
	FRESULT res;
	u8 type;
	DIR *dir;
	FILINFO *fno;
	u8*fn;
	int i=0;
	int cnt=0;
//	char oldname[9][100]={{""}};
//  char newname[9][100]={{""}};
	char (*oldname)[100]=(char(*)[100])mymalloc(SRAMIN,sizeof(char)*9*100);
	char (*newname)[100]=(char(*)[100])mymalloc(SRAMIN,sizeof(char)*9*100);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fno =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	fno->lfsize=_MAX_LFN*2+1;          
	fno->lfname= mymalloc(SRAMIN,fno->lfsize); 
	res = f_opendir(dir, (const TCHAR*)path);
//	printf("打开目录结果：%d\r\n",res);
	if(res==FR_OK&&fno->lfname!=NULL)
		{
			while(1)//查询总的有效文件数
			{
				res=f_readdir(dir,fno);       		//读取目录下的一个文件
//				printf("读取目录结果:%d\r\n",res);
				fn=(u8*)(*fno->lfname?fno->lfname:fno->fname);				
				if(res!=FR_OK||fno->fname[0]==0)
					break;	//错误了/到末尾了,退出		   
				type=f_typetell(fn);	
//				printf("类型是%d\r\n",type);
				if((type&0XFF)==0X31)//取高四位,看看是不是.C书签文件
					{
							strcpy(oldname[i],"0:/");
							strcpy(oldname[i],fn);
//							printf("第%d个要改变的书签名是%s",i+1,oldname[i]);
							bookmarkRename((char*)fn);
							strcpy(newname[i],"0:/");
							strcpy(newname[i],fn);
//							printf(",第%d个新的书签名是%s\r\n",i+1,newname[i]);
							i++;
//						printf("数目i:%d\r\n",i);
					}						
			}	
				cnt=i;
		}
	selectSort_ascending(oldname, 9);
	selectSort_ascending(newname, 9);
  for(i=0;i<cnt;i++)
		{
			res=f_rename(oldname[i],newname[i]);
//			delay_ms(10);
//			if(!res)
//				printf("第%d个修改成功\r\n",i+1);
		}	
	f_closedir(dir);
	myfree(SRAMIN,fno->lfname);
	fn=NULL;
	myfree(SRAMIN,dir);
	dir=NULL;
	myfree(SRAMIN,fno);
	fno=NULL;
	myfree(SRAMIN,oldname);
	oldname=NULL;
	myfree(SRAMIN,newname);
	newname=NULL;
}
void bookmarkRename(char *str)//把书签X变成书签X-1 例如书签10 XXX.c->书签9 XXX.c
{
	char* p = str;
	char temp[3] = "";
	char str1[100] = "";
	char str2[100] = "";
	const char str3[] = "书签";
	char* q = NULL;
	int i = 0, j = -1;
	memset(str2, '\0', sizeof(str2) / sizeof(str2[0]));
	q = strchr(p, ' ');
	strcpy(str2, q);//书签名称
	q=NULL;
	strncpy(temp,p + 4, 2);
	NoBlankSpace(temp);
	j = atoi(temp);
//	printf("要替换的数字是%d\r\n",j);
	sprintf(temp, "%d", j - 1);
	memset(str1, '\0', sizeof(str1) / sizeof(str1[0]));
	strcat(str1, str3);
	strcat(str1, temp);
	strcat(str1, str2);
	strncpy(p, str1, strlen(str1));
	*(p+ strlen(str1)) = '\0';
	p=NULL;
}
void NoBlankSpace(char* pStr)//删去字符串中的空格
{
	char* pTmp = pStr;
	while (*pStr != '\0')
	{
		if (*pStr != ' ')
		{
			*pTmp = *pStr;
			pTmp++;
		}
		++pStr;
	}
	*pTmp = '\0';
}		
void selectSort_ascending(char(*pt)[100], int n)
/*对书签1~10由小到大排序，书签名格式：书签X XXXXX,形参pt为指向二维字符数组的指针。*/
{
	int i, j, k,z;
	char tmp[100];  //临时字符数组
	char num[3]="";
	char* p = NULL;
	int(*temp)[1] = (int(*)[1])mymalloc(SRAMIN,sizeof(int) * n);
	for (i = 0; i < n - 1; i++)  //遍历所有未排序的字符串的趟数
	{
		for (z = 0; z < n; z++)
		{
			p = pt[z];
			memcpy(num, p + 4, 2);
			NoBlankSpace(num);
			j = atoi(num);
			*temp[z] = j;
		}
		k = i;  //变量k存放当前最小字符串的下标
		for (j = i + 1; j < n; j++)               //遍历下标从(i+1)~(n-1)的所有字符串
			if (*temp[k] > * temp[j])  //字符串进行两两大小比较,如果>0,则执行if语句体
				k = j;                     //将当前最小的字符串的数组下标赋值给变量k
		if (k != i)                       //如果k不等于i,则执行if语句体
		{   //两个字符串进行交换操作
			strcpy(tmp, pt[k]);
			strcpy(pt[k], pt[i]);
			strcpy(pt[i], tmp);
		}
	}
	myfree(SRAMIN,temp);
	p = NULL;
}	
int utf8_play()
{
	int m;
    int l;
	u8 dot_cnt;
	int res=-1;
	int time_cnt=0;
    char btmp[20][7]= {""};
	u16 unibuf[100]={0x0000};
    LCD_Clear(WHITE);
    //SPeech("[i1][n2]");
    delay_ms(10);
    Show_UTF8Str(18,80,800,32,buffer1,unibuf,32,strlen(buffer1),0,0);
	memset(buffer2,'\0',sizeof(buffer2)/sizeof(char));
    /*serial communication*/
	SendRDY();
	if(Master_Status!=2)
		return 0;
	while(1)
	{
		printf("Master_Status Wait RDY\r\n");
		if(Master_Status==0)//4s内没收到消息
		{
			printf("Master_Status TimeOut\r\n");
			Show_GBKStr(18,200,800,32,"Master_Status TimeOut",32,0);
			return 0;
		}
		else if(Master_Status==1)//4s内收到消息了，解包
		{
			printf("Master_Status ASW\r\n");
			res=IsMasterRDY();
			printf("isReadyres:%d\r\n",res);
			break;
		}
		
//		if(res)
//			break;
//		time_cnt++;
//		if(time_cnt>1000000)
//			break;//等待超时
	}
	if(res==1)//MasterRDY
	{
		SendText(buffer1, strlen(buffer1));
		if(Master_Status!=2)
			return 0;
		while(1)
		{
			printf("Master WaitDot\r\n");
			if(Master_Status==0)//4s内没收到消息
			{
				printf("Master NASW\r\n");
				Show_GBKStr(18,200,800,32,"Master NO ANSWER",32,0);
				return 0;
			}
				
			else if(Master_Status==1)
			{
				printf("Master ASW\r\n");
				res=UnpackedDot(buffer2,&dot_cnt);
				printf("res:%d\r\n",res);
				if(res!=-1)
				{
					for(l=0; l<strlen(buffer2); l++)
					buffer2[l]=buffer2[l];
					buffer2[l]='\0';
					m=strlen(buffer2)/6;
					for(l=0; l<m; l++)
						memcpy(btmp[l],buffer2+6*l,6);
					LCD_WShowBraille(m,30,240,btmp);
					DX_FCBraille(btmp);
					memset(btmp,'\0',sizeof(btmp));
					return 1;
				}
				else
				{
					Show_GBKStr(18,200,800,32,"Master WRONG DATA",32,0);
					return 0;
				}	
			}
			
//			time_cnt++;
//			if(time_cnt>100000)
//				break;//等待超时
		}
		
	}
	/*
		1. 4s内没收到消息
		2. MasterNRDY
	*/
	else if(res==0)
	{
		printf("Master Not Ready\r\n");
		Show_GBKStr(18,200,800,32,"Master_Status:NOT READY",32,0);
		return 0;
	}
	else if(res==-1)
	{
		printf("Not The Right Answer\r\n");
		Show_GBKStr(18,200,800,32,"WRONG READY OR NOT ANSWER",32,0);
		return 0;
	}
		
}