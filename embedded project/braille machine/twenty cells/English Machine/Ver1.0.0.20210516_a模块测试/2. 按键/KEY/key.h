#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: key.h

* 内容简述：英文学习机Ver1.0.0.20210516_a的按键宏定义

*

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-05-16 陈楷闻 创建该文件

*/



//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY_ENTER        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5)  	//KEY_ENTER按键PE5
#define KEY_RETURN       HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)  	//KEY_RETURN按键PE4
#define KEY_PREVIOUS     HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3) 	//KEY_PREVIOUS按键PE3
#define KEY_NEXT    	 HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)  	//KEY_NEXT按键PE6
#define KEY_BOOKMARK     HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_10)  	//KEY_BOOKMARK按键PI10

#define KEY_ENTER_PRESS			1
#define KEY_RETURN_PRESS		2
#define KEY_PREVIOUS_PRESS		3
#define KEY_NEXT_PRESS			4
#define KEY_BOOKMARK_PRESS		5	

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
