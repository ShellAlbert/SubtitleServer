/*
 * zscreen_ram.c
 *
 *  Created on: 2014年11月7日
 *      Author: shell.albert
 */

#include <stdio.h>
#include <zscreen_ram.h>
#include <math.h>
#include <zbuffer_manager.h>
#include <freetype/freetype.h>
/**
 * screen ram simulator.
 */
int
screen_ram_init (screen_ram_dev*dev, int screen_height, int screen_width)
{
  int ret = 0;
  int i;
  /**
   * check parameter validation.
   */
  if (dev == NULL || screen_width <= 0 || screen_height <= 0)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  /**
   * malloc memory.
   */
  dev->frame_buffer = malloc (sizeof(char) * BUFSIZE_2MB);
  if (dev->frame_buffer == NULL)
    {
      printf ("error:malloc() for frame_buffer failed!\n");
      return -1;
    }

//  dev->screen_ram = malloc (screen_height * sizeof(char*));
//  if (dev->screen_ram == NULL)
//    {
//      printf ("malloc() for screen_ram failed!\n");
//      return -1;
//    }
//  for (i = 0; i < screen_width; i++)
//    {
//      dev->screen_ram[i] = malloc (screen_width * sizeof(char));
//      if (dev->screen_ram[i] == NULL)
//	{
//	  printf ("malloc for screen raw width\n");
//	  return -1;
//	}
//    }
  /**
   * remember size.
   */
  dev->screen_height = screen_height;
  dev->screen_width = screen_width;

  printf ("malloc() screen ram success!\n");
  return 0;
}

int
screen_ram_clear (screen_ram_dev*dev)
{
  if (dev == NULL)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  if (dev->frame_buffer != NULL)
    {
      free (dev->frame_buffer);
      dev->frame_buffer = NULL;
    }
  return 0;
}
/**
 * fill screen RAM with pixel data.
 */
int
screen_ram_fill (screen_ram_dev*dev, enum lean_mode mode, unsigned int x_space, unsigned int *ucs2_buffer, int buflen)
{
  FT_Library library2;
  FT_Face face2;

  FT_GlyphSlot slot2;
  FT_Matrix matrix2; /* transformation matrix */
  FT_Vector pen2; /* untransformed origin  */
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
  if (dev == NULL || ucs2_buffer == NULL || buflen <= 0)
    {
      printf ("error:invalid parameters!\n");
      return -1;
    }

  /**
   * do matrix transfer.
   */
  angle = (0.0 / 360) * 3.14159 * 2;

  error2 = FT_Init_FreeType (&library2);

  error2 = FT_New_Face (library2, "wqy-microhei.ttc", 0, &face2);

  FT_Set_Pixel_Sizes (face2, 40, 0);

  slot2 = face2->glyph;

  switch (mode)
    {
    case lean_mode_normal:
      /**
       * lean normal.
       */
      matrix2.xx = (FT_Fixed) (cos (angle) * 0x10000L);
      matrix2.xy = (FT_Fixed) (-sin (angle) * 0x10000L);
      matrix2.yx = (FT_Fixed) (sin (angle) * 0x10000L);
      matrix2.yy = (FT_Fixed) (cos (angle) * 0x10000L);
      pen2.x = 40 * 10 * 1;
      pen2.y = (dev->screen_height / 2 - 10) * 64;
      break;

      /**
       * right lean.
       */
    case lean_mode_right:
      matrix2.xx = 0x10000L;
      matrix2.xy = 1.0f * 0x10000L;
      matrix2.yx = 0;
      matrix2.yy = 0x10000L;
      pen2.x = 40 * 10 * 1;
      pen2.y = (dev->screen_height / 2 - 10) * 64;
      break;

      /**
       * left lean.
       */
    case lean_mode_left:
      matrix2.xx = 0x10000L;
      matrix2.xy = -1.0f * 0x10000L;
      matrix2.yx = 0;
      matrix2.yy = 0x10000L;
      pen2.x = 40 * 10 * 4;
      pen2.y = (dev->screen_height / 2 - 10) * 64;
      break;
    }

  memset (dev->screen_ram, 0, dev->screen_height * dev->screen_width);

  for (n = 0; n < buflen; n++)
    {
      printf ("freetype now is 0x%x\n", ucs2_buffer[n]);
      FT_Set_Transform (face2, &matrix2, &pen2);

      uiGlyphIndex = FT_Get_Char_Index (face2, ucs2_buffer[n]);
      FT_Load_Glyph (face2, uiGlyphIndex, FT_LOAD_DEFAULT);
      /**
       * 1bit black&white.
       * 8bit colorful.
       */
      FT_Render_Glyph (face2->glyph, FT_RENDER_MODE_NORMAL);

      /**
       * pixel handler.
       */

      x = slot2->bitmap_left;
      y = dev->screen_height - slot2->bitmap_top;
      x_max = x + slot2->bitmap.width;
      y_max = y + slot2->bitmap.rows;

      for (i = x, p = 0; i < x_max; i++, p++)
	{
	  for (j = y, q = 0; j < y_max; j++, q++)
	    {
	      if (i < 0 || j < 0 || i >= dev->screen_width || j >= dev->screen_height)
		{
		  continue;
		}
	      dev->screen_ram[j][i] |= slot2->bitmap.buffer[q * slot2->bitmap.width + p];
	    }
	}

      /**
       * update pen position.
       */
//      pen2.x += slot2->advance.x + 1000;
      pen2.x += slot2->advance.x + x_space;
      pen2.y += slot2->advance.y;
    }

//  screen_ram_print (dev);
//  screen_ram_extract_matrix (&pcbs->screen_dev, pcbs->frame_buffer, BUFSIZE_2MB, &pcbs->frame_len);
  FT_Done_Face (face2);
  FT_Done_FreeType (library2);
  return 0;
}

int
screen_ram_calc_coordinate (screen_ram_dev*dev, ///<
    unsigned int max_screen_height, unsigned int max_screen_width)
{
  int x, y;
  int ram_width;
  int ram_height;
  /**
   * check validation of parameters.
   */
  if (dev == NULL || max_screen_width <= 0 || max_screen_height <= 0)
    {
      printf ("invalid parameter!\n");
      return -1;
    }

  /**
   * initial.
   */
  dev->topest = 0;
  dev->bottomest = 0;
  dev->leftest = 0;
  dev->rightest = 0;

  ram_width = max_screen_width;
  ram_height = max_screen_height;
  /**
   * find the topest coordinate.
   */
  for (x = 0; x < ram_height; x++)
    {
      for (y = 0; y < ram_width; y++)
	{
	  if (dev->screen_ram[x][y] != 0)
	    {
	      dev->topest = x;
	      goto out_topest;
	    }
	}
    }
  out_topest: ///<label
  printf ("topest=%d\n", dev->topest);

  /**
   * find the bottomest coordinate.
   */
  for (x = ram_height - 1; x >= 0; x--)
    {
      for (y = 0; y < ram_width; y++)
	{
	  if (dev->screen_ram[x][y] != 0)
	    {
	      dev->bottomest = x;
	      goto out_bottomest;
	    }
	}
    }
  out_bottomest: ///<label
  printf ("bottomest=%d\n", dev->bottomest);

  dev->str_height = dev->bottomest - dev->topest + 1;
  printf ("str_height=%d\n", dev->str_height);

  /**
   * find the leftest coordinate.
   */
  for (y = 0; y < ram_width; y++)
    {
      for (x = 0; x < ram_height; x++)
	{
	  if (dev->screen_ram[x][y] != 0)
	    {
	      dev->leftest = y;
	      goto out_leftest;
	    }
	}
    }
  out_leftest: ///<label
  printf ("leftest=%d\n", dev->leftest);
  /**
   * find the rightest coordinate.
   */
  for (y = ram_width - 1; y >= 0; y--)
    {
      for (x = 0; x < ram_height; x++)
	{
	  if (dev->screen_ram[x][y] != 0)
	    {
	      dev->rightest = y;
	      goto out_rightest;
	    }
	}
    }
  out_rightest: ///<label
  printf ("rightest=%d\n", dev->rightest);

  dev->str_width = dev->rightest - dev->leftest + 1;
  printf ("str_width=%d\n", dev->str_width);

  return 0;
}
/**
 * extract valid matrix pixel to a plain-buffer.
 */
int
screen_ram_extract_matrix (screen_ram_dev*dev)
{
  int x, y;
  int index;

  /**
   * check validation of parameters!
   */
  if (dev == NULL)
    {
      printf ("invalid parameters!\n");
      return -1;
    }

  dev->frame_height = dev->bottomest - dev->topest + 1;
  dev->frame_width = dev->rightest - dev->leftest + 1;
  dev->frame_buffer_len = dev->frame_height * dev->frame_width;
  printf ("frame size:%d*%d=%d\n", dev->frame_height, dev->frame_width, dev->frame_buffer_len);

  /**
   * extract valid pixel from screen ram to a plain-buffer.
   */
  index = 0;
  for (x = dev->topest; x <= dev->bottomest; x++)
    {
      for (y = dev->leftest; y <= dev->rightest; y++)
	{
	  dev->frame_buffer[index++] = dev->screen_ram[x][y];
	}
    }

  /**
   * print out to check.
   */
#if 1
  for (x = 0; x < dev->frame_height * dev->frame_width; x++)
    {
      if (x % dev->frame_width == 0)
	{
	  printf ("\n");
	}
      if (dev->frame_buffer[x])
	{
	  if (dev->frame_buffer[x] < 128)
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
	  printf ("_");
	}
    }
  printf ("\n");
#endif

  /**
   * write to dist to check out.
   */
#if 0
  FILE *fp;
  int i;
  fp = fopen ("pixel.8bit", "w");
  if (fp != NULL)
    {
      fprintf (fp, "8-bit raw data:%d length\n", dev.frame_buffer_len);
      for (i = 0; i < dev.frame_buffer_len; i++)
	{
	  fprintf (fp, "0x%02x,", *(dev.frame_buffer + i));
	  if ((i + 1) % 10 == 0)
	    {
	      fprintf (fp, "\n");
	    }
	}
      fclose (fp);
    }
#endif

  /**
   * extract finished.
   */
  return 0;
}
/**
 * print screen RAM content.
 */
void
screen_ram_print (screen_ram_dev*dev)
{
  int i, j;
  if (dev == NULL)
    {
      printf ("invalid parameters!\n");
      return;
    }
  for (i = 0; i < dev->screen_height; i++)
    {
      for (j = 0; j < dev->screen_width; j++)
	{
	  if (dev->screen_ram[i][j])
	    {
	      if (dev->screen_ram[i][j] < 85)
		{
		  printf ("\033[42m$\033[0m");
		}
	      else if (dev->screen_ram[i][j] < 128)
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
	      printf ("_");
	    }
	}
      printf ("\n");
    }
}
/**
 * the end of file,tagged by zhangshaoyan.
 */
