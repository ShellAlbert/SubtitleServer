/********************************************* 
Copyright (C), 2010-2020, DADI MEDIA Co., Ltd.
ModuleName:  MXF Parser Module
FileName:    CMXFParserModule.h  CMXFParserModule.cpp
Author:      xiaoliqun@dadimedia.com
Date:        2010/05/31
Version:     v1.0
Description: This module is used to parser MXF files.
Others:      None
History:
< Author >    			         < Date >             < Modification >
xiaoliqun@dadimedia.com          2010/05/31           Create this file. 
xiaoliqun@dadimedia.com          2010/06/02           Complete Audio MXF parser.
xiaoliqun@dadimedia.com          2010/06/07           Complete Video MXF parser.

*********************************************/
#ifndef _CMXFPARSER_MODULE_H_
#define _CMXFPARSER_MODULE_H_

class CMXFParser;
struct MxfInfo;
class C3DVideoMXFParser;

// The Mxf Parser Module interface class.
#ifdef KM_WIN32
//#ifdef _USRDLL
class __declspec (dllexport) CMXFParserModule
#else
class CMXFParserModule
#endif
{
public:
	CMXFParserModule();
	~CMXFParserModule();

	/************************************************* 
	Function:     InitAudioParser
	Description:  Init audio mxf parser.
	Input:        p_cFileName   Mxf file name.
	Output:       m_uFrameSum   The sum_frame_num of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_EssenceIsJpeg2000 This is a jpeg2000 mxf file.
	MXFPARSER_UnKnownType       Unknown file type.
	MXFPARSER_NoDcpEssence      This isn't a mxf file.
	Others:       None
	*************************************************/ 
	int InitAudioParser(const char *p_cFileName, 
		unsigned long &m_uFrameSum);

	/************************************************* 
	Function:     GetAudioInfo
	Description:  Get audio mxf information.
	Input:        None
	Output:       m_uSamplingRate     The sampling rate of mxf file.
	m_uChannelCount     The channel count of mxf file.
	m_uBitsPerSample    The BitsPerSample of mxf file.
	m_bCryptoFlag       The CryptoFlag of mxf file.
	m_cKeyID            The Key ID of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitAudio       No init Audio Mxf parser.
	Others:       None
	*************************************************/ 
	int GetAudioInfo(unsigned long &m_uSamplingRate, 
		unsigned long &m_uChannelCount,
		unsigned long &m_uBitsPerSample,
		bool &m_bHmacFlag,
		bool &m_bCryptoFlag, 
		char *m_cKeyID);

	/************************************************* 
	Function:     GetAudioFrameData
	Description:  Get audio mxf frame data.
	Input:        m_uFrameNumber   Input a frame number.
	Output:       m_cFrameData     Get frame data.
	m_uFrameLength   Get frame length.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitAudioMxf    Run InitMxfParser() first.
	MXFPARSER_CanNotReadAudioData    Can't read audio data.
	Others:       None
	*************************************************/ 
	/// XLQ:注意帧号从0开始，即第一帧的帧号为0，最后一帧为n-1
	int GetAudioFrameData(const unsigned long m_uFrameNumber, 
		char *m_cFrameData, 
		unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength);

	/************************************************* 
	Function:     InitVideoParser
	Description:  Init 2D video mxf parser.
	Input:        p_cFileName   Mxf file name.
	Output:       m_uFrameSum   The sum_frame_num of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_EssenceIsPCM This is a pcm mxf file.
	MXFPARSER_UnKnownType       Unknown file type.
	MXFPARSER_NoDcpEssence      This isn't a mxf file.
	Others:       None
	*************************************************/ 
	int InitVideoParser(const char *p_cFileName, 
		unsigned long &m_uFrameSum);

	/************************************************* 
	Function:     GetVideoInfo
	Description:  Get 2D video mxf information.
	Input:        None
	Output:       m_dAspectRatio      The aspect rate of mxf file.
	m_uWidthSize        The width size of video.
	m_uHeightSize       The height size of video.
	m_uFrameRate        The frame rate of mxf file.
	m_bCryptoFlag       The CryptoFlag of mxf file.
	m_cKeyID            The Key ID of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitVideo       No init Video Mxf parser.
	Others:       None
	*************************************************/ 
	int GetVideoInfo(double &m_dAspectRatio,
		unsigned long &m_uWidthSize,
		unsigned long &m_uHeightSize,
		unsigned long &m_uFrameRate,
		bool &m_bHmacFlag,
		bool &m_bCryptoFlag, 
		char *m_cKeyID);

	/************************************************* 
	Function:     GetVideoFrameData
	Description:  Get 2D video mxf frame data.
	Input:        m_uFrameNumber   Input a frame number.
	Output:       m_cFrameData     Get frame data.
	m_uFrameLength   Get frame length.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitVideoMxf    Run InitMxfParser() first.
	MXFPARSER_CanNotReadVideoData    Can't read video data.
	Others:       None
	*************************************************/ 
	/// XLQ:注意帧号从0开始，即第一帧的帧号为0，最后一帧为n-1
	int GetVideoFrameData(const unsigned long m_uFrameNumber, 
		char *m_cFrameData, 
		unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength);

	/************************************************* 
	Function:     Init3DVideoParser
	Description:  Init 3D video mxf parser.
	Input:        p_cFileName   Mxf file name.
	Output:       m_uFrameSum   The sum_frame_num of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_EssenceIsPCM      This is a pcm mxf file.
	MXFPARSER_UnKnownType       Unknown file type.
	MXFPARSER_NoDcpEssence      This isn't a mxf file.
	Others:       None
	*************************************************/
	int Init3DVideoParser(const char *p_cFileName, 
		unsigned long &m_uFrameSum);

	/************************************************* 
	Function:     Get3DVideoInfo
	Description:  Get 3D video mxf information.
	Input:        None
	Output:       m_dAspectRatio      The aspect rate of mxf file.
	m_uWidthSize        The width size of video.
	m_uHeightSize       The height size of video.
	m_uFrameRate        The frame rate of mxf file.
	m_bCryptoFlag       The CryptoFlag of mxf file.
	m_cKeyID            The Key ID of mxf file.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitVideo       No init Video Mxf parser.
	Others:       None
	*************************************************/
	int Get3DVideoInfo(double &m_dAspectRatio,
		unsigned long &m_uWidthSize,
		unsigned long &m_uHeightSize,
		unsigned long &m_uFrameRate, 
		bool &m_bHmacFlag,
		bool &m_bCryptoFlag, 
		char *m_cKeyID);

	/************************************************* 
	Function:     Get3DLeftFrameData
	Description:  Get 3D video mxf left eye frame data.
	Input:        m_uFrameNumber   Input a frame number.
	Output:       m_cFrameData     Get frame data.
	m_uFrameLength   Get frame length.
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_NoInitVideoMxf    Run InitMxfParser() first.
	MXFPARSER_CanNotReadVideoData    Can't read video data.
	Others:       None
	*************************************************/
	/// XLQ:注意帧号从0开始，即第一帧的帧号为0，最后一帧为n-1
	int Get3DFrameData(const unsigned long m_uFrameNumber, 
		char *m_cLeftFrameData, 
		unsigned long &m_uLeftFrameLength,
		unsigned int &m_uLeftPlaintextOffset,
		unsigned int &m_uLeftSourceLength,
		char *m_cRightFrameData, 
		unsigned long &m_uRightFrameLength,
		unsigned int &m_uRightPlaintextOffset,
		unsigned int &m_uRightSourceLength);


	/// XLQ:获得错误信息
	const char *GetErrorString();

private:

	CMXFParser *m_pAudioMxfParser;  
	const MxfInfo *m_pAudioMxfInfo;

	CMXFParser *m_pVideoMxfParser;   
	const MxfInfo *m_pVideoMxfInfo;

	C3DVideoMXFParser *m_pVideo3DMxfParser;   
	const MxfInfo *m_pVideo3DMxfInfo;


	/// XLQ:设置错误信息
	void SetErrorString(const char *errBuffer);

	/// XLQ:记录错误信息
	const char *errString;
};

#endif
