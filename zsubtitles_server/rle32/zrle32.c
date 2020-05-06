/*
 * zrle32.c
 *
 *  Created on: 2014年10月8日
 *      Author: shell.albert
 */

/**
 * RLE (Run Length Encoding) algorithm.
 * shell.albert@gmail.com
 * zhangshaoyan.
 */
#include <zrle32.h>
//unsigned int test_data[] =
//  { ///<
//    //    0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, 0x12345678, ///<
//    //	0xFFAB, 0xEEFF, ///<
//    //	0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010, ///<
//    //	0xABEF1234, 0xABEF1235, 0xABEF1235, ///<
//    //	0x00000000, 0x00000000, 0x00000000, 0x00000000, ///<
//    //	0x12, 0x34, 0xab, 0xcd, 0x99, 0x99, 0x99, 0x99, 0x99, 0x47, 0x88, 0x88, 0x88, 0x12, 0x80, 0x23, 0x23, 0x23,
//
//    };

/**
 * split a 1*32-bit data into 4*8-bit.
 */
zint32_t
zint32x1_to_char8x4 (zuint32_t data_int32, zuint8_t *data_char8)
{
  /**
   * check valid of parameters.
   */
  if (data_char8 == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }

  data_char8[0] = (zuint32_t) ((data_int32 >> 0) & 0xFF);
  data_char8[1] = (zuint32_t) ((data_int32 >> 8) & 0xFF);
  data_char8[2] = (zuint32_t) ((data_int32 >> 16) & 0xFF);
  data_char8[3] = (zuint32_t) ((data_int32 >> 24) & 0xFF);

  return 0;
}

/**
 * combine 4*8bit to a 1*32-bit data.
 */
zint32_t
zchar8x4_to_int32x1 (zuint8_t *data_char8, zuint32_t *data_int32)
{
  /**
   * check valid of parameters.
   */
  if (data_char8 == NULL || data_int32 == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }

  *data_int32 = (zuint32_t) (data_char8[0] << 0);
  *data_int32 |= (zuint32_t) (data_char8[0] << 8);
  *data_int32 |= (zuint32_t) (data_char8[0] << 16);
  *data_int32 |= (zuint32_t) (data_char8[0] << 24);

  return 0;
}

/**
 * RLE(Run Length Encoding) algorithm in 32-bit format.
 * the format after compress is below:
 * [0] byte :common marker.
 * [1~6] byte: one data section.
 * [1] byte:common marker.
 * [2] byte:repeat time,maximum is 0xFF (255).
 * [3~6]: byte 32-bit original data.
 *
 * Parameters:
 * src: original data in 32-bit format.
 * src_len: original data length in byte.
 * dest: buffer for holding compressed data.
 * dest_len: buffer length in byte.
 *
 * Return value:
 * the length of compressed data in byte.
 *
 * Attention Here!!!
 * dest buffer size should be src size plus 1 at least when the data can not be compressed.
 */
zint32_t
zrle32_compress (zuint32_t *src, zuint32_t src_len_byte, zuint8_t *dest, zuint32_t dest_len)
{
  zuint32_t index;
  zuint32_t rd_pos;    ///<read position for src.
  zuint32_t wr_pos;    ///<write position for dest.
  zuint32_t repeat_times;
  zuint8_t times_part, times_rest;

  /**
   * for marker.
   */
  zuint32_t marker_table[0xFF + 1];    ///<0x00~0xFF
  zuint8_t *pchar;
  zuint8_t index_marker;
  zuint32_t i;
  zuint32_t src_len_int;    ///<byte to int.

  /**
   * check valid of parameters.
   */
  if (src == NULL || src_len_byte <= 0 || dest == NULL || dest_len <= 0)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }

#if 0
  printf ("rle_compress:src len:%d (bytes)\n", src_len_byte);
  printf ("int[0]=0x%x\n", src[0]);
  printf ("int[1]=0x%x\n", src[1]);
  printf ("int[2]=0x%x\n", src[2]);
  printf ("int[3]=0x%x\n", src[3]);
  printf ("sizeof(marker_table)=%d,0xFF=%d\n", sizeof(marker_table), 0xFF);
#endif

  /**
   * find the least common byte as the marker.
   */
  memset (marker_table, 0, sizeof(marker_table));
  pchar = (zuint8_t*) src;

  /**
   * check the original data in byte format.
   */
#if (ZDBG_LEVEL & ZDBG_RLE32)
  for (index = 0; index < src_len_byte; index++)
    {
      printf ("%02x ", pchar[index]);
    }
  printf ("\n");
#endif

  /**
   * counter the times each byte appears..
   */
  for (index = 0; index < src_len_byte; index++)
    {

      marker_table[pchar[index]]++;
//      printf ("pchar[index]=%x,marker_table=%d\n", pchar[index], marker_table[pchar[index]]);
    }

  /**
   * print out count times for each byte from 0x00~0xFF.
   */
#if (ZDBG_LEVEL & ZDBG_RLE32)
  for (index = 0; index <= 0xFF; index++)
    {
      if (marker_table[index])
	{
	  printf ("marker_table[0x%x]=%d\n", index, marker_table[index]);
	}
    }
#endif

  /**
   * find the least counter index,
   * use it as the common marker.
   */
  index_marker = 0;
  for (index = 0; index <= 0xFF; index++)
    {
      if (marker_table[index] < marker_table[index_marker])
	{
	  index_marker = index;
	}
    }

//#if (ZDBG_LEVEL & ZDBG_RLE32)
  printf ("the least common marker is :0x%x, times=%d\n", index_marker, marker_table[index_marker]);
//#endif

  /**
   * run length encoding,compress data begin.
   */
  rd_pos = 0;
  wr_pos = 0;
  repeat_times = 0;
  /**
   * the parameter src_len is in byte format,
   * but we compress data in 32-bit unit,
   * so here convert byte to int.
   */
  src_len_int = src_len_byte / sizeof(zint32_t);
//  printf ("src_len_int=%d/4=%d\n", src_len_byte, src_len_int);
#if 0
  FILE *fp1;
  zuint32_t inde = 0;
  fp1 = fopen ("original.data", "w");
  if (fp1 != NULL)
    {
      fprintf (fp1, "start!\n");
      for (inde = 0; inde < src_len_int; inde++)
	{
	  fprintf (fp1, "[%d]=0x%x\n", inde, src[inde]);
	}
      fprintf (fp1, "ends!\n");
      fclose (fp1);
    }
#endif

  /**
   * use the first byte [0] to hold the common marker.
   * the uncompress function should use the first byte as the common marker
   * to uncompress data.
   */
  dest[wr_pos++] = index_marker;

  /**
   * loop to compress.
   */
  do
    {
      /**
       * counter repeat times.
       */
      while (rd_pos < src_len_int - 1)
	{
	  if (src[rd_pos] == src[rd_pos + 1])
	    {
	      repeat_times++; ///<counter.
	      rd_pos++; ///<shift read position
	    }
	  else
	    {
	      break;
	    }
	}

      /**
       * do not compress when repeat times less than 2 times.
       * keep data original.
       * we count from 0,so here is 2-1.
       */
      if (repeat_times >= (2 - 1))
	{
	  /**
	   * repeat itself.
	   */
	  repeat_times++;
//	  printf ("repeat_times=%d\n", repeat_times);
	  /**
	   * structure for one section.
	   * 8-bt: marker.
	   * 8-bit:repeat times.
	   * 32-bit:repeat data.
	   */
//	  printf ("repeat_timers=%d\n", repeat_times);
	  times_part = repeat_times / 0xFF;
	  times_rest = repeat_times % 0xFF;
//	  printf ("times_part=%d,times_rest=%d(0x%x)\n", times_part, times_rest, times_rest);
	  for (i = 0; i < times_part; i++)
	    {
//	      printf ("times_part_now=%d\n", i);
//	      printf ("[%d]=0x%x\n", wr_pos, index_marker);
	      dest[wr_pos++] = index_marker;
//	      printf ("[%d]=0x%x\n", wr_pos, 0xFF);
	      //dest[wr_pos++] = 0xFF;//old code zhangshaoyan@20150106.
	      dest[wr_pos++] = 0xFF - 1;	      //new code.
	      zint32x1_to_char8x4 (src[rd_pos], &dest[wr_pos]);
	      wr_pos += sizeof(zuint8_t) * 4;
	    }
	  if (times_rest > 0)
	    {
//	      printf ("marker=%d(0x%x),times_rest=%d(0x%x)\n", index_marker, times_rest, times_rest);
//	      printf ("[%d]=0x%x\n", wr_pos, index_marker);
	      dest[wr_pos++] = index_marker;
//	      printf ("[%d]=0x%x\n", wr_pos, times_rest);
	      //dest[wr_pos++] = times_rest;//old code zhangshaoyan@20150106.
	      dest[wr_pos++] = times_rest - 1;	      //new code.
	      zint32x1_to_char8x4 (src[rd_pos], &dest[wr_pos]);
//	      printf ("rest:%x,%x,%x,%x\n", dest[wr_pos + 0], dest[wr_pos + 1], dest[wr_pos + 2], dest[wr_pos + 3]);
	      wr_pos += sizeof(zuint8_t) * 4;
	    }
	}
      else
	{
	  /**
	   * keep original data.
	   */
//	  printf ("repeat_times=%d\n", repeat_times);
	  zint32x1_to_char8x4 (src[rd_pos], &dest[wr_pos]);
	  wr_pos += sizeof(zuint8_t) * 4;
	}

      /**
       * move read position to next section.
       * reset for next counter.
       */
      if (rd_pos >= src_len_int - 1)
	{
	  break;
	}
      else
	{
	  rd_pos++;
	  repeat_times = 0;
	}
    }
  while (1);

  /**
   * compress finish.
   */
#if (ZDBG_LEVEL & ZDBG_RLE32)
  printf ("compress data after RLE32:wr_pos=%d\n", wr_pos);
#endif
  return (wr_pos);
}

zint32_t
zrle32_decompress (zuint8_t *src_data, zuint32_t src_len, zuint8_t *dst_data, zuint32_t dst_len)
{
  zuint8_t marker;
  zint32_t i, j;
  zuint32_t len_decompress = 0;
  zuint32_t repeat_times = 0;
  /**
   * check validation of parameters.
   */
  if (src_data == NULL || src_len <= 0 || dst_data == NULL || dst_len <= 0)
    {
      printf ("error:%s:%d:wrong parameters!\n", __FILE__, __LINE__);
      return -1;
    }

  /**
   * src_data[0]:common marker.
   */
  marker = src_data[0];
#if (ZDBG_LEVEL & ZDBG_RLE32)
  printf ("common marker=0x%x\n", marker);
#endif

  for (i = 1; i < src_len;)
    {
      if (src_data[i] == marker)
	{
	  /**
	   * compressed data.
	   */
	  //repeat_times = src_data[i + 1];//old code.
	  repeat_times = src_data[i + 1] + 1;//new code.
	  for (j = 0; j < repeat_times; j++)
	    {
	      dst_data[len_decompress++] = src_data[i + 2];
	      dst_data[len_decompress++] = src_data[i + 3];
	      dst_data[len_decompress++] = src_data[i + 4];
	      dst_data[len_decompress++] = src_data[i + 5];
	    }
	  /**
	   * move to next section.
	   * one section format is:
	   * 1 byte common marker.
	   * 1 byte repeat times(0xFF maximum).
	   * 4 byte data.
	   * so here is 6.
	   */
	  i += sizeof(zint8_t) * 6;
	}
      else
	{
	  /**
	   * not compressed data.
	   * copy to dest directly.
	   * move length pointer.
	   */
	  dst_data[len_decompress++] = src_data[i];
	  dst_data[len_decompress++] = src_data[i + 1];
	  dst_data[len_decompress++] = src_data[i + 2];
	  dst_data[len_decompress++] = src_data[i + 3];
	  /**
	   * move to next section.
	   * bypass this uncompressed section.
	   */
	  i += sizeof(zint8_t) * 4;
	}
    }

#if (ZDBG_LEVEL & ZDBG_RLE32)
  printf ("after uncompress,len=%d\n", len_decompress);
  for (i = 0; i < len_decompress; i++)
    {
      printf ("0x%x ", dst_data[i]);
    }
  printf ("\n");
#endif
  return len_decompress;
}
/**
 * the end of file,tagged by zhangshaoyan.
 */

