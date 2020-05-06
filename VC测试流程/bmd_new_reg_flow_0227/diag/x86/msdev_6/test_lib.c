/************************************************************************
*  File: test_lib.c
*
*  Library for accessing TEST devices.
*  The code accesses hardware using WinDriver's WDC library.
*  Code was generated by DriverWizard v10.00.
*
*  Jungo Confidential. Copyright (c) 2012 Jungo Ltd.  http://www.jungo.com
*************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "wdc_defs.h"
#include "utils.h"
#include "status_strings.h"
#include "test_lib.h"

/*************************************************************
  Internal definitions
 *************************************************************/
/* WinDriver license registration string */
/* TODO: When using a registered WinDriver version, make sure the license string
         below is your specific WinDriver license registration string and set 
 * the driver name to your driver's name */
#define TEST_DEFAULT_LICENSE_STRING "6C3CC2CFE89E7AD0424A070D434A6F6DC49537A5.zhong"
#define TEST_DEFAULT_DRIVER_NAME "windrvr6"

/* TEST device information struct */
typedef struct {
    WD_TRANSFER      *pIntTransCmds;
    TEST_INT_HANDLER   funcDiagIntHandler;
} TEST_DEV_CTX, *PTEST_DEV_CTX;
/* TODO: You can add fields to store additional device-specific information */

static CHAR gsTEST_LastErr[256];

static DWORD LibInit_count = 0;
/*************************************************************
  Static functions prototypes and inline implementation
 *************************************************************/
static BOOL DeviceValidate(const PWDC_DEVICE pDev);
static void DLLCALLCONV TEST_IntHandler(PVOID pData);
static void ErrLog(const CHAR *sFormat, ...);
static void TraceLog(const CHAR *sFormat, ...);

static inline BOOL IsValidDevice(PWDC_DEVICE pDev, const CHAR *sFunc)
{
    if (!pDev || !WDC_GetDevContext(pDev))
    {
        snprintf(gsTEST_LastErr, sizeof(gsTEST_LastErr) - 1, "%s: NULL device %s\n",
            sFunc, !pDev ? "handle" : "context");
        ErrLog(gsTEST_LastErr);
        return FALSE;
    }

    return TRUE;
}

/*************************************************************
  Functions implementation
 *************************************************************/
/* -----------------------------------------------
    TEST and WDC library initialize/uninit
   ----------------------------------------------- */
DWORD TEST_LibInit(void)
{
    DWORD dwStatus;
 
    /* init only once */
    if (++LibInit_count > 1)
        return WD_STATUS_SUCCESS;
 
#if defined(WD_DRIVER_NAME_CHANGE)
    /* Set the driver name */
    if (!WD_DriverName(TEST_DEFAULT_DRIVER_NAME))
    {
        ErrLog("Failed to set the driver name for WDC library.\n");
        return WD_SYSTEM_INTERNAL_ERROR;
    }
#endif

    /* Set WDC library's debug options (default: level TRACE, output to Debug Monitor) */
    dwStatus = WDC_SetDebugOptions(WDC_DBG_DEFAULT, NULL);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize debug options for WDC library.\n"
            "Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    /* Open a handle to the driver and initialize the WDC library */
    dwStatus = WDC_DriverOpen(WDC_DRV_OPEN_DEFAULT, TEST_DEFAULT_LICENSE_STRING);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to initialize the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        
        return dwStatus;
    }

    return WD_STATUS_SUCCESS;
}

DWORD TEST_LibUninit(void)
{
    DWORD dwStatus;

    if (--LibInit_count > 0)
        return WD_STATUS_SUCCESS;
 
    /* Uninit the WDC library and close the handle to WinDriver */
    dwStatus = WDC_DriverClose();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed to uninit the WDC library. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
    }

    return dwStatus;
}

/* -----------------------------------------------
    Device open/close
   ----------------------------------------------- */
WDC_DEVICE_HANDLE TEST_DeviceOpen(const WD_PCI_CARD_INFO *pDeviceInfo)
{
    DWORD dwStatus;
    PTEST_DEV_CTX pDevCtx = NULL;
    WDC_DEVICE_HANDLE hDev = NULL;

    /* Validate arguments */
    if (!pDeviceInfo)
    {
        ErrLog("TEST_DeviceOpen: Error - NULL device information struct pointer\n");
        return NULL;
    }

    /* Allocate memory for the TEST device context */
    pDevCtx = (PTEST_DEV_CTX)malloc(sizeof (TEST_DEV_CTX));
    if (!pDevCtx)
    {
        ErrLog("Failed allocating memory for TEST device context\n");
        return NULL;
    }

    BZERO(*pDevCtx);

    /* Open a WDC device handle */
    dwStatus = WDC_PciDeviceOpen(&hDev, pDeviceInfo, pDevCtx, NULL, NULL, NULL);

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
    TraceLog("TEST_DeviceOpen: Opened a TEST device (handle 0x%p)\n", hDev);
    return hDev;

Error:    
    if (hDev)
        TEST_DeviceClose(hDev);
    else
        free(pDevCtx);
    
    return NULL;
}

BOOL TEST_DeviceClose(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PTEST_DEV_CTX pDevCtx;
    
    TraceLog("TEST_DeviceClose entered. Device handle: 0x%p\n", hDev);

    if (!hDev)
    {
        ErrLog("TEST_DeviceClose: Error - NULL device handle\n");
        return FALSE;
    }

    pDevCtx = (PTEST_DEV_CTX)WDC_GetDevContext(pDev);
    
    /* Disable interrupts */
    if (WDC_IntIsEnabled(hDev))
    {
        dwStatus = TEST_IntDisable(hDev);
        if (WD_STATUS_SUCCESS != dwStatus)
        {
            ErrLog("Failed disabling interrupts. Error 0x%lx - %s\n",
                dwStatus, Stat2Str(dwStatus));
        }
    }

    /* Close the device */
    dwStatus = WDC_PciDeviceClose(hDev);
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed closing a WDC device handle (0x%p). Error 0x%lx - %s\n",
            hDev, dwStatus, Stat2Str(dwStatus));
    }

    /* Free TEST device context memory */
    if (pDevCtx)
        free (pDevCtx);
    
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
    
    TraceLog("Device does not have any active memory or I/O address spaces\n");
    return TRUE;
}

/* -----------------------------------------------
    Interrupts
   ----------------------------------------------- */
static void DLLCALLCONV TEST_IntHandler(PVOID pData)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)pData;
    PTEST_DEV_CTX pDevCtx = (PTEST_DEV_CTX)WDC_GetDevContext(pDev);
    TEST_INT_RESULT intResult;

    BZERO(intResult);
    intResult.dwCounter = pDev->Int.dwCounter;
    intResult.dwLost = pDev->Int.dwLost;
    intResult.waitResult = (WD_INTERRUPT_WAIT_RESULT)pDev->Int.fStopped;
    intResult.dwEnabledIntType = WDC_GET_ENABLED_INT_TYPE(pDev);
    intResult.dwLastMessage = WDC_GET_ENABLED_INT_LAST_MSG(pDev);
    
    /* Execute the diagnostics application's interrupt handler routine */
    pDevCtx->funcDiagIntHandler((WDC_DEVICE_HANDLE)pDev, &intResult);
}

static BOOL IsItemExists(PWDC_DEVICE pDev, ITEM_TYPE item)
{
    int i;
    DWORD dwNumItems = pDev->cardReg.Card.dwItems;

    for (i=0; i<dwNumItems; i++)
    {
        if (pDev->cardReg.Card.Item[i].item == item)
            return TRUE;
    }

    return FALSE;
}

DWORD TEST_IntEnable(WDC_DEVICE_HANDLE hDev, TEST_INT_HANDLER funcIntHandler)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PTEST_DEV_CTX pDevCtx;
    WDC_ADDR_DESC *pAddrDesc;
    WD_TRANSFER *pTrans;

    TraceLog("TEST_IntEnable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "TEST_IntEnable"))
        return WD_INVALID_PARAMETER;

    if (!IsItemExists(pDev, ITEM_INTERRUPT))
        return WD_OPERATION_FAILED;

    pDevCtx = (PTEST_DEV_CTX)WDC_GetDevContext(pDev);

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
    pTrans[0].dwPort = pAddrDesc->kptAddr + 0x0;
    pTrans[0].cmdTrans = WDC_ADDR_IS_MEM(pAddrDesc) ? WM_DWORD : WP_DWORD;
    pTrans[0].Data.Dword = 0x100;

    /* Store the diag interrupt handler routine, which will be executed by
       TEST_IntHandler() when an interrupt is received */
    pDevCtx->funcDiagIntHandler = funcIntHandler;
    
    /* Enable the interrupts */
    dwStatus = WDC_IntEnable(hDev, pTrans, NUM_TRANS_CMDS, INTERRUPT_CMD_COPY,
        TEST_IntHandler, (PVOID)pDev, WDC_IS_KP(hDev));
        
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        ErrLog("Failed enabling interrupts. Error 0x%lx - %s\n",
            dwStatus, Stat2Str(dwStatus));
        
        free(pTrans);
        
        return dwStatus;
    }

    /* Store the interrupt transfer commands in the device context */
    pDevCtx->pIntTransCmds = pTrans;

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    TraceLog("TEST_IntEnable: Interrupts enabled\n");

    return WD_STATUS_SUCCESS;
}

DWORD TEST_IntDisable(WDC_DEVICE_HANDLE hDev)
{
    DWORD dwStatus;
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    PTEST_DEV_CTX pDevCtx;

    TraceLog("TEST_IntDisable entered. Device handle: 0x%p\n", hDev);

    if (!IsValidDevice(pDev, "TEST_IntDisable"))
        return WD_INVALID_PARAMETER;

    pDevCtx = (PTEST_DEV_CTX)WDC_GetDevContext(pDev);
 
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

    /* Free the memory allocated for the interrupt transfer commands */
    if (pDevCtx->pIntTransCmds)
    {
        free(pDevCtx->pIntTransCmds);
        pDevCtx->pIntTransCmds = NULL;
    }

    return dwStatus;
}

BOOL TEST_IntIsEnabled(WDC_DEVICE_HANDLE hDev)
{
    if (!IsValidDevice((PWDC_DEVICE)hDev, "TEST_IntIsEnabled"))
        return FALSE;

    return WDC_IntIsEnabled(hDev);
}


/* -----------------------------------------------
    Address spaces information
   ----------------------------------------------- */
DWORD TEST_GetNumAddrSpaces(WDC_DEVICE_HANDLE hDev)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    
    if (!IsValidDevice(pDev, "TEST_GetNumAddrSpaces"))
        return 0;

    return pDev->dwNumAddrSpaces;
}

BOOL TEST_GetAddrSpaceInfo(WDC_DEVICE_HANDLE hDev, TEST_ADDR_SPACE_INFO *pAddrSpaceInfo)
{
    PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwAddrSpace, dwMaxAddrSpace;
    BOOL fIsMemory;
    
    if (!IsValidDevice(pDev, "TEST_GetAddrSpaceInfo"))
        return FALSE;

#if defined(DEBUG)
    if (!pAddrSpaceInfo)
    {
        ErrLog("TEST_GetAddrSpaceInfo: Error - NULL address space information pointer\n");
        return FALSE;
    }
#endif

    dwAddrSpace = pAddrSpaceInfo->dwAddrSpace;
    dwMaxAddrSpace = pDev->dwNumAddrSpaces - 1;

    if (dwAddrSpace > dwMaxAddrSpace)
    {
        ErrLog("TEST_GetAddrSpaceInfo: Error - Address space %ld is out of range (0 - %ld)\n",
            dwAddrSpace, dwMaxAddrSpace);
        return FALSE;
    }

    pAddrDesc = &pDev->pAddrDesc[dwAddrSpace];

    fIsMemory = WDC_ADDR_IS_MEM(pAddrDesc);
    
    snprintf(pAddrSpaceInfo->sName, MAX_NAME - 1, "BAR %ld", dwAddrSpace);
    snprintf(pAddrSpaceInfo->sType, MAX_TYPE - 1, fIsMemory ? "Memory" : "I/O");
        
    if (WDC_AddrSpaceIsActive(pDev, dwAddrSpace))
    {
        WD_ITEMS *pItem = &pDev->cardReg.Card.Item[pAddrDesc->dwItemIndex];
        DWORD dwAddr = fIsMemory ? pItem->I.Mem.dwPhysicalAddr : (DWORD)pItem->I.IO.dwAddr;
        
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "0x%0*lX - 0x%0*lX (0x%lx bytes)",
            (int)WDC_SIZE_32 * 2, dwAddr,
            (int)WDC_SIZE_32 * 2, dwAddr + pAddrDesc->dwBytes - 1,
            pAddrDesc->dwBytes);
    }
    else
        snprintf(pAddrSpaceInfo->sDesc, MAX_DESC - 1, "Inactive address space");

    /* TODO: You can modify the code above to set a different address space name/description */

    return TRUE;
}

// Function: TEST_ReadRegister0()
//   Read from Register0 register.
// Parameters:
//   hDev [in] handle to the card as received from TEST_DeviceOpen().
// Return Value:
//   The value read from the register.
UINT32 TEST_ReadRegister0 (WDC_DEVICE_HANDLE hDev)
{
    UINT32 data;

    WDC_ReadAddr32(hDev, TEST_Register0_SPACE, TEST_Register0_OFFSET, &data);
    return data;
}

// Function: TEST_WriteRegister0()
//   Write to Register0 register.
// Parameters:
//   hDev [in] handle to the card as received from TEST_DeviceOpen().
//   data [in] the data to write to the register.
// Return Value:
//   None.
void TEST_WriteRegister0 (WDC_DEVICE_HANDLE hDev, UINT32 data)
{
    WDC_WriteAddr32(hDev, TEST_Register0_SPACE, TEST_Register0_OFFSET, data);
}

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
static void ErrLog(const CHAR *sFormat, ...)
{
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(gsTEST_LastErr, sizeof(gsTEST_LastErr) - 1, sFormat, argp);
#if defined(DEBUG)
    WDC_Err("TEST lib: %s", TEST_GetLastErr());
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
    WDC_Trace("TEST lib: %s", sMsg);
    va_end(argp);
#endif
}

const char *TEST_GetLastErr(void)
{
    return gsTEST_LastErr;
}
