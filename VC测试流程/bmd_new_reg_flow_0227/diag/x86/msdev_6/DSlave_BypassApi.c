/******************************************************************************
 *
 * File Name:
 *
 *      DSlave_BypassApi.c
 *
 * Description:
 *
 *      The "Direct Slave" sample application, which demonstrates how to perform
 *      a transfer using the virtual address obtained with the PLX API.
 *
 * Revision History:
 *
 *      12-01-07 : PLX SDK v5.20
 *
 ******************************************************************************/




   
void
SendFrame (
    PLX_DEVICE_OBJECT *pDevice,U16 FrameNum
    )
{
    PLX_STATUS        rc;
    PLX_DMA_PROP      DmaProp;
    PLX_INTERRUPT     PlxInterrupt;
    PLX_DMA_PARAMS    DmaParams;
    PLX_PHYSICAL_MEM  PciBuffer;
    PLX_NOTIFY_OBJECT NotifyObject;
	FILE			  *frame; 
	VOID			  *pBuffer;
    U16				  k;
    U32				  rnd;
	U32				  header_len;
    U32				  file_len;
    U32				  pad_len;
    U32				  total_len;
    U8				  BDATA;
    char			  filename[64];
	U32				  TMP;


	// Get DMA buffer parameters
    PlxPci_CommonBufferProperties(
        pDevice,
        &PciBuffer
        );

	// Map the buffer into user space
	rc =
		PlxPci_CommonBufferMap(
		pDevice,
		&pBuffer
	);

	if (rc != ApiSuccess)	
	{
	// Error – Unable to map common buffer to user virtual space
		 Cons_printf("can not \n");
	}
    

for(rnd=0;rnd< FrameNum;rnd++) {

    sprintf(filename,"d:/dieying3_j2c/%06d.j2c",fileindex);
    Cons_printf("filename ==  %s \n",filename);

    // open frame file
    frame = fopen(filename,"rb");

   

	/********************************************************************************
	*   Construct Video Frame
	*********************************************************************************/

	
   
	header_len = 8;		// bytes

	fseek(frame,0,SEEK_SET);
	fseek(frame,0,SEEK_END);
	file_len = ftell(frame );
	fseek(frame,0,SEEK_SET);

	Cons_printf("file length ==  %08x \n",file_len);


	if(file_len%4)  pad_len = 4 - (file_len % 4);
	else 	      pad_len =  0;
	
		
	Cons_printf("pad_len ==  %x \n",pad_len);


    total_len = header_len + file_len + pad_len ;  // for DMA transfer byte counter

	Cons_printf("total_len ==  %x \n",total_len);

	
    // copy MAIK to buffer
    //*(U32*)((U8*)pBuffer + 0x0000) = 0x4B49444D; //0x4D44494B;   // 
	*(U8*)((U8*)pBuffer + 0) = 0x4D;  
	*(U8*)((U8*)pBuffer + 1) = 0x44;  
	*(U8*)((U8*)pBuffer + 2) = 0x49;  
	*(U8*)((U8*)pBuffer + 3) = 0x4B;  

    // copy Length to buffer
    //*(U32*)((U8*)pBuffer + 0x0004) = 0x9C910400			; //file_len;000491A4
	BDATA = file_len >> 24;
	*(U8*)((U8*)pBuffer + 4) = BDATA;  
	
	BDATA = file_len >> 16;
	*(U8*)((U8*)pBuffer + 5) = BDATA;  
	
	BDATA = file_len >> 8;
	*(U8*)((U8*)pBuffer + 6) = BDATA;  
	
	BDATA = file_len >> 0;
	*(U8*)((U8*)pBuffer + 7) = BDATA;  


	// copy file bytes to buffer
	fread((U8*)pBuffer + 8 ,file_len,1,frame);


    // 补4字节0 在最后, 总是4字节，不管是不是多余

	*(U8*)((U8*)pBuffer +  8 + file_len + 0)  = 0x00;
	*(U8*)((U8*)pBuffer +  8 + file_len + 1)  = 0x00;
	*(U8*)((U8*)pBuffer +  8 + file_len + 2)  = 0x00;
	*(U8*)((U8*)pBuffer +  8 + file_len + 3)  = 0x00;


	fclose(frame);


	///////////////////////////////////////////////////////////////////
	Cons_printf("round %d\n",rnd);
	 
    // Clear DMA structure
    memset(&DmaProp, 0, sizeof(PLX_DMA_PROP));

    // Initialize the DMA channel
    DmaProp.LocalBusWidth = 2;   // 32-bit
    DmaProp.ReadyInput    = 1;
    DmaProp.Burst		  = 1;
	DmaProp.BurstInfinite = 1;
	DmaProp.DoneInterrupt = 1;
	DmaProp.RouteIntToPci = 1;
	DmaProp.ConstAddrLocal = 1;
	DmaProp.DemandMode     = 1;

	// DMA Open
    rc =
        PlxPci_DmaChannelOpen(
            pDevice,
            0,
            &DmaProp
            );

    if (rc == ApiSuccess)
    {
        Cons_printf("Ok\n");
    }
    else
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }

	//
    Cons_printf("  Register for notification...... ");

    // Clear interrupt fields
    memset(&PlxInterrupt, 0, sizeof(PLX_INTERRUPT));

    // Setup to wait for either DMA channel
    PlxInterrupt.DmaDone = (1 << 0) | (1 << 1);

    rc =
        PlxPci_NotificationRegisterFor(
            pDevice,
            &PlxInterrupt,
            &NotifyObject
            );

    if (rc != ApiSuccess)
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }
    else
    {
        Cons_printf("Ok\n");
    }


    // Transfer the Data
    Cons_printf("  Perform Block DMA transfer..... ");

    // Clear DMA data
    memset(&DmaParams, 0, sizeof(PLX_DMA_PARAMS));

    DmaParams.PciAddr   = PciBuffer.PhysicalAddr;
    DmaParams.LocalAddr = 0x400;							// video address
    DmaParams.ByteCount = total_len;							// 1 frame
    DmaParams.Direction = PLX_DMA_PCI_TO_LOC;

    rc =
        PlxPci_DmaTransferBlock(
            pDevice,
            0,
            &DmaParams,
            0          // Don't wait for completion
            );

    if (rc == ApiSuccess)
    {
        Cons_printf("Ok\n");

        Cons_printf("  Wait for interrupt event....... ");

        rc =
            PlxPci_NotificationWait(
                pDevice,
                &NotifyObject,
                PLX_TIMEOUT_INFINITE								// wait forever
                );

        switch (rc)
        {
            case ApiSuccess:
                Cons_printf("Ok (DMA Int received)\n");
                break;

            case ApiWaitTimeout:
                Cons_printf("*ERROR* - Timeout waiting for Int Event\n");
                break;

            case ApiWaitCanceled:
                Cons_printf("*ERROR* - Interrupt event cancelled\n");
                break;

            default:
                Cons_printf("*ERROR* - API failed\n");
                PlxSdkErrorDisplay(rc);
                break;
        }
    }
    else
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }


    Cons_printf("  Check notification status...... ");
    rc =
        PlxPci_NotificationStatus(
            pDevice,
            &NotifyObject,
            &PlxInterrupt
            );

    if (rc == ApiSuccess)
    {
        Cons_printf("Ok (triggered ints:");

        if (PlxInterrupt.DmaDone & (1 << 0))
            Cons_printf(" DMA_0");

        if (PlxInterrupt.DmaDone & (1 << 1))
            Cons_printf(" DMA_1");

        Cons_printf(")\n");
    }
    else
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }



    // Release the interrupt wait object
    Cons_printf("  Cancelling Int Notification.... ");
    rc =
        PlxPci_NotificationCancel(
            pDevice,
            &NotifyObject
            );

    if (rc != ApiSuccess)
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }
    else
    {
        Cons_printf("Ok\n");
    }


    // Close DMA Channel
    Cons_printf("  Close DMA Channel.............. ");
    rc =
        PlxPci_DmaChannelClose(
            pDevice,
            0
            );

    if (rc == ApiSuccess)
    {
        Cons_printf("Ok\n");
    }
    else
    {
        Cons_printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }

   
	//////// round up to first frame //////////////
	


	if (fileindex != (FRAME_NUM - 1) )  {fileindex = fileindex + 1;}
	else { fileindex = 0;}

	
} // for(rnd=0

    // Unmap CommonBuffer
	rc =
		PlxPci_CommonBufferUnmap(
		pDevice,
		&pBuffer
	);
  
   

}












void delay () {
  U32 ll;

  for(ll=0;ll<100;ll++){};
}

