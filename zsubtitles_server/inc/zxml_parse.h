/**
 * @file zxml_parse.h
 * @date August 24,2014.
 * @author zhangshaoyan shell.albert@gmail.com
 * @version 1.0
 * @brief parse subtitles XML file.
 * @return
 */
#ifndef ZXML_PARSE_H_
#define ZXML_PARSE_H_
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <zdebug.h>
#include <zbuffer_manager.h>

/**
 * HAlign,horizontal alignment property.
 * valid values are left,right and center.
 *
 * default halign=center.
 */
enum halign_mode
{
  halign_left = 0, ///<
  halign_right = 1, ///<
  halign_center = 2, ///<
};
/**
 * VAlign,vertical alignment property.
 * valid values are top,bottom and center.
 *
 * default valign=center.
 */
enum valign_mode
{
  valign_top = 0, ///<
  valign_bottom = 1, ///<
  valign_center = 2, ///<
};

/**
 * structure for chunk subtitles.
 */
typedef struct
{

  ///////////////////////////////////////////////////
  /**
   * SubtitleID.
   * 128-bit UUID.
   *
   * <SubtitleID>a00ccb11-62d4-46b1-bd05-c14b86bea9d7</SubtitleID>
   *
   * plus 1 for the terminator '0'.
   */
  ///////////////////////////////////////////////////
  enum
  {
    z_subtitleid_buffer_size = 36 + 1, ///<
  } z_subtitle_para;
  zuint8_t z_subtitleid[z_subtitleid_buffer_size];

  ///////////////////////////////////////////////////
  /**
   * MovieTitle.
   *
   * <MovieTitle>Dieyingchongchong3</MovieTitle>
   */
  ///////////////////////////////////////////////////
  enum
  {
    z_movietitle_buffer_size = 256, ///<
  } z_movietitle_para;
  zuint8_t z_movietitle[z_movietitle_buffer_size];

  ///////////////////////////////////////////////////
  /**
   * ReelNumber.
   *
   * <ReelNumber>1</ReelNumber>
   */
  ///////////////////////////////////////////////////
  zuint32_t z_reelnumber;

  ///////////////////////////////////////////////////
  /**
   * Language.
   *
   * <Language>Chinese</Language>
   */
  ///////////////////////////////////////////////////
  enum
  {
    z_language_buffer_size = 48,
  } z_language_para;
  zuint8_t z_language[z_language_buffer_size];

  ///////////////////////////////////////////////////
  /**
   * LoadFont properties.
   *
   * <LoadFont Id="simhei" URI="wqy-microhei.ttc" />
   */
  enum
  {
    z_uri_buffer_size = 128, ///<
    z_id_buffer_size = 128, ///<
  } z_uri_para;
  zuint8_t z_uri[z_uri_buffer_size];
  zuint8_t z_id[z_id_buffer_size];

  ///////////////////////////////////////////////////
  /**
   * Font properties.
   *
   * <Font Id="simhei" Color="9F123456" Weight="normal" Size="36" >
   */
  enum
  {
    z_fontid_buffer_size = 128, ///<
  } z_font_para;
  zuint8_t z_fontid[z_fontid_buffer_size];
  /**
   * foreground color to render the font.
   * color is specified as a 32-bit hexadecimal value in an AARRGGBB format.
   * AA=alpha blend,0xFF is opaque,and 0x00 is transparent.
   * RR=red,GG=green,BB=blue.
   * default color=0xFFFFFFFF(white).
   */
  zuint32_t z_font_color;

  /**
   * special effect used to increase contrast of subtitles on screen.
   * options:none,border,shadow.
   * default effect=shadow.
   */
  enum effect_mode
  {
    effect_none = 0,  ///<
    effect_border = 1,  ///<
    effect_shadow = 2,  ///<
  } z_font_effect;

  /**
   * color to use for the effect chosen.
   * color is specified as a 32-bit hexadecimal value in an AARRGGBB format.
   * AA=alpha blend,0xFF is opaque,and 0x00 is transparent.
   * RR=red,GG=green,BB=blue.
   * default effectcolor=0xFF000000(black).
   */
  zuint32_t z_font_effectcolor;

  /**
   * used to indicate that the text should be rendered in italics.
   * options:yes,no.
   * default italic=no.
   */
  enum italic_mode
  {
    italic_no = 0,  ///<
    italic_yes = 1,  ///<
  } z_font_italic;

  /**
   * options:normal,super,sub.
   * default script=normal.
   */
  enum script_mode
  {
    script_normal = 0, ///<
    script_super = 1, ///<
    script_sub = 2, ///<
  } z_font_script;

  /**
   * used to specify the size of the font.
   * size is given in points.
   *
   * default size = 42.
   */
  zuint32_t z_font_size;

  /**
   * used to adjust the aspect ratio of the font.
   * This attribute can be used to increase or decrease the width
   * of each rendered character.
   *
   * value shall be limited to not less than 0.25 and not more than 4.0
   *
   * default AspectAdjust=1.0
   */
  zreal32_t z_font_aspect_adjust;

  /**
   * used to indicate that the text should be underlined.
   * Note:
   * underlining is only performed for horizontal text direction.
   * options:yes,no.
   * default underlined=no.
   */
  enum underlined_mode
  {
    underlined_no = 0, ///<
    underlined_yes = 1, ///<
  } z_font_underlined;

  /**
   * used to specify the thickness of the font.
   * options:bold,normal.
   * default weight=normal.
   */
  enum weight_mode
  {
    weight_normal = 0, ///<
    weight_bold = 1, ///<
  } z_font_weight;

  /**
   * additional spacing between the rendered characters.
   * the spacing is specified in units of em.
   * 1 em is equivalent to the current font size,
   * and 0.5 em is equivalent to half the current font size.
   */
  zreal32_t z_font_spacing;

  ///////////////////////////////////////////////////
  /**
   * Subtitles properties.
   */
  ///////////////////////////////////////////////////
  /**
   * spot number.
   * SpotNumber="1"
   */
  zuint32_t z_spotnumber;

  /**
   * TimeIn value.
   * TimeIn="00:00:06:225"
   */
  struct _z_timein
  {
    zuint8_t hh; ///<hour.
    zuint8_t mm; ///<minute.
    zuint8_t ss; ///<second.
    zuint8_t ee; ///<editable units.
  } z_timein;

  /**
   * TimeOut value.
   *  TimeOut="00:00:07:105"
   */
  struct _z_timeout
  {
    zuint8_t hh; ///<hour.
    zuint8_t mm; ///<minute.
    zuint8_t ss; ///<second.
    zuint8_t ee; ///<editable units.
  } z_timeout;

  /**
   * FadeUpTime is in the format TTT,
   * where TTT=ticks.
   * a 'tick' is defined as 4 msec and has a range 0 to 249.
   *
   * tick/255*4msc.
   * default fadeuptime=20 (this is 80 msec).
   */
  zuint32_t z_fadeuptime;

  /**
   * FadeDownTime is in the format TTT,
   * where TTT=ticks.
   * a 'tick' is defined as 4 msec and has a range 0 to 249.
   *
   * tick/255*4msc.
   * default fadedowntime=20 (this is 80 msec).
   */
  zuint32_t z_fadedowntime;

  ///////////////////////////////////////////////////
  /**
   * Text properties.
   */
  ///////////////////////////////////////////////////
  /**
   * Direction of the rendered text.
   * valid values are horizontal and vertical.
   *
   * default direction=horizontal.
   */
  enum _z_direction_mode
  {
    direction_horizontal = 0, ///<
    direction_vertical = 1, ///<
  } z_text_direction;

  /**
   * HAlign,horizontal alignment property.
   */
  enum halign_mode z_text_halign;
  /**
   * HPosition,default hposition=0.
   */
  zreal32_t z_text_hposition;
  /**
   * VAlign,vertical alignment property.
   */
  enum valign_mode z_text_valign;
  /**
   * VPosition,default vposition=0.
   */
  zreal32_t z_text_vposition;

////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

  /**
   * text,the real words.
   * usual in utf-8 encoding.
   */
  enum
  {
    z_utf8_buffer_size = 512,
  } z_utf8_para;
  zuint32_t z_utf8_len;
  zuint8_t z_utf8[z_utf8_buffer_size];

  /**
   * convert utf8 encoding of Text into unicode-16,
   * store unicode-16 encoding in here.
   * unicode-16 encode.
   * ucs2:2*8=16.
   */
  enum
  {
    z_ucs2_buffer_size = 128, ///<
  } z_ucs2_para;
  zuint32_t z_ucs2_count; ///<number count of unicode-16.
  zuint32_t z_ucs2[z_ucs2_buffer_size];

  /**
   * width&height for this one line frame.
   */
  zuint32_t width;
  zuint32_t height;

  /**
   * flag will be set if find <Text></Text> section in xml file.
   */
  zuint8_t valid_text;

////////////////////////////////////////////////
  /**
   * <Image> </Image>
   * support png format subtitles.
   * added by zhangshaoyan at November 11,2014.
   */
  enum
  {
    z_pngfile_buffer_size = 128, ///<
  } z_png_para;
  zuint8_t z_png_file[z_pngfile_buffer_size];
  zuint32_t z_png_width;
  zuint32_t z_png_height;
  /**
   * HAlign,horizontal alignment property.
   */
  enum halign_mode z_png_halign;
  /**
   * HPosition,default hposition=0.
   */
  zreal32_t z_png_hposition;
  /**
   * VAlign,vertical alignment property.
   */
  enum valign_mode z_png_valign;
  zreal32_t z_png_vposition;

  /**
   * flag will be set if find <Image></Image> section in xml file.
   */
  zuint8_t valid_image;
} ZCHUNK_SUBTITLES;

/**
 * allocate a chunk subtitles structure for xml parse use.
 */
extern zint32_t
zmalloc_chunk_subtitles (ZCHUNK_SUBTITLES **pzchunk_subtitles);
/**
 * free a chunk subtitles structure for xml.
 */
extern void
zfree_chunk_subtitles (ZCHUNK_SUBTITLES **pzchunk_subtitles);

/**
 * @brief split utf8 string and convert into unicode-16(ucs2),
 * store it into chunk structure.
 * @param utf8_str
 * @param zchunk
 * @return
 */
extern zint32_t
zsplit_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t utf8_len, ///<
		     zuint32_t *ucs2_buffer, zuint32_t ucs2_buf_len, zuint32_t *ucs2_count);
/**
 * @brief convert utf8 to unicode-16(ucs2)
 * @param utf8_str
 * @param ucs2_var
 * @return
 */
extern zint32_t
zutf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t *ucs2_var);

/**
 * @brief get one chunk subtitles like the following format:
 *<Subtitle SpotNumber="1" TimeIn="00:00:06:225" TimeOut="00:00:07:105">
 *<Text VAlign="bottom" VPosition="22">*****</Text>
 *</Subtitle>
 * @return
 */
extern zint32_t
zxml_get_chunk_subtitles (xmlNode * root_node, ///< root element in xml file.
    ZCHUNK_SUBTITLES*pchunk, ///< buffer for store chunk subtitles data.
    ZTHREAD_CBS* pcbs); ///<buffer for fpga pixel.

/**
 * @brief parse <SubtitleID></SubtitleID> section.
 * SubtitleID properties.
 */
extern zint32_t
zxml_parse_subtitleid (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <MovieTitle></MovieTitle> section.
 * MovieTitle properties.
 */
extern zint32_t
zxml_parse_movietitle (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <ReelNumber></ReelNumber> section.
 * ReelNumber properties.
 */
extern zint32_t
zxml_parse_reelnumber (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <Language></Language> section.
 * Language properties.
 */
extern zint32_t
zxml_parse_language (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <LoadFont></LoadFont> section.
 * LoadFont properties.
 * URI
 * Id
 */
extern zint32_t
zxml_parse_loadfont (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <Font></Font> section.
 * Font properties.
 * Id
 * Color
 * Effect
 * EffectColor
 * Italic
 * Script
 * Size
 * AspectAdjust
 * Underlined
 * Weight
 * Spacing
 */
extern zint32_t
zxml_parse_font (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief parse <Subtitle></Subtitle> section.
 * Subtitles properties.
 * SpotNumber
 * TimeIn
 * TimeOut
 * FadeUpTime
 * FadeDownTime
 */
extern zint32_t
zxml_parse_subtitle (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);
/**
 * @brief parse <Text></Text> section.
 * Text properties.
 * Direction
 * HAlign
 * HPosition
 * VAlign
 * VPosition
 */
extern zint32_t
zxml_parse_text (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);
/**
 * @brief parse <Image></Image> section.
 * Image properties.
 * HAlign
 * HPosition
 * VAlign
 * VPosition
 */
extern zint32_t
zxml_parse_image (xmlNode *node, ZCHUNK_SUBTITLES *pchunk);

/**
 * @brief
 * pixel format conversation.
 * from 8-bit to 32-bit.
 */
extern zint32_t
zpixel_format_convert (zuint8_t *src, zuint32_t srclen, ///<
		       zuint32_t color, ///<
		       zuint32_t *dst, zuint32_t dstlen);

/**
 * support for png format subtitles.
 * added by zhangshaoyan at November 11,2014.
 */
extern zint32_t
zpng_read_argb (ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs);

/**
 * put a text frame data into FIFO.
 */
extern zint32_t
zxml_put_text_frame_into_fifo(ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs);
/**
 * put a png frame data into FIFO.
 */
extern zint32_t
zxml_put_png_frame_into_fifo(ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs);
#endif /* ZXML_PARSE_H_ */

/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com.
 */

