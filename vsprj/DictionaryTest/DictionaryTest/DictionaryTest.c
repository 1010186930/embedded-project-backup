#include "stdio.h"
#include "string.h"
#include "malloc.h"
#define DANZI_ITEM	32
#define max(a,b) ((a>=b)?a:b)
char danzipath[] = ".\\Debug\\FlashTable.txt";
char two_polyphonic_path[] = ".\\Debug\\polyphonictwo.txt";
int DictionaryItemCount(const char* path)
{
	FILE* fitem = NULL;
	char Itembuf[50] = "";
	int len = 0;
	int one_length = 0, two_length = 0, three_length = 0, four_length = 0, five_length = 0, six_length = 0, seven_length = 0, eight_length = 0, other_length = 0;
	fitem=fopen(path, "r");
	while (!feof(fitem)) {
		fgets(Itembuf, 50, fitem);
		len = strlen(Itembuf)-1;
		
		switch (len)
		{
		case 1*2:
			one_length++;
			break;
		case 2*2:
			two_length++;
			break;
		case 3*2:
			three_length++;
			break;
		case 4*2:
			four_length++;
			break;
		case 5*2:
			five_length++;
			break;
		case 6*2:
			six_length++;
			break;
		case 7*2:
			seven_length++;
			break;
		case 8*2:
			eight_length++;
			break;

		default:
			printf("other itembuf:%s\n", Itembuf);
			other_length++;
			break;
		}
	}
	fclose(fitem);
	printf("one lenght:%3d\n", one_length);
	printf("two lenght:%3d\n", two_length);
	printf("three lenght:%3d\n", three_length);
	printf("four lenght:%3d\n", four_length);
	printf("five lenght:%3d\n", five_length);
	printf("six lenght:%3d\n", six_length);
	printf("seven lenght:%3d\n", seven_length);
	printf("eight lenght:%3d\n", eight_length);
	printf("other lenght:%3d\n", other_length);
	
}
void DanziTranslate(unsigned short* uni, unsigned char len, char* dot,char* path)
{
	FILE* fildanzi = NULL;
	int n = 0;
	int low, middle, high;
	int offset = 0;
	int fplace = 0;
	int fsize = 0;
	unsigned short tmp=0;
	char* dot_tmp =(char*)malloc(sizeof(char)* DANZI_ITEM-2+1) ;
	char* pname = (char*)malloc(sizeof(char) * 100);
	if (pname == NULL)
	{
		printf("allocate error\n");
		return ;
	}
	strcpy(pname, path);
	fildanzi=fopen(pname, "r");
	if (fildanzi == (FILE*)NULL) {
		printf("open dict file error !\n");
		return;
	}
	fseek(fildanzi, 0, SEEK_END);
	fsize = ftell(fildanzi);
	fseek(fildanzi, 0, SEEK_SET);
	for (n = 0; n < len; n++)
	{
		low = 0;
		high = fsize / DANZI_ITEM - 1;
		memset(dot_tmp, '\0', sizeof(dot_tmp));
		while (low <= high)
		{
			middle = (low + high) / 2;
			offset = middle * DANZI_ITEM;
			fseek(fildanzi, offset, 0);
			fread(&tmp, sizeof(unsigned short), 1, fildanzi);
			if (tmp == *(uni+n))
			{
				fgets(dot_tmp, DANZI_ITEM - 2 + 1, fildanzi);
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
	fclose(fildanzi);
	free(pname);
	free(dot_tmp);
}
void TwoPloyphonicTranslate(unsigned short* uni, unsigned char len, char* dot, char* path)
{
	FILE* filtwopoly = NULL;
	int n = 0;
	int k = 0;
	int low, middle, high;
	int offset = 0;
	int fplace = 0;
	int fsize = 0;
	unsigned short unih = 0;
	unsigned short unil = 0;
	unsigned short tmph = 0;
	unsigned short tmpl = 0;
	unsigned char is_smaller = 0;
	unsigned char is_larger = 0;
	unsigned char two_polyphonic_get = 0;
	char* dot_tmp = (char*)malloc(sizeof(char) * DANZI_ITEM - 4 + 1);
	char* pname = (char*)malloc(sizeof(char) * 100);
	if (pname == NULL)
	{
		printf("allocate error\n");
		return;
	}
	strcpy(pname, path);
	filtwopoly = fopen(pname, "r");
	if (filtwopoly == (FILE*)NULL) {
		printf("open dict file error !\n");
		return;
	}
	fseek(filtwopoly, 0, SEEK_END);
	fsize = ftell(filtwopoly);
	fseek(filtwopoly, 0, SEEK_SET);
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
			memset(dot_tmp, '\0', sizeof(dot_tmp));
			while (low <= high)
			{
				middle = (low + high) / 2;
				offset = middle * DANZI_ITEM;
				fseek(filtwopoly, offset, 0);
				fread(&tmph, sizeof(unsigned short), 1, filtwopoly);
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
				fread(&tmpl, sizeof(unsigned short), 1, filtwopoly);
				while (tmph == unih)
				{
					if (tmpl == unil)
					{
						fgets(dot_tmp, DANZI_ITEM - 4 + 1, filtwopoly);
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
						fseek(filtwopoly, offset, 0);
						fread(&tmph, sizeof(unsigned short), 1, filtwopoly);
						fread(&tmpl, sizeof(unsigned short), 1, filtwopoly);
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
						fseek(filtwopoly, offset, 0);
						fread(&tmph, sizeof(unsigned short), 1, filtwopoly);
						fread(&tmpl, sizeof(unsigned short), 1, filtwopoly);
					}
				}
				if(two_polyphonic_get==0)
					DanziTranslate(&unih, 1, dot, danzipath);
			}
			else
			{
				DanziTranslate(&unih, 1, dot, danzipath);
			}
		}
		else
			DanziTranslate(uni+n, 1, dot, danzipath);
	}
	fclose(filtwopoly);
	free(pname);
	free(dot_tmp);
}
int main(void)
{
	char buf[201] = "";
	unsigned char tmp[] = "Äã";

	unsigned short unicode[] = {0x91CD,0x91CF,0x6797,0x680B,0x957F,0x6C5F,0x555,0x6c6c};
	unsigned char gbkl, gbkh;
	unsigned short gbktoken;
	unsigned char cnt= sizeof(unicode) / sizeof(unsigned short);
	TwoPloyphonicTranslate(unicode, cnt, buf, two_polyphonic_path);
	gbkh = tmp[0];
	gbkl = tmp[1];
	gbktoken = (unsigned short)(gbkh << 8) | gbkl;
	printf("unicode corresponding dot:%s\n", buf);
	//FILE* fildanzi = NULL;
	//fildanzi = fopen(danzipath, "r");
	//if (fildanzi == (FILE*)NULL) {
	//	printf("open dict file error !\n");
	//	return;
	//}
	//fseek(fildanzi, 0, SEEK_END);
	//int fsize = ftell(fildanzi);
	//fseek(fildanzi, 0, SEEK_SET);
	/*DictionaryItemCount("d:\\output\\ziguang.txt");*/
}