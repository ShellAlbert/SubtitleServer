/*
 * zscreen_simulator.h
 *
 *  Created on: 2014年12月23日
 *      Author: shell.albert
 */

#ifndef ZSCREEN_SIMULATOR_H_
#define ZSCREEN_SIMULATOR_H_
#include <zdebug.h>
#include <freetype/fttypes.h>
#include <freetype/ftimage.h>
#include <zfree_type.h>
#include <zdevice_info.h>
#include <zbuffer_manager.h>
#define ConstyFrameRLE32BufferSize	(1*1024*1024)
typedef struct
{

  /**
   * screen size in pixel unit.
   */
  zuint32_t width;
  zuint32_t height;

  /**
   * screen ram buffer.
   * each pixel's structure is AARRGGBB(32-bit).
   */
  zuint32_t *ramBuffer;

  /**
   * Virtual RAM for freetype to use.
   */
  ZVirtualRAM vRam;

  /**
   * hold each valid y frame.
   */
  zuint32_t *yFrameBuffer;
  zuint32_t yFrameLength;

  /**
   * hold data after RLE-32 compressed.
   */
  zuint8_t *yFrameRLE32Buffer;
  zuint32_t yFrameRLE32Length;
} ZScreenSimulatorDev;

extern zint32_t
zscreen_simulator_create (ZScreenSimulatorDev **ssDev, zint32_t width, zint32_t height);
extern zint32_t
zscreen_simulator_destroy (ZScreenSimulatorDev **ssDev);

/**
 * draw a pixel on screen.
 */
extern zint32_t
zscreen_simulator_draw_pixel (ZScreenSimulatorDev *ssDev, zint32_t x, zint32_t y, zint32_t pixelData);
/**
 * draw freetype bitmap.
 */
extern zint32_t
zscreen_simulator_draw_bitmap (ZScreenSimulatorDev *ssDev, FT_Bitmap *bitmap, FT_Int x, FT_Int y);
/**
 * draw text on screen simulator.
 * coordinate:(x,y).
 */
extern zint32_t
zscreen_simulator_draw_text (ZScreenSimulatorDev *ssDev, ///<
    zint32_t textLen, zint32_t *textData, ///<
    ZXmlFontAttr *fontAttr, ///<
    ZXmlTextElement *textElement, ///<
    zuint8_t *ttfBuffer, zuint32_t ttfBufferLen);
/**
 * draw png image on screen simulator.
 */
extern zint32_t
zscreen_simulator_draw_image (ZTHREAD_CBS *pcbs,ZScreenSimulatorDev *ssDev, ZXmlImageElement *imageElement);
/**
 * print out.
 */
extern zint32_t
zscreen_simulator_print (ZScreenSimulatorDev *ssDev);

/**
 * clear screen to zero.
 */
extern zint32_t
zscreen_simulator_clear (ZScreenSimulatorDev *ssDev);

/**
 * scan thread.
 */
typedef struct
{
  /**
   * hold thread id.
   */
  pthread_t tid;
  /**
   * used to indicate this y frame is valid or not.
   * 0:valid.
   * -1:invalid.
   */
  zint32_t isValidFrame;
  /**
   * vertical scan from top to bottom.
   */
  zint32_t yStart;
  zint32_t yEnd;
  /**
   * remember the valid y index.
   */
  zint32_t yValidStart;
  zint32_t yValidEnd;

  /**
   * horizontal scan from left to right.
   * scan the whole horizontal line.
   */
  /**
   * remember the valid x index.
   */
  zint32_t xValidStart;
  zint32_t xValidEnd;

  /**
   * scan body.
   */
  ZScreenSimulatorDev *ssDev;
} ZScanDev;
/**
 * vertical scan valid pixel
 */
extern void*
zthread_screen_simulator_vertical_scan (void *arg);
#endif /* ZSCREEN_SIMULATOR_H_ */
