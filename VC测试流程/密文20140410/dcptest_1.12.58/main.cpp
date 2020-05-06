/********************************************* 
Copyright (C), 2010-2020, DADI MEDIA Co., Ltd.
ModuleName:  MXF Parser Module
FileName:    main.cpp
Author:      xiaoliqun@dadimedia.com
Date:        2010/05/31
Version:     v1.0
Description: This module is used to parser MXF files.
Others:      None
History:
< Author >    			         < Date >             < Modification >
xiaoliqun@dadimedia.com          2010/05/31           Create this file. 

*********************************************/
#include <iostream>
#include <stdio.h>
using namespace std;

#include "CMXFParserModule.h"

int main(int argc, char **argv)
{
    char *videoDataBuffer = new char[2 * 1024 * 1024];
    unsigned long videoDataLength = 0;
    char *audioDataBuffer = new char[36000 * 10];
    unsigned long audioDataLength = 0;

    CMXFParserModule cmxfParserModule;
    unsigned long vidoeFrameSum = 0;
    unsigned long audioFrameSum = 0;
    //cmxfParserModule.InitVideoParser("/home/xlq/dcps/dieying3-xyz/dieying3_xyz_video.mxf", vidoeFrameSum);
    //cmxfParserModule.InitAudioParser("/home/xlq/dcps/dieying3-xyz/dieying3_xyz_audio.mxf", audioFrameSum);
    cmxfParserModule.InitVideoParser("/home/xlq/dcps/dieying3_xyz_ct/dieying3_xyz_video_ct.mxf", vidoeFrameSum);
    cmxfParserModule.InitAudioParser("/home/xlq/dcps/dieying3_xyz_ct/dieying3_xyz_audio_ct.mxf", audioFrameSum);

    double m_dAspectRatio = 0;
    unsigned long m_uWidthSize = 0;
    unsigned long m_uHeightSize = 0;
    unsigned long m_uFrameRate = 0;
    bool m_bHmacFlag1 = false;
    bool m_bCryptoFlag1 = false; 
    char m_cKeyID1[16] = "";
    cmxfParserModule.GetVideoInfo(m_dAspectRatio,
        m_uWidthSize,
        m_uHeightSize,
        m_uFrameRate,
        m_bHmacFlag1,
        m_bCryptoFlag1, 
        m_cKeyID1);
    cout<<"-------------------------------"<<endl;
    cout<<m_dAspectRatio<<endl;
    cout<<m_uWidthSize<<endl;
    cout<<m_uHeightSize<<endl;
    cout<<m_uFrameRate<<endl;
    cout<<m_bHmacFlag1<<endl;
    cout<<m_bCryptoFlag1<<endl;
    cout<<m_cKeyID1<<endl;

    unsigned long m_uSamplingRate = 0;
    unsigned long m_uChannelCount = 0;
    unsigned long m_uBitsPerSample = 0;
    bool m_bHmacFlag2 = false;
    bool m_bCryptoFlag2 = false;
    char m_cKeyID2[16] = "";
    cmxfParserModule.GetAudioInfo(m_uSamplingRate, 
        m_uChannelCount,
        m_uBitsPerSample,
        m_bHmacFlag2,
        m_bCryptoFlag2, 
        m_cKeyID2);
    cout<<"-------------------------------"<<endl;
    cout<<m_uSamplingRate<<endl;
    cout<<m_uChannelCount<<endl;
    cout<<m_uBitsPerSample<<endl;
    cout<<m_bHmacFlag2<<endl;
    cout<<m_bCryptoFlag2<<endl;
    cout<<m_cKeyID2<<endl;

    unsigned int m_uPlaintextOffset = 0;
    unsigned int m_uSourceLength = 0;

    int i = 0;
    cmxfParserModule.GetVideoFrameData(i, 
        videoDataBuffer, 
        videoDataLength,
        m_uPlaintextOffset,
        m_uSourceLength);

    cout<<"-------------------------------"<<endl;
    cout<<m_uPlaintextOffset<<endl;
    cout<<m_uSourceLength<<endl;

    cmxfParserModule.GetAudioFrameData(i, 
        audioDataBuffer, 
        audioDataLength,
        m_uPlaintextOffset,
        m_uSourceLength);

    cout<<"-------------------------------"<<endl;
    cout<<m_uPlaintextOffset<<endl;
    cout<<m_uSourceLength<<endl;

#if 1
    FILE *fp1 = fopen("1.j2c", "wb");
    fwrite(videoDataBuffer, 1, videoDataLength, fp1);
    fclose(fp1);

    FILE *fp2 = fopen("1.wav", "wb");
    fwrite(audioDataBuffer, 1, audioDataLength, fp2);
    fclose(fp2);
#endif

    if(NULL != audioDataBuffer)
    {
        delete[] audioDataBuffer;
        audioDataBuffer = NULL;
        audioDataLength = 0;
    }

    if(NULL != videoDataBuffer)
    {
        delete[] videoDataBuffer;
        videoDataBuffer = NULL;
        videoDataLength = 0;
    }


    return 0;
}

