#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "exti.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验4
 外部中断-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
	    
    uart_init(115200);              //初始化USART
	EXTI_Init();                    //外部中断初始化
    LED_Init();                     //初始化LED 

    while(1)
    {
		
       
		//打印OK提示程序运行
		__HAL_UART_DISABLE(&UART1_Handler);
		printf("\r\nALIENTEK 阿波罗STM32F429开发板 串口实验\r\n\r\n"); 
		LED1=1;
        delay_ms(100);             //每隔1s打印一次 
		uart_init(115200);
		printf("\r\n123\r\n\r\n"); 		
		LED1=0;
		delay_ms(100);   
    }
}

