
#include "StdAfx.h"
#include "Codec.h"


//文件说明
//此文件函数全部用标准c编写，可直接移植到支持标准c的平台
//用户如无特殊需求，直接使用该文件
//如有疑问请与技术支持连续


#define CMD_START_ENCODE 0x41
#define CMD_START_DECODE 0x42
#define CMD_SEND_DECODE 0x43
#define CMD_STOP_CODEC 0x44	
#define CMD_SEND_VOLUME 0x45

typedef enum
{
	eRecvCommon,
	eRecvFrameLenH,
	eRecvFrameLenL,
	eRecvFrameCmd,
	eRecvFrameData
}ERecvDataType;

typedef struct tagCodecInfo
{
    char SampleRateId;
	char BitRateId;
	char Volume;
}CodecInfo;

typedef struct tagCommFrm
{
	char Header; // 主机发送为0xFD, 主机接收为0xFC
	char ComLenHH; // 命令长度高字节
	char ComLenLL; // 命令长度低字节
	char CommCode; // 命令码
	union
	{
		CodecInfo Param;
		char Data[64];
	}DataArea;
}CommFrame;

static ivCBSaveData 	*g_pfSaveData;
static ivCBLoadData 	*g_pfLoadData;
static ivCBSendCommand *g_pfSendCommand;
static char            g_tu8RecvFrame[70];
static int 			g_u16RecvBufIndex;
static int 			g_u16RecvFrameDataLen;
static int 			g_u32RecvFrameLen;
static int			    g_eRecvType;
static int             g_Mode;

//比特率ID有六种，取值为0~5,如下表enc_frame_byte
//音量设置：
//编码模式音量设置取值0~7, 设置8为录音模块自动调整
//解码模式设置值为0~7

//采样率和压缩比特率定义的每一帧字节数
static const char enc_frame_byte[][6] = 
{
	{   6,  15, 20, 28, 38,	62	},
	{   10, 15, 20, 25, 32, 42	},
};

static void ResetRecvBuf(void)
{
	g_u16RecvBufIndex = 0;
	g_u16RecvFrameDataLen = 0;
}

static bool Frame_Proc(void)
{
    CommFrame *pCommFrm = (CommFrame *)g_tu8RecvFrame;

	switch (pCommFrm->CommCode)
	{
		case 0x21:  /* 有错，大部分是串口速度不支持 */
		case 0x22:  /* 传输完毕 */
			return 1;

		case 0x23:  /* 向主机申请待解码数据 */
		    if (g_pfLoadData != NULL)
		    {
		        int len;
				int rd;
				
				next_scan:			
				rd = 4;
				g_pfLoadData(&len, &rd);
				if (4 != rd)
				{												
				    return 1;
				}
				else
				{
					if (62 < len)
					{
						goto next_scan;
					}
					else
					{											
						//送数据给5051芯片解码播放
						CommFrame commframe;
						int lentemp;

					    lentemp = len;
					    g_pfLoadData(&commframe.DataArea, &len);

						if (lentemp != len)
						{
						    return 1;
						}
						else
						{		
							int comlen = len+1;
							
							commframe.Header = 0xFD;
							commframe.ComLenHH = (char)(comlen>>8);
							commframe.ComLenLL = (char)comlen;
							commframe.CommCode = CMD_SEND_DECODE;
								
							if (NULL != g_pfSendCommand)
							{
							    g_pfSendCommand((void*)&commframe, len+4);
							}
						}
					}
				}
		    }
			break;

		case 0x20: // 收到一帧编码数据
		    if (g_pfSaveData != NULL)
		    {
		        g_u32RecvFrameLen -= 1; /* 去掉命令码一个字节 */

				if (g_u32RecvFrameLen <= 62)
				{
					g_pfSaveData(&(g_u32RecvFrameLen), 4);
					g_pfSaveData((void *)&pCommFrm->DataArea, g_u32RecvFrameLen);
				}
		    }
			break;

		default:
			break;
	}

	return 0;
}

//开始录音编解码功能
//SamplerateID 
//Bitrateid
//volume 
void Codec_Start(CODECMODEID CodecMode, SAMPLERATEID SampleRate, BITRATEID BitRate, VOLUMEID Volume,
        ivCBLoadData *pfLoadData, ivCBSaveData *pfSaveData, ivCBSendCommand *pfSendCommand)
{    
	CommFrame SendComm;
		
    g_pfLoadData = pfLoadData;
	g_pfSaveData = pfSaveData;
	g_pfSendCommand = pfSendCommand;	
		
	SendComm.Header = 0xFD;
	SendComm.ComLenHH = 0;
	SendComm.ComLenLL = 4;
	
	SendComm.DataArea.Param.SampleRateId = SampleRate;
	SendComm.DataArea.Param.BitRateId = BitRate;	
	SendComm.DataArea.Param.Volume = Volume;
	
	g_Mode = CodecMode;
	if (Codec_encode == CodecMode)
	{
		SendComm.CommCode = CMD_START_ENCODE;  // 开始编码
	}
	else
	{
		SendComm.CommCode = CMD_START_DECODE; // 开始解码
		SendComm.DataArea.Param.Volume &= 0x07;
	}

	if (NULL != g_pfSendCommand)
	{
	    g_pfSendCommand((void*)&SendComm, 7);
	}	
}

//编解码过程中可随时设置音量
void Codec_SetVolume(VOLUMEID Volume)
{
	CommFrame SendComm;
	
	SendComm.Header = 0xFD;
	SendComm.ComLenHH = 0;
	SendComm.ComLenLL = 1;
	SendComm.CommCode = CMD_SEND_VOLUME;
	SendComm.DataArea.Data[0] = Volume; // 重新设置音量
	
	if (NULL != g_pfSendCommand)
	{
	    g_pfSendCommand((void*)&SendComm, 5);
	}
}

//停止编解码功能
void Codec_Stop(void)
{
	CommFrame SendComm;

	SendComm.Header = 0xFD;
	SendComm.ComLenHH = 0;
	SendComm.ComLenLL = 1;
	SendComm.CommCode = CMD_STOP_CODEC;
	
	if (NULL != g_pfSendCommand)
	{
	    g_pfSendCommand((void*)&SendComm, 4);
	}
}

//编解码字节处理函数
bool Codec_Byte_Proc(char ch)
{
    //接收了一个字节数据，处理该数据			
	switch (g_eRecvType)
	{
	    case eRecvCommon:
			if ( 0xFC == ch )	// 一帧的起始
			{
				ResetRecvBuf();
				g_eRecvType = eRecvFrameLenH;
				g_tu8RecvFrame[g_u16RecvBufIndex++] = ch;
			}
			break;
		
		case eRecvFrameLenH:				
			g_tu8RecvFrame[g_u16RecvBufIndex++] = ch;
			g_u16RecvFrameDataLen = (ch << 8);
			g_eRecvType = eRecvFrameLenL;				
			break;

		case eRecvFrameLenL:
			g_tu8RecvFrame[g_u16RecvBufIndex++] = ch;
			g_u16RecvFrameDataLen |= (ch & 0x00FF);
			g_u32RecvFrameLen = g_u16RecvFrameDataLen;

			if (g_u16RecvFrameDataLen > 66) // 最长帧为66
			{
			    g_u16RecvFrameDataLen = 66;
			}			
			break;

		case eRecvFrameCmd:
			g_eRecvType = eRecvFrameData;
		case eRecvFrameData:
			g_tu8RecvFrame[g_u16RecvBufIndex++] = ch;
			g_u16RecvFrameDataLen--;
			if ( 0 == g_u16RecvFrameDataLen )
			{
			    if (Frame_Proc())
				{
					return 1;
				}
				
				g_eRecvType = eRecvCommon;
			}
			break;

		default:
			break;
	}		

	return 0;
}

