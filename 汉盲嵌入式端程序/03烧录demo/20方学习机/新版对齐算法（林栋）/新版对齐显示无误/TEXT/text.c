#include "sys.h"
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"
#include "string.h"
#include "usart.h"
#include "touni.h"
#include "tts.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板V3
//汉字显示 驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
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
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
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
//在指定位置开始显示一个字符串
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
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
			Show_Font(x,y,str,size,mode); //显示这个汉字,空心显示 			
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
    if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
    else
    {
        strlenth=(len-strlenth)/2;
        Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
    }
}
//LCD端,盲文点的LCD显示函数。
// x,y为左上角坐标。两行显示

void LCD_DLBraille(u16 x,u16 y,char braille[20][7]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    for(k=0; k<20; k++)
        for(l=0; l<6; l++)
//				LCD_Draw_Circle(x+32*(k%5)+8*(l/3)-1*(l/6),y+100*(k/5)+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。
            LCD_Draw_Circle(x+20*(k%10)+8*(l/3)-1*(1/6),y+80*(k/10)+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。

}
//圆点半径为2,一个字里方与方的间距7，使得看起来更直观，一方里面点距统一为8，两方所组成的盲字之间间距为9。
void LCD_PriBraille(u16 x,u16 y,char braille[10][13]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    for(k=0; k<10; k++)
        for(l=0; l<12; l++)
//				LCD_Draw_Circle(x+32*(k%5)+8*(l/3)-1*(l/6),y+100*(k/5)+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。
            LCD_Draw_Circle(x+40*(k%5)+8*(l/3)-1*(1/6),y+80*(k/5)+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。

}

//盲文点单行显示,盲文基础学习
void LCD_PriOLBraille(u16 x,u16 y,char braille[10][13]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    for(k=0; k<10; k++)
        for(l=0; l<12; l++)
            LCD_Draw_Circle(x+40*(k%10)+8*(l/3)-1*(l/6),y+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。

}
//小说文字演示
void LCD_OLBraille(u16 x,u16 y,char braille[20][7]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    for(k=0; k<20; k++)
        for(l=0; l<6; l++)
            LCD_Draw_Circle(x+20*(k%20)+8*(l/3)+1*(l/6),y+8*(l%3),2*(braille[k][l]-48));//该函数实现12点的统一有序的排布。

}
//W小说文字演示
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
//W盲文基础学习
void LCD_ShowBraille(u16 x,u16 y,char braille[10][13]) {
    char k,l;//K:字序。10个字，0-9；l：字的点序，0-12，12个点表示两方一个字
    int x0,y0;
    for(k=0; k<10; k++)
        for(l=0; l<12; l++)
        {

            x0=x+14*(l/3)+2*(l/6)+74*k;   //第一方与第三方间距x坐标74，一方里点0与点3相差14， 每方相差16
            y0=y+150*(k/10)+25*(l%3);
            if(braille[k][l]=='1')
                gui_fill_circle(x0,y0,3,BLUE);
            else LCD_Draw_Circle(x0,y0,2);
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
    while(*str!=0)//数据未结束
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


void Show_UTF8Str(u16 x,u16 y,u16 width,u16 height,u8* utfsrc, u16* unidst,u8 size,u8 len,u8 mode,int voiceflage)
{
	StringToUni(utfsrc,unidst,len);
	Show_UNIStr(x,y,width,height,unidst,size,mode);              // Device header
	if(voiceflage==1){SPeechUni(unidst);}
	
	
}


























