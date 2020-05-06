/*
 * zthread_file_receiver.c
 *
 *  Created on: 2014年10月15日
 *      Author: shell.albert
 */
#include <zthread_receiver.h>
#include <zthread_parse.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>

#define ECSPI2_DEV	"/dev/imx5_ecspi2"
zint32_t
zint8x4_to_zint32 (const zuint8_t *dataChar8, zuint32_t *dataInt32)
{
  zuint32_t tmpInt32;
  if (NULL == dataChar8 || NULL == dataInt32)
    {
      return -1;
    }
  tmpInt32 = ((zuint32_t) dataChar8[0] << 24);
  tmpInt32 |= ((zuint32_t) dataChar8[1] << 16);
  tmpInt32 |= ((zuint32_t) dataChar8[2] << 8);
  tmpInt32 |= ((zuint32_t) dataChar8[3] << 0);
  *dataInt32 = tmpInt32;
  return 0;
}
zint32_t
zint32_to_zint8x4 (const zuint32_t dataInt32, zuint8_t *dataChar8)
{
  if (NULL == dataChar8)
    {
      return -1;
    }
  *(dataChar8 + 0) = (zuint32_t) (dataInt32 >> 24);
  *(dataChar8 + 1) = (zuint32_t) (dataInt32 >> 16);
  *(dataChar8 + 2) = (zuint32_t) (dataInt32 >> 8);
  *(dataChar8 + 3) = (zuint32_t) (dataInt32 >> 0);
  return 0;
}
/**
 * access limited.
 */
static ZTHREAD_CBS *gCbsReceiver = NULL;
static zint32_t gfdDev = -1;
static zuint8_t *gPlainBuffer = NULL;
static zuint32_t gPlainBufferTotal = 0;
const zuint8_t gSyncHeader[] =
  { 0x11, 0x11, 0x11, 0x11, ///<
      0x99, 0x99, 0x99, 0x99, ///<
      0x88, 0x88, 0x88, 0x88, ///<
      0x77, 0x77, 0x77, 0x77, ///<
    };
zuint32_t gIRQTimes = 0;
/**
 * switch to show more detail.
 */
zuint32_t bPrintIRQDetail = 0;
zuint32_t bPrintCombineStreamData = 0;
/**
 * @brief thread for receive file from x86.
 */
//#define UseDataGenerateSimulator  0x1
void*
zthread_receiver (void *arg)
{
  zint32_t ret;
  zint32_t flags;
  struct timeval tTv;
  struct timespec tTimeout;
  pthread_mutex_t tMutex;
  pthread_cond_t tCond;

  /**
   * hold the global resource.
   */
  gCbsReceiver = (ZTHREAD_CBS*) (arg);
  if (NULL == gCbsReceiver)
    {
      printf ("err:zthread_receiver exit due to error convert!\n");
      return 0;
    }
#ifdef UseDataGenerateSimulator
  /**
   * read data from local hard disk to simulator data block,
   * copy them into ring buffer to driver Parser thread.
   */
  ZBufferElement *tBufferElement;
  ZBufferElementR2P tBufEleR2P;
  const zint8_t *xmlFileName = "dieying3.xml";
  zint8_t *fileBuffer;
  zuint32_t fileLength;
  FILE *fp;
  fp = fopen (xmlFileName, "r");
  if (NULL == fp)
    {
      printf ("open %s failed\n", xmlFileName);
      return 0;
    }
  fseek (fp, 0L, SEEK_SET);
  fseek (fp, 0L, SEEK_END);
  fileLength = ftell (fp);
  if (fileLength <= 0)
    {
      printf ("wrong file length,exit\n");
      return 0;
    }
  fileBuffer = (zint8_t*) malloc (fileLength);
  if (NULL == fileBuffer)
    {
      printf ("malloc() failed,exit\n");
      return 0;
    }
  fseek (fp, 0L, SEEK_SET);
  if (fread (fileBuffer, fileLength, 1, fp) != 1)
    {
      printf ("fread() failed1\n");
      return 0;
    }
  fclose (fp);

  /**
   * put data into ReceiverToParser FIFO.
   */
  pthread_mutex_lock (&gCbsReceiver->mutexR2P);
  do
    {
      /**
       * sleep until ring buffer has space.
       */
      while (ZRingBuffer_IsFull (gCbsReceiver->ringBufR2P))
	{
	  pthread_cond_wait (&gCbsReceiver->condR2P, &gCbsReceiver->mutexR2P);
	  /**
	   * judge thread exit flag in any sleep code.
	   */
	  if (0x1 == gCbsReceiver->exitFlag)
	    {
	      pthread_mutex_unlock (&gCbsReceiver->mutexR2P);
	      pthread_exit (0);
	    }
	}
      /**
       * directly memory access.
       */
      tBufferElement = ZRingBuffer_PutAElement (gCbsReceiver->ringBufR2P);
      if (NULL == tBufferElement)
	{
	  printf ("err:get invalid element address for putting!\n");
	  break;
	}
      /**
       * put data into ring buffer.
       */
      ZMmap_Buffer_Receiver2Parser (tBufferElement, &tBufEleR2P);
      *(tBufEleR2P.fileType) = Subtitle_File_Type_XML;
      *(tBufEleR2P.fileNameLen) = strlen (xmlFileName);
      memcpy (tBufEleR2P.fileName, xmlFileName, *(tBufEleR2P.fileNameLen));
      *(tBufEleR2P.fileDataLen) = fileLength;
      memcpy (tBufEleR2P.fileData, fileBuffer, fileLength);

      /**
       * add data finish, so the total should be increase.
       */
      ZRingBuffer_IncTotalNum (gCbsReceiver->ringBufR2P);
      /**
       * wake up parser thread.
       */
      pthread_cond_signal (&gCbsReceiver->condR2P);
    }
  while (0);
  pthread_mutex_unlock (&gCbsReceiver->mutexR2P);

  free (fileBuffer);
  printf ("Receiver:put xml data into RingBuffer finish!\n");
  printf ("info:Receiver thread done!\n");
  return 0;
#else
  /**
   * open device file,
   * and start async notify.
   */
  gfdDev = open (ECSPI2_DEV, O_RDWR);
  if (gfdDev < 0)
    {
      printf ("err:open %s failed:%s!", ECSPI2_DEV, strerror (errno));
      return 0;
    }

  /**
   * allocate plain buffer for frame combine.
   */
  gPlainBuffer = (zuint8_t*) malloc (1 * 1024 * 1024);
  if (NULL == gPlainBuffer)
    {
      printf ("err:zthread_receiver:malloc for plain buffer failed!\n");
      return 0;
    }

  /**
   * async notify.
   */
  signal (SIGIO, subtitle_data_async_handler);
  fcntl (gfdDev, F_SETOWN, getpid ());
  flags = fcntl (gfdDev, F_GETFL);
  fcntl (gfdDev, F_SETFL, flags | FASYNC);

  /**
   * timeout to check exit flag.
   */
  pthread_mutex_init (&tMutex, NULL);
  pthread_cond_init (&tCond, NULL);
  printf ("<zreceiver>:start\n");
  while (1)
    {
      /**
       * sleep until others wake me up.
       * check the exit flag.
       */
      if (0x1 == gCbsReceiver->exitFlag)
	{
	  printf ("<zreceiver>:detected exit flag!\n");
	  break;
	}
#if 1
      /**
       * here we should do a sleep to reduce cpu heavy load.
       * timeout 2s to check the exit flag.
       */
      gettimeofday (&tTv, NULL);
      tTimeout.tv_sec = tTv.tv_sec + 2;
      tTimeout.tv_nsec = 0;
      pthread_mutex_lock (&tMutex);
      if (ETIMEDOUT == pthread_cond_timedwait (&tCond, &tMutex, &tTimeout))
	{
	  //printf ("2s timeout!\n");
	}
      pthread_mutex_unlock (&tMutex);
#endif
    }
  /**
   * do clean work here.
   */
  close (gfdDev);
  gfdDev = -1;
  free (gPlainBuffer);
  gPlainBuffer = NULL;
  pthread_cond_destroy (&tCond);
  pthread_mutex_destroy (&tMutex);
#endif
  printf ("<zreceiver>:done!\n");
  pthread_exit (0);
  return 0;
}

void
subtitle_data_async_handler (zint32_t signo)
{
  SPIReg reg;
  zuint32_t length, length_int;
  zint32_t i;
  /**
   * read data length.
   * unit is byte,but spi is 32-bit bus,
   * so do a convert.
   */
  reg.addr = ZAddr_Length;
  if (read (gfdDev, &reg, sizeof(reg)) < 0)
    {
      printf ("<zreceiver>:read spi reg 0x%x!\n", reg.addr);
      return;
    }
  if (bPrintIRQDetail)
    {
      printf ("<zreceiver>:read %d bytes\n", reg.val);
    }

  length = reg.val;
  length_int = length / sizeof(zint32_t);
  if (length <= 0 || length_int <= 0)
    {
      printf ("<zreceiver>:wrong subtitle data length!\n");
      return;
    }

  /**
   * read data to global plain buffer.
   */
  for (i = 0; i < length_int; i++)
    {
      reg.addr = ZAddr_Data;
      if (read (gfdDev, &reg, sizeof(reg)) < 0)
	{
	  printf ("<zreceiver>:read spi reg 0x%x!\n", reg.addr);
	  return;
	}
      zint32_to_zint8x4 (reg.val, &gPlainBuffer[gPlainBufferTotal]);
      gPlainBufferTotal += sizeof(zint32_t);
    }

  /**
   * write any value to length register to clear IRQ.
   */
  reg.addr = ZAddr_Length;
  reg.val = 0x7758521;
  if (write (gfdDev, &reg, sizeof(reg)) < 0)
    {
      printf ("<zreceiver>:write spi reg 0x%x!\n", reg.addr);
      return;
    }
  if (bPrintIRQDetail)
    {
      printf ("<zreceiver>:write 0x7758521 to clear IRQ\n");
    }

//  printf ("IRQ times=%d\n", ++gIRQTimes);
  /**
   * try to parse plain buffer.
   */
  receiverParsePlainBufferCombineFrame ();

  /**
   * re-enable subtitle IRQ source.
   */
  reg.addr = ZAddr_IrqCtl;
  reg.val = (0x1 << 0);
  if (write (gfdDev, &reg, sizeof(reg)) < 0)
    {
      printf ("<zreceiver>:failed to re-enable subtitle IRQ source!\n");
      return;
    }
  if (bPrintIRQDetail)
    {
      printf ("<zreceiver>:re-enable IRQ!\n");
    }
  return;
}

void
receiverParsePlainBufferCombineFrame (void)
{
  zuint32_t fileType;
  zuint32_t fileLength;
  zuint32_t fileNameLen;
  zuint8_t fileName[256];
  zuint32_t fileDataLen;
  zuint8_t *fileData;
  zint8_t bFindSyncHeader;
  zuint32_t bufIndex;
  ZBufferElement *tBufferElement;
  ZBufferElementR2P tBufEleR2P;

  //timeout to check exit flag.
  struct timeval tTv;
  struct timespec tTimeout;
  zuint32_t i;

  /**
   * sync header: 4*4=16 bytes.
   * file type: 4 bytes.
   * file length:4 bytes.
   * file name len:4 bytes.
   * file name: n bytes.
   * file data len:4 bytes.
   * file data: n bytes.
   *
   * so one frame length should be 16+4+4+4+4=32 at least.
   */
  if (gPlainBufferTotal < 32)
    {
      /**
       * do not handle,we need more data.
       */
      return;
    }

  /**
   * search to find sync header.
   */
  bFindSyncHeader = 0x0;
  for (i = 0; i < gPlainBufferTotal - sizeof(gSyncHeader); i++)
    {
      if (0 == strncmp ((zint8_t*) gSyncHeader, (zint8_t*) &gPlainBuffer[i], sizeof(gSyncHeader)))
	{
	  bFindSyncHeader = 0x1;
	  /**
	   * remember the sync header index.
	   */
	  bufIndex = i;
	  if (bPrintCombineStreamData)
	    {
	      printf ("<zreceiver>:find sync header ok!\n");
	    }
	  break;
	}
    }

  /**
   * if we do not find the valid sync header,
   * it means this may be an invalid frame,
   * abandon it,reset buffer.
   */
  if (0x0 == bFindSyncHeader)
    {
      if (gPlainBufferTotal >= 512 * 1024)
	{
	  if (bPrintCombineStreamData)
	    {
	      printf ("<zreciver>:do not find sync header,reset!\n");
	    }
	  gPlainBufferTotal = 0;
	}
      else
	{
	  if (bPrintCombineStreamData)
	    {
	      printf ("<zreceiver>:we need more data!\n");
	    }
	}
      return;
    }

  /**
   * we find the sync header,
   * start to parse.
   * if the rest data in buffer is less than 32 bytes,
   * we should wait for more data.
   */
  if (gPlainBufferTotal - bufIndex < 32)
    {
      return;
    }

  /**
   * skip 16 bytes sync header.
   */
  bufIndex += sizeof(gSyncHeader);
  /**
   * file type.
   */
  zint8x4_to_zint32 (&gPlainBuffer[bufIndex], &fileType);
  bufIndex += sizeof(fileType);
  if (bPrintCombineStreamData)
    {
      printf ("fileType=0x%x\n", fileType);
    }

  /**
   * file length.
   */
  zint8x4_to_zint32 (&gPlainBuffer[bufIndex], &fileLength);
  bufIndex += sizeof(fileLength);
  if (bPrintCombineStreamData)
    {
      printf ("fileLength:%d\n", fileLength);
    }

  /**
   * file name length.
   */
  zint8x4_to_zint32 (&gPlainBuffer[bufIndex], &fileNameLen);
  bufIndex += sizeof(fileNameLen);
  if (bPrintCombineStreamData)
    {
      printf ("fileNameLen:%d\n", fileNameLen);
    }

  /**
   * file name.
   */
  memset (fileName, 0, sizeof(fileName));
  memcpy (fileName, &gPlainBuffer[bufIndex], fileNameLen);
  bufIndex += fileNameLen;
  if (bufIndex > gPlainBufferTotal)
    {
      /**
       * index overflow,we do not have enough data.
       * wait for next time schedule.
       */
      return;
    }
  if (bPrintCombineStreamData)
    {
      printf ("fileName=%s\n", fileName);
    }

  /**
   * file data length.
   */
  zint8x4_to_zint32 (&gPlainBuffer[bufIndex], &fileDataLen);
  bufIndex += sizeof(fileDataLen);
  if (bufIndex > gPlainBufferTotal)
    {
      /**
       * index overflow,we do not have enough data.
       * wait for next time schedule.
       */
      return;
    }
  if (bPrintCombineStreamData)
    {
      printf ("fileDataLen:%d\n", fileDataLen);
    }
  /**
   * file data body.
   */
  fileData = &gPlainBuffer[bufIndex];
  bufIndex += fileDataLen;
  if (bufIndex > gPlainBufferTotal)
    {
      /**
       * index overflow,we do not have enough data.
       * wait for next time schedule.
       */
      return;
    }

  /**
   * put data into ReceiverToParser FIFO.
   */
  pthread_mutex_lock (&gCbsReceiver->mutexR2P);
  do
    {
      /**
       * sleep until ring buffer has space.
       */
      while (ZRingBuffer_IsFull (gCbsReceiver->ringBufR2P))
	{
	  gettimeofday (&tTv, NULL);
	  tTimeout.tv_sec = tTv.tv_sec + 10;
	  tTimeout.tv_nsec = 0;
	  if (ETIMEDOUT == pthread_cond_timedwait (&gCbsReceiver->condR2PNotFull, &gCbsReceiver->mutexR2P, &tTimeout))
	    {
	      printf ("<zreceiver>:waiting RingBufferR2P Not Full...\n");
	    }
	  /**
	   * judge thread exit flag in any sleep code.
	   */
	  if (0x1 == gCbsReceiver->exitFlag)
	    {
	      pthread_mutex_unlock (&gCbsReceiver->mutexR2P);
	      return;
	    }
	}
      /**
       * directly memory access.
       */
      tBufferElement = ZRingBuffer_PutAElement (gCbsReceiver->ringBufR2P);
      if (NULL == tBufferElement)
	{
	  printf ("<zreceiver>:get invalid element address for putting!\n");
	  break;
	}
      /**
       * put data into ring buffer.
       */
      ZMmap_Buffer_Receiver2Parser (tBufferElement, &tBufEleR2P);
      *(tBufEleR2P.fileType) = fileType;
      *(tBufEleR2P.fileNameLen) = fileNameLen;
      memcpy (tBufEleR2P.fileName, fileName, fileNameLen);
      /**
       * terminated with zero.
       */
      tBufEleR2P.fileName[fileNameLen] = 0;
      *(tBufEleR2P.fileDataLen) = fileDataLen;
      memcpy (tBufEleR2P.fileData, fileData, fileDataLen);

      /**
       * add data finish, so the total should be increase.
       */
      ZRingBuffer_IncTotalNum (gCbsReceiver->ringBufR2P);

#if 1
      printf ("\033[42m<zreceiver>:put 0x%x into RingBufferR2P,len:%d,total:%d\033[0m\n", ///<
	  fileType, fileDataLen, ZRingBuffer_GetTotalNum (gCbsReceiver->ringBufR2P));
#endif

      /**
       * We put one element into RingBuffer,so it's not empty.
       * signal to wake up Parser thread.
       */
      pthread_cond_signal (&gCbsReceiver->condR2PNotEmpty);
    }
  while (0);
  pthread_mutex_unlock (&gCbsReceiver->mutexR2P);

  /**
   * shift data for next frame.
   */
  if (gPlainBufferTotal - bufIndex > 0)
    {
      /**
       * the buffer have rest data,
       * should be shift to the front.
       */
      memmove (gPlainBuffer, &gPlainBuffer[bufIndex], gPlainBufferTotal - bufIndex);
      gPlainBufferTotal -= bufIndex;
    }
  else
    {
      gPlainBufferTotal = 0;
    }
  return;
}

/**
 * the end of file,tagged by zhangshaoyan.
 */

