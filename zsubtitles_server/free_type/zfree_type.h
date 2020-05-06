/*
 * zfree_type.h
 *
 *  Created on: 2014年12月23日
 *      Author: shell.albert
 */

#ifndef ZFREE_TYPE_H_
#define ZFREE_TYPE_H_
#include <zdebug.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/config/ftheader.h>
typedef struct
{
  /**
   * buffer for screen ram.
   * [height][width]
   * height<width!
   */
  zuint32_t ramWidth;
  zuint32_t ramHeight;
  //zuint8_t ramBuffer[40 * 2][40 * 100];
  zuint8_t *ramBuffer;

  /**
   * font rectangle glyph.
   */
  zuint32_t topest;
  zuint32_t bottomest;
  zuint32_t leftest;
  zuint32_t rightest;
  /**
   * valid string size.
   */
  zuint32_t str_width;
  zuint32_t str_height;
} ZVirtualRAM;

/**
 * @brief utf8 to unicode-16.
 * @param utf8_str
 * @param ucs2_var
 * @return
 */
extern zint32_t
zfreetype_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t *ucs2_var);

/**
 * @brief split utf8 string and convert into unicode-16(ucs2),
 * store it into chunk structure.
 * @param utf8_str
 * @param zchunk
 * @return
 */
extern zint32_t
zfreetype_split_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t utf8_len, ///<
			      zuint32_t *ucs2_buffer, zuint32_t ucs2_buf_len, zuint32_t *ucs2_count);

//FT_New_Memory_Face
extern zint32_t
zvirtual_ram_create (ZVirtualRAM*dev, zint32_t width, zint32_t height);
extern zint32_t
zvirtual_ram_destroy (ZVirtualRAM*dev);
/**
 * fill screen RAM with pixel data.
 */
enum ZLeanMode
{
  ZLeanMode_Normal = 0x0, ///<
  ZLeanMode_Left = 0x1, ///<
  ZLeanMode_Right = 0x2, ///<
};
extern int
zvirtual_ram_fill (ZVirtualRAM*dev, enum ZLeanMode mode, zuint32_t x_space, ///<
		   zuint32_t *ucs2_buffer, zuint32_t buflen, zuint32_t fontSize, ///<
		   zuint8_t *ttfBuffer, zuint32_t ttfBufferLen);
/**
 * calculate valid pixel data.
 * to find out the topest,bottomest,leftest & rightest coordinate.
 * No need to search all rows!!!
 * for example:
 *
 * xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * xxxxxxxxxxxxxxxxxxxxxxxxxaaaaaxxxxxxxxxxxxxxxxxxxxxxxxxx
 * xxxxxxxxxxxxxxxxxxxaaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * xxxxxxxxxxxxxxxxxaaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * xxxxxxxxxxxxxxxxxxaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 * xxxxxxxxxxaaxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *
 * assume now we only have 3 ucs2 encoding characters,
 * and the font size is 40,so the maximum width is 40*3=120.
 * The size of screen ram simulator is 1920*1080.
 * we do not need to search all columns!
 * we only search the columns from 0 to 120(3*40),bypass other columns to save searching time.
 */
extern int
zvirtual_ram_calc_coordinate (ZVirtualRAM*dev, zuint32_t vRAMHeight, zuint32_t vRAMWidth);
/**
 * print screen RAM content.
 */
extern void
zvirtual_ram_print (ZVirtualRAM*dev);
#endif /* ZFREE_TYPE_H_ */
