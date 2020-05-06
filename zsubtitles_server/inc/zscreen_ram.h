/*
 * zscreen_ram.h
 *
 *  Created on: 2014年11月7日
 *      Author: shell.albert
 */

#ifndef ZSCREEN_RAM_H_
#define ZSCREEN_RAM_H_
/**
 * freetyp2 library.
 */
#include <ft2build.h>
#include FT_FREETYPE_H
/**
 * screen ram device simulator.
 */
typedef struct
{
  /**
   * buffer for screen ram.
   * [height][width]
   * height<width!
   */
  char screen_ram[40 * 2][40 * 100];

  /**
   * screen size.
   */
  int screen_width;
  int screen_height;

  /**
   * valid string glyph coordinate.
   */
  int topest;
  int bottomest;
  int leftest;
  int rightest;
  /**
   * valid string size.
   */
  int str_width;
  int str_height;

  /**
   * pixel buffer for one frame.
   */
  unsigned char *frame_buffer;
  unsigned int frame_buffer_len;
  unsigned int frame_width;
  unsigned int frame_height;
} screen_ram_dev;

/**
 * initial screen ram,
 * allocate memory to simulate a screen ram.
 */
extern int
screen_ram_init (screen_ram_dev*dev, int screen_width, int screen_height);
extern int
screen_ram_clear (screen_ram_dev*dev);
/**
 * fill screen RAM with pixel data.
 */
enum lean_mode
{
  lean_mode_normal = 0x0, ///<
  lean_mode_left = 0x1, ///<
  lean_mode_right = 0x2, ///<
};
extern int
screen_ram_fill (screen_ram_dev*dev, ///<
    enum lean_mode mode, unsigned int x_space, ///<
    unsigned int *ucs2_buffer, int buflen);

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
screen_ram_calc_coordinate (screen_ram_dev*dev, ///<
    unsigned int max_screen_height, unsigned int max_screen_width);
/**
 * extract valid matrix pixel to a plain-buffer.
 */
extern int
screen_ram_extract_matrix (screen_ram_dev*dev);

/**
 * print screen RAM content.
 */
void
screen_ram_print (screen_ram_dev*dev);
#endif /* ZSCREEN_RAM_H_ */
