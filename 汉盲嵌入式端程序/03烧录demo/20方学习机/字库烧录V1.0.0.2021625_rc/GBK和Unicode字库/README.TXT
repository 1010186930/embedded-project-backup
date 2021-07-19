/**
  ******************************************************************************
  * @file    字库烧录

  * @author  Kevin Chen

  * 文件历史：

  * 版本号 日期 作者 说明

  * V1.0.0.2021625_rc  2021-6-25 14:50  Kevin Chen 完成该工程

  ******************************************************************************
  * @attention
  *
  * V1.0.0.2021625_rc 
	
  * 1. 添加GBK32和Unicode32宋体字库，删除了W25Q128前12M挂载的Fatfs文件系统，从0开始写入2839个扇区

  * 2. 需要有UNIGBK.BIN、GBK32_SONG.FON、UNICODE32_SONG.FON按照如下格式存放
         u8*const UNIGBK_PATH="/SYSTEM/FONT/UNIGBK.BIN";			//UNIGBK.BIN的存放位置
         u8*const GBK32SONG_PATH="/SYSTEM/FONT/GBK32_SONG.FON";		//GBK32宋体的存放位置
         u8*const UNI32SONG_PATH="/SYSTEM/FONT/UNICODE32_SONG.FON";	//UNICODE32宋体的存放位置			
  * 3. 目前该工程只支持从SD卡读取
  ******************************************************************************