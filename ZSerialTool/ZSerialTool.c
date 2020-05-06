/**
 * zserialtool.c
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
/**
 * dsp supported commands define.
 */
enum
{
  CmdDspSoftReset = 0x76, ///<
  CmdDspEnableWaterMarking = 0x61, ///<
  CmdDspDisableWaterMarking = 0x62, ///<
  CmdDspSetWaterMarkingSampleFrequency = 0x71, ///<
  CmdDspSetOutputChannel = 0x72, ///<
  CmdDspGetGlobalEnableStatus = 0x68, ///<
  CmdDspGetChannelActiveStatus = 0x6a, ///<
  CmdDspGetSoundPatternActiveStatus = 0x6b, ///<
  CmdDspGetSamplingFrequency = 0x69, ///<
  CmdDspGetVersion = 0x6d, ///<
};
#define ConstSerialPortName	"/dev/ttymxc1"
//#define ConstSerialPortName	"/dev/ttyUSB0"
typedef struct
{
  unsigned char command[128];
  unsigned char cmdValid;
  /**
   * support 128 parameters maximum.
   */
  unsigned int paramTotal;
  unsigned char parameter[128];
  unsigned char paramValid[128];
} CmdStruct;
typedef struct
{
  int gFd;
  pthread_t tidRead;
  pthread_t tidWrite;
  unsigned char exitFlag;
} ZSerialDev;
/**
 * global.
 */
ZSerialDev gDev;

void
printErrMsg (const char *msg)
{
  printf ("<error>:%s", msg);
}
#define WriteUartFailed		do{printErrMsg("write uart failed!\n");}while(0)
void
showHelpInfo ()
{
  printf ("<freeSendMode>:enter once free send mode.\n");
  printf ("<getStatus>:get sound pattern active status.\n");
  printf ("<getVersion>:get DSP firmware version number.\n");
  printf ("<getGlobalEnableStatus>:get global enable status.\n");
  printf ("<enableWaterMarking>:enable water marking.\n");
  printf ("<disableWaterMarking>:disable water marking.\n");
  printf ("<setSamplingFrequency number>:set water marker sampling frequency.\n");
  printf ("<getSamplingFrequency>: get sampling frequency.\n");
  printf ("<setOutputChannel number>:set DSP output channel number.\n");
  printf ("<hardReset>: hard reset the DSP.\n");
  printf ("<softReset>: soft reset the DSP.\n");
  printf ("<clear>: clear the screen.\n");
  printf ("<help>: show help information.\n");
  printf ("<exit>: quit SerialTool.\n");
}
/**
 * hard reset dsp.
 */
#define DSP_IOCTL_MAGIC	's'
#define DSP_IOCTL_RESET_LOW		_IO(DSP_IOCTL_MAGIC,9)
#define DSP_IOCTL_RESET_HIGH	_IO(DSP_IOCTL_MAGIC,10)
void
cmdHardResetDSP (void)
{
  int fd;
  fd = open ("/dev/imx5_cspi", O_RDWR);
  if (fd < 0)
    {
      printErrMsg ("open dsp device file failed!\n");
      return;
    }
  ioctl (fd, DSP_IOCTL_RESET_LOW);
  sleep(1);
  ioctl (fd, DSP_IOCTL_RESET_HIGH);
  sleep(1);
  close (fd);
  printf ("Hard Reset Done!\n");
  return;
}
void
cmdSoftResetDSP (int fd)
{
  char tCmd = CmdDspSoftReset;
  int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdSetOutputChannel (int fd, unsigned char channel)
{
  unsigned char tCommand[2];
  int tLen = sizeof(tCommand);
  if (fd < 0)
    {
      return;
    }
  tCommand[0] = CmdDspSetOutputChannel;
  tCommand[1] = channel;
  if (tLen != write (fd, tCommand, tLen))
    {
      WriteUartFailed
      ;
    }
}
void
cmdSetSamplingFrequency (int fd, unsigned char frequency)
{
  unsigned char tCommand[2];
  int tLen = sizeof(tCommand);
  if (fd < 0)
    {
      return;
    }
  tCommand[0] = CmdDspSetWaterMarkingSampleFrequency;
  tCommand[1] = frequency;
  if (tLen != write (fd, tCommand, tLen))
    {
      WriteUartFailed
      ;
    }
}
void
cmdGetSamplingFrequency (int fd)
{
  unsigned char tCmd = CmdDspGetSamplingFrequency;
  unsigned int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdEnableWaterMarker (int fd, int enable)
{
  unsigned char tCmd = (enable > 0) ? CmdDspEnableWaterMarking : CmdDspDisableWaterMarking;
  int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdGetStatus (int fd)
{
  unsigned char tCmd = CmdDspGetSoundPatternActiveStatus;
  int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdGetVersion (int fd)
{
  unsigned char tCmd = CmdDspGetVersion;
  int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdGetGlobalEnableStatus (int fd)
{
  unsigned char tCmd = CmdDspGetVersion;
  int tLen = sizeof(tCmd);
  if (fd < 0)
    {
      return;
    }
  if (tLen != write (fd, &tCmd, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
void
cmdEnterFreeSendMode (int fd)
{
  unsigned char lineBuffer[1024];
  unsigned char *pData;
  unsigned char tSendData;
  printf ("Enter free send mode.\n");
  printf ("Everything you input will be send out.\n");
  printf ("Split your input with space in hex format.\n");
  printf ("For example: 55 aa 01 02 03 04\n");
  while (1)
    {
      printf ("freeSendMode>");
      memset (lineBuffer, 0, sizeof(lineBuffer));
      if (NULL != fgets (lineBuffer, 1024, stdin))
	{
	  /**
	   * at least one byte,xx.
	   */
	  if (strlen (lineBuffer) < 2)
	    {
	      break;
	    }
	  pData = strtok (lineBuffer, " ");
	  if (NULL != pData)
	    {
	      printf ("sending:");
	    }
	  while (NULL != pData)
	    {
	      sscanf (pData, "%x", &tSendData);
	      printf ("%02x ", tSendData);
	      if (write (fd, &tSendData, sizeof(tSendData)) < 0)
		{
		  WriteUartFailed
		  ;
		  break;
		}
	      pData = strtok (NULL, " ");
	    }
	  printf ("Finished!\n");
	}
      break;
    }
}
void
cmdSetPayLoad (int fd, CmdStruct *tCmd)
{
  unsigned char tSendData[6];
  unsigned int i;
  unsigned int tLen = sizeof(tSendData);
  tSendData[0] = 0x63;
  if (fd < 0)
    {
      printf ("invalid device fd!\n");
      return;
    }
  if (tCmd->paramTotal > 0 && tCmd->paramTotal < 5)
    {
      printf ("Need 5 parameters in total\n");
      printf ("You only supply %d.\n", tCmd->paramTotal);
      return;
    }
  else if (0 == tCmd->paramTotal)
    {
      printf ("Default parameters were selected!\n");
      tSendData[1] = 0x06;
      tSendData[2] = 0xEC;
      tSendData[3] = 0xEC;
      tSendData[4] = 0x67;
      tSendData[5] = 0x89;
    }
  else
    {
      for (i = 0; i < 5; i++)
	{
	  if (tCmd->paramValid[i] != 0x1)
	    {
	      printf ("invalid parameters!\n");
	      return;
	    }
	  tSendData[i + 1] = tCmd->parameter[i];
	}
    }
  /**
   * show the user.
   */
  for (i = 0; i < 6; i++)
    {
      printf ("0x%x,", tSendData[i]);
    }
  printf ("\n");
  if (tLen != write (fd, tSendData, tLen))
    {
      WriteUartFailed
      ;
    }
  return;
}
/**
 * work threa.
 */
void *
threadSerialRead (void *arg)
{
  ZSerialDev *tDev = (ZSerialDev*) (arg);
  fd_set readSet;
  struct timeval tv;
  int ret;
  int i;
  unsigned char rxBuffer[1024];
  while (1)
    {
      /**
       * check exit flag.
       */
      if (0x1 == tDev->exitFlag)
	{
	  break;
	}
      /**
       * select().
       */
      FD_ZERO(&readSet);
      FD_SET(tDev->gFd, &readSet);
      tv.tv_sec = 1;
      tv.tv_usec = 0;
      ret = select (tDev->gFd + 1, &readSet, NULL, NULL, &tv);
      if (ret < 0)
	{
	  printf ("select() error!\n");
	  break;
	}
      else if (0 == ret)
	{
	  //timeout,continue to poll.
	  //printf("select() timeout.\n");
	  continue;
	}
      else
	{
	  if (FD_ISSET(tDev->gFd, &readSet))
	    {
	      ret = read (tDev->gFd, rxBuffer, 1024);
	      if (ret > 0)
		{
		  printf ("Received: %d bytes\n", ret);
		  for (i = 0; i < ret; i++)
		    {
		      printf ("\033[42m%02x,\033[0m", rxBuffer[i]);
		      if ((i + 1) % 10 == 0)
			{
			  printf ("\n");
			}
		    }
		  printf ("\n");
		}
	    }
	}
    }
  printf ("<ThreadRead>:done!\n");
  return 0;
}
void *
threadSerialWrite (void *arg)
{
  ZSerialDev *tDev = (ZSerialDev*) (arg);
  char lineBuffer[1024];
  char *pLine;
  CmdStruct tCmd;
  unsigned int i;
  while (1)
    {
      memset (lineBuffer, 0, sizeof(lineBuffer));
      printf ("\033[31mSerialTool>\033[0m");
      if (NULL != fgets (lineBuffer, 1024, stdin))
	{
	  //split parameters.
	  memset (&tCmd, 0, sizeof(tCmd));
	  /**
	   * the first string is command.
	   */
	  pLine = strtok (lineBuffer, " ");
	  if (NULL != pLine)
	    {
	      memcpy (tCmd.command, pLine, strlen (pLine));
	      tCmd.command[strlen (pLine)] = 0;
	      /**
	       * bug fixed for strtok.
	       */
	      if (tCmd.command[strlen (pLine) - 1] == '\n')
		{
		  tCmd.command[strlen (pLine) - 1] = 0;
		}
	      /**
	       * to avoid only \n character was input.
	       */
	      if (strlen (tCmd.command) <= 0)
		{
		  continue;
		}
	      tCmd.cmdValid = 0x1;
	    }
	  else
	    {
	      continue;
	    }
	  /**
	   * the second string is parameter.
	   * it maybe not exist.
	   */
	  i = 0;
	  pLine = strtok (NULL, " ");
	  while (NULL != pLine)
	    {
	      sscanf (pLine, "%x", &tCmd.parameter[i]);
	      tCmd.paramValid[i] = 0x1;
	      tCmd.paramTotal++;
	      i++;
	      //next.
	      pLine = strtok (NULL, " ");
	    }
//	  printf ("cmd:%s,valid:%d\n", tCmd.command, tCmd.cmdValid);
//	  printf ("param:%x,valid:%d\n", tCmd.parameter, tCmd.paramValid);
//	  break;

	  //parse command.
	  if (!strcmp (tCmd.command, "exit"))
	    {
	      break;
	    }
	  else if (!strcmp (tCmd.command, "clear"))
	    {
	      system ("clear");
	    }
	  else if (!strcmp (tCmd.command, "help"))
	    {
	      showHelpInfo ();
	    }
	  else if (!strcmp (tCmd.command, "hardReset"))
	    {
	      cmdHardResetDSP ();
	    }
	  else if (!strcmp (tCmd.command, "softReset"))
	    {
	      cmdSoftResetDSP (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "setOutputChannel"))
	    {
	      if (!tCmd.paramValid[0])
		{
		  printf ("missing channel parameter!\n");
		}
	      else
		{
		  cmdSetOutputChannel (tDev->gFd, tCmd.parameter[0]);
		}
	    }
	  else if (!strcmp (tCmd.command, "setSamplingFrequency"))
	    {
	      if (!tCmd.paramValid[0])
		{
		  printf ("missing sampling frequency parameter!\n");
		}
	      else
		{
		  cmdSetSamplingFrequency (tDev->gFd, tCmd.parameter[0]);
		}
	    }
	  else if (!strcmp (tCmd.command, "getSamplingFrequency"))
	    {
	      cmdGetSamplingFrequency (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "getGlobalEnableStatus"))
	    {
	      cmdGetGlobalEnableStatus (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "enableWaterMarking"))
	    {
	      cmdEnableWaterMarker (tDev->gFd, 1);
	    }
	  else if (!strcmp (tCmd.command, "disableWaterMarking"))
	    {
	      cmdEnableWaterMarker (tDev->gFd, 0);
	    }
	  else if (!strcmp (tCmd.command, "getStatus"))
	    {
	      cmdGetStatus (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "getVersion"))
	    {
	      cmdGetVersion (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "freeSendMode"))
	    {
	      cmdEnterFreeSendMode (tDev->gFd);
	    }
	  else if (!strcmp (tCmd.command, "payLoad"))
	    {
	      cmdSetPayLoad (tDev->gFd, &tCmd);
	    }
	  else
	    {
	      printf ("Invalid command!\n");
	      printf ("Type 'help' to get more information!\n");
	    }
	}
    }
  /**
   * set exit flag.
   */
  tDev->exitFlag = 0x1;
  printf ("<ThreadWrite>:done!\n");
  return 0;
}
int
main (void)
{
  struct termios tms;
  /**
   * open serial port.
   */
  gDev.gFd = open (ConstSerialPortName, O_RDWR | O_NDELAY | O_NOCTTY);
  if (gDev.gFd < 0)
    {
      printf ("<error>:open %s failed!", ConstSerialPortName);
      return -1;
    }
  tms.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  tms.c_cflag &= ~HUPCL;
  tms.c_lflag = 0;
  tms.c_iflag = IGNPAR;
  tms.c_cc[VTIME] = 0;
  tms.c_cc[VMIN] = 0;
  tcflush (gDev.gFd, TCIFLUSH);
  tcsetattr (gDev.gFd, TCSANOW, &tms);
  fcntl (gDev.gFd, F_SETFL, FNDELAY);

  /**
   * create work threads.
   */
  gDev.exitFlag = 0x0;
  if (0 != pthread_create (&gDev.tidRead, NULL, threadSerialRead, (void*) &gDev))
    {
      printf ("<error>:create read thread failed!\n");
      return -1;
    }
  if (0 != pthread_create (&gDev.tidWrite, NULL, threadSerialWrite, (void*) &gDev))
    {
      printf ("<error>:create write thread failed!\n");
      return -1;
    }

  signal (SIGINT, SIG_IGN);
  /**
   * wait threads done.
   */
  pthread_join (gDev.tidWrite, NULL);
  pthread_join (gDev.tidRead, NULL);
  printf ("<SerialTool>:bye!\n");
  return 0;
}
