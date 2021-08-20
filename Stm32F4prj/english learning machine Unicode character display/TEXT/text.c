#include "sys.h" 
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"	
#include "string.h"												    
#include "usart.h"	
#include "touni.h"

/****************************************************************************

* Copyright (C), 2021-2022,֮��ʵ����

* �ļ���: text.c

* ���ݼ�����Ӣ��ѧϰ��Ver1.0.0.20210516_a������ä����ʾ

* ��ʾGBK�ַ�ʹ�ú��� Show_GBKFont

* ��ʾUTF-8�ַ�ʹ�� Show_UTF8Str

* ��ʾUnicode�ַ�ʹ�� Show_UNIStr

* ��ʾä�ĵ�ʹ�� DrawBraillePattern,�Ƽ�����x = 30�� y = 200

* �ļ���ʷ��

* �汾�� ���� ���� ˵��

* 1.0.0.20210516_a 2021-08-20 �¿��� �������ļ�


*/	 

void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
	{   		    
		for(i=0;i<csize;i++)*mat++=0x00;//�������
		return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//�õ��ֿ��е��ֽ�ƫ����  		  
	W25QXX_Read(mat,foffset+ftinfo.gbk_32_SONG_addr,csize);						
}
									    
 
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	   
void Show_GBKFont(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=32)return;	//��֧�ֵ�size
	Get_HzMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
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
//��ָ��λ�ÿ�ʼ��ʾһ��GBK�ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    	   		   
void Show_GBKStr(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //�ַ���������  	    				    				  	  
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//���� 
        {     
            bHz=0;//�к��ֿ�    
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  		
			Show_GBKFont(x,y,str,size,mode); //��ʾ�������,������ʾ 			
			str+=2; 
			x+=size;//��һ������ƫ��     	    
        }						 
    }   
}  			 		 
//��ָ�����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ���			  
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

//��ָ��λ�ÿ�ʼ��ʾһ��UNICODE�ַ���	    
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//size :�����С
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ    	   		   
void Show_UNIStr(u16 x,u16 y,u16 width,u16 height,u16*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
    u8 bHz=0;     //ASCII��UNICODE 	    				    				  	  
    while(*str!=0x00)//����δ����
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//��ASCII��UNICODE  
	        else              //�ַ�
	        {      
                if(x>(x0+width-size/2))//����
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//Խ�緵��      
		        if(*str==13)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*(u8*)str,size,mode);//��Ч����д�� 
				str++; 
		        x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
	        }
        }else//UNICODE 
        {     
            bHz=0;//��0  
            if(x>(x0+width-size))//����
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//Խ�緵��  		
			Show_UNIFont(x,y,str,size,mode); //��ʾ�������,������ʾ 			
			str++; 
			x+=size;//��һ������ƫ��     	    
        }						 
    }   
}  			 	

//��ʾһ��ָ����С��UNICODE
//x,y :���ֵ�����
//font:����GBK��
//size:�����С
//mode:0,������ʾ,1,������ʾ	   
void Show_UNIFont(u16 x,u16 y,u16 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[128];   
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
	if(size!=32)return;	//��֧�ֵ�size
	Get_UNIMat(font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//�õ���������                          
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
//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַUNICODE
//mat  ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С	
//size:�����С
void Get_UNIMat(u16 *code,unsigned char *mat,u8 size)
{		    
	u16 Uni_value;	
	u8 i=0;
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	Uni_value=*code;
	if(Uni_value>0xFFFF)//�� ���ú���
	{   		    
		for(i=0;i<csize;i++)*mat++=0x00;//�������
		return; //��������
	}          
	 
	foffset=((unsigned long)Uni_value)*csize;	//�õ��ֿ��е��ֽ�ƫ����  		  
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
    char k,l;//K:����10���֣�0-9��l���ֵĵ���0-12��12�����ʾ����һ����
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














		  





