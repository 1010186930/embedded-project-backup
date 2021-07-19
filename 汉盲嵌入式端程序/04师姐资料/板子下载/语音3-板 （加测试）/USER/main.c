#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
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

/************************************************

  汉盲阅读系统开发实验 
 by 		WangWenfei
    盲文学习 小说阅读
************************************************/
 
#define INITMENU 0
#define PRIMENU 1
#define NOVELMENU 2
#define POEMENU 3
#define PRIMODE 4
#define NOVELMODE 5
#define POEMODE 6
#define TITLEMENU 0
#define TITLEMODE 1
#define CATAMENU 7
  FIL  fil_0;   
 FIL  fil_1;
 FIL  fil_2;
 FIL  fil_3;
	char SDbtmp[10][13]={""};
		char FCbtmp[20][7]={""};	
 char shuru1[16];
 char shuru2[16];
 char SMbuf[200];	
 char PMbuf[200];			
 char  nbuf[200]=""; 
 char  BUF[121];
 DWORD P[100];
 //DWORD Ptext[100];	
 DWORD Pdata;
u32 Tqt[120];
u32 Trmd[120];
u32 Dqt[120];
	u32 Drmd[120];
 u8 list[120][70];
		u8 yuyin[120];
int cata_page=0;
	int cata_num=0;
int count_s,i;
	int s;
		int pin;
int j,back_flag=0,contents_back=0,read_back=0;	
		int chang;
 int page;
u8 filecnt,keys,keyi,keyf,keyt;//keyi 父级菜单导航栏标记  keyt 工测菜单导航栏标记  keyf sd文件目录栏导航标记  filecnt——sd文件个数 
u16 tlct=0;//tlct为文件定位指针		
u8 keyi=1,keyt=1,keyf=1,keys=1,Tkey=1;//菜单下导航标志变量，初始化为指向首行
	//u8 initmenu[4][23]={"欢迎使用盲文阅读系统","盲文基础学习","小说阅读","唐诗宋词"};		
	u8 initmenu[3][23]={"欢迎使用盲文阅读系统","盲文基础学习","小说阅读"};	
 u8 testmenu[7][23]={"声母韵母","音节声调","满方测试","英文字母"};	
 //u8 novelmenu[5][23]={"假如给我三天光明","汤姆索亚历险记","昆虫记","三天光明分词版","穴位按摩知识"};
  u8 novelmenu[2][23]={"假如给我三天光明","汤姆索亚历险记"};
 // u8 poemenu[5][23]={"登鹳雀楼","锦瑟","念奴娇"};
u16 emptmp[15]={0x00};
  u16 fultmp[15]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
char  Dirname[MAXDIR][30];	
void initUI(void);
void testUI(void);
void fildirUI(void);
void UIfun(u8 level);
void prim_learn(u8 line);
void novel_read(u8 line);
void poet_read(u8 line);	
void Dividtxt(const TCHAR* path1,const TCHAR* path2,const TCHAR* path3,const TCHAR* path4,const TCHAR* path5);
void Dis_primary(void);
void Page_turn(void);
	 void Dis_novel(u8 *str,u8 r1 , u8 r2);
 void NPage_turn(u8*str,const TCHAR* path,DWORD P1,DWORD P2,u8 r1,u8 r2,u8 r);
	void	CdlNum_Gets(int TLnum,const TCHAR* path1,const TCHAR* path2,const TCHAR* path3,const TCHAR* path4,const TCHAR* path5);
/* void Dis_novel(u8 *str);
 void NPage_turn(u8*str,const TCHAR* path,DWORD P1,DWORD P2);
	 void Dis_FCtest(u8 *str);
 void FCPage_turn(u8*str,const TCHAR* path,DWORD P1,DWORD P2); */
//光标移动函数
//光标移动函数
	void highlight(u8 line){
//			LCD_Fill(15,50,30,220,WHITE);//清除原先位置，白色填充	
//	LCD_Fill(15,30+20*line,30,40+20*line+8,BLUE);
					LCD_Fill(35,60,50,300,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,44+31*line,45,36+31*line+18,BLUE);
}
		void highlight2(u8 line){
//			LCD_Fill(15,50,30,220,WHITE);//清除原先位置，白色填充	
//	LCD_Fill(15,30+20*line,30,40+20*line+8,BLUE);
					LCD_Fill(35,60,50,300,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,44+38*line,45,36+38*line+18,BLUE);
}


		void highlight1(u8 line){

					LCD_Fill(35,40,50,300,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,40+38*line,45,32+38*line+18,BLUE);
}
		void T_highlight(u8 line){
			LCD_Fill(5,15,15,460,WHITE);//清除原先位置，白色填充	
	LCD_Fill(5,30.5*line,15,10+30.5*line,BLUE);
}
//		void T_highlight(u8 line){
//	LCD_Fill(35,10,50,20,WHITE);//清除原先位置，白色填充		
//			LCD_Fill(35,20*line,50,10+20*line,BLUE);
//}

//key0事件发生时，执行的函数
void e0_event_function(u8 *nxt_state){   
    u8 cur_state;   
	cur_state = *nxt_state;   
    switch(cur_state)
    {       
			case INITMENU: //处于0主菜单状态时 根据keyi切换状态界面   
					*nxt_state=keyi;
					UIfun(*nxt_state);//状态切换
							
		  
			break;
			case PRIMENU: //处于1状态时; 切换到末级菜单
					*nxt_state=PRIMODE; 
		    	UIfun(*nxt_state);//状态切换
				break;
			case NOVELMENU://处于2目录菜单，切换到文本阅读
					*nxt_state=NOVELMODE;
				   UIfun(*nxt_state);//	read_act(Dirname[keyf-1]);//20字节为单位阅读，定位的标志量加20，记忆功能
			cata_num=0;
				break;
}
}
//key1事件发生时，执行的函数,	向下导航按钮
void e1_event_function(u8 *nxt_state)
{   
    	
u8 cur_state;   
    cur_state = *nxt_state;   
    switch(cur_state)
    {       
        case INITMENU: //主菜单状态下的key1向下按键触发动作      
            //执行光标移动动作，状态无需切换
				//	keyi=keyi%3+1;//光标标志变量变化
							keyi=keyi%2+1;//光标标志变量变化
				  highlight2(keyi);
				           SPeech(initmenu[keyi]);   //调用发声函数，读出指定长度数据 

//while(TTS_Busy==1){};如果没有这句，则不需要连接busy这条线
 delay_ms(1);    //有停顿比较好，听起来无杂音                  
				  
				  break;
				case PRIMENU://子菜单1
				  keyt=keyt%4+1;
				  highlight(keyt);
					  
	 SPeech(testmenu[keyt-1]);   //调用发声函数，读出指定长度数据  
//while(TTS_Busy==1){};
 delay_ms(1);
				  break;
				case NOVELMENU://子菜单2
					keyf=keyf%2+1;
				  highlight(keyf);
						 SPeech(novelmenu[keyf-1]);   //调用发声函数，读出指定长度数据 
             delay_ms(1);
				  break;			
				case PRIMODE://工测模式下无变化
;
				break;
				case NOVELMODE://阅读模式下，实现翻页继续阅读动作
					;//read_act(Dirname[keyf-1]);
				  break;	
	
    }
}
//keyup 按键发生时，执行的函数
	void e2_event_function(u8 *nxt_state){
				u8 cur_state;   
				cur_state = *nxt_state; 
		//dis(0);
switch(cur_state)
{
	case PRIMENU:
	case NOVELMENU:
	//	case POEMENU:
		*nxt_state=INITMENU;
	  UIfun(*nxt_state);
	break;
	case INITMENU:
		;
	break;
	default:
		if(keyi==1)//根据末次的主菜单导航按钮keyf判别进入的是哪个子目录菜单
	//keyi=1这个是处于盲文基础阅读模式中
			
	  *nxt_state=PRIMENU;
		else{
		if(keyi==2){
			//keyi=2这个是处于小说阅读模式中
		  *nxt_state=NOVELMENU;
		}

		}

		UIfun(*nxt_state);
	break;
	}
}
	
u8 key_v;
int main(void)
{	
	
 	u8 state=0;
		delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		      //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为115200	
 	LED_Init();		  			//初始化与LED连接的硬件接口
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
		while(SD_Init())			//检测SD卡
	{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!check it");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			LED0=!LED0;//DS0闪烁
			delay_ms(200);		    
	}			
			
		UIfun(state);
		dis(0);
while(1){
		key_v=KEY_Scan(0);	
	if(key_v==1)
			e0_event_function(&state);
else if(key_v==4)
			e1_event_function(&state);
	else	if(key_v==5||back_flag==1||contents_back==1){
			dis(0);
		cata_page=0;cata_num=0;Tkey=1;
			back_flag=0;contents_back=0;read_back=0;
						e2_event_function(&state);	
}		
	}
}

void CdlNum_Gets(int TLnum,const TCHAR* path1,const TCHAR* path2,const TCHAR* path3,const TCHAR* path4,const TCHAR* path5){
	Tqt[0]=0;	Trmd[0]=0;
  Dqt[0]=0; Drmd[0]=0;
  	f_open(&fil_2,path1,FA_READ);	
			 for(i=1;i<TLnum;i++){
		f_read(&fil_2, &Tqt[i], sizeof (Tqt[i]), &br);
				 }
	f_close(&fil_2);
				 	f_open(&fil_2,path2,FA_READ);	
			 for(i=1;i<TLnum;i++)
					f_read(&fil_2, &Trmd[i], sizeof (Trmd[i]), &br);	
f_close(&fil_2);				 
	 f_open(&fil_2,path3,FA_READ);	
			 for(i=1;i<TLnum;i++){
	
				 f_read(&fil_2, &Dqt[i], sizeof (Dqt[i]), &br);
			 }
f_close(&fil_2);	
					 	f_open(&fil_2,path4,FA_READ);	
			 for(i=1;i<TLnum;i++)
					f_read(&fil_2, &Drmd[i], sizeof (Drmd[i]), &br);	
f_close(&fil_2);	
memset(list,'\0',sizeof(list));			 
	 f_open(&fil_2,path5,FA_READ);
		for(i=0;i<TLnum;i++)	{
	 f_gets(SMbuf,70,&fil_2);
			memcpy(list[i],SMbuf,sizeof(SMbuf));	
	}
	f_close(&fil_2);	
}

//目录菜单显示，总共的目录个数为 All_num，单页显示个数为 One_num
void Dis_Catamenu(int One_num,int All_num,const TCHAR* path1,const TCHAR* path2){
								 LCD_Clear(WHITE);	
								for(i=0;i<One_num;i++)
													Show_Str(20,30*(i+1),400,24,list[One_num*cata_page+i],24,1); 
								 T_highlight(Tkey);
					//	SPeech(list[cata_page*One_num+cata_num],sizeof(list[cata_page*One_num+cata_num]));  
	
	SPeech(list[cata_page*One_num+cata_num]);
	
            delay_ms(1);  
		while(1){
		key_v=KEY_Scan(0);	
	if(key_v==1){ 
		f_open(&fil_3,path1,FA_READ);
		f_open(&fil_1,path2,FA_READ);
		if(Tkey==cata_num+1){ 
s=One_num*cata_page+cata_num;			
  f_lseek(&fil_3,Tqt[s]*100+Trmd[s]);
	f_lseek(&fil_1,Dqt[s]*1000+Drmd[s]);	
			novel_read(keyf);
		if(read_back==1)
		{	 UIfun(NOVELMODE); }//每章节阅读完返回之后或从开头章节目录选择或从当前所在的章节目录选择
	}
		}
		else if(key_v==2){//章节目录选择键
						cata_num++; Tkey=Tkey%One_num+1;//光标标志变量变化
			 T_highlight(Tkey);
			//	Speech(list[cata_page*One_num+cata_num],sizeof(list[cata_page*One_num+cata_num]));
SPeech(list[cata_page*One_num+cata_num]);			
       delay_ms(1);
		//	}
				if(cata_num==One_num){  cata_page++; cata_num=0; LCD_Clear(WHITE);	T_highlight(Tkey);
							          	for(i=0;i<One_num;i++)
													Show_Str(20,30*(i+1),400,24,list[One_num*cata_page+i],24,1); }
			if(One_num*cata_page+cata_num==All_num){ Tkey=1; cata_page=0;cata_num=0;LCD_Clear(WHITE);	T_highlight(Tkey);
								          for(i=0;i<One_num;i++)
													Show_Str(20,30*(i+1),400,24,list[One_num*cata_page+i],24,1);}//选择到末章节则又从开头章节开始选择	
							 }
    else if(key_v==5||read_back==1)
		{				contents_back=1;read_back=0;
		
		break;		}
		}	
	}
//0级菜单和I,II，III级菜单的界面显示函数
void UIfun(u8 level){    
	          							LCD_Clear(WHITE);//界面切换前，清屏。
							POINT_COLOR=BLUE;

					switch(level){
						case 0://显示主菜单目录
							
						
						//	keyi=1;//对应光标标志变量初始化
							 SPeech(initmenu[keyi]);   //调用发声函数，读出指定长度数据 
             delay_ms(1);
						
							Show_Str(60,70,320,32,initmenu[1],32,1);
			    	 	Show_Str(60,110,320,32,initmenu[2],32,1);//for(u8 i=0;i<3;i++)
						  //Show_Str(60,150,320,32,initmenu[3],32,1);
						  highlight2(keyi);//状态切换后，光标位置必是位于首行
						break;
						case 1://显示工测菜单界面函数
							//keyt=1;
						 SPeech(testmenu[keyt-1]);   //调用发声函数，读出指定长度数据 
             delay_ms(1);
							Show_Str(60,70,500,24,testmenu[0],24,1);
							Show_Str(60,100,500,24,testmenu[1],24,1);
							Show_Str(60,130,500,24,testmenu[2],24,1);
 							Show_Str(60,160,500,24,testmenu[3],24,1);
						//	Show_Str(40,220,200,24,testmenu[0],24,1);
						  highlight(keyt);//状态切换后，光标位置必是位于首行
						break;
						case 2://显示SD卡文件目录菜单界面
						 SPeech(novelmenu[keyf-1]);   //调用发声函数，读出指定长度数据 
             delay_ms(1);
							Show_Str(60,70,500,24,novelmenu[0],24,1);
							Show_Str(60,100,200,24,novelmenu[1],24,1);
						  highlight(keyf);//状态切换后，光标位置必是位于首行
						break;
								
						case 4://工测模式
						prim_learn(keyt);						
						break;
						
						case 5:
							if(keyf==1){ 
CdlNum_Gets(58,"0:/小说/假如给我三天光明/Toffqt.dat","0:/小说/假如给我三天光明/Toffrmd.dat","0:/小说/假如给我三天光明/Doffqt.dat","0:/小说/假如给我三天光明/Doffrmd.dat","0:/小说/假如给我三天光明/list.txt");	
	Dis_Catamenu(14,58,"0:/小说/假如给我三天光明/text.txt","0:/小说/假如给我三天光明/data.txt");
}
						else 						
							if(keyf==2){	
CdlNum_Gets(36,"0:/小说/汤姆索亚历险记/Toffqt.dat","0:/小说/汤姆索亚历险记/Toffrmd.dat","0:/小说/汤姆索亚历险记/Doffqt.dat","0:/小说/汤姆索亚历险记/Doffrmd.dat","0:/小说/汤姆索亚历险记/list.txt");								
			Dis_Catamenu(14,36,"0:/小说/汤姆索亚历险记/text.txt","0:/小说/汤姆索亚历险记/data.txt");
							}
							else			
							novel_read(keyf);
						break;					
					};
	}
										
					
void prim_learn(u8 tline){

		 switch(tline){
			 case 1:		//拼音演示
//Dividtxt("0:/盲文基础学习/拼音/声母韵母/声母韵母.txt","0:/盲文基础学习/拼音/声母韵母/smym.txt","0:/盲文基础学习/拼音/声母韵母/data.txt","0:MWOFFSET1.dat","0:PGMEMORY1.dat");
pin=1;
	SPeech("[i1]");
			 
			 f_open(&fil_2, "0:MWOFFSET1.dat", FA_READ);
			 count_s=0;
while(1)	{		 
				f_read(&fil_2, &P[count_s], sizeof (P[count_s]), &br); 
count_s++;
if(f_tell(&fil_2)==f_size(&fil_2))
break;}
f_close(&fil_2);	
				 f_open(&fil_2, "0:PGMEMORY1.dat", FA_READ);
				f_read(&fil_2, &page, sizeof (page), &br);
f_read(&fil_2, &Pdata, sizeof (Pdata), &br);
f_close(&fil_2);
	 		f_open(&fil_0,"0:/盲文基础学习/拼音/声母韵母/data.txt",FA_READ);	
  	f_open(&fil_1,"0:/盲文基础学习/拼音/声母韵母/smym.txt",FA_READ);	
			 Dis_primary(); 
			 Page_turn();
			 break;
			 case 2:		//数字演示
//Dividtxt("0:/盲文基础学习/拼音/自成音节及声调/音节声调.txt","0:/盲文基础学习/拼音/自成音节及声调/yjsd.txt","0:/盲文基础学习/拼音/自成音节及声调/data.txt","0:MWOFFSET2.dat","0:PGMEMORY2.dat");
pin=2;	 SPeech("[i1]");
			 f_open(&fil_2, "0:MWOFFSET2.dat", FA_READ);
			 count_s=0;
while(1)	{		 
				f_read(&fil_2, &P[count_s], sizeof (P[count_s]), &br); 
count_s++;
if(f_tell(&fil_2)==f_size(&fil_2))
break;}


f_close(&fil_2);	
				 f_open(&fil_2, "0:PGMEMORY2.dat", FA_READ);
				f_read(&fil_2, &page, sizeof (page), &br);
f_read(&fil_2, &Pdata, sizeof (Pdata), &br);
f_close(&fil_2);
	 		f_open(&fil_0,"0:/盲文基础学习/拼音/自成音节及声调/data.txt",FA_READ);	
  	f_open(&fil_1,"0:/盲文基础学习/拼音/自成音节及声调/yjsd.txt",FA_READ);	
Dis_primary(); 
			 Page_turn();
			 break;
			 case 3:		//演示汉字浙江理工大学实验室
//Dividtxt("0:/盲文基础学习/数字及标符/数字及标符.txt","0:/盲文基础学习/数字及标符/numpunc.txt","0:/盲文基础学习/数字及标符/data.txt","0:MWOFFSET3.dat","0:PGMEMORY3.dat");
			 
			 /*
pin=3;		 SPeech("[i1]");	

			 f_open(&fil_2, "0:MWOFFSET3.dat", FA_READ);
			 count_s=0;
while(1)	{		 
				f_read(&fil_2, &P[count_s], sizeof (P[count_s]), &br); 
count_s++;
if(f_tell(&fil_2)==f_size(&fil_2))
break;}
f_close(&fil_2);	
				 f_open(&fil_2, "0:PGMEMORY3.dat", FA_READ);
				f_read(&fil_2, &page, sizeof (page), &br);
f_read(&fil_2, &Pdata, sizeof (Pdata), &br);
f_close(&fil_2);
	 		f_open(&fil_0,"0:/盲文基础学习/数字及标符/data.txt",FA_READ);	
  	f_open(&fil_1,"0:/盲文基础学习/数字及标符/numpunc.txt",FA_READ);	
Dis_primary();
      */			 
	dis(1);		 
		Page_turn();
			 break;
			 case 4:
//Dividtxt("0:/盲文基础学习/英文字母/英文字母.txt","0:/盲文基础学习/英文字母/A-Z.txt","0:/盲文基础学习/英文字母/data.txt","0:MWOFFSET4.dat","0:PGMEMORY4.dat");
 pin=4;    SPeech("[d]"); 	
			 f_open(&fil_2, "0:MWOFFSET4.dat", FA_READ);
			 count_s=0;
while(1)	{		 
				f_read(&fil_2, &P[count_s], sizeof (P[count_s]), &br); 
count_s++;
if(f_tell(&fil_2)==f_size(&fil_2))
break;}
f_close(&fil_2);	
				 f_open(&fil_2, "0:PGMEMORY4.dat", FA_READ);
				f_read(&fil_2, &page, sizeof (page), &br);
f_read(&fil_2, &Pdata, sizeof (Pdata), &br);
f_close(&fil_2);
	 		f_open(&fil_0,"0:/盲文基础学习/英文字母/data.txt",FA_READ);	
  	f_open(&fil_1,"0:/盲文基础学习/英文字母/A-Z.txt",FA_READ);	
Dis_primary();


	Page_turn();
			break; 
	 }
 }


void novel_read(u8 tline){

		 switch(tline){
			 case 1://空方测试
	 
 		memset(SDbtmp,'\0',sizeof(SDbtmp));	
Dis_novel("假如给我三天光明",20,20);
NPage_turn("假如给我三天光明","0:/小说/假如给我三天光明/OFFSET.dat",P[0],P[1],20,20,40);
			 break;
			 case 2:		//满方测试
memset(SDbtmp,'\0',sizeof(SDbtmp));	
Dis_novel("汤姆索亚历险记",20,20);
NPage_turn("汤姆索亚历险记","0:/小说/汤姆索亚历险记/OFFSET.dat",P[2],P[3],20,20,40);			
			 break;
		 }
}

void Dis_primary(){
LCD_Clear(WHITE);	
Show_Str(10,15,200,24,"盲文基础学习",24,1);	
	 SPeech("[i1]");  
	memset(nbuf,'\0',sizeof(nbuf));	
	memset(SMbuf,'\0',sizeof(SMbuf));
for(count_s=0;count_s<10;count_s++){
	f_gets(BUF,128,&fil_0);
  memcpy(SDbtmp[count_s],BUF,12);		 
	f_gets(BUF,128,&fil_1);	
	memcpy(nbuf,BUF,strlen(BUF)-2);
		strcat(SMbuf,nbuf); 

	switch(strlen(nbuf)){
		case 1:
	       strncat(SMbuf,"    ",4);break;
		case 2:
			 strncat(SMbuf,"   ",3);break;
		case 3:
			 strncat(SMbuf,"  ",2);break;
		case 4:
			 strncat(SMbuf," ",1);break;
	}
	
		if(strlen(nbuf))
	{strcat(PMbuf,nbuf);
	switch(keyt)
	{
		case 1 :{ SPeech("[i1]");	strcat(PMbuf,"1");} break; 
		case 2 :{SPeech("[i1]"); if(!strcmp(nbuf,"阴平")||!strcmp(nbuf,"阳平")||!strcmp(nbuf,"上声")||!strcmp(nbuf,"去声"))  ;else strcat(PMbuf,"1");} break;
		case 3 : SPeech("[i1]");	break;
		case 4 :   SPeech("[d]");	break;
	}
strncat(PMbuf,"[p500]",1);
}
	memset(nbuf,'\0',sizeof(nbuf));	
if(count_s==9){	
	Show_Str(18,180,800,32,(u8 *)SMbuf,32,1);	
	SPeech((u8 *)PMbuf); 
	memset(SMbuf,'\0',strlen(SMbuf));
	memset(PMbuf,'\0',strlen(PMbuf));
	LCD_ShowBraille(28,240,SDbtmp);
	DX_ShowBraille(SDbtmp);
		memset(SDbtmp,'\0',sizeof(SDbtmp));	
		 }

    } 
	}

void Page_turn(){
	 while(1){
	key_v=KEY_Scan(0);
						if(key_v==2){		//下翻按键按下
if(f_tell(&fil_0)!=Pdata*10*page){//如果读写指针没有指向文件末尾
f_lseek(&fil_0,f_tell(&fil_0));
f_lseek(&fil_1,f_tell(&fil_1));
	}
else{
	f_lseek(&fil_0,0);
f_lseek(&fil_1,0);
	}
Dis_primary();					
					}
		else
if(key_v==3){		//上翻按键按下
	if(f_tell(&fil_1)!=P[1]){//	如果读写指针没有指向文件开头
	j=f_tell(&fil_0)/(Pdata*10)-1;
	f_lseek(&fil_0,Pdata*10*(j-1));
	f_lseek(&fil_1,P[j-1]);}
	else{
	f_lseek(&fil_0,Pdata*10*(page-1));//显示最后一页
	f_lseek(&fil_1,P[page-1]);	}	
	Dis_primary();	
         }
else 
if(key_v==5){

	back_flag=1;
	f_close(&fil_0);
	f_close(&fil_1);break;}
           }	
				 }


				 void Dis_novel(u8*str , u8 r1 , u8 r2){          //LCD第一行显示r1个字节，第二行显示r2个字节
					LCD_Clear(WHITE);		
				 	POINT_COLOR=RED;
	Show_Str(10,15,200,24,str,24,1);	
	POINT_COLOR=BLUE;
	f_gets(shuru1,r1+r2+1,&fil_3);
	 Show_Str(20,180,800,32,(u8 *)shuru1,32,1);
			SPeech(shuru1);			 
			   delay_ms(1);		 

		f_gets(BUF,121,&fil_1);	
	 for(count_s=0;count_s<20;count_s++)
  memcpy(FCbtmp[count_s],BUF+count_s*6,6);	
		
LCD_WShowBraille(30,240,FCbtmp);
		DX_FCBraille(FCbtmp);
		memset(SDbtmp,'\0',sizeof(SDbtmp));		
 
	}

void NPage_turn(u8*str,const TCHAR* path,DWORD P1,DWORD P2,u8 r1,u8 r2,u8 r){ //LCD一页显示r个字节
	while(1){
	key_v=KEY_Scan(0);
						if(key_v==2){		//下翻按键按下
if(f_tell(&fil_3)!=f_size(&fil_3)){//如果读写指针没有指向文件末尾
f_lseek(&fil_3,f_tell(&fil_3));
f_lseek(&fil_1,f_tell(&fil_1));
	}
else{
	f_lseek(&fil_3,0);
f_lseek(&fil_1,0);
	}
		P1=f_tell(&fil_3);
	P2=f_tell(&fil_1);
	 f_open(&fil_2,path, FA_WRITE | FA_OPEN_ALWAYS);
	 f_write(&fil_2,&P1, sizeof(P1), &bw);
		 f_write(&fil_2,&P2, sizeof(P2), &bw);
		f_close(&fil_2);
Dis_novel(str,r1,r2);
	
					}
//	Show_Str(30,20*(count_s+1),200,16,(u8 *)buf,16,0);	 			
		//显示盲文点符对应的字	
		else
if(key_v==3){		//上翻按键按下
	if(f_tell(&fil_3)!=r){//	如果读写指针没有指向文件开头
//	j=f_tell(&fil_1)/120-1;
	f_lseek(&fil_1,f_tell(&fil_1)-240); 
	f_lseek(&fil_3,f_tell(&fil_3)-2*r);}
	else{
	f_lseek(&fil_1,f_size(&fil_1)-120);//显示最后一页
	f_lseek(&fil_3,f_size(&fil_3)-r);}
			P1=f_tell(&fil_3);
	P2=f_tell(&fil_1);
	 f_open(&fil_2, path, FA_WRITE | FA_OPEN_ALWAYS);
	 f_write(&fil_2,&P1, sizeof(P1), &bw);
		 f_write(&fil_2,&P2, sizeof(P2), &bw);
		f_close(&fil_2);
	Dis_novel(str,r1,r2);	
         }
else
	if(key_v==4){
f_open(&fil_2, path, FA_READ);
	f_read(&fil_2, &P1, sizeof (P1), &br);
	f_read(&fil_2, &P2, sizeof (P2), &br);			 
f_close(&fil_2);		
		f_lseek(&fil_3,P1);
f_lseek(&fil_1,P2);
	Dis_novel(str,r1,r2);}	
else 
if(key_v==5){
	dis(0);
	back_flag=1;read_back=1;
		f_close(&fil_3);
	f_close(&fil_1);
break;}
           }    
				 }				 
 
