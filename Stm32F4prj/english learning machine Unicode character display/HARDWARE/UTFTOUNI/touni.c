#include "touni.h"

unsigned int BytesOneUTF_Required(unsigned char str)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节  
	unsigned char chr = str;
	//如果不是ASCII码,应该是多字节符,计算字节数  
	if (chr >= 0x80)
	{
		if (chr >= 0xE0)
		{
			nBytes = 3;
		}
		else if (chr >= 0xC0)
		{
			nBytes = 2;
		}
		else
		{
			return 0;
		}
	}
	else
		nBytes = 1;
	return nBytes;
}
unsigned int UTF8toUni(const unsigned char* pInput, unsigned short* Unic, unsigned int utfbytes)
{
	// b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
	unsigned char b1, b2, b3;
	unsigned char* pOutput = (unsigned char*)Unic;
	*Unic = 0x0; // 把 *Unic 初始化为全零
	switch (utfbytes)
	{
	case 1:
		b1 = 0x00;
		b2 = *pInput;
		if (b2 <= 0x7F && b2 >= 0x00)
		{
			*pOutput = b2;
			*(pOutput + 1) = 0x00;
		}
		else
			return 0;
		break;
	case 2:
		b1 = *pInput;
		b2 = *(pInput + 1);
		if ((b2 & 0xC0) != 0x80)
			return 0;
		*pOutput = (b1 << 6) + (b2 & 0x3F);
		*(pOutput + 1) = (b1 >> 2) & 0x07;
		break;
	case 3:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80))
			return 0;
		*pOutput = (b2 << 6) + (b3 & 0x3F);
		*(pOutput + 1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
		break;
	default:
		return 0;
	}
	return utfbytes;
}
int StringToUni(unsigned char* Inputbuf, unsigned short* Outputbuf, unsigned int len)
{
	unsigned int utfbytes_Required = 0;
	unsigned int utfbytes = 0;
	unsigned int cnt = 0;
	int index = 0;
	unsigned char* pInputbuf = Inputbuf;
	unsigned char* p = pInputbuf;
	unsigned short* pOutputbuf = Outputbuf;
	unsigned char ch;
	unsigned char buf_Toconvert[4] = { 0x00 };
	while (cnt < len)
	{
		ch = *pInputbuf;
		index = 0;
		utfbytes_Required = BytesOneUTF_Required(ch);
		utfbytes = utfbytes_Required;
		if (cnt + utfbytes > len)
			return -1;
		memset(buf_Toconvert, 0x00, sizeof(buf_Toconvert) / sizeof(char));
		while (utfbytes_Required)
		{
			p = pInputbuf;
			buf_Toconvert[index] = *(p + index);
			index++;
			utfbytes_Required--;
		}
		UTF8toUni(buf_Toconvert, pOutputbuf, utfbytes);
		pOutputbuf++;
		pInputbuf += utfbytes;

		cnt += utfbytes;
	}
	return 1;
}
int UTF8_to_Unicode( unsigned char* utf_buf, unsigned short* uni_buf, unsigned int len)
{
	int res;
	if (utf_buf == NULL || uni_buf == NULL)
		return -1;
	else
		res=StringToUni(utf_buf, uni_buf, len);
	if (res == -1)
		return -1;
	else if(res == 1)
		return 1;
	return -2;
}
int GBKConvertUni(u8* gbkstr, u16* unistr,unsigned int len)
{
	u16 gbktoken = 0;
	u8 gbkh, gbkl;
	u8* gbktmp = gbkstr;
	u16* unitmp = unistr;
	unsigned int cnt = 0;
	while (cnt!=len)
	{
		if (*gbktmp < 0x80)
		{
			*unitmp = ff_convert((unsigned short)(*gbktmp), 1);
			gbktmp++;
			unitmp++;
			cnt++;
		}
		else
		{
			gbkh = *gbktmp;
			gbkl = *(++gbktmp);
			gbktoken = (u16)(gbkh << 8) | gbkl;
			*unitmp = ff_convert(gbktoken, 1);
			gbktmp++;
			unitmp++;
			cnt += 2;
		}
	}
	*unitmp=0x00;//unicode array is ended with 0x00
	gbktmp = NULL;
	unitmp = NULL;
	return 1;
}
int StrToUni(unsigned char* src, unsigned short* dst, int strtype,unsigned int len)//src len
{
	int res;
	if (strtype == UTF_8)
		res=UTF8_to_Unicode(src, dst, len);
	else if (strtype == GBK)
		res=GBKConvertUni(src, dst, len);
	else
		return 0;
	return res;
}
int RequireBytes(unsigned char str)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节  
	unsigned char chr = str;
	//如果不是ASCII码,应该是多字节符,计算字节数  
	if (chr >= 0x80)
	{
		if (chr >= 0xFC && chr <= 0xFD)
		{
			nBytes = 6;
		}
		else if (chr >= 0xF8)
		{
			nBytes = 5;
		}
		else if (chr >= 0xF0)
		{
			nBytes = 4;
		}
		else if (chr >= 0xE0)
		{
			nBytes = 3;
		}
		else if (chr >= 0xC0)
		{
			nBytes = 2;
		}
		else
		{
			return 0;
		}
	}
	else
		nBytes = 1;
	return nBytes;
}
int is_str_utf8(unsigned char* str)
{
	unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节  
	unsigned char chr = *str;
	int bAllAscii = 1;
	unsigned int i = 0;
	for (i=0; str[i] != '\0'; ++i)
	{
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx 
		if (nBytes == 0 && (chr & 0x80) != 0)
		{
			bAllAscii = 0;
		}

		if (nBytes == 0)
		{
			//如果不是ASCII码,应该是多字节符,计算字节数  
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
				{
					nBytes = 6;
				}
				else if (chr >= 0xF8)
				{
					nBytes = 5;
				}
				else if (chr >= 0xF0)
				{
					nBytes = 4;
				}
				else if (chr >= 0xE0)
				{
					nBytes = 3;
				}
				else if (chr >= 0xC0)
				{
					nBytes = 2;
				}
				else
				{
					return Unknown;
				}
				nBytes--;
			}
		}
		else
		{
			//多字节符的非首字节,应为 10xxxxxx 
			if ((chr & 0xC0) != 0x80)
			{
				return Unknown;
			}
			//减到为零为止
			nBytes--;
		}
	}

	//违返UTF8编码规则 
	if (nBytes != 0)
	{
		return Unknown;
	}

	if (bAllAscii)
	{ //如果全部都是ASCII, 也是UTF8
		return ASCII;
	}

	return UTF_8;
}
int is_str_gbk(unsigned char* str)
{
	unsigned int nBytes = 0;//GBK可用1-2个字节编码,中文两个 ,英文一个
	unsigned char chr = *str;
	int bAllAscii = 1; //如果全部都是ASCII,
	unsigned int i = 0;
	for (i=0; str[i] != '\0'; ++i) {
		chr = *(str + i);
		if ((chr & 0x80) != 0 && nBytes == 0) {// 判断是否ASCII编码,如果不是,说明有可能是GBK
			bAllAscii = 0;
		}
		if (nBytes == 0) {
			if (chr >= 0x80) {
				if (chr >= 0x81 && chr <= 0xFE) {
					nBytes = 2;// nBytes =+ 2;
				}
				else {
					return Unknown;
				}
				nBytes--;
			}
		}
		else {
			if (chr < 0x40 || chr>0xFE) {
				return Unknown;
			}
			nBytes--;
		}//else end
	}
	if (nBytes != 0) {   //违返规则
		return Unknown;
	}
	if (bAllAscii) { //如果全部都是ASCII, 不是GBK
		return ASCII;
	}
	return GBK;
}
int EncodeType(unsigned char* Inputbuf)
{
	int encodeType = Unknown;
	encodeType = is_str_utf8(Inputbuf);
	if (encodeType != -1)
	{
		if (encodeType == ASCII)
			return ASCII;
		else if (encodeType == UTF_8)
			return UTF_8;
	}
	else 
	{
		encodeType = is_str_gbk(Inputbuf);
		if (encodeType == GBK)
			return GBK;
		else if (encodeType == ASCII)
			return ASCII;
		else if (encodeType == Unknown)
			return Unknown;
	}
	return -2;
}
int AnalyzeTextType(const char* srcpath)
{
	FIL* fil_text = NULL;
	u8 consecutive_flag = 0;//连续的符合utf-8的
	u8 bytes_required;
	u8 res;
	int fsize,fplace;
	unsigned char tmp[7] = "";
	int text_type = Unknown;
	fil_text=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	if(fil_text==NULL)
	{
		printf("allocate failed\r\n");
		return -2;
	}
	res=f_open(fil_text, srcpath,FA_READ);
	if(res!=0)
		return -2;
	fsize = f_size(fil_text);
	fplace = f_tell(fil_text);
	while (fplace != fsize)
	{
		memset(tmp, '\0', sizeof(tmp) / sizeof(char));
		f_read(fil_text,tmp, sizeof(char), &br);
		bytes_required = RequireBytes(tmp[0]);
		fplace = f_tell(fil_text);
		if (bytes_required == 0)//neither in the range from 0xc0 to 0xf7 nor in the ASCII range,literally GBK
		{
			myfree(SRAMIN,fil_text);
			f_close(fil_text);
			return GBK;
		}
		else if (bytes_required > 1)//overlapped range in GBK or UTF-8
		{
			if (fsize - fplace < bytes_required - 1)
				return GBK;
			f_read(fil_text, tmp + 1, sizeof(char)*(bytes_required - 1), &br);
			text_type = EncodeType(tmp);
			if (text_type == UTF_8)
			{
				consecutive_flag++;
				if (consecutive_flag == 5)
				{
					myfree(SRAMIN,fil_text);
					f_close(fil_text);
					return UTF_8;
				}
			}
			else if (text_type == GBK)
			{
				myfree(SRAMIN,fil_text);
				f_close(fil_text);
				return  GBK;
			}
			else if (text_type == Unknown)
			{
				tmp[2] = 0x00;
				text_type = EncodeType(tmp);
				if (text_type == GBK)
				{
					myfree(SRAMIN,fil_text);
					f_close(fil_text);
					return GBK;
				}
				else
				{
					myfree(SRAMIN,fil_text);
					f_close(fil_text);
					return Unknown;
				}
			}
		}
		fplace = f_tell(fil_text);
	}
	f_close(fil_text);
	myfree(SRAMIN,fil_text);
	return UTF_8;
}
int Init_Uni(unsigned short* src, int unilen)
{
	int cnt = 0;
	unsigned short* psrc = src;
	for(cnt = 0; cnt < unilen; cnt++)
	{
		*(psrc + cnt) = 0x00;
	}
	psrc = NULL;
	return 0;
}
