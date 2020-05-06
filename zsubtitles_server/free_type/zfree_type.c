/*
 * zfree_type.c
 *
 *  Created on: 2014年12月23日
 *      Author: shell.albert
 */
#include <zfree_type.h>
#include <stdio.h>
#include <math.h>
#include <freetype/freetype.h>
/**
 * @brief utf8 to unicode-16.
 * @param utf8_str
 * @param ucs2_var
 * @return
 */
zint32_t
zfreetype_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t *ucs2_var)
{
  /**
   * check valid of parameters.
   */
  if (NULL == utf8_str || NULL == ucs2_var)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  /**
   * ASCII character.
   * less than 128.
   */
  if (*utf8_str < 0x80)
    {
      *ucs2_var = *utf8_str;
      return 0;
    }
  if ((utf8_str[0] & 0xE0) == 0xE0)
    {
      if (utf8_str[1] == 0 || utf8_str[2] == 0)
	{
	  return -1;
	}
      *ucs2_var = (utf8_str[0] & 0x0F) << 12 | (utf8_str[1] & 0x3F) << 6 | (utf8_str[2] & 0x3F);
      return 0;
    }

  if ((utf8_str[0] & 0xC0) == 0xC0)
    {
      if (utf8_str[1] == 0)
	{
	  return -1;
	}
      *ucs2_var = (utf8_str[0] & 0x1F) << 6 | (utf8_str[1] & 0x3F);
      return 0;
    }
  return -1;
}

/**
 * @brief split utf8 string and convert into unicode-16(ucs2),
 * store it into chunk structure.
 * @param utf8_str
 * @param zchunk
 * @return
 */
zint32_t
zfreetype_split_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t utf8_len, ///<
			      zuint32_t *ucs2_buffer, zuint32_t ucs2_buf_len, zuint32_t *ucs2_count)
{
  zuint8_t *ptr;
  zuint32_t ucs2_code;
  zuint32_t ucs2_index;
  zuint32_t step;

  zint32_t tmp_ucs2_count = 0;
  /**
   * check valid of parameters.
   */
  if (NULL == utf8_str || utf8_len <= 0 || NULL == ucs2_buffer || ucs2_buf_len <= 0 || NULL == ucs2_count)
    {
      printf ("<freetype>:split,invalid parameters!\n");
      return -1;
    }

  /**
   * default step is 1.
   */
  step = 1;
  ucs2_index = 0;
  for (ptr = utf8_str; *ptr != '\0'; ptr += step)
    {
      /**
       * ASCII character.
       */
      if (*ptr < 128)
	{
	  //printf ("utf8:0x%x   --->   ", *ptr);
	  zfreetype_utf8_to_ucs2 (ptr, &ucs2_code);
	  //printf ("ucs2:0x%x\n", ucs2_code);
	  /**
	   * store ucs2 code in chunk structure.
	   */
	  ucs2_buffer[ucs2_index++] = ucs2_code;
	  tmp_ucs2_count++;
	  step = 1;
	}
      else
	{
	  /**
	   * other character.
	   */
	  //printf ("utf8:0x%x 0x%x 0x%x   --->   ", *ptr, *(ptr + 1), *(ptr + 2));
	  zfreetype_utf8_to_ucs2 (ptr, &ucs2_code);
	  //printf ("ucs2:0x%x\n", ucs2_code);
	  /**
	   * store ucs2 code in chunk structure.
	   */
	  ucs2_buffer[ucs2_index++] = ucs2_code;
	  tmp_ucs2_count++;
	  step = 3;
	}
    }
  /**
   * return the counter.
   */
  *ucs2_count = tmp_ucs2_count;
  return 0;
}
zint32_t
zvirtual_ram_create (ZVirtualRAM*dev, zint32_t width, zint32_t height)
{
  if (NULL == dev)
    {
      printf ("<VRAM>:init,null poiter!\n");
      return -1;
    }
  /**
   * malloc virtual ram.
   */
  dev->ramBuffer = (zuint8_t*) malloc (width * height);
  if (NULL == dev->ramBuffer)
    {
      printf ("<ZVirtualRAM>:malloc() failed!\n");
      return -1;
    }
  memset (dev->ramBuffer, 0, width * height);
  /**
   * initial internal control variables.
   */
  dev->topest = 0;
  dev->bottomest = 0;
  dev->leftest = 0;
  dev->rightest = 0;
  dev->ramWidth = width;
  dev->ramHeight = height;
  return 0;
}
zint32_t
zvirtual_ram_destroy (ZVirtualRAM*dev)
{
  if (NULL == dev)
    {
      return -1;
    }
  if (NULL != dev->ramBuffer)
    {
      free (dev->ramBuffer);
      dev->ramBuffer = NULL;
    }
  return 0;
}
zint32_t
zvirtual_ram_fill (ZVirtualRAM*dev, enum ZLeanMode mode, zuint32_t x_space, ///<
		   zuint32_t *ucs2_buffer, zuint32_t buflen, zuint32_t fontSize, ///<
		   zuint8_t *ttfBuffer, zuint32_t ttfBufferLen)
{
  FT_Library tFTLibs;
  FT_Face tFace;
  FT_GlyphSlot tSlot;
  FT_Matrix tMatrix; /* transformation matrix */
  FT_Vector tPen; /* untransformed origin  */
  FT_Error error2;
  FT_UInt uiGlyphIndex;
  double angle;
  FT_Int i, j, p, q;
  int x, y;
  FT_Int x_max, y_max;
  int n;

  /**
   * check parameter validation.
   */
  if (dev == NULL || ucs2_buffer == NULL || buflen <= 0 || NULL == ttfBuffer || ttfBufferLen <= 0)
    {
      printf ("<ZVirtualRAM>:invalid parameters!\n");
      return -1;
    }

  /**
   * do matrix transfer.
   */
  angle = (0.0 / 360) * 3.14159 * 2;

  error2 = FT_Init_FreeType (&tFTLibs);

#if 0
  error2 = FT_New_Face (tFTLibs, "wqy-microhei.ttc", 0, &tFace);
#else
  error2 = FT_New_Memory_Face (tFTLibs, ttfBuffer, ttfBufferLen, 0, &tFace);
#endif

  /**
   * set font size.
   */
  FT_Set_Pixel_Sizes (tFace, fontSize, fontSize);

  tSlot = tFace->glyph;

  switch (mode)
    {
    case ZLeanMode_Normal:
      /**
       * lean normal.
       */
      tMatrix.xx = (FT_Fixed) (cos (angle) * 0x10000L);
      tMatrix.xy = (FT_Fixed) (-sin (angle) * 0x10000L);
      tMatrix.yx = (FT_Fixed) (sin (angle) * 0x10000L);
      tMatrix.yy = (FT_Fixed) (cos (angle) * 0x10000L);
      tPen.x = 40 * 10 * 1;
      tPen.y = (dev->ramHeight / 2 - 10) * 64;
      break;

      /**
       * right lean.
       */
    case ZLeanMode_Right:
      tMatrix.xx = 0x10000L;
      tMatrix.xy = 1.0f * 0x10000L;
      tMatrix.yx = 0;
      tMatrix.yy = 0x10000L;
      tPen.x = 40 * 10 * 1;
      tPen.y = (dev->ramHeight / 2 - 10) * 64;
      break;

      /**
       * left lean.
       */
    case ZLeanMode_Left:
      tMatrix.xx = 0x10000L;
      tMatrix.xy = -1.0f * 0x10000L;
      tMatrix.yx = 0;
      tMatrix.yy = 0x10000L;
      tPen.x = 40 * 10 * 4;
      tPen.y = (dev->ramHeight / 2 - 10) * 64;
      break;
    }

  /**
   * clear to zero.
   */
  memset (dev->ramBuffer, 0, dev->ramHeight * dev->ramWidth);

  for (n = 0; n < buflen; n++)
    {
#if 0
      printf ("freetype now is 0x%x\n", ucs2_buffer[n]);
#endif
      FT_Set_Transform (tFace, &tMatrix, &tPen);

      uiGlyphIndex = FT_Get_Char_Index (tFace, ucs2_buffer[n]);
      FT_Load_Glyph (tFace, uiGlyphIndex, FT_LOAD_DEFAULT);
      /**
       * 1bit black&white.
       * 8bit colorful.
       */
      FT_Render_Glyph (tFace->glyph, FT_RENDER_MODE_NORMAL);

      /**
       * pixel handler.
       */

      x = tSlot->bitmap_left;
      y = dev->ramHeight - tSlot->bitmap_top;
      x_max = x + tSlot->bitmap.width;
      y_max = y + tSlot->bitmap.rows;

      for (i = x, p = 0; i < x_max; i++, p++)
	{
	  for (j = y, q = 0; j < y_max; j++, q++)
	    {
	      if (i < 0 || j < 0 || i >= dev->ramWidth || j >= dev->ramHeight)
		{
		  continue;
		}
	      dev->ramBuffer[j * dev->ramWidth + i] |= tSlot->bitmap.buffer[q * tSlot->bitmap.width + p];
	    }
	}

      /**
       * update pen position.
       */
      //      pen2.x += slot2->advance.x + 1000;
      tPen.x += tSlot->advance.x + x_space;
      tPen.y += tSlot->advance.y;
    }
  FT_Done_Face (tFace);
  FT_Done_FreeType (tFTLibs);
  return 0;
}
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
zint32_t
zvirtual_ram_calc_coordinate (ZVirtualRAM*dev, zuint32_t vRAMHeight, zuint32_t vRAMWidth)
{
  zint32_t x, y;
  /**
   * check validation of parameters.
   */
  if (dev == NULL || vRAMWidth <= 0 || vRAMHeight <= 0)
    {
      printf ("<VRAM>:invalid parameter!\n");
      return -1;
    }

  /**
   * initial.
   */
  dev->topest = 0;
  dev->bottomest = 0;
  dev->leftest = 0;
  dev->rightest = 0;

  /**
   * find the topest coordinate.
   * scan direction:from top to bottom,from left to right.
   * if there is one pixel data does not equal to zero,
   * then we find the topest.
   */
  for (x = 0; x < vRAMHeight; x++)
    {
      for (y = 0; y < vRAMWidth; y++)
	{
	  if (dev->ramBuffer[x * vRAMWidth + y] != 0)
	    {
	      dev->topest = x;
	      goto out_topest;
	    }
	}
    }
  out_topest: ///<label
#if 0
  printf ("topest=%d\n", dev->topest);
#endif

  /**
   * find the bottomest coordinate.
   * scan direction:from bottom to top,from left to right.
   */
  for (x = vRAMHeight - 1; x >= 0; x--)
    {
      for (y = 0; y < vRAMWidth; y++)
	{
	  if (dev->ramBuffer[x * vRAMWidth + y] != 0)
	    {
	      dev->bottomest = x;
	      goto out_bottomest;
	    }
	}
    }
  out_bottomest: ///<label
#if 0
  printf ("bottomest=%d\n", dev->bottomest);
#endif

  dev->str_height = dev->bottomest - dev->topest + 1;
#if 0
  printf ("str_height=%d\n", dev->str_height);
#endif

  /**
   * find the leftest coordinate.
   * scan direction:from left to right,from top to bottom.
   */
  for (y = 0; y < vRAMWidth; y++)
    {
      for (x = 0; x < vRAMHeight; x++)
	{
	  if (dev->ramBuffer[x * vRAMWidth + y] != 0)
	    {
	      dev->leftest = y;
	      goto out_leftest;
	    }
	}
    }
  out_leftest: ///<label
#if 0
  printf ("leftest=%d\n", dev->leftest);
#endif

  /**
   * find the rightest coordinate.
   * scan direction:from right to left,from top to bottom.
   */
  for (y = vRAMWidth - 1; y >= 0; y--)
    {
      for (x = 0; x < vRAMHeight; x++)
	{
	  if (dev->ramBuffer[x * vRAMWidth + y] != 0)
	    {
	      dev->rightest = y;
	      goto out_rightest;
	    }
	}
    }
  out_rightest: ///<label
#if 0
  printf ("rightest=%d\n", dev->rightest);
#endif

  dev->str_width = dev->rightest - dev->leftest + 1;
#if 0
  printf ("str_width=%d\n", dev->str_width);
#endif

  return 0;
}
/**
 * print screen RAM content.
 */
void
zvirtual_ram_print (ZVirtualRAM*dev)
{
  int i, j;
  if (NULL == dev)
    {
      printf ("invalid parameters!\n");
      return;
    }
  for (i = 0; i < dev->ramHeight; i++)
    {
      for (j = 0; j < dev->ramWidth; j++)
	{
	  if (dev->ramBuffer[i * dev->ramWidth + j])
	    {
	      if (dev->ramBuffer[i * dev->ramWidth + j] < 85)
		{
		  printf ("\033[42m$\033[0m");
		}
	      else if (dev->ramBuffer[i * dev->ramWidth + j] < 128)
		{
		  printf ("\033[41m+\033[0m");
		}
	      else
		{
		  printf ("\033[43m*\033[0m");
		}
	    }
	  else
	    {
	      printf ("#");
	    }
	}
      printf ("\n");
    }
}
