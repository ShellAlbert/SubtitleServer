/*
 * zspi_device.h
 *
 *  Created on: 2014年11月5日
 *      Author: shell.albert
 */

#ifndef ZSPI_DEVICE_H_
#define ZSPI_DEVICE_H_
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/times.h>
#include <stdlib.h>
const char *spi_device_file = "/dev/imx5_ecspi2";
typedef struct
{
  unsigned int addr;
  unsigned int val;
} SPI_REG;
#endif /* ZSPI_DEVICE_H_ */
