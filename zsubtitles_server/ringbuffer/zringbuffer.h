/*
 * zringbuffer.h
 *
 *  Created on: 2014年12月18日
 *      Author: shell.albert
 */

#ifndef ZRINGBUFFER_H_
#define ZRINGBUFFER_H_
#include <zdebug.h>
/*
 * buffer element.
 */
#define ZFILENAME_LEN	(256)
#define ZBufferElementMaxSize	(2*1024*1024)
typedef struct
{
  zuint32_t elementDataLen;
  zuint8_t *elementData;
} ZBufferElement;
extern zint32_t
ZBufferElement_Allocate (ZBufferElement *element);
extern zint32_t
ZBufferElement_Destroy (ZBufferElement *element);
/**
 * Receiver to Parser.
 */
typedef struct
{
  zuint32_t *fileType;
  zuint32_t *fileNameLen;
  zuint8_t *fileName;
  zuint32_t *fileDataLen;
  zuint8_t *fileData;
} ZBufferElementR2P;
extern zint32_t
ZMmap_Buffer_Receiver2Parser (ZBufferElement*pBufEle, ZBufferElementR2P *bufEleR2P);
/**
 * Parser to Downloader.
 */
typedef struct
{
  zuint8_t *zFrameHead; ///<takes up 16 bytes.

  zuint8_t *zSpotNumber; ///< takes up 2 bytes.

  zuint8_t *zLeftTopX; ///< takes up 2 bytes.
  zuint8_t *zLeftTopY; ///< takes up 2 bytes.

  zuint8_t *zPicWidth; ///< takes up 2 bytes.
  zuint8_t *zPicHeight; ///< takes up 2 bytes.

  zuint8_t *zTimeIn; ///< takes up 3 bytes.
  zuint8_t *zTimeOut; ///< takes up 3 bytes.

  zuint8_t *zFrameTotalNo; ///<takes up 1 bytes.
  zuint8_t *zFrameCurrentNo; ///<takes up 1 bytes.
  zuint8_t *zFadeUpAlphaStep; ///< fade up Alpha step value.
  zuint8_t *zFadeDownAlphaStep; ///< fade down Alpha step value.

  zuint8_t *zFadeUpEnd; ///<takes up 4 bytes,fade up end frame sequence number.
  zuint8_t *zFadeDownStart; ///<takes up 4 bytes,fade down start frame sequence number.

  zuint8_t *zDataLength; ///< takes up 4 bytes.
  zuint8_t *zDataBody;
} ZBufferElementP2D;
extern zint32_t
ZMmap_Buffer_Parser2Downloader (ZBufferElement*pBufEle, ZBufferElementP2D *bufEleP2D);
/**
 * ring buffer.
 */
#define RingBufferElementNum	10
typedef struct
{
  zint32_t rdPos;
  zint32_t wrPos;
  zint32_t totalNum;
  ZBufferElement *elementData;
} ZRingBuffer;

extern zint32_t
ZRingBuffer_Allocate (ZRingBuffer **ringBuffer);
/**
 * get a element and decrease the total number.
 */
extern ZBufferElement*
ZRingBuffer_GetAElement (ZRingBuffer *ringBuffer);
extern zint32_t
ZRingBuffer_DecTotalNum (ZRingBuffer *ringBuffer);
/**
 * put a element and increase the total number.
 */
extern ZBufferElement*
ZRingBuffer_PutAElement (ZRingBuffer *ringBuffer);
extern zint32_t
ZRingBuffer_IncTotalNum (ZRingBuffer *ringBuffer);

/**
 * get total element number.
 */
extern zint32_t
ZRingBuffer_GetTotalNum (ZRingBuffer *ringBuffer);
extern zint32_t
ZRingBuffer_IsFull (ZRingBuffer *ringBuffer);
extern zint32_t
ZRingBuffer_IsEmpty (ZRingBuffer *ringBuffer);
extern zint32_t
ZRingBuffer_Destroy (ZRingBuffer **ringBuffer);
#endif /* ZRINGBUFFER_H_ */
