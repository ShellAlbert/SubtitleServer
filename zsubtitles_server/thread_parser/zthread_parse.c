/**
 * @file zthread_parse.c
 * @date August 28,2014.
 * @version 1.0
 * @author shell.albert@gmail.com
 * @brief 
 * 	Thread for parse xml and extract pixel matrix from font library.
 * 	Convert pixel matrix to standard matrix size,
 * 	copy it into ZFIFO.
 */
#include <zthread_parse.h>
#include <zfree_type.h>
#include <zscreen_simulator.h>
#include <zrle32.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <math.h>
/**
 * print more details when parse xml file.
 * 1:turn on.
 * 0:turn off.
 */
zuint8_t bPrintXmlParseDetail = 0;
zuint8_t bPrintFSMFlow = 1;
zuint8_t bPrintScanDetail = 0;
zuint8_t bPrintRLE32Detail = 0;
zuint8_t ConstSyncFrameHeader[] =
  { 0xCC, 0xCC, 0xCC, 0xCC, ///<
      0x55, 0x55, 0x55, 0x55, ///<
      0xFF, 0xFF, 0xFF, 0xFF, ///<
      0x0, 0x0, 0x0, 0x0, ///<
    };
/**
 * @brief thread for parse.
 */
void *
zthread_parser (void *arg)
{
  zint32_t ret;
  ZXmlInfo *xmlInfo;
  ZScreenSimulatorDev *ssDev;
  zint32_t fsmState;
  ZTHREAD_CBS*pcbs;
  zint32_t i, j;
  zuint32_t nMaxSubtitleElements;
  sigset_t tSigMaskSet;
  /**
   * get process resource pointer.
   */
  pcbs = (ZTHREAD_CBS*) (arg);
  if (NULL == pcbs)
    {
      printf ("<zparser>:invalid thread control block structure!\n");
      return 0;
    }

  /**
   * mask the SIGIO signal.
   * only handle it in Receiver thread.
   */
  sigemptyset (&tSigMaskSet);
  sigaddset (&tSigMaskSet, SIGIO);
  if (pthread_sigmask (SIG_BLOCK, &tSigMaskSet, NULL))
    {
      printf ("<zparser>:block SIGIO signal failed!\n");
      return 0;
    }

  /**
   * initial finite state machine.
   */
  fsmState = FSM_Begin;
  while (1)
    {
      switch (fsmState)
	{
	/**
	 * FSM start,do some initial work and change to next state.
	 */
	case FSM_Begin:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Begin\n");
	    }

	  /**
	   * allocate memory.
	   */
	  xmlInfo = (ZXmlInfo*) malloc (sizeof(ZXmlInfo));
	  if (NULL == xmlInfo)
	    {
	      printf ("<zparser>:failed to allocate memory for ZXmlInfo!\n");
	      fsmState = FSM_Exit;
	      break;
	    }
	  /**
	   * maximum 100 <subtitle></subtitle>
	   */
	  nMaxSubtitleElements = 100;
	  xmlInfo->zSubtitleElement = (ZXmlSubtitleElement*) malloc (sizeof(ZXmlSubtitleElement) * nMaxSubtitleElements);
	  if (NULL == xmlInfo->zSubtitleElement)
	    {
	      printf ("<zparser>:failed to allocate memory for ZXmlSubtitleElement!\n");
	      free (xmlInfo);
	      fsmState = FSM_Exit;
	      break;
	    }
	  xmlInfo->zSubtitleCounter = 0;
	  for (i = 0; i < nMaxSubtitleElements; i++)
	    {
	      /**
	       * text element.
	       */
	      (xmlInfo->zSubtitleElement + i)->zTextCounter = 0;
	      for (j = 0; j < 10; j++)
		{
		  (xmlInfo->zSubtitleElement + i)->textElement[j].zTextLen = 0;
		}
	      /**
	       * image element.
	       */
	      (xmlInfo->zSubtitleElement + i)->zImageCounter = 0;
	      for (j = 0; j < 10; j++)
		{
		  (xmlInfo->zSubtitleElement + i)->imageElement[j].zPathLen = 0;
		}
	    }
	  /**
	   * ttf file memory buffer.
	   *
	   * here we consider that the SDK client may not send the ttf font library file.
	   * so in this situation it loads the default ttf file,
	   * file size is limited to 10MB maximum.
	   */
	  xmlInfo->ttfBuffer = (zuint8_t*) malloc (sizeof(zuint8_t) * 10 * 1024 * 1024);
	  if (NULL == xmlInfo->ttfBuffer)
	    {
	      printf ("<zparser>:failed to allocate memory for ttf file!\n");
	      free (xmlInfo->zSubtitleElement);
	      free (xmlInfo);
	      fsmState = FSM_Exit;
	      break;
	    }
	  xmlInfo->ttfBufferLen = 0;
	  /**
	   * do some initial work and change to next state.
	   */
	  fsmState = FSM_Wait_XML_File;
	  break;

	  /**
	   * The first is to wait for the xml file arriving.
	   */
	case FSM_Wait_XML_File:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Wait_XML_File\n");
	    }
	  ret = ZParser_FSM_Wait_XML_File (pcbs, xmlInfo);
	  /**
	   * if some error happened,reset the FSM.
	   * otherwise change to next state.
	   */
	  fsmState = (ret < 0) ? FSM_End : FSM_Read_XML_File;
	  break;

	  /**
	   * the second is to read xml file from RingBuffer.
	   */
	case FSM_Read_XML_File:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Read_XML_File\n");
	    }
	  ret = ZParser_FSM_Read_XML_File (pcbs, xmlInfo);
	  /**
	   * if some error happened,reset the FSM.
	   * otherwise change to next state.
	   */
	  fsmState = (ret < 0) ? FSM_End : FSM_Parse_XML_File;
	  break;

	  /**
	   * the third is to parse the xml file,
	   * do some pre-processing,
	   * extract more information,count the <Text></Text>
	   * and <Image></Image>,to see if we need ttf file.
	   */
	case FSM_Parse_XML_File:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Parse_XML_File\n");
	    }
	  ret = ZParser_FSM_Parse_XML_File (pcbs, xmlInfo);
	  /**
	   * if some error happened,reset the FSM.
	   * otherwise change to next state.
	   */
	  fsmState = (ret < 0) ? FSM_End : FSM_Wait_TTF_File;
	  break;

	  /**
	   * if textCounter is invalid,then we do not need ttf file.
	   * change to read png file state directly,otherwise
	   * wait for ttf font library.
	   */
	case FSM_Wait_TTF_File:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Wait_TTF_File\n");
	    }
	  /**
	   * bypass wait ttf state in debug mode.
	   */
#if 1
	  ret = ZParser_FSM_Wait_TTF_File (pcbs, xmlInfo);
#else
	  ret = 1;
#endif
	  /**
	   * if some error happened,reset the FSM.
	   * otherwise change to next state.
	   */
	  fsmState = (ret < 0) ? FSM_End : FSM_Load_TTF_File;
	  break;

	  /**
	   * load ttf file:
	   * the user specified ttf file or default ttf file.
	   */
	case FSM_Load_TTF_File:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Load_TTF_File\n");
	    }
	  fsmState = FSM_Create_Screen_Simulator;
	  break;

	  /**
	   * create the screen simulator for canvas work.
	   */
	case FSM_Create_Screen_Simulator:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Create_Screen_Simulator\n");
	    }
	  if (zscreen_simulator_create (&ssDev, pcbs->widthResolution, pcbs->heightResolution) < 0)
	    {
	      printf ("<zparser>:failed to create screen simulator!\n");
	      fsmState = FSM_End;
	    }
	  else
	    {
	      fsmState = FSM_Overlay_Element;
	    }
	  break;
	  /**
	   * loop to overlay elements until handle all elements finished.
	   */
	case FSM_Overlay_Element:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Overlay_Element\n");
	    }
	  for (i = 0; i < xmlInfo->zSubtitleCounter; i++)
	    {
	      /**
	       * handle <Subtitle></Subtitle> section.
	       */
	      ZParser_Handle_Subtitle (pcbs, ///<
		  &xmlInfo->zSubtitleElement[i], &xmlInfo->gFontAttr, ///<
		  ssDev, xmlInfo->ttfBuffer, xmlInfo->ttfBufferLen);
	    }
	  fsmState = FSM_Destroy_Screen_Simulator;
	  break;

	  /**
	   * destroy the screen simulator.
	   * free its memory.
	   */
	case FSM_Destroy_Screen_Simulator:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Destroy_Screen_Simulator\n");
	    }
	  if (NULL != ssDev)
	    {
	      zscreen_simulator_destroy (&ssDev);
	    }
	  fsmState = FSM_End;
	  break;
	  /**
	   * FSM end,do some clean work.
	   */
	case FSM_End:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_End\n");
	    }
	  /*free the document */
	  if (NULL != xmlInfo->doc)
	    {
	      xmlFreeDoc (xmlInfo->doc);
	      xmlInfo->doc = NULL;
	    }
	  /*
	   *Free the global variables that may
	   *have been allocated by the parser.
	   */
	  xmlCleanupParser ();

	  /**
	   * I'm finish my work,so exit now.
	   */
	  free (xmlInfo->ttfBuffer);
	  xmlInfo->ttfBuffer = NULL;
	  free (xmlInfo->zSubtitleElement);
	  xmlInfo->zSubtitleElement = NULL;
	  free (xmlInfo);
	  xmlInfo = NULL;

	  /**
	   * loop.
	   */
	  fsmState = FSM_Begin;
	  break;
	case FSM_Exit:
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Exit\n");
	    }
	  break;
	default:
	  break;
	}
      /**
       * if state is FSM_Exit,exit while(1).
       */
      if (FSM_Exit == fsmState)
	{
	  if (bPrintFSMFlow)
	    {
	      printf ("<zparser>:FSM_Exit\n");
	    }
	  break;
	}
    }
  /**
   * finite state machine ends.
   */
  printf ("<zparser>:thread is done!\n");
  pthread_exit (0);
  return 0;
}

/**
 * infinite to read ring buffer to get xml file type.
 */
zint32_t
ZParser_FSM_Wait_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo)
{
  ZBufferElement *element;
  ZBufferElementR2P tBufEleR2P;
  struct timeval tTv;
  struct timespec tTimeout;
  if (NULL == pcbs || NULL == xmlInfo)
    {
      printf ("<zparser>:wait xml file,invalid parameters!\n");
      return -1;
    }
  /**
   * initial xmlInfo structure.
   */
  xmlInfo->doc = NULL;
  xmlInfo->rootNode = NULL;
  xmlInfo->zSubtitleCounter = 0;
  while (1)
    {
      /**
       * get data from Receiver to Parser ring buffer.
       */
      pthread_mutex_lock (&pcbs->mutexR2P);
//      printf ("wait xml file...\n");
      /**
       * sleep until ring buffer has valid data.
       */
      while (ZRingBuffer_IsEmpty (pcbs->ringBufR2P))
	{
	  gettimeofday (&tTv, NULL);
	  tTimeout.tv_sec = tTv.tv_sec + 10;
	  tTimeout.tv_nsec = 0;
	  if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condR2PNotEmpty, &pcbs->mutexR2P, &tTimeout))
	    {
	      printf ("<zparser>:waiting RingBufferR2P Not Empty...\n");
	    }

	  /**
	   * judge thread exit flag in any sleep code.
	   */
	  if (0x1 == pcbs->exitFlag)
	    {
	      pthread_mutex_unlock (&pcbs->mutexR2P);
	      return -1;
	    }
	}

      /**
       * get valid element address.
       */
      element = ZRingBuffer_GetAElement (pcbs->ringBufR2P);
      if (NULL == element)
	{
	  printf ("<zparser>:failed to get a element from RingBuffer R2P!\n");
	  return -1;
	}
      ZMmap_Buffer_Receiver2Parser (element, &tBufEleR2P);
      printf ("<zparser>:get element type:%x\n", *(tBufEleR2P.fileType));
      if (Subtitle_File_Type_XML == *(tBufEleR2P.fileType))
	{
	  /**
	   * find it,but do not fetch.
	   */
	  pthread_mutex_unlock (&pcbs->mutexR2P);
	  printf ("<zparser>:get xml file,but do not fetch!\n");
	  return 0;
	}
      else
	{
	  /**
	   * do not find,ignore this invalid frame.
	   * trash it from ring buffer.
	   */
	  ZRingBuffer_DecTotalNum (pcbs->ringBufR2P);
	  printf ("<zparser>:this packet is not xml,trash it,wait next packet...\n");
	  continue;
	}
    }
  return 0;
}

zint32_t
ZParser_FSM_Read_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo)
{
  ZBufferElement *element;
  ZBufferElementR2P tBufEleR2P;
  struct timeval tTv;
  struct timespec tTimeout;
  zuint32_t i;
  if (NULL == pcbs || NULL == xmlInfo)
    {
      printf ("<zparser>:read xml file,invalid parameters!\n");
      return -1;
    }

  /**
   * get data from Receiver to Parser ring buffer.
   */
  pthread_mutex_lock (&pcbs->mutexR2P);
  /**
   * sleep until ring buffer has valid data.
   */
  while (ZRingBuffer_IsEmpty (pcbs->ringBufR2P))
    {
      gettimeofday (&tTv, NULL);
      tTimeout.tv_sec = tTv.tv_sec + 10;
      tTimeout.tv_nsec = 0;
      if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condR2PNotEmpty, &pcbs->mutexR2P, &tTimeout))
	{
	  printf ("<zparser>:waiting RingBufferR2P Not Empty...\n");
	}
      /**
       * judge thread exit flag in any sleep code.
       */
      if (0x1 == pcbs->exitFlag)
	{
	  pthread_mutex_unlock (&pcbs->mutexR2P);
	  return -1;
	}
    }

  /**
   * get valid element address.
   */
  element = ZRingBuffer_GetAElement (pcbs->ringBufR2P);
  if (NULL == element)
    {
      printf ("<zparser>:failed to get a element from RtoP ring buffer!\n");
      return -1;
    }
  /**
   * check file type again!
   */
  ZMmap_Buffer_Receiver2Parser (element, &tBufEleR2P);
  if (Subtitle_File_Type_XML == *(tBufEleR2P.fileType))
    {
#if 0
      FILE *fp;
      fp = fopen ("noname.xml", "w");
      if (NULL != fp)
	{
	  fwrite (tBufEleR2P.fileData, *(tBufEleR2P.fileDataLen), 1, fp);
	  fclose (fp);
	}
      printf ("write xml to file ok\n");
#endif
      /**
       * terminated with zero.
       */
      tBufEleR2P.fileName[*(tBufEleR2P.fileNameLen)] = 0;
      /**
       * read xml file from memory to get the DOM.
       */
      xmlInfo->doc = xmlReadMemory ((zint8_t*) tBufEleR2P.fileData, *(tBufEleR2P.fileDataLen), "noname.xml", "UTF-8", 0);
      if (NULL == xmlInfo->doc)
	{
	  printf ("<zparser>:could not parse file %s\n", tBufEleR2P.fileName);
	  return -1;
	}
      /**
       * get the root element node to parse xml file in recursive mode.
       */
      xmlInfo->rootNode = xmlDocGetRootElement (xmlInfo->doc);
      if (NULL == xmlInfo->rootNode)
	{
	  printf ("<zparser>:get root node failed!\n");
	  return -1;
	}

      /**
       * xml parse object has already generated,
       * so the data is no need to hold,remove it from ring buffer.
       */
      ZRingBuffer_DecTotalNum (pcbs->ringBufR2P);
#if 0
      printf ("remove xml file element from RingBuffer,now is %d\n", ///<
	  ZRingBuffer_GetTotalNum (pcbs->ringBufR2P));
#endif
      /**
       * We get one element from RingBuffer,so it's not full.
       * signal to wake up Receiver thread to put more elements.
       */
      pthread_cond_signal (&pcbs->condR2PNotFull);
      pthread_mutex_unlock (&pcbs->mutexR2P);
//      printf ("<zparser>:read xml file successful!\n");
      return 0;
    }
  else
    {
      printf ("<zparser>:FSM read xml file,the element is not xml file!\n");
      return -1;
    }
}
zint32_t
ZParser_FSM_Parse_XML_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo)
{
  /**
   * check validation of parameters.
   */
  if (NULL == pcbs || NULL == xmlInfo)
    {
      printf ("<zparser>:parse xml file,invalid parameters!\n");
      return -1;
    }

  /**
   * check xml validation.
   */
  if (NULL == xmlInfo->doc || NULL == xmlInfo->rootNode)
    {
      printf ("<zparser>:invalid xml doc or rootNode!\n");
      return -1;
    }

  /**
   * set default value before pre-handle.
   */
  xmlInfo->gFontAttr.zId = 0;
  xmlInfo->gFontAttr.zColor = 0xFFFFFFFF;
  xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_shadow;
  xmlInfo->gFontAttr.zEffectColor = 0xFF000000;
  xmlInfo->gFontAttr.zItalic = ZXml_Font_Italic_no;
  xmlInfo->gFontAttr.zScript = ZXml_Font_Script_normal;
  xmlInfo->gFontAttr.zSize = 42;
  xmlInfo->gFontAttr.zAspectAdjust = 1.0f;
  xmlInfo->gFontAttr.zUnderlined = ZXml_Font_Underlined_no;
  xmlInfo->gFontAttr.zWeight = ZXml_Font_Weight_normal;
  xmlInfo->gFontAttr.zSpacing = 0;
  /**
   * pre-handle xml file.
   * parse out <Text></Text> and <Image></Image>
   */
  if (ZParser_PreHandle_XmlFile (xmlInfo->rootNode, xmlInfo) < 0)
    {
      printf ("<zparser>:failed to pre-process xml file!\n");
      return -1;
    }

  /**
   * print out to check.
   */
#if 0
  zint32_t i, j;
  for (i = 0; i < xmlInfo->zSubtitleCounter; i++)
    {
      printf ("SpotNumber:%d\n", xmlInfo->zSubtitleElement[i].zSpotNumber);
      for (j = 0; j < xmlInfo->zSubtitleElement[i].zTextCounter; j++)
	{
	  printf ("<Text>,HAlign:%d,HPosition:%d,VAlign:%d,VPosition:%d.\n", ///<
	      xmlInfo->zSubtitleElement[i].textElement[j].zHalign,///<
	      xmlInfo->zSubtitleElement[i].textElement[j].zHposition,///<
	      xmlInfo->zSubtitleElement[i].textElement[j].zValign,///<
	      xmlInfo->zSubtitleElement[i].textElement[j].zVposition);
	}
    }
#endif
  return 0;
}

/**
 * count the number of <Text> and <Image> of each <Subtitle>
 *
 <Subtitle SpotNumber="13" TimeIn="00:00:30:000" TimeOut="00:00:34:200">
 <Text VAlign="bottom" VPosition="22">Top</Text>
 <Text VAlign="bottom" VPosition="22">Bottom</Text>
 <Text VAlign="bottom" VPosition="22">Left</Text>
 <Text VAlign="bottom" VPosition="22">Right</Text>
 <Text VAlign="bottom" VPosition="22">Middle</Text>
 <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">LeftTopCorner.png</Image>
 <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">RightTopCorner.png</Image>
 <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">LeftBottomCorner.png</Image>
 <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">RightBottomCorner.png</Image>
 </Subtitle>
 */
zint32_t
ZParser_PreHandle_XmlFile (xmlNode *rootNode, ZXmlInfo *xmlInfo)
{
  xmlNode *node;
  xmlNode *childNode;
  zuint8_t buffer[32];
  /**
   * check validation of parameters.
   */
  if (NULL == rootNode || NULL == xmlInfo)
    {
      printf ("<zparser>:pre-processing xml file,invalid parameters!");
      return -1;
    }
  /**
   * iterator all nodes.
   */
  for (node = rootNode; node != NULL; node = node->next)
    {
      /**
       * Font properties.
       */
      if (!strcmp ((char*) node->name, "Font"))
	{
	  xmlChar *pprop;
	  /**
	   * Id="simhei".
	   */
	  /**
	   * Color="FFFFFFFF".
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Color");
	  if (pprop != NULL)
	    {
	      /**
	       * format input in hex format.
	       */
	      sscanf ((zint8_t*) pprop, "%x", &xmlInfo->gFontAttr.zColor);
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zColor = 0xFFFFFFFF;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Color\" attribute,use 0xFFFFFFFF as default!\n");
		}
	    }
	  /**
	   * Effect="shadow"
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Effect");
	  if (pprop != NULL)
	    {
	      if (!strcmp ((zint8_t*) pprop, "none"))
		{
		  xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_none;
		}
	      else if (!strcmp ((zint8_t*) pprop, "border"))
		{
		  xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_border;
		}
	      else if (!strcmp ((zint8_t*) pprop, "shadow"))
		{
		  xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_shadow;
		}
	      else
		{
		  xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_shadow;
		}
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zEffect = ZXml_Font_Effect_shadow;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Effect\" attribute,use shadow as default!\n");
		}
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
	      sscanf ((zint8_t*) pprop, "%x", &xmlInfo->gFontAttr.zEffectColor);
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zEffectColor = 0xFF000000;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"EffectColor\" attribute,use 0xFF000000 as default!\n");
		}
	    }
	  /**
	   * Italic.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Italic");
	  if (pprop != NULL)
	    {
	      if (!strcmp ((zint8_t*) pprop, "yes"))
		{
		  xmlInfo->gFontAttr.zItalic = ZXml_Font_Italic_yes;
		}
	      else if (!strcmp ((zint8_t*) pprop, "no"))
		{
		  xmlInfo->gFontAttr.zItalic = ZXml_Font_Italic_no;
		}
	      else
		{
		  xmlInfo->gFontAttr.zItalic = ZXml_Font_Italic_no;
		}
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zItalic = ZXml_Font_Italic_no;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Italic\" attribute,use no as default!\n");
		}
	    }
	  /**
	   * Script.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Script");
	  if (pprop != NULL)
	    {
	      if (!strcmp ((zint8_t*) pprop, "normal"))
		{
		  xmlInfo->gFontAttr.zScript = ZXml_Font_Script_normal;
		}
	      else if (!strcmp ((zint8_t*) pprop, "super"))
		{
		  xmlInfo->gFontAttr.zScript = ZXml_Font_Script_super;
		}
	      else if (!strcmp ((zint8_t*) pprop, "sub"))
		{
		  xmlInfo->gFontAttr.zScript = ZXml_Font_Script_sub;
		}
	      else
		{
		  xmlInfo->gFontAttr.zScript = ZXml_Font_Script_normal;
		}
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zScript = ZXml_Font_Script_normal;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Script\" attribute,use normal as default!\n");
		}
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
	      xmlInfo->gFontAttr.zSize = atoi ((zint8_t*) pprop);
	      xmlFree (pprop);
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zSize = 42;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Size\" attribute,use 42 as default!\n");
		}
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
	      xmlInfo->gFontAttr.zAspectAdjust = atof ((zint8_t*) pprop);
	      xmlFree (pprop);
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zAspectAdjust = 1.0f;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"AspectAdjust\" attribute,use 1.0f as default!\n");
		}
	    }
	  /**
	   * Underlined.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Underlined");
	  if (pprop != NULL)
	    {
	      if (!strcmp ((zint8_t*) pprop, "yes"))
		{
		  xmlInfo->gFontAttr.zUnderlined = ZXml_Font_Underlined_yes;
		}
	      else if (!strcmp ((zint8_t*) pprop, "no"))
		{
		  xmlInfo->gFontAttr.zUnderlined = ZXml_Font_Underlined_no;
		}
	      else
		{
		  xmlInfo->gFontAttr.zUnderlined = ZXml_Font_Underlined_no;
		}
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zUnderlined = ZXml_Font_Underlined_no;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Underlined\" attribute,use no as default!\n");
		}
	    }
	  /**
	   * Weight.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "Weight");
	  if (pprop != NULL)
	    {
	      if (!strcmp ((zint8_t*) pprop, "bold"))
		{
		  xmlInfo->gFontAttr.zWeight = ZXml_Font_Weight_bold;
		}
	      else if (!strcmp ((zint8_t*) pprop, "normal"))
		{
		  xmlInfo->gFontAttr.zWeight = ZXml_Font_Weight_normal;
		}
	      else
		{
		  xmlInfo->gFontAttr.zWeight = ZXml_Font_Weight_normal;
		}
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zWeight = ZXml_Font_Weight_normal;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Weight\" attribute,use normal as default!\n");
		}
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
	      xmlInfo->gFontAttr.zSpacing = atof ((zint8_t*) pprop);
	      xmlFree (pprop);
	    }
	  else
	    {
	      xmlInfo->gFontAttr.zSpacing = 0.0f;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"Spacing\" attribute,use 0 as default!\n");
		}
	    }
	}
      /**
       * Subtitle properties.
       */
      if (!strcmp ((char*) node->name, "Subtitle"))
	{
	  xmlChar *pprop;
	  /**
	   * supply a shortcut.
	   */
	  ZXmlSubtitleElement *pCurSubEle = &xmlInfo->zSubtitleElement[xmlInfo->zSubtitleCounter];

	  /**
	   * SpotNumber="1"
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "SpotNumber");
	  if (pprop != NULL)
	    {
	      if (bPrintXmlParseDetail)
		{
		  printf ("SpotNumber:%s\n", pprop);
		}
	      pCurSubEle->zSpotNumber = atoi ((zint8_t*) pprop);
	      xmlFree (pprop);
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
	      pCurSubEle->zTimeIn.hh = atoi ((zint8_t*) buffer);
	      /**
	       * mm
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 3, sizeof(zint8_t) * 2);
	      pCurSubEle->zTimeIn.mm = atoi ((zint8_t*) buffer);
	      /**
	       * ss
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 6, sizeof(zint8_t) * 2);
	      pCurSubEle->zTimeIn.ss = atoi ((zint8_t*) buffer);
	      /**
	       * ee
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 9, sizeof(zint8_t) * 3);
	      pCurSubEle->zTimeIn.ee = atoi ((zint8_t*) buffer);
	      xmlFree (pprop);
	    }
	  else
	    {
	      pCurSubEle->zTimeIn.hh = 0;
	      pCurSubEle->zTimeIn.mm = 0;
	      pCurSubEle->zTimeIn.ss = 0;
	      pCurSubEle->zTimeIn.ee = 0;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"TimeIn\" attribute,use %d:%d:%d:%d as default!\n", ///<
		      pCurSubEle->zTimeIn.hh, pCurSubEle->zTimeIn.mm, ///<
		      pCurSubEle->zTimeIn.ss, pCurSubEle->zTimeIn.ee);
		}
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
	      pCurSubEle->zTimeOut.hh = atoi ((zint8_t*) buffer);
	      /**
	       * mm.
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 3, sizeof(zint8_t) * 2);
	      pCurSubEle->zTimeOut.mm = atoi ((zint8_t*) buffer);
	      /**
	       * ss.
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 6, sizeof(zint8_t) * 2);
	      pCurSubEle->zTimeOut.ss = atoi ((zint8_t*) buffer);
	      /**
	       * ee.
	       */
	      memset (buffer, 0, sizeof(buffer));
	      memcpy (buffer, pprop + sizeof(zint8_t) * 9, sizeof(zint8_t) * 3);
	      pCurSubEle->zTimeOut.ee = atoi ((zint8_t*) buffer);
	      xmlFree (pprop);
	    }
	  else
	    {
	      pCurSubEle->zTimeOut.hh = 0;
	      pCurSubEle->zTimeOut.mm = 0;
	      pCurSubEle->zTimeOut.ss = 0;
	      pCurSubEle->zTimeOut.ee = 0;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"TimeOut\" attribute,use %d:%d:%d:%d as default!\n", ///<
		      pCurSubEle->zTimeOut.hh, pCurSubEle->zTimeOut.mm, ///<
		      pCurSubEle->zTimeOut.ss, pCurSubEle->zTimeOut.ee);
		}
	    }

	  /**
	   * FadeUpTime.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "FadeUpTime");
	  if (pprop != NULL)
	    {
	      pCurSubEle->zFadeUpTime = atoi ((zint8_t*) pprop);
	      xmlFree (pprop);
	    }
	  else
	    {
	      pCurSubEle->zFadeUpTime = 20;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"FadeUpTime\" attribute,use 20 as default!\n");
		}
	    }

	  /**
	   * FadeDownTime.
	   */
	  pprop = xmlGetProp (node, (xmlChar*) "FadeDownTime");
	  if (pprop != NULL)
	    {
	      pCurSubEle->zFadeDownTime = atoi ((zint8_t*) pprop);
	      xmlFree (pprop);
	    }
	  else
	    {
	      pCurSubEle->zFadeDownTime = 20;
	      if (bPrintXmlParseDetail)
		{
		  printf ("warning,not find \"FadeDownTime\" attribute,use 20 as default!\n");
		}
	    }

	  /**
	   * <Text VAlign="bottom" VPosition="22">Right</Text>
	   * <Image HAlign="left" HPosition="22" VAlign="bottom" VPosition="22">CountDownSub_ch_R.png</Image>
	   */
	  pCurSubEle->zTextCounter = 0;
	  pCurSubEle->zImageCounter = 0;
	  for (childNode = node->children; childNode != NULL; childNode = childNode->next)
	    {
	      /**
	       * <Text></Text>
	       */
	      if (!strcmp ((char*) childNode->name, "Text"))
		{
		  /**
		   * Direction property.Direction="horizontal".
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "Direction");
		  if (pprop != NULL)
		    {
		      if (!strcmp ((zint8_t*) pprop, "horizontal"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zDirection = Text_Direction_Horizontal;
			}
		      else if (!strcmp ((zint8_t*) pprop, "vertical"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zDirection = Text_Direction_Vertical;
			}
		      else
			{
			  /**
			   * if the specified string is others,then use horizontal as default.
			   */
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zDirection = Text_Direction_Horizontal;
			  printf ("<Text>:\"Direction\" attribute specified in xml file is error,use horizontal as default!\n");
			}
		    }
		  else
		    {
		      /**
		       * default is horizontal.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zDirection = Text_Direction_Horizontal;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Text>:not find \"Direction\" attribute,use horizontal as default!\n");
			}
		    }

		  /**
		   * HAlign property.HAlign="bottom".
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "HAlign");
		  if (pprop != NULL)
		    {
		      if (!strcmp ((zint8_t*) pprop, "left"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zHalign = Text_HAlign_Left;
			}
		      else if (!strcmp ((zint8_t*) pprop, "center"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zHalign = Text_HAlign_Center;
			}
		      else if (!strcmp ((zint8_t*) pprop, "right"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zHalign = Text_HAlign_Right;
			}
		      else
			{
			  /**
			   * if the specified string is others,then use center as default.
			   */
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zHalign = Text_HAlign_Center;
			  printf ("<Text>:\"HAlign\" attribute specified in xml file is error,use center as default!\n");
			}
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default value is center.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zHalign = Text_HAlign_Center;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Text>:not find \"HAlign\" attribute,use center as default!\n");
			}
		    }

		  /**
		   * HPosition="22"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "HPosition");
		  if (pprop != NULL)
		    {
		      /**
		       * ascii to float.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zHposition = atof ((zint8_t*) pprop);
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default value is 0.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zHposition = 0;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Text>:not find \"HPosition\" attribute,use 0 as default!\n");
			}
		    }

		  /**
		   * VAlign="bottom"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "VAlign");
		  if (pprop != NULL)
		    {
		      if (!strcmp ((zint8_t*) pprop, "top"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zValign = Text_VAlign_Top;
			}
		      else if (!strcmp ((zint8_t*) pprop, "center"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zValign = Text_VAlign_Center;
			}
		      else if (!strcmp ((zint8_t*) pprop, "bottom"))
			{
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zValign = Text_VAlign_Bottom;
			}
		      else
			{
			  /**
			   * if the specified string is others,then use center as default.
			   */
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zValign = Text_VAlign_Center;
			  printf ("<Text>:\"VAlign\" attribute specified in xml file is error,use center as default!\n");
			}
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default is center.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zValign = Text_VAlign_Center;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Text>:not find \"VAlign\" attribute,use center as default!\n");
			}
		    }

		  /**
		   * VPosition="22"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "VPosition");
		  if (pprop != NULL)
		    {
		      /**
		       * ascii to float.
		       */
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zVposition = atof ((zint8_t*) pprop);
		      xmlFree (pprop);
		    }
		  else
		    {
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zVposition = 0;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Text>:not find \"VPosition\" attribute,use 0 as default!\n");
			}
		    }

		  /**
		   * <Text>content</Text>
		   */
		  pprop = xmlNodeGetContent (childNode);
		  if (pprop != NULL)
		    {
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zTextLen = strlen ((zint8_t*) pprop);
		      if (pCurSubEle->textElement[pCurSubEle->zTextCounter].zTextLen > ZXMLTEXTELEMENT_ZTEXT_MAXLEN)
			{
			  printf ("<Text>:waring,too long text content,truncate it!\n");
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zTextLen = ZXMLTEXTELEMENT_ZTEXT_MAXLEN;
			}
		      memcpy (pCurSubEle->textElement[pCurSubEle->zTextCounter].zText, ///<
			  (zint8_t*) pprop, ///<
			  pCurSubEle->textElement[pCurSubEle->zTextCounter].zTextLen);
		      xmlFree (pprop);
		    }
		  else
		    {
		      pCurSubEle->textElement[pCurSubEle->zTextCounter].zTextLen = 0;
		    }

		  /**
		   * increase text counter.
		   * this means there are more than one <text></text> in one <subtitle></subtitle> section.
		   * to avoid buffer overflow.
		   */
		  if (pCurSubEle->zTextCounter < ZXMLTEXTELEMENT_MAXNUM - 1)
		    {
		      pCurSubEle->zTextCounter++;
		    }
		  else
		    {
		      printf ("<Text>:warning,textElement is overflow!\n");
		    }
		}

	      /**
	       * <Image></Image>
	       */
	      if (!strcmp ((char*) childNode->name, "Image"))
		{
		  /**
		   * HAlign property.HAlign="bottom".
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "HAlign");
		  if (pprop != NULL)
		    {
		      if (!strcmp ((zint8_t*) pprop, "left"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHalign = Image_HAlign_Left;
			}
		      else if (!strcmp ((zint8_t*) pprop, "center"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHalign = Image_HAlign_Center;
			}
		      else if (!strcmp ((zint8_t*) pprop, "right"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHalign = Image_HAlign_Right;
			}
		      else
			{
			  /**
			   * if the specified string is others,then use center as default.
			   */
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHalign = Image_HAlign_Center;
			  printf ("<Image>:\"HAlign\" attribute specified in xml file is error,use center as default!\n");
			}
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default value is center.
		       */
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHalign = Image_HAlign_Center;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Image>:not find \"HAlign\" attribute,use center as default!\n");
			}
		    }

		  /**
		   * HPosition="22"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "HPosition");
		  if (pprop != NULL)
		    {
		      /**
		       * ascii to float.
		       */
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHposition = atof ((zint8_t*) pprop);
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default value is 0.
		       */
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zHposition = 0;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Image>:not find \"HPosition\" attribute,use 0 as default!\n");
			}
		    }

		  /**
		   * VAlign="bottom"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "VAlign");
		  if (pprop != NULL)
		    {
		      if (!strcmp ((zint8_t*) pprop, "top"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zValign = Image_VAlign_Top;
			}
		      else if (!strcmp ((zint8_t*) pprop, "center"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zValign = Image_VAlign_Center;
			}
		      else if (!strcmp ((zint8_t*) pprop, "bottom"))
			{
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zValign = Image_VAlign_Bottom;
			}
		      else
			{
			  /**
			   * if the specified string is others,then use center as default.
			   */
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zValign = Image_VAlign_Center;
			  printf ("<Image>:\"VAlign\" attribute specified in xml file is error,use center as default!\n");
			}
		      xmlFree (pprop);
		    }
		  else
		    {
		      /**
		       * default is center.
		       */
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zValign = Image_VAlign_Center;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Image>:not find \"VAlign\" attribute,use center as default!\n");
			}
		    }
		  /**
		   * VPosition="22"
		   */
		  pprop = xmlGetProp (childNode, (xmlChar*) "VPosition");
		  if (pprop != NULL)
		    {
		      /**
		       * ascii to float.
		       */
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zVposition = atof ((zint8_t*) pprop);
		      xmlFree (pprop);
		    }
		  else
		    {
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zVposition = 0;
		      if (bPrintXmlParseDetail)
			{
			  printf ("<Image>:not find \"VPosition\" attribute,use 0 as default!\n");
			}
		    }
		  /**
		   * <Image>content</Image>
		   */
		  pprop = xmlNodeGetContent (childNode);
		  if (pprop != NULL)
		    {
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPathLen = strlen ((zint8_t*) pprop);
		      if (pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPathLen > ZXMLIMAGEELEMENT_ZPATH_MAXLEN)
			{
			  printf ("<Image>:warning,too long path,truncate it!\n");
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPathLen = ZXMLIMAGEELEMENT_ZPATH_MAXLEN;
			}
		      memcpy (pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPath, ///<
			  (zint8_t*) pprop, ///<
			  pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPathLen);
		      xmlFree (pprop);
		    }
		  else
		    {
		      pCurSubEle->imageElement[pCurSubEle->zImageCounter].zPathLen = 0;
		    }

		  /**
		   * increase image counter.
		   * this means there are more than one <image></image> in one <subtitle></subtitle> section.
		   * to avoid buffer overflow.
		   */
		  if (pCurSubEle->zImageCounter < ZXMLIMAGEELEMENT_MAXNUM - 1)
		    {
		      pCurSubEle->zImageCounter++;
		    }
		  else
		    {
		      printf ("<Image>:warning,imageElement is overflow!\n");
		    }
		}
	    }
	  /**
	   * increase subtitle counter.
	   */
	  xmlInfo->zSubtitleCounter++;
	}
      /**
       * process next subtitle.
       */
      if (NULL != node->children)
	{
	  ZParser_PreHandle_XmlFile (node->children, xmlInfo);
	}
    }
  return 0;
}
/**
 * wait ttf file ok.
 */
zint32_t
ZParser_FSM_Wait_TTF_File (ZTHREAD_CBS *pcbs, ZXmlInfo *xmlInfo)
{
  zint32_t i, j;
  zint32_t bNeedTTFFile;
  ZBufferElement *element;
  ZBufferElementR2P tBufEleR2P;
  struct timeval tTv;
  struct timespec tTimeout;
  zint32_t ret = 0;
  if (NULL == pcbs || NULL == xmlInfo)
    {
      printf ("<zparser>:wait ttf file,invalid parameters!\n");
      return -1;
    }
  /**
   * if no <Text></Text> section in xml file,
   * the judge source is the zTextCounter equals to zero,
   * then no need to wait for ttf file.
   */
  bNeedTTFFile = 0x0;
  for (i = 0; i < xmlInfo->zSubtitleCounter; i++)
    {
      if (xmlInfo->zSubtitleElement[i].zTextCounter > 0)
	{
	  bNeedTTFFile = 0x1;
	  printf ("<zparser>:need TTF file\n");
	  break;
	}
    }
  /**
   * do not need ttf file.
   */
  if (0x0 == bNeedTTFFile)
    {
      printf ("<zparser>:do not need TTF file!\n");
      return 0;
    }
  /**
   * need ttf file,block here,
   * until received ttf file.
   */
#if 0
  for (i = 0; i < xmlInfo->zSubtitleCounter; i++)
    {
      for (j = 0; j < (xmlInfo->zSubtitleElement + i)->zTextCounter; j++)
	{
	  printf ("<Text>:%s\n", (xmlInfo->zSubtitleElement + i)->textElement[j].zText);
	}
      for (j = 0; j < (xmlInfo->zSubtitleElement + i)->zImageCounter; j++)
	{
	  printf ("<Image>:%s\n", (xmlInfo->zSubtitleElement + i)->imageElement[j].zPath);
	}
    }
#endif
#if 0
  printf ("waiting for TTF file...\n");
  printf ("Press Enter to continue...\n");
  getchar ();
#endif
  /**
   * get data from Receiver to Parser ring buffer.
   */
  pthread_mutex_lock (&pcbs->mutexR2P);
  /**
   * sleep until ring buffer has valid data.
   */
  while (ZRingBuffer_IsEmpty (pcbs->ringBufR2P))
    {
      /**
       * timeout to check exit flag.
       */
      gettimeofday (&tTv, NULL);
      tTimeout.tv_sec = tTv.tv_sec + 10;
      tTimeout.tv_nsec = 0;
      if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condR2PNotEmpty, &pcbs->mutexR2P, &tTimeout))
	{
	  printf ("<zparser>:waiting for RingBufferR2P Not Empty...\n");
	}
      /**
       * judge thread exit flag in any sleep code.
       */
      if (0x1 == pcbs->exitFlag)
	{
	  pthread_mutex_unlock (&pcbs->mutexR2P);
	  return -1;
	}
    }

  /**
   * get valid element address.
   */
  element = ZRingBuffer_GetAElement (pcbs->ringBufR2P);
  if (NULL == element)
    {
      printf ("<zparser>:failed to get a element from RingBuffer R2P!\n");
      return -1;
    }
  ZMmap_Buffer_Receiver2Parser (element, &tBufEleR2P);
  if (Subtitle_File_Type_TTF == *(tBufEleR2P.fileType))
    {
      /**
       * find it,but do not fetch.
       */
      printf ("<zparser>:find TTF file!\n");
      memcpy (xmlInfo->ttfBuffer, tBufEleR2P.fileData, *tBufEleR2P.fileDataLen);
      xmlInfo->ttfBufferLen = *tBufEleR2P.fileDataLen;

      /**
       * write received data to local file to check.
       */
#if 0
      FILE *fp;
      fp = fopen ("my.ttf", "w");
      if (NULL != fp)
	{
	  fwrite (tBufEleR2P.fileData, *tBufEleR2P.fileDataLen, 1, fp);
	  fclose (fp);
	  fp = NULL;
	}
#endif
      /**
       * element's life is over,remove it from RingBuffer.
       */
      ZRingBuffer_DecTotalNum (pcbs->ringBufR2P);
#if 0
      printf ("remove ttf element from RingBuffer,now number is %d\n", ///<
	  ZRingBuffer_GetTotalNum (pcbs->ringBufR2P));
#endif
      /**
       * We get one element from RingBuffer,so it's not full.
       * signal to wake up Reciever thread to put more element into it.
       */
      pthread_cond_signal (&pcbs->condR2PNotFull);
    }
  else
    {
      /**
       * do not find ttf file,
       * the second sended file is not ttf type.
       * load default ttf file.
       */
      printf ("<zparser>:warning,the second packet is not TTF file,load default!\n");
      FILE *fp;
      zuint32_t tFileLen;
      fp = fopen ("wqy-microhei.ttc", "r");
      if (fp != NULL)
	{
	  fseek (fp, 0, SEEK_END);
	  tFileLen = ftell (fp);
	  if (tFileLen > 0 && tFileLen < (10 * 1024 * 1024))
	    {
	      fseek (fp, 0, SEEK_SET);
	      fread (xmlInfo->ttfBuffer, tFileLen, 1, fp);
	      xmlInfo->ttfBufferLen = tFileLen;
	      printf ("<zparser>:load default ttf file success!\n");
	    }
	  else
	    {
	      printf ("<zparser>:default ttf,wrong file length!\n");
	      ret = -1;
	    }
	  fclose (fp);
	  fp = NULL;
	}
      else
	{
	  printf ("<zparser>:open default ttf file failed!\n");
	  ret = -1;
	}
    }
  pthread_mutex_unlock (&pcbs->mutexR2P);
  return ret;
}
/**
 * handle one subtitle section.
 * <Subtitle></Subtitle>
 */
zint32_t
ZParser_Handle_Subtitle (ZTHREAD_CBS *pcbs, ///<
    ZXmlSubtitleElement *subElement, ZXmlFontAttr *fontAttr, ///<
    ZScreenSimulatorDev*ssDev, ///<
    zuint8_t *ttfBuffer, zuint32_t ttfBufferLen)
{
  zint32_t i, j, k, t;
  zint32_t eachFrameRows;
  zint32_t yFrameCnt;
  zint32_t yRestRows;
  zint32_t yFrameCntValid;
  zint32_t offsetIndex;
  zint32_t lenCompress;
  ZBufferElement *tBufferElement;
  ZBufferElementP2D tBufEleP2D;
  zint32_t xPhysical, yPhysical, widthPhysical, heightPhysical;
  zuint32_t nCurFrameNo;
  zuint32_t frameDataLen;
  zuint32_t tTimeIn, tTimeOut;
  /**
   * mutex timeout handler.
   */
  struct timeval tCurTime;
  struct timespec tTimeoutTime;
  zuint32_t tTemp32;
  zuint32_t tTempFadeUpEnd, tTempFadeDownStart;
  zint32_t ret;

  /**
   * clear screen to zero before a new <Subtitle></Subtitle> starts.
   * this zscreen_simulator_clear do not work well.
   */
#if 0
  zscreen_simulator_clear (ssDev);
#else
  for (i = 0; i < ssDev->height; i++)
    {
      for (j = 0; j < ssDev->width; j++)
	{
	  *(ssDev->ramBuffer + i * ssDev->width + j) = 0;
	}
    }
#endif

  if (bPrintScanDetail)
    {
      printf ("Do drawing,SpotNumber:%d\n", subElement->zSpotNumber);
    }
  /**
   * draw <Text></Text> on screen.
   */
  for (i = 0; i < subElement->zTextCounter; i++)
    {
      if (bPrintScanDetail)
	{
	  printf ("drawing <Text>:%s\n", subElement->textElement[i].zText);
	}
      ZParser_Handle_SubtitleText (&subElement->textElement[i], fontAttr, ssDev, ttfBuffer, ttfBufferLen);
    }
  /**
   * draw <Image></Image> on screen.
   */
  for (j = 0; j < subElement->zImageCounter; j++)
    {
      if (bPrintScanDetail)
	{
	  printf ("drawing <Image>:%s\n", subElement->imageElement[j].zPath);
	}
      ZParser_Handle_SubtitleImage (pcbs, &subElement->imageElement[j], ssDev);
    }
//  printf ("Start to split screen....(Enter to continue)");
//  getchar ();
  /**
   * So far,we've drawn <Text> & <Image> on the screen simulator.
   * Go on,we should split it into n parts,scan progressive,
   * to find out the valid y frames and do RLE32 compress algorithm.
   */
  eachFrameRows = 100;
  yFrameCnt = ssDev->height / eachFrameRows;
  yRestRows = ssDev->height % eachFrameRows;
  if (bPrintScanDetail)
    {
      printf ("Total:%d,split into %d parts! Rest rows:%d\n", ssDev->height, yFrameCnt, yRestRows);
    }
  ZScanDev *scanDev = (ZScanDev*) malloc (sizeof(ZScanDev) * yFrameCnt);
  for (i = 0; i < yFrameCnt; i++)
    {
      (scanDev + i)->ssDev = ssDev;
      (scanDev + i)->yStart = (i * eachFrameRows);
      (scanDev + i)->yEnd = scanDev[i].yStart + eachFrameRows - 1;
    }
  /**
   * combine the rest rows into the last yFrame part.
   */
  if (yRestRows > 0)
    {
      (scanDev + yFrameCnt - 1)->yEnd += yRestRows;
    }
  if (bPrintScanDetail)
    {
      for (i = 0; i < yFrameCnt; i++)
	{
	  printf ("part:%d,yStart:%d,yEnd:%d\n", i, (scanDev + i)->yStart, (scanDev + i)->yEnd);
	}
    }

  /**
   * start scan progressive threads.
   */
  for (i = 0; i < yFrameCnt; i++)
    {
      if (0 != pthread_create (&(scanDev + i)->tid, NULL, zthread_screen_simulator_vertical_scan, (void*) (scanDev + i)))
	{
	  printf ("<zparser>:progressive scan,create thread %d failed!\n", i);
	}
      else
	{
	  if (bPrintScanDetail)
	    {
	      printf ("<zparser>:scan thread %x starts,scan range:%d to %d.\n", ///<
		  (zuint32_t) (scanDev + i)->tid, (scanDev + i)->yStart, (scanDev + i)->yEnd);
	    }
	}
    }
  /**
   * wait for threads finish.
   */
  for (i = 0; i < yFrameCnt; i++)
    {
      pthread_join ((scanDev + i)->tid, NULL);
      if (bPrintScanDetail)
	{
	  printf ("<zparser>:scan thread %0x done!\n", (zuint32_t) (scanDev + i)->tid);
	}
    }

  /**
   * scan the whole screen finish,
   * so,now,we should count the total counter of valid y frames.
   * and compress each y frame use the RLE-32,
   * then put it into Parser to Downloader Ring Buffer.
   */
  yFrameCntValid = 0;
  for (i = 0; i < yFrameCnt; i++)
    {
      if ((scanDev + i)->isValidFrame < 0)
	{
	  continue;
	}
      yFrameCntValid++;
    }
  if (bPrintScanDetail)
    {
      printf ("<zparser>:valid y Frame:%d\n", yFrameCntValid);
    }

  /**
   * loop to handle each y frame.
   */
  nCurFrameNo = 0;
  for (i = 0; i < yFrameCnt; i++)
    {
      /**
       * bypass invalid yFrame.
       */
      if ((scanDev + i)->isValidFrame < 0)
	{
	  continue;
	}
      /**
       * calculate subtitle picture physical size.
       */
      widthPhysical = (scanDev + i)->xValidEnd - (scanDev + i)->xValidStart + 1;
      heightPhysical = (scanDev + i)->yValidEnd - (scanDev + i)->yValidStart + 1;
      if (widthPhysical <= 0 || heightPhysical <= 0)
	{
	  continue;
	}

      /**
       * the number of first split frame was encoded from 1.
       */
      nCurFrameNo++;
      if (bPrintScanDetail)
	{
	  printf ("ValidFrame:%d,(%d,%d),(%d*%d)\n", ///<
	      i, (scanDev + i)->xValidStart, (scanDev + i)->yValidStart, widthPhysical, heightPhysical);
	}
      /**
       * copy valid yFrame pixel data from screen simulator to temporary buffer.
       */
      offsetIndex = 0;
      ssDev->yFrameLength = 0;
      for (j = (scanDev + i)->yValidStart; j <= (scanDev + i)->yValidEnd; j++)
	{
	  for (k = (scanDev + i)->xValidStart; k <= (scanDev + i)->xValidEnd; k++)
	    {
	      ssDev->yFrameBuffer[offsetIndex] = ssDev->ramBuffer[j * ssDev->width + k];
	      offsetIndex++;
	      ssDev->yFrameLength += sizeof(zuint32_t);
	    }
	}
      if (ssDev->yFrameLength <= 0)
	{
	  printf ("<zparser>:wrong yFrameLength!\n");
	  continue;
	}
      /**
       * compress pixel data use RLE-32.
       */
      //adjust byte order.
      //it is very important here.
      for (t = 0; t < ssDev->yFrameLength; t++)
	{
	  tTemp32 = *(ssDev->yFrameBuffer + t);
	  tTemp32 = htonl (tTemp32);
	  *(ssDev->yFrameBuffer + t) = tTemp32;
	}
#if 0
      FILE *fp;
      char buffer[256];
      sprintf (buffer, "%d.%d.RGB", subElement->zSpotNumber, i);
      fp = fopen (buffer, "w");
      if (NULL != fp)
	{
	  fwrite (ssDev->yFrameBuffer, ssDev->yFrameLength, 1, fp);
	  fclose (fp);
	}
#endif
#if 0
      printf ("error maybe start here!\n");
      printf ("yFrameBuffer=%p,yFrameLength=%d,yFrameRLE32Buffer=%p,Length=%d\n",      ///<
	  ssDev->yFrameBuffer, ssDev->yFrameLength, ssDev->yFrameRLE32Buffer, ConstyFrameRLE32BufferSize);
#endif
      /**
       * reset length.
       */
      ssDev->yFrameRLE32Length = 0;
      lenCompress = zrle32_compress (ssDev->yFrameBuffer, ssDev->yFrameLength, ssDev->yFrameRLE32Buffer, ConstyFrameRLE32BufferSize);
      if (bPrintRLE32Detail)
	{
	  printf ("original len:%d,compressed length:%d\n", ssDev->yFrameLength, lenCompress);
	}
      if (lenCompress <= 0)
	{
	  printf ("<zparser>:wrong compressed length!\n");
	  continue;
	}

      /**
       * put this frame into Parser to Downloader Ring Buffer.
       */
      pthread_mutex_lock (&pcbs->mutexP2D);
      do
	{
	  /**
	   * sleep until ring buffer has space.
	   */
	  while (ZRingBuffer_IsFull (pcbs->ringBufP2D))
	    {
	      /**
	       * timeout handler.
	       * if the RingBuffer P2D is not empty in 60 seconds,
	       * it means some error must happened.
	       */
	      gettimeofday (&tCurTime, NULL);
	      tTimeoutTime.tv_sec = tCurTime.tv_sec + 10;
	      tTimeoutTime.tv_nsec = 0;
	      if (ETIMEDOUT == pthread_cond_timedwait (&pcbs->condP2DNotFull, &pcbs->mutexP2D, &tTimeoutTime))
		{
		  printf ("<zparser>:waiting RingBufferP2D Not Full...\n");
		}
	      /**
	       * judge thread exit flag in any sleep code.
	       */
	      if (0x1 == pcbs->exitFlag)
		{
		  pthread_mutex_unlock (&pcbs->mutexP2D);
		  return -1;
		}
	    }

	  /**
	   * directly memory access.
	   */
	  tBufferElement = ZRingBuffer_PutAElement (pcbs->ringBufP2D);
	  if (NULL == tBufferElement)
	    {
	      printf ("<zparser>:get invalid element address for putting!\n");
	      break;
	    }
	  /**
	   * put data into ring buffer.
	   */
	  ZMmap_Buffer_Parser2Downloader (tBufferElement, &tBufEleP2D);
	  frameDataLen = 0;
	  //sync frame header,16 bytes.
	  memcpy (tBufEleP2D.zFrameHead, ConstSyncFrameHeader, sizeof(zuint8_t) * 16);
	  frameDataLen += sizeof(zuint8_t) * 16;
	  //SpotNumber,2 bytes.
	  *(tBufEleP2D.zSpotNumber + 0x0) = (zuint8_t) ((subElement->zSpotNumber & 0xFFFF) >> 8);
	  *(tBufEleP2D.zSpotNumber + 0x1) = (zuint8_t) (subElement->zSpotNumber & 0xFF);
	  frameDataLen += sizeof(zuint8_t) * 2;
	  //LeftTop x,2 bytes.
	  xPhysical = (scanDev + i)->xValidStart;
	  *(tBufEleP2D.zLeftTopX + 0x0) = (zuint8_t) ((xPhysical & 0xFFFF) >> 8);
	  *(tBufEleP2D.zLeftTopX + 0x1) = (zuint8_t) (xPhysical & 0xFF);
	  frameDataLen += sizeof(zuint8_t) * 2;
	  //LeftTop y,2 bytes.
	  yPhysical = (scanDev + i)->yValidStart;
	  *(tBufEleP2D.zLeftTopY + 0x0) = (zuint8_t) ((yPhysical & 0xFFFF) >> 8);
	  *(tBufEleP2D.zLeftTopY + 0x1) = (zuint8_t) (yPhysical & 0xFF);
	  frameDataLen += sizeof(zuint8_t) * 2;
	  //picture width,2 bytes.
	  *(tBufEleP2D.zPicWidth + 0x0) = (zuint8_t) ((widthPhysical & 0xFFFF) >> 8);
	  *(tBufEleP2D.zPicWidth + 0x1) = (zuint8_t) (widthPhysical & 0xFF);
	  //printf ("widthPhysical:%d\n", widthPhysical);
	  frameDataLen += sizeof(zuint8_t) * 2;
	  //picture height,2 bytes.
	  *(tBufEleP2D.zPicHeight + 0x0) = (zuint8_t) ((heightPhysical & 0xFFFF) >> 8);
	  *(tBufEleP2D.zPicHeight + 0x1) = (zuint8_t) (heightPhysical & 0xFF);
	  //printf ("heigthPhysical:%d\n", heightPhysical);
	  frameDataLen += sizeof(zuint8_t) * 2;

	  //TimeIn,3 bytes,Absolute Value.
	  tTimeIn = subElement->zTimeIn.hh * pcbs->frameRate * 60 * 60;
	  tTimeIn += subElement->zTimeIn.mm * pcbs->frameRate * 60;
	  tTimeIn += subElement->zTimeIn.ss * pcbs->frameRate;
	  tTimeIn += subElement->zTimeIn.ee * pcbs->frameRate / 250;
	  *(tBufEleP2D.zTimeIn + 0x0) = (zuint8_t) ((tTimeIn & 0xFF0000) >> 16);
	  *(tBufEleP2D.zTimeIn + 0x1) = (zuint8_t) ((tTimeIn & 0xFF00) >> 8);
	  *(tBufEleP2D.zTimeIn + 0x2) = (zuint8_t) ((tTimeIn & 0xFF) >> 0);
	  frameDataLen += sizeof(zuint8_t) * 3;

	  //TimeOut,3 bytes,Absolute Value.
	  tTimeOut = subElement->zTimeOut.hh * pcbs->frameRate * 60 * 60;
	  tTimeOut += subElement->zTimeOut.mm * pcbs->frameRate * 60;
	  tTimeOut += subElement->zTimeOut.ss * pcbs->frameRate;
	  tTimeOut += subElement->zTimeOut.ee * pcbs->frameRate / 250;
	  *(tBufEleP2D.zTimeOut + 0x0) = (zuint8_t) ((tTimeOut & 0xFF0000) >> 16);
	  *(tBufEleP2D.zTimeOut + 0x1) = (zuint8_t) ((tTimeOut & 0xFF00) >> 8);
	  *(tBufEleP2D.zTimeOut + 0x2) = (zuint8_t) ((tTimeOut & 0xFF) >> 0);
	  frameDataLen += sizeof(zuint8_t) * 3;

	  //Subtitle Total Frame Numbers.1 bytes.
	  *(tBufEleP2D.zFrameTotalNo) = (zuint8_t) (yFrameCntValid);
	  frameDataLen += sizeof(zuint8_t) * 1;
	  //Current subtitle frame number,1 bytes.
	  *(tBufEleP2D.zFrameCurrentNo) = (zuint8_t) (nCurFrameNo);
	  frameDataLen += sizeof(zuint8_t) * 1;

	  //FadeUpEnd,frame sequence number,Absolute Value.
	  tTempFadeUpEnd = tTimeIn + (subElement->zFadeUpTime * 4 / (1000 / pcbs->frameRate));
	  *(tBufEleP2D.zFadeUpEnd + 0x0) = (zuint8_t) ((tTempFadeUpEnd & 0xFF000000) >> 24);
	  *(tBufEleP2D.zFadeUpEnd + 0x1) = (zuint8_t) ((tTempFadeUpEnd & 0x00FF0000) >> 16);
	  *(tBufEleP2D.zFadeUpEnd + 0x2) = (zuint8_t) ((tTempFadeUpEnd & 0x0000FF00) >> 8);
	  *(tBufEleP2D.zFadeUpEnd + 0x3) = (zuint8_t) ((tTempFadeUpEnd & 0x000000FF) >> 0);
	  frameDataLen += sizeof(zuint8_t) * 4;

	  //FadeDownStart,Absolute Value.
	  tTempFadeDownStart = tTimeOut - (subElement->zFadeDownTime * 4 / (1000 / pcbs->frameRate));
	  *(tBufEleP2D.zFadeDownStart + 0x0) = (zuint8_t) ((tTempFadeDownStart & 0xFF000000) >> 24);
	  *(tBufEleP2D.zFadeDownStart + 0x1) = (zuint8_t) ((tTempFadeDownStart & 0x00FF0000) >> 16);
	  *(tBufEleP2D.zFadeDownStart + 0x2) = (zuint8_t) ((tTempFadeDownStart & 0x0000FF00) >> 8);
	  *(tBufEleP2D.zFadeDownStart + 0x3) = (zuint8_t) ((tTempFadeDownStart & 0x000000FF) >> 0);
	  frameDataLen += sizeof(zuint8_t) * 4;

	  //fade up alpha step value.
	  *(tBufEleP2D.zFadeUpAlphaStep) = (zuint8_t) ceil ((255.0 / (tTempFadeUpEnd - tTimeIn)));
	  frameDataLen += sizeof(zuint8_t) * 1;

	  //fade down alpha step value.
	  *(tBufEleP2D.zFadeDownAlphaStep) = (zuint8_t) ceil ((255.0 / (tTimeOut - tTempFadeDownStart)));
	  frameDataLen += sizeof(zuint8_t) * 1;

	  //Data Length,4 bytes.
	  *(tBufEleP2D.zDataLength + 0x0) = (zuint8_t) ((lenCompress & 0xFF000000) >> 24);
	  *(tBufEleP2D.zDataLength + 0x1) = (zuint8_t) ((lenCompress & 0x00FF0000) >> 16);
	  *(tBufEleP2D.zDataLength + 0x2) = (zuint8_t) ((lenCompress & 0x0000FF00) >> 8);
	  *(tBufEleP2D.zDataLength + 0x3) = (zuint8_t) ((lenCompress & 0x000000FF) >> 0);
	  frameDataLen += sizeof(zuint8_t) * 4;
	  //Data Body.
	  memcpy (tBufEleP2D.zDataBody, ssDev->yFrameRLE32Buffer, lenCompress);
	  frameDataLen += sizeof(zuint8_t) * lenCompress;
	  /**
	   * update element data length.
	   */
	  tBufferElement->elementDataLen = frameDataLen;
	  /**
	   * add data finish, so the total should be increase.
	   */
	  ZRingBuffer_IncTotalNum (pcbs->ringBufP2D);

#if 1
	  printf ("\033[45m<zparser>:put data into P2D RingBuffer ok! Total is %d\033[0m\n", ///<
	      ZRingBuffer_GetTotalNum (pcbs->ringBufP2D));
#endif

	  /**
	   * We put one element to RingBuffer,so it's not empty,
	   * signal to wake up Downloader thread.
	   * wake up downloader thread.
	   */
	  pthread_cond_signal (&pcbs->condP2DNotEmpty);
	}
      while (0);
      pthread_mutex_unlock (&pcbs->mutexP2D);
    }

  /**
   * goto label define here.
   */
  free (scanDev);
  return 0;
#if 0
  /**
   * store this spotnumber subtitle to local hard disk.
   */
  FILE *fp;
  zuint32_t tData;
  fp = fopen ("ss.dat", "w+");
  if (NULL != fp)
    {
      /**
       * width*height.
       */
      printf ("width:%d,height:%d\n", ssDev->width, ssDev->height);
      tData = htonl (ssDev->width);
      fwrite (&tData, sizeof(tData), 1, fp);
      tData = htonl (ssDev->height);
      fwrite (&tData, sizeof(tData), 1, fp);

      /**
       * find out how many valid.
       */
      yFrameCntValid = 0;
      for (i = 0; i < yFrameCnt; i++)
	{
	  if ((scanDev + i)->isValidFrame < 0)
	    {
	      continue;
	    }
	  yFrameCntValid++;
	}
      /**
       * write y frame count.
       */
      tData = htonl (yFrameCntValid);
      fwrite (&tData, sizeof(tData), 1, fp);
      /**
       * write each frame valid y start ,y end,x start,x end.
       */
      for (i = 0; i < yFrameCnt; i++)
	{
	  if ((scanDev + i)->isValidFrame < 0)
	    {
	      continue;
	    }

	  //yValidStart.
	  tData = htonl ((scanDev + i)->yValidStart);
	  fwrite (&tData, sizeof(tData), 1, fp);
	  //yValidEnd
	  tData = htonl ((scanDev + i)->yValidEnd);
	  fwrite (&tData, sizeof(tData), 1, fp);

	  //xValidStart.
	  tData = htonl ((scanDev + i)->xValidStart);
	  fwrite (&tData, sizeof(tData), 1, fp);
	  //xValidEnd.
	  tData = htonl ((scanDev + i)->xValidEnd);
	  fwrite (&tData, sizeof(tData), 1, fp);
	}

      /**
       * write pixel data.
       */
      for (i = 0; i < ssDev->width * ssDev->height; i++)
	{
	  tData = htonl (ssDev->ramBuffer[i]);
	  fwrite (&tData, sizeof(tData), 1, fp);
	}
      fclose (fp);
    }
#endif

  return 0;
}
zint32_t
ZParser_Handle_SubtitleText (ZXmlTextElement *textElement, ZXmlFontAttr *fontAttr, ///<
			     ZScreenSimulatorDev*ssDev, ///<
			     zuint8_t *ttfBuffer, zuint32_t ttfBufferLen)
{
  zuint32_t tBuffer[256];
  zuint32_t tUCS2Count;
  zint32_t ret;
  /**
   * check validation of parameters.
   */
  if (NULL == textElement || NULL == ssDev)
    {
      printf ("<zparser>:handle subtitle text,null pointer!\n");
      return -1;
    }
#if 0
  printf ("Direction:%d\n", textElement->zDirection);
  printf ("HAlign:%d,HPosition:%d\n", textElement->zHalign, textElement->zHposition);
  printf ("VAlign:%d,VPosition:%d\n", textElement->zValign, textElement->zVposition);
  printf ("len:%d,data:%s\n", textElement->zTextLen, textElement->zText);
#endif
  /**
   * libfreetype2 only supports ucs2(unicode-16),
   * so here convert utf8 to ucs2.
   */
  ret = zfreetype_split_utf8_to_ucs2 ((zuint8_t*) textElement->zText, textElement->zTextLen, ///<
				      tBuffer, 256, &tUCS2Count);
  if (ret < 0)
    {
      printf ("<zparser>:split utf8 to ucs2 failed!\n");
      return -1;
    }

  /**
   * draw text on (x,y).
   */
  zscreen_simulator_draw_text (ssDev, ///<
      tUCS2Count, (zint32_t*) tBuffer, ///<
      fontAttr, textElement, ///<
      ttfBuffer, ttfBufferLen);
  /**
   * show pixel matrix on screen.
   */
  //zscreen_simulator_print (ssDev);
  return 0;
}
zint32_t
ZParser_Handle_SubtitleImage (ZTHREAD_CBS *pcbs, ZXmlImageElement *imageElement, ZScreenSimulatorDev*ssDev)
{
  /**
   * check validation of parameters.
   */
  if (NULL == imageElement || NULL == ssDev)
    {
      printf ("<zparser>:handle subtitle image,null pointer!\n");
      return -1;
    }
  zscreen_simulator_draw_image (pcbs, ssDev, imageElement);
#if 0
  printf ("len:%d,data:%s\n", imageElement->zPathLen, imageElement->zPath);
#endif
  return 0;
}
/**
 * the end of file,tagged by zhangshaoyan.
 * shell.albert@gmail.com.
 */
