#include "suocun.h"
#include "htptb.h"

#define suo1  PFout(0)			    //独立外设
#define suo2  PFout(1)
#define suo3  PFout(2)
#define suo4  PFout(3)
#define suo5  PFout(4)
#define suo6  PFout(5)
#define suo7  PFout(6)
#define suo8  PFout(7)				   
#define suo9  PFout(12)
#define suo10 PFout(13)
#define suo11 PFout(14)
#define suo12 PFout(15)
#define suo13 PGout(1)
#define suo14 PGout(2)
#define suo15 PGout(3) 
void DXdis(u16 dx[15]);
void SUOCUN_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOG, ENABLE);	 
	  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOF  0 1 2 3 4 5 6 7 12 13 14 15共12个IO口

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOG, &GPIO_InitStructure);					 //根据设定参数初始化GPIOG  1 2 3共3个IO口
	    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC,&GPIO_InitStructure);					 //根据设定参数初始化GPIOC 0 1 2 3 4 5 6 7共8个数据口
		GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
}
 
 void DX_ShowBraille(char tbtmp[][13]){

       u8 count_s;//计数器
	   u16 dx[15];		//
	 char gdtmp[9];//过渡数组gd
	 	 char ts[2][6];
	 int i,j,a,b,m,n,x,y,l,k;
	 
	 	 for(i=0;i<5;i++)
	 for(j=0;j<12;j++)
	 {
		 m=tbtmp[i][j];
		 tbtmp[i][j]=tbtmp[9-i][11-j];
		 tbtmp[9-i][11-j]=m;
		 //stbtmp[i*12+j]=tbtmp[i][j];
	 }
	 
	 for(count_s=0;count_s<10;count_s+=2){  //2个字24位为一个操作周期，5个周期10个字
		 
		  for(i=count_s;i<count_s+2;i++)
		 {
			 
		 n=tbtmp[i][2];
		 tbtmp[i][2]=tbtmp[i][0];
		 tbtmp[i][0]=n;
		 
		 x=tbtmp[i][5];
		 tbtmp[i][5]=tbtmp[i][3];
		 tbtmp[i][3]=x;
			 
			 y=tbtmp[i][8];
		 tbtmp[i][8]=tbtmp[i][6];
		 tbtmp[i][6]=y;
		 
		 k=tbtmp[i][11];
		 tbtmp[i][11]=tbtmp[i][9];
		 tbtmp[i][9]=k;

	 
			 
			 strncpy(ts[0],tbtmp[i],6);
		 for(j=0;j<3;j++)
		 {
			 a=ts[0][j];
			 ts[0][j]=ts[0][5-j];
			 ts[0][5-j]=a;
		 }
		 strncpy(tbtmp[i],ts[0],6);
		 strncpy(ts[1],tbtmp[i]+6,6);
		  for(j=0;j<3;j++)
		 {
			 b=ts[1][j];
			 ts[1][j]=ts[1][5-j];
			 ts[1][5-j]=b;
		 }
		 //strncpy(tbtmp[i],ts[0],6);
		 strncpy(tbtmp[i]+6,ts[1],6);//以前错在strncat(tbtmp[i],ts[1],6)此步相当于没有，
		 //tbtmp[i][13]='\0';
		 //strncat(tbtmp[i],ts[1],6);
		 tbtmp[i]==tbtmp[i];
		 
	 }
		 
		strncpy(gdtmp,tbtmp[count_s],8);//一个周期内的第一组转换，复制btmp[count_s]的前8位,完成第一组数据输出点采集	        //据说memcpy效率高，可复制中间包含'\0'的数据，strncpy好了吗只能复制字符串，strncpy，memcpy有可能出现内存重叠情况，程序出错可以考虑修改为memmove			
		gdtmp[8]='\0';//手动补\0做字符串结束标记符号,保证正确识别				                                              //memcpy(gdtmp,btmp[count_s],8);	
		reverse(gdtmp);//字符串倒转，用于计算机存储正确点位序列																						盲文点列中1~8（7，8为下一方的1，2）排列书写的顺序(低位在前高位在后)与对应D0~D7的赋值在数位上（从高到底）相反，所以倒转序列适应锁存器输出
		dx[3*count_s/2]=binary(gdtmp);//二进制字符串转十进制整型数，存于点显数组第一个元素中，用于I/O口赋值
		memset(gdtmp,'\0',sizeof(gdtmp));//完成一次存储操作后，将过渡数组清零。以采集第二组
		strncpy(gdtmp,tbtmp[count_s]+8,5);//第二组数据输出点采集，先复制第一组盲文点列的后5位已包含'\0'
		strncat(gdtmp,tbtmp[count_s+1],4);//再添加第二组盲文点列的前4个点序。完成第二组数据输出点的采集。
		gdtmp[8]='\0';//手动补'\0',虽然一般情况下strncat会在目标字符串末尾补\0
		reverse(gdtmp);//2
	   dx[3*count_s/2+1]=binary(gdtmp);//转十进制整数，存于点显数组的第二个元素中。
		memset(gdtmp,'\0',sizeof(gdtmp));//完成第二次存储操作，过渡数组清零以采集第三组
		strcpy(gdtmp,tbtmp[count_s+1]+4);//复制第二组盲文点列后9位(包含\0)点列。													灵活运用strcpy中的所要拷贝的字符串的首地址(地址btmp[count_s+1]+4为第二组盲文点列第4位(从0位开始计数)的地址，即后9位(包含\0)点列的起始地址)，并已补上\0。
		reverse(gdtmp);//3
		dx[3*count_s/2+2]=binary(gdtmp);//转十进制整数，存于点显数组的第三个元素中。
			//至此一个周期内的三个数据输出点阵转换采集完毕，并转为十进制数值存于点显数组中。	
		}	
//2.开始输出 
//逐个选通，选通下一个前先关闭当前锁存器，
//更新前清空数据输出口，置0。
		DXdis(dx);
	}
void DXdis(u16 dx[15]){//点显器整屏显示
//逐个选通，选通下一个前先关闭当前锁存器，
//更新前清空数据输出口，置0。
GPIO_Write(GPIOC,0x00);
suo1=1;
GPIO_Write(GPIOC,dx[0]);
suo1=0;        
GPIO_Write(GPIOC,0x00);
suo2=1;        
GPIO_Write(GPIOC,dx[1]);
suo2=0;        
GPIO_Write(GPIOC,0x00);
suo3=1;        
GPIO_Write(GPIOC,dx[2]);
suo3=0;        
GPIO_Write(GPIOC,0x00);
suo4=1;        
GPIO_Write(GPIOC,dx[3]);
suo4=0;       
GPIO_Write(GPIOC,0x00);
suo5=1;        
GPIO_Write(GPIOC,dx[4]);	
suo5=0;        
GPIO_Write(GPIOC,0x00);
suo6=1;        
GPIO_Write(GPIOC,dx[5]);
suo6=0;        
GPIO_Write(GPIOC,0x00);
suo7=1;        
GPIO_Write(GPIOC,dx[6]);
suo7=0;        
GPIO_Write(GPIOC,0x00);
suo8=1;        
GPIO_Write(GPIOC,dx[7]);
suo8=0;        
GPIO_Write(GPIOC,0x00);
suo9=1;        
GPIO_Write(GPIOC,dx[8]);
suo9=0;        
GPIO_Write(GPIOC,0x00);
suo10=1;       
GPIO_Write(GPIOC,dx[9]);
suo10=0;       
GPIO_Write(GPIOC,0x00);
suo11=1;       
GPIO_Write(GPIOC,dx[10]);
suo11=0;        
GPIO_Write(GPIOC,0x00);
suo12=1;       
GPIO_Write(GPIOC,dx[11]);
suo12=0;       
GPIO_Write(GPIOC,0x00);
suo13=1;       
GPIO_Write(GPIOC,dx[12]);
suo13=0;       
GPIO_Write(GPIOC,0x00);
suo14=1;       
GPIO_Write(GPIOC,dx[13]);
suo14=0;       
GPIO_Write(GPIOC,0x00);
suo15=1;       
GPIO_Write(GPIOC,dx[14]);
suo15=0;
GPIO_Write(GPIOC,0x00);
}


//满空方测试显示
void dis(char b){
	GPIO_Write(GPIOF,0xf0ff);//全打开锁存器 
	suo13=1;
	suo14=1;
	suo15=1;
	if(b){//全上
		GPIOC->BSRR=0xff;
		delay_ms(100);
	}

	else{  //全下
		GPIOC->BRR=0xff;//置零
				delay_ms(100);
		GPIOC->BRR=0xff;
	}
	GPIO_Write(GPIOF,0x00);//关闭锁存器 
	suo13=0;
	suo14=0;
	suo15=0;
}
