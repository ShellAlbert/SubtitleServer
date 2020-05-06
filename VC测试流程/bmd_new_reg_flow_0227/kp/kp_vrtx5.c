/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/************************************************************************
*  File: kp_vrtx5.c
*
*  Kernel PlugIn driver for accessing VIRTEX5 devices.
*  The code accesses hardware using WinDriver's WDC library.
*************************************************************************/

#include "kpstdlib.h"
#include "wd_kp.h"
#include "utils.h"
#include "wdc_defs.h"
#include "samples/shared/bits.h"
#include "../virtex5_lib.h"

/*************************************************************
  Functions prototypes
 *************************************************************/
BOOL __cdecl KP_VRTX5_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext);
void __cdecl KP_VRTX5_Close(PVOID pDrvContext);
void __cdecl KP_VRTX5_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    BOOL fIsKernelMode);
BOOL __cdecl KP_VRTX5_IntEnable(PVOID pDrvContext,
    WD_KERNEL_PLUGIN_CALL *kpCall, PVOID *ppIntContext);
void __cdecl KP_VRTX5_IntDisable(PVOID pIntContext);
BOOL __cdecl KP_VRTX5_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt);
DWORD __cdecl KP_VRTX5_IntAtDpc(PVOID pIntContext, DWORD dwCount);
BOOL __cdecl KP_VRTX5_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved);
DWORD __cdecl KP_VRTX5_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved);
BOOL __cdecl KP_VRTX5_Event(PVOID pDrvContext, WD_EVENT *wd_event);
static void KP_VRTX5_Err(const CHAR *sFormat, ...);
static void KP_VRTX5_Trace(const CHAR *sFormat, ...);

/*************************************************************
  Functions implementation
 *************************************************************/

/* KP_Init is called when the Kernel PlugIn driver is loaded.
   This function sets the name of the Kernel PlugIn driver and the driver's
   open callback function. */
BOOL __cdecl KP_Init(KP_INIT *kpInit)
{
    /* Verify that the version of the WinDriver Kernel PlugIn library
       is identical to that of the windrvr.h and wd_kp.h files */
    if (WD_VER != kpInit->dwVerWD)
    {
        /* Re-build your Kernel PlugIn driver project with the compatible
           version of the WinDriver Kernel PlugIn library (kp_nt<version>.lib)
           and windrvr.h and wd_kp.h files */

        return FALSE;
    }

    kpInit->funcOpen = KP_VRTX5_Open;
    strcpy (kpInit->cDriverName, KP_VRTX5_DRIVER_NAME);

    return TRUE;
}

/* KP_VRTX5_Open is called when WD_KernelPlugInOpen() is called from the user
   mode. pDrvContext will be passed to the rest of the Kernel PlugIn callback
   functions. */
BOOL __cdecl KP_VRTX5_Open(KP_OPEN_CALL *kpOpenCall, HANDLE hWD,
    PVOID pOpenData, PVOID *ppDrvContext)
{
    PWDC_DEVICE pDev;
    WDC_ADDR_DESC *pAddrDesc;
    DWORD dwSize, dwStatus;
    void *temp;

    KP_VRTX5_Trace("KP_VRTX5_Open entered\n");
    
    kpOpenCall->funcClose = KP_VRTX5_Close;
    kpOpenCall->funcCall = KP_VRTX5_Call;
    kpOpenCall->funcIntEnable = KP_VRTX5_IntEnable;
    kpOpenCall->funcIntDisable = KP_VRTX5_IntDisable;
    kpOpenCall->funcIntAtIrql = KP_VRTX5_IntAtIrql;
    kpOpenCall->funcIntAtDpc = KP_VRTX5_IntAtDpc;
    kpOpenCall->funcIntAtIrqlMSI = KP_VRTX5_IntAtIrqlMSI; 
    kpOpenCall->funcIntAtDpcMSI = KP_VRTX5_IntAtDpcMSI;
    kpOpenCall->funcEvent = KP_VRTX5_Event;

    /* Initialize the VIRTEX5 library */
    dwStatus = VIRTEX5_LibInit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_VRTX5_Err("KP_VRTX5_Open: Failed to initialize the VIRTEX5"
            "library: %s", VIRTEX5_GetLastErr());
        return FALSE;
    }

    /* Create a copy of device information in the driver context */
    dwSize = sizeof(WDC_DEVICE);
    pDev = malloc(dwSize);
    if (!pDev)
        goto malloc_error;

    COPY_FROM_USER(&temp, pOpenData, sizeof(void *));
    COPY_FROM_USER(pDev, temp, dwSize);

    dwSize = sizeof(WDC_ADDR_DESC) * pDev->dwNumAddrSpaces;
    pAddrDesc = malloc(dwSize);
    if (!pAddrDesc)
        goto malloc_error;
    
    COPY_FROM_USER(pAddrDesc, pDev->pAddrDesc, dwSize);
    pDev->pAddrDesc = pAddrDesc;

    *ppDrvContext = pDev;

    KP_VRTX5_Trace("KP_VRTX5_Open: Kernel PlugIn driver opened "
        "successfully\n");
    
    return TRUE;
    
malloc_error:
    KP_VRTX5_Err("KP_VRTX5_Open: Failed allocating %ld bytes\n", dwSize);
    VIRTEX5_LibUninit();
    return FALSE;
}

/* KP_VRTX5_Close is called when WD_KernelPlugInClose() is called from the
   user mode */
void __cdecl KP_VRTX5_Close(PVOID pDrvContext)
{
    DWORD dwStatus;

    KP_VRTX5_Trace("KP_VRTX5_Close entered\n");
    
    /* Uninit the VIRTEX5 library */
    dwStatus = VIRTEX5_LibUninit();
    if (WD_STATUS_SUCCESS != dwStatus)
    {
        KP_VRTX5_Err("KP_VRTX5_Close: Failed to uninit the VIRTEX5 "
            "library: %s", VIRTEX5_GetLastErr());
    }

    /* Free the memory allocated for the driver context */
    if (pDrvContext)
    {
        free(((PWDC_DEVICE)pDrvContext)->pAddrDesc);
        free(pDrvContext);  
    }
}

/* KP_VRTX5_Call is called when WD_KernelPlugInCall() is called from the
   user mode */
void __cdecl KP_VRTX5_Call(PVOID pDrvContext, WD_KERNEL_PLUGIN_CALL *kpCall,
    BOOL fIsKernelMode)
{
    KP_VRTX5_Trace("KP_VRTX5_Call entered. Message: 0x%lx\n",
        kpCall->dwMessage);
    
    kpCall->dwResult = KP_VRTX5_STATUS_OK;
    
    switch (kpCall->dwMessage)
    {
    case KP_VRTX5_MSG_VERSION: /* Get the version of the Kernel PlugIn */
        {
            DWORD dwVer = 100;
            KP_VRTX5_VERSION *pUserKPVer =
                (KP_VRTX5_VERSION *)(kpCall->pData);
            COPY_TO_USER_OR_KERNEL(&pUserKPVer->dwVer, &dwVer, sizeof(DWORD),
                fIsKernelMode);
            COPY_TO_USER_OR_KERNEL(pUserKPVer->cVer, "My Driver V1.00",
                sizeof("My Driver V1.00") + 1, fIsKernelMode);
            kpCall->dwResult = KP_VRTX5_STATUS_OK;
        }
        break ;
    default:
        kpCall->dwResult = KP_VRTX5_STATUS_MSG_NO_IMPL;
    }

    /* NOTE: You can modify the messages above and/or add your own
             Kernel PlugIn messages.
             When changing/adding messages, be sure to also update the
             messages definitions in ../virtex5_lib.h. */
}

/* KP_VRTX5_IntEnable is called when WD_IntEnable() is called from the user
   mode with a Kernel PlugIn handle. The interrupt context (pIntContext) will
   be passed to the rest of the Kernel PlugIn interrupt functions.
   The function returns TRUE if interrupts are enabled successfully. */
BOOL __cdecl KP_VRTX5_IntEnable(PVOID pDrvContext,
    WD_KERNEL_PLUGIN_CALL *kpCall, PVOID *ppIntContext)
{
    KP_VRTX5_Trace("KP_VRTX5_IntEnable entered\n");
    
    /* You can allocate specific memory for each interrupt in *ppIntContext */
    
    /* In this sample we will set the interrupt context to the driver context,
       which has been set in KP_VRTX5_Open to hold the device information. */
    *ppIntContext = pDrvContext;

    /* TODO: You can add code here to write to the device in order
             to physically enable the hardware interrupts */

    return TRUE;
}

/* KP_VRTX5_IntDisable is called when WD_IntDisable() is called from the user
   mode with a Kernel PlugIn handle */
void __cdecl KP_VRTX5_IntDisable(PVOID pIntContext)
{
    /* Free any memory allocated in KP_VRTX5_IntEnable() here */
}

/* KP_VRTX5_IntAtIrql returns TRUE if deferred interrupt processing (DPC) for
   level-sensitive interrupt is required.
   The function is called at HIGH IRQL - at physical interrupt handler.
   Most library calls are NOT allowed at this level, for example:
   NO   WDC_xxx() or WD_xxx calls, apart from the WDC read/write address or
        register functions, WDC_MultiTransfer(), WD_Transfer(),
        WD_MultiTransfer() or WD_DebugAdd().
   NO   malloc().
   NO   free().
   YES  WDC read/write address or configuration space functions, 
        WDC_MultiTransfer(), WD_Transfer(), WD_MultiTransfer() or 
        WD_DebugAdd(), or wrapper functions that call these functions.
   YES  specific kernel OS functions (such as WinDDK functions) that can
        be called from HIGH IRQL. [Note that the use of such functions may
        break the code's portability to other OSs.] */
BOOL __cdecl KP_VRTX5_IntAtIrql(PVOID pIntContext, BOOL *pfIsMyInterrupt)
{
    /* This specific sample is designed to demonstrate Message-Signaled
       Interrupts (MSI) only! Using the sample as-is on an OS that cannot
       enable MSIs will cause the OS to HANG when an interrupt occurs! */

    /* If the data read from the hardware indicates that the interrupt belongs
       to you, you must set *pfIsMyInterrupt to TRUE;
       otherwise, set it to FALSE (this will allow ISRs of other drivers to be
       invoked). */
    *pfIsMyInterrupt = FALSE;
    return FALSE;
}

/* KP_VRTX5_IntAtDpc is a Deferred Procedure Call for additional
   level-sensitive interupt processing. This function is called if
   KP_VRTX5_IntAtIrql returned TRUE. KP_VRTX5_IntAtDpc returns the number of
   times to notify the user mode of the interrupt (i.e. return from WD_IntWait).
 */
DWORD __cdecl KP_VRTX5_IntAtDpc(PVOID pIntContext, DWORD dwCount)
{
    return dwCount;
}

/* KP_VRTX5_IntAtIrqlMSI returns TRUE if deferred interrupt processing (DPC)
   for Message-Signaled Interrupts (MSI) or Extended Message-Signaled Interrupts
   (MSI-X) is required.
   The function is called at HIGH IRQL - at physical interrupt handler.
   Note: Do not use the dwReserved parameter. 
   Most library calls are NOT allowed at this level, for example:
   NO   WDC_xxx() or WD_xxx calls, apart from the WDC read/write address or
        register functions, WDC_MultiTransfer(), WD_Transfer(),
        WD_MultiTransfer() or WD_DebugAdd().
   NO   malloc().
   NO   free().
   YES  WDC read/write address or configuration space functions, 
        WDC_MultiTransfer(), WD_Transfer(), WD_MultiTransfer() or 
        WD_DebugAdd(), or wrapper functions that call these functions.
   YES  specific kernel OS functions (such as WinDDK functions) that can
        be called from HIGH IRQL. [Note that the use of such functions may
        break the code's portability to other OSs.] */
BOOL __cdecl KP_VRTX5_IntAtIrqlMSI(PVOID pIntContext, ULONG dwLastMessage,
    DWORD dwReserved)
{
    /* There is no need to acknowledge MSI/MSI-X. However, you can implement
       the same functionality here as done in the KP_VRTX5_IntAtIrql handler
       to read/write data from/to registers at HIGH IRQL. */
    return TRUE;
}

/* KP_VRTX5_IntAtDpcMSI is a Deferred Procedure Call for additional
   Message-Signaled Interrupts (MSI) or Extended Message-Signaled Interrupts
   (MSI-X) processing.
   This function is called if KP_VRTX5_IntAtIrqlMSI returned TRUE.
   KP_VRTX5_IntAtDpcMSI returns the number of times to notify the user mode of
   the interrupt (i.e. return from WD_IntWait). */
DWORD __cdecl KP_VRTX5_IntAtDpcMSI(PVOID pIntContext, DWORD dwCount,
    ULONG dwLastMessage, DWORD dwReserved)
{
    return dwCount;
}

/* KP_VRTX5_Event is called when a Plug-and-Play/power management event for
   the device is received, if EventRegister() was first called from the
   user mode with the Kernel PlugIn handle. */
BOOL __cdecl KP_VRTX5_Event(PVOID pDrvContext, WD_EVENT *wd_event)
{
    return TRUE; /* Return TRUE to notify the user mode of the event */
}

/* -----------------------------------------------
    Debugging and error handling
   ----------------------------------------------- */
static void KP_VRTX5_Err(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Err("%s: %s", KP_VRTX5_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

static void KP_VRTX5_Trace(const CHAR *sFormat, ...)
{
#if defined(DEBUG)
    CHAR sMsg[256];
    va_list argp;
    va_start(argp, sFormat);
    vsnprintf(sMsg, sizeof(sMsg) - 1, sFormat, argp);
    WDC_Trace("%s: %s", KP_VRTX5_DRIVER_NAME, sMsg);
    va_end(argp);
#endif
}

