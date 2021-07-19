#ifndef __CODEC_H__
#define __CODEC_H__

/* Save data callback type */
typedef bool (ivCBSaveData)(
	void*		pSrcBuffer,
	int			nBufferBytes
	);

/* Load data callback type */
typedef bool (ivCBLoadData)(
	void*		pDstBuffer,
	int* 	pnBufferBytes
	);

/* Save command callback type */
typedef bool (ivCBSendCommand)(
	void*		pSrcBuffer,
	int			nBufferBytes
	);

typedef enum
{
    Codec_encode,  // 录音编码模式
	Codec_decode   // 解码播音模式
}CODECMODEID;

//录音采样率，支持8k和16k两种
typedef enum
{
	SR_8000,
	SR_16000
}SAMPLERATEID;

//编解码bitrate设置
//编解码质量由低到高共6种
typedef enum
{
	BR_bitrate0,
	BR_bitrate1,
	BR_bitrate2,
	BR_bitrate3,
	BR_bitrate4,
	BR_bitrate5
}BITRATEID;

//编解码volume设置
typedef enum
{
	BR_volume0,
	BR_volume1,
	BR_volume2,
	BR_volume3,
	BR_volume4,
	BR_volume5,
	BR_volume6,
	BR_volume7,
	BR_volumeauto  // 仅编码时可用
}VOLUMEID;

void Codec_Start(CODECMODEID CodecMode, SAMPLERATEID SampleRate, BITRATEID BitRate, VOLUMEID Volume,
        ivCBLoadData pfLoadData, ivCBSaveData pfSaveData, ivCBSendCommand pfSendCommand);
void Codec_Stop(void);
void Codec_SetVolume(VOLUMEID Volume);
bool Codec_Byte_Proc(char ch);

#endif
