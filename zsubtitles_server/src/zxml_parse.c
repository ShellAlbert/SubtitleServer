/**
 * @file zxml_parse.c
 * @date August 24,2014.
 * @author zhangshaoyan shell.albert@gmail.com
 * @version 1.0
 * @brief parse subtitles XML file.
 * @return
 */
#include <zxml_parse.h>
#include <assert.h>
/**
 * define this macro to ignore compile error!
 *   __pngconf.h__ in libpng already includes setjmp.h;
 *   __dont__ include it again.;
 */
#define PNG_SKIP_SETJMP_CHECK  0x1
#include <png.h>
#include <setjmp.h>

enum
{
  flag_find_it_no = 0x0, ///<
  flag_find_it_yes = 0x1, ///<
};

/**
 * allocate a chunk subtitles structure for xml parse use.
 */
zint32_t
zmalloc_chunk_subtitles (ZCHUNK_SUBTITLES **pzchunk_subtitles)
{
  do
    {
      (*pzchunk_subtitles) = (ZCHUNK_SUBTITLES*) malloc (sizeof(ZCHUNK_SUBTITLES));
      if ((*pzchunk_subtitles) == NULL)
	{
	  ZPrintError("malloc() failed for zchunk_subtitles!\n");
	  break;
	}

      /**
       * initial internal variables.
       */
      (*pzchunk_subtitles)->valid_image = 0x0;
      (*pzchunk_subtitles)->valid_text = 0x0;
      /**
       * success here.
       */
      return 0;
    }
  while (0);
  /**
   * some error occur when it reaches here.
   */
  if (*pzchunk_subtitles != NULL)
    {
      free (*pzchunk_subtitles);
      *pzchunk_subtitles = NULL;
    }
  return -1;
}

/**
 * free a chunk subtitles structure for xml.
 */
void
zfree_chunk_subtitles (ZCHUNK_SUBTITLES **pzchunk_subtitles)
{
  if ((*pzchunk_subtitles) != NULL)
    {
      free ((*pzchunk_subtitles));
      (*pzchunk_subtitles) = NULL;
    }
  return;
}

/**
 * @brief get one chunk subtitles like the following format:
 *<Subtitle SpotNumber="1" TimeIn="00:00:06:225" TimeOut="00:00:07:105">
 *<Text VAlign="bottom" VPosition="22">*****</Text>
 *</Subtitle>
 * @return
 */
zint32_t
zxml_get_chunk_subtitles (xmlNode * root_node, ///<root element in xml file.
    ZCHUNK_SUBTITLES*pchunk, ///< buffer for store chunk subtitles data.
    ZTHREAD_CBS* pcbs) ///<buffer for fpga pixel.
{
  xmlNode *node;

  /**
   * iterator all nodes.
   */
  for (node = root_node; node != NULL; node = node->next)
    {
      /**
       * SubtitleID properties.
       */
      if (!strcmp ((zint8_t*) node->name, "SubtitleID"))
	{
	  if (zxml_parse_subtitleid (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * MovieTitle properties.
       */
      if (!strcmp ((zint8_t*) node->name, "MovieTitle"))
	{
	  if (zxml_parse_movietitle (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * ReelNumber properties.
       */
      if (!strcmp ((zint8_t*) node->name, "ReelNumber"))
	{
	  if (zxml_parse_reelnumber (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * Language properties.
       */
      if (!strcmp ((zint8_t*) node->name, "Language"))
	{
	  if (zxml_parse_language (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * LoadFont properties.
       */
      if (!strcmp ((zint8_t*) node->name, "LoadFont"))
	{
	  if (zxml_parse_loadfont (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * Font properties.
       */
      if (!strcmp ((zint8_t*) node->name, "Font"))
	{
	  if (zxml_parse_font (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * Subtitle properties.
       */
      if (!strcmp ((zint8_t*) node->name, "Subtitle"))
	{
	  if (zxml_parse_subtitle (node, pchunk) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * Text properties.
       */
      if (!strcmp ((zint8_t*) node->name, "Text"))
	{
	  if (zxml_parse_text (node, pchunk) < 0)
	    {
	      return -1;
	    }
	  if (zxml_put_text_frame_into_fifo (pchunk, pcbs) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">
       * CountDownSub_ch_LFE.png
       * </Image>
       *
       * If we find png format subtitles,use it first.
       */
      if (!strcmp ((zint8_t*) node->name, "Image"))
	{
	  if (zxml_parse_image (node, pchunk) < 0)
	    {
	      return -1;
	    }
	  if (zxml_put_png_frame_into_fifo (pchunk, pcbs) < 0)
	    {
	      return -1;
	    }
	}

      /**
       * recursive call this function to get all children.
       * @return
       */
      zxml_get_chunk_subtitles (node->children, pchunk, pcbs);
    }
  return 0;
}
/**
 * @brief parse <SubtitleID></SubtitleID> section.
 * SubtitleID properties.
 */
zint32_t
zxml_parse_subtitleid (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint32_t len;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * <SubtitleID>content</SubtitleID>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      len = strlen ((zint8_t*) pprop);
      if (len > z_subtitleid_buffer_size - 1)
	{
	  ZPrintError("no enough space!\n");
	  return -1;
	}
      memset (pchunk->z_subtitleid, 0, z_subtitleid_buffer_size);
      strncpy ((zint8_t*) pchunk->z_subtitleid, (zint8_t*) pprop, len);
      printf ("subtitleid=%s\n", pchunk->z_subtitleid);
      xmlFree (pprop);
    }
  return 0;
}
/**
 * @brief parse <MovieTitle></MovieTitle> section.
 * MovieTitle properties.
 */
zint32_t
zxml_parse_movietitle (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint32_t len;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * <MovieTitle>content</MovieTitle>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      len = strlen ((zint8_t*) pprop);
      if (len > z_movietitle_buffer_size)
	{
	  ZPrintError("no enough space!\n");
	  return -1;
	}
      memset (pchunk->z_movietitle, 0, z_movietitle_buffer_size);
      strncpy ((zint8_t*) pchunk->z_movietitle, (zint8_t*) pprop, len);
      printf ("movietitle=%s\n", pchunk->z_movietitle);
      xmlFree (pprop);
    }
  return 0;
}
/**
 * @brief parse <ReelNumber></ReelNumber> section.
 * ReelNumber properties.
 */
zint32_t
zxml_parse_reelnumber (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }

  /**
   * <ReelNumber>content</ReelNumber>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      pchunk->z_reelnumber = atoi ((zint8_t*) pprop);
      printf ("z_reelnumber=%d\n", pchunk->z_reelnumber);
      xmlFree (pprop);
    }
  return 0;
}
/**
 * @brief parse <Language></Language> section.
 * Language properties.
 */
zint32_t
zxml_parse_language (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint32_t len;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * <Language>content</Language>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      len = strlen ((zint8_t*) pprop);
      if (len > z_language_buffer_size)
	{
	  ZPrintError("no enough space!\n");
	  return -1;
	}
      memset (pchunk->z_language, 0, z_language_buffer_size);
      strncpy ((zint8_t*) pchunk->z_language, (zint8_t*) pprop, len);
      xmlFree (pprop);
    }
  return 0;
}
/**
 * @brief parse <LoadFont></LoadFont> section.
 * LoadFont properties.
 * URI
 * Id
 */
zint32_t
zxml_parse_loadfont (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint32_t len;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * URI.
   */
  pprop = xmlGetProp (node, (xmlChar*) "URI");
  if (pprop != NULL)
    {
      len = strlen ((zint8_t*) pprop);
      if (len > z_uri_buffer_size)
	{
	  ZPrintError("no enough space!\n");
	  return -1;
	}
      memset (pchunk->z_uri, 0, z_uri_buffer_size);
      strncpy ((zint8_t*) pchunk->z_uri, (zint8_t*) pprop, len);
      xmlFree (pprop);
    }
  /**
   * Id.
   */
  return 0;
}

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
zint32_t
zxml_parse_font (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint8_t flag_find_it;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * Id.
   */
  /**
   * Color.
   */
  pprop = xmlGetProp (node, (xmlChar*) "Color");
  if (pprop != NULL)
    {
      /**
       * format input in hex format.
       */
      sscanf ((zint8_t*) pprop, "%x", &pchunk->z_font_color);
    }
  else
    {
      pchunk->z_font_color = 0xFFFFFFFF;
      printf ("warning,not find \"Color\" attribute,use 0x%x as default!\n", pchunk->z_font_color);
    }

  /**
   * Effect.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Effect");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "none"))
	{
	  pchunk->z_font_effect = effect_none;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "border"))
	{
	  pchunk->z_font_effect = effect_border;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "shadow"))
	{
	  pchunk->z_font_effect = effect_shadow;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is shadow.
       */
      pchunk->z_font_effect = effect_shadow;
      printf ("warning,not find \"Effect\" attribute,use shadow as default!\n");
    }

  /**
   * EffectColor.
   */
  pprop = xmlGetProp (node, (xmlChar*) "EffectColor");
  if (pprop != NULL)
    {
      /**
       * format input in hex format.
       */
      sscanf ((zint8_t*) pprop, "%x", &pchunk->z_font_effectcolor);
    }
  else
    {
      pchunk->z_font_effectcolor = 0xFFFFFFFF;
      printf ("warning,not find \"EffectColor\" attribute,use 0x%x as default!\n", pchunk->z_font_effectcolor);
    }

  /**
   * Italic.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Italic");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "yes"))
	{
	  pchunk->z_font_italic = italic_yes;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "no"))
	{
	  pchunk->z_font_italic = italic_no;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is no.
       */
      pchunk->z_font_italic = italic_no;
      printf ("warning,not find \"Italic\" attribute,use no as default!\n");
    }

  /**
   * Script.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Script");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "normal"))
	{
	  pchunk->z_font_script = script_normal;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "super"))
	{
	  pchunk->z_font_script = script_super;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "sub"))
	{
	  pchunk->z_font_script = script_sub;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is normal.
       */
      pchunk->z_font_script = script_normal;
      printf ("warning,not find \"Script\" attribute,use normal as default!\n");
    }

  /**
   * Size.
   */
  pprop = xmlGetProp (node, (xmlChar*) "Size");
  if (pprop != NULL)
    {
      /**
       * ascii to integer.
       */
      pchunk->z_font_size = atoi ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_font_size = 42;
      printf ("warning,not find \"Size\" attribute,use %d as default!\n", pchunk->z_font_size);
    }

  /**
   * AspectAdjust.
   */
  pprop = xmlGetProp (node, (xmlChar*) "AspectAdjust");
  if (pprop != NULL)
    {
      /**
       * ascii to integer.
       */
      pchunk->z_font_aspect_adjust = atof ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_font_aspect_adjust = 1.0f;
      printf ("warning,not find \"AspectAdjust\" attribute,use %f as default!\n", pchunk->z_font_aspect_adjust);
    }

  /**
   * Underlined.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Underlined");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "yes"))
	{
	  pchunk->z_font_underlined = underlined_yes;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "no"))
	{
	  pchunk->z_font_underlined = underlined_no;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is no.
       */
      pchunk->z_font_underlined = underlined_no;
      printf ("warning,not find \"Underlined\" attribute,use no as default!\n");
    }

  /**
   * Weight.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Weight");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "bold"))
	{
	  pchunk->z_font_weight = weight_bold;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "normal"))
	{
	  pchunk->z_font_weight = weight_normal;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is normal.
       */
      pchunk->z_font_weight = weight_normal;
      printf ("warning,not find \"Weight\" attribute,use normal as default!\n");
    }

  /**
   * Spacing.
   */
  pprop = xmlGetProp (node, (xmlChar*) "Spacing");
  if (pprop != NULL)
    {
      /**
       * ascii to float.
       */
      pchunk->z_font_spacing = atof ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_font_spacing = 0.0f;
      printf ("warning,not find \"Spacing\" attribute,use %f as default!\n", pchunk->z_font_spacing);
    }
  return 0;
}
/**
 * @brief parse <Subtitle></Subtitle> section.
 * Subtitles properties.
 * SpotNumber
 * TimeIn
 * TimeOut
 * FadeUpTime
 * FadeDownTime
 */
zint32_t
zxml_parse_subtitle (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zuint8_t buffer[32];
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }

  /**
   * SpotNumber="1"
   */
  pprop = xmlGetProp (node, (xmlChar*) "SpotNumber");
  if (pprop != NULL)
    {
      /**
       * ascii to integer.
       */
      pchunk->z_spotnumber = atoi ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_spotnumber = 0;
      printf ("warning,not find \"SpotNumber\" attribute,use %d as default!\n", pchunk->z_spotnumber);
    }

  /**
   * TimeIn="00:00:06:225"
   */
  pprop = xmlGetProp (node, (xmlChar*) "TimeIn");
  if (pprop != NULL)
    {
      /**
       * hh
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop, sizeof(zint8_t) * 2);
      pchunk->z_timein.hh = atoi ((zint8_t*) buffer);
      /**
       * mm
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 3, sizeof(zint8_t) * 2);
      pchunk->z_timein.mm = atoi ((zint8_t*) buffer);
      /**
       * ss
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 6, sizeof(zint8_t) * 2);
      pchunk->z_timein.ss = atoi ((zint8_t*) buffer);
      /**
       * ee
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 9, sizeof(zint8_t) * 3);
      pchunk->z_timein.ee = atoi ((zint8_t*) buffer);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_timein.hh = 0;
      pchunk->z_timein.mm = 0;
      pchunk->z_timein.ss = 0;
      pchunk->z_timein.ee = 0;
      printf ("warning,not find \"TimeIn\" attribute,use %d:%d:%d:%d as default!\n", ///<
	  pchunk->z_timein.hh, pchunk->z_timein.mm, pchunk->z_timein.ss, pchunk->z_timein.ee);
    }

  /**
   * TimeOut="00:00:07:105"
   */
  pprop = xmlGetProp (node, (xmlChar*) "TimeOut");
  if (pprop != NULL)
    {
      /**
       * hh.
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop, sizeof(zint8_t) * 2);
      pchunk->z_timeout.hh = atoi ((zint8_t*) buffer);
      /**
       * mm.
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 3, sizeof(zint8_t) * 2);
      pchunk->z_timeout.mm = atoi ((zint8_t*) buffer);
      /**
       * ss.
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 6, sizeof(zint8_t) * 2);
      pchunk->z_timeout.ss = atoi ((zint8_t*) buffer);
      /**
       * ee.
       */
      memset (buffer, 0, sizeof(buffer));
      memcpy (buffer, pprop + sizeof(zint8_t) * 9, sizeof(zint8_t) * 3);
      pchunk->z_timeout.ee = atoi ((zint8_t*) buffer);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_timeout.hh = 0;
      pchunk->z_timeout.mm = 0;
      pchunk->z_timeout.ss = 0;
      pchunk->z_timeout.ee = 0;
      printf ("warning,not find \"TimeOut\" attribute,use %d:%d:%d:%d as default!\n", ///<
	  pchunk->z_timein.hh, pchunk->z_timein.mm, pchunk->z_timein.ss, pchunk->z_timein.ee);
    }

  /**
   * FadeUpTime.
   */
  pprop = xmlGetProp (node, (xmlChar*) "FadeUpTime");
  if (pprop != NULL)
    {
      pchunk->z_fadeuptime = atoi ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_fadeuptime = 20;
      printf ("warning,not find \"FadeUpTime\" attribute,use %d as default!\n", pchunk->z_fadeuptime);
    }

  /**
   * FadeDownTime.
   */
  pprop = xmlGetProp (node, (xmlChar*) "FadeDownTime");
  if (pprop != NULL)
    {
      pchunk->z_fadedowntime = atoi ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_fadedowntime = 20;
      printf ("warning,not find \"FadeDownTime\" attribute,use %d as default!\n", pchunk->z_fadedowntime);
    }

  return 0;
}

/**
 * @brief parse <Text></Text> section.
 * Text properties.
 * Direction
 * HAlign
 * HPosition
 * VAlign
 * VPosition
 */
zint32_t
zxml_parse_text (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint8_t flag_find_it;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * Direction.
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "Direction");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "horizontal"))
	{
	  pchunk->z_text_direction = direction_horizontal;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "vertical"))
	{
	  pchunk->z_text_direction = direction_vertical;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is horizontal.
       */
      pchunk->z_text_direction = direction_horizontal;
      printf ("warning,not find \"Weight\" attribute,use horizontal as default!\n");
    }
  /**
   * HAlign="bottom"
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "HAlign");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "left"))
	{
	  pchunk->z_text_halign = halign_left;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "center"))
	{
	  pchunk->z_text_halign = halign_center;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "right"))
	{
	  pchunk->z_text_halign = halign_right;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
      xmlFree (pprop);
    }

  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is center.
       */
      pchunk->z_text_halign = halign_center;
      printf ("warning,not find \"HAlign\" attribute,use center as default!\n");
    }

  /**
   * HPosition="22"
   */
  pprop = xmlGetProp (node, (xmlChar*) "HPosition");
  if (pprop != NULL)
    {
      /**
       * ascii to float.
       */
      pchunk->z_text_hposition = atof ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_text_hposition = 0;
      printf ("warning,not find \"HAlign\" attribute,use %f as default!\n", pchunk->z_text_hposition);
    }

  /**
   * VAlign="bottom"
   */
  flag_find_it = flag_find_it_no;
  pprop = xmlGetProp (node, (xmlChar*) "VAlign");
  if (pprop != NULL)
    {
      if (!strcmp ((zint8_t*) pprop, "top"))
	{
	  pchunk->z_text_valign = valign_top;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "center"))
	{
	  pchunk->z_text_valign = valign_center;
	  flag_find_it = flag_find_it_yes;
	}
      else if (!strcmp ((zint8_t*) pprop, "bottom"))
	{
	  pchunk->z_text_valign = valign_bottom;
	  flag_find_it = flag_find_it_yes;
	}
      else
	{
	  flag_find_it = flag_find_it_no;
	}
      xmlFree (pprop);
    }
  if (flag_find_it_no == flag_find_it)
    {
      /**
       * default is center.
       */
      pchunk->z_text_valign = valign_center;
      printf ("warning,not find \"VAlign\" attribute,use center as default!\n");
    }

  /**
   * VPosition="22"
   */
  pprop = xmlGetProp (node, (xmlChar*) "VPosition");
  if (pprop != NULL)
    {
      /**
       * ascii to float.
       */
      pchunk->z_text_vposition = atof ((zint8_t*) pprop);
      xmlFree (pprop);
    }
  else
    {
      pchunk->z_text_vposition = 0;
      printf ("warning,not find \"VPosition\" attribute,use %f as default!\n", pchunk->z_text_vposition);
    }

  /**
   * <Text>content</Text>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      pchunk->z_utf8_len = strlen ((zint8_t*) pprop);
      if (pchunk->z_utf8_len > z_utf8_buffer_size)
	{
	  ZPrintError("no enough buffer space!\n");
	  return -1;
	}
      memset (pchunk->z_utf8, 0, z_utf8_buffer_size);
      strncpy ((zint8_t*) pchunk->z_utf8, (zint8_t*) pprop, pchunk->z_utf8_len);
      xmlFree (pprop);
      /**
       * set valid flag.
       */
      pchunk->valid_text = 0x1;
    }
  return 0;
}

/**
 * @brief parse <Image></Image> section.
 * Image properties.
 * HAlign
 * HPosition
 * VAlign
 * VPosition
 */
zint32_t
zxml_parse_image (xmlNode *node, ZCHUNK_SUBTITLES *pchunk)
{
  xmlChar *pprop;
  zint32_t len;
  if (node == NULL || pchunk == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * <Image>content</Image>
   */
  pprop = xmlNodeGetContent (node);
  if (pprop != NULL)
    {
      printf ("content:%s (png file name)\n", pprop);
      len = strlen ((zint8_t*) pprop);
      if (len > z_pngfile_buffer_size)
	{
	  ZPrintError("no enough buffer space!\n");
	  return -1;
	}
      memset (pchunk->z_png_file, 0, z_pngfile_buffer_size);
      strncpy ((zint8_t*) pchunk->z_png_file, (zint8_t*) pprop, len);
      xmlFree (pprop);
      /**
       * set valid flag.
       */
      pchunk->valid_image = 0x1;
    }
  return 0;
}
/**
 * @brief split utf8 string and convert into unicode-16(ucs2),
 * store it into chunk structure.
 * @param utf8_str
 * @param zchunk
 * @return
 */
zint32_t
zsplit_utf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t utf8_len, ///<
		     zuint32_t *ucs2_buffer, zuint32_t ucs2_buf_len, zuint32_t *ucs2_count)
{
  zuint8_t *ptr;
  zuint32_t ucs2_code;
  zuint32_t ucs2_index;
  zuint32_t step;

  zint32_t tmp_ucs2_count = 0;
  /**
   * check valid of parameters.
   */
  if (utf8_str == NULL || utf8_len <= 0)
    {
      return -1;
    }

  /**
   * default step is 1.
   */
  step = 1;
  ucs2_index = 0;
  for (ptr = utf8_str; *ptr != '\0'; ptr += step)
    {
      /**
       * ASCII character.
       */
      if (*ptr < 128)
	{
#if (ZDBG_LEVEL&ZDBG_XML)
	  printf ("utf8:0x%x   --->   ", *ptr);
#endif
	  zutf8_to_ucs2 (ptr, &ucs2_code);
#if (ZDBG_LEVEL&ZDBG_XML)
	  printf ("ucs2:0x%x\n", ucs2_code);
#endif
	  /**
	   * store ucs2 code in chunk structure.
	   */
	  ucs2_buffer[ucs2_index++] = ucs2_code;
	  tmp_ucs2_count++;
	  step = 1;
	}
      else
	{
	  /**
	   * other character.
	   */
#if (ZDBG_LEVEL&ZDBG_XML)
	  printf ("utf8:0x%x 0x%x 0x%x   --->   ", *ptr, *(ptr + 1), *(ptr + 2));
#endif
	  zutf8_to_ucs2 (ptr, &ucs2_code);
#if (ZDBG_LEVEL&ZDBG_XML)
	  printf ("ucs2:0x%x\n", ucs2_code);
#endif
	  /**
	   * store ucs2 code in chunk structure.
	   */
	  ucs2_buffer[ucs2_index++] = ucs2_code;
	  tmp_ucs2_count++;
	  step = 3;
	}
    }

  *ucs2_count = tmp_ucs2_count;
  return 0;
}

/**
 * @brief utf8 to unicode-16.
 * @param utf8_str
 * @param ucs2_var
 * @return
 */
zint32_t
zutf8_to_ucs2 (zuint8_t *utf8_str, zuint32_t *ucs2_var)
{
  /**
   * check valid of parameters.
   */
  if (utf8_str == NULL || ucs2_var == NULL)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  /**
   * ASCII character.
   * less than 128.
   */
  if (*utf8_str < 0x80)
    {
      *ucs2_var = *utf8_str;
      return 0;
    }
  if ((utf8_str[0] & 0xE0) == 0xE0)
    {
      if (utf8_str[1] == 0 || utf8_str[2] == 0)
	{
	  return -1;
	}
      *ucs2_var = (utf8_str[0] & 0x0F) << 12 | (utf8_str[1] & 0x3F) << 6 | (utf8_str[2] & 0x3F);
      return 0;
    }

  if ((utf8_str[0] & 0xC0) == 0xC0)
    {
      if (utf8_str[1] == 0)
	{
	  return -1;
	}
      *ucs2_var = (utf8_str[0] & 0x1F) << 6 | (utf8_str[1] & 0x3F);
      return 0;
    }
  return -1;
}

/**
 * @brief
 * pixel format conversation.
 * from 8-bit to 32-bit.
 */
zint32_t
zpixel_format_convert (zuint8_t *src, zuint32_t srclen, ///<
		       zuint32_t color, ///<
		       zuint32_t *dst, zuint32_t dstlen)
{
  zuint8_t *pdest;
  zint32_t i;
  zuint8_t alpha;
  zuint8_t red;
  zuint8_t green;
  zuint8_t blue;
  /**
   * check validation of parameters!
   */
  if (src == NULL || srclen <= 0)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  if (dst == NULL || dstlen <= 0)
    {
      printf ("invalid parameters!\n");
      return -1;
    }
  /**
   * check buffer size.
   */
  if (dstlen < srclen * sizeof(zint32_t))
    {
      printf ("destination buffer is not enough!\n");
      return -1;
    }

  /**
   * split color.
   * AARRGGBB to AA,RR,GG,BB.
   */
  alpha = (zuint8_t) ((color >> 24) & 0xFF);
  red = (zuint8_t) ((color >> 16) & 0xFF);
  green = (zuint8_t) ((color >> 8) & 0xFF);
  blue = (zuint8_t) ((color >> 0) & 0xFF);

  /**
   * do matrix copy.
   */
  memset (dst, 0, dstlen);
  pdest = (zuint8_t*) dst;
  for (i = 0; i < srclen; i++)
    {
      if (src[i] != 0)
	{
	  *(pdest + 0x0) = alpha;
	}
      else
	{
	  /**
	   * transparent.
	   */
	  *(pdest + 0x0) = 0x0;
	}

      if (src[i] < 128)
	{
	  *(pdest + 0x0) = 0x0;

	}
      else
	{

	  /**
	   * color value
	   * let A= color value in ttf font library.
	   * let B= full color range,0x00~0xFF.
	   * let C= color value in xml file.
	   *
	   * so here,
	   * color=(A/B)*C.
	   */
	  *(pdest + 0x1) = (zuint8_t) (src[i] / 255.0 * red);
	  *(pdest + 0x2) = (zuint8_t) (src[i] / 255.0 * green);
	  *(pdest + 0x3) = (zuint8_t) (src[i] / 255.0 * blue);

//      printf("A:%02x,R:%02x,G:%02x,B:%02x\n",*(pdest + 0x0),*(pdest + 0x1),*(pdest + 0x2),*(pdest + 0x3));
	}
      /**
       * move to next.
       */
      pdest += sizeof(zint32_t);
    }
  return 0;
}

/**
 * support for png format subtitles.
 * added by zhangshaoyan at November 11,2014.
 */
zint32_t
zpng_read_argb (ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs)
{
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep *prows;
  zint32_t channels;
  zint32_t bpp;
  zint32_t color_type;
  zint32_t width, height;
  zint32_t i, j, k;
  zuint32_t offset;
  zuint8_t *pbuffer;

  if (pchunk == NULL || pcbs == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  fp = fopen ((zint8_t*) pchunk->z_png_file, "rb");
  if (fp == NULL)
    {
      ZPrintError("open %s failed!\n", pchunk->z_png_file);
      return -1;
    }
  /**
   * create read struct
   */
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  assert(png_ptr);
  /**
   * create info struct
   */
  info_ptr = png_create_info_struct (png_ptr);
  assert(info_ptr);

  setjmp(png_jmpbuf (png_ptr));

  /**
   * I/O initialization using standard C streams
   */
  png_init_io (png_ptr, fp);

  /**
   * read entire image , ignore alpha channel.
   */
  png_read_png (png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
  prows = png_get_rows (png_ptr, info_ptr);
  /**
   * get parameters from structure.
   */
  channels = png_get_channels (png_ptr, info_ptr);
  width = info_ptr->width;
  height = info_ptr->height;
  color_type = info_ptr->color_type;
  bpp = info_ptr->pixel_depth;

  printf ("png size:%d*%d,", width, height);
  printf ("color type:%d\n", color_type);
  printf ("pixel depth:%d\n", bpp);
  printf ("channel:%d\n", channels);

  /**
   * remember png size.
   */
  pchunk->z_png_width = width;
  pchunk->z_png_height = height;

  /**
   * read png data.
   */
#if 0
  offset = 0;
  pbuffer = (zuint8_t*) pcbs->pixel_buffer;
  /**
   * each pixel has 4 bytes,ARGB.
   */
  pcbs->pixel_len = height * width * sizeof(int);
  printf ("pcbs->pixel_len=%d*%d*%d=%d\n", height, width,sizeof(int), pcbs->pixel_len);
  for (i = 0; i < height; i++)
    {
      for (j = 0; j < channels * width; j += channels)
	{
	  /**
	   * if channels=3,pixel format is RGB.
	   * if channels=4,pixel format is RGBA.
	   */

	  printf ("(");
	  for (k = 0; k < channels; k++)
	    {
	      printf ("%d,", prows[i][j + k]);
	    }
	  printf ("),");

	  /**
	   * convert pixel format to ARGB.
	   */
	  switch (channels)
	    {
	      case 3:
	      *(pbuffer + offset + 0) = prows[i][j + 0]; //Red
	      *(pbuffer + offset + 1) = prows[i][j + 1];//Green
	      *(pbuffer + offset + 2) = prows[i][j + 2];//Blue
	      *(pbuffer + offset + 3) = 0xFF;//Alpha
	      break;

	      case 4:
	      *(pbuffer + offset + 0) = prows[i][j + 0];//Red
	      *(pbuffer + offset + 1) = prows[i][j + 1];//Green
	      *(pbuffer + offset + 2) = prows[i][j + 2];//Blue
	      *(pbuffer + offset + 3) = prows[i][j + 3];//Alpha
	      break;

	      default:
	      break;
	    }
	  /**
	   * increase offset.
	   */
	  offset += sizeof(zint32_t);
	}
      printf ("\n");

    }

  printf ("pixel buffer content.................\n");
  printf ("pixel buffer content.................\n");
  printf ("pixel buffer content.................\n");
  for (i = 0; i < pcbs->pixel_len; i++)
    {
      printf ("0x%02x,", pbuffer[i]);
      if ((i + 1) % (width*sizeof(int)) == 0)
	{
	  printf ("\n");
	}
    }
  printf ("\n");

  /**
   * resource memory.
   */
  png_destroy_read_struct (&png_ptr, &info_ptr, 0);
  fclose (fp);
#endif
  return 0;
}

/**
 * added by zhangshaoyan at November 14,2014.
 * for split task into more detail parts.
 */

/**
 * put a text frame data into FIFO.
 */
zint32_t
zxml_put_text_frame_into_fifo (ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs)
{
  zint32_t ret;
  zfpga_dev fpga;
  zuint32_t coordinate_x, coordinate_y;
  zuint32_t tmp32;

  if (pchunk == NULL || pcbs == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * lock resource.
   */
  pthread_mutex_lock (&pcbs->res_mutex);
  /**
   * wait for resource free.
   */
  while (pcbs->flag_resource != RESOURCE_FREE)
    {
      /**
       * check thread exit flag?
       */
      if (pcbs->exitFlag)
	{
	  pthread_mutex_unlock (&pcbs->res_mutex);
	  return -1;
	}
      pthread_cond_wait (&pcbs->res_cond, &pcbs->res_mutex);
    }
  ////////////////////////////////////////////////
  /**
   * split utf8 string and convert into unicode-16(ucs2),
   * store it into chunk structure.
   */
  zsplit_utf8_to_ucs2 (pchunk->z_utf8, pchunk->z_utf8_len, ///<
		       pchunk->z_ucs2, z_ucs2_buffer_size, &pchunk->z_ucs2_count);
  /////////////////////////////////////////////////
  /**
   * extract pixel matrix data from font library use freetype.
   * fill these original data into screen simulator RAM.
   *
   * the 3rd parameter is the character space,
   * the affect is clear when set value more than 1000.
   */
  //screen_ram_fill (&pcbs->screen_dev, lean_mode_normal, 0, pchunk->z_ucs2, pchunk->z_ucs2_count);
//screen_ram_fill (&pcbs->screen_dev, lean_mode_left, 0,pchunk->z_ucs2, pchunk->z_ucs2_count);
//  screen_ram_fill (&pcbs->screen_dev, lean_mode_right, 100, pchunk->z_ucs2, pchunk->z_ucs2_count);
  ///////////////////////////////////////////////////
  /**
   * calculate topest,bottomest,leftest,rightest coordinate for compress.
   * we do not need to search all columns!
   * the valid columns are only from 0 to number*font size.
   * here is ucs2_count * font size.
   * pchunk->z_ucs2_count * pchunk->font_size
//   */
//  screen_ram_calc_coordinate (&pcbs->screen_dev, ///<
//      pcbs->screen_dev.screen_height, ///<
//      pcbs->screen_dev.screen_width);
//  /**
//   * extract valid pixel matrix from screen ram.
//   */
//  screen_ram_extract_matrix (&pcbs->screen_dev);

  /**
   * pixel format convert.
   * from 8-bit to 32-bit.
//   */
//  ret = zpixel_format_convert (pcbs->screen_dev.frame_buffer, pcbs->screen_dev.frame_buffer_len, ///<
//			       pchunk->z_font_color, ///<
//			       pcbs->pixel_buffer, BUFSIZE_2MB);
//
//  if (ret < 0)
//    {
//      ZPrintError("pixel format convert failed!\n");
//      return -1;
//    }
//  pcbs->pixel_len = pcbs->screen_dev.frame_buffer_len * sizeof(zint32_t);
//
//#if 1
//  FILE *fp;
//  int i;
//  unsigned char *pchar = (unsigned char*) pcbs->pixel_buffer;
//  fp = fopen ("pixel.argb", "w");
//  if (fp != NULL)
//    {
//      fprintf (fp, "pixel len:%d\n", pcbs->pixel_len);
//      fprintf (fp, "width:%d,height:%d\n", pcbs->screen_dev.frame_width, pcbs->screen_dev.frame_height);
//      fprintf (fp, "32-bit raw data:%d bytes length\n", pcbs->pixel_len);
//      fprintf (fp, "width:%d,height:%d\n", pcbs->screen_dev.frame_width, pcbs->screen_dev.frame_height);
//
//      for (i = 0; i < pcbs->pixel_len; i++)
//	{
//	  fprintf (fp, "0x%02x,", pchar[i]);
//	  if ((i + 1) % (pcbs->screen_dev.frame_width * sizeof(int)) == 0)
//	    {
//	      fprintf (fp, "\n");
//	    }
//	}
//      fclose (fp);
//    }
//#endif

  /**
   * print out to check.
   */
#if 0
  for (i = 0; i < pcbs->pixel_len / sizeof(zint32_t); i++)
    {
      if (pcbs->pixel_buffer[i] & 0x00FFFFFF)
	{
	  printf ("\033[41m+\033[0m");
	}
      else
	{
	  printf ("_");
	}
      if ((i + 1) % pcbs->screen_dev.frame_width == 0)
	{
	  printf ("\n");
	}
    }
  printf ("\n");
#endif

  /**
   * arrange pixel data from chunk subtitles to FPGA Pixel Matrix format.
   * prepare to download to FPGA.
   */
//  if (zmmap_buffer_to_fpga_frame (pcbs->rle32_buffer, &fpga) < 0)
//    {
//      ZPrintError("zmmap_buffer_to_fpga_frame() failed!\n");
//      return -1;
//    }
//  /**
//   * clean.
//   */
//  pcbs->rle32_len = 0;
//  memset (pcbs->rle32_buffer, 0, BUFSIZE_2MB);

  /**
   * 16 bytes fixed frame header for sync.
   */
  memcpy (fpga.z_framehead, fixed_frame_header, 16);
  ///increase length
//  pcbs->rle32_len += framehead_length;

  /**
   * SpotNumber
   * takes up 2 bytes.
   */
  memset (fpga.z_spotnumber, 0, spotnumber_length);
  *(fpga.z_spotnumber + 0x0) = (zuint8_t) ((pchunk->z_spotnumber & 0xFFFF) >> 8);
  *(fpga.z_spotnumber + 0x1) = (zuint8_t) (pchunk->z_spotnumber & 0xFF);
  ///increase length
//  pcbs->rle32_len += spotnumber_length;

  /**
   * text left-top x coordinate.(horizontal).
   * takes up 2 bytes.
   */
  switch (pchunk->z_text_halign)
    {
    case halign_left:
      coordinate_x = pchunk->z_text_hposition;
      break;

    case halign_center:
//      coordinate_x = (pcbs->widthResolution >> 1) + pchunk->z_text_hposition - (pcbs->screen_dev.frame_width >> 1);
      break;

    case halign_right:
//      coordinate_x = pcbs->widthResolution - pchunk->z_text_hposition - pcbs->screen_dev.frame_width;
      break;

    default:
      /**
       * default is left.
       */
      coordinate_x = pchunk->z_text_hposition;
      break;
    }
  memset (fpga.z_left_top_x_coordinate, 0, left_top_x_coordinate_length);
  *(fpga.z_left_top_x_coordinate + 0x0) = (coordinate_x & 0xFF00) >> 8;
  *(fpga.z_left_top_x_coordinate + 0x1) = (coordinate_x & 0xFF);
  ///increase length
//  pcbs->rle32_len += left_top_x_coordinate_length;

  /**
   * text left-top y coordinate.(vertical).
   * takes up 2 bytes.
   */
  switch (pchunk->z_text_valign)
    {
    case valign_top:
      coordinate_y = pchunk->z_text_vposition;
      break;

    case valign_center:
//      coordinate_y = (pcbs->heightResolution >> 1) + pchunk->z_text_vposition - (pcbs->screen_dev.frame_height >> 1);
      break;

    case valign_bottom:
//      coordinate_y = pcbs->heightResolution - pchunk->z_text_vposition - pcbs->screen_dev.frame_height;
      break;
    default:
      /**
       * default is top.
       */
      coordinate_y = pchunk->z_text_vposition;
      break;
    }
  memset (fpga.z_left_top_y_coordinate, 0, left_top_y_coordinate_length);
  *(fpga.z_left_top_y_coordinate + 0x0) = (coordinate_y & 0xFF00) >> 8;
  *(fpga.z_left_top_y_coordinate + 0x1) = (coordinate_y & 0xFF);
  ///increase length
//  pcbs->rle32_len += left_top_y_coordinate_length;

  /**
   * text pixel matrix width.
   * takes up 2 bytes.
   */
  memset (fpga.z_picture_width, 0, subtitles_pixel_matrix_width_length);
//  *(fpga.z_picture_width + 0x0) = (zuint8_t) ((pcbs->screen_dev.frame_width & 0xFFFF) >> 8);
//  *(fpga.z_picture_width + 0x1) = (zuint8_t) (pcbs->screen_dev.frame_width & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_pixel_matrix_width_length;

  /**
   * text pixel matrix height
   * takes up 2 bytes.
   */
  memset (fpga.z_picture_height, 0, subtitles_pixel_matrix_height_length);
//  *(fpga.z_picture_height + 0x0) = (zuint8_t) ((pcbs->screen_dev.frame_height & 0xFFFF) >> 8);
//  *(fpga.z_picture_height + 0x1) = (zuint8_t) (pcbs->screen_dev.frame_height & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_pixel_matrix_height_length;

  /**
   * timein, start time.
   * takes up 3 bytes.
   */
  memset (fpga.z_timein, 0, subtitles_time_in_length);
  tmp32 = pchunk->z_timein.hh * pcbs->frameRate * 60 * 60 + ///<
      pchunk->z_timein.mm * pcbs->frameRate * 60 + ///<
      pchunk->z_timein.ss * pcbs->frameRate + ///<
      pchunk->z_timein.ee * pcbs->frameRate / 250;
  *(fpga.z_timein + 0x0) = (zuint8_t) ((tmp32 >> 16) & 0xFF);
  *(fpga.z_timein + 0x1) = (zuint8_t) ((tmp32 >> 8) & 0xFF);
  *(fpga.z_timein + 0x2) = (zuint8_t) ((tmp32 >> 0) & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_time_in_length;

  /**
   * timeout, stop time.
   * takes up 3 bytes.
   */
  memset (fpga.z_timeout, 0, subtitles_time_out_length);
  tmp32 = pchunk->z_timeout.hh * pcbs->frameRate * 60 * 60 + ///<
      pchunk->z_timeout.mm * pcbs->frameRate * 60 + ///<
      pchunk->z_timeout.ss * pcbs->frameRate + ///<
      pchunk->z_timeout.ee * pcbs->frameRate / 250;
  *(fpga.z_timeout + 0x0) = (zuint8_t) ((tmp32 >> 16) & 0xFF);
  *(fpga.z_timeout + 0x1) = (zuint8_t) ((tmp32 >> 8) & 0xFF);
  *(fpga.z_timeout + 0x2) = (zuint8_t) ((tmp32 >> 0) & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_time_out_length;

  /**
   * reserved 4 bytes.
   * clear to zero.
   */
  memset (fpga.z_reserved, 0, reserved_length);
  ///increase length
//  pcbs->rle32_len += reserved_length;
  /////////////////////////////////////////////////////

  /**
   * pixel matrix data length.
   * takes up 4 bytes.
   * will be set after merge pixel data.
   */
  memset (fpga.z_data_length, 0, subtitles_pixel_matrix_data_length_length);

  /**
   * set busy flag.
   * wake up download thread to compress & send to FPGA.
   */
  pcbs->flag_resource = RESOURCE_BUSY;
  pthread_mutex_unlock (&pcbs->res_mutex);
  pthread_cond_signal (&pcbs->res_cond);
  return 0;
}

/**
 * put a png frame data into FIFO.
 */
zint32_t
zxml_put_png_frame_into_fifo (ZCHUNK_SUBTITLES*pchunk, ZTHREAD_CBS* pcbs)
{
  zint32_t ret;
  zfpga_dev fpga;
  zuint32_t coordinate_x, coordinate_y;
  zuint32_t tmp32;

  if (pchunk == NULL || pcbs == NULL)
    {
      ZPrintError("invalid parameters!\n");
      return -1;
    }
  /**
   * lock resource.
   */
  pthread_mutex_lock (&pcbs->res_mutex);
  /**
   * wait for resource free.
   */
  while (pcbs->flag_resource != RESOURCE_FREE)
    {
      /**
       * check thread exit flag?
       */
      if (pcbs->exitFlag)
	{
	  pthread_mutex_unlock (&pcbs->res_mutex);
	  return -1;
	}
      pthread_cond_wait (&pcbs->res_cond, &pcbs->res_mutex);
    }

  /////////////////////////////////////////////
  zpng_read_argb (pchunk, pcbs);

  /**
   * arrange pixel data from chunk subtitles to FPGA Pixel Matrix format.
   * prepare to download to FPGA.
   */
//  if (zmmap_buffer_to_fpga_frame (pcbs->rle32_buffer, &fpga) < 0)
//    {
//      ZPrintError("zmmap_buffer_to_fpga_frame() failed!\n");
//      return -1;
//    }
  /**
   * clean.
   */
//  pcbs->rle32_len = 0;
//  memset (pcbs->rle32_buffer, 0, BUFSIZE_2MB);

  /**
   * 16 bytes fixed frame header for sync.
   */
  memcpy (fpga.z_framehead, fixed_frame_header, 16);
  ///increase length
//  pcbs->rle32_len += framehead_length;

  /**
   * SpotNumber
   * takes up 2 bytes.
   */
  memset (fpga.z_spotnumber, 0, spotnumber_length);
  *(fpga.z_spotnumber + 0x0) = (zuint8_t) ((pchunk->z_spotnumber & 0xFFFF) >> 8);
  *(fpga.z_spotnumber + 0x1) = (zuint8_t) (pchunk->z_spotnumber & 0xFF);
  ///increase length
//  pcbs->rle32_len += spotnumber_length;

  /**
   * png left-top x coordinate.(horizontal).
   * takes up 2 bytes.
   */
  switch (pchunk->z_png_halign)
    {
    case halign_left:
      coordinate_x = pchunk->z_png_hposition;
      break;

    case halign_center:
      coordinate_x = (pcbs->widthResolution >> 1) + pchunk->z_png_hposition - (pchunk->z_png_width >> 1);
      break;

    case halign_right:
      coordinate_x = pcbs->widthResolution - pchunk->z_png_hposition - pchunk->z_png_width;
      break;

    default:
      /**
       * default is left.
       */
      coordinate_x = pchunk->z_png_hposition;
      break;
    }
  memset (fpga.z_left_top_x_coordinate, 0, left_top_x_coordinate_length);
  *(fpga.z_left_top_x_coordinate + 0x0) = (coordinate_x & 0xFF00) >> 8;
  *(fpga.z_left_top_x_coordinate + 0x1) = (coordinate_x & 0xFF);
  ///increase length
//  pcbs->rle32_len += left_top_x_coordinate_length;

  /**
   * png left-top y coordinate.(vertical).
   * takes up 2 bytes.
   */
  switch (pchunk->z_png_valign)
    {
    case valign_top:
      coordinate_y = pchunk->z_png_vposition;
      break;

    case valign_center:
      coordinate_y = (pcbs->heightResolution >> 1) + pchunk->z_png_vposition - (pchunk->z_png_height >> 1);
      break;

    case valign_bottom:
      coordinate_y = pcbs->heightResolution - pchunk->z_png_vposition - pchunk->z_png_height;
      break;
    default:
      /**
       * default is top.
       */
      coordinate_y = pchunk->z_png_vposition;
      break;
    }
  memset (fpga.z_left_top_y_coordinate, 0, left_top_y_coordinate_length);
  *(fpga.z_left_top_y_coordinate + 0x0) = (coordinate_y & 0xFF00) >> 8;
  *(fpga.z_left_top_y_coordinate + 0x1) = (coordinate_y & 0xFF);
  ///increase length
//  pcbs->rle32_len += left_top_y_coordinate_length;

  /**
   * image pixel matrix width.
   * takes up 2 bytes.
   */
  memset (fpga.z_picture_width, 0, subtitles_pixel_matrix_width_length);
  *(fpga.z_picture_width + 0x0) = (zuint8_t) ((pchunk->z_png_width & 0xFFFF) >> 8);
  *(fpga.z_picture_width + 0x1) = (zuint8_t) (pchunk->z_png_width & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_pixel_matrix_width_length;

  /**
   * image pixel matrix height
   * takes up 2 bytes.
   */
  memset (fpga.z_picture_height, 0, subtitles_pixel_matrix_height_length);
  *(fpga.z_picture_height + 0x0) = (zuint8_t) ((pchunk->z_png_height & 0xFFFF) >> 8);
  *(fpga.z_picture_height + 0x1) = (zuint8_t) (pchunk->z_png_height & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_pixel_matrix_height_length;

  /**
   * timein, start time.
   * takes up 3 bytes.
   */
  memset (fpga.z_timein, 0, subtitles_time_in_length);
  tmp32 = pchunk->z_timein.hh * pcbs->frameRate * 60 * 60 + ///<
      pchunk->z_timein.mm * pcbs->frameRate * 60 + ///<
      pchunk->z_timein.ss * pcbs->frameRate + ///<
      pchunk->z_timein.ee * pcbs->frameRate / 250;
  *(fpga.z_timein + 0x0) = (zuint8_t) ((tmp32 >> 16) & 0xFF);
  *(fpga.z_timein + 0x1) = (zuint8_t) ((tmp32 >> 8) & 0xFF);
  *(fpga.z_timein + 0x2) = (zuint8_t) ((tmp32 >> 0) & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_time_in_length;

  /**
   * timeout, stop time.
   * takes up 3 bytes.
   */
  memset (fpga.z_timeout, 0, subtitles_time_out_length);
  tmp32 = pchunk->z_timeout.hh * pcbs->frameRate * 60 * 60 + ///<
      pchunk->z_timeout.mm * pcbs->frameRate * 60 + ///<
      pchunk->z_timeout.ss * pcbs->frameRate + ///<
      pchunk->z_timeout.ee * pcbs->frameRate / 250;
  *(fpga.z_timeout + 0x0) = (zuint8_t) ((tmp32 >> 16) & 0xFF);
  *(fpga.z_timeout + 0x1) = (zuint8_t) ((tmp32 >> 8) & 0xFF);
  *(fpga.z_timeout + 0x2) = (zuint8_t) ((tmp32 >> 0) & 0xFF);
  ///increase length
//  pcbs->rle32_len += subtitles_time_out_length;

  /**
   * reserved 4 bytes.
   * clear to zero.
   */
  memset (fpga.z_reserved, 0, reserved_length);
  ///increase length
//  pcbs->rle32_len += reserved_length;
  /////////////////////////////////////////////////////

  /**
   * pixel matrix data length.
   * takes up 4 bytes.
   * will be set after merge pixel data.
   */
  memset (fpga.z_data_length, 0, subtitles_pixel_matrix_data_length_length);

  /////////////////////////////////////////////

  /**
   * set busy flag.
   * wake up download thread to compress & send to FPGA.
   */
  pcbs->flag_resource = RESOURCE_BUSY;
  pthread_mutex_unlock (&pcbs->res_mutex);
  pthread_cond_signal (&pcbs->res_cond);
  return 0;
}
/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com.
 */
