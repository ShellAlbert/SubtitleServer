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
#ifndef _CMXFParser_H_
#define _CMXFParser_H_

#include <MXF.h>
using namespace ASDCP;

// Video frame buffer's size.
const unsigned int VIDEO_FRAME_BUFFER_SIZE = 1302083 * 2;

// Audio frame buffer's size.
const unsigned int AUDIO_FRAME_BUFFER_SIZE = 192000 * 2;

// The length of UUID.
const unsigned int UUID_LENGTH = 16;

// This struct include MXF file's informations.
struct MxfInfo
{
	const char *m_cFileName;           // Mxf file name.

	unsigned long m_uFrameSum;         // The sum frame number of mxf file.
	unsigned long m_uFrameRate;        // The frame rate of mxf file.
	bool m_bCryptoFlag;                // The CryptoFlag of mxf file.
	bool m_bHmacFlag;				   // The HMACFlag of mxf file.
	char m_cKeyID[UUID_LENGTH];        // The Key ID of mxf file.
	char m_cAssetUuid[UUID_LENGTH];    // The Asset UUID of mxf file.

	double m_dAspectRatio;             // The aspect rate of mxf file.
	unsigned long m_uWidthSize;        // The width size of video.
	unsigned long m_uHeightSize;       // The height size of video.

	unsigned long m_uSamplingRate;     // The sampling rate of mxf file.
	unsigned long m_uChannelCount;     // The channel count of mxf file.
	unsigned long m_uBitsPerSample;    // The BitsPerSample of mxf file.

};

// The base class of Mxf Parser.
class CMXFParser
{
public:
	CMXFParser();
	virtual ~CMXFParser();

	virtual int InitMxfParser(const char *p_cFileName) = 0;

	virtual int SetMxfInfo() = 0;

	virtual const MxfInfo* GetMxfInfo() = 0;

	virtual int GetFrameData(const unsigned long m_uFrameNumber, 
        char *p_cFrameData, 
        unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength) = 0;

	//virtual int GetFrameData(const unsigned long m_uFrameNumber, 
	//    char *p_cLeftFrameData, 
	//    unsigned long &m_uLeftFrameLength,
	//    char *p_cRightFrameData,
	//    unsigned long &m_uRightFrameLength) = 0;

};

// The audio subclass of Mxf Parser.
class CAudioMXFParser: public CMXFParser
{
public:
	CAudioMXFParser();
	~CAudioMXFParser();

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
	int InitMxfParser(const char *p_cFileName);


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
	int SetMxfInfo();


	/************************************************* 
	Function:     GetMxfInfo
	Description:  Get audio mxf information.
	Input:        None
	Output:       None
	Return:       a MxfInfo* pointer      This struct include mxf info.
	Others:       None
	*************************************************/ 
	const MxfInfo* GetMxfInfo();


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
	int GetFrameData(const unsigned long m_uFrameNumber, 
		char *p_cFrameData, 
		unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength);

	/// XLQ:获得错误信息
	const char *GetErrorString();

private:
	// Audio Mxf reader which is came from asdcplib.
	PCM::MXFReader     m_AudioReader;
	// Audio Frame Buffer which is came from asdcplib.
	PCM::FrameBuffer   m_AudioFrameBuffer;

	// Audio Mxf Informations.
	MxfInfo m_AudioMxfInfo;

	/// XLQ:设置错误信息
	void SetErrorString(const char *errBuffer);
	/// XLQ:记录错误信息
	const char *errString;
};

// The 2D video subclass of Mxf Parser.
class CVideoMXFParser: public CMXFParser
{
public:
	CVideoMXFParser();
	~CVideoMXFParser();

	/************************************************* 
	Function:     InitMxfParser
	Description:  Init 2D video mxf parser.
	Input:        p_cFileName   Mxf file name.
	Output:       None
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_EssenceIsPCM This is a pcm mxf file.
	MXFPARSER_UnKnownType       Unknown file type.
	MXFPARSER_NoDcpEssence      This isn't a mxf file.
	Others:       None
	*************************************************/ 
	int InitMxfParser(const char *p_cFileName);

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
	int SetMxfInfo();

	/************************************************* 
	Function:     GetMxfInfo
	Description:  Get 2D video mxf information.
	Input:        None
	Output:       None
	Return:       a MxfInfo* pointer      This struct include mxf info.
	Others:       None
	*************************************************/ 
	const MxfInfo* GetMxfInfo();

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
	int GetFrameData(const unsigned long m_uFrameNumber, 
		char *p_cFrameData, 
		unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength);

	/// XLQ:获得错误信息
	const char *GetErrorString();

private:
	// Video Mxf reader which is came from asdcplib.
	JP2K::MXFReader    m_VideoReader;
	// Video Frame Buffer which is came from asdcplib.
	JP2K::FrameBuffer  m_VideoFrameBuffer;

	// Video Mxf Informations.
	MxfInfo m_VideoMxfInfo;

	/// XLQ:设置错误信息
	void SetErrorString(const char *errBuffer);
	/// XLQ:记录错误信息
	const char *errString;
};

// The 3D video subclass of Mxf Parser.
class C3DVideoMXFParser: public CMXFParser
{
public:
	C3DVideoMXFParser();
	~C3DVideoMXFParser();

	/************************************************* 
	Function:     InitMxfParser
	Description:  Init 3D video mxf parser.
	Input:        p_cFileName   Mxf file name.
	Output:       None
	Return:       MXFPARSER_InitValue         Init value.
	MXFPARSER_Success           Success.
	MXFPARSER_EssenceIsPCM      This is a pcm mxf file.
	MXFPARSER_UnKnownType       Unknown file type.
	MXFPARSER_NoDcpEssence      This isn't a mxf file.
	Others:       None
	*************************************************/ 
	int InitMxfParser(const char *p_cFileName);

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
	int SetMxfInfo();

	/************************************************* 
	Function:     GetMxfInfo
	Description:  Get 3D video mxf information.
	Input:        None
	Output:       None
	Return:       a MxfInfo* pointer      This struct include mxf info.
	Others:       None
	*************************************************/ 
	const MxfInfo* GetMxfInfo();

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
	int GetFrameData(const unsigned long m_uFrameNumber, 
		char *p_cFrameData, 
		unsigned long &m_uFrameLength,
		unsigned int &m_uPlaintextOffset,
		unsigned int &m_uSourceLength);

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
	int GetFrameData(const unsigned long m_uFrameNumber, 
		char *p_cLeftFrameData, 
		unsigned long &m_uLeftFrameLength,
		unsigned int &m_uLeftPlaintextOffset,
		unsigned int &m_uLeftSourceLength,
		char *p_cRightFrameData,
		unsigned long &m_uRightFrameLength,
		unsigned int &m_uRightPlaintextOffset,
		unsigned int &m_uRightSourceLength);

	/// XLQ:获得错误信息
	const char *GetErrorString();

private:
	// 3D Video Mxf reader which is came from asdcplib.
	JP2K::MXFSReader m_VideoReader;
	// Video Frame Buffer which is came from asdcplib.
	JP2K::FrameBuffer m_LeftVideoFrameBuffer;
	JP2K::FrameBuffer m_RightVideoFrameBuffer; 

	// 3D Video Mxf Informations.
	MxfInfo m_VideoMxfInfo;

	/// XLQ:设置错误信息
	void SetErrorString(const char *errBuffer);
	/// XLQ:记录错误信息
	const char *errString;
};


#endif
