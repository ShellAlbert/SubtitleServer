/*
 * zdevice_info.h
 *
 *  Created on: 2014年12月24日
 *      Author: shell.albert
 */

#ifndef ZDEVICE_INFO_H_
#define ZDEVICE_INFO_H_
#include <zdebug.h>
#include <libxml/tree.h>
/**
 * Font attribute.
 */
enum
{
  /**
   * Effect.
   */
  ZXml_Font_Effect_none = 0, ///<
  ZXml_Font_Effect_border, ///<
  ZXml_Font_Effect_shadow, ///<
  /**
   * Italic.
   */
  ZXml_Font_Italic_yes, ///<
  ZXml_Font_Italic_no, ///<

  /**
   * Script.
   */
  ZXml_Font_Script_normal, ///<
  ZXml_Font_Script_super, ///<
  ZXml_Font_Script_sub, ///<

  /**
   * Underlined.
   */
  ZXml_Font_Underlined_yes, ///<
  ZXml_Font_Underlined_no, ///<

  /**
   * Weight.
   */
  ZXml_Font_Weight_bold, ///<
  ZXml_Font_Weight_normal, ///<
};
typedef struct
{
  zuint32_t zId;
  zuint32_t zColor;
  zuint8_t zEffect;
  zuint32_t zEffectColor;
  zuint8_t zItalic;
  zuint8_t zScript;
  zuint8_t zSize;
  zreal32_t zAspectAdjust;
  zuint8_t zUnderlined;
  zuint8_t zWeight;
  zreal32_t zSpacing;
} ZXmlFontAttr;
/**
 * 5K array size.
 */
#define ZXMLTEXTELEMENT_ZTEXT_MAXLEN	(256)
#define ZXMLIMAGEELEMENT_ZPATH_MAXLEN	(256)
#define ZXMLTEXTELEMENT_MAXNUM		(10)
#define ZXMLIMAGEELEMENT_MAXNUM		(10)
typedef struct
{
  zint32_t zDirection;
  zint32_t zValign;
  zint32_t zVposition;
  zint32_t zHalign;
  zint32_t zHposition;
  zuint32_t zTextLen;
  zint8_t zText[ZXMLTEXTELEMENT_ZTEXT_MAXLEN];
} ZXmlTextElement;
typedef struct
{
  zint32_t zValign;
  zint32_t zVposition;
  zint32_t zHalign;
  zint32_t zHposition;
  zuint32_t zPathLen;
  zint8_t zPath[ZXMLIMAGEELEMENT_ZPATH_MAXLEN];
} ZXmlImageElement;
typedef struct
{
  zuint32_t hh;
  zuint32_t mm;
  zuint32_t ss;
  zuint32_t ee;
} ZXmlTime;
typedef struct
{
  zint32_t zSpotNumber;
  ZXmlTime zTimeIn;
  ZXmlTime zTimeOut;
  zuint32_t zFadeUpTime;
  zuint32_t zFadeDownTime;
  zint32_t zTextCounter;
  ZXmlTextElement textElement[ZXMLTEXTELEMENT_MAXNUM];
  zint32_t zImageCounter;
  ZXmlImageElement imageElement[ZXMLIMAGEELEMENT_MAXNUM];
} ZXmlSubtitleElement;
typedef struct
{
  xmlDoc *doc;
  xmlNode *rootNode;
  zint32_t zSubtitleCounter;
  ZXmlSubtitleElement *zSubtitleElement;

  /**
   * the global font attribute.
   */
  ZXmlFontAttr gFontAttr;

  /**
   * ttf file memory buffer.
   */
  zuint8_t *ttfBuffer;
  zuint32_t ttfBufferLen;
} ZXmlInfo;

enum
{
  /**
   * Text Direction.
   */
  Text_Direction_Horizontal = 0, ///<
  Text_Direction_Vertical = 1, ///<

  /**
   * Text HAlign.
   */
  Text_HAlign_Left = 2, ///<
  Text_HAlign_Center = 3, ///<
  Text_HAlign_Right = 4, ///<

  /**
   * Text VAlign.
   */
  Text_VAlign_Top = 5, ///<
  Text_VAlign_Center = 6, ///<
  Text_VAlign_Bottom = 7, ///<

  /**
   * Image HAlign.
   */
  Image_HAlign_Left = 8, ///<
  Image_HAlign_Center = 9, ///<
  Image_HAlign_Right = 10, ///<

  /**
   * Image VAlign.
   */
  Image_VAlign_Top = 11, ///<
  Image_VAlign_Center = 12, ///<
  Image_VAlign_Bottom = 13, ///<
};

#endif /* ZDEVICE_INFO_H_ */
