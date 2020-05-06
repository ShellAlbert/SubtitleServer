#include <zthread_download.h>
#include <zspi_device.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <zbuffer_manager.h>
/**
 * @brief thread for download data to FPGA.
 */
void*
zthread_download (void *arg)
{
  struct timeval tCurTime;
  struct timespec tTimeoutTime;
  ZBufferElement *tElement;
  ZBufferElementP2D tBufEleP2D;
  sigset_t tSigMaskSet;
  ZTHREAD_CBS*pcbs = (ZTHREAD_CBS*) (arg);
  if (NULL == pcbs)
    {
      printf ("<zdownloader>:invalid thread control block structure!\n");
      return 0;
    }
  /**
   * mask the SIGIO signal.
   * only handle it in Receiver thread.
   */
  sigemptyset (&tSigMaskSet);
  sigaddset (&tSigMaskSet, SIGIO);
  if (pthread_sigmask (SIG_BLOCK, &tSigMaskSet, NULL))
    {
      printf ("<zdownloader>:block SIGIO signal failed!\n");
      return 0;
    }
  /**
   * loop to wait the P2D RingBuffer has data.
   */
  printf ("<zdownloader>:start\n");
  do
    {
      pthread_mutex_lock (&pcbs->mutexP2D);
      /**
       * sleep until ring buffer has space.
       */
      while (ZRingBuffer_IsEmpty (pcbs->ringBufP2D))
	{
	  /**
	   * pthread_cond_wait_timeout.
	   * pthread_kill detect the parser is live or not.
	   * write strong code tomorrow.
	   */
	  /**
	   * timeout wait 1s.
	   */
	  gettimeofday (&tCurTime, NULL);
	  tTimeoutTime.tv_sec = tCurTime.tv_sec + 10;
	  tTimeoutTime.tv_nsec = 0;
	  if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condP2DNotEmpty, &pcbs->mutexP2D, &tTimeoutTime))
	    {
	      printf ("<zdownloader>:waiting RingBufferP2D Not Empty...\n");
	    }
	  /**
	   * judge thread exit flag in any sleep code.
	   */
	  if (0x1 == pcbs->exitFlag)
	    {
	      pthread_mutex_unlock (&pcbs->mutexP2D);
	      goto ZLabelThreadDone;
	    }
	}

      /**
       * get valid element address.
       */
      tElement = ZRingBuffer_GetAElement (pcbs->ringBufP2D);
      if (NULL == tElement)
	{
	  printf ("<zdownloader>:get a null element from P2D ring buffer!\n");
	  return 0;
	}
      ZMmap_Buffer_Parser2Downloader (tElement, &tBufEleP2D);
#if 1
      printf ("\033[43m<zdownloader>:fetch a element from RingBuffer,len:%d,total is %d.\033[0m\n", ///<
	  tElement->elementDataLen, ZRingBuffer_GetTotalNum (pcbs->ringBufP2D));
#endif
      /**
       * write RingBuffer data to local file.
       */
#if 0
      FILE *tFp;
      zint8_t tFileName[256];
      zuint32_t tTmpData32;
      tTmpData32 = (zuint32_t) (*(tBufEleP2D.zSpotNumber + 0x0)) << 8;
      tTmpData32 |= (zuint32_t) (*(tBufEleP2D.zSpotNumber + 0x1)) << 0;
      sprintf (tFileName, "SpotNumber%d-%d.%d.hex", ///<
	  tTmpData32,///<
	  *(tBufEleP2D.zFrameTotalNo),///<
	  *(tBufEleP2D.zFrameCurrentNo));
      printf ("%s\n", tFileName);
      tFp = fopen (tFileName, "w");
      if (NULL != tFp)
	{
	  if (1 != fwrite (tElement->elementData, tElement->elementDataLen, 1, tFp))
	    {
	      printf ("fwrite() failed!\n");
	    }
	  fclose (tFp);
	  tFp = NULL;
	}
//      printf ("Press Enter to continue...\n");
//      getchar ();
#endif

#if 1
      /**
       * send data to ZProjectorSimulator to display.
       */
      zthread_download_SendToZProjectSimulator (tElement->elementData, tElement->elementDataLen);
      sleep (1);
#endif
      /**
       *
       * so the data is no need to hold,remove it from ring buffer.
       */
      ZRingBuffer_DecTotalNum (pcbs->ringBufP2D);

      /**
       * We released one element,so the RingBuffer is not full.
       * signal to wake up Parser thread to put more elements.
       */
      pthread_cond_signal (&pcbs->condP2DNotFull);
      pthread_mutex_unlock (&pcbs->mutexP2D);
    }
  while (1);
  ZLabelThreadDone: printf ("<zdownloader>:thread done!\n");
  return 0;
}

zint32_t
zthread_download_SendToZProjectSimulator (zuint8_t *buffer, zuint32_t bufferLen)
{
  int sockfd;
  struct sockaddr_in serverAddr;
  unsigned int totalBytes;
  unsigned int sendBytes;
  unsigned int alreadySendBytes;
  unsigned int tData;
  /**
   * check validation of parameters.
   */
  if (NULL == buffer || bufferLen <= 0)
    {
      return -1;
    }

  //create socket.
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf (stderr, "Socket Error:%s\a\n", strerror (errno));
      return -1;
    }
  bzero (&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons (1986);
  serverAddr.sin_addr.s_addr = inet_addr ("172.23.144.86");
  if (connect (sockfd, (struct sockaddr *) (&serverAddr), sizeof(struct sockaddr)) < 0)
    {
      fprintf (stderr, "Connect error:%s\n", strerror (errno));
      return -1;
    }
  printf ("connect to server ok!\n");
//  send data length.
  printf ("sending data length (%d)...\n", bufferLen);
  tData = htonl (bufferLen);
  totalBytes = sizeof(int);
  while (totalBytes > 0)
    {
      sendBytes = send (sockfd, &tData, sizeof(int), 0);
      totalBytes -= sendBytes;
    }
  //send data body.
  printf ("sending data body...\n");
  totalBytes = bufferLen;
  alreadySendBytes = 0;
  while (totalBytes > 0)
    {
      sendBytes = send (sockfd, &buffer[alreadySendBytes], totalBytes, 0);
      alreadySendBytes += sendBytes;
      totalBytes -= sendBytes;
      printf ("send %d bytes.\n", sendBytes);
    }
  printf ("send finish!\n");
  close (sockfd);
  return 0;
}
/**
 * the end of file,tagged by zhangshaoyan.
 */
