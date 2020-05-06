/********************************************* 
Copyright (C), 2010-2020, DADI MEDIA Co., Ltd.
ModuleName:  MXF Parser Module
FileName:    ResultMXFParser.h
Author:      xiaoliqun@dadimedia.com
Date:        2010/05/31
Version:     v1.0
Description: This module is used to parser MXF files.
Others:      None
History:
< Author >    			         < Date >             < Modification >
xiaoliqun@dadimedia.com          2010/05/31           Create this file. 

*********************************************/
#ifndef _RESULT_MXFPARSER_H_
#define _RESULT_MXFPARSER_H_

const int MXFPARSER_Success = 0;                // Success.
const int MXFPARSER_InitValue = -1;             // Init value.

const int MXFPARSER_NoInitVideo = 0xE1011001;            // Run InitMxfParser() first.
//const char *MXFPARSER_NoInitVideo_ERRSTR = "��Ҫ�ȳ�ʼ����ƵMXF�ļ���";

const int MXFPARSER_NoInitAudio = 0xE1011002;            // Run InitMxfParser() first.
//const char *MXFPARSER_NoInitAudio_ERRSTR = "��Ҫ�ȳ�ʼ����ƵMXF�ļ���";

const int MXFPARSER_OutOfMemory = 0xE1011003;            // Can't allocate memory.
//const char *MXFPARSER_OutOfMemory_ERRSTR = "MXF�ļ�����ģ�����޷������ڴ档";

const int MXFPARSER_UnKnownType = 0xE1011004;            // Unknown file type.
//const char *MXFPARSER_UnKnownType_ERRSTR = "�޷�ʶ���MXF�ļ����͡�";

const int MXFPARSER_NoDcpEssence = 0xE1011005;           // This isn't a mxf file.
//const char *MXFPARSER_NoDcpEssence_ERRSTR = "���ļ�����MXF�ļ���";

const int MXFPARSER_EssenceIsPcm = 0xE1011006;           // This is a pcm mxf file.
//const char *MXFPARSER_EssenceIsPcm_ERRSTR = "���ļ�������ƵMXF�ļ���";

const int MXFPARSER_NoInitVideoMxf = 0xE1011007;         // No init Video Mxf parser.
//const char *MXFPARSER_NoInitVideoMxf_ERRSTR = "���ļ�������ƵMXF�ļ�";

const int MXFPARSER_CanNotOpenVideo = 0xE1011008;        // Can't open video file.
//const char *MXFPARSER_CanNotOpenVideo_ERRSTR = "�޷�����Ƶ�ļ���";

const int MXFPARSER_CanNotFillVideoDesc = 0xE1011009;    // Can't fill video desc.
//const char *MXFPARSER_CanNotFillVideoDesc_ERRSTR = "�޷������Ƶ������";

const int MXFPARSER_CanNotFillVideoInfo = 0xE1011010;   // Can't fill video info.
//const char *MXFPARSER_CanNotFillVideoInfo_ERRSTR = "�޷������Ƶ��Ϣ��";

const int MXFPARSER_CanNotReadVideoData = 0xE1011011;   // Can't read video data.
//const char *MXFPARSER_CanNotReadVideoData_ERRSTR = "�޷���ȡ��Ƶ���ݡ�";

const int MXFPARSER_EssenceIsJpeg2000 = 0xE1011012;     // This is a jpeg2000 mxf file.
//const char *MXFPARSER_EssenceIsJpeg2000_ERRSTR = "����һ��Jpeg2000��MXF�ļ���";

const int MXFPARSER_NoInitAudioMxf = 0xE1011013;        // No init Audio Mxf parser.
//const char *MXFPARSER_NoInitAudioMxf_ERRSTR = "û�г�ʼ����ƵMXF�ļ���";

const int MXFPARSER_CanNotFillAudioDesc = 0xE1011014;   // Can't fill audio desc.
//const char *MXFPARSER_CanNotFillAudioDesc_ERRSTR = "�޷������Ƶ������";

const int MXFPARSER_CanNotFillAudioInfo = 0xE1011015;   // Can't fill audio info.
//const char *MXFPARSER_CanNotFillAudioInfo_ERRSTR = "�޷������Ƶ��Ϣ��";

const int MXFPARSER_CanNotOpenAudio = 0xE1011016;       // Can't open audio file.
//const char *MXFPARSER_CanNotOpenAudio_ERRSTR = "�޷�����Ƶ�ļ���";

const int MXFPARSER_CanNotReadAudioData = 0xE1011017;   // Can't read audio data.
//const char *MXFPARSER_CanNotReadAudioData_ERRSTR = "�޷���ȡ��Ƶ���ݡ�";

const int MXFPARSER_VideoFrameNumInvalid = 0xE1011018;  // The video frame number is invalid.
//const char *MXFPARSER_VideoFrameNumInvalid_ERRSTR = "��Ƶ֡����Ч��";

const int MXFPARSER_AudioFrameNumInvalid = 0xE1011019;  // The audio frame number is invalid.
//const char *MXFPARSER_AudioFrameNumInvalid_ERRSTR = "��Ƶ֡����Ч��";

#endif
