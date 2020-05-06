/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/************************************************************************
*  File: virtex5_lib.c
*
*  Library for accessing VIRTEX5 devices.
*  The code accesses hardware using WinDriver's WDC library.
*************************************************************************/

#if defined (__KERNEL__)
    #include "kpstdlib.h"
#else
#include <stdio.h>
#include <stdarg.h>
#endif
#include "wdc_defs.h"
#include "utils.h"
#include "status_strings.h"
#include "virtex5_lib.h"

/*************************************************************
  Internal definitions
 *************************************************************/
/* WinDriver license registration string */
/* TODO: When using a registered WinDriver version, make sure the license string
         below is your specific WinDriver license registration string and set 
 * the driver name to your driver's name */
#define VIRTEX5_DEFAULT_LICENSE_STRING "12345abcde12345.abcde"
#define VIRTEX5_DEFAULT_DRIVER_NAME "windrvr6"

/* VIRTEX5 device information struct */
typedef struct {
    VIRTEX5_INT_HANDLER funcDiagIntHandler;
    VIRTEX5_EVENT_HANDLER funcDiagEventHandler;
    VIRTEX5_DMA_HANDLE hDma;
    PVOID pBuf;
    BOOL fIsRead;
    UINT32 u32Pattern;
    DWORD dwTotalCount;
} VIRTEX5_DEV_CTX, *PVIRTEX5_DEV_CTX;
/* TODO: You can add fields to store additional device-specific information */

static CHAR gsVIRTEX5_LastErr[256];

/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/
#if !defined(__KERNEL__)
static BOOL DeviceValidate(const PWDC_DEVICE pDev);
#endif
static void DLLCALLCONV VIRTEX5_IntHandler(PVOID pData);
static void VIRTEX5_EventHandler(WD_EVENT *pEvent, PVOID pData);
static void ErrLog(const CHAR *sFormat, ...);
static void TraceLog(const CHAR *sFormat, ...);

static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        ErrLog("%s: NULL device %s\n", sFunc, !pDev ? "handle" : "context");
        return FALSE;
    }

    return TRUE;
}

/*************************************************************
  Functions implementation
 *************************************************************/
/* -----------------------------------------------
    VIRTEX5 and WDC library initialize/uninit
   ----------------------------------------------- */
DWORD VIRTEX5_LibInit(void)
{
    DWORD dwStatus;
 
#if defined(WD_DRIVER_NAME_CHANGE)
    /* Set the driver name */
    if (!WD_DriverName(VIRTEX5_DEFAULT_DRIVER_NAME))
    {
        ErrLog("Failed to set the driver name for WDC library.\n");
        return WD_SYSTEM_INTERNAL_ERROR;
    }
#endif

    /* Set WDC library's debug options
     * (default: level TRACE, output to Debug Monitor) */
    dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize debug options for WDC library.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    /* Open a handle to the driver and initialize the WDC library */
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT,
        VIRTEX5_DEFAULT_LICENSE_STRING);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    return WD_STATUS_SUCCESS;
}

DWORD VIRTEX5_LibUninit(void)
{
    DWORD dwStatus;

    /* Uninit the WDC library and close the handle to WinDriver */
    dwStatus = WDC_DriverClose();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to uninit the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

//#if !defined(__KERNEL__)
/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
WDC_DEVICE_HANDLE VIRTEX5_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo,
    char *kp_name)
{
    DWORD dwStatus;
    PVIRTEX5_DEV_CTX pDevCtx = NULL;
    WDC_DEVICE_HANDLE hDev = NULL;

    /* Validate arguments */
    if (!pDeviceInfo)
    {
        ErrLog("VIRTEX5_DeviceOpen: Error - NULL device information struct "
            "pointer\n");
        return NULL;
    }

    /* Allocate memory for the VIRTEX5 device context */
    pDevCtx = (PVIRTEX5_DEV_CTX)calloc(1, sizeof(VIRTEX5_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("Failed allocating memory for VIRTEX5 device context\n");
        return NULL;
    }

    /* Open a Kernel PlugIn WDC device handle */
    dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx, NULL,
        kp_name && *kp_name ? kp_name : KP_VRTX5_DRIVER_NAME, &hDev);
    
    /* If failed, try opening a WDC device handle without using Kernel PlugIn */
    if(dwStatus != WD_STATUS_SUCCESS)
    {
        dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx, NULL, NULL,
            NULL);
    }
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed opening a WDC device handle. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        goto Error;
    }

    /* Validate device information */
    if (!DeviceValidate((PWDC_DEVICE)hDev))
        goto Error;

    /* Return handle to the new device */
    TraceLog("VIRTEX5_DeviceOpen: Opened a VIRTEX5 device (handle 0x%p)\n"
        "Device is %s using a Kernel PlugIn driver (%s)\n", hDev,
        (WDC_IS_KP(hDev))? "" : "not" , KP_VRTX5_DRIVER_NAME);
    return hDev;

Error:    
    if (hDev)
        VIRTEX5_DeviceClose(hDev);
    else
        free(pDevCtx);
    
    return NULL;
}

BOOL VIRTEX5_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PVIRTEX5_DEV_CTX pDevCtx;
    
    TraceLog("VIRTEX5_DeviceClose entered. Device handle: 0x%p\n", hDev);

    if (!hDev)
    {
        ErrLog("VIRTEX5_DeviceClose: Error - NULL device handle\n");
        return FALSE;
    }

    pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(pDev);
    
    /* Disable interrupts */
    if (WDC_IntIsEnabled(hDev))
    {
        dwStatus = VIRTEX5_IntDisable(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n", dwStatus,
                Stat2Str(dwStatus));
        }
    }

    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx - %s\n",
            hDev, dwStatus, Stat2Str(dwStatus));
    }

    /* Free VIRTEX5 device context memory */
    if (pDevCtx)
        free(pDevCtx);
    
    return (WD_STATUS_SUCCESS == dwStatus);
}

static BOOL DeviceValidate(const PWDC_DEVICE pDev)
{
    DWORD i, dwNumAddrSpaces = pDev->dwNumAddrSpaces;

    /* TODO: You can modify the implementation of this function in order to
             verify that the device has all expected resources. */
    
    /* Verify that the device has at least one active address space */
    for (i = 0; i < dwNumAddrSpaces; i++)
    {
        if (WDC_AddrSpaceIsActive(pDev, i))
            return TRUE;
    }
    
    ErrLog("Device does not have any active memory or I/O address spaces\n");
    return FALSE;
}

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
static void DLLCALLCONV VIRTEX5_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    PVIRTEX5_DEV_CTX pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(pDev);
    VIRTEX5_INT_RESULT intResult;

    BZERO(intResult);
    intResult.dwCounter = pDev->Int.dwCounter;
    intResult.dwLost = pDev->Int.dwLost;
    intResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;

    intResult.fIsMessageBased =
        (WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE ||
        WDC_GET_ENABLED_INT_TYPE(pDev) == INTERRUPT_MESSAGE_X) ?
        TRUE : FALSE;
    intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev);

    intResult.fIsRead = pDevCtx->fIsRead;
    intResult.pBuf = pDevCtx->pBuf;
    intResult.u32Pattern = pDevCtx->u32Pattern;
    intResult.dwTotalCount = pDevCtx->dwTotalCount;
    
    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &intResult);
}

DWORD VIRTEX5_IntEnable(WDC_DEVICE_HANDLE hDev,
    VIRTEX5_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PVIRTEX5_DEV_CTX pDevCtx;
	WDC_ADDR_DESC *pAddrDesc;
    WD_TRANSFER *pTrans;

    TraceLog("VIRTEX5_IntEnable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "VIRTEX5_IntEnable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(pDev);

    /* Check if interrupts are already enabled */
    if (WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already enabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* Define the number of interrupt transfer commands to use */
    #define NUM_TRANS_CMDS 1

    /* Allocate memory for the interrupt transfer commands */
    pTrans = (WD_TRANSFER*)calloc(NUM_TRANS_CMDS, sizeof(WD_TRANSFER));
    if (!pTrans)
    {
        ErrLog("Failed allocating memory for interrupt transfer commands\n");
        return WD_INSUFFICIENT_RESOURCES;
    }

    /* Prepare the interrupt transfer commands */
    /* The transfer commands will be executed by WinDriver in the kernel
      for each interrupt that is received */

    /* #1: Write to the Register0 register */
    pAddrDesc = &pDev->pAddrDesc[AD_PCI_BAR0];
    pTrans[0].dwPort = pAddrDesc->kptAddr + VIRTEX5_DMAINT_OFFSET;				//0x1010;
    pTrans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
    pTrans[0].Data.Dword = 0x1;				// legacy int clear bit


    /* Store the diag interrupt handler routine, which will be executed by
       VIRTEX5_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;
    
    /* Enable the interrupts */
   dwStatus = WDC_IntEnable(hDev, pTrans, NUM_TRANS_CMDS, 0, VIRTEX5_IntHandler,(PVOID)pDev, WDC_IS_KP(hDev));
   //  dwStatus = WDC_IntEnable(hDev, NULL, 0, 0, VIRTEX5_IntHandler,(PVOID)pDev, WDC_IS_KP(hDev)); 
	
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    TraceLog("VIRTEX5_IntEnable: Interrupts enabled\n");

    return WD_STATUS_SUCCESS;
}

DWORD VIRTEX5_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;

    TraceLog("VIRTEX5_IntDisable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "VIRTEX5_IntDisable"))
        return WD_INVALID_PARAMETER;

    if (!WDC_IntIsEnabled(hDev))
    {
        ErrLog("Interrupts are already disabled ...\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    /* TODO: You can add code here to write to the device in order
             to physically disable the hardware interrupts */

    /* Disable the interrupts */
    dwStatus = WDC_IntDisable(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

BOOL VIRTEX5_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    if (!IsValidDevice((PWDC_DEVICE)hDev, "VIRTEX5_IntIsEnabled"))
        return FALSE;

    return WDC_IntIsEnabled(hDev);
}

/* -----------------------------------------------
    Direct Memory Access (DMA)
   ----------------------------------------------- */
static BOOL IsValidDmaHandle(VIRTEX5_DMA_HANDLE hDma, CHAR *sFunc)
{
    BOOL ret = (hDma && IsValidDevice(hDma->hDev, sFunc)) ? TRUE : FALSE;

    if (!hDma)
        ErrLog("%s: NULL DMA Handle\n", sFunc);

    return ret;
}

DWORD VIRTEX5_DMAOpen(WDC_DEVICE_HANDLE hDev, PVOID *ppBuf, DWORD dwOptions,
    DWORD dwBytes, VIRTEX5_DMA_HANDLE *ppDmaHandle)
{
    DWORD dwStatus;
    PVIRTEX5_DEV_CTX pDevCtx;
    VIRTEX5_DMA_HANDLE pVIRTEX5Dma = NULL;

    if (!IsValidDevice((PWDC_DEVICE)hDev, "VIRTEX5_DMAOpen"))
        return WD_INVALID_PARAMETER;
    
    if (!ppBuf)
    {
        ErrLog("VIRTEX5_DMAOpen: Error - NULL DMA buffer pointer\n");
        return WD_INVALID_PARAMETER;
    }

    pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(hDev);
    if (pDevCtx->hDma)
    {
        ErrLog("VIRTEX5_DMAOpen: Error - DMA already open\n");
        return WD_OPERATION_ALREADY_DONE;
    }

    pVIRTEX5Dma = (VIRTEX5_DMA_STRUCT *)calloc(1, sizeof(VIRTEX5_DMA_STRUCT));
    if (!pVIRTEX5Dma)
    {
        ErrLog("VIRTEX5_DMAOpen: Failed allocating memory for VIRTEX5 DMA "
            "struct\n");
        return WD_INSUFFICIENT_RESOURCES;
    }
    pVIRTEX5Dma->hDev = hDev;
    
    /* Allocate and lock a DMA buffer */
    dwStatus = WDC_DMAContigBufLock(hDev, ppBuf, dwOptions, dwBytes,
        &pVIRTEX5Dma->pDma);

    if (WD_STATUS_SUCCESS != dwStatus) 
    {
        ErrLog("VIRTEX5_DMAOpen: Failed locking a DMA buffer. "
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        goto Error;
    }
    
    *ppDmaHandle = (VIRTEX5_DMA_HANDLE)pVIRTEX5Dma;

    /* update the device context */
    pDevCtx->hDma = pVIRTEX5Dma;
    pDevCtx->pBuf = *ppBuf;
    
    return WD_STATUS_SUCCESS;

Error:
    if (pVIRTEX5Dma)
        VIRTEX5_DMAClose((VIRTEX5_DMA_HANDLE)pVIRTEX5Dma);
    
    return dwStatus;
}

void VIRTEX5_DMADevicePrepare(VIRTEX5_DMA_HANDLE hDma, UINT32 len)
{
    UINT32 tlps;
    UINT32 LowerAddr;
    BYTE UpperAddr;
    WDC_DEVICE_HANDLE hDev;
    PVIRTEX5_DEV_CTX pDevCtx;

    if (!IsValidDmaHandle(hDma, "VIRTEX5_DMADevicePrepare"))
        return;

    hDev = hDma->hDev;

    LowerAddr = (UINT32)hDma->pDma->Page[0].pPhysicalAddr;
    UpperAddr = (BYTE)((hDma->pDma->Page[0].pPhysicalAddr >> 32) & 0xFF);

	/* Set lower 32bit of DMA address */
    VIRTEX5_WriteReg32(hDev, VIRTEX5_DMAADDR_OFFSET, LowerAddr);

    /* Set DMA Length */
    VIRTEX5_WriteReg32(hDev, VIRTEX5_DMALEN_OFFSET, len);

    

    pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(hDev);
    pDevCtx->fIsRead = TRUE;
    pDevCtx->u32Pattern = 0;
    pDevCtx->dwTotalCount = 1024*1024*2/4;
}

void VIRTEX5_DMAClose(VIRTEX5_DMA_HANDLE hDma)
{
    DWORD dwStatus = WD_STATUS_SUCCESS;
    PVIRTEX5_DEV_CTX pDevCtx;

    TraceLog("VIRTEX5_DMAClose entered.");
    
    if (!IsValidDmaHandle(hDma, "VIRTEX5_DMAClose"))
        return;
    
    TraceLog(" Device handle: 0x%p\n", hDma->hDev);
    
    if (hDma->pDma)
    {
        dwStatus = WDC_DMABufUnlock(hDma->pDma);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("VIRTEX5_DMAClose: Failed unlocking DMA buffer. "
                "Error 0x%lX - %s\n", dwStatus, Stat2Str(dwStatus));
        }
    }
    else
    {
        TraceLog("VIRTEX5_DMAClose: DMA is not currently open ... "
            "(device handle 0x%p, DMA handle 0x%p)\n", hDma->hDev, hDma);
    }
     
    pDevCtx = (PVIRTEX5_DEV_CTX)WDC_GetDevContext(hDma->hDev);
    pDevCtx->hDma = NULL;
    pDevCtx->pBuf = NULL;

    free(hDma);
}

static WORD code2size(BYTE bCode)
{
    if (bCode > 0x05)
        return 0;
    return (WORD)(128 << bCode);
}



void VIRTEX5_DMASyncCpu(VIRTEX5_DMA_HANDLE hDma)
{
    WDC_DMASyncCpu(hDma->pDma);
}

void VIRTEX5_DMASyncIo(VIRTEX5_DMA_HANDLE hDma)
{
    WDC_DMASyncIo(hDma->pDma);
}

void VIRTEX5_DMAStart(VIRTEX5_DMA_HANDLE hDma)
{
    if (!IsValidDmaHandle(hDma, "VIRTEX5_DMAStart"))
        return;

    VIRTEX5_DMASyncCpu(hDma);

    /* Start DMA Transfer */
    VIRTEX5_WriteReg32(hDma->hDev, VIRTEX5_DMACTL_OFFSET,0x1);
}







/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
DWORD VIRTEX5_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    
    if (!IsValidDevice(pDev, "VIRTEX5_GetNumAddrSpaces"))
        return 0;

    return pDev->dwNumAddrSpaces;
}

BOOL VIRTEX5_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev,
    VIRTEX5_ADDR_SPACE_INFO *pAddrSpaceInfo)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwAddrSpace, dwMaxAddrSpace;
    BOOL fIsMemory;
    
    if (!IsValidDevice(pDev, "VIRTEX5_GetAddrSpaceInfo"))
        return FALSE;

    dwAddrSpace = pAddrSpaceInfo->dwAddrSpace;
    dwMaxAddrSpace = pDev->dwNumAddrSpaces - 1;

    if (dwAddrSpace > dwMaxAddrSpace)
    {
        ErrLog("VIRTEX5_GetAddrSpaceInfo: Error - Address space %ld "
            "is out of range (0 - %ld)\n", dwAddrSpace, dwMaxAddrSpace);
        return FALSE;
    }

    pAddrDesc = &pDev->pAddrDesc[dwAddrSpace];

    fIsMemory = WDC_ADDR_IS_MEM(pAddrDesc);
    
    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "BAR %ld", dwAddrSpace);
    snprintf(pAddrSpaceInfo->sType, MAX_TYPE - 1, fIsMemory ? "Memory" : "I/O");
        
    if (WDC_AddrSpaceIsActive(pDev, dwAddrSpace))
    {
        WD_ITEMS *pItem = &pDev->cardReg.Card.Item[pAddrDesc->dwItemIndex];
        DWORD dwAddr = fIsMemory ? pItem->I.Mem.dwPhysicalAddr :
            (DWORD)pItem->I.IO.dwAddr;
        
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1,
            "0x%0*lX - 0x%0*lX (0x%lx bytes)", (int)WDC_SIZE_32 * 2, dwAddr,
            (int)WDC_SIZE_32 * 2, dwAddr + pAddrDesc->dwBytes - 1,
            pAddrDesc->dwBytes);
    }
    else
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "Inactive address space");

    /* TODO: You can modify the code above to set a different address space
     * name/description */

    return TRUE;
}

/**
 * Function name:  VIRTEX5_WriteReg8, VIRTEX5_WriteReg16, VIRTEX5_WriteReg32
 * Description: Read 8/16/32 bits from a register
 * Parameters: 
 *     @hDev: handle to the card as received from VIRTEX5_DeviceOpen().
 *     @offset: register offset.
 * Return values: read data
 * Scope: global
 **/
UINT32 VIRTEX5_ReadReg32(WDC_DEVICE_HANDLE hDev, DWORD offset)
{
    UINT32 data;

    WDC_ReadAddr32(hDev, VIRTEX5_SPACE, offset, &data);
    return data;
}

WORD VIRTEX5_ReadReg16(WDC_DEVICE_HANDLE hDev, DWORD offset)
{
    WORD data;

    WDC_ReadAddr16(hDev, VIRTEX5_SPACE, offset, &data);
    return data;
}

BYTE VIRTEX5_ReadReg8(WDC_DEVICE_HANDLE hDev, DWORD offset)
{
    BYTE data;

    WDC_ReadAddr8(hDev, VIRTEX5_SPACE, offset, &data);
    return data;
}

/**
 * Function name:  VIRTEX5_WriteReg8, VIRTEX5_WriteReg16, VIRTEX5_WriteReg32
 * Description: Write 8/16/32 bits to a register
 * Parameters: 
 *     @hDev: handle to the card as received from VIRTEX5_DeviceOpen().
 *     @offset: register offset.
 *     @data: the data to write to the register.
 * Return values: none
 * Scope: global
 **/
void VIRTEX5_WriteReg32(WDC_DEVICE_HANDLE hDev, DWORD offset, UINT32 data)
{
    WDC_WriteAddr32(hDev, VIRTEX5_SPACE, offset, data);
}

void VIRTEX5_WriteReg16(WDC_DEVICE_HANDLE hDev, DWORD offset, WORD data)
{
    WDC_WriteAddr16(hDev, VIRTEX5_SPACE, offset, data);
}

void VIRTEX5_WriteReg8(WDC_DEVICE_HANDLE hDev, DWORD offset, BYTE data)
{
    WDC_WriteAddr8(hDev, VIRTEX5_SPACE, offset, data);
}

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
static void ErrLog(const CHAR *sFormat, ...)
{
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(gsVIRTEX5_LastErr, sizeof(gsVIRTEX5_LastErr) - 1, sFormat, argp);
#if defined(DEBUG)
#if defined (__KERNEL__)
    WDC_Err("KP VIRTEX5 lib: %s", gsPCI_LastErr);
#else
    WDC_Err("VIRTEX5 lib: %s", gsPCI_LastErr);
#endif
#endif
    va_end(argp);
}

static void TraceLog(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
#if defined (__KERNEL__)
    WDC_Trace("KP VIRTEX5 lib: %s", sMsg);
#else
    WDC_Trace("VIRTEX5 lib: %s", sMsg);
#endif
    va_end(argp);
#endif
}

const char *VIRTEX5_GetLastErr(void)
{
    return gsVIRTEX5_LastErr;
}

