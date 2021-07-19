#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "readwrite.h"
#include "usart.h"
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: exti.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的按键中断初始化和按键中断服务函数

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-16 陈楷闻 创建该文件

*/

//外部中断初始化
FRESULT res;
u32 length=0;
u8 readbuf[100]="";
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();           //开启GPIOI时钟
    
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;            //PE3、PE4、PE5、PE6
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    
    GPIO_Initure.Pin=GPIO_PIN_10;           //PI10
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);
    
    //中断线3-PE3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
    
    //中断线4-PE4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,1);       //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);             //使能中断线4
    
    //中断线5、6-PE5、PE6
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,2);       //抢占优先级为2，子优先级为2
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //使能中断线5和中短线6
    
    //中断线10-PI10
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,3);   //抢占优先级为2，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线10
	
    
}


void EXTI_DeInit(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();           //开启GPIOI时钟
    
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;            //PE3、PE4、PE5、PE6
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    
    GPIO_Initure.Pin=GPIO_PIN_10;           //PI10
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);
    
    //中断线3-PE3
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,0);       //抢占优先级为2，子优先级为0
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);             //使能中断线3
    
    //中断线4-PE4
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,1);       //抢占优先级为2，子优先级为1
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);             //使能中断线4
    
    //中断线5、6-PE5、PE6
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,2);       //抢占优先级为2，子优先级为2
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);             //使能中断线5和中短线6
    
    //中断线10-PI10
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,3);   //抢占优先级为2，子优先级为3
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);         //使能中断线10
	
    
}

//中断服务函数
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);//调用中断处理公用函数
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);//调用中断处理公用函数
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);//调用中断处理公用函数
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);//调用中断处理公用函数
}


//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	int cnt=0;
    delay_ms(100);      //消抖
    switch(GPIO_Pin)
    {
        case GPIO_PIN_3:
            if(KEY_PREVIOUS==0) 
            {
				cnt=Get_Parentdirectory();
				printf("Get_Parentdirectory:%d\r\n",cnt);
            }
            break;
        case GPIO_PIN_4:
            if(KEY_RETURN==0)  //LED1翻转
            {
                cnt=Confirm_Buttom(1); 
				printf("Confirm_Buttom 1:%d\r\n",cnt);				
            }
            break;
        case GPIO_PIN_5:
            if(KEY_ENTER==0)  //同时控制LED0,LED1翻转 
            {
                Get_Nextpage(161);
            }
            break;
		case GPIO_PIN_6:
			if(KEY_NEXT==0)  //同时控制LED0,LED1翻转 
			{
				Get_Nextpage(161);
			}
			break;
        case GPIO_PIN_10:
            if(KEY_BOOKMARK==0)  
            {
				cnt=Confirm_Buttom(0); 
				printf("Confirm_Buttom 0:%d\r\n",cnt);
            }
            break;
    }
}
