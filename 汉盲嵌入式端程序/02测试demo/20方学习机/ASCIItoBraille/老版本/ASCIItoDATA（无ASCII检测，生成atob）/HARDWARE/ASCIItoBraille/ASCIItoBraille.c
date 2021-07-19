#include "ASCIItoBraille.h"
char* StrtoBraille(char ascii)
{
	char temp[7]="";
	if(ascii>='a'&&ascii<='z')
		ascii=ascii-32;
	else if(ascii<' '||ascii>'z')
		ascii=' ';
	ascii=ascii-' ';
	return asc2_braille[ascii];
}