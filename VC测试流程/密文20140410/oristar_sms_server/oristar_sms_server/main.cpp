#include "mvc2api.h"
#include "CMXFParserModule.h"
#include "mvc2api_securitymanager.h"

#pragma warning(disable : 4996)

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace mvc2;

/// XLQ:
#define MM_LINUX
#define MVC2API_NETWORK_ONLY
//#define IMB_IP_ADDRESS "172.23.168.58"
//#define IMB_IP_ADDRESS "mvc201-000258.local"
//#define IMB_IP_ADDRESS "mvc201-000104.local"
//#define IMB_IP_ADDRESS "mvc201-000026.local"
//#define IMB_IP_ADDRESS "mvc201-001042.local"
//#define IMB_IP_ADDRESS "mvc201-001028.local"
#define IMB_IP_ADDRESS "192.168.1.103"
//#define IMB_IP_ADDRESS "10.7.75.1"
/// XLQ

const char* bin2hex(unsigned char* bin_buf, unsigned int bin_len, char* str_buf, unsigned int str_len)
{
  if ( bin_buf == 0
       || str_buf == 0
       || ((bin_len * 2) + 1) > str_len )
    return 0;

//#ifdef CONFIG_RANDOM_UUID
//  const char* use_random_uuid  = getenv("KM_USE_RANDOM_UUID");
//  if ( use_random_uuid != 0 && use_random_uuid[0] != 0 && use_random_uuid[0] != '0' )
//    return bin2hex_rand(bin_buf, bin_len, str_buf, str_len);
//#endif

  char* p = str_buf;

  for ( unsigned int i = 0; i < bin_len; i++ )
    {
      *p = (bin_buf[i] >> 4) & 0x0f;
      *p += *p < 10 ? 0x30 : 0x61 - 10;
      p++;

      *p = bin_buf[i] & 0x0f;
      *p += *p < 10 ? 0x30 : 0x61 - 10;
      p++;
    }

  *p = '\0';
  return str_buf;
}

TMmRc TransferAudio_CT(MvcDecoder& dec, 
	const unsigned char *audioDataBuffer, 
	unsigned long &audioDataLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength,
	bool &m_bHmacFlag,
	char *m_cKeyID)
{
	DataBuffer dataBuffer;
	TMmRc ret = MMRC_Ok;

	ret = dec.getDataBuffer(dataBuffer, 36000 * 10);
	if (MM_IS_ERROR(ret))
	{
		printf("could not get audio databuffer -> abort\n");
		return(ret);
	}

	uint32_t readbytes = 0;

	/// XLQ:从IV段、CV段这32个字节的后面PlainText Offset段的开始取值，到E(V)的最后一个字节！！！
	if(true == m_bHmacFlag)
	{
		memcpy( dataBuffer.getBufferAddress(), 
			(audioDataBuffer + 32), 
			(audioDataLength - 32 - 56) );

		readbytes = audioDataLength - 32 - 56;

		dataBuffer.setMicValue( (audioDataBuffer + (audioDataLength - 56) ), 56);
	}
	else
	{
		memcpy( dataBuffer.getBufferAddress(), 
			(audioDataBuffer + 32), 
			(audioDataLength - 32) );
		
		readbytes = audioDataLength - 32;
	}

	dataBuffer.setDecryptionSize(m_uPlaintextOffset, m_uSourceLength);

    char keyId[64] = "";
    bin2hex((unsigned char *)m_cKeyID, 16, keyId, 64);
    //printf("%s", keyId);

    mvc2::UuidValue keyid(keyId);
	//mvc2::UuidValue keyid = "25091308b27ed5bf19daec4a1b32a6be";

	dataBuffer.setKeyId(keyid, audioDataBuffer, audioDataBuffer + 16);

	//dataBuffer.setKeyIndex(1, audioDataBuffer, audioDataBuffer + 16);

	uint32_t padding = (16 - (readbytes & 0x0f)) & 0x0f;
	uint8_t *buf = dataBuffer.getBufferAddress();
	for (uint32_t i = 0; i < padding; i++)
	{
		buf[readbytes + i] = 0;
	}

	dataBuffer.send(readbytes + padding);

	return(ret);
}

TMmRc TransferVideo_CT(MvcDecoder& dec, 
	const unsigned char *videoDataBuffer, 
	unsigned long &videoDataLength,
	unsigned int &m_uPlaintextOffset,
	unsigned int &m_uSourceLength,
	bool &m_bHmacFlag,
	char *m_cKeyID)
{
	DataBuffer dataBuffer;
	TMmRc ret = MMRC_Ok;

	ret = dec.getDataBuffer(dataBuffer, 2 * 1024 * 1024);
	if (MM_IS_ERROR(ret))
	{
		printf("could not get databuffer -> abort\n");
		return(ret);
	}

	uint32_t readbytes = 0;

	/// XLQ:从IV段、CV段这32个字节的后面PlainText Offset段的开始取值，到E(V)的最后一个字节！！！
	if(true == m_bHmacFlag)
	{
		memcpy( dataBuffer.getBufferAddress(), 
			(videoDataBuffer + 32), 
			(videoDataLength - 32 - 56) );

		readbytes = videoDataLength - 32 - 56;

		dataBuffer.setMicValue( (videoDataBuffer + (videoDataLength - 56) ), 56);			// wb:固定死 56字节MIC 块？？？
	}
	else
	{
		memcpy( dataBuffer.getBufferAddress(), 
			(videoDataBuffer + 32), 
			(videoDataLength - 32) );
		
		readbytes = videoDataLength - 32;
	}

	dataBuffer.setDecryptionSize(m_uPlaintextOffset, m_uSourceLength);
	
    char keyId[64] = "";
    bin2hex((unsigned char *)m_cKeyID, 16, keyId, 64);
    //printf("%s", keyId);

    mvc2::UuidValue keyid(keyId);
	//mvc2::UuidValue keyid = "007203b983345b84bcb0c928e8bab01b";

	dataBuffer.setKeyId(keyid, videoDataBuffer, videoDataBuffer + 16);

	//dataBuffer.setKeyIndex(0, videoDataBuffer, videoDataBuffer + 16);

	uint32_t padding = (16 - (readbytes & 0x0f)) & 0x0f;
	uint8_t *buf = dataBuffer.getBufferAddress();
	for (uint32_t i = 0; i < padding; i++)
	{
		buf[readbytes + i] = 0;
	}
	//	datbuf.setUserData(framecount);
	//printf("sending pic %d\n",framecount);
	//printf("readbytes %d\n", readbytes);
	//printf("padding %d\n", padding);
	dataBuffer.send(readbytes + padding);
	//fclose(infile);

	//datbuf.wait(100);

	return(ret);
}


TMmRc TransferVideo_PT(MvcDecoder& dec, char *videoDataBuffer, unsigned long &videoDataLength)
{
	DataBuffer dataBuffer;
	TMmRc ret = MMRC_Ok;

	ret = dec.getDataBuffer(dataBuffer, 2 * 1024 * 1024);
	if (MM_IS_ERROR(ret))
	{
		printf("could not get databuffer -> abort\n");
		return(ret);
	}

	memcpy(dataBuffer.getBufferAddress(), videoDataBuffer, videoDataLength);    // videoDataLength 应该是明文J2C的实际长度，直到 FFD9
	uint32_t readbytes = videoDataLength;

	// copy one frame here (as an example we fill the buffer with 1's)
	//uint32_t readbytes = static_cast<uint32_t>(fread(datbuf.getBufferAddress(),1,(size_t)(datbuf.getFreeSize()),infile));
	//int readbytes = fread(datbuf.getBufferAddress(), 1, 1301870, infile);
	//printf("datbuf.getFreeSize() %d\n", datbuf.getFreeSize());
	//printf("readbytes1 %d\n", readbytes);
	uint32_t padding = (16 - (readbytes & 0x0f)) & 0x0f;
	uint8_t *buf = dataBuffer.getBufferAddress();
	for (uint32_t i = 0; i < padding; i++)
	{
		buf[readbytes + i] = 0;
	}
	//	datbuf.setUserData(framecount);
	//printf("sending pic %d\n",framecount);
	//printf("readbytes %d\n", readbytes);
	//printf("padding %d\n", padding);
	dataBuffer.send(readbytes + padding);
	//fclose(infile);

	//datbuf.wait(100);

	return(ret);
}

TMmRc TransferAudio_PT(MvcDecoder& dec, char *audioDataBuffer, unsigned long &audioDataLength)
{
	DataBuffer dataBuffer;
	TMmRc ret = MMRC_Ok;

	ret = dec.getDataBuffer(dataBuffer, 36000 * 10);
	if (MM_IS_ERROR(ret))
	{
		printf("could not get audio databuffer -> abort\n");
		return(ret);
	}

	memcpy(dataBuffer.getBufferAddress(), audioDataBuffer, audioDataLength);
	uint32_t readbytes = audioDataLength;

	uint32_t padding = ( 16 - (readbytes & 0x0f) ) & 0x0f;
	uint8_t *buf = dataBuffer.getBufferAddress();
	for (uint32_t i = 0; i < padding; i++)
	{
		buf[readbytes + i] = 0;
	}
	//	datbuf.setUserData(framecount);
	//printf("sending pic %d\n",framecount);
	//printf("readbytes %d\n", readbytes);
	//printf("padding %d\n", padding);
	dataBuffer.send(readbytes + padding);

	//datbuf.send(audiosDataLength);
	return(ret);
}

#if 1

/// XLQ:播放明文的例子。
int main(int argc, char **argv)
{
	TMmRc ret;

	//if (argc < 2)
	//{
	//    printf("specify filename with full path and printf-style number counting\n");
	//    printf("%s \"<path>\\%%08d\"\n",argv[0]);
	//    exit(0);
	//}

	// MVC20x card enumeration example
	//{
	//    /// XLQ:
	//#ifndef MVC2API_NETWORK_ONLY
	//    MvcDeviceIterator mvcitor;
	//#else
	//    MvcNetDeviceIterator mvcitor(IMB_IP_ADDRESS);
	//#endif
	//    /// XLQ
	//
	//    MvcDevice mvcdev;
	//
	//    while(mvcdev = mvcitor.getNext())
	//    {
	//        printf("MVC card found: UID: %d\n",mvcdev.getUID());
	//        NetworkInterfaceInfo netinfo;
	//        netinfo = mvcdev.getNetworkConfiguration();
	//        uint8_t ipaddr[4];
	//        uint8_t mac[6];
	//        netinfo.getIPAddress(ipaddr);
	//        netinfo.getMACAddress(mac);
	//        printf("NetworkConfiguration: %d.%d.%d.%d at MAC: %02x-%02x-%02x-%02x-%02x-%02x\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3],
	//            mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//    }
	//}

	/// XLQ:连接板卡。
	// use first card example
	MvcDevice mvcdevice;
	/// XLQ:
#ifndef MVC2API_NETWORK_ONLY
	if (! ( mvcdevice = MvcDeviceIterator().getIndex(0) ) )
#else
	if (! ( mvcdevice = MvcNetDeviceIterator(IMB_IP_ADDRESS).getIndex(0) ) )
#endif
		/// XLQ
	{
		printf("MVC card not found\n");
		exit(0);
	}
	else
	{
		printf("MVC card is found!\n");
	}

	/// XLQ:视频部分
	Jpeg2kDecoder j2kdec(&ret, mvcdevice);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create Jpeg2k video decoder: %d\n",ret);
		exit(0);
	}

	j2kdec.setFrameRate(24.00);			// set frame rate, so we don't need to set timestamps anymore

	// create the video output
	//VideoOutput videoout(&ret, mvcdevice, VideoOutput::VideoProperty_Default);
	VideoOutput videoout(&ret, mvcdevice, VideoOutput::VideoProperty_Dual_HDTV);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create video output: %d\n",ret);
		exit(0);
	}

	// setting a video mode (optional)
	//if (MM_IS_ERROR(ret = videoout.setVideoMode(VideoMode::Mode_1920_1080_2400_p)))
	if (MM_IS_ERROR(ret = videoout.setVideoMode(VideoMode::Mode_2048_1080_2400_p)))
	{
		printf("failed to set video mode: %d\n",ret);
	}

	// connect decoder with video output
	if (MM_IS_ERROR(ret = j2kdec.connectOutput(videoout)))
	{
		printf("failed to connect decoder with video output: %d\n",ret);
		exit(0);
	}

	/// XLQ:音频部分
	PCMDecoder pcmDec(&ret, mvcdevice, 24, 6);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create pcm audio decoder: %d\n", ret);
		exit(0);
	}

	AudioOutput audioout(&ret, mvcdevice, 6);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create audio output: %d\n",ret);
		exit(0);
	}

	if (MM_IS_ERROR(ret = audioout.setOutputFrequency(48000)))
	{
		printf("failed to set audio output frequency: %d\n",ret);
	}


	// connect decoder with video output
	if (MM_IS_ERROR(ret = pcmDec.connectOutput(audioout)))
	{
		printf("failed to connect decoder with audio output: %d\n",ret);
		exit(0);
	}
	///

	/// XLQ:播放控制部分
	PlaybackControl playctrl(&ret, mvcdevice);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create playback control: %d\n",ret);
		exit(0);
	}

	// connect decoder with playback
	ret = playctrl.connect(j2kdec);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to connect playback control with video decoder: %d\n",ret);
		exit(0);
	}

	/// XLQ:
	ret = playctrl.connect(pcmDec);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to connect playback control with audio decoder: %d\n",ret);
		exit(0);
	}
	///

	char *videoDataBuffer = new char[2 * 1024 * 1024];
	unsigned long videoDataLength = 0;
	char *audioDataBuffer = new char[36000 * 10];
	unsigned long audioDataLength = 0;

	CMXFParserModule cmxfParserModule;
	unsigned long vidoeFrameSum = 0;
	unsigned long audioFrameSum = 0;
	cmxfParserModule.InitVideoParser("D:\\dieying3_xyz_pt\\dieying3_xyz_video_pt.mxf", vidoeFrameSum);
	cmxfParserModule.InitAudioParser("D:\\dieying3_xyz_pt\\dieying3_xyz_audio_pt.mxf", audioFrameSum);

	double m_dAspectRatio = 0;
	unsigned long m_uWidthSize = 0;
	unsigned long m_uHeightSize = 0;
	unsigned long m_uFrameRate = 0;
	bool m_bHmacFlag1 = false;
	bool m_bCryptoFlag1 = false; 
	char m_cKeyID1[16] = "";													// wb:   16字节？
	cmxfParserModule.GetVideoInfo(m_dAspectRatio,
		m_uWidthSize,
		m_uHeightSize,
		m_uFrameRate,
		m_bHmacFlag1,
		m_bCryptoFlag1, 
		m_cKeyID1);

	if(true == m_bCryptoFlag1)
	{
		/// XLQ:播放的是密文。
		return -1;
	}

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

	if(true == m_bCryptoFlag2)
	{
		/// XLQ:播放的是密文。
		return -1;
	}

	for (int i = 0; i < 20; i++)			// 20 picture preload before run
	{
		unsigned int m_uPlaintextOffset;
		unsigned int m_uSourceLength;

		cmxfParserModule.GetVideoFrameData(i, 
			videoDataBuffer, 
			videoDataLength,
			m_uPlaintextOffset,
			m_uSourceLength);

		ret = TransferVideo_PT(j2kdec, videoDataBuffer, videoDataLength);
		if (ret)
		{
			exit(0);
		}

		cmxfParserModule.GetAudioFrameData(i, 
			audioDataBuffer, 
			audioDataLength,
			m_uPlaintextOffset,
			m_uSourceLength);

		TransferAudio_PT(pcmDec, audioDataBuffer, audioDataLength);
		if (ret)
		{
			exit(0);
		}

	}

	if (ret == MMRC_Ok)
	{
		playctrl.run();

		for (int i = 0; i < vidoeFrameSum; i++)
		{
			unsigned int m_uPlaintextOffset;
			unsigned int m_uSourceLength;
			cmxfParserModule.GetVideoFrameData(i, 
				videoDataBuffer, 
				videoDataLength,
				m_uPlaintextOffset,
				m_uSourceLength);
			ret = TransferVideo_PT(j2kdec, videoDataBuffer, videoDataLength);				// wb : length 是什么长度
			if (ret)
			{
				exit(0);
			}

			cmxfParserModule.GetAudioFrameData(i, 
				audioDataBuffer, 
				audioDataLength,
				m_uPlaintextOffset,
				m_uSourceLength);
			TransferAudio_PT(pcmDec, audioDataBuffer, audioDataLength);
			if (ret)
			{
				exit(0);
			}
		}

		j2kdec.setEndOfStream();

		/// XLQ:
		pcmDec.setEndOfStream();
		/// XLQ
	}

	if (ret != MMRC_Ok)
	{
		//printf("picture transfer failed (%s)\n",filename);
		printf("picture transfer failed\n");
	}

	playctrl.waitForEndOfStream();

	printf("End of stream reached\n");


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

#else

/// XLQ:播放密文的例子。
int main(int argc, char **argv)
{
	TMmRc ret;

	//if (argc < 2)
	//{
	//    printf("specify filename with full path and printf-style number counting\n");
	//    printf("%s \"<path>\\%%08d\"\n",argv[0]);
	//    exit(0);
	//}

	// MVC20x card enumeration example
	//{
	//    /// XLQ:
	//#ifndef MVC2API_NETWORK_ONLY
	//    MvcDeviceIterator mvcitor;
	//#else
	//    MvcNetDeviceIterator mvcitor(IMB_IP_ADDRESS);
	//#endif
	//    /// XLQ
	//
	//    MvcDevice mvcdev;
	//
	//    while(mvcdev = mvcitor.getNext())
	//    {
	//        printf("MVC card found: UID: %d\n",mvcdev.getUID());
	//        NetworkInterfaceInfo netinfo;
	//        netinfo = mvcdev.getNetworkConfiguration();
	//        uint8_t ipaddr[4];
	//        uint8_t mac[6];
	//        netinfo.getIPAddress(ipaddr);
	//        netinfo.getMACAddress(mac);
	//        printf("NetworkConfiguration: %d.%d.%d.%d at MAC: %02x-%02x-%02x-%02x-%02x-%02x\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3],
	//            mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//    }
	//}

	// use first card example
	MvcDevice mvcdevice;
	/// XLQ:
#ifndef MVC2API_NETWORK_ONLY
	if (! ( mvcdevice = MvcDeviceIterator().getIndex(0) ) )
#else
	if (! ( mvcdevice = MvcNetDeviceIterator(IMB_IP_ADDRESS).getIndex(0) ) )
#endif
		/// XLQ
	{
		printf("MVC card not found\n");
		exit(0);
	}
	else
	{
		printf("MVC card is found!\n");
	}


	SecurityManager *sm = new SecurityManager(&ret, mvcdevice);
	if(MMRC_Ok != ret)
	{
		printf("1SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("1SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->loadCertificateChainFile("TMS.MikroM.DC-SMS.V1.000001_chain.pem");
	if(MMRC_Ok != ret)
	{
		printf("2SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("2SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->loadPrivateKeyFile("TMS.MikroM.DC-SMS.V1.000001_prkey.pem");
	if(MMRC_Ok != ret)
	{
		printf("3SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("3SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->connect();
	if(MMRC_Ok != ret)
	{
		printf("4SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("4SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->startSuite();
	if(MMRC_Ok != ret)
	{
		printf("5SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("5SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->uploadCplFile("CPL_c6120e4a-8e09-4999-a195-c71efc4430c8.cpl.xml");
	if(MMRC_Ok != ret)
	{
		printf("6SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("6SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->uploadKdmFile("SM.MikroM.MVC201-001028.v3.pem.6f3e363f-b6c1-4f24-aa7c-41b345cfe386.kdm.xml");
	if(MMRC_Ok != ret)
	{
		printf("7SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("7SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	char authId[] = "1234567890";
	UuidValue cplUuidArray[10] = {"c6120e4a8e094999a195c71efc4430c8"};
	uint32_t arrayLen = 1;
	uint64_t keyExpTime = 0;
	ret = sm->playShow(authId, cplUuidArray, arrayLen, &keyExpTime);
	if(MMRC_Ok != ret)
	{
		printf("8SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("8SecurityManager is success! Success code is [%d]\n", ret);
		printf("keyExpTime is [%d]\n", keyExpTime);

		//return 0;
	}

	//uint8_t *certDataBuffer = new uint8_t[1 * 1024 * 1024];
	//uint32_t certDataLength = 0;

	//uint32_t which = 0;

	//ret = sm->getCertificate(which, certDataBuffer, &certDataLength);
	//if(MMRC_Ok != ret)
	//{
	//	printf("SecurityManager is error! Error code is [%d]\nThe certDataLength is [%d]\n", ret, certDataLength);

	//	return -1;
	//}
	//else
	//{
	//	printf("SecurityManager is success! Success code is [%d]\nThe certDataLength is [%d]\n", ret, certDataLength);

	//	return 0;
	//}

	//SecurityManager::SM_OPERATION smOp = SecurityManager::SM_OPERATION::Operation_Unknown;
	//SecurityManager::SUITE_STATUS suiteStat;
	//uint64_t secureTime;
	//int32_t time_adjust;
	//ret = sm->queryStatus(&smOp, &suiteStat, &secureTime, &time_adjust);
	//if(MMRC_Ok != ret)
	//{
	//	printf("SecurityManager is error! Error code is [%d]\nThe certDataLength is [%d]\n", ret, certDataLength);

	//	return -1;
	//}
	//else
	//{
	//	printf("SecurityManager is success! Success code is [%d]\n", ret);
	//	printf("smOp is %d\n", smOp);
	//	printf("suiteStat is %d\n", suiteStat);
	//	printf("secureTime is %d\n", secureTime);
	//	printf("time_adjust is %d\n", time_adjust);

	//	return 0;
	//}

	//if(NULL != certDataBuffer)
	//{
	//	delete[] certDataBuffer;
	//	certDataBuffer = NULL;
	//	certDataLength = 0;
	//}

	/// XLQ:视频部分
	Jpeg2kDecoder j2kdec(&ret, mvcdevice);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create Jpeg2k video decoder: %d\n",ret);
		exit(0);
	}

	j2kdec.setFrameRate(24.00);			// set frame rate, so we don't need to set timestamps anymore

	// create the video output
	//VideoOutput videoout(&ret, mvcdevice, VideoOutput::VideoProperty_Default);
	VideoOutput videoout(&ret, mvcdevice, VideoOutput::VideoProperty_Dual_HDTV);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create video output: %d\n",ret);
		exit(0);
	}

	// setting a video mode (optional)
	//if (MM_IS_ERROR(ret = videoout.setVideoMode(VideoMode::Mode_1920_1080_2400_p)))
	if (MM_IS_ERROR(ret = videoout.setVideoMode(VideoMode::Mode_2048_1080_2400_p)))
	{
		printf("failed to set video mode: %d\n",ret);
		exit(0);
	}

	// connect decoder with video output
	if (MM_IS_ERROR(ret = j2kdec.connectOutput(videoout)))
	{
		printf("failed to connect decoder with video output: %d\n",ret);
		exit(0);
	}

	if (MM_IS_ERROR(ret = j2kdec.setSecurityManager(*sm)))					// DECODER 里 调用SM 干什么
	{
		printf("j2kDecode failed to setSecurityManager: %d\n",ret);
		exit(0);
	}


	/// XLQ:音频部分		// wb     bitsample /  channel number
	PCMDecoder pcmDec(&ret, mvcdevice, 24, 6);									// wb:  实例化时，已把参数带入； 有单独函数可以进行同样设置
	if (MM_IS_ERROR(ret))													    // 看不出哪里设置了 输入的采样率，也许默认？？
	{
		printf("failed to create pcm audio decoder: %d\n", ret);
		exit(0);
	}

	AudioOutput audioout(&ret, mvcdevice, 6);								
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create audio output: %d\n",ret);
		exit(0);
	}

	if (MM_IS_ERROR(ret = audioout.setOutputFrequency(48000)))					// wb : 设置了 输出采样率，输入难道是默认 48000？？
	{
		printf("failed to set audio output frequency: %d\n",ret);
	}


	// connect decoder with video output
	if (MM_IS_ERROR(ret = pcmDec.connectOutput(audioout)))
	{
		printf("failed to connect decoder with audio output: %d\n",ret);
		exit(0);
	}
	///

	if (MM_IS_ERROR(ret = pcmDec.setSecurityManager(*sm)))					  // wb: 同  VIDEO 的问题
	{
		printf("pcmDecode failed to setSecurityManager: %d\n",ret);
		exit(0);
	}

	/// XLQ:播控部分
	PlaybackControl playctrl(&ret, mvcdevice);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to create playback control: %d\n",ret);
		exit(0);
	}

	// connect decoder with playback
	ret = playctrl.connect(j2kdec);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to connect playback control with video decoder: %d\n",ret);
		exit(0);
	}

	/// XLQ:
	ret = playctrl.connect(pcmDec);
	if (MM_IS_ERROR(ret))
	{
		printf("failed to connect playback control with audio decoder: %d\n",ret);
		exit(0);
	}
	/// XLQ

	unsigned char *videoDataBuffer = new unsigned char[2 * 1024 * 1024];
	unsigned long videoDataLength = 0;
	unsigned char *audioDataBuffer = new unsigned char[36000 * 10];
	unsigned long audioDataLength = 0;

	CMXFParserModule cmxfParserModule;
	unsigned long vidoeFrameSum = 0;
	unsigned long audioFrameSum = 0;
	cmxfParserModule.InitVideoParser("D:\\dieying3_xyz_ct\\dieying3_xyz_video_ct.mxf", vidoeFrameSum);
	cmxfParserModule.InitAudioParser("D:\\dieying3_xyz_ct\\dieying3_xyz_audio_ct.mxf", audioFrameSum);

	double m_dAspectRatio = 0;
	unsigned long m_uWidthSize = 0;
	unsigned long m_uHeightSize = 0;
	unsigned long m_uFrameRate = 0;
	bool m_bHmacFlag1 = false;
	bool m_bCryptoFlag1 = false; 
	char m_cKeyID1[16] = "";								    // wb ：   16个字节？？ 只包括 KEYID ？
	cmxfParserModule.GetVideoInfo(m_dAspectRatio,
		m_uWidthSize,
		m_uHeightSize,
		m_uFrameRate,
		m_bHmacFlag1,
		m_bCryptoFlag1, 
		m_cKeyID1);

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

	for (int i = 0; i < 20; i++)			// 20 picture preload before run
	{
		unsigned int m_uPlaintextOffset;
		unsigned int m_uSourceLength;

		cmxfParserModule.GetVideoFrameData(i, 
			(char *)videoDataBuffer,							// 这个BUFFER 里包括什么？？ 是密文结构的全部内容吗？？
			videoDataLength,
			m_uPlaintextOffset,
			m_uSourceLength);

		ret = TransferVideo_CT(j2kdec, 
			videoDataBuffer, 
			videoDataLength,
			m_uPlaintextOffset,
			m_uSourceLength,
			m_bHmacFlag1,
			m_cKeyID1);
		if (ret)
		{
			exit(0);
		}

		cmxfParserModule.GetAudioFrameData(i,
			(char *)audioDataBuffer, 
			audioDataLength,
			m_uPlaintextOffset,
			m_uSourceLength);

		ret = TransferAudio_CT(pcmDec, 
			audioDataBuffer, 
			audioDataLength,
			m_uPlaintextOffset,
			m_uSourceLength,
			m_bHmacFlag2,
			m_cKeyID2);
		if (ret)
		{
			exit(0);
		}

	}

	if (ret == MMRC_Ok)
	{
		playctrl.run();

		for (int i = 0; i < vidoeFrameSum; i++)
		{
			unsigned int m_uPlaintextOffset;
			unsigned int m_uSourceLength;

			cmxfParserModule.GetVideoFrameData(i, 
				(char *)videoDataBuffer, 
				videoDataLength,
				m_uPlaintextOffset,
				m_uSourceLength);

			ret = TransferVideo_CT(j2kdec, 
				videoDataBuffer, 
				videoDataLength,
				m_uPlaintextOffset,
				m_uSourceLength,
				m_bHmacFlag1,
				m_cKeyID1);
			if (ret)
			{
				exit(0);
			}

			cmxfParserModule.GetAudioFrameData(i,
				(char *)audioDataBuffer, 
				audioDataLength,
				m_uPlaintextOffset,
				m_uSourceLength);

			ret = TransferAudio_CT(pcmDec, 
				audioDataBuffer, 
				audioDataLength,
				m_uPlaintextOffset,
				m_uSourceLength,
				m_bHmacFlag2,
				m_cKeyID2);
			if (ret)
			{
				exit(0);
			}
		}

		j2kdec.setEndOfStream();

		/// XLQ:
		pcmDec.setEndOfStream();
		/// XLQ
	}

	if (ret != MMRC_Ok)
	{
		//printf("picture transfer failed (%s)\n",filename);
		printf("picture transfer failed\n");
	}

	playctrl.waitForEndOfStream();

	printf("End of stream reached\n");

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

	/// XLQ:加密影片结束
	ret = sm->stopShow(authId);
	if(MMRC_Ok != ret)
	{
		printf("9SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("9SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	UuidValue cplUuid = "c6120e4a8e094999a195c71efc4430c8";
	ret = sm->purgeCpl(cplUuid);
	if(MMRC_Ok != ret)
	{
		printf("10SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("10SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}

	ret = sm->stopSuite();
	if(MMRC_Ok != ret)
	{
		printf("11SecurityManager is error! Error code is [%d]\n", ret);

		return -1;
	}
	else
	{
		printf("11SecurityManager is success! Success code is [%d]\n", ret);

		//return 0;
	}
	/// XLQ

	return 0;
}

#endif
