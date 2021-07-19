#include "string.h"
#include "exti.h"
#include "dot_driver.h"

/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: dot_driver.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的CSK4002语音合成芯片点阵驱动函数和盲文二进制点序处理适配函数

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-18 陈楷闻 创建该文件

*/


/** 
  * @name   DOT_Init
  *
  * @brief  锁存器数据和LE的IO初始化
  *
  * @param  无
  *
  * @retval 无
  */
unsigned char InputBraille_IO[LATCH_NUM]="";
void DOT_Init()
{
	GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOD时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();           //开启GPIOH时钟
	__HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOG时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();           //开启GPIOI时钟
	__HAL_RCC_GPIOJ_CLK_ENABLE();           //开启GPIOJ时钟
    
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;            //PH6、PH10、PH12、PH13、PH14、PH15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_7;            //PI0、PI1、PI2、PI5、PI7
	HAL_GPIO_Init(GPIOI,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;            //PJ7、PJ12、PJ13、PJ14
	HAL_GPIO_Init(GPIOJ,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8;            //PG7、PG8
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_7;            //PD3、PD7
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;            //PC8、PC9
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_15;          //PB13、PB15
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	Dot_Control(ALLDOWN);
}

/** 
  * @name   Disable_Latch
  *
  * @brief  锁存器LE的失能
  *
  * @param  无
  *
  * @retval 无
  */
void Disable_Latch()
{
	LE0=0;
	LE1=0;
	LE2=0;
	LE3=0;
	LE4=0;
	LE5=0;
	LE6=0;
	LE7=0;
	LE8=0;
	LE9=0;
	LE10=0;
	LE11=0;
	LE12=0;
	LE13=0;
	LE14=0;
}

/** 
  * @name   Enable_Latch
  *
  * @brief  锁存器LE的使能
  *
  * @param  无
  *
  * @retval 无
  */
void Enable_Latch()
{
	LE0=1;
	LE1=1;
	LE2=1;
	LE3=1;
	LE4=1;
	LE5=1;
	LE6=1;
	LE7=1;
	LE8=1;
	LE9=1;
	LE10=1;
	LE11=1;
	LE12=1;
	LE13=1;
	LE14=1;
}

/** 
  * @name   Reset_DataPort
  *
  * @brief  锁存器Data的复位
  *
  * @param  无
  *
  * @retval 无
  */
void Reset_DataPort()
{
	D0=0;
	D1=0;
	D2=0;
	D3=0;
	D4=0;
	D5=0;
	D6=0;
	D7=0;
}

/** 
  * @name   Set_DataPort
  *
  * @brief  锁存器Data的置位
  *
  * @param  无
  *
  * @retval 无
  */
void Set_DataPort()
{
	D0=1;
	D1=1;
	D2=1;
	D3=1;
	D4=1;
	D5=1;
	D6=1;
	D7=1;
}

/** 
  * @name   Dot_Control
  *
  * @brief  20方点阵的全起或全落
  *
  * @param  command：ALLUP表示全上，ALLDOWN表示全下
  *
  * @retval 无
  */
void Dot_Control(Control_Command command)
{
	Enable_Latch();
	switch(command)
	{
		case ALLDOWN:
			Reset_DataPort();
			break;
		case ALLUP:
			Set_DataPort();
			break;
		default:
			break;
	}
	Disable_Latch();
}
void Cell_Control(unsigned char num)
{
	Disable_Latch();
	switch(num)
	{
		case 1:
		{
			Reset_DataPort();
			LE0=1;
			Set_DataPort();
			LE0=0;
			break;
		}
		case 2:
		{
			Reset_DataPort();
			LE1=1;
			Set_DataPort();
			LE1=0;
			break;
		}
		case 3:
		{
			Reset_DataPort();
			LE2=1;
			Set_DataPort();
			LE2=0;
			break;
		}
		case 4:
		{
			Reset_DataPort();
			LE3=1;
			Set_DataPort();
			LE3=0;
			break;
		}
		case 5:
		{
			Reset_DataPort();
			LE4=1;
			Set_DataPort();
			LE4=0;
			break;
		}
		case 6:
		{
			Reset_DataPort();
			LE5=1;
			Set_DataPort();
			LE5=0;
			break;
		}
		case 7:
		{
			Reset_DataPort();
			LE6=1;
			Set_DataPort();
			LE6=0;
			break;
		}
		case 8:
		{
			Reset_DataPort();
			LE7=1;
			Set_DataPort();
			LE7=0;
			break;
		}
		case 9:
		{
			Reset_DataPort();
			LE8=1;
			Set_DataPort();
			LE8=0;
			break;
		}
		case 10:
		{
			Reset_DataPort();
			LE9=1;
			Set_DataPort();
			LE9=0;
			break;
		}
		case 11:
		{
			Reset_DataPort();
			LE10=1;
			Set_DataPort();
			LE10=0;
			break;
		}
		case 12:
		{
			Reset_DataPort();
			LE11=1;
			Set_DataPort();
			LE11=0;
			break;
		}
		case 13:
		{
			Reset_DataPort();
			LE12=1;
			Set_DataPort();
			LE12=0;
			break;
		}
		case 14:
		{
			Reset_DataPort();
			LE13=1;
			Set_DataPort();
			LE13=0;
			break;
		}
		case 15:
		{
			Reset_DataPort();
			LE14=1;
			Set_DataPort();
			LE14=0;
			break;
		}
		default:
			break;		
	}
}
/** 
  * @name   Data_Write
  *
  * @brief  将处理过的盲文二进制点序按照排针和盲文点的连接顺序，写入Data的IO，并按序使能对应Latch的LE
  *
  * @param  InputToIO[LATCH_NUM]：存放转换为15个8位十进制120盲文二进制点序的数组
  *
  * @retval 无
  */
void Data_Write(unsigned char InputToIO[LATCH_NUM])
{
	Reset_DataPort();
	LE0=1;
	D0=Dec_To_Bit(InputToIO[0],8);
	D1=Dec_To_Bit(InputToIO[0],7);
	D2=Dec_To_Bit(InputToIO[0],6);
	D3=Dec_To_Bit(InputToIO[0],5);
	D4=Dec_To_Bit(InputToIO[0],4);
	D5=Dec_To_Bit(InputToIO[0],3);
	D6=Dec_To_Bit(InputToIO[0],2);
	D7=Dec_To_Bit(InputToIO[0],1);	
	LE0=0;
	
	Reset_DataPort();
	LE1=1;
	D0=Dec_To_Bit(InputToIO[1],8);
	D1=Dec_To_Bit(InputToIO[1],7);
	D2=Dec_To_Bit(InputToIO[1],6);
	D3=Dec_To_Bit(InputToIO[1],5);
	D4=Dec_To_Bit(InputToIO[1],4);
	D5=Dec_To_Bit(InputToIO[1],3);
	D6=Dec_To_Bit(InputToIO[1],2);
	D7=Dec_To_Bit(InputToIO[1],1);	
	LE1=0;
	
	Reset_DataPort();
	LE2=1;
	D0=Dec_To_Bit(InputToIO[2],8);
	D1=Dec_To_Bit(InputToIO[2],7);
	D2=Dec_To_Bit(InputToIO[2],6);
	D3=Dec_To_Bit(InputToIO[2],5);
	D4=Dec_To_Bit(InputToIO[2],4);
	D5=Dec_To_Bit(InputToIO[2],3);
	D6=Dec_To_Bit(InputToIO[2],2);
	D7=Dec_To_Bit(InputToIO[2],1);	
	LE2=0;
	
	Reset_DataPort();
	LE3=1;
	D0=Dec_To_Bit(InputToIO[3],8);
	D1=Dec_To_Bit(InputToIO[3],7);
	D2=Dec_To_Bit(InputToIO[3],6);
	D3=Dec_To_Bit(InputToIO[3],5);
	D4=Dec_To_Bit(InputToIO[3],4);
	D5=Dec_To_Bit(InputToIO[3],3);
	D6=Dec_To_Bit(InputToIO[3],2);
	D7=Dec_To_Bit(InputToIO[3],1);	
	LE3=0;
	
	Reset_DataPort();
	LE4=1;
	D0=Dec_To_Bit(InputToIO[4],8);
	D1=Dec_To_Bit(InputToIO[4],7);
	D2=Dec_To_Bit(InputToIO[4],6);
	D3=Dec_To_Bit(InputToIO[4],5);
	D4=Dec_To_Bit(InputToIO[4],4);
	D5=Dec_To_Bit(InputToIO[4],3);
	D6=Dec_To_Bit(InputToIO[4],2);
	D7=Dec_To_Bit(InputToIO[4],1);	
	LE4=0;
	
	Reset_DataPort();
	LE5=1;
	D0=Dec_To_Bit(InputToIO[5],8);
	D1=Dec_To_Bit(InputToIO[5],7);
	D2=Dec_To_Bit(InputToIO[5],6);
	D3=Dec_To_Bit(InputToIO[5],5);
	D4=Dec_To_Bit(InputToIO[5],4);
	D5=Dec_To_Bit(InputToIO[5],3);
	D6=Dec_To_Bit(InputToIO[5],2);
	D7=Dec_To_Bit(InputToIO[5],1);	
	LE5=0;
	
	Reset_DataPort();
	LE6=1;
	D0=Dec_To_Bit(InputToIO[6],8);
	D1=Dec_To_Bit(InputToIO[6],7);
	D2=Dec_To_Bit(InputToIO[6],6);
	D3=Dec_To_Bit(InputToIO[6],5);
	D4=Dec_To_Bit(InputToIO[6],4);
	D5=Dec_To_Bit(InputToIO[6],3);
	D6=Dec_To_Bit(InputToIO[6],2);
	D7=Dec_To_Bit(InputToIO[6],1);	
	LE6=0;
	
	Reset_DataPort();
	LE7=1;
	D0=Dec_To_Bit(InputToIO[7],8);
	D1=Dec_To_Bit(InputToIO[7],7);
	D2=Dec_To_Bit(InputToIO[7],6);
	D3=Dec_To_Bit(InputToIO[7],5);
	D4=Dec_To_Bit(InputToIO[7],4);
	D5=Dec_To_Bit(InputToIO[7],3);
	D6=Dec_To_Bit(InputToIO[7],2);
	D7=Dec_To_Bit(InputToIO[7],1);	
	LE7=0;
	
	Reset_DataPort();
	LE8=1;
	D0=Dec_To_Bit(InputToIO[8],8);
	D1=Dec_To_Bit(InputToIO[8],7);
	D2=Dec_To_Bit(InputToIO[8],6);
	D3=Dec_To_Bit(InputToIO[8],5);
	D4=Dec_To_Bit(InputToIO[8],4);
	D5=Dec_To_Bit(InputToIO[8],3);
	D6=Dec_To_Bit(InputToIO[8],2);
	D7=Dec_To_Bit(InputToIO[8],1);	
	LE8=0;
	
	Reset_DataPort();
	LE9=1;
	D0=Dec_To_Bit(InputToIO[9],8);
	D1=Dec_To_Bit(InputToIO[9],7);
	D2=Dec_To_Bit(InputToIO[9],6);
	D3=Dec_To_Bit(InputToIO[9],5);
	D4=Dec_To_Bit(InputToIO[9],4);
	D5=Dec_To_Bit(InputToIO[9],3);
	D6=Dec_To_Bit(InputToIO[9],2);
	D7=Dec_To_Bit(InputToIO[9],1);	
	LE9=0;
	
	Reset_DataPort();
	LE10=1;
	D0=Dec_To_Bit(InputToIO[10],8);
	D1=Dec_To_Bit(InputToIO[10],7);
	D2=Dec_To_Bit(InputToIO[10],6);
	D3=Dec_To_Bit(InputToIO[10],5);
	D4=Dec_To_Bit(InputToIO[10],4);
	D5=Dec_To_Bit(InputToIO[10],3);
	D6=Dec_To_Bit(InputToIO[10],2);
	D7=Dec_To_Bit(InputToIO[10],1);	
	LE10=0;
	
	Reset_DataPort();
	LE11=1;
	D0=Dec_To_Bit(InputToIO[11],8);
	D1=Dec_To_Bit(InputToIO[11],7);
	D2=Dec_To_Bit(InputToIO[11],6);
	D3=Dec_To_Bit(InputToIO[11],5);
	D4=Dec_To_Bit(InputToIO[11],4);
	D5=Dec_To_Bit(InputToIO[11],3);
	D6=Dec_To_Bit(InputToIO[11],2);
	D7=Dec_To_Bit(InputToIO[11],1);	
	LE11=0;
	
	Reset_DataPort();
	LE12=1;
	D0=Dec_To_Bit(InputToIO[12],8);
	D1=Dec_To_Bit(InputToIO[12],7);
	D2=Dec_To_Bit(InputToIO[12],6);
	D3=Dec_To_Bit(InputToIO[12],5);
	D4=Dec_To_Bit(InputToIO[12],4);
	D5=Dec_To_Bit(InputToIO[12],3);
	D6=Dec_To_Bit(InputToIO[12],2);
	D7=Dec_To_Bit(InputToIO[12],1);	
	LE12=0;
	
	Reset_DataPort();
	LE13=1;
	D0=Dec_To_Bit(InputToIO[13],8);
	D1=Dec_To_Bit(InputToIO[13],7);
	D2=Dec_To_Bit(InputToIO[13],6);
	D3=Dec_To_Bit(InputToIO[13],5);
	D4=Dec_To_Bit(InputToIO[13],4);
	D5=Dec_To_Bit(InputToIO[13],3);
	D6=Dec_To_Bit(InputToIO[13],2);
	D7=Dec_To_Bit(InputToIO[13],1);	
	LE13=0;
	
	Reset_DataPort();
	LE14=1;
	D0=Dec_To_Bit(InputToIO[14],8);
	D1=Dec_To_Bit(InputToIO[14],7);
	D2=Dec_To_Bit(InputToIO[14],6);
	D3=Dec_To_Bit(InputToIO[14],5);
	D4=Dec_To_Bit(InputToIO[14],4);
	D5=Dec_To_Bit(InputToIO[14],3);
	D6=Dec_To_Bit(InputToIO[14],2);
	D7=Dec_To_Bit(InputToIO[14],1);	
	LE14=0;
	Reset_DataPort();
	
}

/** 
  * @name   Braille_Display
  *
  * @brief  将输入的盲文二进制点序转换为能输入到IO口的十进制数，并输入到IO口
  *
  * @param  InputBraille：输入的盲文二进制点序，最多不超过120个
  * @param  InputBraille_Length：输入盲文二进制点序的长度，用于将其分组，每组6个字符
  * @param  InputToIO：存放转换处理后盲文二进制点序

  * @retval 无
  */
void Braille_Display(unsigned char* InputBraille,unsigned char InputBraille_Length,unsigned char InputToIO[LATCH_NUM])
{
	Process_BrailleBinary(InputBraille,InputBraille_Length,InputToIO);
	Data_Write(InputToIO);
}
