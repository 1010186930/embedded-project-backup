#include "sys.h" 
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"	
#include "string.h"												    
#include "usart.h"	
#include "touni.h"

/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: text.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的文字盲文显示

* 显示GBK字符使用函数 Show_GBKFont

* 显示UTF-8字符使用 Show_UTF8Str

* 显示Unicode字符使用 Show_UNIStr

* 显示盲文点使用 DrawBraillePattern,推荐参数x = 30， y = 200

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-08-20 陈楷闻 创建该文件


*/	 

void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
		for(i=0;i<csize;i++)*mat++=0x00;//填充满格
		return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//得到字库中的字节偏移量  		  
	W25QXX_Read(mat,foffset+ftinfo.gbk_32_SONG_addr,csize);						
}
									    
 
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	   
void Show_GBKFont(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=32)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
//在指定位置开始显示一个GBK字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void Show_GBKStr(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //字符或者中文  	    				    				  	  
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库    
            if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//越界返回  		
			Show_GBKFont(x,y,str,size,mode); //显示这个汉字,空心显示 			
			str+=2; 
			x+=size;//下一个汉字偏移     	    
        }						 
    }   
}  			 		 
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_GBKStr(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_GBKStr(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}   

//在指定位置开始显示一个UNICODE字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void Show_UNIStr(u16 x,u16 y,u16 width,u16 height,u16*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //ASCII或UNICODE 	    				    				  	  
    while(*str!=0x00)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//非ASCII的UNICODE  
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*(u8*)str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//UNICODE 
        {     
            bHz=0;//归0  
            if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//越界返回  		
			Show_UNIFont(x,y,str,size,mode); //显示这个汉字,空心显示 			
			str++; 
			x+=size;//下一个汉字偏移     	    
        }						 
    }   
}  			 	

//显示一个指定大小的UNICODE
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	   
void Show_UNIFont(u16 x,u16 y,u16 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=32)return;	//不支持的size
	Get_UNIMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址UNICODE
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
void Get_UNIMat(u16 *code,unsigned char *mat,u8 size)
{		    
	u16 Uni_value;	
	u8 i=0;
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
	Uni_value=*code;
	if(Uni_value>0xFFFF)//非 常用汉字
	{   		    
		for(i=0;i<csize;i++)*mat++=0x00;//填充满格
		return; //结束访问
	}          
	 
	foffset=((unsigned long)Uni_value)*csize;	//得到字库中的字节偏移量  		  
	W25QXX_Read(mat,foffset+ftinfo.uni_32_SONG_addr,csize);						
}


void Show_UTF8Str(u16 x,u16 y,u16 width,u16 height,u8* utfsrc, u16* unidst,u8 size,u8 len,u8 mode)
{
	StringToUni(utfsrc,unidst,len);
	Show_UNIStr(x,y,width,height,unidst,size,mode);
}

void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
        if(len==0)return;
        LCD_Fill(x0,y0,x0+len-1,y0,color);        
}

void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{                                                                                          
        u32 i;
        u32 imax = ((u32)r*707)/1000+1;
        u32 sqmax = (u32)r*(u32)r+(u32)r/2;
        u32 x=r;
        gui_draw_hline(x0-r,y0,2*r,color);
        for (i=1;i<=imax;i++) 
        {
                if ((i*i+x*x)>sqmax)// draw lines from outside  
                {
                        if (x>imax) 
                        {
                                gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
                                gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
                        }
                        x--;
                }
                // draw lines from inside (center)  
                gui_draw_hline(x0-x,y0+i,2*x,color);
                gui_draw_hline(x0-x,y0-i,2*x,color);
        }
}


void LCD_WShowBraille(int n,u16 x,u16 y,char braille[][7]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    int x0,y0;
    for(k=0; k<n; k++)
        for(l=0; l<6; l++)
        {
            //x0=x+14*(l/3)+2*(l/6)+74*k;
            //y0=y+150*(k/10)+25*(l%3);

            x0=x+16*(l/3)+32*k+5*k;
            y0=y+25*(l%3)+150*(k/n);

            //x0=x+16*(l/3)+32*k+5*k;
            //y0=y+25*(l%3)+150*(k/20);
            if(braille[k][l]=='1')
                gui_fill_circle(x0,y0,3,BLUE);
            else LCD_Draw_Circle(x0,y0,2);
        }

}

void DrawBraillePattern(u16 x, u16 y, char* dot, int len)
{
	int cnt,i;
	char btmp[20][7]={""};
	cnt = len / 6;
	for(i = 0; i < cnt; i++)
		memcpy(btmp[i],dot+6*i,6);
	LCD_WShowBraille(cnt,x,y,btmp);
}














		  






