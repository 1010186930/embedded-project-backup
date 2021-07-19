#include "readwrite.h"
#include "string.h"
extern u32 length;
extern u8 readbuf[100];
FRESULT times_Read(u8* path)//读 创建次数.txt(nums.txt)获取创建书签的次数
{
	DIR *dir;
	FIL *fil;
	FRESULT res;
	
	dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	fil=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	res=f_opendir(dir,(const TCHAR*)"3:");
	printf("key1 f_opendir res:%d\r\n",res);
	res=f_open(fil,(const TCHAR*)path,FA_READ);
	printf("key1 f_open res:%d\r\n",res);
	if(!res)
	{
		while(!res)
		{
			res=f_read(fil,readbuf,100,&br);
			printf("key1 f_read res:%d\r\n",res);
			if (res || br < 100) 
//			f_gets((TCHAR*)readbuf,121,fil);
				break;
		}				
	}
	f_close(fil);
	f_closedir(dir);
	length=strlen(readbuf);
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
	res=f_opendir(dir,(const TCHAR*)"3:");
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
	res=f_opendir(dir,(const TCHAR*)"3:");
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
u8 Scan_Rootdirectory(u8* path)
{
    u8 res=0;
    DIR*dir=0;
    FILINFO*finfo=0;
    u8*fn=0;
    int i=0;
    int cnt=0;
    dir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
    finfo =(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
    res=f_opendir(dir,(const TCHAR*)path);
	printf("key0 res:%d\r\n",res);
    if(res==0)
    {
        while(1)
        {
            res=f_readdir(dir,finfo);
            fn=(u8*)(finfo->fname);
            if(!finfo->fname[0]||res!=FR_OK) break;
            if(finfo->fattrib==AM_DIR)
            {
                while(*fn!='\0')
				{
					printf("第%d个文件夹的第%d个字节为:%x\r\n",cnt+1,i+1,*fn);
					i+=1;
					fn++;
				}
				cnt++;
				i=0;
            }
			
        }
    }
    f_closedir(dir);
    myfree(SRAMIN,dir);
    myfree(SRAMIN,finfo);
    return cnt;
}
