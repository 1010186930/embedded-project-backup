#include "translate.h"

/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: translate.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的汉盲翻译

* 支持单字和双字多音字

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-08-20 陈楷闻 创建该文件


*/

/** 
  * @name   TwoPloyphonicTranslate
  *
  * @brief  二分查表实现双字多音字翻译，如果没找到按照单字翻译
  *
  * @param  uni:待翻译Unicode数组 
  * @param  len:待翻译Unicode长度
  * @param  dot:翻译结果，盲文点序
  * @param  path:双字翻译表路径
  *
  * @retval 无
  */
void TwoPloyphonicTranslate(unsigned short* uni, unsigned char len, char* dot, char* path)
{
	FIL* filtwopoly = NULL;
	u8 res=0;
	int n = 0;
	int k = 0;
	int low, middle, high;
	int offset = 0;
	int fsize = 0;
	unsigned short unih = 0;
	unsigned short unil = 0;
	unsigned short tmph = 0;
	unsigned short tmpl = 0;
	unsigned char is_smaller = 0;
	unsigned char is_larger = 0;
	unsigned char two_polyphonic_get = 0;
	char* dot_tmp = (char*)mymalloc(SRAMIN,sizeof(char) * DANZI_ITEM - 4 + 1);
	char* pname = (char*)mymalloc(SRAMIN,sizeof(char) * 100);
	filtwopoly=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	if (pname == NULL)
	{
		printf("allocate error\n");
		return;
	}
	strcpy(pname, path);
	res=f_open(filtwopoly,pname, FA_READ);
	if (res!=0) 
	{
		printf("open dict file error !\n");
		return;
	}
	fsize = f_size(filtwopoly);
	for (n = 0; n < len; n++)
	{
		two_polyphonic_get = 0;
		is_larger = 0;
		is_smaller = 0;
		if (n != len - 1)
		{
			unih = *(uni + n);
			unil = *(uni + n + 1);
			low = 0;
			high = fsize / DANZI_ITEM - 1;
			memset(dot_tmp, '\0', DANZI_ITEM - 4 + 1);
			while (low <= high)
			{
				middle = (low + high) / 2;
				offset = middle * DANZI_ITEM;
				f_lseek(filtwopoly, offset);
				res=f_read(filtwopoly,&tmph, sizeof(unsigned short), &br);
				if(res!=0)
					return;
				if (tmph == unih)
				{
					break;
				}
				else if (tmph > unih)
				{
					high = middle - 1;
				}
				else
					low = middle + 1;
			}
			if (low <= high)
			{
				k = middle;
				f_read( filtwopoly,&tmpl, sizeof(unsigned short), &br);
				while (tmph == unih)
				{
					if (tmpl == unil)
					{
						f_gets(dot_tmp, DANZI_ITEM - 4 + 1, filtwopoly);
						strcat(dot, dot_tmp);
						n++;
						two_polyphonic_get = 1;
						break;
					}
					else if (tmpl < unil)
					{
						is_smaller = 1;
						if (is_larger == 1)
						{
							/*DanziTranslate(&unih, 1, dot, danzipath);*/
							two_polyphonic_get = 0;
							break;
						}
						k++;
						offset = k * DANZI_ITEM;
						f_lseek(filtwopoly, offset);
						f_read(filtwopoly,&tmph, sizeof(unsigned short),&br );
						f_read(filtwopoly,&tmpl, sizeof(unsigned short),&br );
					}
					else
					{
						is_larger = 1;
						if (is_smaller == 1)
						{
							/*DanziTranslate(&unih, 1, dot, danzipath);*/
							two_polyphonic_get = 0;
							break;
						}
						k--;
						offset = k * DANZI_ITEM;
						f_lseek(filtwopoly, offset);
						f_read(filtwopoly,&tmph, sizeof(unsigned short), &br);
						f_read(filtwopoly,&tmpl, sizeof(unsigned short), &br );
					}
				}
				if(two_polyphonic_get==0)
					DanziTranslate(&unih, 1, dot, "0:/danzitbl.txt");
			}
			else
			{
				DanziTranslate(&unih, 1, dot, "0:/danzitbl.txt");
			}
		}
		else
			DanziTranslate(uni+n, 1, dot, "0:/danzitbl.txt");
	}
	f_close(filtwopoly);
	myfree(SRAMIN,pname);
	myfree(SRAMIN,dot_tmp);
	myfree(SRAMIN,filtwopoly);
}

/** 
  * @name   DanziTranslate
  *
  * @brief  二分查表单字翻译为盲文，找不到输出空方
  *
  * @param  uni:待翻译Unicode数组 
  * @param  len:待翻译Unicode长度
  * @param  dot:翻译结果，盲文点序
  * @param  path:单字翻译表路径
  *
  * @retval 无
  */
void DanziTranslate(unsigned short* uni, unsigned char len, char* dot,char* path)
{
	FIL* fildanzi = NULL;
	u8 res;
	int n = 0;
	int low, middle, high;
	int offset = 0;
	int fsize = 0;
	unsigned short tmp=0;
	char* dot_tmp =(char*)mymalloc(SRAMIN,sizeof(char)* DANZI_ITEM-2+1) ;
	char* pname = (char*)mymalloc(SRAMIN,sizeof(char) * 100);
	fildanzi=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	if (pname == NULL)
	{
		printf("allocate error\n");
		return;
	}
	strcpy(pname, path);
	res=f_open(fildanzi,pname, FA_READ);
	if (res!=0) {
		printf("open error\r\n");
		return;
	}
	fsize = f_size(fildanzi);
	for (n = 0; n < len; n++)
	{
		low = 0;
		high = fsize / DANZI_ITEM - 1;
		memset(dot_tmp, '\0', sizeof(char)* DANZI_ITEM-2+1);
		while (low <= high)
		{
			middle = (low + high) / 2;
			offset = middle * DANZI_ITEM;
			f_lseek(fildanzi, offset);
			res=f_read(fildanzi,&tmp, sizeof(unsigned short), &br );
			if(res!=0)
				return;
			if (tmp == *(uni+n))
			{
				f_gets(dot_tmp, DANZI_ITEM - 2 + 1, fildanzi);
				strcat(dot, dot_tmp);
				break;
			}
			else if (tmp > * (uni+n))
			{
				high = middle - 1;
			}
			else
				low = middle + 1;
		}
		if (low > high)
			strcat(dot, "000000");
	}
	f_close(fildanzi);
	myfree(SRAMIN,fildanzi);
	myfree(SRAMIN,pname);
	myfree(SRAMIN,dot_tmp);
}


