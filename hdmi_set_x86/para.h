
//#define SHOW_LOG
//-------------------------------------------------------------
//		Play Control
//-------------------------------------------------------------

//#define AUDIO_ONLY
//#define VIDEO_ONLY
#define AV_ALL

#define BUF_NUM    4			// define the number of buffered frame before play start
#define AUDIO_START_INDEX   0
#define VIDEO_START_INDEX   0

#define FRAME_DIVIDOR       1
#define FRAME_NUMBER        5232//9167
#define AUDIO_END_INDEX     5231  //FRAME_NUMBER*FRAME_DIVIDOR  - 1    //3000*2 - 1

#define EN_3D 						0
//#define 3D_MODE 						1

//-------------------------------------------------------------
//		Register Declaration
//-------------------------------------------------------------

///////////////////////////////////// OPB Comunication ////////////////////////////////////
#define OPB_ADDR     	 0x1400						// Address that appear on OPB Bus
//////////////////////////////////// General Purpose //////////////////////////////////////
#define RST_CTL		 0x1800
#define SECOND  		 60*1

//////////////////////////////////// Local Bus Comunication ///////////////////////////////
#define LBUS_ADDR     	 0x1c00						// Address that appear on OPB Bus

//////////////////////////////////// LocalBus Registers ////////////////////////////////////

//******************* PlayBack Control
#define  PLAYBACK_CTL   				0x00						// Start to Play
#define  PLAYBACK_STS		  			0x04

//******************* Video
#define  WM_HEADER 					0x08						// INTOPIX Watermark Header parameter

#define  FRAME_RATE					0x10						// Video Source Parameter 1
//#define  frame_rate    48					// 48fps
#define  frame_rate    96					// 96fps

#define  JPEG_WIDTH_HEIGHT				0x14						// Video Source Parameter 2

#define  MODE_4K						0x18
// default to 1,these value determine the value of INTOPIX_DECOPT
//#define mode4k						0x00
#define mode4k						0x01
//#define mode4k						0x02

// micro	:	MODE_4K_Off = 0   			     	// only decode 2K resolution
// IMB 		:	INTOPIX_DECOPT	0x73020080		// Detect 4K,Drop 4K
// micro	:	MODE_4K_Automatic = 1   		 	// automatically switch to 4K depending on the video material
// IMB		:	INTOPIX_DECOPT	0x73020000		// Normal
// micro	:	MODE_4K_Always = 2   		    	// upscale any resolution to 4K
// IMB		:	INTOPIX_DECOPT	0x73020100		// upscale the frame (e.g. from 2k to 4k)

#define left_eye_white_line_CTL		0x28    //250, 1998/2*25%

#define right_eye_white_line_CTL		0x2c    //749, 1998/2*75%

//******************************* LVDS Timing
// set below register according to VIDEO_MODE
#define P_H_ACTIVE_CTL       		0x30
#define P_H_BACK_PORCH_CTL   		0x34
#define P_H_FRONT_PORCH_CTL  		0x38
#define P_H_TOTAL_CTL        		0x3c
#define P_V_ACTIVE_CTL       		0x40
#define P_V_BACK_PORCH_CTL   		0x44
#define P_V_FRONT_PORCH_CTL  		0x48
#define P_V_TOTAL_CTL        		0x4c

///// HDMI
#define mpeg_v_total_CTL       	0x54
#define mpeg_h_total_CTL       	0x58
#define mpeg_v_total           0x438  //1080

////////////////////////////////////// MISC /////////////////////////////

#define FPGA_VERSION			  0x5c

///// HDMI 
#define codestream_kind_CTL    	0x60
#define codestream_kind      0x02 //0x01: JPEG2000, 0x02: MPEG2

#define jpeg_video_delay_num_CTL 0x70
#define jpeg_video_delay_num 	0x3B9ACA0 + 0x17D7840/2

#define jpeg_audio_delay_num_CTL 0x74
#define jpeg_audio_delay_num 	0x00

#define mpeg_video_delay_num_CTL 0x78
#define mpeg_video_delay_num 0

#define mpeg_audio_delay_num_CTL 0x7c
#define mpeg_audio_delay_num 0x3B9ACA0/3

#define frame_in_ddr_CTL 				0x80
#define frame_in_ddr         2*frame_rate

#define slave_sync_sel_CTL 			0x90
#define slave_sync_sel   0  //0: master, 1: slave
#define aux_fpga_CTL							0x94
#define aux_fpga			0x01

#define timecode_CTL							0x9c

///////////////////////////////////////// AUDIO //////////////////////////
#define audio_setup_delay_CTL  	0xa0
//set mode , default to 0

#define audio_setup_delay_VALUE  0xa4
#define  audio_setup_delay		0x0

#define audio_in_freq_CTL				0xa8
#define  audio_in_freq				48000
//#define  audio_in_freq			96000

#define audio_out_freq_CTL     	0xac
#define  audio_out_freq				48000
//#define  audio_out_freq			96000

#define audio_chn_num_CTL		 		0xb0
#define	 audio_chn_num				6
//#define	 audio_chn_num				8
//#define	 audio_chn_num				16

#define audio_chn_map1_CTL				0xb4
#define audio_chn_map1        ????

#define audio_chn_map2_CTL				0xb8
#define audio_chn_map2        ????

#define audio_chn_mute_CTL				0xbc
#define audio_chn_mute        ????

#define hdmi_map_mute_CTL				0xc0
#define hdmi_map_mute					????

////////////////////////////////////////// DEBUG /////////////////////////
#define  SOFT_DEBUG		        0xf0

////////////////////////////////////////////////////////////////////////////////////////////////////////
//						Video Mode Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define mode_2048_1080_2400_P
//#define mode_2048_1080_3000_P
//#define mode_2048_1080_4800_P
//#define mode_2048_1080_5000_P 
//#define mode_2048_1080_6000_P
#define mode_2048_1080_9600_P
//#define mode_2048_1080_12000_P

//#define mode_2048_858_2400_P
//#define mode_2048_858_3000_P
//#define mode_2048_858_4800_P
//#define mode_2048_858_5000_P
//#define mode_2048_858_6000_P
//#define mode_2048_858_9600_P
//#define mode_2048_858_12000_P

//#define mode_1998_1080_2400_P
//#define mode_1998_1080_3000_P
//#define mode_1998_1080_4800_P
//#define mode_1998_1080_5000_P
//#define mode_1998_1080_6000_P
//#define mode_1998_1080_9600_P
//#define mode_1998_1080_12000_P

//#define mode_1920_1080_2400_P
//#define mode_1920_1080_3000_P
//#define mode_1920_1080_4800_P
//#define mode_1920_1080_5000_P
//#define mode_1920_1080_6000_P
//#define mode_1920_1080_9600_P
//#define mode_1920_1080_12000_P

//#define mode_3840_2160_2400_P
//#define mode_3840_2160_3000_P

//#define mode_3996_2160_2400_P
//#define mode_3996_2160_3000_P

//#define mode_4096_1716_2400_P    
//#define mode_4096_1716_3000_P

//#define mode_4096_2160_2400_P    
//#define mode_4096_2160_3000_P

//************************************************

#ifdef mode_2048_1080_2400_P
#define P_V_TOTAL            0x9C4  +0x1//2500
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_1080_2400_P\n");
#endif

#ifdef mode_2048_1080_3000_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_1080_3000_P\n");
#endif

#ifdef mode_2048_1080_4800_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_1080_4800_P\n");
#endif

#ifdef mode_2048_1080_5000_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_1080_5000_P\n");
#endif

#ifdef mode_2048_1080_6000_P
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_1080_6000_P\n");
#endif

#ifdef mode_2048_1080_9600_P
#define P_V_TOTAL            0x465  + 0x1//1125
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_1080_9600_P\n");
#endif

#ifdef mode_2048_1080_12000_P
#define P_V_TOTAL            0x442  + 0x1//1090
#define jpeg_resolution			 0x04380800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_1080_12000_P\n");
#endif

#ifdef mode_2048_858_2400_P
#define P_V_TOTAL            0x9C4  + 0x1//2500
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_858_2400_P\n");
#endif

#ifdef mode_2048_858_3000_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_858_3000_P\n");
#endif

#ifdef mode_2048_858_4800_P
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_858_4800_P\n");
#endif

#ifdef mode_2048_858_5000_P
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf("mode_2048_858_5000_P\n");
#endif

#ifdef mode_2048_858_6000_P	
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_858_6000_P\n");
#endif

#ifdef mode_2048_858_9600_P
#define P_V_TOTAL            0x465  //1125
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_858_9600_P\n");
#endif

#ifdef mode_2048_858_12000_P
#define P_V_TOTAL            0x442 + 0x1 //1090
#define jpeg_resolution			 0x035A0800
#define left_eye_white_line	 0x200	//512,2048*0.25
#define MODE_STR printf(" mode_2048_858_12000_P\n");
#endif

#ifdef mode_1998_1080_2400_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_2400_P\n");
#endif

#ifdef mode_1998_1080_3000_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_3000_P\n");
#endif

#ifdef mode_1998_1080_4800_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_4800_P\n");
#endif

#ifdef mode_1998_1080_5000_P
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_5000_P\n");
#endif

#ifdef mode_1998_1080_6000_P
#define P_V_TOTAL            0x5DC  //1500
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_6000_P\n");
#endif

#ifdef mode_1998_1080_9600_P
#define P_V_TOTAL            0x465  + 0x1//1125
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_9600_P\n");
#endif

#ifdef mode_1998_1080_12000_P
#define P_V_TOTAL            0x442  + 0x1//1090
#define jpeg_resolution			 0x043807ce
#define left_eye_white_line	 0x1F4	//500,1998*0.25
#define MODE_STR printf(" mode_1998_1080_12000_P\n");
#endif

#ifdef mode_1920_1080_2400_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf(" mode_1920_1080_2400_P\n");
#endif

#ifdef mode_1920_1080_3000_P
#define P_V_TOTAL            0x9C4  //2500
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf(" 		mode_1920_1080_3000_P\n");
#endif

#ifdef mode_1920_1080_4800_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf(" mode_1920_1080_4800_P\n");

#endif

#ifdef mode_1920_1080_5000_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf(" mode_1920_1080_5000_P\n");
#endif

#ifdef mode_1920_1080_6000_P
#define P_V_TOTAL            0x5DC  + 0x1//1500
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf("mode_1920_1080_6000_P\n");
#endif

#ifdef mode_1920_1080_9600_P
#define P_V_TOTAL            0x465  + 0x1//1125
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf("mode_1920_1080_9600_P\n");
#endif

#ifdef mode_1920_1080_12000_P
#define P_V_TOTAL            0x442  + 0x1//1090
#define jpeg_resolution			 0x04380780
#define left_eye_white_line	 0x1E0	//480,1920*0.25
#define MODE_STR printf(" mode_1920_1080_12000_P\n");

#endif

#ifdef mode_3840_2160_2400_P
#define P_V_TOTAL            0x8CA  //2250
#define jpeg_resolution			 0x08700f00
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf("mode_1920_1080_12000_P\n");

#endif

#ifdef mode_3840_2160_3000_P
#define P_V_TOTAL            0x882  //2178
#define jpeg_resolution			 0x08700f00
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf(" mode_3840_2160_3000_P\n");
#endif

#ifdef mode_3996_2160_2400_P
#define P_V_TOTAL            0x8CA  //2250
#define jpeg_resolution			 0x08700f9c
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf("mode_3996_2160_2400_P\n");

#endif

#ifdef mode_3996_2160_3000_P
#define P_V_TOTAL            0x882  //2178
#define jpeg_resolution			 0x08700f9c
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf("mode_3996_2160_3000_P\n");

#endif

#ifdef mode_4096_1716_2400_P
#define P_V_TOTAL            0x8CA  //2250
#define jpeg_resolution			 0x06b41000
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf(" mode_4096_1716_2400_P\n");

#endif

#ifdef mode_4096_1716_3000_P
#define P_V_TOTAL            0x882  //2178
#define jpeg_resolution			 0x06b41000
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf("mode_4096_1716_3000_P\n");
#endif

#ifdef mode_4096_2160_2400_P
#define P_V_TOTAL            0x8CA  //2250
#define jpeg_resolution			 0x08701000
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf("mode_4096_2160_2400_P\n");
#endif

#ifdef mode_4096_2160_3000_P
#define P_V_TOTAL            0x882  //2178
#define jpeg_resolution			 0x08701000
#define left_eye_white_line	 0x00	//512,2048*0.25
#define MODE_STR printf(" mode_4096_2160_3000_P\n");
#endif