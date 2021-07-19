#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
  	 




//#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
//#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
//#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键3(WK_UP) 
//#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)//读取按键0
//#define KEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)
//#define KEY5  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)

//#define KEY0_PRES 	1	//KEY0按下
//#define KEY1_PRES	  3	//KEY1按下
//#define KEY2_PRES   2	//KEY_UP按下(即WK_UP/KEY_UP)
//#define KEY3_PRES 	4	//KEY0按下
//#define KEY4_PRES 	5
//#define KEY5_PRES 	6




//以下按键初始化供测试用
#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 
#define RETURN GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define BK     GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)

#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES	  2	//KEY1按下
#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)

#define RETURN_PRES 5	
#define BK_PRES		6











void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
#endif
