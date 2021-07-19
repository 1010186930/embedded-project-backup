#include "StdAfx.h"
#include "Codec.h"

static FILE *Fp;
static CODECMODEID g_systate;
static VOLUMEID g_volume;

//存储数据回调
//录音编码时候存储压缩数据
bool app_savedata(
	void*		pSrcBuffer,
	int			nBufferBytes
	)
{
	//windows平台存储文件
    fwrite(pSrcBuffer, 1, nBufferBytes, Fp);

	return 0;
}

//读取数据回调
//解码播音时候读取压缩数据
bool app_loadsave(
	void*		pDstBuffer,
	int* 	pnBufferBytes
	)
{
	//windows平台读取文件
    *pnBufferBytes = fwrite(pDstBuffer, 1, *pnBufferBytes, Fp);

	return 0;
}

//送命令回调
//负责将编解码的命令通过串口发送出去
bool app_sendcommand(
	void*		pSrcBuffer,
	int			nBufferBytes
	)
{
    //根据具体的平台，将buf中数据用串口发送出去
    
	return 0;
}

//获取串口接收数据
char UART_Get_byte(void)
{
	char ch;

	//从串口接收buf中取一个数据

	return ch;
}

// 启动录音编码
void StartEncode(void)
{    
    VOLUMEID volume = BR_volume7;
	
    g_systate = Codec_encode;
	g_volume = BR_volume7;
	
    Fp = fopen("codec.bts", "wb");

	//开始编码,设置采样率16k，比特率5，音量7
    Codec_Start(g_systate, SR_16000, BR_bitrate5, g_volume, NULL, app_savedata, app_sendcommand);

	while (1)
	{
	    //如果数据处理字节返回非0，有错误，退出
	    if (Codec_Byte_Proc(UART_Get_byte()))
	    {
	        Codec_Stop();
	    }

		//是否需要退出
		if (g_systate != Codec_encode)
		{
		    Codec_Stop();
		}

		//是否重设音量
		if (g_volume != volume)
		{
		    g_volume = volume;
		    Codec_SetVolume(g_volume);
		}
	}

	fclose(Fp);
}

void StartDecode(void)
{
    VOLUMEID volume = BR_volume7;

	// 解码播音样例
    g_systate = Codec_decode;
	g_volume = BR_volume7;
	
    Fp = fopen("codec.bts", "rb");
	
    Codec_Start(g_systate, SR_16000, BR_bitrate5, g_volume, app_loadsave, NULL, app_sendcommand);

	while (1)
	{
	    //如果数据处理字节返回非0，有错误或播音完成，退出
	    if (Codec_Byte_Proc(UART_Get_byte()))
	    {
	        Codec_Stop();
	    }

		//是否需要退出
		if (g_systate != Codec_encode)
		{
		    Codec_Stop();
		}

		//是否重设音量
		if (g_volume != volume)
		{
		    g_volume = volume;
		    Codec_SetVolume(g_volume);
		}
	}

	fclose(Fp);
}

