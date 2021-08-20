#include "segment.h"


/****************************************************************************

* Copyright (C), 2021-2022,之江实验室

* 文件名: segment.c

* 内容简述：英文学习机Ver1.0.0.20210516_a的最大逆向匹配分词

* 分词字典的最大字长为10

* 文件历史：

* 版本号 日期 作者 说明

* 1.0.0.20210516_a 2021-08-20 陈楷闻 创建该文件


*/	 

//void DevideString(unsigned short* dst, unsigned short* src,int len)
//{
//	unsigned short* pdst = dst;
//	unsigned short* psrc = src;
//	int isChange = 0;//means the current character is not a Chinese character
//	int res = 0;
//	int res_last = 0;
//	unsigned short* head = src;
//	unsigned short* end = src;
//	unsigned short* tmpbuf = NULL;
//	int tmp_len = 0;
//	int cnt = 0;
//	res_last = isChCharacter(*psrc);
//	while (len)
//	{
//		res = isChCharacter(*psrc);
//		if (res != res_last)
//			isChange = 1;
//		else
//			isChange = 0;
//		if (isChange)
//		{
//			end = psrc;
//			tmp_len = end - head;
//			tmpbuf = (unsigned short*)malloc(sizeof(unsigned short) * tmp_len);
//			for (cnt = 0; cnt < tmp_len; cnt++)
//			{
//				*(tmpbuf + cnt) = *(head + cnt);
//				printf("tmp+%d:%x\n", cnt,*(tmpbuf + cnt));
//			}
//			head = end;
//			/*segement*/
//
//			free(tmpbuf);
//			printf("************************\n");
//			*pdst++ = '|';
//			*pdst++ = *psrc++;
//		}
//		else
//		{
//			*pdst++ = *psrc++;
//		}
//		res_last = res;
//		len--;
//	}
//}

int Unicnt(unsigned short* src, int unilen)
{
	int cnt = 0;
	unsigned short* psrc = src;
	while ((*(psrc++) != 0x00) && (cnt < unilen))
		cnt++;
	psrc = NULL;
	return cnt;
}
void Unidisplay(unsigned short* src, int unilen)
{
	int i = 0;
	unsigned short* psrc = src;
	for (i = 0; i < unilen; i++)
		printf("src[%d]:%x\r\n", i, *(psrc + i));
	psrc = NULL;
}

int UniInvercpy(unsigned short* dst, unsigned short* src, int get_len, int tol_len)
{
	unsigned short* pdst = dst;
	unsigned short* psrc = src;
	int i = 0;
	int offset;
	while (pdst == NULL || psrc == NULL)
		return -2;
	if (get_len > tol_len)
		get_len = tol_len;
	offset = tol_len - get_len;
	for (i = 0; i < get_len; i++)
		*(pdst++) = *(psrc + offset + i);
	pdst = NULL;
	psrc = NULL;
	return offset;
}

void RangeDefine(int* low, int* high, int* baseaddress, int len)
{
	switch (len)
	{
	case 2:
		*low = 0;
		*high = TWO_CNT - 1;
		*baseaddress = TWO_START;
		break;
	case 3:
		*low = 0;
		*high = THREE_CNT - 1;
		*baseaddress = THREE_START;
		break;
	case 4:
		*low = 0;
		*high = FOUR_CNT - 1;
		*baseaddress = FOUR_START;
		break;
	case 5:
		*low = 0;
		*high = FIVE_CNT - 1;
		*baseaddress = FIVE_START;
		break;
	case 6:
		*low = 0;
		*high = SIX_CNT - 1;
		*baseaddress = SIX_START;
		break;
	case 7:
		*low = 0;
		*high = SEVEN_CNT - 1;
		*baseaddress = SEVEN_START;
		break;
	case 8:
		*low = 0;
		*high = EIGHT_CNT - 1;
		*baseaddress = EIGHT_START;
		break;
	case 9:
		*low = 0;
		*high = NINE_CNT - 1;
		*baseaddress = NINE_START;
		break;
	default:
		*low = 0;
		*high = 0;
		*baseaddress = 0;
		break;
	}
}
int Unicmp(unsigned short* dst, unsigned short* src, int src_cnt)
{
	int i;
	for (i = 0; i < src_cnt; i++)
	{
		if (*(src + i) == *(dst + i))
			continue;
		else if (*(dst + i) > * (src + i))
			return 1;// dst > src
		else
			return -1;//dst < src
	}
	return 0;//dst == src
}
int StringMatch(unsigned short* src, int length, FIL* pSeg)//被修改过，可能有问题
{
	unsigned short* psrc = src;
	unsigned short* unitmp = (unsigned short*)mymalloc(SRAMIN,sizeof(unsigned short) * length);
	int low, middle, high, baseaddress;
	int offset = 0;
	int k = 0;
	int res = 0;
	unsigned char is_smaller = 0;
	unsigned char is_larger = 0;
	if (psrc == NULL)
	{
		myfree(SRAMIN,unitmp);
		return -2;
	}
		
	if (unitmp == NULL)
	{
		printf("allocate error\n");
		return -2;
	}
	RangeDefine(&low, &high, &baseaddress, length);//define low, high and baseaddress value
	while (low <= high)//dichotomous method
	{
		middle = (low + high) / 2;
		offset = middle * WORD_ITEM_LEN + baseaddress;
		f_lseek(pSeg, offset);
		f_read(pSeg, unitmp, sizeof(unsigned short)*length, &br);
		/*printf("--------first locate unitmp--------\n");
		Unidisplay(unitmp, length);*/

		if (*unitmp == *psrc)
			break;
		else if (*unitmp > * psrc)
			high = middle - 1;
		else
			low = middle + 1;
	}
	if (low <= high)//find the first unicode matched
	{
		k = middle;
		while (*unitmp == *psrc)// find the following matched unicode
		{
			/*printf("***********k:%d************\n", k);
			Unidisplay(unitmp, length);*/
			psrc = src;
			res = Unicmp(unitmp, psrc, length);
			if (res == 0)
			{
				myfree(SRAMIN,unitmp);
				return 0;//find current length in table
			}
				
			else if (res == -1)//unitmp<src
			{
				is_smaller = 1;
				if (is_larger == 1)
				{
					myfree(SRAMIN,unitmp);
					return -1;
				}	
				k++;
			}
			else if (res == 1)//unitmp>src
			{
				is_larger = 1;
				if (is_smaller == 1)
				{
					myfree(SRAMIN,unitmp);
					return -1;
				}
				k--;
			}
			offset = k * WORD_ITEM_LEN + baseaddress;
			f_lseek(pSeg, offset);
			f_read(pSeg, unitmp, sizeof(unsigned short)*length,&br);
		}
		myfree(SRAMIN,unitmp);
		return -1;
	}
	else
	{
		myfree(SRAMIN,unitmp);
		return -1;
	}
		
	
	/*free(pname);*/
}
void AddSegSymbol(unsigned short* src, int addplace, int len)
{
	unsigned short* psrc = src;
	int i = 0;
	for (i = len - 1; i > addplace; i--)
	{
		*(psrc + i) = *(psrc + i - 1);
	}
	*(psrc + addplace) = 0x20;
}
int StringSegment(unsigned short* dst, unsigned short* src, int src_len,int* seg_dst_len,FIL *pSeg)
{
	unsigned short* pdst = dst;
	unsigned short* psrc = src;
	int get_len = 0;
	int seg_offset = 0;
	int i = 0;
	int res = -1;
	int dst_len = src_len;
	int addsegcnt = 0;
	unsigned short unitmp[100] = { 0x00 };
	for (i = 0; i < src_len; i++)
		*(pdst + i) = *(psrc + i);
	if (src_len == 1)
	{
		*seg_dst_len = 1;
		/*printf("---------print dst-----------\n");*/
		/*Unidisplay(pdst, dst_len);*/
		return 0;// one character
	}
	while (src_len > 1)
	{
		pdst = dst;
		if (src_len < WORD_ITEM_MAXCNT)
			get_len = src_len;
		else
			get_len = WORD_ITEM_MAXCNT;
		for (i = get_len; i >= 2; i--)
		{
			/*printf("*******GET %d*******\n", i);*/
			seg_offset = UniInvercpy(unitmp, psrc, i, src_len);
			/*printf("offset:%d\n", seg_offset);*/
			/*Unidisplay(unitmp, i);*/
			res = StringMatch(unitmp, i, pSeg);
			/*printf("is match %d\n", res);*/
			if (res == 0)
				break;
		}
		if (res == -1)
		{
			src_len -= 1;
			dst_len += 1;
			seg_offset = seg_offset + 1;
			/*printf("no found current offset:%d\n", seg_offset);*/
		}
		else if (res == 0)
		{
			/*seg_get = src_len - seg_offset;
			current_get += seg_get;
			printf("seg_get:%d,current_get:%d\n", seg_get,current_get);*/
			src_len -= i;
			/*printf("found current offset:%d\n", seg_offset);*/
			if (seg_offset != 0)
			{
				dst_len += 1;
			}
		}
		if (src_len != 0)
		{
			addsegcnt += 1;
			AddSegSymbol(pdst, seg_offset, dst_len);
		}
	}
	/*Unidisplay(pdst, dst_len);*/
	*seg_dst_len = dst_len;
	return addsegcnt;
}
int DevideString(unsigned short* dst, unsigned short* src, int len, FIL* pSeg)
{
	unsigned short* pdst = dst;
	unsigned short* psrc = src;
	int res = 0;
	int res_last = 0;
	unsigned short* head = src;
	unsigned short* end = src;
	unsigned short* tmpbuf = NULL;
	int tmp_len = 0;
	int tmp_seg_dstlen = 0;
	int addsegcnt = 0;
	int cnt = 0;
	res_last = isChCharacter(*psrc);
	while (len)
	{
		res = isChCharacter(*psrc);
		if (res!=res_last && res==0)
		{
			end = psrc;//first symbol not Chinese character
			tmp_len = end - head;
			tmpbuf = (unsigned short*)mymalloc(SRAMIN,sizeof(unsigned short) * tmp_len);
			if(tmpbuf==NULL)
			{
				printf("allocate error!\r\n");
				return -2;
			}
			for (cnt = 0; cnt < tmp_len; cnt++)
			{
				*(tmpbuf + cnt) = *(head + cnt);
				/*printf("tmp+%d:%x\n", cnt, *(tmpbuf + cnt));*/
			}
			/*printf("---------print pdst-----------\n");*/
			addsegcnt += StringSegment(pdst, tmpbuf, tmp_len,&tmp_seg_dstlen, pSeg);
			pdst += tmp_seg_dstlen;
			*pdst++ = *psrc;
			myfree(SRAMIN,tmpbuf);
		}
		else if(res!=res_last && res==1)
		{
			head = psrc;
		}
		else if(res==0)
		{
			*pdst++ = *psrc;
		}
		res_last = res;
		/*pdst++;*/
		psrc++;
		len--;
		if (len == 0 && res == 1 && res_last == 1)
		{
			end = psrc;
			tmp_len = end - head;
			tmpbuf = (unsigned short*)mymalloc(SRAMIN,sizeof(unsigned short) * tmp_len);
			for (cnt = 0; cnt < tmp_len; cnt++)
			{
				*(tmpbuf + cnt) = *(head + cnt);
				/*printf("tmp+%d:%x\n", cnt, *(tmpbuf + cnt));*/
			}
			/*printf("---------print pdst-----------\n");*/
			addsegcnt += StringSegment(pdst, tmpbuf, tmp_len, &tmp_seg_dstlen, pSeg);
			pdst += tmp_seg_dstlen;
			myfree(SRAMIN,tmpbuf);
		}
	}
	return addsegcnt;
}

