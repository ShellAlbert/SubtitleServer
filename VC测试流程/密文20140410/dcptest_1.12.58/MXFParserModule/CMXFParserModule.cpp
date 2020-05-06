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
#include "CMXFParser.h"
#include "CMXFParserModule.h"
#include "ResultMXFParser.h"

//const int MXFPARSER_NoInitVideo = 0xE1011001;            // Run InitMxfParser() first.
const char *MXFPARSER_NoInitVideo_ERRSTR = "需要先初始化视频MXF文件。";

//const int MXFPARSER_NoInitAudio = 0xE1011002;            // Run InitMxfParser() first.
const char *MXFPARSER_NoInitAudio_ERRSTR = "需要先初始化音频MXF文件。";

//const int MXFPARSER_OutOfMemory = 0xE1011003;            // Can't allocate memory.
const char *MXFPARSER_OutOfMemory_ERRSTR = "MXF文件解析模块中无法分配内存。";

//const int MXFPARSER_UnKnownType = 0xE1011004;            // Unknown file type.
const char *MXFPARSER_UnKnownType_ERRSTR = "无法识别的MXF文件类型。";

//const int MXFPARSER_NoDcpEssence = 0xE1011005;           // This isn't a mxf file.
const char *MXFPARSER_NoDcpEssence_ERRSTR = "该文件不是MXF文件。";

//const int MXFPARSER_EssenceIsPcm = 0xE1011006;           // This is a pcm mxf file.
const char *MXFPARSER_EssenceIsPcm_ERRSTR = "该文件不是音频MXF文件。";

//const int MXFPARSER_NoInitVideoMxf = 0xE1011007;         // No init Video Mxf parser.
const char *MXFPARSER_NoInitVideoMxf_ERRSTR = "该文件不是视频MXF文件";

//const int MXFPARSER_CanNotOpenVideo = 0xE1011008;        // Can't open video file.
const char *MXFPARSER_CanNotOpenVideo_ERRSTR = "无法打开视频文件。";

//const int MXFPARSER_CanNotFillVideoDesc = 0xE1011009;    // Can't fill video desc.
const char *MXFPARSER_CanNotFillVideoDesc_ERRSTR = "无法填充视频描述。";

//const int MXFPARSER_CanNotFillVideoInfo = 0xE1011010;   // Can't fill video info.
const char *MXFPARSER_CanNotFillVideoInfo_ERRSTR = "无法填充视频信息。";

//const int MXFPARSER_CanNotReadVideoData = 0xE1011011;   // Can't read video data.
const char *MXFPARSER_CanNotReadVideoData_ERRSTR = "无法读取视频数据。";

//const int MXFPARSER_EssenceIsJpeg2000 = 0xE1011012;     // This is a jpeg2000 mxf file.
const char *MXFPARSER_EssenceIsJpeg2000_ERRSTR = "这是一个Jpeg2000的MXF文件。";

//const int MXFPARSER_NoInitAudioMxf = 0xE1011013;        // No init Audio Mxf parser.
const char *MXFPARSER_NoInitAudioMxf_ERRSTR = "没有初始化音频MXF文件。";

//const int MXFPARSER_CanNotFillAudioDesc = 0xE1011014;   // Can't fill audio desc.
const char *MXFPARSER_CanNotFillAudioDesc_ERRSTR = "无法填充音频描述。";

//const int MXFPARSER_CanNotFillAudioInfo = 0xE1011015;   // Can't fill audio info.
const char *MXFPARSER_CanNotFillAudioInfo_ERRSTR = "无法填充音频信息。";

//const int MXFPARSER_CanNotOpenAudio = 0xE1011016;       // Can't open audio file.
const char *MXFPARSER_CanNotOpenAudio_ERRSTR = "无法打开音频文件。";

//const int MXFPARSER_CanNotReadAudioData = 0xE1011017;   // Can't read audio data.
const char *MXFPARSER_CanNotReadAudioData_ERRSTR = "无法读取音频数据。";

//const int MXFPARSER_VideoFrameNumInvalid = 0xE1011018;  // The video frame number is invalid.
const char *MXFPARSER_VideoFrameNumInvalid_ERRSTR = "视频帧数无效。";

//const int MXFPARSER_AudioFrameNumInvalid = 0xE1011019;  // The audio frame number is invalid.
const char *MXFPARSER_AudioFrameNumInvalid_ERRSTR = "音频帧数无效。";


CMXFParserModule::CMXFParserModule():
m_pAudioMxfParser(NULL), 
	m_pAudioMxfInfo(NULL),
	m_pVideoMxfParser(NULL), 
	m_pVideoMxfInfo(NULL), 
	m_pVideo3DMxfParser(NULL),
	m_pVideo3DMxfInfo(NULL)
{

}

CMXFParserModule::~CMXFParserModule()
{
	if (NULL == m_pAudioMxfParser)
	{

	}
	else
	{
		delete m_pAudioMxfParser;
	}

	if (NULL == m_pVideoMxfParser)
	{

	}
	else
	{
		delete m_pVideoMxfParser;
	}

	if (NULL == m_pVideo3DMxfParser)
	{

	}
	else
	{
		delete m_pVideo3DMxfParser;
	}
}

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
int CMXFParserModule::InitAudioParser(const char *p_cFileName, 
	unsigned long &m_uFrameSum)
{
	assert( (NULL != p_cFileName) 
		&& "Audio's fileName is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pAudioMxfParser)
	{
		m_pAudioMxfParser = new CAudioMXFParser;
	}
	else
	{
		delete m_pAudioMxfParser;
		m_pAudioMxfParser = new CAudioMXFParser;
	}

	if (NULL == m_pAudioMxfParser)
	{
		result = MXFPARSER_OutOfMemory;
		SetErrorString(MXFPARSER_OutOfMemory_ERRSTR);
		return result;
	}

	// Init Audio Mxf parser.
	result = m_pAudioMxfParser->InitMxfParser(p_cFileName);

	if (MXFPARSER_Success == result)
	{
		// Get Audio Mxf file informations.
		m_pAudioMxfInfo = m_pAudioMxfParser->GetMxfInfo();
		m_uFrameSum = m_pAudioMxfInfo->m_uFrameSum;
	}
	else
	{
		SetErrorString("无法初始化音频MXF文件。");
	}

	return result;
}

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
int CMXFParserModule::GetAudioInfo(unsigned long &m_uSamplingRate, 
	unsigned long &m_uChannelCount, 
	unsigned long &m_uBitsPerSample,
	bool &m_bHmacFlag,
	bool &m_bCryptoFlag, 
	char *m_cKeyID)
{
	assert( (NULL != m_cKeyID ) 
		&& "Audio's keyID is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pAudioMxfInfo)
	{   
		result = MXFPARSER_NoInitAudio;
		SetErrorString(MXFPARSER_NoInitAudio_ERRSTR);
	}
	else
	{
		// Get Audio Mxf file informations.
		m_uSamplingRate		= m_pAudioMxfInfo->m_uSamplingRate;
		m_uChannelCount		= m_pAudioMxfInfo->m_uChannelCount;
		m_uBitsPerSample	= m_pAudioMxfInfo->m_uBitsPerSample;
		m_bHmacFlag			= m_pAudioMxfInfo->m_bHmacFlag;
		m_bCryptoFlag		= m_pAudioMxfInfo->m_bCryptoFlag;
		memcpy(m_cKeyID, m_pAudioMxfInfo->m_cKeyID, UUID_LENGTH);

		result = MXFPARSER_Success;
	}

	return result;
}

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
int CMXFParserModule::GetAudioFrameData(const unsigned long m_uFrameNumber, 
	char *m_cFrameData, 
	unsigned long &m_uFrameLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength)
{
	assert( (NULL != m_cFrameData ) 
		&& "Audio's frameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pAudioMxfInfo)
	{    
		result = MXFPARSER_NoInitAudio;
		SetErrorString(MXFPARSER_NoInitAudio_ERRSTR);
	}
	else
	{
		if (m_uFrameNumber >= m_pAudioMxfInfo->m_uFrameSum)
		{
			result = MXFPARSER_AudioFrameNumInvalid;
			SetErrorString(MXFPARSER_AudioFrameNumInvalid_ERRSTR);
		}
		else
		{
			result = m_pAudioMxfParser->GetFrameData(m_uFrameNumber, 
				m_cFrameData, 
				m_uFrameLength,
				m_uPlaintextOffset,
				m_uSourceLength);

			if (MXFPARSER_Success != result)
			{
				SetErrorString("无法从音频MXF文件中获得数据。");
			}
		}
	}

	return result;
}

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
int CMXFParserModule::InitVideoParser(const char *p_cFileName, 
	unsigned long &m_uFrameSum)
{
	assert( (NULL != p_cFileName) 
		&& "Video's fileName is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideoMxfParser)
	{
		m_pVideoMxfParser = new CVideoMXFParser;
	}
	else
	{
		delete m_pVideoMxfParser;
		m_pVideoMxfParser = new CVideoMXFParser;
	}

	if (NULL == m_pVideoMxfParser)
	{
		result = MXFPARSER_OutOfMemory;
		SetErrorString(MXFPARSER_OutOfMemory_ERRSTR);
		return result;
	}

	// Init 2D Video Mxf parser.
	result = m_pVideoMxfParser->InitMxfParser(p_cFileName);

	if (MXFPARSER_Success == result)
	{
		// Get Video Mxf file informations.
		m_pVideoMxfInfo = m_pVideoMxfParser->GetMxfInfo();
		m_uFrameSum = m_pVideoMxfInfo->m_uFrameSum;
	}
	else
	{
		SetErrorString("无法初始化2D视频MXF文件。");
	}

	return result;
}

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
int CMXFParserModule::GetVideoInfo(double &m_dAspectRatio, 
	unsigned long &m_uWidthSize,
	unsigned long &m_uHeightSize,
	unsigned long &m_uFrameRate, 
	bool &m_bHmacFlag,
	bool &m_bCryptoFlag, 
	char *m_cKeyID)
{
	assert( (NULL != m_cKeyID) 
		&& "Video's keyID is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideoMxfInfo)
	{    
		result = MXFPARSER_NoInitVideo;
		SetErrorString(MXFPARSER_NoInitVideo_ERRSTR);
	}
	else
	{
		// Get Video Mxf file formations.
		m_dAspectRatio	= m_pVideoMxfInfo->m_dAspectRatio;
		m_uWidthSize	= m_pVideoMxfInfo->m_uWidthSize;
		m_uHeightSize	= m_pVideoMxfInfo->m_uHeightSize;
		m_uFrameRate	= m_pVideoMxfInfo->m_uFrameRate;
		m_bHmacFlag		= m_pVideoMxfInfo->m_bHmacFlag;
		m_bCryptoFlag	= m_pVideoMxfInfo->m_bCryptoFlag;
		memcpy(m_cKeyID, m_pVideoMxfInfo->m_cKeyID, UUID_LENGTH);

		result = MXFPARSER_Success;
	}

	return result;
}

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
int CMXFParserModule::GetVideoFrameData(const unsigned long m_uFrameNumber, 
	char *m_cFrameData, 
	unsigned long &m_uFrameLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength)
{
	assert( (NULL != m_cFrameData) 
		&& "Video's frameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideoMxfInfo)
	{
		result = MXFPARSER_NoInitVideo;
		SetErrorString(MXFPARSER_NoInitVideo_ERRSTR);
	}
	else
	{
		if (m_uFrameNumber >= m_pVideoMxfInfo->m_uFrameSum)
		{
			result = MXFPARSER_VideoFrameNumInvalid;
			SetErrorString(MXFPARSER_VideoFrameNumInvalid_ERRSTR);
		}
		else
		{
			result = m_pVideoMxfParser->GetFrameData(m_uFrameNumber, 
				m_cFrameData, 
				m_uFrameLength,
				m_uPlaintextOffset,
				m_uSourceLength);

			if (MXFPARSER_Success != result)
			{
				SetErrorString("无法从2D视频MXF文件中获得数据。");
			}
		}
	}

	return result;
}

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
int CMXFParserModule::Init3DVideoParser(const char *p_cFileName, 
	unsigned long &m_uFrameSum)
{
	assert( (NULL != p_cFileName) 
		&& "3D Video's fileName is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideo3DMxfParser)
	{
		m_pVideo3DMxfParser = new C3DVideoMXFParser;
	}
	else
	{
		delete m_pVideo3DMxfParser;
		m_pVideo3DMxfParser = new C3DVideoMXFParser;
	}

	if (NULL == m_pVideo3DMxfParser)
	{
		result = MXFPARSER_OutOfMemory;
		SetErrorString(MXFPARSER_OutOfMemory_ERRSTR);
		return result;
	}

	// Init 3D Video Mxf parser.
	result = m_pVideo3DMxfParser->InitMxfParser(p_cFileName);

	if (MXFPARSER_Success == result)
	{
		// Get 3D Video Mxf informations.
		m_pVideo3DMxfInfo = m_pVideo3DMxfParser->GetMxfInfo();
		m_uFrameSum = m_pVideo3DMxfInfo->m_uFrameSum;
	}
	else
	{
		SetErrorString("无法初始化3D视频MXF文件。");
	}

	return result;
}

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
int CMXFParserModule::Get3DVideoInfo(double &m_dAspectRatio, 
	unsigned long &m_uWidthSize,
	unsigned long &m_uHeightSize,
	unsigned long &m_uFrameRate,
	bool &m_bHmacFlag,
	bool &m_bCryptoFlag, 
	char *m_cKeyID)
{
	assert( (NULL != m_cKeyID) 
		&& "3D Video's keyID is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideo3DMxfInfo)
	{    
		result = MXFPARSER_NoInitVideo;
		SetErrorString(MXFPARSER_NoInitVideo_ERRSTR);
	}
	else
	{
		// Get 3D Video Mxf file informations.
		m_dAspectRatio	= m_pVideo3DMxfInfo->m_dAspectRatio;
		m_uWidthSize	= m_pVideo3DMxfInfo->m_uWidthSize;
		m_uHeightSize	= m_pVideo3DMxfInfo->m_uHeightSize;
		m_uFrameRate	= m_pVideo3DMxfInfo->m_uFrameRate;
		m_bHmacFlag		= m_pVideo3DMxfInfo->m_bHmacFlag;
		m_bCryptoFlag	= m_pVideo3DMxfInfo->m_bCryptoFlag;
		memcpy(m_cKeyID, m_pVideo3DMxfInfo->m_cKeyID, UUID_LENGTH);

		result = MXFPARSER_Success;
	}

	return result;
}

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
int CMXFParserModule::Get3DFrameData(const unsigned long m_uFrameNumber, 
	char *m_cLeftFrameData, 
	unsigned long &m_uLeftFrameLength,
	unsigned int &m_uLeftPlaintextOffset,
	unsigned int &m_uLeftSourceLength,
	char *m_cRightFrameData, 
	unsigned long &m_uRightFrameLength,
	unsigned int &m_uRightPlaintextOffset,
	unsigned int &m_uRightSourceLength)
{
	assert( (NULL != m_cLeftFrameData) 
		&& "3D Video's LeftFrameData is invalid, must be nonzero!" );

	assert( (NULL != m_cRightFrameData) 
		&& "3D Video's RightFrameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;

	if (NULL == m_pVideo3DMxfInfo)
	{
		result = MXFPARSER_NoInitVideo;
		SetErrorString(MXFPARSER_NoInitVideo_ERRSTR);
	}
	else
	{
		if (m_uFrameNumber >= m_pVideo3DMxfInfo->m_uFrameSum)
		{
			result = MXFPARSER_VideoFrameNumInvalid;
			SetErrorString(MXFPARSER_VideoFrameNumInvalid_ERRSTR);
		}
		else
		{
			result = m_pVideo3DMxfParser->GetFrameData(m_uFrameNumber, 
				m_cLeftFrameData, 
				m_uLeftFrameLength,
				m_uLeftPlaintextOffset,
				m_uLeftSourceLength,
				m_cRightFrameData, 
				m_uRightFrameLength,
				m_uRightPlaintextOffset,
				m_uRightSourceLength);

			if (MXFPARSER_Success != result)
			{
				SetErrorString("无法从3D视频MXF文件中获得数据。");
			}
		}
	}

	return result;
}


void CMXFParserModule::SetErrorString(const char *errBuffer)
{
	errString = errBuffer;
}

const char *CMXFParserModule::GetErrorString()
{
	return errString;
}

