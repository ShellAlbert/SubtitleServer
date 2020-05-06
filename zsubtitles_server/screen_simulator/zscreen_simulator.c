/*
 * zscreen_simulator.c
 *
 *  Created on: 2014年12月23日
 *      Author: shell.albert
 */
#include <zscreen_simulator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <freetype/freetype.h>
#include <assert.h>
#include <zthread_parse.h>
#include <sys/time.h>
/**
 * define this macro to ignore compile error!
 *   __pngconf.h__ in libpng already includes setjmp.h;
 *   __dont__ include it again.;
 */
#define PNG_SKIP_SETJMP_CHECK  0x1
#include <png.h>
#include <setjmp.h>

/**
 * dynamic create a screen simulator.
 */
zint32_t
zscreen_simulator_create (ZScreenSimulatorDev **ssDev, zint32_t width, zint32_t height)
{
  if (width <= 0 || height <= 0)
    {
      printf ("<zscreen>:create,invalid parameters!\n");
      return -1;
    }
  do
    {
      /**
       * malloc structure itself.
       */
      (*ssDev) = (ZScreenSimulatorDev*) malloc (sizeof(ZScreenSimulatorDev));
      if (NULL == (*ssDev))
	{
	  printf ("<zscreen>:malloc() failed!\n");
	  break;
	}
      /**
       * malloc internal.
       */
      (*ssDev)->width = width;
      (*ssDev)->height = height;
      (*ssDev)->ramBuffer = (zuint32_t*) malloc (sizeof(zuint32_t) * width * height);
      if (NULL == (*ssDev)->ramBuffer)
	{
	  printf ("<zscreen>:create,failed to malloc memory for ram buffer!\n");
	  break;
	}

      /**
       * malloc memory for store y frames original pixel data.
       * 1MB *4=4MB.
       */
      (*ssDev)->yFrameBuffer = (zuint32_t*) malloc (sizeof(zuint32_t) * 1 * 1024 * 1024);
      if (NULL == (*ssDev)->yFrameBuffer)
	{
	  printf ("<zscreen>:failed to malloc memory for yFrameBuffer!\n");
	  break;
	}
      (*ssDev)->yFrameLength = 0;

      /**
       * malloc memory for store y frames pixel data after RLE-32 compressing.
       */
      (*ssDev)->yFrameRLE32Buffer = (zuint8_t*) malloc (sizeof(zuint8_t) * ConstyFrameRLE32BufferSize);
      if (NULL == (*ssDev)->yFrameRLE32Buffer)
	{
	  printf ("<zscreen>:failed to malloc memory for yFrameRLE32Buffer!\n");
	  break;
	}
      (*ssDev)->yFrameRLE32Length = 0;
      /**
       * success here.
       */
      return 0;
    }
  while (0);
  /**
   * error occured,free memory.
   */
  zscreen_simulator_destroy (ssDev);
  return -1;
}
zint32_t
zscreen_simulator_destroy (ZScreenSimulatorDev **ssDev)
{
  if (NULL != (*ssDev))
    {
      if (NULL != (*ssDev)->ramBuffer)
	{
	  free ((*ssDev)->ramBuffer);
	  (*ssDev)->ramBuffer = NULL;
	}
      if (NULL != (*ssDev)->yFrameBuffer)
	{
	  free ((*ssDev)->yFrameBuffer);
	  (*ssDev)->yFrameBuffer = NULL;
	}
      if (NULL != (*ssDev)->yFrameRLE32Buffer)
	{
	  free ((*ssDev)->yFrameRLE32Buffer);
	  (*ssDev)->yFrameRLE32Buffer = NULL;
	}
      free ((*ssDev));
      (*ssDev) = NULL;
    }
  printf ("<zscreen>:destroy done!\n");
  return 0;
}
zint32_t
zscreen_simulator_draw_pixel (ZScreenSimulatorDev *ssDev, zint32_t x, zint32_t y, zint32_t pixelData)
{
  /**
   * check validation of parameters.
   */
  if (NULL == ssDev)
    {
      printf ("<zscreen>:draw pixel,invalid parameters!\n");
      return -1;
    }
  if (x < 0 || x > (ssDev->width - 1) || y < 0 || y > (ssDev->height - 1))
    {
      printf ("<zscreen>:draw pixel:(x,y) overflow!\n");
      return -1;
    }
  /**
   * update screen pixel (x,y).
   */
  //printf ("(%d)=%x\n", ssDev->width * y + x, pixelData);
  ssDev->ramBuffer[ssDev->width * y + x] = pixelData;
  return 0;
}
/**
 * draw freetype bitmap.
 */
zint32_t
zscreen_simulator_draw_bitmap (ZScreenSimulatorDev *ssDev, FT_Bitmap *bitmap, FT_Int x, FT_Int y)
{
  zint32_t i, j, p, q;
  if (NULL == ssDev || NULL == bitmap)
    {
      printf ("<screen>:draw bitmap,invalid parameters!\n");
      return -1;
    }
  //draw line by line.
  for (i = x, p = 0; i < x + bitmap->width; i++, p++)
    {
      for (j = y, q = 0; j < y + bitmap->rows; j++, q++)
	{
	  if (i < 0 || j < 0 || i >= ssDev->width || j >= ssDev->height)
	    {
	      continue;
	    }
	  /**
	   * draw a pixel.
	   *
	   * row1..........................................
	   * row2..........................................
	   * row3..........................................
	   * row4..........................................
	   */
	  //printf ("(%d,%d)=0x%x\n", i, j, bitmap->buffer[q * bitmap->width + p]);
	  zscreen_simulator_draw_pixel (ssDev, i, j, (zuint32_t) bitmap->buffer[q * bitmap->width + p]);
	}
    }
  return 0;
}
/**
 * draw text on screen simulator.
 * coordinate:(x,y).
 */
zint32_t
zscreen_simulator_draw_text (ZScreenSimulatorDev *ssDev, ///<
    zint32_t textLen, zint32_t *textData, ///<
    ZXmlFontAttr *fontAttr, ///<
    ZXmlTextElement *textElement, ///<
    zuint8_t *ttfBuffer, zuint32_t ttfBufferLen)
{
  zint32_t i, j;
  zint32_t ix, iy;
  zint32_t vram_width, vram_height;
  zint8_t tPixelData8;
  zuint32_t tPixelData32;
  zuint32_t xPhysical, yPhysical;
  /**
   * check validation of parameters.
   */
  if (NULL == ssDev || textLen <= 0 || NULL == textData || NULL == fontAttr || NULL == textElement)
    {
      printf ("<zscreen>:error,draw text,invalid parameters!\n");
      return -1;
    }
  if (NULL == textElement)
    {
      printf ("<zscreen>:draw text,invalid textElement!\n");
      return -1;
    }
  /**
   * create a virtual RAM to draw text on it.
   * the VRAM must be created big enough to hold all pixels.
   * width should be textLen*fontSize at least.
   * height should be fontSize at least.
   * But for some coordinate transformation,
   * we increase the size both width and height by hands to avoid some critical situation happened.
   * width stretch one font size.
   * height stretch one font size.
   */
  vram_width = (textLen + 1) * fontAttr->zSize;
  vram_height = (fontAttr->zSize * 2);
  zvirtual_ram_create (&ssDev->vRam, vram_width, vram_height);
  /**
   * fill font pixel to virtual ram.
   */
  zvirtual_ram_fill (&ssDev->vRam, ZLeanMode_Normal, 0, ///<
		     (zuint32_t*) textData, textLen, fontAttr->zSize, ///<
		     ttfBuffer, ttfBufferLen);
  //printf ("after ram fill:\n");
  //zvirtual_ram_print (&ssDev->vRam);
  /**
   * calculate the valid pixel data coordinate,width and height.
   */
  //printf ("calculate coordinate:\n");
  zvirtual_ram_calc_coordinate (&ssDev->vRam, ssDev->vRam.ramHeight, ssDev->vRam.ramWidth);
  //zvirtual_ram_print (&ssDev->vRam);

  /**
   * So here we get the pixel matrix size width and height.
   * The next we should calculate its physical coordinate on screen simulator.
   */
//  printf ("resolution:%d*%d\n", ssDev->width, ssDev->height);
  //(x,y):x,horizontal coordinate.
  switch (textElement->zHalign)
    {
    case Text_HAlign_Left:
      xPhysical = textElement->zHposition;
      break;
    case Text_HAlign_Center:
      xPhysical = (ssDev->width - ssDev->vRam.str_width) / 2 + textElement->zHposition;
      break;
    case Text_HAlign_Right:
      xPhysical = (ssDev->width - ssDev->vRam.str_width - textElement->zHposition);
      break;
    default:
      /**
       * default HAlign is center.
       */
      xPhysical = (ssDev->width - ssDev->vRam.str_width) / 2 + textElement->zHposition;
      break;
    }

  //(x,y):y,vertical coordinate.
  switch (textElement->zValign)
    {
    case Text_VAlign_Top:
      yPhysical = textElement->zVposition;
      break;
    case Text_VAlign_Center:
      yPhysical = (ssDev->height - ssDev->vRam.str_height) / 2 + textElement->zVposition;
      break;
    case Text_VAlign_Bottom:
      yPhysical = (ssDev->height - ssDev->vRam.str_height - textElement->zVposition);
      break;
    default:
      /**
       * default VAlign is center.
       */
      yPhysical = (ssDev->height - ssDev->vRam.str_height) / 2 + textElement->zVposition;
      break;
    }

#if 0
  printf ("Physical Address:\n");
  printf ("Resolution:%d*%d\n", ssDev->width, ssDev->height);
  printf ("Subtitle:%d*%d\n", ssDev->vRam.str_width, ssDev->vRam.str_height);
  printf ("coordinate:(%d,%d)\n", xPhysical, yPhysical);
#endif
  /**
   * draw valid pixel data on the specified coordinate in screen simulator.
   * scan line by line.
   * from top to bottom,from left to right.
   */

  iy = yPhysical;
  for (i = ssDev->vRam.topest; i <= ssDev->vRam.bottomest; i++)
    {
      ix = xPhysical;
      for (j = ssDev->vRam.leftest; j <= ssDev->vRam.rightest; j++)
	{
//	  if (ssDev->vRam.ramBuffer[i * ssDev->vRam.ramWidth + j])
//	    {
//	      printf ("$");
//	    }
//	  else
//	    {
//	      printf ("_");
//	    }
	  /**
	   * if (x,y) is overflow,ignore it!
	   */
	  if (ix >= ssDev->width || iy >= ssDev->height)
	    {
	      printf ("<zscreen>:warning,(ix,iy) coordinate is overflow! ignore it!\n");
	      continue;
	    }
	  else
	    {
	      //pixel data from freetype.
	      tPixelData8 = ssDev->vRam.ramBuffer[i * ssDev->vRam.ramWidth + j];
	      if (0 != tPixelData8)
		{
		  tPixelData32 = 0;

//		  tPixelData32 |= (zuint32_t) (0xff << 24);  //Alpha.
//		  tPixelData32 |= (zuint32_t) (0xff << 16);  //Red.
//		  tPixelData32 |= (zuint32_t) (0xff << 8);  //Green.
//		  tPixelData32 |= (zuint32_t) (tPixelData8 << 0);  //Blue.
		  //tPixelData32 |= ((zuint32_t) tPixelData8) << 24;  //Alpha.
		  tPixelData32 |= ((zuint32_t) 0xff) << 24;  //Alpha.
		  tPixelData32 |= (zuint32_t) (fontAttr->zColor & 0x00FF0000);  //Red.
		  tPixelData32 |= (zuint32_t) (fontAttr->zColor & 0x0000FF00);  //Green.
		  tPixelData32 |= (zuint32_t) (fontAttr->zColor & 0x000000FF);  //Blue.
		  //printf ("PixelData:0x%x\n", tPixelData32);
		}
	      else
		{
		  //transparent.
		  tPixelData32 = 0x0;
		}
	      /**
	       * here we use 'or' or 'and' operator,
	       * to keep original pixel data to safe.
	       */
	      ssDev->ramBuffer[iy * ssDev->width + ix] |= tPixelData32;
	    }
	  //next column.
	  ix++;
	}
//      printf ("\n");
      //next row.
      iy++;
    }
  zvirtual_ram_destroy (&ssDev->vRam);
  return 0;
#if 0
  FT_Library tLibrary;
  FT_Face tFace;
  FT_Error tError;
  FT_Matrix tMatrix; /* transformation matrix */
  FT_Vector tPen; /* untransformed origin  */
  FT_UInt uiGlyphIndex;
  double angle;
  zint32_t ix, iy;
  zint32_t i;
  if (NULL == ssDev || NULL == textData)
    {
      printf ("<screen>:draw text,null pointer!\n");
      return -1;
    }
  printf ("ucs2 count=%d\n", textLen);
  for (i = 0; i < textLen; i++)
    {
      printf ("0x%x\n", textData[i]);
    }
  tError = FT_Init_FreeType (&tLibrary);
  tError = FT_New_Face (tLibrary, "wqy-microhei.ttc", 0, &tFace);
  tError = FT_Set_Pixel_Sizes (tFace, 40, 0);
  /**
   * prepare matrix.
   */
  angle = (0.0 / 360) * 3.14159 * 2;
  tMatrix.xx = (FT_Fixed) (cos (angle) * 0x10000L);
  tMatrix.xy = (FT_Fixed) (-sin (angle) * 0x10000L);
  tMatrix.yx = (FT_Fixed) (sin (angle) * 0x10000L);
  tMatrix.yy = (FT_Fixed) (cos (angle) * 0x10000L);

  tPen.x = (x) * 64;
  tPen.y = (y) * 64;

  for (i = 0; i < textLen; i++)
    {
      printf ("handling 0x%x\n", textData[i]);
      FT_Set_Transform (tFace, &tMatrix, &tPen);
      /* load glyph image into the slot (erase previous one) */
      uiGlyphIndex = FT_Get_Char_Index (tFace, textData[i]);
      FT_Load_Glyph (tFace, uiGlyphIndex, FT_LOAD_DEFAULT);
      /**
       * 1bit black&white.
       * 8bit colorful.
       */
      FT_Render_Glyph (tFace->glyph, FT_RENDER_MODE_NORMAL);
      if (tError)
	{
	  continue; /* ignore errors */
	}
      ix = tFace->glyph->bitmap_left;
      //iy = ssDev->height - tFace->glyph->bitmap_top;
      iy = ssDev->height - tFace->glyph->bitmap_top - tFace->glyph->bitmap.rows;
      printf ("(ix,iy):(%d,%d)\n", ix, iy);
      zscreen_simulator_draw_bitmap (ssDev, &tFace->glyph->bitmap, ix, iy);
      //update pen position.
      tPen.x += tFace->glyph->advance.x;
      tPen.y += tFace->glyph->advance.y;
    }
  FT_Done_Face (tFace);
  FT_Done_FreeType (tLibrary);
  return 0;
#endif
}

/**
 * load png from memory.
 */
typedef struct
{
  zuint8_t *buffer;
  zuint32_t size;
  zuint32_t offset;
} ZPngDataStream;
void
zload_png_from_memory (png_structp pngPtr, png_bytep data, png_size_t length)
{
  //Here we get our IO pointer back from the read struct.
  //This is the parameter we passed to the png_set_read_fn() function.
  ZPngDataStream *pngDataSource = png_get_io_ptr (pngPtr);
  if (pngDataSource->offset + length <= pngDataSource->size)
    {
      memcpy (data, pngDataSource->buffer + pngDataSource->offset, length);
      pngDataSource->offset += length;
    }
  else
    {
      png_error (pngPtr, "zload_png_from_memory() failed");
    }
  return;
}
/**
 * draw png image on screen simulator.
 */
zint32_t
zscreen_simulator_draw_image (ZTHREAD_CBS *pcbs, ZScreenSimulatorDev *ssDev, ZXmlImageElement *imageElement)
{
  /**
   * for libpng.
   */
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep *prows;
  zint32_t channels;
  zint32_t bpp;
  zint32_t color_type;
  zuint32_t imageWidth, imageHeight;
  zint32_t i, j;
  /**
   * physical coordinate x&y.
   */
  zuint32_t xPhysical, yPhysical;
  zuint32_t ix, iy;
  zuint32_t tPixelData32;

  //for timeout.
  struct timeval tTv;
  struct timespec tTimeout;
  ZBufferElement *element;
  ZBufferElementR2P tBufEleR2P;
  ZPngDataStream gPngDataStream;

  if (NULL == ssDev || NULL == imageElement)
    {
      printf ("<zscreen>:draw image,null pointer!\n");
      return -1;
    }

  /**
   * get PNG file info struct (memory is allocated by libpng)
   * create read struct
   */
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  assert(png_ptr);

  /**
   * get PNG image data info struct (memory is allocated by libpng)
   * create info struct
   */
  info_ptr = png_create_info_struct (png_ptr);
  assert(info_ptr);
  setjmp(png_jmpbuf (png_ptr));

  /**
   * fetch png data from RingBuffer.
   */
  pthread_mutex_lock (&pcbs->mutexR2P);
  //wait until RingBuffer has element.
  while (ZRingBuffer_IsEmpty (pcbs->ringBufR2P))
    {
      gettimeofday (&tTv, NULL);
      tTimeout.tv_sec = tTv.tv_sec + 10;
      tTimeout.tv_nsec = 0;
      if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condR2PNotEmpty, &pcbs->mutexR2P, &tTimeout))
	{
	  printf ("<zscreen>:wait for R2P has png file...\n");
	}
      /**
       * judge thread exit flag in any sleep code.
       */
      if (0x1 == pcbs->exitFlag)
	{
	  pthread_mutex_unlock (&pcbs->mutexR2P);
	  return -1;
	}
    }
  /**
   * get valid element address.
   */
  element = ZRingBuffer_GetAElement (pcbs->ringBufR2P);
  if (NULL == element)
    {
      printf ("<zparser>:failed to get a element from RtoP ring buffer!\n");
      pthread_mutex_unlock (&pcbs->mutexR2P);
      return -1;
    }
  /**
   * check file type again!
   */
  ZMmap_Buffer_Receiver2Parser (element, &tBufEleR2P);
  if (Subtitle_File_Type_PNG == *(tBufEleR2P.fileType))
    {
      if (*(tBufEleR2P.fileDataLen) <= 0)
	{
	  printf ("<zscreen>:error element length!\n");
	  pthread_mutex_unlock (&pcbs->mutexR2P);
	  return -1;
	}
      printf ("<zscreen>:find png File:%s\n", tBufEleR2P.fileName);
//      printf ("press Enter to continue...\n");
//      getchar ();
      gPngDataStream.buffer = tBufEleR2P.fileData;
      gPngDataStream.size = *tBufEleR2P.fileDataLen;
      gPngDataStream.offset = 0;
      /**
       * load png from memory.
       */
      png_set_read_fn (png_ptr, (png_voidp) &gPngDataStream, zload_png_from_memory);
      /**
       * read entire image , ignore alpha channel.
       */
      png_read_png (png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
      prows = png_get_rows (png_ptr, info_ptr);
      /**
       * get parameters from structure.
       */
      channels = png_get_channels (png_ptr, info_ptr);
      imageWidth = info_ptr->width;
      imageHeight = info_ptr->height;
      color_type = info_ptr->color_type;
      bpp = info_ptr->pixel_depth;

#if 1
      printf ("<zscreen>:png size:%d*%d,", imageWidth, imageHeight);
      printf ("<zscreen>:color type:%d\n", color_type);
      printf ("<zscreen>:pixel depth:%d\n", bpp);
      printf ("<zscreen>:channel:%d\n", channels);
#endif

      /**
       * So here we get the pixel matrix size width and height.
       * The next we should calculate its physical coordinate on screen simulator.
       */
      printf ("resolution:%d*%d\n", ssDev->width, ssDev->height);
      //(x,y):x,horizontal coordinate.
      switch (imageElement->zHalign)
	{
	case Image_HAlign_Left:
	  xPhysical = imageElement->zHposition;
	  break;
	case Image_HAlign_Center:
	  xPhysical = (ssDev->width - imageWidth) / 2 + imageElement->zHposition;
	  break;
	case Image_HAlign_Right:
	  xPhysical = (ssDev->width - imageWidth - imageElement->zHposition);
	  break;
	default:
	  /**
	   * default HAlign is center.
	   */
	  xPhysical = (ssDev->width - imageWidth) / 2 + imageElement->zHposition;
	  break;
	}

      //(x,y):y,vertical coordinate.
      switch (imageElement->zValign)
	{
	case Image_VAlign_Top:
	  yPhysical = imageElement->zVposition;
	  break;
	case Image_VAlign_Center:
	  yPhysical = (ssDev->height - imageHeight) / 2 + imageElement->zVposition;
	  break;
	case Image_VAlign_Bottom:
	  yPhysical = (ssDev->height - imageHeight - imageElement->zVposition);
	  break;
	default:
	  /**
	   * default VAlign is center.
	   */
	  yPhysical = (ssDev->height - imageHeight) / 2 + imageElement->zVposition;
	  break;
	}

#if 0
      printf ("Physical Address:\n");
      printf ("Resolution:%d*%d\n", ssDev->width, ssDev->height);
      printf ("Subtitle:%d*%d\n", imageWidth, imageHeight);
      printf ("coordinate:(%d,%d)\n", xPhysical, yPhysical);
#endif
      /**
       * draw valid pixel data on the specified coordinate in screen simulator.
       * scan line by line.
       * from top to bottom,from left to right.
       */
      iy = yPhysical;
      for (i = 0; i < imageHeight; i++)
	{
	  ix = xPhysical;
	  for (j = 0; j < channels * imageWidth; j += channels)
	    {
	      /**
	       * if (x,y) is overflow,ignore it!
	       */
	      if (ix >= ssDev->width || iy >= ssDev->height)
		{
		  printf ("<zscreen>:warning,(ix,iy) coordinate is overflow! ignore it!\n");
		  continue;
		}
	      else
		{
		  /**
		   * convert pixel format to ARGB.
		   */
		  switch (channels)
		    {
		    case 3:
		      tPixelData32 = (zuint32_t) (0xFF) << 24; //Alpha
		      tPixelData32 |= (zuint32_t) prows[i][j + 0] << 16; //Red
		      tPixelData32 |= (zuint32_t) prows[i][j + 1] << 8; //Green
		      tPixelData32 |= (zuint32_t) prows[i][j + 2] << 0; //Blue
		      break;

		    case 4:
		      tPixelData32 = ((zuint32_t) prows[i][j + 3]) << 24; //Alpha
		      tPixelData32 |= ((zuint32_t) prows[i][j + 0]) << 16; //Red
		      tPixelData32 |= ((zuint32_t) prows[i][j + 1]) << 8; //Green
		      tPixelData32 |= ((zuint32_t) prows[i][j + 2]) << 0; //Blue
		      break;

		    default:
		      tPixelData32 = 0x0;
		      break;
		    }
		  /**
		   * here we use 'or' or 'and' operator,
		   * to keep original pixel data to safe.
		   */
		  ssDev->ramBuffer[iy * ssDev->width + ix] |= tPixelData32;
		}
	      //next column.
	      ix++;
	    }
	  //next row.
	  iy++;
	}
      /**
       * resource memory.
       */
      png_destroy_read_struct (&png_ptr, &info_ptr, 0);
    }
  else
    {
      printf ("<zscreen>:fetched element is not PNG! It is 0x%x\n", *(tBufEleR2P.fileType));
      pthread_mutex_unlock (&pcbs->mutexR2P);
      return -1;
    }
  /**
   * element's life is over,remove it from RingBuffer.
   */
  ZRingBuffer_DecTotalNum (pcbs->ringBufR2P);
  /**
   * We get one element from RingBuffer,so it's not full,
   * signal to wake up Receiver thread to fill more elements.
   */
  pthread_cond_signal (&pcbs->condR2PNotFull);
  pthread_mutex_unlock (&pcbs->mutexR2P);
#if 0
  printf ("<zscreen>:remove png element from RingBufferR2P,total is %d\n", ///<
      ZRingBuffer_GetTotalNum (pcbs->ringBufR2P));
#endif
  return 0;

  /**
   * I/O initialization,read png from file.
   */
#if 0
  fp = fopen ((zint8_t*) imageElement->zPath, "rb");
  if (NULL == fp)
    {
      printf ("<zscreen>:draw image,open %s failed!\n", imageElement->zPath);
      return -1;
    }
  png_init_io (png_ptr, fp);
  fclose (fp);
#endif
  return 0;
}
/**
 * print out.
 */
zint32_t
zscreen_simulator_print (ZScreenSimulatorDev *ssDev)
{
  zuint32_t i, j;
  if (NULL == ssDev)
    {
      printf ("<zscreen>:print,invalid parameters!\n");
      return -1;
    }
  printf ("<zscreen>:size(w*h):%d*%d\n", ssDev->width, ssDev->height);
  for (i = 0; i < ssDev->height; i++)
    {
      for (j = 0; j < ssDev->width; j++)
	{
//	  printf ("%d", ssDev->ramBuffer[ssDev->width * i + j]);
	  if (ssDev->ramBuffer[ssDev->width * i + j] != 0)
	    {
	      printf ("\033[44m*\033[0m");
	    }
	  else
	    {
	      printf ("\033[42m-\033[0m");
	    }
	}
      printf ("\n");
    }
  return 0;
}
/**
 * clear screen to zero.
 */
zint32_t
zscreen_simulator_clear (ZScreenSimulatorDev *ssDev)
{
  if (NULL == ssDev)
    {
      printf ("<zscreen>:clear,null pointer!\n");
      return -1;
    }
  memset (ssDev->ramBuffer, 0, ssDev->width * ssDev->height);
  return 0;
}
/**
 * vertical scan valid pixel
 */
void*
zthread_screen_simulator_vertical_scan (void *arg)
{
  zint32_t ix, iy;
  ZScanDev *scanDev = (ZScanDev*) (arg);
  if (NULL == scanDev || NULL == scanDev->ssDev)
    {
      printf ("<zscreen>:vscan,invalid scan device!\n");
      return 0;
    }
  zuint8_t bPrintVerticalScanDetail = 0;
  /**
   * initial.
   */
  scanDev->isValidFrame = -1;
  scanDev->yValidStart = -1;
  scanDev->yValidEnd = -1;
  scanDev->xValidStart = -1;
  scanDev->xValidEnd = -1;
  /**
   * vertical scan from top to bottom to find out the y valid start index.
   */
  for (iy = scanDev->yStart; iy <= scanDev->yEnd; iy++)
    {
      for (ix = 0; ix < scanDev->ssDev->width; ix++)
	{
	  if (0 != scanDev->ssDev->ramBuffer[iy * scanDev->ssDev->width + ix])
	    {
	      scanDev->yValidStart = iy;
	      goto FindYValidStart;
	    }
	}
    }
  FindYValidStart: if (bPrintVerticalScanDetail)
    {
      printf ("yValidStart=%d\n", scanDev->yValidStart);
    }
  /**
   * vertical scan from bottom to top to find out the y valid end index.
   */
  for (iy = scanDev->yEnd; iy >= scanDev->yStart; iy--)
    {
      for (ix = 0; ix < scanDev->ssDev->width; ix++)
	{
	  if (0 != scanDev->ssDev->ramBuffer[iy * scanDev->ssDev->width + ix])
	    {
	      scanDev->yValidEnd = iy;
	      goto FindYValidEnd;
	    }
	}
    }
  FindYValidEnd: if (bPrintVerticalScanDetail)
    {
      printf ("yValidEnd=%d\n", scanDev->yValidEnd);
    }
  /**
   * horizontal scan from left to right to find out the x valid start index.
   */
  for (ix = 0; ix < scanDev->ssDev->width; ix++)
    {
      for (iy = scanDev->yValidStart; iy <= scanDev->yValidEnd; iy++)
	{
	  if (0 != scanDev->ssDev->ramBuffer[iy * scanDev->ssDev->width + ix])
	    {
	      scanDev->xValidStart = ix;
	      goto FindXValidStart;
	    }
	}
    }
  FindXValidStart: if (bPrintVerticalScanDetail)
    {
      printf ("xValidStart=%d\n", scanDev->xValidStart);
    }
  /**
   * horizontal scan from right to left to find out the y valid end index.
   */
  for (ix = (scanDev->ssDev->width - 1); ix >= 0; ix--)
    {
      for (iy = scanDev->yValidStart; iy <= scanDev->yValidEnd; iy++)
	{
	  if (0 != scanDev->ssDev->ramBuffer[iy * scanDev->ssDev->width + ix])
	    {
	      scanDev->xValidEnd = ix;
	      goto FindXValidEnd;
	    }
	}
    }
  FindXValidEnd: if (bPrintVerticalScanDetail)
    {
      printf ("xValidEnd=%d\n", scanDev->xValidEnd);
    }

  /**
   * check validation of x&y coordinate.
   *
   * yValidEnd must >= yValidStart.
   * xValidEnd must >= xValidStart.
   */
  if (scanDev->yValidStart < 0 || scanDev->yValidEnd < 0 || scanDev->xValidStart < 0 || scanDev->xValidEnd < 0)
    {
      if (bPrintVerticalScanDetail)
	{
	  printf ("<zscreen>:error,invalid coordinate index!\n");
	}
      return 0;
    }
  if (scanDev->yValidEnd < scanDev->yValidStart)
    {
      if (bPrintVerticalScanDetail)
	{
	  printf ("<zscreen>:error,yValidEnd < yValidStart!\n");
	}
      return 0;
    }
  if (scanDev->xValidEnd < scanDev->xValidStart)
    {
      if (bPrintVerticalScanDetail)
	{
	  printf ("<zscreen>:error,xValidEnd < xValidStart!\n");
	}
      return 0;
    }
  /**
   * indicate this y frame is valid.
   * the structure transfered will hold the four important paramters:
   * yValidStart,yValidEnd,xValidStart,xValidEnd.
   */
  scanDev->isValidFrame = 0;
  return 0;
}
