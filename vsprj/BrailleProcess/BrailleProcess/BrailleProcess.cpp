#include "stdio.h"
#include "string.h"
#define BRAILLE_CELL_NUM		20
#define CELL_DOT_NUM			6
#define LATCH_NUM				15

/**
  * @name   Inverse_BrailleCell
  *
  * @brief  将1~20方盲文转换为20~1方盲文
  *
  * @param  InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM+1]：分过组的盲文组

  * @retval 无
  */
void Inverse_BrailleCell(unsigned char InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM + 1])
{
	unsigned char i, j = 0;
	unsigned char value = 0;
	for (i = 0; i < BRAILLE_CELL_NUM / 2; i++)
	{
		for (j = 0; j < CELL_DOT_NUM; j++)
		{
			value = InputBrailleGroup[i][j];
			InputBrailleGroup[i][j] = InputBrailleGroup[BRAILLE_CELL_NUM - 1 - i][j];
			InputBrailleGroup[BRAILLE_CELL_NUM - 1 - i][j] = value;
		}
	}
}
/**
  * @name   Binary_To_Dec
  *
  * @brief  8位二进制转换为十进制
  *
  * @param  InputBinary输入的8位二进制字符串

  * @retval 返回二进制对应的十进制数
  */
unsigned char Binary_To_Dec(unsigned char* InputBinary)
{
	unsigned char index, dec = 0;
	const unsigned char set[] = { 1,2,4,8,16,32,64,128 };
	for (index = 8; index >= 1; index--)
	{
		if (*InputBinary == '1')
			dec += set[index - 1];
		InputBinary++;
	}
	return dec;

}
/**
  * @name   Change_DotSequence
  *
  * @brief  将点序由123456转换为321654，转换后每次获取8个点序，转换为十进制后存入IO_Input
  *
  * @param  InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM+1]：转换为20~1方的盲文二进制点序
  * @param  IO_Input[LATCH_NUM]：存放15个输入到IO的十进制值

  * @retval 无
  */
void Change_DotSequence(unsigned char InputBrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM + 1], unsigned char IO_Input[LATCH_NUM])
{
	unsigned char i, j, k = 0;
	unsigned char value = 0;
	unsigned char Braille_Get[9] = "";
	for (i = 0; i < BRAILLE_CELL_NUM; i += 4)
	{
		for (j = i; j < i + 4; j++)
		{
			value = InputBrailleGroup[j][2];
			InputBrailleGroup[j][2] = InputBrailleGroup[j][0];
			InputBrailleGroup[j][0] = value;
			value = InputBrailleGroup[j][5];
			InputBrailleGroup[j][5] = InputBrailleGroup[j][3];
			InputBrailleGroup[j][3] = value;
		}
		strncpy((char*)Braille_Get, (const char*)InputBrailleGroup[i], 6);
		strncat((char*)Braille_Get, (const char*)InputBrailleGroup[i + 1], 2);
		Braille_Get[8] = '\0';
		IO_Input[k] = Binary_To_Dec(Braille_Get);
		memset(Braille_Get, '\0', sizeof(Braille_Get));
		strncpy((char*)Braille_Get, (const char*)InputBrailleGroup[i + 1] + 2, 4);
		strncat((char*)Braille_Get, (const char*)InputBrailleGroup[i + 2], 4);
		Braille_Get[8] = '\0';
		IO_Input[k + 1] = Binary_To_Dec(Braille_Get);
		memset(Braille_Get, '\0', sizeof(Braille_Get));
		strncpy((char*)Braille_Get, (const char*)InputBrailleGroup[i + 2] + 4, 2);
		strncat((char*)Braille_Get, (const char*)InputBrailleGroup[i + 3], 6);
		Braille_Get[8] = '\0';
		IO_Input[k + 2] = Binary_To_Dec(Braille_Get);
		memset(Braille_Get, '\0', sizeof(Braille_Get));
		k += 3;
	}
}
void Process_BrailleBinary(unsigned char* InputBraille, unsigned char InputBraille_Length, unsigned char InputToIO[LATCH_NUM])
{
	unsigned char BrailleGroup[BRAILLE_CELL_NUM][CELL_DOT_NUM + 1] = { "" };
	unsigned char cnt = 0;
	unsigned char i = 0;
	cnt = InputBraille_Length / CELL_DOT_NUM;
	for (i = 0; i < cnt; i++)
		memcpy(BrailleGroup[i], InputBraille + CELL_DOT_NUM * i, CELL_DOT_NUM);
	Inverse_BrailleCell(BrailleGroup);
	Change_DotSequence(BrailleGroup, InputToIO);
}
/**
  * @name   Dec_To_Bit
  *
  * @brief  将十进制数按位显示
  *
  * @param  dec：8位无符号十进制数
  * @param  bit：要显示的位的值，从1到8

  * @retval 返回某一位的值：0或1
  */
unsigned char Dec_To_Bit(unsigned char dec, unsigned char bit)
{
	return (dec >> (bit - 1) & 1);
}
int main()
{
	unsigned char a;
	unsigned char InputBraille[121] = "101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110101101011010110";
	unsigned char InputToIO[15] = { 0x00 };
	Process_BrailleBinary(InputBraille, 120, InputToIO);
	for (int i = 0; i < 15; i++)
	{
		a = Dec_To_Bit(InputToIO[i], 8);
		a = Dec_To_Bit(InputToIO[i], 7);
		a = Dec_To_Bit(InputToIO[i], 6);
		a = Dec_To_Bit(InputToIO[i], 5);
		a = Dec_To_Bit(InputToIO[i], 4);
		a = Dec_To_Bit(InputToIO[i], 3);
		a = Dec_To_Bit(InputToIO[i], 2);
		a = Dec_To_Bit(InputToIO[i], 1);
	}
}
	 