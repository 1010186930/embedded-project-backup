#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "string.h"
#include "sdram.h"
#include "malloc.h"
#include "usmart.h"
#include "nand.h"    
#include "ftl.h"  
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验40
 NAND FLASH实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
    
int main(void)
{
	u8 key,t=0;
	
	u16 i;
	u8 *buf;
	u8 *backbuf;	
    int a=0x1234;
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART	
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //SDRAM初始化
    LCD_Init();                     //LCD初始化
    my_mem_init(SRAMIN);            //初始化内部内存池
    my_mem_init(SRAMEX);            //初始化外部SDRAM内存池
    my_mem_init(SRAMCCM);		    //初始化CCM内存池 
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2016/2/16");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");   
 	while(FTL_Init())			    //检测NAND FLASH,并初始化FTL
	{
		LCD_ShowString(30,190,200,16,16,"NAND Error!");
		delay_ms(500);				 
		LCD_ShowString(30,190,200,16,16,"Please Check");
		delay_ms(500);				 
		LED0=!LED0;//DS0闪烁
	}
//	NAND_ReadSpare(0,0,buf,4);
//	for(i=0;i<4;i++)
//	printf("spare 0:%d",*(buf+i));
//	for(i=0;i<nand_dev.block_totalnum;i++)
//	{
//		printf("lut[%d]:%d\r\n",i,nand_dev.lut[i]);
//	}
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//申请一个扇区的缓存
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//申请一个扇区的缓存
 	POINT_COLOR=BLUE;						//设置字体为蓝色 
	sprintf((char*)buf,"NAND Size:%dMB",(nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);
	LCD_ShowString(30,190,200,16,16,buf);	//显示NAND容量  
	FTL_ReadSectors(backbuf,1,NAND_ECC_SECTOR_SIZE,1);//预先读取扇区0到备份区域,防止乱写导致文件系统损坏.
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0按下,读取sector
				key=FTL_ReadSectors(buf,1,NAND_ECC_SECTOR_SIZE,1);//读取扇区
				if(key==0)//读取成功
				{
					LCD_ShowString(30,210,200,16,16,"USART1 Sending Data...  ");
					printf("Sector 2 data is:\r\n");
					for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)
					{
						printf("%x ",buf[i]);//输出数据
					}
					printf("\r\ndata end.\r\n");
					LCD_ShowString(30,210,200,16,16,"USART1 Send Data Over!  "); 
				}
				break;
			case KEY1_PRES://KEY1按下,写入sector
//				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//填充数据(随机的,根据t的值来确定) 
			buf[0]=(u8)a;
			buf[1]=(u8)(a>>8);
			buf[2]='\0';
		//strcpy((char*)(buf+2),"10101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101101010101010101010101010101010101010101010101101010101010101010101010101010101010101010101010101010101010101010101");
			
				LCD_ShowString(30,210,210,16,16,"Writing data to sector..");
				key=FTL_WriteSectors(buf,1,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Write data successed    ");//写入成功
				else LCD_ShowString(30,210,200,16,16,"Write data failed       ");//写入失败
				break;
			case KEY2_PRES://KEY2按下,恢复sector的数据
				LCD_ShowString(30,210,210,16,16,"Recovering data...      ");
				key=FTL_WriteSectors(backbuf,1,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Recovering data OK      ");//恢复成功
				else LCD_ShowString(30,210,200,16,16,"Recovering data failed  ");//恢复失败
				break;
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;
			t=0;
		}
	} 
}
