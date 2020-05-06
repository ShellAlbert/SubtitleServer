/**
 * @file zthread_parse.h
 * @date August 28,2014.
 * @version 1.0
 * @author shell.albert@gmail.com
 * @brief 
 * 	Thread for parse xml and extract pixel matrix from font library.
 * 	Convert pixel matrix to standard matrix size,
 * 	copy it into ZFIFO.
 */

#ifndef _ZTHREAD_PARSE_H__
#define _ZTHREAD_PARSE_H__
#include <zxml_parse.h>
#include <zdebug.h>
#include <zbuffer_manager.h>
#include <zscreen_simulator.h>
#include <zdevice_info.h>
enum
{
  Subtitle_File_Type_XML = 0x1014, ///<
  Subtitle_File_Type_TTF = 0x1015, ///<
  Subtitle_File_Type_TTC = 0x1016, ///<
  Subtitle_File_Type_PNG = 0x1017, ///<
};

/**
 * finite state machine.
 */
enum
{
  /**
   * the main FSM.
   */
  /**
   * begin.
   */
  FSM_Begin = 0x9900, ///<
  /**
   * xml.
   */
  FSM_Wait_XML_File = 0x9901, ///<
  FSM_Read_XML_File = 0x9902, ///<
  FSM_Parse_XML_File = 0x9903, ///<

  /**
   * ttf.
   */
  FSM_Wait_TTF_File = 0x9904, ///<
  FSM_Load_TTF_File = 0x9905, ///<

  /**
   * create the screen simulator for canvas.
   */
  FSM_Create_Screen_Simulator = 0x9906, ///<
  /**
   * start to overlay element.
   */
  FSM_Overlay_Element = 0x9907, ///<

  /**
   * destroy the screen simulator.
   */
  FSM_Destroy_Screen_Simulator = 0x9908, ///<
  /**
   * FSM end.
   */
  FSM_End = 0x9909, ///<
  /**
   * FSM exit.
   */
  FSM_Exit = 0x990a, ///<

  /**
   * sub-fsm.
   */
  /**
   * overlay element.
   */
  /**
   * png.
   */
  FSM_Wait_PNG_File = 0x9920, ///<
  FSM_Parse_PNG_File = 0x9921, ///<
  FSM_Overlay_Element_Begin = 0x9911, ///<
  FSM_Overlay_Element_Wait = 0x9912, ///<
  FSM_Overlay_Element_Read = 0x9913, ///<
  FSM_Overlay_Element_Parse = 0x9914, ///<
  FSM_Overlay_Element_End = 0x9915, ///<
};

/**
 * @brief thread for parse.
 */
extern void
*
zthread_parser (void *arg);

/**
 * the out fsm.
 * finite state machine functions.
 */
zint32_t
ZParser_FSM_Wait_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo);
zint32_t
ZParser_FSM_Read_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo);
zint32_t
ZParser_FSM_Parse_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo);

/**
 * wait ttf file ok.
 */
zint32_t
ZParser_FSM_Wait_TTF_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo);
/**
 * load ttf file.
 */
zint32_t
ZParser_FSM_Load_TTF_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo);
/**
 * count the number of <Text> and <Image> of each <Subtitle>
 */
zint32_t
ZParser_PreHandle_XmlFile (xmlNode *rootNode, ZXmlInfo *xmlInfo);
/**
 * handle one subtitle section.
 * <Subtitle></Subtitle>
 */
zint32_t
ZParser_Handle_Subtitle (ZTHREAD_CBS *pcbs, ///<
    ZXmlSubtitleElement *subElement, ZXmlFontAttr *fontAttr, ///<
    ZScreenSimulatorDev*ssDev, ///<
    zuint8_t *ttfBuffer, zuint32_t ttfBufferLen);
zint32_t
ZParser_Handle_SubtitleText (ZXmlTextElement *textElement, ZXmlFontAttr *fontAttr, ///<
			     ZScreenSimulatorDev*ssDev, ///<
			     zuint8_t *ttfBuffer, zuint32_t ttfBufferLen);
zint32_t
ZParser_Handle_SubtitleImage (ZTHREAD_CBS *pcbs, ZXmlImageElement *imageElement, ZScreenSimulatorDev*ssDev);

extern zuint8_t ConstSyncFrameHeader[];
#endif //_ZTHREAD_PARSE_H__
/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com.
 */
