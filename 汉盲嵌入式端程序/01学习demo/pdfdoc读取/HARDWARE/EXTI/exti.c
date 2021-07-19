#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "readwrite.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//外部中断初始化
FRESULT res;
u32 filelength=0;
u8 readbuf[100]="";
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOH_CLK_ENABLE();               //开启GPIOH时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;     //PH2,3
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    
    
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
	
    GPIO_Initure.Pin=GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    //中断线2-PH2
    HAL_NVIC_SetPriority(EXTI2_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //使能中断线2
    
    //中断线3-PH3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,1);       //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,2);       //抢占优先级为2，子优先级为2
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);             //使能中断线4
    
}


//中断服务函数


void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//调用中断处理公用函数
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//调用中断处理公用函数
}

//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(100);      //消抖
	int cnt=0;
    switch(GPIO_Pin)
    {
        case GPIO_PIN_2:
            if(KEY1==0)  
            {
                res=times_Read("0:/测试.doc");
				if(res==0)
				{
					printf("Doc Length:%d\r\n",filelength);
					for(cnt=0;cnt<filelength;cnt++)
						printf("Doc Content:%2x\r\n",readbuf[cnt]);
				}	
            }
            break;
        case GPIO_PIN_3:
            if(KEY0==0)  
            {
                res=times_Read("0:/测试.pdf");
				if(res==0)
				{
					printf("PDF Length:%d\r\n",filelength);
					for(cnt=0;cnt<filelength;cnt++)
						printf("PDF Content:%2x\r\n",readbuf[cnt]);
				}	
            }
            break;
		case GPIO_PIN_13:
            if(KEY2==0)  
            {
                res=times_Read("0:/测试.txt");
				if(res==0)
				{
					printf("TXT Length:%d\r\n",filelength);
					for(cnt=0;cnt<filelength;cnt++)
						printf("TXT Content:%2x\r\n",readbuf[cnt]);
				}	
            }
            break;
    }
}
