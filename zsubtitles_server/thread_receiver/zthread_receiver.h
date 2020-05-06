/*
 * zthread_file_receiver.h
 *
 *  Created on: 2014年10月15日
 *      Author: shell.albert
 */

/**
 * This thread is used as the file receiver,like a server daemon.
 * It receives subtitle file and font library file from x86 SDK client,
 * and then trigger xml parse thread to parse out character pixel data
 * from *.ttc\/\*.ttf file,download them to FPGA to achieve the subtitle
 * parse purpose.
 *
 * Thanks to the TCP connection is reliable,so we don't need to check
 * the transfered data with md5,sha1 or sha256.
 */
#ifndef ZTHREAD_FILE_RECEIVER_H_
#define ZTHREAD_FILE_RECEIVER_H_

#include <zdebug.h>
#include <zbuffer_manager.h>
#include <zrle32.h>

/**
 * spi register define here.
 */
#define ZAddr_Length	(0x80<<8)
#define ZAddr_Data	(0x84<<8)
#define ZAddr_IrqCtl	(0x88<<8)

typedef struct
{
  zuint32_t addr;
  zuint32_t val;
} SPIReg;
extern zint32_t
zint8x4_to_zint32 (const zuint8_t *dataChar8, zuint32_t *dataInt32);
extern zint32_t
zint32_to_zint8x4 (const zuint32_t dataInt32, zuint8_t *dataChar8);
/**
 * @brief thread for receive file from x86.
 */
extern void
*
zthread_receiver (void *arg);

extern void
subtitle_data_async_handler (zint32_t signo);

extern void
receiverParsePlainBufferCombineFrame (void);
#endif /* ZTHREAD_FILE_RECEIVER_H_ */
