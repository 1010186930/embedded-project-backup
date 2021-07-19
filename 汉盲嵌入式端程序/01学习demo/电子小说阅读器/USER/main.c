/**************************************
项目名称：电子小说阅读器
作者：化作尘
时间：2020年12月8日09:36:05
版本：v1.0
邮箱：2809786963@qq.com
博客教程：化作尘
哔哩哔哩：化作尘my
**************************************/


#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	 
#include "piclib.h"	
#include "string.h"	
#include "math.h"	
#include "touch.h"

u8 menu_num;//当前主菜单页数
u32 page;//当前页数
//char chapter_name[50];//当前章节名
u32 page_offset[1000];//保存页数偏移
//u32 page_off_mult;//页数偏移倍数
u8 txt_num;//当前书架图书数量
u8 txt_path_name[30][60];//导入图书路径
u32 bookmark[30];				//保存对应书签 也就是页数 下次进入直接偏移页数
char txt_name[4][30]={"","","",""};//显示4本图书名字
u8 pname[60];			//正在打开的文件名路径
int SIZE=24;			//初始化字体
char buff[2500];//一页数据 缓冲文档
int page_mul_cnt=0;//存取次数 一次1000页
int now_page_mul;//当前1000页的倍数
u32 data_offset;//读取偏移  读写flash用


//显示主界面
void menu(void);
//获取触摸键值
u16 get_ts_val(int key);
//读取文件数据到LCD
int read_to_lcd(void);
//更新小说名
u8 lcd_dis_files(u8 * path);
//阅读模式
void read_mode(void);
//设置字体格式
int setting(void);
//设置界面
void show_setting(void);
//目录功能
u32 txt_dir_scan(void);
//文件系统功能 导入小说
void fatfs_choose(void);
//读取数据到flash
void file_data_to_w25q(void);
//根据页码得到页数
u32 ident_get_page(u32 page_offset);
//遍历文件 显示到屏幕
u8 mf_scan_file(u8 * path);
//刷新显示书架名字
void re_dis_txtname(int i);
//带路径的文件名 转 不带路径文件名
char* path_change(char *oldpath);
//根据页数得到页码
u32 ident_get_offset(u32 page);
//更新内存1000页页码
void update_page_offset(void);
//读出书签数据
void read_bookmark(void);
//保存书签数据到flash
void save_bookmark(void);
//保存书名数据到flash
void save_bookinfo(void);
//删除小说
void del_txt(u8 *path);
//删除小说控制界面
void del_txt_ctrl(void);
	



int main(void)
{        
 	//DIR picdir;	 		//图片目录
	u32 recode=0;
	
	u8 key;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化 
	tp_dev.init();			//触摸初始化	
	W25QXX_Init();				//初始化W25Q128
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	
	
	/******************防止二次烧入****************************/
	W25QXX_Read((u8 *)&recode,1024*1024*4-4,4);//读出到内存
	if(recode != 0xffaaffaa)
	{
		recode = 0xffaaffaa;
		W25QXX_Write((u8 *)&recode,1024*1024*4-4,4);//写入flash
		//W25QXX_Erase_Chip();//擦除整片
		txt_num=0;//初始化小说本数0 不然程序无法运行 
		save_bookinfo();//保存到flash
	}
	/******************防止二次烧入****************************/
	
	
	while(font_init()) 			//检查字库
	{  
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,70,200,16,16,"SD Card OK");
		LCD_ShowString(30,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}     

	//f_opendir(&picdir,"0:/SYSTEM/PICTURE"); //打开目录
	delay_ms(150);
	piclib_init();										//初始化画图	  
	delay_ms(150);
	LCD_Clear(GBLUE);//清屏
	ai_load_picfile("0:/SYSTEM/PICTURE/logo.gif",0,0,lcddev.width,lcddev.height,1);//显示图
	read_bookmark();//读出图书 书签数据
	
	memset(page_offset,0,1000*4);//清空页码数组
	menu_num=1;//显示第一页
	//lcd_dis_files("0:/TXT");//更新系统书名
	menu();//显示主界面
	while(1)
	{
		key = get_ts_val(1);
		if(key!=0)
		{
			printf("key=%d\n",key);
			switch(key)//选择要打开的图书
			{
				case 1: if(txt_num-4*(menu_num-1)>=1){
									strcpy((char *)pname,(char *)txt_path_name[0+(menu_num-1)*4]);//把要打开的书籍名称赋值
					
									page = bookmark[0+(menu_num-1)*4];//根据flash保存的书签初始化进入的页数
									if(page >= 0xffff)page=0;
										read_mode();//进入阅读模式
									bookmark[0+(menu_num-1)*4] = page;//退出保存书签
									save_bookmark();
									}break;
				
				case 2:	if(txt_num-4*(menu_num-1)>=2){
									strcpy((char *)pname,(char *)txt_path_name[1+(menu_num-1)*4]);
					
									page = bookmark[1+(menu_num-1)*4];
									if(page >= 0xffff)page=0;
									read_mode();//进入阅读模式
									bookmark[1+(menu_num-1)*4] = page;
									save_bookmark();
									}break;
									
									
				case 3: if(txt_num-4*(menu_num-1)>=3){
									strcpy((char *)pname,(char *)txt_path_name[2+(menu_num-1)*4]);
					
									page = bookmark[2+(menu_num-1)*4];
									if(page >= 0xffff)page=0;
									read_mode();//进入阅读模式
									bookmark[2+(menu_num-1)*4] = page;
									save_bookmark();
									}break;
									
				case 4: if(txt_num-4*(menu_num-1)>=4){
									strcpy((char *)pname,(char *)txt_path_name[3+(menu_num-1)*4]);
					
									page = bookmark[3+(menu_num-1)*4];
									if(page >= 0xffff)page=0;
									read_mode();//进入阅读模式
									bookmark[3+(menu_num-1)*4] = page;
									save_bookmark();
									}break;

				case 5:		//导入图书
									fatfs_choose();//文件系统选择文件导入
									save_bookinfo();//保存书籍数据到flash
									printf("当前图书数量：%d\r",txt_num);
									break;
				
				case 6:		del_txt_ctrl();//删除图书
									break;
				
				case 111: if((txt_num-1)/4+1>menu_num)menu_num++;
										printf("menu_num = %d\n",menu_num);//上一页
									break;
				
				case 222: if(1<menu_num)menu_num--;
										printf("menu_num = %d\n",menu_num);//下一页
									break;	
			}
			
			menu();//显示主界面
			data_offset=0;//偏移清零
			page_mul_cnt=0;//1000页倍数清零
		}
		delay_ms(10);
	}
}

//阅读模式 进入阅读
void read_mode(void)
{
	int key;
	u8 res,n;
	char spr_name[30];

	sprintf((char *)spr_name,"%s",pname);
	res=mf_open((char *)spr_name,FA_READ);//只读打开
	if(res==FR_OK)printf("打开：%s\n",(char *)spr_name);
	else return;//打开失败
	
	LCD_Clear(WHITE);//清屏
	n=page/1000+1;//计算要更新几次
	while(n--)
	{
		LCD_ShowNum(200,376,page/1000+1-n,2,24);//显示进度条
		LCD_ShowChar(234,376,'/',24,0);
		LCD_ShowNum(250,376,page/1000+1,2,24);
		file_data_to_w25q();	//更新页数 一次1000页
	}
	mf_lseek(page_offset[page%1000]);//得到初始偏移
	
	read_to_lcd();//读取文档数据到屏幕
	
	Show_Str(420,770,200,24,"返回",24,0);
	Show_Str(5,770,50,24,"设置",24,0);
	Show_Str(218,770,50,24,"目录",24,0);
	LCD_ShowNum(400,0,page,5,16);

	
	while(1)
	{
			key = get_ts_val(2);
			if(key != 0)
			{
//				printf("key == %d\n",key);
				switch(key)//操作选择
				{
					case 101:
										page++;
										if(page%1000==0 && page>0)
										{
											//更新缓冲页码数组
											file_data_to_w25q();//先写入页码到flash
											update_page_offset();//更新内存1000页页码
										}
										res=mf_lseek(page_offset[page%100]);
										if(res==0)read_to_lcd();
										break;//下一页
					case 202:	
										
										if(page==0)
										{
											Show_Str(180,730,200,24,(u8*)"已经是第一页",24,0);
											break;
										}
										page--;
										if(page%999==0 && page>0)
										{
											update_page_offset();//更新内存1000页页码
										}
										res=mf_lseek(page_offset[page%1000]);
										if(res==0)read_to_lcd();
										break;//上一页
					case 303:
					case 404:break;
					case 5: 
									return;//保存数据到flash//返回 
					case 6:
								res=setting();
								if(res)  //修改设置退出
								{
										res=0;
										LCD_Clear(BACK_COLOR);//设置完还原界面
										Show_Str(50,50,400,700,(u8 *)buff,SIZE,0); 
								}else //返回退出 
								{
									LCD_Fill(0,570,480,800,BACK_COLOR);
									Show_Str(50,50,400,700,(u8 *)buff,SIZE,0); 
								}
								break;
					case 7:page=txt_dir_scan();//目录扫描 返回页数
								
								update_page_offset();//更新内存1000页页码
								res=mf_lseek(page_offset[page%1000]);//偏移到页码
								if(res==0)read_to_lcd();
								break;
				}
				Show_Str(420,770,200,24,"返回",24,0);
				Show_Str(5,770,50,24,"设置",24,0);
				Show_Str(218,770,50,24,"目录",24,0);
				LCD_ShowNum(400,0,page,5,16);
				//printf("lseek=%ld\n",offset);
			}
	}
}

//设置界面
void show_setting()
{
	int i;
	u16 clor[7] = {WHITE,BLACK,BLUE,BRED,GRED,GBLUE,RED};
	LCD_Fill(0,570,480,800,GRAY);
	LCD_Fill(45,580,75,604,BLACK);
	LCD_Fill(190,580,220,604,WHITE);
	LCD_Fill(335,580,365,604,GBLUE);
	
	Show_Str(34+45,580,64,24,"夜间",24,1);
	Show_Str(34+190,580,64,24,"白天",24,1);
	Show_Str(34+335,580,64,24,"护眼",24,1);
	
	Show_Str(10,630,50,24,"背景",24,1);
	Show_Str(10,690,50,24,"字体",24,1);
	Show_Str(10,750,50,24,"字号",24,1);
	for(i=0; i<7; i++)
	{
		LCD_Fill(80+i*50,630,120+i*50,654,clor[i]);
		LCD_Fill(80+i*50,690,120+i*50,714,clor[i]);
	}
	Show_Str(80,750,400,24,"12号     16号     24号    32号",24,1);
	//Show_Str(420,770,200,24,"返回",24,0);
}
//设置
int setting(void)
{	
	int key;
	show_setting();//显示界面
	while(1)
	{
		key = get_ts_val(3);
		if(key)
		{
//			printf("key3=%d\n",key);
//			printf("%d,%d\n",tp_dev.x[0],tp_dev.y[0]);
			switch(key)//操作选择
			{
				//case 5:key=0;goto exit;break;//跳出
				//背景
				case 10:BACK_COLOR=WHITE;return 1;
				case 11:BACK_COLOR=BLACK;return 1;
				case 12:BACK_COLOR=BLUE;return 1;
				case 13:BACK_COLOR=BRED;return 1;
				case 14:BACK_COLOR=GRED;return 1;
				case 15:BACK_COLOR=GBLUE;return 1;
				case 16:BACK_COLOR=RED;return 1;
				//字体颜色
				case 17:POINT_COLOR = WHITE;return 1;
				case 18:POINT_COLOR = BLACK;return 1;
				case 19:POINT_COLOR = BLUE;return 1;
				case 20:POINT_COLOR = BRED;return 1;
				case 21:POINT_COLOR = GRED;return 1;
				case 22:POINT_COLOR = GBLUE;return 1;
				case 23:POINT_COLOR = RED;return 1;
				case 30:POINT_COLOR = WHITE;BACK_COLOR=BLACK;return 1;
				case 31:POINT_COLOR = BLACK;BACK_COLOR=WHITE;return 1;
				case 32:POINT_COLOR = BLACK;BACK_COLOR=GBLUE;return 1;
				//字号
				case 24:SIZE=12;return 1;
				case 25:SIZE=16;return 1;
				case 26:SIZE=24;return 1;
				case 28:SIZE=32;return 1;
				case 27:goto exit;
			}
			
		}
	}
exit:
	return 0;
}
//显示主界面
void menu(void)
{
	char spr_buf[60];//拼接数组
	
	int len1,len2,len3,len4;

	BACK_COLOR = BROWN;//背景
	POINT_COLOR = BLACK;//字体颜色
	LCD_Clear(WHITE);
	//ai_load_picfile((u8 *)"0:/PICTURE/menu.jpg",0,0,lcddev.width,lcddev.height,1);//显示图片
	LCD_Fill(0,0,480,50,BROWN);
	Show_Str(220,10,200,24,"书架",24,0);
	Show_Str(380,770,100,24,"导入图书",24,1);
	Show_Str(0,770,100,24,"删除图书",24,1);
	
	//printf("txt_num = %d\n",txt_num);//打印当前书架数量
	//txt_num=0;
	if(txt_num != 0)
	{
		BACK_COLOR = WHITE;//背景
		POINT_COLOR = BLACK;//字体颜色
		
		LCD_Fill(0,350,480,380,WHITE);
		LCD_Fill(0,700,480,730,WHITE);
		
		if(txt_num-4*(menu_num-1)>=1)
		{
			strcpy((char *)txt_name[0],path_change((char *)txt_path_name[0+(menu_num-1)*4]));//提取文件路径
			len1 = strlen(txt_name[0]);//获取文件长度
			
			sprintf(spr_buf,"0:/SYSTEM/PICTURE/%s.jpg",txt_name[0]);//查看系统是否有匹配图片
			if(mf_open(spr_buf,FA_OPEN_EXISTING)==FR_OK)
				ai_load_picfile(spr_buf,60,100,150,250,1);	//显示图片1
			else 
				ai_load_picfile("0:/SYSTEM/PICTURE/txt.jpg",60,100,150,250,1);	//显示图片1
			
			Show_Str(60+75-len1*4,350,200,16,(u8 *)txt_name[0],16,0);//显示名字
		}
		
		if(txt_num-4*(menu_num-1)>=2)
		{
			strcpy((char *)txt_name[1],path_change((char *)txt_path_name[1+(menu_num-1)*4]));//提取文件路径
			len2 = strlen(txt_name[1]);//获取文件长度
			
			sprintf(spr_buf,"0:/SYSTEM/PICTURE/%s.jpg",txt_name[1]);//查看系统是否有匹配图片
			if(mf_open(spr_buf,FA_OPEN_EXISTING)==FR_OK)
				ai_load_picfile(spr_buf,280,100,150,250,1);
			
			else 
				ai_load_picfile("0:/SYSTEM/PICTURE/txt.jpg",280,100,150,250,1);//显示图片2
			
			Show_Str(280+75-len2*4,350,200,16,(u8 *)txt_name[1],16,0);//显示名字
		}
		
		
		if(txt_num-4*(menu_num-1)>=3)
		{
			strcpy((char *)txt_name[2],path_change((char *)txt_path_name[2+(menu_num-1)*4]));//提取文件路径
			len3 = strlen(txt_name[2]);//获取文件长度
			
			sprintf(spr_buf,"0:/SYSTEM/PICTURE/%s.jpg",txt_name[2]);//查看系统是否有匹配图片
			if(mf_open(spr_buf,FA_OPEN_EXISTING)==FR_OK)
				ai_load_picfile(spr_buf,60,450,150,250,1);//显示图片3
			else 
				ai_load_picfile("0:/SYSTEM/PICTURE/txt.jpg",60,450,150,250,1);//显示图片3
			
			Show_Str(60+75-len3*4,700,200,16,(u8 *)txt_name[2],16,0);//显示名字
		}
		
		
		if(txt_num-4*(menu_num-1)>=4)
		{
			strcpy((char *)txt_name[3],path_change((char *)txt_path_name[3+(menu_num-1)*4]));//提取文件路径
			len4 = strlen(txt_name[3]);//获取文件长度
			
			sprintf(spr_buf,"0:/SYSTEM/PICTURE/%s.jpg",txt_name[3]);//查看系统是否有匹配图片
			if(mf_open(spr_buf,FA_OPEN_EXISTING)==FR_OK)
				ai_load_picfile(spr_buf,280,450,150,250,1);//显示图片4
			else 
				ai_load_picfile("0:/SYSTEM/PICTURE/txt.jpg",280,450,150,250,1);//显示图片4
			
			Show_Str(280+75-len4*4,700,200,16,(u8 *)txt_name[3],16,0);//显示名字
		}
	}
	
}



//读取文件文本数据到lcd
int read_to_lcd(void)
{
	int len;
	u8 res=0,hc_cnt=0;
	u16 tlen=0;
	
	LCD_Clear(BACK_COLOR);
	
	
	if(SIZE==12)len=2400;
	else if(SIZE==16)len=1200;
	else if(SIZE==24)len=700;
	else if(SIZE==32)len=400;
	while(1)
	{
		res=f_read(file,buff+tlen,1,&br);
		
		if(!res)	//读数据
		{
			tlen+=br;
			
			if(*(buff+tlen-1)=='\n')hc_cnt++;
			
			if(tlen+hc_cnt*SIZE>len && strncmp(buff+tlen-2,"，",2)==0)//退出条件
				break;
			
			else if(tlen+hc_cnt*SIZE>len && strncmp(buff+tlen-2,"。",2)==0)//退出条件
				break;
			
			else if(tlen+hc_cnt*SIZE>len && buff[tlen-1]=='\n')
				break;
		}	
	}
	buff[tlen]='\0';//加尾部
	
	res=Show_Str(50,50,400,700,(u8 *)buff,SIZE,0); //显示到屏幕
	//printf("next_page: tlen=%d ,  offset=%ld,  hc_cnt=%d\n",(int)tlen,offset,hc_cnt);
	
	return tlen;
}


//获取触摸键值
int x_last,y_last,x_last2,y_last2,flag=0,time=0;
u16 get_ts_val(int key)
{
	int i=10;
	int val = 0;
	//if(flag==1)time++;
	if((tp_dev.scan(0))==1 && flag==0)
	{
//		printf("按下\n");
//		printf("[%d,%d]%d\r\n",x_last,y_last,flag);
		flag=1;
		x_last=tp_dev.x[0];
		y_last=tp_dev.y[0];
	}
	if(flag==1)
	{
		flag=0;
		while(1)
		{
			tp_dev.scan(0);
			delay_ms(1);
			if(tp_dev.x[0]>480)
			{
//				printf("松开\n");
//  			printf("[%d,%d]\n",x_last2,y_last2);
				if(x_last2-x_last<-50)val=101;//<-
				else if(x_last2-x_last>50)val=202;//->
				else if(y_last2-y_last<-50)val=111;//下
				else if(y_last2-y_last>50)val=222;//上
				tp_dev.x[0]=0;tp_dev.y[0]=0;
				break;
			}
				x_last2=tp_dev.x[0];
				y_last2=tp_dev.y[0];
		}
	}
	if(val != 0)
	{
		x_last2=0;y_last2=0;
		return val;
	}
	
	if(key == 1)//界面1  选择图书
	{
		if(x_last2>60 && x_last2<210)
		{
			if(y_last2>100 && y_last2<100+250)val=1;
			if(y_last2>450 && y_last2<700)val=3;
		}else if(x_last2>280 && x_last2<430)
		{
			if(y_last2>100 && y_last2<100+250)val=2;
			if(y_last2>450 && y_last2<700)val=4;
		}		
		if(x_last2>400 && y_last2>750 && x_last2<479 && y_last2<799)val=5;//导入图书
		if(x_last2>0 && y_last2>750 && x_last2<100 && y_last2<799)val=6;//删除图书
	}
	else if(key == 2)//阅读模式
	{
		if(x_last2<100 && y_last2>750)val=6;//设置
		else if(x_last2>400 && y_last2>750 && x_last2<479 && y_last2<799)val=5;//返回
		else if(x_last2>215 && y_last2>750 && x_last2<265 && y_last2<799)val=7;//目录
		else if(x_last2>230 && y_last2>10 && x_last2<479 && y_last2<700)val=101;//下一页
		else if(x_last2>0 && y_last2>10 && x_last2<220 && y_last2<700)val=202;//上一页
	}
	else if(key == 3)//界面3 设置
	{
		for(i=0; i<7; i++)
		{
			if(x_last2>80+i*50 && x_last2<80+(i+1)*50)
			{
				if(y_last2>630 && y_last2<654)val=10+i;//10-16
				if(y_last2>690 && y_last2<714)val=17+i;//17-23
			}
		}
		if(y_last2>745 && y_last2<790)
		{
			if(x_last2>80 && x_last2<150)val=24;
			if(x_last2>180 && x_last2<230)val=25;
			if(x_last2>250 && x_last2<320)val=26;
			if(x_last2>330 && x_last2<450)val=28;
		}
		else if(y_last2>540 && y_last2<600)
		{
			if(x_last2>45 && x_last2<145)val=30;
			else if(x_last2>190 && x_last2<290)val=31;
			else if(x_last2>335 && x_last2<435)val=32;
		}
		else if(x_last2>100 && y_last2>100 && x_last2<380 && y_last2<540)val=27;//取消设置
	}
	else if(key == 4)//目录
	{
		for(i=0; i<20; i++)
		{
			if(x_last2>10 && x_last2<400+50)
			{
				if(y_last2>40*i && y_last2<40*(i+1))val=i+1;//1-20
			}
		}
		if(x_last2>400 && x_last2<480 && y_last2>10 && y_last2<760)val=21;
	}
	else if(key == 5)//界面5 文件系统
	{
		for(i=0; i<15; i++)
		{
			if(x_last2>10 && x_last2<450)
			{
				if(y_last2>90+45*i && y_last2<90+45*(i+1))val=i+1;//1-15
			}else if(x_last2>400 && y_last2>750 && x_last2<479 && y_last2<799)val=17;//返回
			
		}
	}
	else if(key==6)//是否选择 
	{
		if(x_last2>180 && y_last2>420 && x_last2<230 && y_last2<480)val=1;//是
		else if(x_last2>250 && y_last2>420 && x_last2<320 && y_last2<480)val=2;//否
	}
	else if(key==7)//删除图书
	{
		if(x_last2>200 && y_last2>85 && x_last2<245 && y_last2<125)val=1;
		else if(x_last2>425 && y_last2>80 && x_last2<470 && y_last2<125)val=2;
		else if(x_last2>200 && y_last2>430 && x_last2<245 && y_last2<475)val=3;
		else if(x_last2>200 && y_last2>430 && x_last2<245 && y_last2<475)val=4;
		else if(x_last2>400 && y_last2>750 && x_last2<479 && y_last2<799)val=5;//返回
	}
	
	x_last2=0;y_last2=0;
	return val;
}



//更新系统小说名
u8 lcd_dis_files(u8 * path)
{
	FRESULT res;	  
  char *fn;   /* This function is assuming non-Unicode cfg. */
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
  res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
  if (res == FR_OK) 
	{	
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
			//if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
			fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
			if(strncmp(fn-4+strlen(fn),".txt",4)==0)//先判断是不是小说
				sprintf((char *)txt_path_name[txt_num],"%s/%s",path,fn);//拼接路径
			txt_num++;
		} 
  }	  
	myfree(SRAMIN,fileinfo.lfname);
}


//目录功能
u32 txt_dir_scan()
{
	int key;
	u32 dir_offset[20];
	int i;
	int cnt;
	int res,flag,flag2;
	int tlen;
	u32 tlen_sum=0;
	
	mf_lseek(0);
	
txt_dir_choose:
	res=0;flag=0;flag2=0;tlen=2;cnt=0;
	
	for(i=0; i<20; i++)
	{
		if(i%2)LCD_Fill(0,i*40,400,(i+1)*40,0xCDF7);
		else LCD_Fill(0,i*40,400,(i+1)*40,GRAY);
	}
	
	while(1)
	{
		res=f_read(file,buff+tlen,1,&br);
		if(!res && br==1)	//读数据
		{
			tlen+=br;
			tlen_sum+=1;//偏移累加
			if(flag==0 && strncmp(buff+tlen-2,"第",2)==0)flag=1;
			if(flag>=1)
			{
				if(flag2==0 && strncmp(buff+tlen-2,"章",2)==0)//开始检测章
				{
					flag2=1;
				}else if(*(buff+tlen-1)=='\n')tlen-=1;//除去多余的\n
				else if(*(buff+tlen-1)>'A' && *(buff+tlen-1)<'z')tlen-=1;//除去
				
				flag++;	
				if(flag>50){
					flag=0;flag2=0;
				}
			}
			if(flag2 >= 1)
			{
				if(*(buff+tlen-1)=='\n' || *(buff+tlen-1)=='\r'
					|| (*(buff+tlen-1)>'A' && *(buff+tlen-1)<'z'))//检测完毕条件
				{
					if(flag<50)//限制显示
						*(buff+tlen-1)='\0';
					else 
						*(buff+tlen-9)='\0';
					
					printf("%s\n",buff+tlen-flag);//buff+tlen-flag-2就是第首 偏移tlen_sum+tlen-flag-2
					dir_offset[cnt] = tlen_sum-flag;//记录偏移位置 就是目录章节首偏移
					
					Show_Str(0,16+cnt*40,400,16+(cnt+1)*40,(u8*)buff+tlen-flag,16,1);//显示章节到LCD
					cnt++;//下一章
					
					flag=0;//清零检测下一章位置
					flag2=0;
					
					tlen=2;//清零重检测
					
					if(cnt==20)break;//20章节检测完毕
				}
			}
			if(tlen>1100)tlen=0;
		}	
		else 
		{
			printf("读取完成\n");
			break;
		}
	}
	while(1)//触摸操作
	{
		key = get_ts_val(4);
		
		if(key>0 && key<=20)
		{
			
			printf("跳转页数：%d\n",ident_get_page(dir_offset[key-1]));
			return ident_get_page(dir_offset[key-1]);//返回页数
			
		}
		else if(key==21)
		{
			return 0;
		}
		else if(key==111)goto txt_dir_choose;//继续
		
	}
	
}







//读取页码数据到flash
void file_data_to_w25q()
{
	int len,page_cnt=0;
	u8 res=0,hc_cnt=0;
	u16 tlen=0;
	
	
	LCD_ShowChar(256,400,'%',24,0);
	
	if(SIZE==12)len=2400;				//字体适配
	else if(SIZE==16)len=1200;
	else if(SIZE==24)len=700;
	else if(SIZE==32)len=400;
	
	res=mf_lseek(data_offset);//偏移更新
	
	if(res==0)
		printf("开始写入页码到flash\n");
	else 
		return ;
	
	while(1)
	{
			while(1)//开始识别页码
			{
					while(1)
					{
						res=f_read(file,buff+tlen,1,&br);
						if(!res && br==1)	//读数据
						{
							tlen+=br;
							if(*(buff+tlen-1)=='\n')hc_cnt++;
							
							if(tlen+hc_cnt*SIZE>len && strncmp(buff+tlen-2,"，",2)==0)//退出条件
								break;
							
							else if(tlen+hc_cnt*SIZE>len && strncmp(buff+tlen-2,"。",2)==0)//退出条件
								break;
							
							else if(tlen+hc_cnt*SIZE>len && buff[tlen-1]=='\n')
								break;
						}	
						else 
						{
							data_offset += tlen;
							W25QXX_Write((u8*)page_offset,1024*1024*5+4*1000*page_mul_cnt+4*(page_cnt%1000),4*(page_cnt%1000));//写入flash
							return ;
						}
					}
					
					//if(page_cnt<5)printf("flash :tlen=%d ,  offset=%ld,  hc_cnt=%d\r\n",(int)tlen,offset,hc_cnt);
					page_offset[page_cnt] = data_offset;//保存页码
					data_offset+=(tlen);//文件偏移更新
					
					page_cnt++;//页码加
					if(page_cnt==1000)
					{	page_cnt=0;
						break;//一次读1000个页码
					}
					
					hc_cnt=0;tlen=0;
					
					if(page_cnt%10==0)
						LCD_ShowNum(220,400,page_cnt/10,2,24);//显示加载百分比
			}
			W25QXX_Write((u8*)page_offset,1024*1024*5+4*1000*page_mul_cnt,4*1000);//写入flash
			page_mul_cnt++;//倍数自加
			
			break;//识别页码完毕 跳出
//			if(page_mul_cnt==1)//读取1组 1000*1=1000页
//			{	
			
//			}
	}
	
}





//根据页码(偏移)得到页数
u32 ident_get_page(u32 page_off)
{
	u32 n=0,pg_offset1,pg_offset2;
	u32 cnt=100;
	while(cnt--)
	{
		if(page_off > page_offset[999])//如果比本篇幅最大还要大就更新1000页页码
		{
			file_data_to_w25q();//写入页码到flash
			page+=1000;//更新页码
			update_page_offset();//更新内存1000页页码
		}
		else if(page_off < page_offset[0])//如果比本篇幅最小还要小就更新到上一个页码
		{
			page-=1000;//更新页码
			update_page_offset();//更新内存1000页页码
		}
		else break;
	}
	cnt =1000*100;
	while(cnt)
	{
		W25QXX_Read((u8 *)&pg_offset1,1024*1024*5+4*n,4);//读出页码数据
		W25QXX_Read((u8 *)&pg_offset2,1024*1024*5+4*(n+1),4);//读出页码数据
		
		if(page_off>=pg_offset1 && page_off<pg_offset2)break;
		
		if(page_off>pg_offset1+500*20)n+=20;
		else if(page_off>pg_offset1)n+=1;
		else if(page_off<pg_offset1)n-=1;
		else if(page_off<pg_offset1-500*20)n-=20;
	}
	printf("%ld->%ld \r\n",page_offset,n);
	return n;
}


//根据页数得到页码
u32 ident_get_offset(u32 page)
{
	u32 pg_offset;
	W25QXX_Read((u8 *)&pg_offset,1024*1024*5+4*page,4);//读出页码数据
	
	printf("%ld->%ld  \r\n",page,pg_offset);
	return pg_offset;
}

//更新内存1000页页码
void update_page_offset(void)
{
		now_page_mul = page/1000;
		memset(page_offset,0,1000*4);
		W25QXX_Read((u8 *)&page_offset,1024*1024*5+(page/1000)*4*1000,4*1000);//读出到内存
}
			


//文件系统功能 导入小说
char txt_pathname[15][60];//带路径的文件名
void fatfs_choose(void)
{
	int i;
	int key,key2;
	mf_scan_file("0:/");
	while(1)
	{
		key = get_ts_val(5);
		if(key>0 && key<16)
		{
			if(strncmp(txt_pathname[key-1]-4+strlen(txt_pathname[key-1]),".txt",4)==0)
			{
				LCD_Fill(140,300,340,340,BROWN);//
				LCD_Fill(140,340,340,480,GRAY);//
				Show_Str(192,304,200,24,"导入图书",24,1);
				Show_Str(150,360,200,16,"是否导入书籍到书架？",16,1);
				Show_Str(145,400,200,16,(u8*)txt_pathname[key-1],16,1);
				LCD_Fill(180,440,220,470,WHITE);//白
				LCD_Fill(260,440,300,470,WHITE);//白
				Show_Str(190,445,200,24,"是     否",24,1);
				while(1)
				{
					key2 = get_ts_val(6);
					if(key2==1){
						strcpy((char*)txt_path_name[txt_num],(char*)txt_pathname[key-1]);//保存路径
						txt_num++;
						printf("新添书籍：%s\r\n",txt_path_name[txt_num-1]);
						for(i=0; i<txt_num; i++)
						{
							printf("%s\r\n",txt_path_name[i]);
						}
//						mf_scan_file("0:/");
						return ;
//						break;
					}
					else if(key2==2){mf_scan_file("0:/");
					break;}
				}
			}
			else mf_scan_file((u8*)txt_pathname[key-1]);
		}else if(key==17)
		{
			break;
		}
	}
	
}
	
	
	
	

//删除小说
void del_txt(u8 *path)
{
	u8 i,j;
	for(i=0; i<txt_num; i++)
	{
		if(strcmp(path,txt_path_name[i])==0)
		{
			for(j=i; j<txt_num; j++)
			{
				strcpy(txt_path_name[j],txt_path_name[j+1]);
			}
			txt_num--;
			break;
		}
	}save_bookinfo();//保存数据到flash
}

//删除小说控制界面
void del_txt_ctrl(void)
{
	int key,key2;
	Show_Str(380,770,100,24,"    取消",24,0);
	Show_Str(0 , 770,100,24,"        ",24,0);
	if(txt_num-4*(menu_num-1)>=1)
	{
		ai_load_picfile("0:/SYSTEM/PICTURE/del.jpg",210,85,30,30,1);	//显示图片del
	}
	
	if(txt_num-4*(menu_num-1)>=2)
	{
		ai_load_picfile("0:/SYSTEM/PICTURE/del.jpg",430,85,30,30,1);	//显示图片del
	}
	
	if(txt_num-4*(menu_num-1)>=3)
	{
		ai_load_picfile("0:/SYSTEM/PICTURE/del.jpg",210,435,30,30,1);	//显示图片del
	}
	
	if(txt_num-4*(menu_num-1)>=4)
	{
		ai_load_picfile("0:/SYSTEM/PICTURE/del.jpg",430,435,30,30,1);	//显示图片del
	}
	
	while(1)
	{
		key = get_ts_val(7);
		if(key!=0 && txt_num-4*(menu_num-1)>=key)
		{
				LCD_Fill(140,300,340,340,BROWN);//
				LCD_Fill(140,340,340,480,GRAY);//
				Show_Str(192,304,200,24,"删除图书",24,1);
				Show_Str(150,360,200,16,"是否将书籍移出书架？",16,1);
				Show_Str(145,400,200,16,(u8*)txt_path_name[key-1+(menu_num-1)*4],16,1);
				LCD_Fill(180,440,220,470,WHITE);//白
				LCD_Fill(260,440,300,470,WHITE);//白
				Show_Str(190,445,200,24,"是     否",24,1);
				while(1)
				{
					key2 = get_ts_val(6);
					if(key2==1){    //是
						del_txt(txt_path_name[key-1+(menu_num-1)*4]);
						return ;
					}
					else if(key2==2) //否
						return ;
				}
		}else if(key==5)return ;//退出
	}
	
}

//遍历文件 显示到屏幕
u8 mf_scan_file(u8 * path)
{
	u8 txt_cnt=0;
	int i;
	FRESULT res;	  
  char *fn;   /* This function is assuming non-Unicode cfg. */

 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
	  
	
	res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
	if (res == FR_OK) 
	{	
		LCD_Clear(WHITE);//显示界面
		LCD_Fill(0,0,480,50,BROWN);//
		Show_Str(208,7,200,32,"文件",32,1);
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
			fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
			sprintf(txt_pathname[txt_cnt],"%s/%s",path,fn);//拼接路径名
			//printf("%s\n",txt_pathname[txt_cnt]);
			//显示到屏幕
			LCD_Fill(50,90+txt_cnt*45,75,90+(txt_cnt+1)*45,BROWN);
			LCD_DrawLine(75,90+(txt_cnt+1)*45,400,90+(txt_cnt+1)*45);
			Show_Str(80,100+txt_cnt*45,300,24,(u8*)fn,24,0);
			
			txt_cnt++;
			if(txt_cnt>=15)break;
		}
		for(i=0; i<15-txt_cnt; i++)
			memset(txt_pathname[15-1-i],0,60);//清空上一次的路径
	
		Show_Str(420,770,200,24,"返回",24,0);
		
		
  }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}
	
	




//带路径的文件名 转 不带路径文件名
char* path_change(char *oldpath)
{
	//   0:TXT/斗罗大陆.txt  -> 斗罗大陆
	int i;
	static char path_buf[30];//保存名字
	char old_path[60];
	
	strcpy(old_path,oldpath);//防止修改本源
	
	
	old_path[strlen(old_path)-4]='\0';  //   0:TXT/斗罗大陆
	
	for(i=strlen(old_path); i>0; i--)  
	{
		if(old_path[i]=='/')
			break;
	}
	strcpy(path_buf,old_path+i+1);    //     斗罗大陆
	
	
	return path_buf;
}






//保存书签数据到flash
void save_bookmark(void)
{
	u8 i;

	for(i=0; i<txt_num; i++)
	{
		W25QXX_Write((u8*)&bookmark[i],1024*1024*4+i*64+60,4);//写入flash
		printf("保存书签 %d：%ld\r\n",i,bookmark[i]);
	}
}
//保存书名数据到flash
void save_bookinfo(void)
{
	u8 i;

	for(i=0; i<txt_num; i++)
	{
		W25QXX_Write((u8*)txt_path_name[i],1024*1024*4+i*64,60);//写入flash
		printf("%s	",txt_path_name[i]);
	}W25QXX_Write((u8*)&txt_num,1024*1024*4+30*64,1);//写入flash
	printf("写入书数量：%d\r\n",txt_num);
}



//读出书签数据
void read_bookmark(void)
{
	u8 i;
	for(i=0; i<30; i++)
	{
		W25QXX_Read((u8 *)txt_path_name[i],1024*1024*4+i*64,60);//读出到内存
		W25QXX_Read((u8 *)&bookmark[i],1024*1024*4+i*64+60,4);//读出到内存
		printf("读出书签 %d：%ld\r\n",i,bookmark[i]);
	}W25QXX_Read((u8 *)&txt_num,1024*1024*4+30*64,1);//读出到内存
	printf("读出书数量：%d\r\n",txt_num);
}






