/*
 * zrle32.h
 *
 *  Created on: 2014年10月8日
 *      Author: shell.albert
 */

#ifndef ZRLE32_H_
#define ZRLE32_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zdebug.h>

/**
 * split a 1*32-bit data into 4*8-bit.
 */
extern zint32_t
zint32x1_to_char8x4 (zuint32_t data_int32, zuint8_t *data_char8);
/**
 * combine 4*8bit to a 1*32-bit data.
 */
extern zint32_t
zchar8x4_to_int32x1 (zuint8_t *data_char8, zuint32_t *data_int32);

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
extern zint32_t
zrle32_compress (zuint32_t *src, zuint32_t src_len_byte, zuint8_t *dest, zuint32_t dest_len);

extern zint32_t
zrle32_decompress (zuint8_t *src_data, zuint32_t src_len, zuint8_t *dst_data, zuint32_t dst_len);
#endif /* ZRLE32_H_ */
