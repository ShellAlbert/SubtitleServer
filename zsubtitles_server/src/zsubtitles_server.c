/**
 * @file zsubtitles_parse_server.c
 * @version 1.0
 * @date August 27,2014.
 * @author zhangshaoyan shell.albert@gmail.com
 * @brief 
 *  This program works as a subtitles parse server,
 *  its physical communicate media is VNIC.
 *  Receive xml format subtitles file and font library 
 *  with .ttf or .ttc suffix from ZSDK_CLIENT running in x86.
 *  Parse xml file,extract pixel data from font library,
 *  then send pixel matrix data to FPGA through SPI interface.
 */
#include <zthread_receiver.h>
#include <zthread_parse.h>
#include <zthread_download.h>
#include <zringbuffer.h>
#include <signal.h>
/**
 * process global variables.
 */
ZTHREAD_CBS gCbs;
/**
 * signal handler.
 */
void
ZSignalHandler (zint32_t signo)
{
  printf ("<zsubtitle>:received signal %d!\n", signo);
  /**
   * set exit flag.
   * all threads will check this flag.
   */
  gCbs.exitFlag = 0x1;
  return;
}
int
main (int argc, char **argv)
{
  int ret;

  /**
   * allocate ring buffer.
   * Receiver to Parser.
   */
  if (ZRingBuffer_Allocate (&gCbs.ringBufR2P) < 0)
    {
      return -1;
    }
  pthread_mutex_init (&gCbs.mutexR2P, NULL);
  pthread_cond_init (&gCbs.condR2PNotFull, NULL);
  pthread_cond_init (&gCbs.condR2PNotEmpty, NULL);
  /**
   * allocate ring buffer.
   * Parser to Downloader.
   */
  if (ZRingBuffer_Allocate (&gCbs.ringBufP2D) < 0)
    {
      return -1;
    }
  pthread_mutex_init (&gCbs.mutexP2D, NULL);
  pthread_cond_init (&gCbs.condP2DNotFull, NULL);
  pthread_cond_init (&gCbs.condP2DNotEmpty, NULL);
  /**
   * initial thread exit flag.
   */
  gCbs.exitFlag = 0x0;

  /**
   * video resolution.
   */
  gCbs.widthResolution = 1024;
  gCbs.heightResolution = 768;

  /**
   * frame rate.
   */
  gCbs.frameRate = 24;

  /**
   * install signal handler.
   */
  signal (SIGINT, ZSignalHandler);

  /**
   * create thread for receiver subtitle data.
   */
  ret = pthread_create (&gCbs.zTidReceiver, NULL, zthread_receiver, (void*) &gCbs);
  if (ret)
    {
      printf ("<zsubtitle>:create zthread_receiver failed:%d!\n", ret);
      return -1;
    }
  /**
   * create thread for parse xml.
   */
  ret = pthread_create (&gCbs.zTidParser, NULL, zthread_parser, (void*) &gCbs);
  if (ret)
    {
      printf ("<zsubtitle>:create zthread_parser failed:%d!\n", ret);
      return -1;
    }

  /**
   * create thread for download subtitles pixel data to FPGA.
   */
  ret = pthread_create (&gCbs.zTidDownloader, NULL, zthread_download, (void*) &gCbs);
  if (ret)
    {
      printf ("<zsubtitle>:create zthread_downloader failed:%d!\n", ret);
      return -1;
    }
  /**
   * wait for thread exit.
   */
  pthread_join (gCbs.zTidReceiver, NULL);
  pthread_join (gCbs.zTidParser, NULL);
  pthread_join (gCbs.zTidDownloader, NULL);

  /**
   * free memory here.
   * do some clean work.
   */
  ZRingBuffer_Destroy (&gCbs.ringBufR2P);
  ZRingBuffer_Destroy (&gCbs.ringBufP2D);
  printf ("<zsubtitle>:process done!\n");
  return 0;
}

/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com.
 */
