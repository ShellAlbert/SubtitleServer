/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/************************************************************************
*  File: virtex5_diag.c
*
*  Sample user-mode diagnostics application for accessing VIRTEX5
*  devices using WinDriver's API.
*************************************************************************/

#include <stdio.h>
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
#include "samples/shared/diag_lib.h"
#include "samples/shared/wdc_diag_lib.h"
#include "samples/shared/pci_regs.h"
#include "../virtex5_lib.h"
#include <windows.h> 
#include "../para.h"
/*************************************************************
  General definitions
 *************************************************************/





/* Error messages display */
#define VIRTEX5_ERR printf


/*************************************************************
  Global variables
 *************************************************************/
/* User's input command */
static CHAR gsInput[256];

/* --------------------------------------------------
    VIRTEX5 configuration registers information
   -------------------------------------------------- */
/* Configuration registers information array */
static const WDC_REG gVIRTEX5_CfgRegs[] = {
    { WDC_AD_CFG_SPACE, PCI_VID, WDC_SIZE_16, WDC_READ_WRITE, "VID",
        "Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_DID, WDC_SIZE_16, WDC_READ_WRITE, "DID",
        "Device ID" },
    { WDC_AD_CFG_SPACE, PCI_CR, WDC_SIZE_16, WDC_READ_WRITE, "CMD",
        "Command" },
    { WDC_AD_CFG_SPACE, PCI_SR, WDC_SIZE_16, WDC_READ_WRITE, "STS", "Status" },
    { WDC_AD_CFG_SPACE, PCI_REV, WDC_SIZE_32, WDC_READ_WRITE, "RID_CLCD",
        "Revision ID & Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCSC, WDC_SIZE_8, WDC_READ_WRITE, "SCC",
        "Sub Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CCBC, WDC_SIZE_8, WDC_READ_WRITE, "BCC",
        "Base Class Code" },
    { WDC_AD_CFG_SPACE, PCI_CLSR, WDC_SIZE_8, WDC_READ_WRITE, "CALN",
        "Cache Line Size" },
    { WDC_AD_CFG_SPACE, PCI_LTR, WDC_SIZE_8, WDC_READ_WRITE, "LAT",
        "Latency Timer" },
    { WDC_AD_CFG_SPACE, PCI_HDR, WDC_SIZE_8, WDC_READ_WRITE, "HDR",
        "Header Type" },
    { WDC_AD_CFG_SPACE, PCI_BISTR, WDC_SIZE_8, WDC_READ_WRITE, "BIST",
        "Built-in Self Test" },
    { WDC_AD_CFG_SPACE, PCI_BAR0, WDC_SIZE_32, WDC_READ_WRITE, "BADDR0",
        "Base Address 0" },
    { WDC_AD_CFG_SPACE, PCI_BAR1, WDC_SIZE_32, WDC_READ_WRITE, "BADDR1",
        "Base Address 1" },
    { WDC_AD_CFG_SPACE, PCI_BAR2, WDC_SIZE_32, WDC_READ_WRITE, "BADDR2",
        "Base Address 2" },
    { WDC_AD_CFG_SPACE, PCI_BAR3, WDC_SIZE_32, WDC_READ_WRITE, "BADDR3",
        "Base Address 3" },
    { WDC_AD_CFG_SPACE, PCI_BAR4, WDC_SIZE_32, WDC_READ_WRITE, "BADDR4",
        "Base Address 4" },
    { WDC_AD_CFG_SPACE, PCI_BAR5, WDC_SIZE_32, WDC_READ_WRITE, "BADDR5",
        "Base Address 5" },
    { WDC_AD_CFG_SPACE, PCI_CIS, WDC_SIZE_32, WDC_READ_WRITE, "CIS",
        "CardBus CIS Pointer" },
    { WDC_AD_CFG_SPACE, PCI_SVID, WDC_SIZE_16, WDC_READ_WRITE, "SVID",
        "Sub-system Vendor ID" },
    { WDC_AD_CFG_SPACE, PCI_SDID, WDC_SIZE_16, WDC_READ_WRITE, "SDID",
        "Sub-system Device ID" },
    { WDC_AD_CFG_SPACE, PCI_EROM, WDC_SIZE_32, WDC_READ_WRITE, "EROM",
        "Expansion ROM Base Address" },
    { WDC_AD_CFG_SPACE, PCI_CAP, WDC_SIZE_8, WDC_READ_WRITE, "NEW_CAP",
        "New Capabilities Pointer" },
    { WDC_AD_CFG_SPACE, PCI_ILR, WDC_SIZE_32, WDC_READ_WRITE, "INTLN",
        "Interrupt Line" },
    { WDC_AD_CFG_SPACE, PCI_IPR, WDC_SIZE_32, WDC_READ_WRITE, "INTPIN",
        "Interrupt Pin" },
    { WDC_AD_CFG_SPACE, PCI_MGR, WDC_SIZE_32, WDC_READ_WRITE, "MINGNT",
        "Minimum Required Burst Period" },
    { WDC_AD_CFG_SPACE, PCI_MLR, WDC_SIZE_32, WDC_READ_WRITE, "MAXLAT",
        "Maximum Latency" },
    };

#define VIRTEX5_CFG_REGS_NUM (sizeof(gVIRTEX5_CfgRegs)/sizeof(WDC_REG))

/* -----------------------------------------------
    VIRTEX5 run-time registers information
   ----------------------------------------------- */
/* Run-time registers information array */

typedef struct {
    VIRTEX5_DMA_HANDLE hDma;
    PVOID pBuf;
} DIAG_DMA, *PDIAG_DMA;

/*************************************************************
  Static functions prototypes
 *************************************************************/

/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId);
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot);
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot);
static void DeviceClose(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma);



/* ----------------------------------------------------
    Direct Memory Access (DMA)
   ---------------------------------------------------- */
static void DIAG_DMAClose(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma);
static void DMAOpen(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma);
static void DiagDmaIntHandler(WDC_DEVICE_HANDLE hDev,
    VIRTEX5_INT_RESULT *pIntResult);

static BOOL int_flag;
static BOOL DEBUG;



static void KEYACT(WDC_DEVICE_HANDLE hDev,UINT32 LIC_KEY_LSB,UINT32 LIC_KEY_MSB);




///////////////////////////////////////////////////////////


/*************************************************************
  Functions implementation
 *************************************************************/
int main(void)
{
	unsigned int i;
unsigned int j;
  unsigned int rdata;
    WDC_DEVICE_HANDLE hDev = NULL;
    DWORD dwStatus;
	DIAG_DMA dma;
    printf("\n");
    printf("VIRTEX5 diagnostic utility.\n");
    printf("Application accesses hardware using " WD_PROD_NAME ".\n");

    /* Initialize the VIRTEX5 library */
    dwStatus = VIRTEX5_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        VIRTEX5_ERR("virtex5_diag: Failed to initialize the VIRTEX5 library: "
            "%s", VIRTEX5_GetLastErr());
        return dwStatus;
    }

    /* Find and open a VIRTEX5 device (by default ID) */
    hDev = DeviceFindAndOpen(VIRTEX5_DEFAULT_VENDOR_ID,
        VIRTEX5_DEFAULT_DEVICE_ID);

    
	
	
	/* Display main diagnostics menu for communicating with the device */

  

	/* De-assert Initiator Reset */
   
	int_flag  = FALSE;
	DEBUG	  = FALSE;
	//DEBUG	  = TRUE;

    DMAOpen(hDev, &dma);
			
			
			
			
			
			
			

	
    printf("program finshed.......\n");


	DIAG_DMAClose(hDev, &dma);
    DeviceClose(hDev, NULL);


    dwStatus = VIRTEX5_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        VIRTEX5_ERR("virtex5_diag: Failed to uninit the VIRTEX5 library: %s",
            VIRTEX5_GetLastErr());
    }
    
    return dwStatus;
}


/* -----------------------------------------------
    Device find, open and close
   ----------------------------------------------- */
/* Find and open a VIRTEX5 device */
static WDC_DEVICE_HANDLE DeviceFindAndOpen(DWORD dwVendorId, DWORD dwDeviceId)
{
    WD_PCI_SLOT slot;
    
    if (!DeviceFind(dwVendorId, dwDeviceId, &slot))
        return NULL;

    return DeviceOpen(&slot);
}

/* Find a VIRTEX5 device */
static BOOL DeviceFind(DWORD dwVendorId, DWORD dwDeviceId, WD_PCI_SLOT *pSlot)
{
    DWORD dwStatus;
    DWORD i, dwNumDevices;
    WDC_PCI_SCAN_RESULT scanResult;

    if (!dwVendorId)
    {
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwVendorId,
            "Enter vendor ID", TRUE, 0, 0))
        {
            return FALSE;
        }

        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&dwDeviceId,
            "Enter device ID", TRUE, 0, 0))
        {
            return FALSE;
        }
    }

    BZERO(scanResult);
    dwStatus = WDC_PciScanDevices(dwVendorId, dwDeviceId, &scanResult);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        VIRTEX5_ERR("DeviceFind: Failed scanning the PCI bus.\n"
            "Error: 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return FALSE;
    }

    dwNumDevices = scanResult.dwNumDevices;
    if (!dwNumDevices)
    {
        VIRTEX5_ERR("No matching device was found for search criteria "
            "(Vendor ID 0x%lX, Device ID 0x%lX)\n", dwVendorId, dwDeviceId);
        return FALSE;
    }
    
    printf("\nFound %ld matching device(s) "
        "[Vendor ID 0x%lX%s, Device ID 0x%lX%s]:\n",
        dwNumDevices, dwVendorId, dwVendorId ? "" : " (ALL)",
        dwDeviceId, dwDeviceId ? "" : " (ALL)");
    
    for (i = 0; i < dwNumDevices; i++)
    {
        printf("\n%2ld. Vendor ID: 0x%lX, Device ID: 0x%lX\n", i + 1,
            scanResult.deviceId[i].dwVendorId,
            scanResult.deviceId[i].dwDeviceId);

        WDC_DIAG_PciDeviceInfoPrint(&scanResult.deviceSlot[i], FALSE);
    }
    printf("\n");

    if (dwNumDevices > 1)
    {
        sprintf(gsInput, "Select a device (1 - %ld): ", dwNumDevices);
        if (DIAG_INPUT_SUCCESS != DIAG_InputDWORD(&i, gsInput, FALSE, 1,
            dwNumDevices))
        {
            return FALSE;
        }
    }

    *pSlot = scanResult.deviceSlot[i - 1];

    return TRUE;
}

/* Open a handle to a VIRTEX5 device */
static WDC_DEVICE_HANDLE DeviceOpen(const WD_PCI_SLOT *pSlot)
{
    WDC_DEVICE_HANDLE hDev;
    DWORD dwStatus;
    WD_PCI_CARD_INFO deviceInfo;
    
    /* Retrieve the device's resources information */
    BZERO(deviceInfo);
    deviceInfo.pciSlot = *pSlot;
    dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        VIRTEX5_ERR("DeviceOpen: Failed retrieving the device's resources "
            "information.\nError 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        return NULL;
    }

    /* NOTE: You can modify the device's resources information here, if
       necessary (mainly the deviceInfo.Card.Items array or the items number -
       deviceInfo.Card.dwItems) in order to register only some of the resources
       or register only a portion of a specific address space, for example. */

    /* Open a handle to the device */
    hDev = VIRTEX5_DeviceOpen(&deviceInfo, NULL);
    if (!hDev)
    {
        VIRTEX5_ERR("DeviceOpen: Failed opening a handle to the device: %s",
            VIRTEX5_GetLastErr());
        return NULL;
    }

    return hDev;
}

/* Close handle to a VIRTEX5 device */
static void DeviceClose(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma)          
{
    if (!hDev)
        return;

     if (pDma)   
         DIAG_DMAClose(hDev, pDma);      
         
    if (!VIRTEX5_DeviceClose(hDev))
    {
        VIRTEX5_ERR("DeviceClose: Failed closing VIRTEX5 device: %s",
            VIRTEX5_GetLastErr());
    }
	else printf("close devic\n");
}



/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
/* DMA menu options */
enum {
    MENU_DMA_OPEN_CLOSE = 1,
    MENU_DMA_EXIT = DIAG_EXIT_MENU
};

static void DIAG_DMAClose(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma)
{
    DWORD dwStatus;

    if (!pDma)
        return;
    
    if (VIRTEX5_IntIsEnabled(hDev))
    {
        dwStatus = VIRTEX5_IntDisable(hDev);
        printf("DMA interrupts disable%s\n",
            (WD_STATUS_SUCCESS == dwStatus) ? "d" : " failed");
    }

    if (pDma->hDma)
    {
        VIRTEX5_DMAClose(pDma->hDma);
        printf("DMA closed (handle 0x%p)\n", pDma->hDma);
    }
    
    BZERO(*pDma);
}

/* Diagnostics interrupt handler routine */
static void DiagDmaIntHandler(WDC_DEVICE_HANDLE hDev,
    VIRTEX5_INT_RESULT *pIntResult)
{

	unsigned int k;

    if(DEBUG)  printf("\n###\nDMA %s based interrupt, recieved #%ld\n",
        pIntResult->fIsMessageBased ? "message" : "line",
        pIntResult->dwCounter);
    
    if(DEBUG) printf("Interrupt entered\n\n");
    
	int_flag = TRUE;

}


static void DMAOpen(WDC_DEVICE_HANDLE hDev, PDIAG_DMA pDma)
{
    DWORD dwStatus, dwOptions,  i;
    UINT32  dma_len;
    UINT32 LowerAddr;
	UINT32 LowerAddr_data;
    UINT32  RDATA;
	UINT32  INIT_DATA;
	UINT32  loop;
    UINT32   video_fileindex;
	UINT32   audio_fileindex;


	FILE			  *frame; 
	FILE			  *vframe;
	BOOL			   vframe_flag;

	UINT32				  header_len;
    UINT32				  file_len;
    UINT32				  pad_len;
    UINT32				  total_len;
    BYTE				  BDATA;
	UINT32				  k;

    char				  filename[64];
	FILE				  *keyfile;
	int					  num1,num2;
	UINT32				  INTOPIX_DECOPT;
	UINT32				  OPB_FRAME_RATE;
	UINT32				  TMP;

    //******************************************************************************************************************************
	//						 Register Setting Start
	//******************************************************************************************************************************
   
    //******************************************************************************************************************************
	//						 Soft Reset
	//						 Where should it be placed in the MICROM's Flow ?????
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf(" Soft reset the FPGA system \n");
	printf("***************************************************************************\n");
	printf("\n");
	   
	VIRTEX5_WriteReg32(hDev, RST_CTL, 0x01); // soft reset
	for(k=0;k<5;k++) 	printf("Soft Reseting.......\n");									// some delay for full Reset
	VIRTEX5_WriteReg32(hDev, RST_CTL, 0x00); // soft reset
	printf("Soft Reset Finished.......\n");


    //******************************************************************************************************************************
	//						 Debug Mode: it can be turn off when normal operation
	//						 Where should it be placed in the MICROM's Flow ?????
	//******************************************************************************************************************************
    // DEBUG:        bit0 			 fake_wm_en 						// enable Watermark always  
    //               bit1			 wm_logo_en							// enable LOGO of Watermark function
 
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("Debug Mode Set \n");
	printf("***************************************************************************\n");
	printf("\n");
	   
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, SOFT_DEBUG);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,0x03);	        
    printf("Debug Mode Set Finish:  Always Enable Watermark, Enable LOGO.......\n");
	printf("This step can be turn off to disable LOGO and enable automatic-control Watermak\n");


    //******************************************************************************************************************************
	//						 Key Activation
	//						 Where should it be placed in the MICROM's Flow ?????
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf(" INTOPIX's JPEG2000 Decoder Core's KEY Activation\n");
	printf("***************************************************************************\n");
	printf("\n");
	   
    printf("Key Activation Start.......\n");
	keyfile = fopen("D:/intopix_key.txt","rb");
	fscanf(keyfile,"%x %x",&num1,&num2);
	printf("%x\n%x\n",num1,num2);

	
	//KEYACT(hDev,num2,num1);   						// turn it off when it's not Production Version
	fclose(keyfile); 
    printf("Key Activation Finished.......\n");


 

	//******************************************************************************************************************************
	//						 Frate Rate Setting ,  
	//						 MICROM's setFrameRate of JPEG200 Decoder
	//             in this function, set
	//						 1) FRAME_RATE register
	//						 2) set frame_in_ddr_CTL register  ( normally frame_rate * 2)
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("Frame Rate Setting \n");
	printf("***************************************************************************\n");
	printf("\n");

	//	 FRAME_RATE
	//	 bit7:0     frame_rate
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, FRAME_RATE);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,frame_rate);	     
    TMP  = frame_rate  ; 
    printf("Set Frame Rate: %d\n",TMP);
 
	// frame_in_ddr_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, frame_in_ddr_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,frame_in_ddr);
    TMP = frame_in_ddr;
	printf("Set FRAME IN DDR: %d\n",TMP);	 
	 
	 
	//******************************************************************************************************************************
	//						 MODE_4K Setting ,  
	//						 MICROM's set4kMode of JPEG200 Decoder
	//             in this function, set
	//						 1) MODE_4K register
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000 4K MODE Select \n");
	printf("***************************************************************************\n");
	printf("\n");
	   
    // MODE 4K
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, MODE_4K);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,mode4k);
    TMP = mode4k;
    printf("Set 4K MODE: %d\n",TMP);	

 
    //******************************************************************************************************************************
	//						 LVDS timing / WIDTH/HEIGHT/LEFT_EYE/RIGHT_EYE Setting ,  
	//						 MICROM's setVideoMode of Video Ouput class
	//             in this function, set
	//						 1) set JPEG_WIDTH_HEIGHT
	//						 2) set LEFT/RIGHT_EYE control of 3D
	//						 3) set LVDS timing parameter
	//						 4) set MPEG_HEIGHT/WIDTH (tmp, later to consider where to place them )
	//		There are lot of Video mode, and lot of combination of those register setting 
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("Video Mode Seting \n");
	printf("***************************************************************************\n");
	printf("\n");
	   
    MODE_STR
    printf("\n");

    // JPEG_WIDTH_HEIGHT	 
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, JPEG_WIDTH_HEIGHT);   
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,jpeg_resolution);
    TMP = 	jpeg_resolution;
	//printf("Set JPEG_WIDTH_HEIGHT: %d\n",TMP);
    printf("Set JPEG WIDTH : %d\n",TMP & 0x0000ffff);
    printf("Set JPEG HEIGHT: %d\n",TMP >> 16);
    
    // left_eye_white_line_CTL
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, left_eye_white_line_CTL);   
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,left_eye_white_line);	
    TMP = left_eye_white_line;
	printf("Set 3D LEFT EYE: 0x%x\n",TMP);
	  
    // right_eye_white_line_CTL
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, right_eye_white_line_CTL);   
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,right_eye_white_line);
    TMP = 	right_eye_white_line;
	printf("Set 3D RIGHT EYE: 0x%x\n",TMP);
 
   // lvds module control setting
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_V_ACTIVE_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_V_ACTIVE);	
    TMP = 	P_V_ACTIVE;
	printf("Set P_V_ACTIVE: 0x%x\n",TMP);

	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_V_BACK_PORCH_CTL);   // write adr
	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_V_BACK_PORCH);	
	TMP = 	P_V_BACK_PORCH;
	printf("SET P_V_BACK_PORCH: 0x%x\n",TMP);

	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_V_FRONT_PORCH_CTL);   // write adr
	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_V_FRONT_PORCH);
	TMP = 	P_V_FRONT_PORCH;
	printf("SET P_V_FRONT_PORCH: 0x%x\n",TMP);

	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_V_TOTAL_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_V_TOTAL);
    TMP = 	P_V_TOTAL;
	printf("SET P_V_TOTAL: 0x%x\n",TMP);

	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_H_ACTIVE_CTL);   // write adr
	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_H_ACTIVE);	
	TMP = 	P_H_ACTIVE;
	printf("SET P_H_ACTIVE: 0x%x\n",TMP);
	
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_H_BACK_PORCH_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_H_BACK_PORCH);
    TMP = 	P_H_BACK_PORCH;
	printf("SET P_H_BACK_PORCH: 0x%x\n",TMP);
	
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_H_FRONT_PORCH_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_H_FRONT_PORCH);	
	TMP = 	P_H_FRONT_PORCH;
	printf("SET P_H_FRONT_PORCH: 0x%x\n",TMP);

	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, P_H_TOTAL_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,P_H_TOTAL);
	TMP = 	P_H_TOTAL;
	printf("SET P_H_TOTAL: 0x%x\n",TMP);
	
	 // mpeg_v_total_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, mpeg_v_total_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,mpeg_v_total);
    TMP = 	mpeg_v_total;
	printf("SET mpeg_v_total: %d\n",TMP);
   
    // mpeg_h_total_CTL  
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, mpeg_h_total_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,mpeg_h_total);
    TMP = 	mpeg_h_total;
	printf("SET mpeg_h_total: %d\n",TMP);
	
	
    //******************************************************************************************************************************
	//						 FPGA_VERSION
	//						 MICROM's ???  of  ???? classs
	//             in this function, set
	//						 1) read JPEG_WIDTH_HEIGHT
	//******************************************************************************************************************************
     printf("\n");
	 printf("\n");
	 printf("\n");
	 printf("***************************************************************************\n");
	 printf("FPGA Version \n");
	 printf("***************************************************************************\n");
	 printf("\n");
	   
    // FPGA_VERSION
   	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, FPGA_VERSION);   // write adr
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_ANC_RDATA);
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_RDATA);
	printf("Read FPGA Version: 0x%x \n",RDATA);
	   
	
	//******************************************************************************************************************************
	//						 Video Type select:  MPEG/JPEG/HDMI  
	//						 MICROM's ????????? of ??????????? class
	//             in this function, set
	//						 1) set Video Type
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf(" JPEG/MPEG/HEMI select \n");
	printf("***************************************************************************\n");
	printf("\n");
	// codestream_kind_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, codestream_kind_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,codestream_kind);
    TMP = 	codestream_kind;
	printf("SET codestream_kind: %d.......\n",TMP);
    printf("Set CodeStream Kind: .......\n");
	printf("CodeStream Kind: 1: JPEG2000,  2: MPEG2,  3: HDMI .......\n");
     
     
	//******************************************************************************************************************************
	//						 Set Video/Audio Start Delay   
	//						 MICROM's setStartDelay of MVC Decoder class, void setStartDelay(float delay);
    //             MVC Decoder is parent class of JPEG Decoder/PCM Decoder/ MPEG Decoder
    //
	//             in this function, set
	//						 1) set Video Delay for JPEG2000/MPEG
	//						 2) set Audio Delay for JPEG2000/MPEG
	//******************************************************************************************************************************
 
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000/MPEG2 Video/Audio Start Delay \n");
	printf("***************************************************************************\n");
	printf("\n");
   
    // jpeg_video_delay_num_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, jpeg_video_delay_num_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,jpeg_video_delay_num);
    TMP = 	jpeg_video_delay_num;
	printf("SET jpeg_video_delay_num: 0x%x.......\n",TMP);
	 
    // jpeg_audio_delay_num_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, jpeg_audio_delay_num_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,jpeg_audio_delay_num);
    TMP = 	jpeg_audio_delay_num;
	printf("SET jpeg_audio_delay_num: 0x%x.......\n",TMP);
		  
    // mpeg_video_delay_num_CTL  
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, mpeg_video_delay_num_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,mpeg_video_delay_num);
    TMP = 	mpeg_video_delay_num;
	printf("SET mpeg_video_delay_num: 0x%x.......\n",TMP);
	
    // mpeg_audio_delay_num_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, mpeg_audio_delay_num_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,mpeg_audio_delay_num);
    TMP = 	mpeg_audio_delay_num;
	printf("SET mpeg_audio_delay_num: 0x%x.......\n",TMP);
   
   
   
    //******************************************************************************************************************************
	//						 Master Slave IMB Board Select 
	//						 MICROM's setSyncSlave of PlaybackControl class, TMmRc setSyncSlave(bool enable);
	//
	//             in this function, set
	//						 1) set slave_sync_sel
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000 Master Slave Select \n");
	printf("***************************************************************************\n");
	printf("\n");
    // slave_sync_sel_CTL
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, slave_sync_sel_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,slave_sync_sel);
    TMP = 	slave_sync_sel;
	printf("SET slave_sync_sel: %x.......\n",TMP);
    printf("Master/Slave Select : .......\n");
	printf("Master : 0,   Slave : 1  .......\n");
  
   
    //******************************************************************************************************************************
	//						 TIME CODE   
	//						 MICROM's settimecode of Video Data Buffer class and gettimecode of VideoDecoderStatus
	//
	//******************************************************************************************************************************
     printf("\n");
	 printf("\n");
	 printf("\n");
	 printf("***************************************************************************\n");
	 printf("Time Code Function \n");
	 printf("***************************************************************************\n");
	 printf("\n");
    // timecode_CTL usage example
    // set
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, timecode_CTL);   // write adr
    VIRTEX5_WriteReg32(hDev, LBUS_WDATA,0x12345678);
	printf("Test: Set TIME CODE = 0x12345678 \n");
    // get
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, timecode_CTL);   // write adr
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_ANC_RDATA);
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_RDATA);
	printf("Test: Get TIME CODE = 0x%x \n",RDATA);
	   
	 
	
    //******************************************************************************************************************************
	//						 Audio output delay  
	//						 MICROM's SetOutputDelay of Audio Ouput class,TMmRc setOutputDelay(int32_t delay);
	//             in this function, set
	//						 1) set CTL register   (maybe leave it to default value)
	//						 2) set VALUE register
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000 Audio Output Delay Control \n");
	printf("***************************************************************************\n");
	printf("\n");
    // audio_setup_delay_CTL
    // VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_setup_delay_CTL);   // write adr
  	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,?????);
 
    // audio_setup_delay_VALUE
    // VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_setup_delay_VALUE);   // write adr
  	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,??????);
  	 
  	 
    //******************************************************************************************************************************
	//						 Audio Frequency of Input Stream
	//						 MICROM's setSampleFrequency of PCMDataBuffer/PCMDecoder class, TMmRc setSampleFrequency(uint32_t frequency);
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000 Audio Input Frequecny \n");
	printf("***************************************************************************\n");
	printf("\n");
    // audio_in_freq_CTL
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_in_freq_CTL);   // write adr
  	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_in_freq);
  	TMP = 	audio_in_freq;
	printf("SET Audio In Frequency: %d\n",TMP);

  	 
	//******************************************************************************************************************************
	//						 Audio Frequency of Output 
	//						 MICROM's setOutputFrequency of Audio Output class, TMmRc setOutputFrequency(uint32_t frequency);
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000 Audio Output Frequecny \n");
	printf("***************************************************************************\n");
	printf("\n");
    // audio_out_freq_CTL
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_out_freq_CTL);   // write adr
  	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_out_freq);
  	TMP = 	audio_out_freq;
	printf("SET Audio Output Frequency: %d\n",TMP);

   	 
    //******************************************************************************************************************************
	//						 Audio Channel number  
	//						 MICROM's setNumberOfChannels of PCMDecoder class, setNumberOfChannels(uint8_t numberOfchannels);
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000/MPEG Audio channel number \n");
	printf("***************************************************************************\n");
	printf("\n");
    // audio_chn_num_CTL
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_chn_num_CTL);   // write adr
  	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_chn_num);
  	TMP = 	audio_chn_num;
	printf("SET Audio Channel Number : %d\n",TMP);
		
  	 
	//******************************************************************************************************************************
	//						 Audio Channel Mapping 
	//						 MICROM's setChannelMapping of PCMDecoder class
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG2000/MPEG Audio Channel Mapping Control\n");
	printf("***************************************************************************\n");
	printf("\n");
	// audio_chn_map1_CTL
	// VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_chn_map1_CTL);   // write adr
  	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_chn_map1);
    // printf("Set Audio Channel MAP1 : .......\n");
    	 
    	 
	// audio_chn_map2_CTL
	// VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_chn_map2_CTL);   // write adr
	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_chn_map2);
	//	 printf("Set Audio Channel MAP2 : .......\n");


	//******************************************************************************************************************************
	//						 Audio Channel Mute 
	//						 MICROM's setChannelMute of Audio Ouput class
	//******************************************************************************************************************************
    printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("JPEG3000/MPEG Audio Channel MUTE control and HDMI AUDIO MAP && MUTE control\n");
	printf("***************************************************************************\n");
	printf("\n");
	// audio_chn_mute_CTL
	// VIRTEX5_WriteReg32(hDev, LBUS_ADDR, audio_chn_mute_CTL);   // write adr
	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,audio_chn_mute);
	// printf("Set Audio Channel MUTE : .......\n");

	// hdmi_map_mute_CTL
	// VIRTEX5_WriteReg32(hDev, LBUS_ADDR, hdmi_map_mute_CTL);   // write adr
	// VIRTEX5_WriteReg32(hDev, LBUS_WDATA,hdmi_map_mute);
	// printf("Set HDMI Channel MAP and MUTE : .......\n");

  

	//******************************************************************************************************************************
	//						 JPEG2000 DECODER OPB register SETTING
	//						 WaterMark Header Setting
	//							This operation can be placed in where of MICROM's API flow ???????????????
	//						 IT's must be setted after MODE_4K and FRAME_RATE have been set
	//******************************************************************************************************************************
	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("***************************************************************************\n");
	printf("Enter INTOPIX Core's OPB Register Setting and CIVOLUTON's Watermark Core Setting\n");
	printf("***************************************************************************\n");
	printf("\n");
	   
	// Read <MODE 4K> register and generate INTOPIX_DECOPT value
  	// micro	:	MODE_4K_Off = 0   			     	// only decode 2K resolution																	
	// IMB 		:	INTOPIX_DECOPT	0x73020080		// Detect 4K,Drop 4K 
	// micro	:	MODE_4K_Automatic = 1   		 	// automatically switch to 4K depending on the video material
	// IMB		:	INTOPIX_DECOPT	0x73020000		// Normal
	// micro	:	MODE_4K_Always = 2   		    	// upscale any resolution to 4K
	// IMB		:	INTOPIX_DECOPT	0x73020100		// upscale the frame (e.g. from 2k to 4k)
  
    VIRTEX5_WriteReg32(hDev, LBUS_ADDR, MODE_4K);   // write adr
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_ANC_RDATA);
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_RDATA);
	printf("Read MODE_4K: %x \n",RDATA);
	   
	if(RDATA==0)   INTOPIX_DECOPT =	0x73020080;
	if(RDATA==1)   INTOPIX_DECOPT =	0x73020000;
	if(RDATA==2)   INTOPIX_DECOPT =	0x73020100;
	  
    // Read <FRAME_RATE> register and generate OPB_FRAME_RATE and WM_HEADER
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, FRAME_RATE);   // write adr
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_ANC_RDATA);
	RDATA  =  VIRTEX5_ReadReg32(hDev, LBUS_RDATA);
	printf("Get FRAME_RATE: %d \n",RDATA);
			
	// Generate OPB_FRAME_RATE
	//#define  OPB_FRAME_RATE    0x74010080					// 24fps
 	//#define  OPB_FRAME_RATE	   0x74010092					// 30fps		
 	//#define  OPB_FRAME_RATE    0x74010081					// 48fps
 	//#define  OPB_FRAME_RATE    0x740100A1					// 50fps
 	//#define  OPB_FRAME_RATE    0x740100A2					// 60fps
 	//#define  OPB_FRAME_RATE    0x740100B0					// 96fps
 	//#define  OPB_FRAME_RATE	   0x740100B2					// 120fps
   
    if(RDATA==24)   OPB_FRAME_RATE  =  	0x74010080		;
    if(RDATA==30)   OPB_FRAME_RATE  =  	0x74010092		;	
	if(RDATA==48)   OPB_FRAME_RATE  =  	0x74010081		;
	if(RDATA==50)   OPB_FRAME_RATE  =  	0x740100A1		;
	if(RDATA==60)   OPB_FRAME_RATE  =  	0x740100A2		;
	if(RDATA==96)   OPB_FRAME_RATE  =  	0x740100B0		;
	if(RDATA==120)  OPB_FRAME_RATE  =  	0x740100B2		;


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
 			
 	if(EN_3D==0) {
 		if(RDATA== 24) 	TMP = 0x00;
 		if(RDATA== 30) 	TMP = 0x20;
 		if(RDATA== 48) 	TMP = 0x30;
 		if(RDATA== 50) 	TMP = 0x40;
 		if(RDATA== 60) 	TMP = 0x50;
 		if(RDATA== 96) 	TMP = 0x60;
 		if(RDATA== 120) TMP = 0x70;
 	}
 	else {
 		if(RDATA== 48) 	TMP = 0x81;
 		if(RDATA== 60) 	TMP = 0xA1;
 		if(RDATA== 96) 	TMP = 0xB1;
 		if(RDATA== 120) TMP = 0xD1;
 	}	 				 				 				 				 				 				 				 				
 				 				 				 				
	VIRTEX5_WriteReg32(hDev, LBUS_ADDR, WM_HEADER);   // write adr  	 
  	VIRTEX5_WriteReg32(hDev, LBUS_WDATA,TMP);    
    printf("Set CIVOLUTION Core's parameter (WM_VALUE): 0x%x,  According to FRAME RATE and 3D mode control\n", TMP);
        
    printf("Please Check:  MICROM's Frame Rate and CIVOLUTION's Core parameter Relation !!!!!!!!!!!!!\n"); 
	printf("If  2D  Mode:\n");
	printf("WM_VALUE  0x00			//24fps  \n");
 	printf("WM_VALUE  0x20			//30fps  \n");
 	printf("WM_VALUE  0x30			//48fps  \n");
 	printf("WM_VALUE  0x40			//50fps  \n");
 	printf("WM_VALUE  0x50			//60fps  \n");
 	printf("WM_VALUE  0x60			//96fps  \n");
 	printf("WM_VALUE  0x70			//120fps  \n");
 	printf("If  3D  Mode:\n");
  	printf("WM_VALUE  0x81			//24FPS single eye   \n");
 	printf("WM_VALUE  0xA1			//30fps  \n");
 	printf("WM_VALUE  0xB1			//48fps  \n");
 	printf("WM_VALUE  0xD1			//60fps  \n");
	printf("\n");
    printf("\n");



	//1.	设置Rate
    //OPB_WRITE，ADDRESS=0x0010，DATA=0x74010080  (80 for 24 fps)
	VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0010);		// write adr 
	VIRTEX5_WriteReg32(hDev, OPB_WDATA,OPB_FRAME_RATE);	// write adr 24fps	
	printf("Set Intopix's OPB Register  ---> Set Frame Rate(0x%x)\n",OPB_FRAME_RATE);			
	// this can be read from FRAME_RATE 
	// and convert to OPB's value
	// OPB_FRAME_RATE    0x74010080		// 24fps
 	// OPB_FRAME_RATE	   0x74010092		// 30fps		
 	// OPB_FRAME_RATE    0x74010081		// 48fps
 	// OPB_FRAME_RATE    0x740100A1       // 50fps
 	// OPB_FRAME_RATE    0x740100A2		// 60fps
 	// OPB_FRAME_RATE    0x740100B0		// 96fps
 	// OPB_FRAME_RATE	   0x740100B2		// 120fps	
    printf("Please Check:  MICROM's Frame Rate and INTOPIX Register setting Relation !!!!!!!!!!!!!\n"); 
    printf("OPB_FRAME_RATE    0x74010080	// 24fps\n");
 	printf("OPB_FRAME_RATE	  0x74010092	// 30fps\n");		
 	printf("OPB_FRAME_RATE    0x74010081	// 48fps\n");
 	printf("OPB_FRAME_RATE    0x740100A1	// 50fps\n");
 	printf("OPB_FRAME_RATE    0x740100A2	// 60fps\n");
 	printf("OPB_FRAME_RATE    0x740100B0	// 96fps\n");
 	printf("OPB_FRAME_RATE	  0x740100B2	// 120fps \n");
 	printf("\n");
	printf("\n");
	printf("\n");
     
																									  
																									
																											
     // 2.	设置PRP options
     // OPB_WRITE，ADDRESS=0x0010，DATA=0x73020000   (0000 for normal option)
	 VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0010);				// write adr
	 VIRTEX5_WriteReg32(hDev, OPB_WDATA,INTOPIX_DECOPT);				// write adr
	 printf("Set Intopix's OPB Register  ---> DECOPT = 0x%x\n",INTOPIX_DECOPT);		
	 // this can be read from MODE_4K
	 // and convert to OPB's value
	 //micro	:	MODE_4K_Off = 0					// only decode 2K resolution																	
	 //IMB 	:	INTOPIX_DECOPT	0x73020080		// Detect 4K,Drop 4K 
	 //micro	:	MODE_4K_Automatic = 1   		// automatically switch to 4K depending on the video material
	 //IMB		:	INTOPIX_DECOPT	0x73020000		// Normal
	 //micro	:	MODE_4K_Always = 2   		    // upscale any resolution to 4K
	 //IMB		:	INTOPIX_DECOPT	0x73020100		// upscale the frame (e.g. from 2k to 4k)
     printf("Please Check:  MICROM's 4K mode and INTOPIX Register setting Relation !!!!!!!!!!!!!\n"); 
	 printf("micro:	MODE_4K_Off 	  = 0			// only decode 2K resolution\n");																	
	 printf("IMB  :	INTOPIX_DECOPT	  = 0x73020080	// Detect 4K,Drop 4K\n"); 
	 printf("micro:	MODE_4K_Automatic = 1			// automatically switch to 4K depending on the video material\n");
	 printf("IMB  :	INTOPIX_DECOPT	  = 0x73020000	// Normal\n");
	 printf("micro:	MODE_4K_Always 	  = 2			// upscale any resolution to 4K\n");
	 printf("IMB  :	INTOPIX_DECOPT	  = 0x73020100	// upscale the frame ,e.g. from 2k to 4k\n");
	 printf("\n");
	 printf("\n");
	 printf("\n");
	  
  
	 // 3.	设置 watermark mode
	 // OPB_WRITE，ADDRESS=0x014C，DATA=0x00000002（水印设置为自动模式）
	 VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x014C);				// write adr
	 VIRTEX5_WriteReg32(hDev, OPB_WDATA,0x00000002);				// write adr
     printf("Set Intopix's OPB Register  ---> Set WaterMark Mode to autimatically\n");		

	 // 4.	设置 PRP mode
	 //OPB_WRITE，ADDRESS=0x0000，DATA=0x00000010（PRP设置为play模式）
	 VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0000);				// write adr
	 VIRTEX5_WriteReg32(hDev, OPB_WDATA,0x00000010);				// write adr
     printf("Set Intopix's OPB Register  ---> Set PRP to Play mode\n");	
 
	 // 5.	设置 POP mode
	 // OPB_WRITE，ADDRESS=0x0100，DATA=0x00000012（POP设置为play once模式）
	 VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0100);				// write adr
	 VIRTEX5_WriteReg32(hDev, OPB_WDATA,0x00000012);				// write adr
     printf("Set Intopix's OPB Register  ---> Set POP to Play Once mode\n");	



   

	//******************************************************************************************************************************
	//						 Transfer Parameter to Auxilary FPGA
	//						 This operation can be placed in where of MICROM's API flow ???????????????
	//						 It must be set after all parameter have been set
	//******************************************************************************************************************************
   // aux_fpga_CTL
   // these register should be set after all registers have been set
	  VIRTEX5_WriteReg32(hDev, LBUS_ADDR, aux_fpga_CTL);   // write adr
      VIRTEX5_WriteReg32(hDev, LBUS_WDATA,0x1);						 // transfer part of register value to Auxilary FPGA (Spartan6)
     printf("Transfer Parameter to Auxilary FPGA (Spartan6) !!!!\n");
	 

	
     printf("Register setting finished !!!!!!!!!!!!!!!!!\n");
     printf("Press Any Key to Continue !!!!!!!!!!!!!!!!!\n");

     scanf("%s",&TMP);

  

    /*********************************************************************
	 *  Regiseter set finish
	 *********************************************************************/



 
	dwOptions  |= DMA_TO_DEVICE;
	/* The BMD reference design does not support s/g DMA, so we use contiguous
     */
    dwOptions |= DMA_KERNEL_BUFFER_ALLOC;

    /* Open DMA handle */
    //dwStatus = VIRTEX5_DMAOpen(hDev, &pDma->pBuf, dwOptions,1024*1024*10, &pDma->hDma);
	dwStatus = VIRTEX5_DMAOpen(hDev, &pDma->pBuf, dwOptions,1024*1024*12, &pDma->hDma);

    if (WD_STATUS_SUCCESS != dwStatus)
    {
        printf("\nFailed to open DMA handle. Error 0x%lx - %s\n", dwStatus,
            Stat2Str(dwStatus));
        return;
    }
 


	//*************************************************************************************
	// Generate Pattern
	// Initialize the DMA buffer with user defined pattern
	//*************************************************************************************
	LowerAddr		= (UINT32)pDma->hDma->pDma->Page[0].pPhysicalAddr;

    // UpperAddr = (BYTE)((pDma->hDma->pDma->Page[0].pPhysicalAddr >> 32) & 0xFF);
    //dma_len = 36032 ;  // for DMA transfer byte counter

 
   //////////////////////////////////////////////////////////////////////////////////////////
   /////  fixed pattern
   //////////////////////////////////////////////////////////////////////////////////////////



  video_fileindex = VIDEO_START_INDEX;
  audio_fileindex = AUDIO_START_INDEX;

#ifdef VIDEO_ONLY
//#ifdef AUDIO_ONLY
    vframe_flag  = TRUE;			// TRUE : video   FALSE: audio
#else
	vframe_flag  = FALSE;			// TRUE : video   FALSE: audio
#endif
	
	


	for(loop=1;loop<24*2*60*120;loop++) {
     
	  if(vframe_flag==FALSE) {
		 // 48KHZ 24P
		  //sprintf(filename,"e:/AUDIO/48khz_16channel/%06d.wav",audio_fileindex);
		  //sprintf(filename,"e:/AUDIO/48khz_08channel/%06d.wav",fileindex);        
		  //sprintf(filename,"e:/AUDIO/48khz_06channel/%06d.wav",audio_fileindex);

		  // 96KHZ 24P
		  //sprintf(filename,"e:/AUDIO/96khz_16channel/%06d.wav",fileindex);
		  //sprintf(filename,"e:/AUDIO/96khz_08channel/%06d.wav",fileindex);
		  //sprintf(filename,"e:/AUDIO/96khz_06channel/%06d.wav",fileindex);
		
		  // 48KHZ HFR
		  //sprintf(filename,"e:/AUDIO/48khz_06channel_120f/%06d.wav",audio_fileindex);
		  //sprintf(filename,"e:/AUDIO/48khz_06channel_60f/%06d.wav",audio_fileindex);
		  //sprintf(filename,"e:/AUDIO/48khz_06channel_48f/%06d.wav",audio_fileindex);

		  // 96KHZ HFR
		  //sprintf(filename,"e:/AUDIO/96khz_06channel_120f/%06d.wav",fileindex);
		  //sprintf(filename,"e:/AUDIO/96khz_06channel_60f/%06d.wav",fileindex);
		  //sprintf(filename,"e:/AUDIO/96khz_06channel_48f/%06d.wav",fileindex);
	  
		  // JPEG2000 AUDIO 
		 sprintf(filename,"E:/diying3-xyz_wave_24fps/%06d.wav",audio_fileindex);
		 //sprintf(filename,"E:/diying3-xyz_wave_48fps/%06d.wav",audio_fileindex);
		 //sprintf(filename,"E:/diying3-xyz_wave_96fps/%06d.wav",audio_fileindex);
		 //sprintf(filename,"E:/diying3-xyz_wave_120fps/%06d.wav",audio_fileindex);

		
		  //sprintf(filename,"d:/avatar/AVATAR-3D_wave_48fps/%06d.wav",audio_fileindex);
		  //sprintf(filename,"E:/dieyingchongchong/audio/%06d.a",audio_fileindex);
		  //sprintf(filename,"e:/disney/audio/%06d.a",audio_fileindex);
	      //sprintf(filename,"e:/StEM_2048_858/StEM_2048_858_wave/%06d.wav",audio_fileindex);
		 
		  // MPEG2 AUDIO
	 	  //sprintf(filename,"e:/baishechuanshuo/wave/%06d.wav",audio_fileindex);
	
		  frame = fopen(filename,"rb");
      }  
	  else  {
       //*********************** JPEG2000 **********************/
	   //------ 2048x1080
       sprintf(filename,"E:/dieyingchongchong/j2c/%06d.j2c",video_fileindex/FRAME_DIVIDOR);	   	  //
	   //sprintf(filename,"E:/diying3-xyz_j2c_padding/%06d.j2c",video_fileindex/5);	   	  //
	    //sprintf(filename,"e:/disney/video/%06d.j2c",video_fileindex);

	    //sprintf(filename,"e:/hellboy2_2048_1080_padding/%06d.j2c",video_fileindex/5);
	   // sprintf(filename,"e:/doubleface.j2c",video_fileindex);

	   //Sprintf(filename,"D:/lixiaolong/j2c/%06dL.j2c",video_fileindex);
	   //sprintf(filename,"D:/lixiaolong/j2c/%06dR.j2c",(video_fileindex-1)/2);
	  
       //------ 2048x858  (from 3D Source)
	   //sprintf(filename,"e:/4K-IMB test/kalari_3d_24fps/%06dL.j2c",video_fileindex);	 
	    //sprintf(filename,"e:/4K-IMB test/kalari_3d_60fps/%06dL.j2c",video_fileindex);	
       //sprintf(filename,"e:/StEM_2048_858/StEM_2048_858_j2c_padding/%06d.j2c",video_fileindex);


	   //------ 1920x1080
	    //sprintf(filename,"e:/ShiQuanJiuMei_1920x1080_padding/%06d.j2c",video_fileindex);	 //HD		
       
	   //------ 1998x1080	 
	   //sprintf(filename,"e:/AVATAR/%07d.j2c",video_fileindex/FRAME_DIVIDOR);
         //sprintf(filename,"d:/avatar/AVATAR-3D_j2c_padding/%06dL.j2c",video_fileindex/FRAME_DIVIDOR);


       //------ 4096x1716 
        //sprintf(filename,"e:/4K-IMB test/UnEnc_24_fps_3Min_4k/%06d.j2c",video_fileindex); 
	   //sprintf(filename,"e:/4K-IMB test/UnEnc_48fps_5Min_4k/%06d.j2c",video_fileindex); 
	
        
       //******************* MPEG2 ******************//

	  //sprintf(filename,"e:/baishechuanshuo/mpg_padding/mpeg2_%06d.mpg",video_fileindex);   
	  //sprintf(filename,"E:/mpeg2_padding/mpeg2_%06d.mpg",video_fileindex);
	  //sprintf(filename,"E:/mpeg2_video1/mpg_padding/%06d.mpg",fileindex);
	  //sprintf(filename,"E:/mpeg2_padding/mpeg2_001000.mpg",fileindex);
	     frame = fopen(filename,"rb");
       }





	/********************************************************************************
	*   Construct Video Frame
	*********************************************************************************/
    header_len = 8;		// bytes

	fseek(frame,0,SEEK_SET);
	fseek(frame,0,SEEK_END);
	file_len = ftell(frame );
	fseek(frame,0,SEEK_SET);

	//printf("file length ==  %08x \n",file_len);


	if(file_len%8)  pad_len = 8 - (file_len % 8);
	else 	      pad_len =  0;
	
		
	//printf("pad_len ==  %x \n",pad_len);


    dma_len = header_len + file_len + pad_len ;  // for DMA transfer byte counter

	//printf("dma_len ==  %x \n",dma_len);

	
	// MAAK   
	((BYTE*)(pDma->pBuf))[0] = 0x4D;  
	((BYTE*)(pDma->pBuf))[1]  = 0x44;
	if(vframe_flag) 
	  ((BYTE*)(pDma->pBuf))[2]  = 0x49;  
    else
	  ((BYTE*)(pDma->pBuf))[2]  = 0x41;  

	((BYTE*)(pDma->pBuf))[3]  = 0x4B;  

   // copy Length to buffer
  	BDATA = file_len >> 24;
	((BYTE*)(pDma->pBuf))[4] = BDATA;  	
	BDATA = file_len >> 16;
	((BYTE*)(pDma->pBuf))[5] = BDATA;  
	BDATA = file_len >> 8;
	((BYTE*)(pDma->pBuf))[6] = BDATA;  
	BDATA = file_len >> 0;
	((BYTE*)(pDma->pBuf))[7] = BDATA;  

    // read file 
	fread( (BYTE*)(pDma->pBuf) + 8  ,file_len,1,frame);

    // 补8字节0 在最后, 总是4字节，不管是不是多余
	((BYTE*)(pDma->pBuf))[8 + file_len + 0]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 1]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 2]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 3]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 4]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 5]  = 0x00;
	((BYTE*)(pDma->pBuf))[8 + file_len + 6]  = 0x00;
	

	fclose(frame);

if (1) {
	int_flag = FALSE;
	if(DEBUG) printf("set int_flag to FALSE .................................................\n");

    //*************************************************************************************
	//	Prepare the device registers for DMA transfer 
	//*************************************************************************************
    
	/* Set lower 32bit of DMA address */
    VIRTEX5_WriteReg32(hDev, VIRTEX5_DMAADDR_OFFSET, LowerAddr);

    /* Set DMA Length */
    VIRTEX5_WriteReg32(hDev, VIRTEX5_DMALEN_OFFSET, dma_len);
 
	RDATA = VIRTEX5_ReadReg32(hDev, VIRTEX5_DMAADDR_OFFSET);
	if(DEBUG) printf("Read DMAADDR Register === %x", RDATA);

	RDATA = VIRTEX5_ReadReg32(hDev, VIRTEX5_DMALEN_OFFSET);
	if(DEBUG) printf("Read DMALEN Register === %x", RDATA);

	//*********************************************************************************
	//  Interrupt Enalbe
	//*********************************************************************************
    if (!VIRTEX5_IntIsEnabled(hDev))
        {
            dwStatus = VIRTEX5_IntEnable(hDev, DiagDmaIntHandler);

            if (WD_STATUS_SUCCESS != dwStatus)
            {
                printf("\nFailed enabling DMA interrupts. Error 0x%lx - %s\n",
                    dwStatus, Stat2Str(dwStatus));
                goto Error;
            }


            printf("\nDMA interrupts enabled\n");
     }
   
	
	//*********************************************************************************
	// Start DMA 
	//*********************************************************************************
    /* Start DMA */
    if(DEBUG) printf("Start DMA transfer\n");
    VIRTEX5_DMASyncCpu(pDma->hDma);
    /* Start DMA Transfer */
    VIRTEX5_WriteReg32(hDev, VIRTEX5_DMACTL_OFFSET,0x1);
	

  	/* Perform necessary cleanup before exiting the program */
    while (int_flag==FALSE) {
	};
    if(DEBUG) printf("detect int_flag to TRUE .................................................\n");
	
	// increase parameter for next DMA
	//dma_len   = dma_len + 8;
#ifdef  SHOW_LOG
	//printf("audio_fileindex= %d\n",audio_fileindex);
	printf("video_fileindex= %d\n",video_fileindex);
	//printf(".........\n");
	
#endif
	////////////////////////
   	// change
	if(loop==BUF_NUM)  {
	for(k=0;k<20;k++ )
	   printf(".............................. before play start\n");
		
		VIRTEX5_WriteReg32(hDev, LBUS_ADDR, PLAYBACK_CTL);				// write adr
	  VIRTEX5_WriteReg32(hDev, LBUS_WDATA,0x01);				// write adr
	printf("Play start\n");
     //VIRTEX5_WriteReg32(hDev, 0x1100, 0x1);				// write adr

	}

/*#ifdef VIDEO_ONLY
	  if(fileindex == 620) fileindex = 0;
	  else   fileindex = fileindex + 1;
#endif*/


#ifdef VIDEO_ONLY
	  if(video_fileindex == VIDEO_END_INDEX) video_fileindex = VIDEO_START_INDEX;
	  else   video_fileindex = video_fileindex + 1;
#endif


#ifdef AUDIO_ONLY
	  if(audio_fileindex == AUDIO_END_INDEX) audio_fileindex = AUDIO_START_INDEX;
	  else   audio_fileindex = audio_fileindex + 1;
#endif


#ifdef AV_ALL
	if(vframe_flag!=FALSE) {
	//		printf("audio_fileindex= %d\n",audio_fileindex);
	  //  printf("video_fileindex= %d\n",video_fileindex);

		if(video_fileindex == VIDEO_END_INDEX) video_fileindex = VIDEO_START_INDEX;
		else   video_fileindex = video_fileindex + 1;
    
		if(audio_fileindex == AUDIO_END_INDEX) audio_fileindex = AUDIO_START_INDEX;
		else   audio_fileindex = audio_fileindex + 1;

	
    }

#endif


#ifdef AV_ALL
	vframe_flag  = ~vframe_flag;				// Audio/Video
	//printf("phase3 ................\n");
#endif


} // if 0
 }  // for(loop)

   
    return;
 
Error:
    DIAG_DMAClose(hDev, pDma);
}



static void KEYACT(WDC_DEVICE_HANDLE hDev,UINT32 LIC_KEY_LSB,UINT32 LIC_KEY_MSB)
{
    DWORD dwStatus, dwOptions,  i;
    UINT32  dma_len;
    UINT32 LowerAddr;
	UINT32 LowerAddr_data;
    UINT32  RDATA;
	UINT32  INIT_DATA;
	UINT32  loop;
    UINT32   video_fileindex;
	UINT32   audio_fileindex;


	FILE			  *frame; 
	FILE			  *vframe;
	BOOL			   vframe_flag;

	UINT32				  header_len;
    UINT32				  file_len;
    UINT32				  pad_len;
    UINT32				  total_len;
    BYTE				  BDATA;
	UINT32   k;

    char			  filename[64];


	UINT32		TMP_TRIAL;
	char			tmp;





if(1) {
	//printf("typein any num to go on\n");
	//scanf("%s",&tmp);
 //读取 PRP_KEY_CSR寄存器
   VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0045);				// write adr

   RDATA = VIRTEX5_ReadReg32(hDev, OPB_ANC_RDATA);				// read ancillary data
   //printf("Read opb Ancillary Data = %x.......\n",RDATA);

   RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read sts
   //printf("Read opb status = %x.......\n",RDATA);
   
   while(RDATA==0) {
	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read sts untile the end
      //printf("Read opb status = %x.......\n",RDATA);
   }


   RDATA = VIRTEX5_ReadReg32(hDev, OPB_RDATA);				// read  data
   printf("Read opb PRP_KEY_CSR = %x.......\n",RDATA);
   
   
   
   //判断ACT。如果ACT=1，则IP CORE IS FULL_MODE；如果ACT=0，则进行流程图下一步。
   if(RDATA & 0x100) 
   	printf("IP CORE IS FULL_MODE\n");
   
   
   while( !(RDATA & 0x100) ){
	 		//判断KEY_ACT_TRIAL。如果KEY_ACT_TRIAL<4，则进行流程图下一步；否则IP CORE IS BLOCKED_MODE
   		if(RDATA & 0x100) {
   			   	printf("IP CORE IS FULL_MODE\n");
   		      break;
   		}
   		if( (RDATA & 0xF)>=0x4 ) {  
   			  printf("IP CORE IS BLOCK_MODE\n");	
   		    break;	  
   		}
   		
   		
  	   	  //IP CORE IS LIMITED_MODE
  	   	  printf("IP CORE IS LIMITED_MODE\n");   				
  	   	  //TMP_TRIAL   =  KEY _ACT_TRIAL;	
  	   	  TMP_TRIAL = (RDATA & 0xF) ; 
  	   	  printf("TMP_TRIAL = %x.......\n",TMP_TRIAL);

	
	  	   	printf("Check CORE KEY LOGIC IS READY ???\n");   		
			
			//printf("typein any num to go on\n");
			//scanf("%s",&tmp);
  	   	  //读取 PRP_KEY_CSR寄存器,判断CORE KEY LOGIC is READY or NOT 
  		 	  VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0045);				// write adr
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_ANC_RDATA);				// read ancillary data
  	   	  //printf("Read Ancillary Data = %x.......\n",RDATA);  	      
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read Status data
  	   	  //printf("Read opb Status = %x.......\n",RDATA);
  	   	  while(RDATA==0) {
  		 	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read Status data
  	      	//printf("Read opb Status = %x.......\n",RDATA);
  	   	  }
  	      
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_RDATA);				// read PRP_KEY_CSR data
  	   	  printf("Read PRP_KEY_CSR = %x.......\n",RDATA);
  	   	  
  	   	  
  	   	  //判断RDY。如果RDY=1，则进行流程图下一步；如果RDY=0，则重新读取PRP_KEY_CSR寄存器，直到RDY=1。
  	   	  
  	   	  while( !(RDATA&0x10) ){
  	   	  	    printf("Core Key Logic is no ready.......\n");
 	   	  		//printf("typein any num to go on\n");
				//scanf("%s",&tmp);

				VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0045);				// write adr
 		    	RDATA = VIRTEX5_ReadReg32(hDev, OPB_ANC_RDATA);				// read ancillary data
 		    	RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
 		    	while(RDATA==0) {
  			  		RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
  		    	}
			
		  			RDATA = VIRTEX5_ReadReg32(hDev, OPB_RDATA);				// read ancillary data
		  			printf("Read PRP_KEY_CSR = %x.......\n",RDATA);
  
		  }
 						
 						///// download key  				
 					printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");
					//printf("typein any num to go on\n");
					//scanf("%s",&tmp);

   					//WRITE (PRP_LIC_KEY_LSB,LIC_KEY_LSB)
   					VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0042);		// write adr 
					printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");
	  				VIRTEX5_WriteReg32(hDev, OPB_WDATA,/*0x49f79d1d*/LIC_KEY_LSB);	// write adr LIC_KEY_LSB
					printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");

					//printf("typein any num to go on\n");
					//scanf("%s",&tmp);

					//WRITE (PRP_LIC_KEY_MSB,LIC_KEY_MSB)
					VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0043);		// write adr 
	  				printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");
					VIRTEX5_WriteReg32(hDev, OPB_WDATA,/*0x47c7d90d*/LIC_KEY_MSB);	// write adr LIC_KEY_MSB
					printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");

					//printf("typein any num to go on\n");
					//scanf("%s",&tmp);

					//WRITE (PRP_KEY_ACTIVATE,0x00000000)
					VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0044);		// write adr 
	  				printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");
					VIRTEX5_WriteReg32(hDev, OPB_WDATA,0x00000000);	// write adr PRP_KEY_ACTIVATE
  					printf("CORE KEY LOGIC IS REDAY, DOWNLOAD KEY\n");
   				


  	   	  printf("Check KEY_ACT_TRIAL number\n"); 

		  //printf("typein any num to go on\n");
		  //scanf("%s",&tmp);

   				//读取 PRP_KEY_CSR寄存器
  		  VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0045);				// write adr
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_ANC_RDATA);				// read ancillary data
  	   	  //printf("Read Ancillary Data = %x.......\n",RDATA);
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
  	   	  //printf("Read Status = %x.......\n",RDATA);
  	   	  while(RDATA==0) {
  		 	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
  	      	//printf("Read Status = %x.......\n",RDATA);
  	   	  }
  	   	  RDATA = VIRTEX5_ReadReg32(hDev, OPB_RDATA);				// read ancillary data
  	   	  printf("Read PRP_KEY_CSR = %x.......\n",RDATA);
  	   	  
  	   	  
  	   	 
  	   	  //KEY_ACT_TRIAL =1 or  KEY_ACT_TRIAL != TMP_TRIAL
  	   	  while(  !(   ((RDATA & 0xF)==0x0) || ((RDATA & 0xF)!=TMP_TRIAL)     )  ){
  	   		//printf("typein any num to go on\n");
			//scanf("%s",&tmp);

			VIRTEX5_WriteReg32(hDev, OPB_ADDR, 0x0045);				// write adr
  	   	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_ANC_RDATA);				// read ancillary data
  	   	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
  	   	  	while(RDATA==0) {
  		 	  		RDATA = VIRTEX5_ReadReg32(hDev, OPB_STS);				// read ancillary data
  	      	}
  	   	  	RDATA = VIRTEX5_ReadReg32(hDev, OPB_RDATA);				// read ancillary data
  	   	  	printf("Read PRP_KEY_CSR = %x.......\n",RDATA);
  	   		}
   				
   		 printf("Return to check ACT \n"); 
   			   		
   	}


	if(RDATA & 0x100) {
   	printf("IP CORE IS FULL_MODE\n");
	printf("IP CORE IS FULL_MODE\n");
	}
	scanf("%s",&tmp);
   	
}  // if(0)

}