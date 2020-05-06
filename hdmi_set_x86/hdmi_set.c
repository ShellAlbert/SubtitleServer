#include <stdio.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include "para.h"

#define DEVICE_FILE_NAME	"/dev/mvc_card/card0"

/*ioctl magic number define*/
#define ZIOCTL_MAGIC1           'Z'
#define ZIOCTL_MAGIC2           'S'

/*test for basic read&write*/
#define ZIOCTL_RW_REGS_8        _IOWR(ZIOCTL_MAGIC1,0,char)
#define ZIOCTL_RW_REGS_16       _IOWR(ZIOCTL_MAGIC1,1,short)
#define ZIOCTL_RW_REGS_32       _IOWR(ZIOCTL_MAGIC1,2,int)

/*DRW:direct read & write*/
#define ZIOCTL_DRW_MAGIC	'S'
#define ZIOCTL_DR_REGS		_IOR(ZIOCTL_DRW_MAGIC,0,char)
#define ZIOCTL_DW_REGS		_IOW(ZIOCTL_DRW_MAGIC,1,char)
#define ZIOCTL_DMA_START	_IOW(ZIOCTL_DRW_MAGIC,2,char)
#define ZIOCTL_DMA_STOP		_IOW(ZIOCTL_DRW_MAGIC,3,char)

/*structure for ioctl()
 *basic pci register read&write
 */
typedef struct
{
  unsigned int offset;
  unsigned int value;
} STRU_DRW;

int
write_reg (int fd, int addr, int val)
{
  STRU_DRW drw;
  drw.offset = addr;
  drw.value = val;
  if (ioctl (fd, ZIOCTL_DW_REGS, &drw) != 0)
    {
      printf ("error:ioctl() write register failed!\n");
      return -1;
    }
  return 0;
}
int
read_reg (int fd, int addr)
{
  STRU_DRW drw;
  drw.offset = addr;
  if (ioctl (fd, ZIOCTL_DR_REGS, &drw) != 0)
    {
      printf ("ioctl() read register failed!\n");
      return -1;
    }
  else
    {
      return drw.value;
    }
}
int
main (void)
{
  int fd;
  int flags;
  int TMP;
  int RDATA;
  int OPB_FRAME_RATE;
  int INTOPIX_DECOPT;
  /**
   * open device file.
   */
  fd = open (DEVICE_FILE_NAME, O_RDWR);
  if (fd < 0)
    {
      printf ("open %s failed!\n", DEVICE_FILE_NAME);
      return -1;
    }
  flags = fcntl (fd, F_GETFD);
  flags |= FD_CLOEXEC;
  fcntl (fd, F_SETFD, flags);

  /**
   * set registers.
   */
  printf("Soft reset the FPGA system...\n");
  write_reg (fd, RST_CTL, 0x01); // soft reset
  sleep(5);// some delay for full Reset
  write_reg (fd, RST_CTL, 0x00); // soft reset
  printf ("Soft Reset Finished!\n");

  printf("Debug Mode Set\n");
  write_reg (fd, LBUS_ADDR, SOFT_DEBUG);   // write adr
  write_reg (fd, LBUS_WDATA, 0x03);

  printf ("Frame Rate Setting \n");
  //	 FRAME_RATE
  //	 bit7:0     frame_rate
  write_reg (fd, LBUS_ADDR, FRAME_RATE);   // write adr
  write_reg (fd, LBUS_WDATA, frame_rate);

  // frame_in_ddr_CTL
  write_reg (fd, LBUS_ADDR, frame_in_ddr_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, frame_in_ddr);


  printf ("JPEG2000 4K MODE Select \n");
  // MODE 4K
  write_reg (fd, LBUS_ADDR, MODE_4K);   // write adr
  write_reg (fd, LBUS_WDATA, mode4k);

  printf ("Video Mode Seting \n");

  // JPEG_WIDTH_HEIGHT
  write_reg (fd, LBUS_ADDR, JPEG_WIDTH_HEIGHT);
  write_reg (fd, LBUS_WDATA, jpeg_resolution);
  TMP = jpeg_resolution;
  //printf("Set JPEG_WIDTH_HEIGHT: %d\n",TMP);
  printf ("Set JPEG WIDTH : %d\n", TMP & 0x0000ffff);
  printf ("Set JPEG HEIGHT: %d\n", TMP >> 16);

  // left_eye_white_line_CTL
  write_reg (fd, LBUS_ADDR, left_eye_white_line_CTL);
  write_reg (fd, LBUS_WDATA, left_eye_white_line);
  TMP = left_eye_white_line;
  printf ("Set 3D LEFT EYE: 0x%x\n", TMP);

  // right_eye_white_line_CTL
  write_reg (fd, LBUS_ADDR, right_eye_white_line_CTL);
  write_reg (fd, LBUS_WDATA, right_eye_white_line);
  TMP = right_eye_white_line;
  printf ("Set 3D RIGHT EYE: 0x%x\n", TMP);

  // lvds module control setting
  write_reg (fd, LBUS_ADDR, P_V_ACTIVE_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_V_ACTIVE);
  TMP = P_V_ACTIVE;
  printf ("Set P_V_ACTIVE: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_V_BACK_PORCH_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_V_BACK_PORCH);
  TMP = P_V_BACK_PORCH;
  printf ("SET P_V_BACK_PORCH: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_V_FRONT_PORCH_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_V_FRONT_PORCH);
  TMP = P_V_FRONT_PORCH;
  printf ("SET P_V_FRONT_PORCH: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_V_TOTAL_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_V_TOTAL);
  TMP = P_V_TOTAL;
  printf ("SET P_V_TOTAL: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_H_ACTIVE_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_H_ACTIVE);
  TMP = P_H_ACTIVE;
  printf ("SET P_H_ACTIVE: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_H_BACK_PORCH_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_H_BACK_PORCH);
  TMP = P_H_BACK_PORCH;
  printf ("SET P_H_BACK_PORCH: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_H_FRONT_PORCH_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_H_FRONT_PORCH);
  TMP = P_H_FRONT_PORCH;
  printf ("SET P_H_FRONT_PORCH: 0x%x\n", TMP);

  write_reg (fd, LBUS_ADDR, P_H_TOTAL_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, P_H_TOTAL);
  TMP = P_H_TOTAL;
  printf ("SET P_H_TOTAL: 0x%x\n", TMP);

  // mpeg_v_total_CTL
  write_reg (fd, LBUS_ADDR, mpeg_v_total_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, mpeg_v_total);
  TMP = mpeg_v_total;
  printf ("SET mpeg_v_total: %d\n", TMP);

  // mpeg_h_total_CTL
  write_reg (fd, LBUS_ADDR, mpeg_h_total_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, mpeg_h_total);
  TMP = mpeg_h_total;
  printf ("SET mpeg_h_total: %d\n", TMP);


  printf ("FPGA Version \n");
  // FPGA_VERSION
  write_reg (fd, LBUS_ADDR, FPGA_VERSION);   // write adr
  RDATA = read_reg (fd, LBUS_ANC_RDATA);
  RDATA = read_reg (fd, LBUS_RDATA);
  printf ("Read FPGA Version: 0x%x \n", RDATA);

  printf (" JPEG/MPEG/HEMI select \n");
  // codestream_kind_CTL
  write_reg (fd, LBUS_ADDR, codestream_kind_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, codestream_kind);
  TMP = codestream_kind;
  printf ("SET codestream_kind: %d.......\n", TMP);
  printf ("Set CodeStream Kind: .......\n");
  printf ("CodeStream Kind: 1: JPEG2000,  2: MPEG2,  3: HDMI .......\n");

  printf ("JPEG2000/MPEG2 Video/Audio Start Delay \n");
  // jpeg_video_delay_num_CTL
  write_reg (fd, LBUS_ADDR, jpeg_video_delay_num_CTL);   // write adr
  write_reg (fd, LBUS_WDATA, jpeg_video_delay_num);
   TMP = jpeg_video_delay_num;
   printf ("SET jpeg_video_delay_num: 0x%x.......\n", TMP);

   // jpeg_audio_delay_num_CTL
   write_reg (fd, LBUS_ADDR, jpeg_audio_delay_num_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, jpeg_audio_delay_num);
   TMP = jpeg_audio_delay_num;
   printf ("SET jpeg_audio_delay_num: 0x%x.......\n", TMP);

   // mpeg_video_delay_num_CTL
   write_reg (fd, LBUS_ADDR, mpeg_video_delay_num_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, mpeg_video_delay_num);
   TMP = mpeg_video_delay_num;
   printf ("SET mpeg_video_delay_num: 0x%x.......\n", TMP);

   // mpeg_audio_delay_num_CTL
   write_reg (fd, LBUS_ADDR, mpeg_audio_delay_num_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, mpeg_audio_delay_num);
   TMP = mpeg_audio_delay_num;
   printf ("SET mpeg_audio_delay_num: 0x%x.......\n", TMP);


   printf ("JPEG2000 Master Slave Select \n");

   // slave_sync_sel_CTL
   write_reg (fd, LBUS_ADDR, slave_sync_sel_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, slave_sync_sel);
   TMP = slave_sync_sel;
   printf ("SET slave_sync_sel: %x.......\n", TMP);
   printf ("Master/Slave Select : .......\n");
   printf ("Master : 0,   Slave : 1  .......\n");


   printf ("Time Code Function \n");
   // timecode_CTL usage example
   // set
   write_reg (fd, LBUS_ADDR, timecode_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, 0x12345678);
   printf ("Test: Set TIME CODE = 0x12345678 \n");
   // get
   write_reg (fd, LBUS_ADDR, timecode_CTL);   // write adr
   RDATA = read_reg (fd, LBUS_ANC_RDATA);
   RDATA = read_reg (fd, LBUS_RDATA);
   printf ("Test: Get TIME CODE = 0x%x \n", RDATA);

   printf ("JPEG2000 Audio Input Frequecny \n");
   // audio_in_freq_CTL
   write_reg (fd, LBUS_ADDR, audio_in_freq_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, audio_in_freq);
   TMP = audio_in_freq;
   printf ("SET Audio In Frequency: %d\n", TMP);

   printf ("JPEG2000 Audio Output Frequecny \n");
   // audio_out_freq_CTL
   write_reg (fd, LBUS_ADDR, audio_out_freq_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, audio_out_freq);
   TMP = audio_out_freq;
   printf ("SET Audio Output Frequency: %d\n", TMP);

   printf ("JPEG2000/MPEG Audio channel number \n");
   // audio_chn_num_CTL
   write_reg (fd, LBUS_ADDR, audio_chn_num_CTL);   // write adr
   write_reg (fd, LBUS_WDATA, audio_chn_num);
   TMP = audio_chn_num;
   printf ("SET Audio Channel Number : %d\n", TMP);

   ///// HDMI
   write_reg (fd, LBUS_ADDR, 0xc0);				// write adr
   write_reg (fd, LBUS_WDATA, 0x01);				// write adr
   ///// HDMI

   printf ("Enter INTOPIX Core's OPB Register Setting and CIVOLUTON's Watermark Core Setting\n");

   // Read <MODE 4K> register and generate INTOPIX_DECOPT value
    // micro	:	MODE_4K_Off = 0   			     	// only decode 2K resolution
    // IMB 		:	INTOPIX_DECOPT	0x73020080		// Detect 4K,Drop 4K
    // micro	:	MODE_4K_Automatic = 1   		 	// automatically switch to 4K depending on the video material
    // IMB		:	INTOPIX_DECOPT	0x73020000		// Normal
    // micro	:	MODE_4K_Always = 2   		    	// upscale any resolution to 4K
    // IMB		:	INTOPIX_DECOPT	0x73020100		// upscale the frame (e.g. from 2k to 4k)

   write_reg (fd, LBUS_ADDR, MODE_4K);   // write adr
    RDATA = read_reg (fd, LBUS_ANC_RDATA);
    RDATA = read_reg (fd, LBUS_RDATA);
    printf ("Read MODE_4K: %x \n", RDATA);

    if (RDATA == 0)
      INTOPIX_DECOPT = 0x73020080;
    if (RDATA == 1)
      INTOPIX_DECOPT = 0x73020000;
    if (RDATA == 2)
      INTOPIX_DECOPT = 0x73020100;

    // Read <FRAME_RATE> register and generate OPB_FRAME_RATE and WM_HEADER
    write_reg (fd, LBUS_ADDR, FRAME_RATE);   // write adr
    RDATA = read_reg (fd, LBUS_ANC_RDATA);
    RDATA = read_reg (fd, LBUS_RDATA);
    printf ("Get FRAME_RATE: %d \n", RDATA);

    // Generate OPB_FRAME_RATE
    //#define  OPB_FRAME_RATE    0x74010080					// 24fps
    //#define  OPB_FRAME_RATE	   0x74010092					// 30fps
    //#define  OPB_FRAME_RATE    0x74010081					// 48fps
    //#define  OPB_FRAME_RATE    0x740100A1					// 50fps
    //#define  OPB_FRAME_RATE    0x740100A2					// 60fps
    //#define  OPB_FRAME_RATE    0x740100B0					// 96fps
    //#define  OPB_FRAME_RATE	   0x740100B2					// 120fps

    if (RDATA == 24)
      OPB_FRAME_RATE = 0x74010080;
    if (RDATA == 30)
      OPB_FRAME_RATE = 0x74010092;
    if (RDATA == 48)
      OPB_FRAME_RATE = 0x74010081;
    if (RDATA == 50)
      OPB_FRAME_RATE = 0x740100A1;
    if (RDATA == 60)
      OPB_FRAME_RATE = 0x740100A2;
    if (RDATA == 96)
      OPB_FRAME_RATE = 0x740100B0;
    if (RDATA == 120)
      OPB_FRAME_RATE = 0x740100B2;

    // WM_HEADER: bit0:    enable_3d,	 bit7:4   fps_mode
    //----- 2D
    //	 #define WM_VALUE  0x00							//24fps
    //	 #define WM_VALUE  0x20							//30fps
    //	 #define WM_VALUE  0x30							//48fps
    //   #define WM_VALUE  0x40							//50fps
    //	 #define WM_VALUE  0x50							//60fps
    //	 #define WM_VALUE  0x60							//96fps
    //	 #define WM_VALUE  0x70							//120fps
    //------ 3D
    //   #define WM_VALUE  0x81							//24FPS single eye
    //	 #define WM_VALUE  0xA1							//30fps
    //	 #define WM_VALUE  0xB1							//48fps
    //	 #define WM_VALUE  0xD1							//60fps

    if (EN_3D == 0)
      {
        if (RDATA == 24)
  	TMP = 0x00;
        if (RDATA == 30)
  	TMP = 0x20;
        if (RDATA == 48)
  	TMP = 0x30;
        if (RDATA == 50)
  	TMP = 0x40;
        if (RDATA == 60)
  	TMP = 0x50;
        if (RDATA == 96)
  	TMP = 0x60;
        if (RDATA == 120)
  	TMP = 0x70;
      }
    else
      {
        if (RDATA == 48)
  	TMP = 0x81;
        if (RDATA == 60)
  	TMP = 0xA1;
        if (RDATA == 96)
  	TMP = 0xB1;
        if (RDATA == 120)
  	TMP = 0xD1;
      }

    write_reg (fd, LBUS_ADDR, WM_HEADER);   // write adr
    write_reg (fd, LBUS_WDATA, TMP);
    printf ("Set CIVOLUTION Core's parameter (WM_VALUE): 0x%x,  According to FRAME RATE and 3D mode control\n", TMP);

    printf ("Please Check:  MICROM's Frame Rate and CIVOLUTION's Core parameter Relation !!!!!!!!!!!!!\n");
    printf ("If  2D  Mode:\n");
    printf ("WM_VALUE  0x00			//24fps  \n");
    printf ("WM_VALUE  0x20			//30fps  \n");
    printf ("WM_VALUE  0x30			//48fps  \n");
    printf ("WM_VALUE  0x40			//50fps  \n");
    printf ("WM_VALUE  0x50			//60fps  \n");
    printf ("WM_VALUE  0x60			//96fps  \n");
    printf ("WM_VALUE  0x70			//120fps  \n");
    printf ("If  3D  Mode:\n");
    printf ("WM_VALUE  0x81			//24FPS single eye   \n");
    printf ("WM_VALUE  0xA1			//30fps  \n");
    printf ("WM_VALUE  0xB1			//48fps  \n");
    printf ("WM_VALUE  0xD1			//60fps  \n");
    printf ("\n");
    printf ("\n");

    //1.	����Rate
    //OPB_WRITE��ADDRESS=0x0010��DATA=0x74010080  (80 for 24 fps)
    write_reg (fd, OPB_ADDR, 0x0010);		// write adr
    write_reg (fd, OPB_WDATA, OPB_FRAME_RATE);	// write adr 24fps
    printf ("Set Intopix's OPB Register  ---> Set Frame Rate(0x%x)\n", OPB_FRAME_RATE);
    // this can be read from FRAME_RATE
    // and convert to OPB's value
    // OPB_FRAME_RATE    0x74010080		// 24fps
    // OPB_FRAME_RATE	   0x74010092		// 30fps
    // OPB_FRAME_RATE    0x74010081		// 48fps
    // OPB_FRAME_RATE    0x740100A1       // 50fps
    // OPB_FRAME_RATE    0x740100A2		// 60fps
    // OPB_FRAME_RATE    0x740100B0		// 96fps
    // OPB_FRAME_RATE	   0x740100B2		// 120fps
    printf ("Please Check:  MICROM's Frame Rate and INTOPIX Register setting Relation !!!!!!!!!!!!!\n");
    printf ("OPB_FRAME_RATE    0x74010080	// 24fps\n");
    printf ("OPB_FRAME_RATE	  0x74010092	// 30fps\n");
    printf ("OPB_FRAME_RATE    0x74010081	// 48fps\n");
    printf ("OPB_FRAME_RATE    0x740100A1	// 50fps\n");
    printf ("OPB_FRAME_RATE    0x740100A2	// 60fps\n");
    printf ("OPB_FRAME_RATE    0x740100B0	// 96fps\n");
    printf ("OPB_FRAME_RATE	  0x740100B2	// 120fps \n");
    printf ("\n");
    printf ("\n");
    printf ("\n");

    // 2.	����PRP options
    // OPB_WRITE��ADDRESS=0x0010��DATA=0x73020000   (0000 for normal option)
    write_reg (fd, OPB_ADDR, 0x0010);				// write adr
    write_reg (fd, OPB_WDATA, INTOPIX_DECOPT);				// write adr
    printf ("Set Intopix's OPB Register  ---> DECOPT = 0x%x\n", INTOPIX_DECOPT);
    // this can be read from MODE_4K
    // and convert to OPB's value
    //micro	:	MODE_4K_Off = 0					// only decode 2K resolution
    //IMB 	:	INTOPIX_DECOPT	0x73020080		// Detect 4K,Drop 4K
    //micro	:	MODE_4K_Automatic = 1   		// automatically switch to 4K depending on the video material
    //IMB		:	INTOPIX_DECOPT	0x73020000		// Normal
    //micro	:	MODE_4K_Always = 2   		    // upscale any resolution to 4K
    //IMB		:	INTOPIX_DECOPT	0x73020100		// upscale the frame (e.g. from 2k to 4k)
    printf ("Please Check:  MICROM's 4K mode and INTOPIX Register setting Relation !!!!!!!!!!!!!\n");
    printf ("micro:	MODE_4K_Off 	  = 0			// only decode 2K resolution\n");
    printf ("IMB  :	INTOPIX_DECOPT	  = 0x73020080	// Detect 4K,Drop 4K\n");
    printf ("micro:	MODE_4K_Automatic = 1			// automatically switch to 4K depending on the video material\n");
    printf ("IMB  :	INTOPIX_DECOPT	  = 0x73020000	// Normal\n");
    printf ("micro:	MODE_4K_Always 	  = 2			// upscale any resolution to 4K\n");
    printf ("IMB  :	INTOPIX_DECOPT	  = 0x73020100	// upscale the frame ,e.g. from 2k to 4k\n");
    printf ("\n");
    printf ("\n");
    printf ("\n");

    // 3.	���� watermark mode
    // OPB_WRITE��ADDRESS=0x014C��DATA=0x00000002��ˮӡ����Ϊ�Զ�ģʽ��
    write_reg (fd, OPB_ADDR, 0x014C);				// write adr
    write_reg (fd, OPB_WDATA, 0x00000002);				// write adr
    printf ("Set Intopix's OPB Register  ---> Set WaterMark Mode to autimatically\n");

    // 4.	���� PRP mode
    //OPB_WRITE��ADDRESS=0x0000��DATA=0x00000010��PRP����Ϊplayģʽ��
    write_reg (fd, OPB_ADDR, 0x0000);				// write adr
    write_reg (fd, OPB_WDATA, 0x00000010);				// write adr
    printf ("Set Intopix's OPB Register  ---> Set PRP to Play mode\n");

    // 5.	���� POP mode
    // OPB_WRITE��ADDRESS=0x0100��DATA=0x00000012��POP����Ϊplay onceģʽ��
    write_reg (fd, OPB_ADDR, 0x0100);				// write adr
    write_reg (fd, OPB_WDATA, 0x00000012);				// write adr
    printf ("Set Intopix's OPB Register  ---> Set POP to Play Once mode\n");

    //******************************************************************************************************************************
    //						 Transfer Parameter to Auxilary FPGA
    //						 This operation can be placed in where of MICROM's API flow ???????????????
    //						 It must be set after all parameter have been set
    //******************************************************************************************************************************
    // aux_fpga_CTL
    // these register should be set after all registers have been set
    write_reg (fd, LBUS_ADDR, aux_fpga_CTL);   // write adr
    write_reg (fd, LBUS_WDATA, 0x1);						 // transfer part of register value to Auxilary FPGA (Spartan6)
    printf ("Transfer Parameter to Auxilary FPGA (Spartan6) !!!!\n");


    /*********************************************************************
     *  Regiseter set finish
     *********************************************************************/

    ///// HDMI
    write_reg (fd, LBUS_ADDR, PLAYBACK_CTL);				// write adr
    write_reg (fd, LBUS_WDATA, 0x01);				// write adr
    ///// HDMI

    printf ("HDMI mode set finished!\n");

  /**
   * close and quit.
   */
  close (fd);
  return 0;
}


