/*
 * zthread_download.h
 *
 *  Created on: 2014年10月8日
 *      Author: shell.albert
 */

#ifndef ZTHREAD_DOWNLOAD_H_
#define ZTHREAD_DOWNLOAD_H_

#include <zdebug.h>
#include <zbuffer_manager.h>
#include <zrle32.h>

/**
 * @brief thread for download data to FPGA.
 */
extern void
*
zthread_download (void *arg);

extern zint32_t
zthread_download_SendToZProjectSimulator(zuint8_t *buffer,zuint32_t bufferLen);
#endif /* ZTHREAD_DOWNLOAD_H_ */
