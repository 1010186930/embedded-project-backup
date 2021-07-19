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
#include<stdlib.h>
#include<string.h>
#define MAXSIZE 10//数组大小基数
#define MAXBK 10//书签数目
const char ext[]=".c";//书签的扩展名
void k3_last(u8 n);
void move_Bookmarkcursor_Last(u16 n);
void selectSort(char(*pt)[50], int n);
void select_Bookmark(u16 index);
void move_Bookmarkcursor_Next(u16 n);
FRESULT bookmark_Read(const u8*path,u8*buf_Read);
int display_Bookmark();
u32 path_Change(u32 cnt1,u16 cnt2);
FRESULT create_New(const u8*path,const u8*path2,UINT nums);
FRESULT times_Init(const u8*path);
void Int_To_Str(int x, char* Str);//十进制转换成字符串
u32 times_Read(const u8*path);
FRESULT times_Write(const u8*path);
u16 scan_Bookmark (const u8* path,int num);//获取书签的数目并且将要替换的书签名赋值给nameReplaced
void findExt( char* file_name, char* name, char* ext);
u8 s_files(u8*);
void highlight( u8 );
int mouse=0;
int countdown;
int flag=0;
u8 k1_event(u8);
u8 k2_event();
void k3_event(u8);
int curplace=0;
u16 bkplace=0;//书签的位置信息
u8 buf_Read[100]="";//存放书签读到的地址
u8 path[200]={""};//全局变量，存放当前地址
u8 nameReplaced[100]="";//要替换的书签名
u8*pshu=0;
void k4_event();
void k5_event();
void k6_event();
char buffer1[38];
char buffer2[121]="";
FIL fil1,fil2;//fil3删了
u8*pathtxt=0;
u8*pathdata=0;
int demo=1;
void dis_txt();
void play();
void reco();
DWORD P1,P2;//P1是text的位置，P2是data的位置
DWORD P3,P4;//P3是读到的text的位置，P4是读到的data的位置
u8 path_c[6*MAXSIZE]="";//创建的地方	

FRESULT res;
BYTE buffer[20*MAXSIZE]="";//按照gb2312编码，一个汉字两个字节
char filename[50][50]={{""}};//存放fn的二维数组
char midname[50]="";//需要发声的数组
char bookmarkname[10][50]={{""}};//存放书签名的二维数组
char bkmidname[50]="";//修改后需要发声的数组
int main()
{
	
	
	FRESULT res1;
	int key_v;
	u8 menu=0;
	int n_files;
	int bk_nums;
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
	TIM3_Int_Init(99,7199);//0.01s	
	times_Init("0:/nums.txt");
//	n_files=display_Bookmark("0:");
	n_files=s_files("0:");
	strcpy((char*)path,"0:");

//写书签（dir 和 fil都不是内存分配出来的）
//	while(1)
//	{
//		key_v=KEY_Scan(0);
//		if(key_v==1&&demo==1)
//		{
//			mouse=0;
//		  n_files=k1_event(curplace);
//		}
//		else if(key_v==2&&demo==0)
//		{
//			n_files=k2_event();
//			cnt1=times_Read("0:/num.txt");
//		  cnt2=scan_Bookmark("0:");//已有书签的个数
//			path_Change(cnt1,cnt2);
//			res1=create_New((const u8*)path_c,buffer,cnt3);
//			if(res1==FR_OK)
//				times_Write("0:/num.txt");
//			demo=1;
//		}
//		else if(key_v==2&&demo==1)
//		{
//			n_files=k2_event();
//		}
//		else if(key_v==3&&demo==1)
//		{
//		 k3_event(n_files);
//		}
//	}

//写书签（dir和fil通过分配内存）
//while(1)
//	{
//		key_v=KEY_Scan(0);
//		if(key_v==1&&demo==1)
//		{
//			mouse=0;
//		  n_files=k1_event(curplace);
//		}
//		else if(key_v==2)
//		{
//			n_files=k2_event();
//		}
//		else if(key_v==3&&demo==1)
//		{
//		 k3_event(n_files);
//		}
//	}

//读书签
//while(1)
//	{
//		key_v=KEY_Scan(0);
//		if(key_v==1&&demo==1)
//		{
//			mouse=0;
//		  n_files=k1_event(curplace);
//		}
//		else if(key_v==2)
//			n_files=k2_event();
//		else if(key_v==3&&demo==1)
//		 k3_event(n_files);
//		else if(key_v==4)
//		  k4_event();
//    else if(key_v==5)
//		  k5_event();
//		else if(key_v==6)
//		{
//			bk_nums=display_Bookmark();
//		}
//		else if(key_v==1&&demo==2)
//		{
//			select_Bookmark(bkplace);
//		}
//		else if(key_v==3&&demo==2)
//		 move_Bookmarkcursor_Next(bk_nums);
//	}
//	return 0;
//}
//按键重排
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
//		else if(key_v==4)
//		  k4_event();
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
		LCD_LED=0;
		flag=1;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,DISABLE);
	}
}
return 0;
}
//while(1)
//{
//	int i=0;
//	for(	 i=0;i<14;i++)

//	Show_Str(60,32+32*i,200,32,"123浙江理工大学",32,0);
//}
//return 0;
//}


//读书签测试
//	while(1)
//	{
//		key_v=KEY_Scan(0);
//		if(key_v==1)
//		{
//			bk_nums=display_Bookmark();
//		}
//		else if(key_v==2&&demo==2)
//		{
//			select_Bookmark(bkplace);
//		}
//		else if(key_v==3&&demo==2)
//		{
//			 move_Bookmarkcursor(bk_nums);
//		}
//		
//	}
//	return 0;
//}
////			cnt1=times_Read("0:/num.txt");
//			printf("%d\r\n",cnt1);
//			LCD_Clear(WHITE);
//			display_Bookmark("0:");
//		}
//		else if(key_v==2)
//		{
//			times_Write("0:/num.txt");
//			bookmark_Read("0:/bookmark2.c",buf_Read);
//			printf("记录的内容%s\r\n",buf_Read);
//		}
//		else if(key_v==3)
//		{
//			printf("创建之前书签创建的次数  %d\r\n",cnt1);
//			cnt2=scan_Bookmark("0:");//已有书签的个数
//			printf("创建之前书签的个数     %d\r\n",cnt2);
//			path_Change(cnt1,cnt2);
//			res1=create_New((const u8*)path_c,buffer,cnt3);
//			if(res1==FR_OK)
//				times_Write("0:/num.txt");
//			else
//				printf("创建失败\r\n");
//			cnt1=times_Read("0:/num.txt");
//			printf("创建之后书签创建的次数  %d\r\n",cnt1);
//			cnt2=scan_Bookmark("0:");//已有书签的个数
//			printf("创建之后书签的个数     %d\r\n",cnt2);
			
			
//			cnt2=scan_Bookmark("0:");//已有书签的个数
//			printf("创建之前书签的个数     %d\r\n",cnt2);
//			path_Change(cnt1,cnt2);
//			res1=create_New((const u8*)path_c,buffer,cnt3);
//			if(res1==FR_OK)
//				times_Write("0:/num.txt");
//			else
//				printf("创建失败\r\n");
//			LCD_Clear(WHITE);
//			s_files("0:/01盲文基础学习");
//		}
//	}
	

//师姐的操作
//		while(1)
//	{
//	  key_v=KEY_Scan(0);
//		if(key_v==1&&demo==1)
//		{
//			mouse=0;
//		  n_files=k1_event(curplace);
//		}
//		 else if(key_v==3&&demo==1)
//			 k3_event(n_files);
//		else if(key_v==2)
//		  n_files=k2_event();
//	  else if(key_v==4&&demo==0)
//		  k4_event();
//    else if(key_v==5&&demo==0)
//		    k5_event();
//		else if(key_v==6&&demo==0)
//		    k6_event();
//		if(countdown==30000)
//		{
//			LCD_LED=0;
//			flag=1;
//		}
//		if(flag==1&&KEY_Scan(0)!=0)
//		{
//			countdown=0;
//			LCD_LED=1;
//		}
//	}
//	return 0;
//}


u8 s_files(u8*path)
{
	u8 res=0;
	DIR*dir=0;
	FILINFO*finfo=0;
	u8*fn=0;
	u8*pathp=0;
	u8 va;
	int i=0;
	u32 num1=5;
	u32 num2=6;
	BYTE buffer[20*MAXSIZE]="";
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
				strcpy((u8*)filename[i],(u8*)fn);
				Show_Str(60,32+32*i,500,32,(u8*)fn,32,0);
				i++;
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
	u8*tetbl;
	u8*pathp=0;
	u8 va;
	int i=0;
	int k=0;
	u8 red=0;
	u8 ret=0;
  LCD_Clear(WHITE);
	SPeech("[i1][n1]");
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
				strcpy((u8*)filename[i],(u8*)fn);
				Show_Str(60,32+32*i,500,32,(u8*)fn,32,0);
				i++;
				if(i==1)
				{
					memset(midname,'\0',sizeof(midname));
					strncpy(midname,filename[0]+2,strlen(filename[0])-2);
					SPeech(midname);
				}
			}
			else
			{
				demo=0;
				break;
			}
		}
	}
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
		if(red==FR_OK&&ret==FR_OK)
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
	char*p=0;
	char*q=0;
	int i=0;
	int k;
	u8 fnt[100]={""};
	u8 bkname[100]={""};
	u32 cnt1;//创建书签的次数
  u16 cnt2;//已有书签的个数
  u32 cnt3;//要写入文本地址的长度
	int index;//要新建的书签编号
	LCD_Clear(WHITE);
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	finfo->lfsize=_MAX_LFN*2+1;          
	finfo->lfname= mymalloc(SRAMIN,finfo->lfsize);
	pathp=mymalloc(SRAMIN,511);
  pathp[0]=0;
	strcat((char*)pathp,(char*)path);
	memset(fnt,0,100);
	memset(bkname,0,100);//文档所属的名称
	memset(path_c,0,6*MAXSIZE);
	memset(buffer,0,20*MAXSIZE);//要写入地址的路径
	if(demo==0)
	{
		f_close(&fil1);
		f_close(&fil2);
		myfree(SRAMIN,pathtxt);
		myfree(SRAMIN,pathdata);
		strcpy((char*)buffer,(const char*)path);
		cnt3=strlen((const char*)buffer);
		cnt1=times_Read("0:/nums.txt");
		cnt2=scan_Bookmark("0:",-1);//已有书签的个数
		index=path_Change(cnt1,cnt2);
		scan_Bookmark("0:",index);
		if(*nameReplaced!=NULL)
		{
			f_unlink((char*)nameReplaced);
		}
		q=strrchr((const char*)pathp,'/');
		strcpy((char*)bkname,q?q+3:pathp);//把数字和'/'去了
		strcat(path_c,bkname);
		strcat(path_c,ext);
		res=create_New((const u8*)path_c,buffer,cnt3);
		if(res==FR_OK)
			times_Write("0:/nums.txt");
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
			  Show_Str(60,32+32*i,500,32,(u8*)fn,32,0);
				if(!strcmp(fnt,fn))
					k=i;
				i++;
			}
		}
		memset(midname,'\0',sizeof(midname));
		strncpy(midname,filename[k]+2,strlen(filename[k])-2);
		SPeech(midname);
		curplace=k;
		mouse=curplace;
		highlight(mouse);
	}
	f_closedir(dir);
	myfree(SRAMIN,finfo->lfname);
	myfree(SRAMIN,dir);
  myfree(SRAMIN,finfo);
	myfree(SRAMIN,pathp);
	return i;
}	


void k3_event(u8 n)
{
	curplace=(curplace+1)%n;
	mouse=curplace;
	highlight(mouse);
	memset(midname,'\0',sizeof(midname));
	delay_ms(1);
	strncpy(midname,filename[curplace]+2,strlen(filename[curplace])-2);
	SPeech(midname);
}
void k3_last(u8 n)
{
	if((curplace-1)<0)
		curplace=n-1;
	else
		curplace=(curplace-1)%n;
	mouse=curplace;
	highlight(mouse);
	memset(midname,'\0',sizeof(midname));
	delay_ms(1);
	strncpy(midname,filename[curplace]+2,strlen(filename[curplace])-2);
	SPeech(midname);
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
void highlight(const u8 line)
{
	LCD_Fill(35,45,45,1000,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,45+32*line,45,55+32*line,BLUE);
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
{
	int m;
	int l;
	char btmp[20][7]={""};
	LCD_Clear(WHITE);
  	Show_Str(18,80,800,32,(u8*)buffer1,32,1);
	SPeech("[i1][n1]");
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
//void reco()
//{
// f_write(&fil3,&P1, sizeof(P1), &bw);
// f_write(&fil3,&P2, sizeof(P2), &bw);
//}

u16 scan_Bookmark (const u8* path,int num)//获取书签的数目并且将要替换的书签名赋值给nameReplaced
{
	u16 cnt=0;
	FRESULT res;
	DIR *dir;
	FILINFO *fno;
	u8*fn;
	char buf[10];//存储fn
	char numtostr[5]="";
	memset(nameReplaced,0,100);
	memset(nameReplaced,0,50);
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
				if(res!=FR_OK||fno->fname[0]==0)break;	//错误了/到末尾了,退出		   
					res=f_typetell(fn);	
				if((res&0XFF)==0X31)//取高四位,看看是不是.C书签文件
					{
						strncpy(buf,fn,6);
						if(num==MAXBK)
						{
							sprintf(numtostr, "%d", num);
							if(strstr(buf,"10"))
							{
								strcpy(nameReplaced,"0:/");
								strcat(nameReplaced,fn);
							}
						}
						else if(num>0)
						{
							sprintf(numtostr, "%d", num);
							if(strstr(buf,numtostr)!=NULL&&strstr(buf,"0")==NULL)
							{
								strcpy(nameReplaced,"0:/");
								strcat(nameReplaced,fn);
							}
						}
						cnt++;//有效文件数增加1
					}								
			 }  
		} 
	f_closedir(dir);
	myfree(SRAMIN,fno->lfname);
	myfree(SRAMIN,dir);
	myfree(SRAMIN,fno);
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
}
u32 times_Read(const u8*path)//读 创建次数.txt获取创建书签的次数
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
FRESULT times_Write(const u8*path)//写 创建次数.txt，增加创建书签的次数，写入成功后返回值为0
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
FRESULT times_Init(const u8*path)//初始化 创建次数.txt，初始值为0，写入成功后返回值为0
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
void Int_To_Str(int x, char* Str)
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
		res=f_open(fil,(const TCHAR*)path,FA_READ|FA_WRITE|FA_CREATE_ALWAYS);//可以新建写入，也可以已有文件覆盖写入
		res = f_write(fil,&P1,sizeof(P1),&bw);
		res = f_write(fil,&P2,sizeof(P2),&bw);
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
	return res;	
}
u32 path_Change(u32 cnt1,u16 cnt2)//cnt1是创建书签的次数，cnt2是已有书签个数
{
	char idx[MAXSIZE]="";
	char bk[3*MAXSIZE]="书签";
	char *str=idx;
	memset(path_c,0,6*MAXSIZE);
	strcpy((char*)path_c,"0:/");
	if(cnt2<=MAXBK-1)
	{
		cnt2++;
		Int_To_Str((int)cnt2, str);
		strcat(bk,str);
		strcat(bk," ");
		strcat(path_c,bk);
		return 0;
	}
	else if(cnt2==MAXBK)
	{
		if ((cnt1 + 1) % (MAXBK) == 0)
		{
			Int_To_Str(MAXBK, str);
			strcat(bk, str);
			strcat(bk," ");
			strcat(path_c, bk);
			return MAXBK;
		}
		else
		{
			cnt1 = (cnt1 + 1) % (MAXBK);
			Int_To_Str((int)cnt1, str);
			strcat(bk, str);
			strcat(bk," ");
			strcat(path_c, bk);
			return cnt1;
		}
	}
}
int display_Bookmark()
{
	FRESULT res;
	int i=0;
	int line=0;
	int num=0;//书签个数
	DIR *bkdir;
	FILINFO *bkfileinfo;//文件信息
	u8 *fn;   			//长文件名
	char processedname[50]="";//文件名不带扩展名
	char extension[3]="";
	memset(processedname,0,50);
	memset(&bookmarkname[0][0],0,sizeof(char)*10*50);
	LCD_Clear(WHITE);
	bkdir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
  bkfileinfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
	bkfileinfo->lfsize=_MAX_LFN*2+1;          
	bkfileinfo->lfname= mymalloc(SRAMIN,bkfileinfo->lfsize);
	demo=2;
	bkplace=0;//记录读到书签所处目录的位置
	highlight(0);
	memset(path,0,200);
	strcpy((char*)path,"0:");
//	num=scan_Bookmark("0:");
//	if(num==0)
//	{
//		LCD_Clear(WHITE);
//		Show_Str(60,242,500,42,"没有书签文件！",32,0);
//		SPeech("没有书签文件");
//	}
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
				memset(processedname,0,50);
			}	    
		}
		selectSort(bookmarkname, line); 
		for(i=0;i<line;i++)
		Show_Str(60,32+32*i,500,32,(u8*)bookmarkname[i],32,0);
		memset(bkmidname,'\0',sizeof(bkmidname));
		strcpy(bkmidname,bookmarkname[0]);
		SPeech(bkmidname);
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
	memset(buf_Read,0,100);
	P3=0;
	P4=0;
	res=f_opendir(dir,"0:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_READ);//可以新建写入，也可以已有文件覆盖写入
		res = f_read(fil,&P3,sizeof(P3),&br);
		res = f_read(fil,&P4,sizeof(P4),&br);
		printf("第一个数字%d\r\n",(int)P3);
		printf("第二个数字%d\r\n",(int)P4);
		for(;;)
		{
			res = f_read(fil,buf_Read,100,&br);
			printf("%s\r\n",buf_Read);
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
{
//	FRESULT res=0;
//	DIR*dir=0;
//	FILINFO*finfo=0;
//	u8*fn=0;
	u8*pathp=0;
	int i=0;
	u8 red=0;
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
	  if(red==FR_OK&&ret==FR_OK)
		{
		  f_lseek(&fil1,P3);
		  f_lseek(&fil2,P4);
		  f_gets(buffer2,121,&fil2);
		  f_gets(buffer1,38,&fil1);
		  play();
	 	  P3=0;
		  P4=0;
		}
	}
}
void move_Bookmarkcursor_Next(u16 n)
{
	u16 cursor;
	SPeech("[i1][n2]");
	bkplace=(bkplace+1)%n;
	cursor=bkplace;
	highlight(cursor);
	memset(bkmidname,'\0',sizeof(bkmidname));
	delay_ms(1);
	strcpy(bkmidname,bookmarkname[bkplace]);
	SPeech(bkmidname);
}
void move_Bookmarkcursor_Last(u16 n)
{
	u16 cursor;
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
	SPeech(bkmidname);
}
void selectSort(char(*pt)[50], int n)  //形参pt为指向二维字符数组的指针
{
	int i, j, k;
	char tmp[50];  //临时字符数组
	for (i = 0; i < n - 1; i++)  //遍历所有未排序的字符串的趟数
	{
		k = i;  //变量k存放当前最小字符串的下标
		for (j = i + 1; j < n; j++)               //遍历下标从(i+1)~(n-1)的所有字符串
			if (strcmp(pt[k], pt[j]) > 0)  //字符串进行两两大小比较,如果>0,则执行if语句体
				k = j;                     //将当前最小的字符串的数组下标赋值给变量k
		if (k != i)                       //如果k不等于i,则执行if语句体
		{   //两个字符串进行交换操作
			strcpy(tmp, pt[k]);
			strcpy(pt[k], pt[i]);
			strcpy(pt[i], tmp);
		}
	}
	if(n==10)
	{
		strcpy(tmp, pt[0]);
		strcpy(pt[0], pt[1]);
		strcpy(pt[1], tmp);
		for (i = 0; i < n-1; i++)
		{
			strcpy(tmp, pt[i]);
			strcpy(pt[i], pt[i+1]);
			strcpy(pt[i+1], tmp);
		}
	}
}
	
	
	