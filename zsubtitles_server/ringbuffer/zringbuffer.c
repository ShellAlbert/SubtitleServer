/*
 * zringbuffer.c
 *
 *  Created on: 2014年12月18日
 *      Author: shell.albert
 */
#include <zringbuffer.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
zint32_t
ZBufferElement_Allocate (ZBufferElement *element)
{
  /**
   * check validation of parameters.
   */
  if (NULL == element)
    {
      return -1;
    }
  /**
   * initial.
   */
  element->elementDataLen = 0;
  element->elementData = (zuint8_t*) malloc (sizeof(zuint8_t) * ZBufferElementMaxSize);
  if (NULL == element->elementData)
    {
      printf ("<zringbuffer>:failed to malloc memory for ZBufferElement->elementData!\n");
      return -1;
    }
  return 0;
}
zint32_t
ZBufferElement_Destroy (ZBufferElement *element)
{
  /**
   * check validation of parameters.
   */
  if (NULL == element)
    {
      return -1;
    }
  if (NULL != element->elementData)
    {
      free (element->elementData);
      element->elementData = NULL;
    }
  return 0;
}
zint32_t
ZMmap_Buffer_Receiver2Parser (ZBufferElement*pBufEle, ZBufferElementR2P *bufEleR2P)
{
  zint32_t offset;
  if (NULL == pBufEle || NULL == bufEleR2P)
    {
      return -1;
    }
  offset = 0;
  //fileType.
  bufEleR2P->fileType = (zuint32_t*) &pBufEle->elementData[offset];
  offset += sizeof(zuint32_t);
  //fileNameLen.
  bufEleR2P->fileNameLen = (zuint32_t*) &pBufEle->elementData[offset];
  offset += sizeof(zuint32_t);
  //fileName.
  bufEleR2P->fileName = (zuint8_t*) &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * ZFILENAME_LEN;
  //fileDataLen.
  bufEleR2P->fileDataLen = (zuint32_t*) &pBufEle->elementData[offset];
  offset += sizeof(zuint32_t);
  //fileData.
  bufEleR2P->fileData = (zuint8_t*) &pBufEle->elementData[offset];
  return 0;
}
zint32_t
ZMmap_Buffer_Parser2Downloader (ZBufferElement*pBufEle, ZBufferElementP2D *bufEleP2D)
{
  zint32_t offset;
  if (NULL == pBufEle || NULL == bufEleP2D)
    {
      return -1;
    }
  offset = 0;
  //frame header,16 bytes.
  bufEleP2D->zFrameHead = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 16;
  //spot number,2 bytes.
  bufEleP2D->zSpotNumber = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 2;
  //left top x,2 bytes.
  bufEleP2D->zLeftTopX = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 2;
  //left top y,2 bytes.
  bufEleP2D->zLeftTopY = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 2;
  //picture width,2 bytes.
  bufEleP2D->zPicWidth = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 2;
  //picture height,2 bytes.
  bufEleP2D->zPicHeight = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 2;
  //time in,3 bytes.
  bufEleP2D->zTimeIn = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 3;
  //time out,3 bytes.
  bufEleP2D->zTimeOut = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 3;

  //frame total number,1 byte.
  bufEleP2D->zFrameTotalNo = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 1;
  //current frame number,1 byte.
  bufEleP2D->zFrameCurrentNo = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 1;

  //fade up alpha step value.
  bufEleP2D->zFadeUpAlphaStep = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 1;
  //fade down alpha step value.
  bufEleP2D->zFadeDownAlphaStep = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 1;

  //fade up end frame sequence number,4 bytes.
  bufEleP2D->zFadeUpEnd = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 4;

  //fade down start frame sequence number,4 bytes.
  bufEleP2D->zFadeDownStart = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 4;

  //data length,4 bytes.
  bufEleP2D->zDataLength = &pBufEle->elementData[offset];
  offset += sizeof(zuint8_t) * 4;

  //data body.
  bufEleP2D->zDataBody = &pBufEle->elementData[offset];
  return 0;
}
zint32_t
ZRingBuffer_Allocate (ZRingBuffer **ringBuffer)
{
  zint32_t i;

  /**
   * allocate structure itself.
   */
  (*ringBuffer) = (ZRingBuffer*) malloc (sizeof(ZRingBuffer));
  if (NULL == (*ringBuffer))
    {
      printf ("err:malloc ZRingBuffer failed!\n");
      return -1;
    }
  /**
   * initial.
   */
  (*ringBuffer)->rdPos = 0;
  (*ringBuffer)->wrPos = 0;
  (*ringBuffer)->totalNum = 0;
  /**
   * malloc element.
   */
  (*ringBuffer)->elementData = (ZBufferElement*) malloc (sizeof(ZBufferElement) * RingBufferElementNum);
  if (NULL == (*ringBuffer)->elementData)
    {
      printf ("malloc ZBufferElement failed!\n");
      return -1;
    }
  /**
   * malloc element internal.
   */
  for (i = 0; i < RingBufferElementNum; i++)
    {
      if (ZBufferElement_Allocate ((*ringBuffer)->elementData + i) < 0)
	{
	  return -1;
	}
    }
  return 0;
}

zint32_t
ZRingBuffer_Destroy (ZRingBuffer **ringBuffer)
{
  zint32_t i;

  /**
   * parameter validation.
   */
  if (NULL == (*ringBuffer))
    {
      return -1;
    }

  /**
   * free element.
   */
  for (i = 0; i < RingBufferElementNum; i++)
    {
      if (NULL != ((*ringBuffer)->elementData + i))
	{
	  ZBufferElement_Destroy ((*ringBuffer)->elementData + i);
//	  ((*ringBuffer)->elementData + i) = NULL;
	}
    }

  /**
   * free ringbuffer.
   */
  if (NULL != (*ringBuffer))
    {
      free ((*ringBuffer));
      (*ringBuffer) = NULL;
    }
  return 0;
}

/**
 * get a element and decrease the total number.
 */
ZBufferElement*
ZRingBuffer_GetAElement (ZRingBuffer *ringBuffer)
{
  /**
   * check the validation of parameters.
   */
  if (NULL == ringBuffer)
    {
      return NULL;
    }
  /**
   * if total number is less than or equal to zero,
   * it means there is not data in fifo.
   */
  if (ringBuffer->totalNum <= 0)
    {
      return NULL;
    }

  /**
   * check pointer validation.
   */
  if (NULL == ringBuffer->elementData)
    {
      return NULL;
    }
  /**
   * return read pointer address.
   */
  return (ringBuffer->elementData + ringBuffer->rdPos);
}
zint32_t
ZRingBuffer_DecTotalNum (ZRingBuffer *ringBuffer)
{
  if (NULL == ringBuffer)
    {
      return -1;
    }

  ringBuffer->rdPos++;
  /**
   * generate loop FIFO.
   */
  if (ringBuffer->rdPos >= RingBufferElementNum)
    {
      ringBuffer->rdPos = 0;
    }

  /**
   * decrease available space.
   */
  if (ringBuffer->totalNum > 0)
    {
      ringBuffer->totalNum--;
    }
  return 0;
}
/**
 * put a element and increase the total number.
 */
ZBufferElement*
ZRingBuffer_PutAElement (ZRingBuffer *ringBuffer)
{
  /**
   * check validation of parameters.
   */
  if (NULL == ringBuffer)
    {
      return NULL;
    }
  if (NULL == ringBuffer->elementData)
    {
      return NULL;
    }
  return (ringBuffer->elementData + ringBuffer->wrPos);
}
zint32_t
ZRingBuffer_IncTotalNum (ZRingBuffer *ringBuffer)
{
  if (NULL == ringBuffer)
    {
      return -1;
    }

  ringBuffer->wrPos++;
  /**
   * generate loop FIFO.
   */
  if (ringBuffer->wrPos >= RingBufferElementNum)
    {
      ringBuffer->wrPos = 0;
    }

  /**
   * increase total number.
   */
  ringBuffer->totalNum++;
  return 0;
}

/**
 * get total element number.
 */
zint32_t
ZRingBuffer_GetTotalNum (ZRingBuffer *ringBuffer)
{
  if (NULL == ringBuffer)
    {
      return -1;
    }
  return ringBuffer->totalNum;
}
zint32_t
ZRingBuffer_IsFull (ZRingBuffer *ringBuffer)
{
  if (ringBuffer->totalNum >= RingBufferElementNum)
    {
      return 0x1;
    }
  else
    {
      return 0x0;
    }
}
zint32_t
ZRingBuffer_IsEmpty (ZRingBuffer *ringBuffer)
{
  if (ringBuffer->totalNum > 0)
    {
      return 0x0;
    }
  else
    {
      return 0x1;
    }
}
