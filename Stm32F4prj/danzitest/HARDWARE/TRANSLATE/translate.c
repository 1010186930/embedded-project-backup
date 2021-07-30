#include "translate.h"
void DanziTranslate(unsigned short* uni, unsigned char len, char* dot,char* path)
{
	FIL* fildanzi = NULL;
	u8 res;
	int n = 0;
	int low, middle, high;
	int offset = 0;
	int fplace = 0;
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

void GBKConvertUni(u8* gbkstr,u16* unistr)
{
	u16 gbktoken=0;
	vu8 gbkh,gbkl;
	u8* gbktmp=gbkstr;
	u16* unitmp=unistr;
	while(*gbktmp != '\0')
	{
		if(*gbktmp < 0x80)
		{
			*unitmp=ff_convert((WCHAR)(*gbktmp),1);
			gbktmp++;
			unitmp++;
		}
		else
		{
			gbkh=*gbktmp;
			gbkl=*(++gbktmp);
			gbktoken=(u16)(gbkh<<8)|gbkl;
			*unitmp=ff_convert(gbktoken,1);
			gbktmp++;
			unitmp++;
		}				
	}
	gbktmp=NULL;
	unitmp=NULL;
}
