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
#include "htptb.h"
#include "suocun.h"
#include<stdlib.h>
#include<string.h>
#define MAXDIR 20
 
/************************************************

 汉盲转换显示 实验 
 by Xuanjianqiang
    虚拟现实实验室
		10个字刷新一次
************************************************/
#define INITMENU 0
#define TESTMENU 1
#define FILDIRMENU 2
#define TESTMODE 3
#define READMODE 4
u8 filecnt,keys,keyi,keyf,keyt;//keyi 父级菜单导航栏标记  keyt 工测菜单导航栏标记  keyf sd文件目录栏导航标记  filecnt——sd文件个数 
u16 tlct=0;//tlct为文件定位指针		
u8 keyi=1,keyt=1,keyf=1;//菜单下导航标志变量，初始化为指向首行
	u8 initmenu[3][23]={"欢迎使用盲文阅读系统","工测DEMO","进入阅读目录"};		
 u8 testmenu[7][23]={"工测模式","空方测试","满方测试","拼音演示","数字演示","文字演示","唐诗演示"};			
char YSsym[10][13]={"110000000000","111100000000","101100000000","110100000000","100110000000","001010000000","010001000000","101001000000","001011000000","010010000000"};
char YSsz[10][13]={"001111010111","001111100000","001111110000","001111100100","001111100110","001111100010","001111110100","001111110110","001111110010","001111010110"};
char YSwz[21]={"浙理工大学虚拟现实室"}; 
char TS[21]={"床前明月光疑似地上霜"};
char  Dirname[MAXDIR][30];	
void initUI(void);
void testUI(void);
void fildirUI(void);
void UIfun(u8 level);
void test_act(u8 line);
void read_act(char *filename);
//光标移动函数
//	void highlight(u8 line){
//			LCD_Fill(15,50,30,220,WHITE);//清除原先位置，白色填充	
//	LCD_Fill(15,30+20*line,30,40+20*line+8,BLUE);
//}

	void highlight1(u8 line){
	LCD_Fill(35,60,50,300,WHITE);//清除原先位置，白色填充	
	LCD_Fill(35,44+31*line,45,36+31*line+18,BLUE);
}
		void highlight(u8 line){
//			LCD_Fill(15,50,30,220,WHITE);//清除原先位置，白色填充	
//	LCD_Fill(15,30+20*line,30,40+20*line+8,BLUE);
					LCD_Fill(30,90,50,300,WHITE);//清除原先位置，白色填充	
	LCD_Fill(30,74+31*line,45,66+31*line+18,BLUE);
}

//key0事件发生时，执行的函数   确定
void e0_event_function(u8 *nxt_state){   
    u8 cur_state;   
	cur_state = *nxt_state;   
    switch(cur_state)
    {       
			case INITMENU: //处于0主菜单状态时 根据keyi切换状态界面   
					*nxt_state=keyi;
					UIfun(*nxt_state);//状态切换
			break;
			case TESTMENU: //处于1状态时; 切换到末级菜单
					*nxt_state=TESTMODE; 
		    	UIfun(*nxt_state);//状态切换
				break;
			case FILDIRMENU://处于2目录菜单，切换到文本阅读
					*nxt_state=READMODE;
					read_act(Dirname[keyf-1]);//20字节为单位阅读，定位的标志量加20，记忆功能
				break;
}
}
//key1事件发生时，执行的函数,	向下导航按钮   向下
void e1_event_function(u8 *nxt_state)
{   
    u8 cur_state;   
    cur_state = *nxt_state;   
    switch(cur_state)
    {       
        case INITMENU: //主菜单状态下的key1向下按键触发动作      
            //执行光标移动动作，状态无需切换
					keyi=keyi%2+1;//光标标志变量变化
				  highlight(keyi);
				  break;
				case TESTMENU://子菜单1
				  keyt=keyt%6+1;
				  highlight(keyt);
				  break;
				case FILDIRMENU://子菜单2
					keyf=keyf%filecnt+1;
				  highlight(keyf);
				  break;
				case TESTMODE://工测模式下无变化
					;
				break;
				case READMODE://阅读模式下，实现翻页继续阅读动作
					read_act(Dirname[keyf-1]);
				  break;				
    }
}
//keyup 按键发生时，执行的函数       返回
	void e2_event_function(u8 *nxt_state){
				u8 cur_state;   
				cur_state = *nxt_state; 
		
switch(cur_state)
{
	case TESTMENU:
	case FILDIRMENU:
		*nxt_state=INITMENU;
	  UIfun(*nxt_state);
	break;
	case INITMENU:
		;
	break;
	default:
		if(keyi==1)//根据末次的主菜单导航按钮keyf判别进入的是哪个子目录菜单
	  *nxt_state=TESTMENU;
		else{
		*nxt_state=FILDIRMENU;
		tlct=0;//退出阅读，文件定位指针清零
		//也可写为 UIfun(keyi)
		}
		UIfun(*nxt_state);
	break;
	}
}

int main(void)
{	 u8 key_v,state=0;
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		      //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200	
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					  //初始化按键
	LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);	//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
	SUOCUN_init();//点显器初始化
	font_init();
UIfun(state);
	f_mount(fs[0],"0:",1); 
while(1){
		key_v=KEY_Scan(0);	
	switch(key_v){
		case 1:
			e0_event_function(&state);
		break;
		case 2:
			e1_event_function(&state);
		break;
		case 3:
			e2_event_function(&state);
		dis(0);//空方显示
		break;
	}	
		
}	
		
}		
//0级菜单和I,II，III级菜单的界面显示函数
void UIfun(u8 level){
							LCD_Clear(WHITE);//界面切换前，清屏。
							POINT_COLOR=RED;
								Show_Str(10,20,320,32,initmenu[0],32,0);//大写24号标题
	            POINT_COLOR=BLUE;
					switch(level){	
						case 0://显示主菜单目录
							keyi=1;//对应光标标志变量初始化
							Show_Str(60,70,320,24,initmenu[1],24,0);
			    	 	Show_Str(60,110,320,24,initmenu[2],24,1);
						  highlight1(1);//状态切换后，光标位置必是位于首行
						break;
						case 1://显示工测菜单界面函数
							keyt=1;
							Show_Str(40,70,500,24,testmenu[0],24,0);
							Show_Str(60,100,500,24,testmenu[1],24,0);
							Show_Str(60,130,500,24,testmenu[2],24,0);
 							Show_Str(60,160,500,24,testmenu[3],24,0);
							Show_Str(60,190,500,24,testmenu[4],24,0);
							Show_Str(60,220,500,24,testmenu[5],24,0);
							Show_Str(60,250,500,24,testmenu[6],24,0);
						  highlight(1);//状态切换后，光标位置必是位于首行
						break;
						case 2://显示SD卡文件目录菜单界面
							keyf=1;
						fildirUI();
						highlight(1);//状态切换后，光标位置必是位于首行
						break;
						case 3://工测模式
							test_act(keyt);
						break;
					};
	}
	//目录菜单显示界面				
void fildirUI(void){								
		//支持最大文件目录项
	//FATFS *fs;
//	char *fn;
	DIR dir;
	FIL fil;
	FILINFO f_info;
	u8 maxdir=0,s=0;//s:目录下文件计数。line：导航标记位，0为默认第一栏
	f_mount(fs[0],"0:",1); 		//挂载SD卡	，0;无错误	
	while(SD_Init())			//检测SD卡
	{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!check it");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			LED0=!LED0;//DS0闪烁
			delay_ms(200);		    
	}			
	POINT_COLOR=RED;
  if((f_opendir(&dir,(const TCHAR*)"0:"))!=FR_OK)		//打开SD卡（代号0）根目录
				Show_Str(20,50,200,24,(u8 *)"PATH ERRO!! CHECK ",24,1);//错误处理
        memset(Dirname[0],0,13*MAXDIR);
while(1)//遍历目录
 {
     if(f_readdir(&dir,&f_info)==FR_OK)
     {
         if(f_info.fname[0]==0)break;
     }
     else break;
     if(f_info.fattrib & AM_ARC)        // 目录if(f_readdir(&dir, &fno) == FR_OK) 
     {
         if(maxdir<MAXDIR)
         {
             strncpy(Dirname[maxdir],f_info.fname,13);
             maxdir++;
          }
     }	
}
    filecnt=maxdir;//获得SD卡下文件数	
		for(s=0;s<maxdir;s++)
		Show_Str(30,50+20*s,200,16,(u8 *)Dirname[s],16,1);//一页最多显示10项
 	}										
					
void test_act(u8 tline){
		 LCD_Clear(WHITE);//界面切换前，清屏。
		//Show_Str(30,20,320,32,initmenu[0],32,1);//大写24号标题
		Show_Str(30,60,320,24,testmenu[0],24,1);
		Show_Str(60,90,320,24,testmenu[tline],24,1);
		 switch(tline){
			 case 1://空方测试
				 dis(0);
			break;
			 case 2:		//满方测试
				 dis(1);
			 break;
			 case 3:		//拼音演示
			 Show_Str(30,120,800,32," b  p  m  f   d  a  o  u  en  un",32,1);
			// Show_Str(15,160,200,24,"a  o  u  en  un ",24,1);
			 LCD_ShowBraille(28,240,YSsym);//LCD上两行盲文点阵的显示
			 DX_ShowBraille(YSsym);//点显器输出盲文点阵
			 break;
			 case 4:		//数字演示
  		Show_Str(30,120,800,32," 0  1   2  3  4 5  6   7  8  9",32,1); 
			//Show_Str(10,160,200,24," 5  6   7  8  9",24,1); 
			LCD_ShowBraille(28,240,YSsz);//显示器点位图案输出
			DX_ShowBraille(YSsz);//点显器输出
			//memset(btmp,'\0',sizeof(btmp));
			 break;
			 case 5:		//演示汉字浙江理工大学实验室
				memset(btmp,'\0',sizeof(btmp));
		    Show_Str(30,120,800,32," 浙 理 工 大 学 虚 拟 现 实 室",32,1);//第一行显示5个字
				//Show_Str(8,170,200,24," 虚 拟 现 实 室",24,0);//下一行显示5个字，纵向留有80供显示第一行文字的盲文
			 text2b_lcd(YSwz);
			 LCD_ShowBraille(28,240,btmp);//LCD上两行盲文点阵的显示
			 DX_ShowBraille(btmp);//点显器输出盲文点阵
			 memset(btmp,'\0',sizeof(btmp));
			 break;
			 case 6:
 		  Show_Str(30,120,800,32," 床 前 明 月 光 疑 似 地 上 霜",32,1);//第一行显示5个字
			//Show_Str(8,170,200,24," 疑 似 地 上 霜",24,0);//下一行显示5个字，纵向留有80供显示第一行文字的盲文
			 text2b_lcd(TS);
			LCD_ShowBraille(28,240,btmp);//LCD上两行盲文点阵的显示
			 DX_ShowBraille(btmp);//点显器输出盲文点阵
			 memset(btmp,'\0',sizeof(btmp));
			 			 break; 
  
             
	 }
 }
void read_act(char *filename){	//文本阅读--文本浏览
		FIL fil_0; 
		UINT rd;
  char ltmp[11]="\0";//存储tftlcd一行5个字的数据，置初始化\0是保证strncpy后末尾有结束标识符，使得数据传递没有异常
	      char buffer[60];	
		    memset(btmp,'\0',sizeof(btmp));		
		   LCD_Clear(WHITE); 
		POINT_COLOR=RED; 
		 	  if(!f_open(&fil_0,filename,FA_READ)){       //打开成功	
				Show_Str(10,40,220,16,Dirname[keyf-1],12,0);//注意下标文件
				Show_Str(40,40,220,16,"打开成功，key1读取文字",12,0);/*提示语句*/
				f_lseek(&fil_0,tlct);//移动定位文件指针到离文件起始处tlct个字节的位置，为读写文件服务
				f_read(&fil_0,buffer,20,&rd);//一次读取20个字节，10个字。
				f_close(&fil_0);
			//中文转换为直观的盲文点字序列，12位一组两方一个字保存到字符数组btmp[10][13]中，10组可以存储10个字。
				text2b_lcd(buffer);
 	 	  //开始输出		
		  	memset(ltmp,'\0',sizeof(ltmp));	
				POINT_COLOR=RED; 
				Show_Str(10,60,200,16,"汉字及对应盲文",16,0);
				POINT_COLOR=BLUE; 
				Show_Str(30,80,200,16,(u8 *)strncpy(ltmp,buffer,10),16,0);//第一行显示5个字
				Show_Str(30,160,200,16,strcpy(ltmp,buffer+10),16,0);//下一行显示5个字，纵向留有80供显示第一行文字的盲文
				POINT_COLOR=BLACK; 
				LCD_ShowBraille(30,100,btmp);//LCD上两行盲文点阵的显示
				DX_ShowBraille(btmp);//点显器输出盲文点阵
		    memset(btmp,'\0',sizeof(btmp));
				tlct=tlct+20;//指针tclt偏移20为下次读准备
					if(!(rd==20)){
					POINT_COLOR=BLACK; 
					Show_Str(30,220,200,16,"读取完毕,即将从头开始",16,0);
					tlct=0;
				}	
	   }
		    else
			    Show_Str(10,150,200,16,"打开失败，检查sd卡文件",16,0);
}