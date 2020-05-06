/**
 * @file zdebug.h
 * @date August 24,2014.
 * @author zhangshaoyan shell.albert@gmail.com
 * @version 1.0
 * @brief define differenct level to output different debug messages.
 */
#ifndef _ZDEBUG_H__
#define _ZDEBUG_H__
/**
 * debug level.
 */
#define ZDBG_XML	(0x1<<0)///<xml parse.
#define ZDBG_CHUNK	(0x1<<1)///chunk subtitles.
#define ZDBG_FT2	(0x1<<2)///<freetyp2.
#define ZDBG_RAW_DATA	(0x1<<3)///<pixel raw binary data.
#define ZDBG_RLE32	(0x1<<4)///<Run length code.

//#define ZDBG_LEVEL	(ZDBG_RLE32)

#define ZDBG_LEVEL  (ZDBG_XML|ZDBG_CHUNK|ZDBG_RAW_DATA) ///<only xml code.


#define ZPrintError(fmt,args...)  \
  do{ \
      fprintf(stderr,"error in %s:%d\n",__FILE__,__LINE__); \
      fprintf(stderr,fmt,##args);\
  }while(0)
/////////

/**
 * redefine data types for easy porting.
 */
typedef char zint8_t;
typedef unsigned char zuint8_t;

typedef int zint32_t;
typedef unsigned int zuint32_t;

typedef float zreal32_t;
typedef double zreal64_t;

#endif //_ZDEBUG_H__
/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com
 */
