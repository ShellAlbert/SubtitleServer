/********************************************* 
Copyright (C), 2010-2020, DADI MEDIA Co., Ltd.
ModuleName:  MXF Parser Module
FileName:    CMXFParser.h  CMXFParser.cpp
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
//#include "asdcp-test.h"

#include "CMXFParser.h"
#include "ResultMXFParser.h"


CMXFParser::CMXFParser()
{

}

CMXFParser::~CMXFParser()
{

}

CAudioMXFParser::CAudioMXFParser()
{
	memset(&m_AudioMxfInfo, 0, sizeof(MxfInfo));
}

CAudioMXFParser::~CAudioMXFParser()
{

}

/************************************************* 
Function:     InitMxfParser
Description:  Init audio mxf parser.
Input:        p_cFileName   Mxf file name.
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_EssenceIsJpeg2000 This is a jpeg2000 mxf file.
MXFPARSER_UnKnownType       Unknown file type.
MXFPARSER_NoDcpEssence      This isn't a mxf file.
Others:       None
*************************************************/ 
int CAudioMXFParser::InitMxfParser(const char *p_cFileName)
{
	assert( (NULL != p_cFileName) 
		&& "Audio's fileName is invalid, must be nonzero!" );

	m_AudioMxfInfo.m_cFileName = p_cFileName;

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Input mxf file name and get mxf file type.
	EssenceType_t EssenceType;
	kumuResult = ASDCP::EssenceType(m_AudioMxfInfo.m_cFileName, EssenceType);

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		// Whether this mxf file is a audio mxf file.
		switch ( EssenceType )
		{

		case ESS_JPEG_2000:
			result = MXFPARSER_EssenceIsJpeg2000;
			break;

		case ESS_PCM_24b_48k:
			// If the mxf file is audio mxf file then set mxf info.
			result = SetMxfInfo();
			break;

		default:
			result = MXFPARSER_UnKnownType;
		}
	}
	else
	{
		result = MXFPARSER_NoDcpEssence;
	}

	return result;
}

/************************************************* 
Function:     SetMxfInfo
Description:  Set audio mxf information.
Input:        None
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitAudioMxf    Run InitMxfParser() first.
MXFPARSER_CanNotFillAudioDesc    Can't fill audio desc.
MXFPARSER_CanNotFillAudioInfo    Can't fill audio info.
MXFPARSER_CanNotOpenAudio        Can't open audio file.
Others:       None
*************************************************/ 
int CAudioMXFParser::SetMxfInfo()
{    
	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_AudioMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitAudioMxf;
	}
	else
	{
		kumuResult = m_AudioReader.OpenRead(m_AudioMxfInfo.m_cFileName);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		// Get Mxf file's informations.
		PCM::AudioDescriptor audioDesc;
		kumuResult = m_AudioReader.FillAudioDescriptor(audioDesc);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_AudioMxfInfo.m_uFrameSum = audioDesc.ContainerDuration;
			m_AudioMxfInfo.m_uFrameRate = (audioDesc.EditRate.Numerator) 
				/ (audioDesc.EditRate.Denominator);

			m_AudioMxfInfo.m_uSamplingRate = (audioDesc.AudioSamplingRate.Numerator)
				/ (audioDesc.AudioSamplingRate.Denominator);
			m_AudioMxfInfo.m_uChannelCount = audioDesc.ChannelCount;
			m_AudioMxfInfo.m_uBitsPerSample = audioDesc.QuantizationBits;

			// Set Audio Frame Buffer's size.
			m_AudioFrameBuffer.Capacity(AUDIO_FRAME_BUFFER_SIZE);
		}
		else
		{
			return MXFPARSER_CanNotFillAudioDesc;
		}

		WriterInfo writerInfo;
		kumuResult = m_AudioReader.FillWriterInfo(writerInfo);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_AudioMxfInfo.m_bCryptoFlag = writerInfo.EncryptedEssence;
			m_AudioMxfInfo.m_bHmacFlag	 = writerInfo.UsesHMAC;
			memcpy(m_AudioMxfInfo.m_cKeyID, 
				writerInfo.CryptographicKeyID, 
				UUID_LENGTH);
			memcpy(m_AudioMxfInfo.m_cAssetUuid, 
				writerInfo.AssetUUID, 
				UUID_LENGTH);
		}
		else
		{
			return MXFPARSER_CanNotFillAudioInfo;
		}

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotOpenAudio;
	}

	return result;
}

/************************************************* 
Function:     GetMxfInfo
Description:  Get audio mxf information.
Input:        None
Output:       None
Return:       a MxfInfo* pointer      This struct include mxf info.
Others:       None
*************************************************/ 
const MxfInfo* CAudioMXFParser::GetMxfInfo()
{
	return &m_AudioMxfInfo;
}

/************************************************* 
Function:     GetFrameData
Description:  Get audio mxf frame data.
Input:        m_uFrameNumber   Input a frame number.
Output:       p_cFrameData     Get frame data.
m_uFrameLength   Get frame length.
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitAudioMxf    Run InitMxfParser() first.
MXFPARSER_CanNotReadAudioData    Can't read audio data.
Others:       None
*************************************************/ 
int CAudioMXFParser::GetFrameData(const unsigned long m_uFrameNumber, 
	char *p_cFrameData, 
	unsigned long &m_uFrameLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength)
{
	assert( (NULL != p_cFrameData ) 
		&& "Audio's frameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_AudioMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitAudioMxf;
	}
	else
	{
		// Get frame data buffer.
		kumuResult = m_AudioReader.ReadFrame(m_uFrameNumber, 
			m_AudioFrameBuffer);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		m_uFrameLength = m_AudioFrameBuffer.Size();
		memcpy(p_cFrameData, m_AudioFrameBuffer.Data(), m_uFrameLength);
		m_uPlaintextOffset  = m_AudioFrameBuffer.PlaintextOffset();
		m_uSourceLength		= m_AudioFrameBuffer.SourceLength();

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotReadAudioData;
	}

	return result;
}

void CAudioMXFParser::SetErrorString(const char *errBuffer)
{
	errString = errBuffer;
}

const char *CAudioMXFParser::GetErrorString()
{
	return errString;
}


CVideoMXFParser::CVideoMXFParser():m_VideoFrameBuffer(VIDEO_FRAME_BUFFER_SIZE)
{
	memset(&m_VideoMxfInfo, 0, sizeof(MxfInfo));
}

CVideoMXFParser::~CVideoMXFParser()
{

}

/************************************************* 
Function:     InitMxfParser
Description:  Init 2D video mxf parser.
Input:        p_cFileName   Mxf file name.
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_EssenceIsPCM      This is a pcm mxf file.
MXFPARSER_UnKnownType       Unknown file type.
MXFPARSER_NoDcpEssence      This isn't a mxf file.
Others:       None
*************************************************/ 
int CVideoMXFParser::InitMxfParser(const char *p_cFileName)
{
	assert( (NULL != p_cFileName) 
		&& "Video's fileName is invalid, must be nonzero!" );

	m_VideoMxfInfo.m_cFileName = p_cFileName;

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Input mxf file name and get mxf file type.
	EssenceType_t EssenceType;
	kumuResult = ASDCP::EssenceType(m_VideoMxfInfo.m_cFileName, EssenceType);

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		switch ( EssenceType )
		{
			//case ESS_MPEG2_VES:
			//    result = read_MPEG2_file(Options);
			//    break;

		case ESS_JPEG_2000:
			//if ( Options.stereo_image_flag )
			//    result = read_JP2K_S_file(Options);
			//else
			//result = read_JP2K_file(Options);

			// If the mxf file is jpeg2000 mxf file then set mxf info.
			result = SetMxfInfo();
			break;

			//case ESS_JPEG_2000_S:
			//    result = read_JP2K_S_file(Options);
			//    break;

		case ESS_PCM_24b_48k:
		case ESS_PCM_24b_96k:
			result = MXFPARSER_EssenceIsPcm;
			break;

		default:
			result = MXFPARSER_UnKnownType;
		}
	}
	else
	{
		result = MXFPARSER_NoDcpEssence;
	}

	return result;
}

/************************************************* 
Function:     SetMxfInfo
Description:  Set 2D video mxf information.
Input:        None
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitAudioMxf    Run InitMxfParser() first.
MXFPARSER_CanNotFillVideoDesc    Can't fill video desc.
MXFPARSER_CanNotFillVideoInfo    Can't fill video info.
MXFPARSER_CanNotOpenVideo        Can't open video file.
Others:       None
*************************************************/ 
int CVideoMXFParser::SetMxfInfo()
{
	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_VideoMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitVideoMxf;
	}
	else
	{
		kumuResult = m_VideoReader.OpenRead(m_VideoMxfInfo.m_cFileName);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		// Get Mxf file's informations.
		JP2K::PictureDescriptor videoDesc;
		kumuResult = m_VideoReader.FillPictureDescriptor(videoDesc);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_VideoMxfInfo.m_uFrameSum = videoDesc.ContainerDuration;
			m_VideoMxfInfo.m_uFrameRate = (videoDesc.EditRate.Numerator) 
				/ (videoDesc.EditRate.Denominator);
			m_VideoMxfInfo.m_dAspectRatio = videoDesc.AspectRatio.Quotient();
			m_VideoMxfInfo.m_uWidthSize = videoDesc.StoredWidth;
			m_VideoMxfInfo.m_uHeightSize = videoDesc.StoredHeight;
		}
		else
		{
			return MXFPARSER_CanNotFillVideoDesc;
		}

		WriterInfo writerInfo;
		kumuResult = m_VideoReader.FillWriterInfo(writerInfo);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_VideoMxfInfo.m_bCryptoFlag = writerInfo.EncryptedEssence;
			m_VideoMxfInfo.m_bHmacFlag   = writerInfo.UsesHMAC;
			memcpy(m_VideoMxfInfo.m_cKeyID, 
				writerInfo.CryptographicKeyID, 
				UUID_LENGTH);
			memcpy(m_VideoMxfInfo.m_cAssetUuid, 
				writerInfo.AssetUUID, 
				UUID_LENGTH);
		}
		else
		{
			return MXFPARSER_CanNotFillVideoInfo;
		}

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotOpenVideo;
	}

	return result;

}

/************************************************* 
Function:     GetMxfInfo
Description:  Get 2D video mxf information.
Input:        None
Output:       None
Return:       a MxfInfo* pointer      This struct include mxf info.
Others:       None
*************************************************/ 
const MxfInfo* CVideoMXFParser::GetMxfInfo()
{
	return &m_VideoMxfInfo;
}

/************************************************* 
Function:     GetFrameData
Description:  Get 2D video mxf frame data.
Input:        m_uFrameNumber   Input a frame number.
Output:       p_cFrameData     Get frame data.
m_uFrameLength   Get frame length.
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitVideoMxf    Run InitMxfParser() first.
MXFPARSER_CanNotReadVideoData    Can't read video data.
Others:       None
*************************************************/ 
int CVideoMXFParser::GetFrameData(const unsigned long m_uFrameNumber, 
	char *p_cFrameData, 
	unsigned long &m_uFrameLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength)
{   
	assert( (NULL != p_cFrameData) 
		&& "Video's frameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_VideoMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitVideoMxf;
	}
	else
	{
		// Get frame data buffer.
		kumuResult = m_VideoReader.ReadFrame(m_uFrameNumber, 
			m_VideoFrameBuffer);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		m_uFrameLength = m_VideoFrameBuffer.Size();
		memcpy(p_cFrameData, m_VideoFrameBuffer.Data(), m_uFrameLength);
		m_uPlaintextOffset  = m_VideoFrameBuffer.PlaintextOffset();
		m_uSourceLength		= m_VideoFrameBuffer.SourceLength();

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotReadVideoData;
	}

	return result;
}

void CVideoMXFParser::SetErrorString(const char *errBuffer)
{
	errString = errBuffer;
}

const char *CVideoMXFParser::GetErrorString()
{
	return errString;
}



C3DVideoMXFParser::C3DVideoMXFParser():
m_LeftVideoFrameBuffer(VIDEO_FRAME_BUFFER_SIZE),
	m_RightVideoFrameBuffer(VIDEO_FRAME_BUFFER_SIZE)
{
	memset(&m_VideoMxfInfo, 0, sizeof(MxfInfo));
}

C3DVideoMXFParser::~C3DVideoMXFParser()
{

}

/************************************************* 
Function:     InitMxfParser
Description:  Init 3D video mxf parser.
Input:        p_cFileName   Mxf file name.
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_EssenceIsPCM This is a pcm mxf file.
MXFPARSER_UnKnownType       Unknown file type.
MXFPARSER_NoDcpEssence      This isn't a mxf file.
Others:       None
*************************************************/ 
int C3DVideoMXFParser::InitMxfParser(const char *p_cFileName)
{
	assert( (NULL != p_cFileName) 
		&& "3D Video's fileName is invalid, must be nonzero!" );

	m_VideoMxfInfo.m_cFileName = p_cFileName;

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Input mxf file name and get mxf file type.
	EssenceType_t EssenceType;
	kumuResult = ASDCP::EssenceType(m_VideoMxfInfo.m_cFileName, EssenceType);

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		switch ( EssenceType )
		{
		case ESS_JPEG_2000:
			//if ( Options.stereo_image_flag )
			//    result = read_JP2K_S_file(Options);
			//else
			//result = read_JP2K_file(Options);

			// If the mxf file is jpeg2000 mxf file then set mxf info.
			result = SetMxfInfo();
			break;

			//case ESS_JPEG_2000_S:
			//    result = read_JP2K_S_file(Options);
			//    break;

		case ESS_PCM_24b_48k:
		case ESS_PCM_24b_96k:
			result = MXFPARSER_EssenceIsPcm;
			break;

		default:
			result = MXFPARSER_UnKnownType;
		}
	}
	else
	{
		result = MXFPARSER_NoDcpEssence;
	}

	return result;
}

/************************************************* 
Function:     SetMxfInfo
Description:  Set 3D video mxf information.
Input:        None
Output:       None
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitAudioMxf    Run InitMxfParser() first.
MXFPARSER_CanNotFillVideoDesc    Can't fill video desc.
MXFPARSER_CanNotFillVideoInfo    Can't fill video info.
MXFPARSER_CanNotOpenVideo        Can't open video file.
Others:       None
*************************************************/ 
int C3DVideoMXFParser::SetMxfInfo()
{
	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_VideoMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitVideoMxf;
	}
	else
	{
		kumuResult = m_VideoReader.OpenRead(m_VideoMxfInfo.m_cFileName);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		// Get Mxf file's informations.
		JP2K::PictureDescriptor videoDesc;
		kumuResult = m_VideoReader.FillPictureDescriptor(videoDesc);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_VideoMxfInfo.m_uFrameSum = videoDesc.ContainerDuration;
			m_VideoMxfInfo.m_uFrameRate = (videoDesc.EditRate.Numerator) 
				/ (videoDesc.EditRate.Denominator);
			m_VideoMxfInfo.m_dAspectRatio = videoDesc.AspectRatio.Quotient();
			m_VideoMxfInfo.m_uWidthSize = videoDesc.StoredWidth;
			m_VideoMxfInfo.m_uHeightSize = videoDesc.StoredHeight;
		}
		else
		{
			return MXFPARSER_CanNotFillVideoDesc;
		}

		WriterInfo writerInfo;
		kumuResult = m_VideoReader.FillWriterInfo(writerInfo);
		if ( ASDCP_SUCCESS(kumuResult) )
		{
			m_VideoMxfInfo.m_bCryptoFlag = writerInfo.EncryptedEssence;
			m_VideoMxfInfo.m_bHmacFlag	 = writerInfo.UsesHMAC;
			memcpy(m_VideoMxfInfo.m_cKeyID, 
				writerInfo.CryptographicKeyID, 
				UUID_LENGTH);
			memcpy(m_VideoMxfInfo.m_cAssetUuid, 
				writerInfo.AssetUUID, 
				UUID_LENGTH);
		}
		else
		{
			return MXFPARSER_CanNotFillVideoInfo;
		}

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotOpenVideo;
	}

	return result;

}

/************************************************* 
Function:     GetMxfInfo
Description:  Get 3D video mxf information.
Input:        None
Output:       None
Return:       a MxfInfo* pointer      This struct include mxf info.
Others:       None
*************************************************/ 
const MxfInfo* C3DVideoMXFParser::GetMxfInfo()
{
	return &m_VideoMxfInfo;
}

/************************************************* 
Function:     GetFrameData
Description:  Get 3D video mxf frame data.
Input:        m_uFrameNumber   Input a frame number.
Output:       p_cFrameData     Get frame data.
m_uFrameLength   Get frame length.
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitVideoMxf    Run InitMxfParser() first.
MXFPARSER_CanNotReadVideoData    Can't read video data.
Others:       None
*************************************************/ 
int C3DVideoMXFParser::GetFrameData(const unsigned long m_uFrameNumber, 
	char *p_cFrameData, 
	unsigned long &m_uFrameLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength)
{   
	assert( (NULL != p_cFrameData) 
		&& "3D Video's frameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_VideoMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitVideoMxf;
	}
	else
	{
		// Get frame data buffer, note: we onle get left eye frame data,
		// because our server is 2D. 
		kumuResult = m_VideoReader.ReadFrame(m_uFrameNumber, JP2K::SP_LEFT,
			m_LeftVideoFrameBuffer);
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		m_uFrameLength = m_LeftVideoFrameBuffer.Size();
		memcpy(p_cFrameData, m_LeftVideoFrameBuffer.Data(), m_uFrameLength);
		m_uPlaintextOffset  = m_LeftVideoFrameBuffer.PlaintextOffset();
		m_uSourceLength		= m_LeftVideoFrameBuffer.SourceLength();

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotReadVideoData;
	}

	return result;
}

/************************************************* 
Function:     GetFrameData
Description:  Get 3D video mxf frame data.
Input:        m_uFrameNumber   Input a frame number.
Output:       p_cFrameData     Get frame data.
m_uFrameLength   Get frame length.
Return:       MXFPARSER_InitValue         Init value.
MXFPARSER_Success           Success.
MXFPARSER_NoInitVideoMxf    Run InitMxfParser() first.
MXFPARSER_CanNotReadVideoData    Can't read video data.
Others:       None
*************************************************/ 
int C3DVideoMXFParser::GetFrameData(const unsigned long m_uFrameNumber, 
	char *p_cLeftFrameData, 
	unsigned long &m_uLeftFrameLength,
	unsigned int &m_uLeftPlaintextOffset,
	unsigned int &m_uLeftSourceLength,
	char *p_cRightFrameData,
	unsigned long &m_uRightFrameLength,
	unsigned int &m_uRightPlaintextOffset,
	unsigned int &m_uRightSourceLength)
{   
	assert( (NULL != p_cLeftFrameData) 
		&& "3D Video's LeftFrameData is invalid, must be nonzero!" );

	assert( (NULL != p_cRightFrameData) 
		&& "3D Video's RightFrameData is invalid, must be nonzero!" );

	int result = MXFPARSER_InitValue;
	Result_t kumuResult = RESULT_OK;

	// Whether init mxf parser already.
	if (NULL == m_VideoMxfInfo.m_cFileName)
	{
		return MXFPARSER_NoInitVideoMxf;
	}
	else
	{
		// Get frame data buffer, note: we onle get left eye frame data,
		// because our server is 2D. 
		kumuResult = m_VideoReader.ReadFrame(m_uFrameNumber, JP2K::SP_LEFT,
			m_LeftVideoFrameBuffer);

		if ( ASDCP_SUCCESS(kumuResult) )
		{
			kumuResult = m_VideoReader.ReadFrame(m_uFrameNumber, JP2K::SP_RIGHT,
				m_RightVideoFrameBuffer);
		}
		else
		{
			result = MXFPARSER_CanNotReadVideoData;
		}
	}

	if ( ASDCP_SUCCESS(kumuResult) )
	{
		m_uLeftFrameLength = m_LeftVideoFrameBuffer.Size();
		memcpy(p_cLeftFrameData, m_LeftVideoFrameBuffer.Data(), m_uLeftFrameLength);
		m_uLeftPlaintextOffset  = m_LeftVideoFrameBuffer.PlaintextOffset();
		m_uLeftSourceLength		= m_LeftVideoFrameBuffer.SourceLength();

		m_uRightFrameLength = m_RightVideoFrameBuffer.Size();
		memcpy(p_cRightFrameData, m_RightVideoFrameBuffer.Data(), m_uRightFrameLength);
		m_uRightPlaintextOffset  = m_RightVideoFrameBuffer.PlaintextOffset();
		m_uRightSourceLength		= m_RightVideoFrameBuffer.SourceLength();

		result = MXFPARSER_Success;
	}
	else
	{
		result = MXFPARSER_CanNotReadVideoData;
	}

	return result;
}

void C3DVideoMXFParser::SetErrorString(const char *errBuffer)
{
	errString = errBuffer;
}

const char *C3DVideoMXFParser::GetErrorString()
{
	return errString;
}
