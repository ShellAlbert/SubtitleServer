#ifndef _ZBUFFER_MANAGER_H__
#define _ZBUFFER_MANAGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <zscreen_ram.h>
#include <zdebug.h>
#include <zringbuffer.h>

/**
 * single character pixel matrix size.
 */
//#define PIXEL_MATRIX_SIZE 128 //128x128
//#define PIXEL_MATRIX_SIZE 96 //96x96
//#define PIXEL_MATRIX_SIZE 48 //48x48
#define PIXEL_MATRIX_SIZE 40 //32x32
//#define PIXEL_MATRIX_SIZE 32 //32x32
//#define PIXEL_MATRIX_SIZE 16 //16x16
//#define PIXEL_MATRIX_SIZE 8 //8x8

/**
 * buffer size for freetype pixel matrix.
 * standard square pixel matrix.
 * for example 8*8
 * 
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	$ $ $ $ $ $ $ $
 * 	
 */
#define ZBUFSIZE_FT_PIXEL_MATRIX	(PIXEL_MATRIX_SIZE*PIXEL_MATRIX_SIZE)

/**
 * maximum numbers of one chunk subtiles.
 * in utf-8 encoding,3 bytes stand for one character.
 * in unicode-16(ucs2) encoding,4 bytes stand for one character.
 * 
 * maximum numbers of character of one chunk subtitles.
 */
#define MAX_NUM_CHARS_OF_CHUNK_SUBTITLES	(100)	

/**
 * buffer size for chunk subtitles.
 * usually contains one line characters.
 */
#define BUFSIZE_CHUNK_SUBTITILES	(2*1024*1024-512)

/**
 * thread control blocks.
 */
#define ZFILENAME_XML_LEN	128

enum
{
  RESOURCE_FREE = 0x1987, ///<free now.
  RESOURCE_BUSY = 0x6891, ///< busy now.
};

/**
 * buffer size constant.
 */
#define BUFSIZE_1MB	(1*1024*1024)
#define BUFSIZE_2MB	(2*1024*1024)

/***
 * offset & length of one complete frame.
 */
enum
{
  /**
   * fixed frame head.
   * 128-bit.(16 bytes)
   */
  framehead_offset = 0x0, ///<
  framehead_length = 16, ///<

  /**
   * SpotNumber
   * takes up 2 bytes.
   */
  spotnumber_offset = framehead_offset + framehead_length, ///<
  spotnumber_length = 2, ///<

  /**
   * subtitles left-top x coordinate.(horizontal).
   * takes up 2 bytes.
   */
  left_top_x_coordinate_offset = spotnumber_offset + spotnumber_length, ///<
  left_top_x_coordinate_length = 2, ///<

  /**
   * subtitles left-top y coordinate.(vertical).
   * takes up 2 bytes.
   */
  left_top_y_coordinate_offset = left_top_x_coordinate_offset + left_top_x_coordinate_length, ///<
  left_top_y_coordinate_length = 2, ///<

  /**
   * subtitles pixel matrix width.
   * takes up 2 bytes.
   */
  subtitles_pixel_matrix_width_offset = left_top_y_coordinate_offset + left_top_y_coordinate_length, ///<
  subtitles_pixel_matrix_width_length = 2, ///<

  /**
   * subtitles pixel matrix height
   * takes up 2 bytes.
   */
  subtitles_pixel_matrix_height_offset = subtitles_pixel_matrix_width_offset + subtitles_pixel_matrix_width_length, ///<
  subtitles_pixel_matrix_height_length = 2, ///<

  /**
   * timein, start time.
   * takes up 3 bytes.
   */
  subtitles_time_in_offset = subtitles_pixel_matrix_height_offset + subtitles_pixel_matrix_height_length, ///<
  subtitles_time_in_length = 3, ///<

  /**
   * timeout, stop time.
   * takes up 3 bytes.
   */
  subtitles_time_out_offset = subtitles_time_in_offset + subtitles_time_in_length, ///<
  subtitles_time_out_length = 3, ///<

  /**
   * reserved 4 bytes.
   */
  reserved_offset = subtitles_time_out_offset + subtitles_time_out_length, ///<
  reserved_length = 4, ///<

  /**
   * pixel matrix data length.
   * takes up 4 bytes.
   */
  subtitles_pixel_matrix_data_length_offset = reserved_offset + reserved_length, ///<
  subtitles_pixel_matrix_data_length_length = 4, ///<

  /**
   * pixel matrix data body.
   * (RLE compressed)
   */
  subtitles_pixel_matrix_data_body_offset = subtitles_pixel_matrix_data_length_offset + subtitles_pixel_matrix_data_length_length, ///<
  subtitles_pixel_matrix_data_body_length = 0, ///<
};

/**
 * 16 bytes fixed frame head for sync.
 */
extern const zint8_t fixed_frame_header[];

/**
 * frame structure for FPGA.
 * data will be arranged into this format,
 * and send to FPGA through SPI interface.
 */
typedef struct
{
  zuint8_t *z_framehead; ///<takes up 16 bytes.

  zuint8_t *z_spotnumber; ///< takes up 2 bytes.

  zuint8_t *z_left_top_x_coordinate; ///< takes up 2 bytes.
  zuint8_t *z_left_top_y_coordinate; ///< takes up 2 bytes.

  zuint8_t *z_picture_width; ///< takes up 2 bytes.
  zuint8_t *z_picture_height; ///< takes up 2 bytes.

  zuint8_t *z_timein; ///< takes up 3 bytes.
  zuint8_t *z_timeout; ///< takes up 3 bytes.

  zuint8_t *z_reserved; ///< takes up 4 bytes.
  zuint8_t *z_data_length; ///< takes up 4 bytes.
  zuint8_t *z_data_body;
} zfpga_dev;
/**
 * memory map a plain buffer to FPGA frame structure for easy operation.
 */
extern zint32_t
zmmap_buffer_to_fpga_frame (zuint8_t *pbuffer, zfpga_dev *fpga);

typedef struct
{
  /**
   * subtitles file name
   * (include absolute path).
   */
  zuint8_t xmlfile[128];

  /**
   * resource flag.
   * ready or not ?
   */
  zuint32_t flag_resource;

  /**
   * Video Resolution.
   * These values should be transfered from x86 endian.
   * Will be set by a stand alone receive thread.
   * to communicate with x86.
   */
  zuint32_t widthResolution;
  zuint32_t heightResolution;
  zuint32_t frameRate;

  /**
   * pthread mutex.
   */
  pthread_mutex_t res_mutex;
  pthread_cond_t res_cond;
  /**
   * thread id for receive data.
   */
  pthread_t zTidReceiver;
  /**
   * thread id for parse xml.
   */
  pthread_t zTidParser;
  /**
   * thread id for download pixel matrix to FPGA.
   */
  pthread_t zTidDownloader;

  /**
   * ring buffer between threads.
   */
  ZRingBuffer *ringBufR2P; ///<receiver to parser.
  pthread_mutex_t mutexR2P;
  pthread_cond_t condR2PNotFull;
  pthread_cond_t condR2PNotEmpty;

  ZRingBuffer *ringBufP2D; ///<parser to downloader.
  pthread_mutex_t mutexP2D;
  pthread_cond_t condP2DNotFull;
  pthread_cond_t condP2DNotEmpty;

  /**
   * pthread exit flag?
   */
  zuint8_t exitFlag;

//  /**
//   * screen RAM simulator device.
//   */
//  screen_ram_dev screen_dev;
//
//  /**
//   * buffer original pixel matrix.
//   * pixel format:ARGB
//   */
//  zuint32_t *pixel_buffer;
//  zuint32_t pixel_len;
//
//  /**
//   * buffer for pixel matrix data after RLE32 compressing.
//   */
//  zuint8_t *rle32_buffer;
//  zuint32_t rle32_len;
} ZTHREAD_CBS;

#endif //_ZBUFFER_MANAGER_H__
