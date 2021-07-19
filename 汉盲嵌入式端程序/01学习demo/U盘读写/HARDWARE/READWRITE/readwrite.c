#include "readwrite.h"
extern u32 filelength;
extern u8 readbuf[100];
FRESULT times_Read(const u8*path)//读 创建次数.txt(nums.txt)获取创建书签的次数
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"3:");
	res=f_open(fil,(const TCHAR*)path,FA_READ);
	filelength=f_size(fil);
	if(!res)
	{
		while(!res)
		{
			res=f_read(fil,readbuf,100,&br);
			if (res || br < 100) 
				break;
		}				
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,dir);
	myfree(SRAMIN,fil);
	dir=NULL;
	fil=NULL;
	return res;	
}
FRESULT times_Write(const u8*path)//写 创建次数.txt(nums.txt)，增加创建书签的次数，写入成功后返回值为0
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	const char buf[]="我";
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"3:");
	res=f_open(fil,(const TCHAR*)path,FA_READ|FA_WRITE);
	printf("open ok\r\n");
	f_lseek(fil,f_size(fil));
	printf("f_lseek ok\r\n");
	if(!res)
	{
		res=f_write(fil,buf,2,&bw);	
		printf("write ok\r\n");		
	}
	f_close(fil);
	printf("f_close ok\r\n");
	f_closedir(dir);
	printf("f_closedir ok\r\n");
	myfree(SRAMIN,fil);
	printf("free fil ok\r\n");
	myfree(SRAMIN,dir);
	printf("free dir ok\r\n");
	dir=NULL;
	fil=NULL;
	return res;	
}
FRESULT times_Init(const u8*path)//初始化 创建次数.txt(nums.txt)，初始值为0，写入成功后返回值为0
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	u8 buf[]="你我他123abc";
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,"3:");
	if(!res)
	{
		res=f_open(fil,(const TCHAR*)path,FA_CREATE_NEW|FA_WRITE);
			if(!res)
			{
				do
				{
					res = f_write(fil,buf,strlen(buf),&bw);
					if(res)
						break;
				}
				while (bw <strlen(buf));
			}
	}
	f_close(fil);
	f_closedir(dir);
	myfree(SRAMIN,fil);
	myfree(SRAMIN,dir);
	dir=NULL;
	fil=NULL;
	return res;	
}
