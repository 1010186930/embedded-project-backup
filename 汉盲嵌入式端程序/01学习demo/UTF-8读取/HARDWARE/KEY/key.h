#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//KEY驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//下面的方式是通过位带操作方式读取IO
//#define KEY0        PHin(3) //KEY0按键PH3
//#define KEY1        PHin(2) //KEY1按键PH2
//#define KEY2        PCin(13)//KEY2按键PC13
//#define WK_UP       PAin(0) //WKUP按键PA0


//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0按键PH3
#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1按键PH2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2按键PC13
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define WKUP_PRES   4

extern unsigned char key_0_status;
extern unsigned char key_wkup_status;


/** 
  * @brief 注册按键最大个数
  */
#define Key_Num_Max		6


/** 
  * @brief 按键触发模式状态枚举
  */
typedef enum
{
	N_Trig = 0, 																				/*!< 0 空 */		
	L_Trig ,																						/*!< 1 低电平触发 */			
	H_Trig,																							/*!< 2 高电平触发 */
}Trig_Mode_TypeDef;

/** 
  * @brief 按键模式状态枚举
  */
typedef enum
{
	N_Click = 0,																				/*!< 0 空 */	
	S_Click ,																						/*!< 1 单击 */			
	D_Click,																						/*!< 2 双击 */
	L_Press,																						/*!< 3 长按 */
}Key_Mode_TypeDef;


/** 
  * @name   Init_Key_Struct
  *
  * @brief  初始化按键
  *
  * @param  Update_Key_CallBack：更新按键状态
  * @param  Debug_CallBack：打印按键调试信息
  *
  * @retval 0：成功；
	*					1：Update_Key_CallBack == NULL；
  */
char Init_Key_Struct(void (*Update_Key_CallBack)(void),void (*Debug_CallBack)(unsigned char *debug_mess));

/** 
  * @name   Reg_Key
  *
  * @brief  添加注册按键（注：如果按键已经注册过，那么再次注册会覆盖之前注册过的相同的按键）
  *
  * @param  key_s：按键状态
  * @param  count：按键计数
  * @param  Trig_Mode_E：按键触发模式
  * @param  Key_Mode_E：按键模式
  * @param  Key_Click_CallBack：按键触发回调
  *
  * @retval 0：成功；
	*					1：Key_Click_CallBack == NULL；
	*					2：Key.Reg_Key_Num > Key_Num_Max；
  */
char Reg_Key(unsigned char *key_s,const unsigned short count, 
	Trig_Mode_TypeDef Trig_Mode_E, Key_Mode_TypeDef  Key_Mode_E, void (*Key_Click_CallBack)(void));

/** 
  * @name   Key_Detect
  *
  * @brief  按键检测
  *
  * @param  无
  *
  * @retval 0：成功；
	*					1：Key.Update_Key_CallBack == NULL；
  */
char Key_Detect(void);

/** 
  * @name   Get_Version_Mess
  *
  * @brief  打印Key_Detect组件版本信息
  *
  * @param  无
  *
  * @retval 返Key_Detect组件版本信息
  */
char *Get_Version_Mess(void);
void Key_Registration(void);
void key0_S_CallBack(void);
void key0_D_CallBack(void);
void key0_L_CallBack(void);
void keywkup_S_CallBack(void);
void keywkup_D_CallBack(void);
void keywkup_L_CallBack(void);


void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
