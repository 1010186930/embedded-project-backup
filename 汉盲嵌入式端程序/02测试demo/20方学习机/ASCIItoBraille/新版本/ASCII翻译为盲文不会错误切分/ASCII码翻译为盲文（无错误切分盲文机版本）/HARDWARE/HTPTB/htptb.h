#ifndef __HTPTB_H
#define __HTPTB_H
char *getcnchar(char *cnChar);
char *ptb(char *pystring);
char *ftb(char *bf);
void reverse(char s[]);
//unsigned char *text2b_lcd(char shuru[20]);
void text2b_lcd(char shuru[20]);
unsigned char binary(char *str_bin);
extern char gotbraille[13];
extern char btmp[10][13];

#endif
