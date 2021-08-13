#ifndef __SEGMENT_H
#define __SEGMENT_H
#include "sys.h" 
#include "translate.h"
#define isChCharacter(a) (((a >= 0x4E00 && a <= 0x9FFF) || (a >= 0x3400 && a <= 0x4DBF) || (a >= 0xF900 && a <= 0xFAFF)) ? 1 : 0)
#define WORD_ITEM_LEN	32
#define WORD_ITEM_MAXCNT	9
#define TWO_CNT			179296
#define THREE_CNT		147828
#define FOUR_CNT		188628
#define FIVE_CNT		14620
#define SIX_CNT			7567
#define SEVEN_CNT		8138
#define EIGHT_CNT		1669
#define NINE_CNT		641
#define TWO_START		0
#define TWO_END			TWO_START + TWO_CNT * WORD_ITEM_LEN - 1//0x578BFF
#define THREE_START		TWO_END + 1//0x578C00
#define THREE_END		THREE_START	 + THREE_CNT * WORD_ITEM_LEN - 1//0x9FBA7F
#define FOUR_START		THREE_END + 1//0x9FBA80
#define FOUR_END		FOUR_START + FOUR_CNT * WORD_ITEM_LEN - 1
#define FIVE_START		FOUR_END + 1 //0XFBD500
#define FIVE_END		FIVE_START + FIVE_CNT * WORD_ITEM_LEN - 1
#define SIX_START		FIVE_END + 1//0x102F880
#define SIX_END			SIX_START + SIX_CNT * WORD_ITEM_LEN - 1
#define SEVEN_START		SIX_END + 1//0x106AA60
#define SEVEN_END		SEVEN_START + SEVEN_CNT * WORD_ITEM_LEN - 1
#define EIGHT_START		SEVEN_END + 1//0x10AA3A0
#define EIGHT_END		EIGHT_START + EIGHT_CNT * WORD_ITEM_LEN - 1
#define NINE_START		EIGHT_END + 1//0x10B7440
#define NINE_END		NINE_START + NINE_CNT * WORD_ITEM_LEN - 1
extern char segpath[100];
int DevideString(unsigned short* dst, unsigned short* src, int len, FIL* pSeg);
int StringSegment(unsigned short* dst, unsigned short* src, int src_len,int* seg_dst_len,FIL *pSeg);
void AddSegSymbol(unsigned short* src, int addplace, int len);
int StringMatch(unsigned short* src, int length, FIL* pSeg);
int Unicmp(unsigned short* dst, unsigned short* src, int src_cnt);
void RangeDefine(int* low, int* high, int* baseaddress, int len);
int UniInvercpy(unsigned short* dst, unsigned short* src, int get_len, int tol_len);
void Unidisplay(unsigned short* src, int unilen);
int Unicnt(unsigned short* src, int unilen);

#endif