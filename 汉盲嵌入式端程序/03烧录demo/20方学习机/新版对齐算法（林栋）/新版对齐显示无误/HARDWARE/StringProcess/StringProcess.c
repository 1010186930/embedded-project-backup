#include "StringProcess.h"
#include "tts.h"
char* BufDisplay(char* buffer,char *bufdisplay)
{
	char* q = buffer;
	char* p = bufdisplay;
	char* k = q;
	int tune = 0;
	while (*q != '\0')
	{
		if (*q >= 0x61 && *q <= 0x7A)
		{
			k = q;
			tune = *(++k);
			if (tune >= 0x31 && tune <= 0x34)
			{
				*p = *q;
				q += 2;
				p++;
			}
			else
			{
				*p = *q;
				p++;
				q++;

			}
		}
		else
		{
			*p = *q;
			p++;
			q++;
		}
	}
	*p = '\0';
	return bufdisplay;
}
char* ReplaceSubStr(const char* str, const char* srcSubStr, const char* dstSubStr, char* out)
{
	char* out_temp = out;
	int src_len = strlen(srcSubStr);//子串长度
	int dst_len = strlen(dstSubStr);//替换串长度
	int i=0;
	if (!str && !out)
	{
		return NULL;
	}
	if (!srcSubStr && !dstSubStr)
	{
		return out;
	}
	
	while (*str != '\0')//str是原字符串
	{
		if (*str == *srcSubStr)//srcSubStr是要查找的子串
		{
			//可能发生替换
			const char* str_temp = str;
			int flag = 0;
			for (i = 0; i < src_len; i++)
			{
				if (str_temp[i] != srcSubStr[i])
				{
					flag = 1;
					break;
				}
			}
			if (flag)
			{
				//说明不用替换
				*out_temp++ = *str++;
			}
			else
			{
				//说明要替换
				for (i = 0; i < dst_len; i++)
				{
					*out_temp++ = dstSubStr[i];
				}
				str = str + src_len;
			}
		}
		else
		{
			//不用替换
			*out_temp++ = *str++;
		}
	}
	*out_temp = 0;
	return out;
}
void SpecialSynthesis(char *str)
{
	if(strstr(str,"üē"))
		{
			
			SPeech("第十一课 韵母 uā uāi uī uō vē ér");
		}
		else if(strstr(str,"üān"))
		{
			
			SPeech("第十五课 韵母 vān ǖn iōng");
		}
		else if(!strcmp(str,"第一课 标调基本规则"))
		{
			SPeech("第一课 标掉基本规则");
		}
		else if(!strcmp(str,"标调规则教程"))
		{
			SPeech("标掉规则教程");
		}
		else if(!strcmp(str,"通用盲文标调规则教程"))
		{
			SPeech("通用盲文标掉规则教程");
		}
		else
			SPeech(str);
}
void SpecialSynthesisInBk(char * synbuf, char * srcbuf)
{
	char *q;
	if(strstr(srcbuf,"üē"))
		{
			q = strchr(srcbuf, ' ');
			*(q+1) = '\0';
			strcpy(synbuf, srcbuf);//
			strcat(synbuf, "第十一课 韵母uā uāi uī uō vē ér");
			
			SPeech(synbuf);
		}
		else if(strstr(srcbuf,"üān"))
		{
			q=strchr(srcbuf,' ');
			*(q+1) = '\0';
			strcpy(synbuf, srcbuf);//
			strcat(synbuf,"第十五课 韵母 vān ǖn iōng");
			
			SPeech(synbuf);
		}	
		else if(strstr(srcbuf,"标调基本规则"))
		{
			q=strchr(srcbuf,' ');
			*(q+1) = '\0';
			strcpy(synbuf, srcbuf);//
			strcat(synbuf,"第一课 标掉基本规则");
			
			SPeech(synbuf);
		}	
		else	
		SPeech(srcbuf);
}
