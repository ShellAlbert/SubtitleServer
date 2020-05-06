/*! \file mm_rc.h
File is property of &copy; MikroM GmbH, 1997-2005
\author Timo Kabsch (created 2005/03/03)

This file defines the global return code type \ref TMmRc, along with an enumeration denoting the error/warning codes. 
*/

#ifndef _MM_RC_H_
#define _MM_RC_H_

//! Denotes an return code to be a warning code. Using return codes without this macro denotes it to be an error code.
#define MM_WARNING(x) (-(x))

#define MM_IS_WARNING(x) ((x) > MMRC_Ok)
#define MM_IS_ERROR(x)   ((x) < MMRC_Ok)
#define MM_SUCCESS(x)   ((x) == MMRC_Ok)

/*! Enumeration of all return codes. */
enum EMmRc
{
	// global return codes
	MMRC_Ok														= 0,		//!< Denotes successful operation.
	MMRC_UnknownError                	         	= -1,		//!< Denotes an unknown error.
	MMRC_MemAllocateFailed           	         	= -2,		//!< Denotes an memory allocation error.
	MMRC_NotImplemented										= -3,		//!< Denotes an not implemented feature.
	MMRC_Timeout												= -4,		//!< Denotes a timeout
	MMRC_NotSupported                               = -5,		//!< Denotes a not supported mode or feature
	MMRC_FileOpenFailed                             = -6,		//!< Denotes an error in opening the file
	MMRC_DirectoryOpenFailed                        = -7,		//!< Denotes an error in reading a directory
	MMRC_NoThread												= -8,		
	MMRC_InvalidPointer										= -9,
	MMRC_DivisionByZero										= -10,
	MMRC_MemError												= -11,
	MMRC_PointerIsNull										= -12,
	MMRC_PointerMustBeNull									= -13,
	MMRC_UseError												= -14,
	MMRC_LogicalError											= -15,
	MMRC_RangeError											= -16,
	MMRC_WrongHandle                                = -17,
	MMRC_ReadSizeOutOfRange                         = -18,
	MMRC_AssignError                                = -19,   //!< Denotes an error in assignment (e.g. one device to two masters)
	MMRC_ExecuteError											= -20,
	MMRC_FileAlreadyExists									= -21,
	MMRC_WrongFileFormat										= -22,
	MMRC_WrongVersion											= -23,
   MMRC_Warning                	         	      = -24,    //!< Denotes that something is not perfect (but may fail)
   MMRC_FatalError                	         	   = -25,    //!< Denotes a fatal error (whole program must stop or be in error mode).
   MMRC_Busy                	         	         = -26,
   MMRC_OutOfMemory                                = -27,
	MMRC_Aborted												= -28,
	MMRC_ValueNotSet											= -29, 
	MMRC_DeviceIsInUse										= -30,
	MMRC_WriteFileFailed										= -31,
	MMRC_FileAlreadyOpened									= -32,
	MMRC_ParseError											= -33,
	MMRC_ArgumentOutOfRange									= -34,
	MMRC_DeleteFileFailed									= -35,
	MMRC_DeleteDirectoryFailed								= -36,
	MMRC_InvalidArgument										= -37,
	MMRC_ReadFileFailed										= -38,
	MMRC_SeekFileFailed										= -39,
	MMRC_UnknownAccessMode									= -40,
	MMRC_FileNotOpened										= -41,
	MMRC_AlignmentError  								   = -42,
	MMRC_CheckSystemError									= -43,   //!< Denotes a previously failed function call that wrote the global error variable, read errno at Linux and ECOS or call GetLastError() at Windows.

	MMRC_MM_API_ClientIdNotFound							= -1000,		
	MMRC_MM_API_NoMoreClientIds							= -1001,
	MMRC_MM_API_AccessDenied								= -1002,
	MMRC_MM_API_SendDataFailed								= -1003,
	MMRC_MM_API_ReceiveDataFailed							= -1004,
	MMRC_MM_API_IncompletePacket							= -1005,
	MMRC_MM_API_ComFunctionNotImplemented				= -1006,		//!< Client has not implemented currently called com function.
	MMRC_MM_API_ReceiveDataTruncated 					= -1007,		//!< Response data from client is to short.
	MMRC_MM_API_AnswerBufferToSmall						= -1008,
	MMRC_MM_API_ParseError									= -1009,
	MMRC_MM_API_InvalidHandle								= -1010,
	MMRC_MM_API_ArgQuantityOutOfRange               = -1011,
	MMRC_MM_API_IndexQuantityOutOfRange             = -1012,
	MMRC_MM_API_ReadSizeOutOfRange                  = -1013,		//!< Denotes that the read size is wrong (to short)
	
	MMRC_MM_API_Arg1OutOfRange								= -1801,
	MMRC_MM_API_Arg2OutOfRange								= -1802,
	MMRC_MM_API_Arg3OutOfRange								= -1803,
	MMRC_MM_API_Arg4OutOfRange								= -1804,
	MMRC_MM_API_Arg5OutOfRange								= -1805,
	MMRC_MM_API_Arg6OutOfRange								= -1806,
	MMRC_MM_API_Arg7OutOfRange								= -1807,
	MMRC_MM_API_Arg8OutOfRange								= -1808,
	MMRC_MM_API_Arg9OutOfRange								= -1809,
	
	MMRC_MM_API_Index1OutOfRange							= -1901,
	MMRC_MM_API_Index2OutOfRange							= -1902,
	MMRC_MM_API_Index3OutOfRange							= -1903,
	MMRC_MM_API_Index4OutOfRange							= -1904,
	MMRC_MM_API_Index5OutOfRange							= -1905,
	MMRC_MM_API_Index6OutOfRange							= -1906,
	MMRC_MM_API_Index7OutOfRange							= -1907,
	MMRC_MM_API_Index8OutOfRange							= -1908,
	MMRC_MM_API_Index9OutOfRange							= -1909,
																	
	MMRC_MM_COM_TimeOut					            	= -2000,
	MMRC_MM_COM_ReceiveBufferToSmall						= -2001,
	MMRC_MM_COM_UnknownProtocol							= -2002,
	MMRC_MM_COM_UnknownPort									= -2003,
	MMRC_MM_COM_NoAck											= -2004,
	MMRC_MM_COM_InvalidHandle								= -2005,
	MMRC_MM_COM_NoConnection                        = -2006,
	MMRC_MM_COM_NoExclusvieAccess                   = -2007,
	MMRC_MM_COM_IsAlreadyConnected                  = -2008,
	MMRC_MM_COM_MessageIdMismatch							= -2009,
	MMRC_MM_COM_MessageTypeMismatch						= -2010,
	MMRC_MM_COM_Pending                             = -2011,		//!< it's not an error, denotes the message is still pending and will be returned later

	MMRC_UART_NotInitialized								= -3000,
	MMRC_UART_ReadTimeout									= -3001,
	MMRC_UART_FifoOverflow									= -3002,
	MMRC_UART_FifoUnderrun									= -3003,
	MMRC_UART_SameBaudrate									= -3004,
	MMRC_UART_RangeError										= -3005,

	MMRC_FIFO_Overflow										= -4000,
	MMRC_FIFO_Underrun										= -4001,

	MMRC_ADEC_WriteFailed									= -5000,
	MMRC_ADEC_ReadFailed										= -5001,
	MMRC_ADEC_ReadTimeout									= -5002,
	MMRC_ADEC_UnexpectedData								= -5003,

	MMRC_I2C_AccessFailed									= -6000,

	MMRC_LCD_DataWriteTimeout								= -7000,
	MMRC_LCD_CommandWriteTimeout							= -7001,

	MMRC_FLASH_UnknownType									= -8000,
	MMRC_FLASH_EraseTimeout									= -8001,
	MMRC_FLASH_SectorLockFailed							= -8002,
	MMRC_FLASH_SectorUnlockFailed							= -8003,
	MMRC_FLASH_WriteTimeout									= -8004,
	
	MMRC_PLAYBACK_IsAlreadyPlaying                  = -9000,
	MMRC_PLAYBACK_IsNotPlaying                      = -9001,
	MMRC_PLAYBACK_AquireHardwareFailed              = -9002,
	MMRC_PLAYBACK_CreatingThreadFailed              = -9003,
	MMRC_PLAYBACK_SplicingDisabled                  = -9004,
	MMRC_PLAYBACK_UnknownFileFormat                 = -9005,
	MMRC_PLAYBACK_WrongSplicingMode                 = -9006,
	MMRC_PLAYBACK_NoNewFile                         = -9007,
	MMRC_PLAYBACK_NoDevice                          = -9008,
	MMRC_PLAYBACK_TimeDiffToHigh							= -9009,
	MMRC_PLAYBACK_DevicesNotReady							= -9010,
	MMRC_PLAYBACK_BlackModeNotSupported					= -9011,
	MMRC_PLAYBACK_IsPlayingSyncedBlackframes	      = -9012,
	
	MMRC_VIDEO_ManualVideoModeNotSet                = -10001,


	MMRC_PLAYLIST_ClipDontBelongToActiveScene								= -11001,
	MMRC_PLAYLIST_ClipDontBelongToThisDevice								= -11002,
	MMRC_PLAYLIST_ClipIsNotValid												= -11003,
	MMRC_PLAYLIST_ClipsDontHaveCycles										= -11004,
	MMRC_PLAYLIST_CurrentCounterCantBeHigherAsMaxCounter				= -11005,
	MMRC_PLAYLIST_DeviceCallBackPointerIsNull								= -11006,
	MMRC_PLAYLIST_DeviceFinishedScene										= -11007,
	MMRC_PLAYLIST_DeviceHasNoName												= -11008,
	MMRC_PLAYLIST_DevicesDontHaveCycles										= -11009,
	MMRC_PLAYLIST_ErrorInAttributeAttributeWasDeleted					= -11010,
	MMRC_PLAYLIST_IdCantBeZero													= -11011,
	MMRC_PLAYLIST_IdMustBeZero													= -11012,
	MMRC_PLAYLIST_InvalidDeviceID												= -11013,
	MMRC_PLAYLIST_InvalidObjectPosition		 								= -11014,
	MMRC_PLAYLIST_KeyWordIsNoEvent											= -11015,
	MMRC_PLAYLIST_MainAndDeviceCallBackPointerAreNull					= -11016,
	MMRC_PLAYLIST_MainCallBackPointerIsNull								= -11017,
	MMRC_PLAYLIST_MainLoopCantBelongToDevice								= -11018,
	MMRC_PLAYLIST_MainLoopCantBelongToScene								= -11019,
	MMRC_PLAYLIST_MainLoopDontHaveParent									= -11020,
	MMRC_PLAYLIST_MaxCounterCantBeLowerAsCurrentCounter				= -11021,
	MMRC_PLAYLIST_NoActiveClipForThisDeviceAtThisMoment				= -11022,
	MMRC_PLAYLIST_NoActiveSceneAtThisMoment								= -11023,
	MMRC_PLAYLIST_NoNewClipForThisDeviceAtThisMoment					= -11024,
	MMRC_PLAYLIST_NoRootObjectDefinedPlayListWillBeEmpty				= -11025,
	MMRC_PLAYLIST_ObjectIsInUnassignedObjectsLoop						= -11026,
	MMRC_PLAYLIST_ObjectIsNotAClip											= -11027,
	MMRC_PLAYLIST_ObjectIsNotADevice											= -11028,
	MMRC_PLAYLIST_ObjectIsNotALoop											= -11029,
	MMRC_PLAYLIST_ObjectIsNotAMainLoop										= -11030,
	MMRC_PLAYLIST_ObjectIsNotAScene											= -11031,
	MMRC_PLAYLIST_ObjectIsNotASubPlaylist									= -11032,
	MMRC_PLAYLIST_ObjectIsNotASubTitle										= -11033,
	MMRC_PLAYLIST_ObjectIsNotInLoop											= -11034,
	MMRC_PLAYLIST_ObjectIsNotInPlayList										= -11035,
	MMRC_PLAYLIST_ObjectIsNotSubObject										= -11036,
	MMRC_PLAYLIST_ObjectNotFound												= -11037,
	MMRC_PLAYLIST_ObjectsHaveNoCommonParrent								= -11038,
	MMRC_PLAYLIST_OldSubObjectWasBeenRemoved								= -11039,
	MMRC_PLAYLIST_OnlyDeviceCanBeSubObjectFromScene						= -11040,
	MMRC_PLAYLIST_OnlyLoopOrSubPlayListCanBeSubObjectFromDevice		= -11041,
	MMRC_PLAYLIST_OnlySceneOrScriptListCanBeSubObjectFromMainLoop	= -11042,
	MMRC_PLAYLIST_PlayListEnd													= -11043,
	MMRC_PLAYLIST_PlayListInOutFormatIsOnlyToWrite						= -11044,
	MMRC_PLAYLIST_PlayListIsInconsistent 									= -11045,
	MMRC_PLAYLIST_PlayListStart												= -11046,
	MMRC_PLAYLIST_RootObjectCantBeSubObject								= -11047,
	MMRC_PLAYLIST_SameDeviceExistSeveralInTheSameScene					= -11048,
	MMRC_PLAYLIST_SceneCantBelongToDevice									= -11049,
	MMRC_PLAYLIST_SubObjectFromClipMustBeSubTitle						= -11050,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeDevice							= -11051,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeMainLoop						= -11052,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeScene							= -11053,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeSubTitle						= -11054,
	MMRC_PLAYLIST_SubObjectFromSubPlayListMustBeLoop					= -11055,
	MMRC_PLAYLIST_SubObjectFromSubtitleNotPossible						= -11056,
	MMRC_PLAYLIST_SubPlayListsDontHaveCycles								= -11057,
	MMRC_PLAYLIST_SubtitlesDontHaveCycles									= -11058,
	MMRC_PLAYLIST_SubTitlesDontHaveSubObjects								= -11059,
	MMRC_PLAYLIST_ThisDeviceIsDisabledAtTheMoment						= -11060,
	MMRC_PLAYLIST_ThisDeviceIsNotPlayingAtTheMoment						= -11061,
	MMRC_PLAYLIST_ThisDeviceIsPlayingAtTheMoment							= -11062,
	MMRC_PLAYLIST_ThisDeviceNameDontExist									= -11063,
	MMRC_PLAYLIST_ThisInfoDontExist											= -11064,
	MMRC_PLAYLIST_ThisLinkageMakesTheTreeInconsistent					= -11065,
	MMRC_PLAYLIST_ThisSubObjectDontExist									= -11066,
	MMRC_PLAYLIST_TooManyObjects												= -11067,
	MMRC_PLAYLIST_UnassignedObjectLoopCantBeMoved						= -11068,
	MMRC_PLAYLIST_XmlFormatError												= -11069,
	MMRC_PLAYLIST_XmlNoFile														= -11070,
	MMRC_PLAYLIST_XmlNoObject													= -11071,
	MMRC_PLAYLIST_XmlNoParser													= -11072,
	MMRC_PLAYLIST_XmlUnknownElement											= -11073,
	MMRC_PLAYLIST_EventKeyWordHasIncorrectSyntax							= -11074,
	MMRC_PLAYLIST_DeviceNameMustBeUnique									= -11075,
	MMRC_PLAYLIST_PlaylistIsAComplexPlayList								= -11076,
	MMRC_PLAYLIST_UndefinedSyncMode											= -11077,
	MMRC_PLAYLIST_NoUuidDefined												= -11078,
	MMRC_PLAYLIST_SameDeviceExistSeveralInTheSameGroup					= -11079,
	MMRC_PLAYLIST_SameClipNameExistSeveralAtTheSameDevice				= -11080,
	MMRC_PLAYLIST_SpacerAreNotSupported										= -11081,
	MMRC_PLAYLIST_PlaylistIsASubPlayList									= -11082,
	MMRC_PLAYLIST_PlaylistIsNotASubPlayList								= -11083,
	MMRC_PLAYLIST_OldRootObjectHasBeenRemoved								= -11084,
	MMRC_PLAYLIST_ErrorInPlayListStructure									= -11085,
	MMRC_PLAYLIST_PlayListActualAndMaxLengthIsEndless					= -11086,
	MMRC_PLAYLIST_PlayListMaxLengthIsEndless								= -11087,
	MMRC_PLAYLIST_PlayListSomeClipLengthAreUndefined					= -11088,
	MMRC_PLAYLIST_DocumentIsNotAPlayList									= -11089,
	MMRC_PLAYLIST_UnknownPlayListVersion									= -11090,
	MMRC_PLAYLIST_ViewDocumentIsAUnknownVersionOfXmlPLViewStream	= -11091,
	MMRC_PLAYLIST_ViewDocumentIsNotAXmlPLViewStream						= -11092,
	MMRC_PLAYLIST_ViewXmlPLViewStreamTagError								= -11093,
	MMRC_PLAYLIST_ViewXmlPLViewStreamErrorInValue						= -11094,
	MMRC_PLAYLIST_ViewDocumentUnknownErrorInXmlPLViewStream			= -11095,
	MMRC_PLAYLIST_PlaylistIsNotMemberOfThisGroup                   = -11096,
	MMRC_PLAYLIST_ScriptsDontHaveSubObjects								= -11097,
	MMRC_PLAYLIST_SubObjectFromScriptNotPossible							= -11098,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeScript							= -11099,
 	MMRC_PLAYLIST_ScriptsDontHaveCycles										= -11100,
 	MMRC_PLAYLIST_OnlyScriptCanBeSubObjectFromScriptList				= -11101,
 	MMRC_PLAYLIST_ScriptCantBelongToDevice									= -11102,
	MMRC_PLAYLIST_ScriptListCantBelongToDevice							= -11103,
	MMRC_PLAYLIST_ObjectIsNotAScript											= -11104,
	MMRC_PLAYLIST_ThisScriptNameDontExist									= -11105,
	MMRC_PLAYLIST_ScriptCantBelongToScene									= -11106,
	MMRC_PLAYLIST_ScriptListCantBelongToScene								= -11107,
	MMRC_PLAYLIST_ScriptListCantRunInLoop									= -11108,
	MMRC_PLAYLIST_SubObjectFromLoopCantBeScriptList						= -11109,
	MMRC_PLAYLIST_ScriptListIsNotActive										= -11110,
	MMRC_PLAYLIST_NoActiveScript												= -11111,
	MMRC_PLAYLIST_ObjectIsNotAScriptList									= -11112,
	MMRC_PLAYLIST_ThisScriptIsDisabledAtTheMoment						= -11113,
	MMRC_PLAYLIST_ThisScriptListIsDisabledAtTheMoment					= -11114,
	MMRC_PLAYLIST_ScriptDontBelongToActivScriptList						= -11115,
	MMRC_PLAYLIST_ActiveScriptIsRunning										= -11116,
	MMRC_PLAYLIST_ErrorStartingScript										= -11117,
	MMRC_PLAYLIST_OnlyDeviceCanBeSubObjectScene							= -11118,

	MMRC_ROUTER_Error											= -12000,
	MMRC_ROUTER_EmptyAddress								= -12001,
	MMRC_ROUTER_UnknownAddress								= -12002,
	MMRC_ROUTER_NoMatch										= -12003,
	MMRC_ROUTER_ErrorGetOwnAddress						= -12004,
	MMRC_ROUTER_UnknownProtocolId							= -12005,
	MMRC_ROUTER_UnknownProtocolVersion					= -12006,
	MMRC_ROUTER_ProtocolViolation							= -12007,
	MMRC_ROUTER_Fragmented									= -12008,
	MMRC_ROUTER_Unchanged									= -12009,

	MMRC_LIST_EndOfList										= -13000,
	MMRC_LIST_NotFound										= -13001,
	MMRC_LIST_NoList											= -13002,

	MMRC_INET_NoSocket										= -14000,
	MMRC_INET_BindFailed										= -14001,
	MMRC_INET_ListenFailed									= -14002,
	MMRC_INET_SetOptFailed									= -14003,
	MMRC_INET_InvalidAddress								= -14004,
	MMRC_INET_HangUp                                = -14005,
	MMRC_INET_CheckErrno                            = -14006,

	MMRC_MVC_OpenFailed										= -15000,
	MMRC_MVC_DeviceClassAccessFailed						= -15001,
	MMRC_MVC_IoControlAnswerSizeMismatch				= -15002,
	MMRC_MVC_CardIndexOutOfRange							= -15003,
	MMRC_MVC_DeviceAlreadyLocked							= -15004,
	MMRC_MVC_DeviceIsNotLocked								= -15005,
	MMRC_MVC_IoAnswerBufferToSmall						= -15006,
	MMRC_MVC_DeviceNotAcquired								= -15007,
	MMRC_MVC_DeviceCouldNotBeAcquired						= -15008,
	MMRC_MVC_UnsupportedCommInterface					= -15009,
	MMRC_MVC_WrongFileObject								= -15010,
	MMRC_MVC_NoUpdateRunning								= -15011,
	MMRC_MVC_GetSemaphoreFailed							= -15012,
	MMRC_MVC_TransferError									= -15013,
	MMRC_MVC_TransferTimeout								= -15014,
	MMRC_MVC_InvalidHandle									= -15015,
	MMRC_MVC_ForbiddenThread								= -15016,
	MMRC_MVC_BufferAlignmentError							= -15017,
	MMRC_MVC_CardStillPlaying								= -15018,
	MMRC_MVC_CardAlreadyPlaying							= -15019,
	MMRC_MVC_CardNotPlaying									= -15020,
	MMRC_MVC_ReleaseHardwareFailed						= -15021,
	MMRC_MVC_UnsupportedSplicingMode						= -15022,
	MMRC_MVC_CommFifoSemaphoreTimeout					= -15023,
	MMRC_MVC_CommFifoWaitOnWriteSpaceTimeout			= -15024,
	MMRC_MVC_CommFifoWaitOnReadDataTimeout				= -15025,
	MMRC_MVC_CommFifoUnexpectedPayload					= -15026,
	MMRC_MVC_CommFifoUnexpectedPayloadSize				= -15027,
	MMRC_MVC_CommFifoUnexpectedStartCode				= -15028,

	MMRC_XML_NoParser											= -16000,
	MMRC_XML_NoFile											= -16001,
	MMRC_XML_UnknownElement									= -16002,
	MMRC_XML_UnknownAttribute								= -16003,
	MMRC_XML_InvalidAttrValue								= -16004,
	MMRC_XML_FormatError										= -16005,
	MMRC_XML_ReadFileFailed									= -16006,
	MMRC_XML_RenameFileFailed								= -16007,
	MMRC_XML_WriteFileFailed								= -16008,
	MMRC_XML_WriteFlashConfigFileFailed					= -16009,
   MMRC_XML_WriteConfigFileFailed						= -16010,
   MMRC_XML_ElementMissing       						= -16011,
   MMRC_XML_WrongDocumentType      						= -16012,
   MMRC_XML_UnknownDocumentType   						= -16013,
   MMRC_XML_WrongDocumentVersion      					= -16014,
   MMRC_XML_UnknownDocumentVersion 						= -16015,
	MMRC_XML_ErrorInAttribute								= -16016,
	MMRC_XML_DocumentModified								= -16017,

	MMRC_DEVICE_IsAlreadyAssignedToMaster				= -17000,

	MMRC_HOS														= -18000,
	MMRC_HOS_StiOutOfRange									= -18001,
	MMRC_HOS_UnknownIrq										= -18002,
	MMRC_HOS_TimerNotFound									= -18003,
	MMRC_HOS_DeviceIndexOutOfRange						= -18004,
	MMRC_HOS_DeviceWriteError								= -18005,
	MMRC_HOS_DeviceReadError								= -18006,
	MMRC_HOS_Timeout											= -18007,
	MMRC_HOS_UnexpectedData									= -18008,
	MMRC_HOS_ImageCrcError									= -18009,
	MMRC_HOS_ImageUnknownType								= -18010,
	MMRC_HOS_ImageDependencyError							= -18011,
	MMRC_HOS_ImageSizeError									= -18012,
	MMRC_HOS_StreamScanning									= -18013,
	MMRC_HOS_StreamNoNewVersion							= -18014,
	MMRC_HOS_StreamScanTimeout								= -18015,
	MMRC_HOS_StreamCrcFailure								= -18016,
	MMRC_HOS_StreamScanSlotsFull							= -18017,

	MMRC_GROUP_GroupNotEmpty								= -19000,
	MMRC_GROUP_NoGroupConfigFile							= -19001,
	MMRC_GROUP_OldGroupConfigFile							= -19002,
	MMRC_GROUP_UnknownElement								= -19003,
	MMRC_GROUP_GroupNameDoNotExist						= -19004,

	MMRC_DISPLAY_NoChildsAvailable								= -20000,
	MMRC_DISPLAY_UnknownChildNumber								= -20001,
	MMRC_DISPLAY_InvalidChildNumber								= -20002,
	MMRC_DISPLAY_InvalidButtonNumber								= -20003,
	MMRC_DISPLAY_NoButtonsAvailable								= -20004,
	MMRC_DISPLAY_UnknownButtonNumber								= -20005,
	MMRC_DISPLAY_InvalidSubWindowNumber							= -20006,
	MMRC_DISPLAY_NoSubWindowsAvailable							= -20007,
	MMRC_DISPLAY_UnknownSubWindowNumber							= -20008,
	MMRC_DISPLAY_CantInsertSubWindow								= -20009,
	MMRC_DISPLAY_DifferentColorMode								= -20010,
	MMRC_DISPLAY_WrongColorMode									= -20011,
	MMRC_DISPLAY_ColorModeIsUndefined							= -20012,
	MMRC_DISPLAY_ThisWindowIsNotASubwindow						= -20013,
	MMRC_DISPLAY_VissibleSectionLeftTheWindow					= -20014,
	MMRC_DISPLAY_NoGraphicBufferAvailable						= -20015,
   MMRC_DISPLAY_ColorTableFull               		      = -20016,
   MMRC_DISPLAY_AreaOutOfRange               		      = -20017,
	MMRC_DISPLAY_PixelOutOfRange									= -20018,
	MMRC_DISPLAY_InvalidMenuItemType								= -20019,
	MMRC_DISPLAY_NoVisibleSubItem									= -20020,
	MMRC_DISPLAY_NoPreviousVisibleItem							= -20021,
	MMRC_DISPLAY_NoNextVisibleItem								= -20022,
	MMRC_DISPLAY_NoSelectableSubItem								= -20023,
	MMRC_DISPLAY_NoPreviousSelectableItem						= -20024,
	MMRC_DISPLAY_NoNextSelectableItem							= -20025,
	MMRC_DISPLAY_VissibleSectionCantBeBiggerAsTheWindow	= -20026,
	MMRC_DISPLAY_NoParentWindowAvailable						= -20027,
	MMRC_DISPLAY_GraphicBuffersAreDifferent					= -20028,
	MMRC_DISPLAY_NoSecondGraphicBufferAvailable				= -20029,
	MMRC_DISPLAY_ThisScreenDoNotExist							= -20030,
	MMRC_DISPLAY_InvalidScreenNumber								= -20031,
	MMRC_DISPLAY_NoScreenAvailable								= -20032,
	MMRC_DISPLAY_MenuCallBackPointerIsNull						= -20033,
	MMRC_DISPLAY_ThisScreenManagerDoNotExist					= -20034,
	MMRC_DISPLAY_InvalidScreenManagerNumber					= -20035,
	MMRC_DISPLAY_NoScreenManagerAvailable						= -20036,

	MMRC_MVC2															= -21000,
	MMRC_MVC2_DeviceNotFound										= -21001,
	MMRC_MVC2_DeviceBusy												= -21002,
	MMRC_MVC2_MemAllocateFailed									= -21003,
	MMRC_MVC2_EndOfStream											= -21004,
//	MMRC_MVC2_TimeOut													= -21005,			// redundant with MMRC_TimeOut
	MMRC_MVC2_NeededBufferSizeTooBig								= -21006,
	MMRC_MVC2_BufferInactive										= -21007,
	MMRC_MVC2_IoControlWrongParameter							= -21008,
	MMRC_MVC2_IoControlReturnSizeMismatch						= -21009,
	MMRC_MVC2_IoControlDefaultRc									= -21010,
	MMRC_MVC2_IoControlPending										= -21011,
	MMRC_MVC2_IoControlTimeOut										= -21012,
	MMRC_MVC2_IoControlCallFailed									= -21013,
	MMRC_MVC2_LogClientIdWrong										= -21014,
	MMRC_MVC2_NoMoreLogMessages									= -21015,
	MMRC_MVC2_NoDeviceIdLeft										= -21016,
	MMRC_MVC2_OpenFailed												= -21017,
	MMRC_MVC2_WrongDriverVersion									= -21018,
	MMRC_MVC2_BadPowerSupply										= -21019,
	MMRC_MVC2_BootImageNotLoaded									= -21020,
	MMRC_MVC2_LogMessageBufferTooSmall							= -21021,
	MMRC_MVC2_DecoderTypeUnknown									= -21022,
	MMRC_MVC2_OutOfDecoderResources								= -21023,
	MMRC_MVC2_DecoderNotFound										= -21024,
	MMRC_MVC2_OutputNotFound										= -21025,
	MMRC_MVC2_OutputConnectionError								= -21026,
	MMRC_MVC2_OutOfStreamMemory									= -21027,
	MMRC_MVC2_OutOfDMASlots											= -21028,
	MMRC_MVC2_OutputAlreadyConnected								= -21029,
	MMRC_MVC2_OutOfOutputResources								= -21030,
	MMRC_MVC2_WrongDecoderType										= -21031,
	MMRC_MVC2_VideoModeNotSupported								= -21032,
	MMRC_MVC2_PlaybackNotFound										= -21033,
	MMRC_MVC2_OutOfPlaybackResources								= -21034,
	MMRC_MVC2_DecoderAlreadyConnected							= -21035,
	MMRC_MVC2_DecoderNotConnected									= -21036,
	MMRC_MVC2_TestDataSendError									= -21037,
	MMRC_MVC2_TestDataReceiveError								= -21038,
	MMRC_MVC2_ConfigAccessFailed									= -21039,
	MMRC_MVC2_ConfigRangeError										= -21040,
	MMRC_MVC2_FPGAProgrammingFailed								= -21041,
	MMRC_MVC2_UnexpectedData										= -21042,
	MMRC_MVC2_FlashError												= -21043,
   MMRC_MVC2_InvalidObject								         = -21044,
	MMRC_MVC2_SecurityUpdateRejected					         = -21045,
	MMRC_MVC2_SecurityUpdateFlashError							= -21046,
	MMRC_MVC2_SecurityAccessDenied								= -21047,
	MMRC_MVC2_FirmwareLoadFailed									= -21048,
	MMRC_MVC2_GPIO_AccessFailed									= -21049,
	MMRC_MVC2_InvalidProgressInfo									= -21050,
	MMRC_MVC2_OperationPending										= -21051,
	MMRC_MVC2_FirmwareLoadPending									= -21052,
	MMRC_MVC2_AppletInstallFailed									= -21053,
	MMRC_MVC2_DeviceKeyInvalid										= -21054,
	MMRC_MVC2_SampleFrequencyNotSupported						= -21055,
	MMRC_MVC2_NewClipAlreadySet									= -21056,
	MMRC_MVC2_EndOfClip												= -21057,
	MMRC_MVC2_UnknownStreamType									= -21058,
	MMRC_MVC2_TooManyFramesInBuffer								= -21059,
	MMRC_MVC2_BufferAlreadySent									= -21060,
	MMRC_MVC2_IncompleteRenderCommand							= -21061,
	MMRC_MVC2_RenderBufferFull										= -21062,
	MMRC_MVC2_MemoryTestFailed										= -21063,
	MMRC_MVC2_FirmwareRejected_ELFCorrupt						= -21064,
	MMRC_MVC2_FirmwareRejected_NoELFSupport					= -21065,
	MMRC_MVC2_FirmwareRejected_ChecksumFailed					= -21066,
	MMRC_MVC2_FirmwareRejected_FailedSMConnection			= -21067, //!< only on rev A and B without SM loading a MM2 file
	MMRC_MVC2_FirmwareRejected_PlainFirmware					= -21068,
	MMRC_MVC2_FirmwareRejected_MM2Corrupt						= -21069,
	MMRC_MVC2_FirmwareRejected_SizeMismatch					= -21070,
	MMRC_MVC2_FirmwareRejected_SystemMismatch					= -21071,
	MMRC_MVC2_FirmwareRejected_SignatureError					= -21072,
	MMRC_MVC2_FirmwareRejected_SMStartCommandFailed			= -21073,
	MMRC_MVC2_FirmwareRejected_SMStartDataFailed				= -21074,
	MMRC_MVC2_FirmwareRejected_SMStartWrongRunstate			= -21075,
	MMRC_MVC2_FirmwareRejected_SMFWKeyMSentFailed			= -21076,
	MMRC_MVC2_FirmwareRejected_SMFWKeyReceiveFailed			= -21077,
	MMRC_MVC2_FirmwareRejected_SMFWKeyDenied					= -21078,
	MMRC_MVC2_FirmwareRejected_WrongHashSize					= -21079,
	MMRC_MVC2_FirmwareRejected_WrongSigSize					= -21080,
	MMRC_MVC2_FirmwareRejected_SMSigSentFailed				= -21081,
	MMRC_MVC2_FirmwareRejected_SMSigReceiveFailed			= -21082,
	MMRC_MVC2_FirmwareRejected_AuthenticationFailed			= -21083,
	MMRC_MVC2_ElementNotFound										= -21084,
	MMRC_MVC2_XMLParseError											= -21085,
	MMRC_MVC2_WrongXMLType											= -21086,
	MMRC_MVC2_OutputNotConnected									= -21087,
	MMRC_MVC2_Authentication_Failed								= -21088,
	MMRC_MVC2_Authentication_Access_Denied						= -21089,
	MMRC_MVC2_Authentication_Unknown								= -21090,
	MMRC_MVC2_IP_WrongPacketVersion								= -21091,
	MMRC_MVC2_IP_ConnectionClosed									= -21092,
	MMRC_MVC2_IP_SendFailed											= -21093,
	MMRC_MVC2_IP_RecvFailed											= -21094,
	MMRC_MVC2_IP_SendSizeMismatch									= -21095,
	MMRC_MVC2_IP_RecvSizeMismatch									= -21096,
	MMRC_MVC2_ColorConversionNotPossible						= -21097,
	MMRC_MVC2_FirmwareStartPending								= -21098,
	MMRC_MVC2_ConfigAccessDenied									= -21099,
	MMRC_MVC2_FirmwareRejected_CRCError							= -21100,
	MMRC_MVC2_FirmwareRejected_SelftestFailed					= -21101,
	MMRC_MVC2_TestFailed												= -21102, //!< CAVP Test failed
	MMRC_MVC2_TestOrderWrong										= -21103, //!< the order of the CAVP test calls are wrong
	MMRC_MVC2_MissingUpdateFile									= -21104,
	MMRC_MVC2_FileSystemError										= -21105,
	MMRC_MVC2_UnknownCPL												= -21106,
	MMRC_MVC2_SocketOpenFailed										= -21107,
	MMRC_MVC2_SocketConnectFailed									= -21108,
	MMRC_MVC2_SM_StartFailed										= -21109,
	MMRC_MVC2_SM_NotConnected										= -21110,
	MMRC_MVC2_SM_AlreadyConnected									= -21111,
	MMRC_MVC2_SM_ConnectionLost									= -21112,
	MMRC_MVC2_SM_RRPFailed											= -21113,
	MMRC_MVC2_SM_RRPInvalid											= -21114,
	MMRC_MVC2_SM_ResponderBusy										= -21115,
	MMRC_MVC2_SM_BadRequestResponse								= -21116,
	MMRC_MVC2_SM_UnexpectedResponse								= -21117,
	MMRC_MVC2_SM_ITMSizeMismatch									= -21118,
	MMRC_MVC2_NoNetworkInfo											= -21119,
	MMRC_MVC2_SM_WrongRequestId									= -21120,
	MMRC_MVC2_SM_ITMStructureError								= -21121,
	MMRC_MVC2_SocketNameFailed										= -21122,
	MMRC_MVC2_KeyIdNotFound											= -21123,
	MMRC_MVC2_SM_InvalidObject										= -21124,
	MMRC_MVC2_FirmwareRejected_WrongKey							= -21125,
	MMRC_MVC2_FirmwareRejected_DecryptionFailed				= -21126,
	MMRC_MVC2_FirmwareRejected_UnknownSignKey					= -21127,
	MMRC_MVC2_FirmwareRejected_SignVerifyFailed				= -21128,
	MMRC_MVC2_FirmwareRejected_SignatureFailed				= -21129,
	MMRC_MVC2_FeatureNotActivated									= -21130,
	MMRC_MVC2_NoSyncMaster											= -21131,
	MMRC_MVC2_OutOfConnectionResources							= -21132,
	MMRC_MVC2_ConnectionNotFound									= -21133,
	MMRC_MVC2_InputNotFound											= -21134,
	MMRC_MVC2_OutOfInputResources									= -21135,
	MMRC_MVC2_FirmwareRejected_Downgrade						= -21136,
	MMRC_MVC2_FirmwareRejected_UUIN_SizeMismatch          = -21137, //!< update user info to big
	MMRC_MVC2_UpdateHeaderTransmFailed							= -21138, //!< transmission of update header failed
	MMRC_MVC2_UpdateHeaderNoAdmission							= -21139, //!< no admission for update header received
	MMRC_MVC2_UpdateHeaderRejected								= -21140,
	MMRC_MVC2_UpdateImageTransferFailed                   = -21141, //!< transmission of update image failed
	MMRC_MVC2_UpdateImageNoStatus									= -21142, //!< no status for update image installation received
	MMRC_MVC2_IP_ConnectionNotFound								= -21143,
	MMRC_MVC2_IP_MemoryBufferNotFound							= -21144,
	MMRC_MVC2_MissingFPGAFunction									= -21145,
	MMRC_MVC2_InputInactive											= -21146, //!< Video Input is not activated
	

	MMRC_APPLET_API													= -22000,
	MMRC_APPLET_API_UnknowError									= -22001,
	
	MMRC_APPLET_API_RSATimeout                            = -22021, //!< timeout at receiving the response
	MMRC_APPLET_API_RSAWrongChaining                      = -22022, //!< illegal chaining in both direction requested
	MMRC_APPLET_API_RSAUnknownErr 								= -22023, //!< unknown smartcard error
	MMRC_APPLET_API_RSAParaErr                            = -22024, //!< illegal parameter
	MMRC_APPLET_API_RSAStatusErr                          = -22025, //!< smartcard returns error code, see at separate return code
	MMRC_APPLET_API_WrongValueIndex                       = -22026,
	MMRC_APPLET_API_ValueBitsNotInitialized					= -22027,
	MMRC_APPLET_API_SelectError                           = -22028,
	MMRC_APPLET_API_SomeValueBitsAlreadyInitialized       = -22029,
	MMRC_APPLET_API_RSARecvSizeErr                        = -22030,
	MMRC_APPLET_API_DeviceNotReady                        = -22031, //!< was not initialized before use
	MMRC_APPLET_API_SC_CardCryptogramCheckFailed          = -22032, //!< applet secure channel card cryptogram check failed

	MMRC_SMARTCARD_INIT_RSAInitNoInitErr						= -22101,
	MMRC_SMARTCARD_INIT_ATRMissed									= -22102,
	MMRC_SMARTCARD_INIT_ATRWrongChecksum						= -22103,
	MMRC_SMARTCARD_INIT_ATRWrongSize								= -22104,
	MMRC_SMARTCARD_INIT_TSByteUnknown							= -22105,
	MMRC_SMARTCARD_INIT_WrongDeviceId							= -22106,
	MMRC_SMARTCARD_INIT_WrongDeviceNumber						= -22107,
	MMRC_SMARTCARD_INIT_PTSErr										= -22108,
	MMRC_SMARTCARD_INIT_IdentifyErr								= -22109,
	MMRC_SMARTCARD_INIT_SpeedSwitchErr							= -22110,
	MMRC_SMARTCARD_INIT_UnknownErr								= -22111,

	MMRC_SMARTCARD_CONF_PrewriteErr                       = -22130,
	MMRC_SMARTCARD_CONF_WriteErr                          = -22131,
	MMRC_SMARTCARD_CONF_FuseErr                           = -22132,
	MMRC_SMARTCARD_CONF_SelectRootErr                     = -22133,
	MMRC_SMARTCARD_CONF_BootErr                           = -22134,

	MMRC_RTC_API_ArgumentErr										= -22151,
	MMRC_RTC_API_InitFailed											= -22152,
	MMRC_RTC_API_OutOfRange											= -22153,
	MMRC_RTC_API_WriteFailure									   = -22154,
	MMRC_RTC_API_Power1Failure										= -22155, //!< cover Button with direct line to security controller is open
	MMRC_RTC_API_Power2Failure										= -22156, //!< power fail bit at RTC is activated
	MMRC_RTC_API_ChecksumErr										= -22157,
	MMRC_RTC_API_NoSetupTime										= -22158, //!< no setup time was previously saved in RTC

	MMRC_CRYPTO_DCI_InitErr										   = -22200,
	MMRC_CRYPTO_DCI_RSAGetPublicKeyErr						   = -22201,
	MMRC_CRYPTO_DCI_RSAExtPublicKeyEncErr					   = -22202,
	MMRC_CRYPTO_DCI_RSAIntPublicKeyEncErr					   = -22203,
	MMRC_CRYPTO_DCI_RSAIntPrivateKeyDecErr					   = -22204,
	MMRC_CRYPTO_DCI_RSAIntOpCmpErr					         = -22205,
	MMRC_CRYPTO_DCI_RSAExtOpCmpErr					         = -22206,
	MMRC_CRYPTO_DCI_RandSeedErr									= -22207,
	MMRC_CRYPTO_DCI_RandGenErr										= -22208,
	MMRC_CRYPTO_MM_RSAIntPublicKeyEncErr				      = -22209,
	MMRC_CRYPTO_MM_RSAIntPrivateKeyDecErr				      = -22210,
	MMRC_CRYPTO_MM_RSAIntOpCmpErr		   				      = -22211, //!< cmp of reference and op. output fails
	MMRC_CRYPTO_MM_RSAPlainSizeMismatch   				      = -22212, //!< sizes of reference and op. output differ
	MMRC_CRYPTO_MM_RSAGetPublicKeyErr  				         = -22213,
	MMRC_CRYPTO_DCI_3DESEncErr                            = -22214,
	MMRC_CRYPTO_DCI_3DESDecErr                            = -22215,
	MMRC_CRYPTO_DCI_3DESIntOpCmpErr                       = -22216,
	MMRC_CRYPTO_DCI_AES128CBCEncErr                       = -22217,
	MMRC_CRYPTO_DCI_AES128CBCDecErr                       = -22218,
	MMRC_CRYPTO_DCI_AES128CBCIntOpCmpErr                  = -22219,
	MMRC_CRYPTO_DCI_AES128ECBEncErr                       = -22220,
	MMRC_CRYPTO_DCI_AES128ECBDecErr                       = -22221,
	MMRC_CRYPTO_DCI_AES128ECBIntOpCmpErr                  = -22222,
	MMRC_CRYPTO_SHA1InitErr											= -22223,
	MMRC_CRYPTO_SHA1ExecErr											= -22224,
	MMRC_CRYPTO_SHA1FinalErr										= -22225,
	MMRC_CRYPTO_SHA1CmpErr											= -22226,
	MMRC_CRYPTO_SHA256InitErr										= -22227,
	MMRC_CRYPTO_SHA256ExecErr										= -22228,
	MMRC_CRYPTO_SHA256FinalErr										= -22229,
	MMRC_CRYPTO_SHA256CmpErr										= -22230,
	MMRC_CRYPTO_MD5InitErr											= -22231,
	MMRC_CRYPTO_MD5ExecErr											= -22232,
	MMRC_CRYPTO_MD5FinalErr											= -22233,
	MMRC_CRYPTO_MD5CmpErr											= -22234,
	MMRC_CRYPTO_HMAC_SHA1InitErr									= -22235,
	MMRC_CRYPTO_HMAC_SHA1ExecErr									= -22236,
	MMRC_CRYPTO_HMAC_SHA1FinalErr								   = -22237,
	MMRC_CRYPTO_HMAC_SHA1CmpErr									= -22238,
	MMRC_CRYPTO_DCI_RSAwithSha256signErr						= -22239,
	MMRC_CRYPTO_DCI_RSAwithSha256verifyErr						= -22240,
	MMRC_CRYPTO_MM_AES_ECB_CineLinkCmpErr                 = -22241,
	MMRC_CRYPTO_MM_RSAwithSha256verifiyErr                = -22242,
	MMRC_CRYPTO_SignatureCheckFails                       = -22243, //!< failure of a universal signature verify operation
	MMRC_CRYPTO_PRNGTestErr											= -22244, //!< failure of Physical RNG known answer test
	MMRC_CRYPTO_MM_AES128CBCFpgaCmpErr                    = -22245,
	MMRC_CRYPTO_HMAC_InitErr										= -22246,
	MMRC_CRYPTO_HMAC_ExecErr										= -22247,
	MMRC_CRYPTO_HMAC_FinalErr									   = -22248,
	MMRC_CRYPTO_AES_CBC_InitErr                           = -22249,
	MMRC_CRYPTO_AES_CBC_EncErr                            = -22250,
	MMRC_CRYPTO_AES_CBC_DecErr                            = -22251,
	MMRC_CRYPTO_AES_ECB_InitErr                           = -22252,
	MMRC_CRYPTO_AES_ECB_EncErr										= -22253,
	MMRC_CRYPTO_AES_ECB_DecErr										= -22254,
	MMRC_CRYPTO_MM_RSAPublicKeyDecErr  				         = -22255,
	MMRC_CRYPTO_SignatureNegativeTestErr 				      = -22256,
	MMRC_CRYPTO_ContinuousPhysicalRNGTestErr					= -22257,
	MMRC_CRYPTO_ContinuousDeterministicRNGTestErrc        = -22258,

	

	MMRC_PRODUCTION_SmartcardInstallationErr              = -22300,
	MMRC_PRODUCTION_FmIdWriteErr									= -22301, //!< failure on saving Forensic Marking Id
	MMRC_PRODUCTION_InstallMikromPublicKeys					= -22302, //!< these keys are required to verify MikroM firmware
	MMRC_PRODUCTION_InitDataFormatVersionErr              = -22303,
	MMRC_PRODUCTION_SetDataFormatVersionErr               = -22304,
	MMRC_PRODUCTION_GetDataFormatVersionErr               = -22305,
	MMRC_PRODUCTION_InitSystemInfoErr                     = -22306,
	MMRC_PRODUCTION_SetSystemInfoErr                      = -22307,
	MMRC_PRODUCTION_GetSystemInfoErr                      = -22308,
	MMRC_PRODUCTION_InitESAErr										= -22309,
	MMRC_PRODUCTION_SetESAErr										= -22310,
	MMRC_PRODUCTION_GetESAErr										= -22311,
	MMRC_PRODUCTION_InitHardwareInfoErr							= -22312,
	MMRC_PRODUCTION_SetHardwareInfoErr							= -22313,
	MMRC_PRODUCTION_GetHardwareInfoErr							= -22314,
	MMRC_PRODUCTION_TestHardwareInfoErr                   = -22315,
	MMRC_PRODUCTION_TestPictureCrcErr							= -22316,

	MMRC_FPGA_MNG_ParameterError									= -22370, //!< programming parameter does not work with current hardware
	MMRC_FPGA_MNG_Busy												= -22371, //!< fpga programming in progress without error but further data is required
	MMRC_FPGA_MNG_AllDoneSuccess									= -22372, //!< all data is programmed and check was successful
	MMRC_FPGA_MNG_AllDoneError										= -22373, //!< all data is programmed and check  was NOT successful	(e.g. size check gives too much data)	
	MMRC_FPGA_MNG_ProgramModeFailed								= -22374, //!< could not set fpga to program mode
	MMRC_FPGA_MNG_ProgramModeDoneLow								= -22375, //!< status line 'done' did not signalize correct programmed fpga
	MMRC_FPGA_MNG_AllDoneSignError								= -22376, //!< status line 'done' shows a well programmed fpga before all data was programmed and after final portion the signature check fails

	MMRC_SECIF_BusInitErr											= -22401,
	MMRC_SECIF_FpgaErr												= -22402,
	MMRC_SECIF_BusErr													= -22403,
	MMRC_SECIF_WriteErr												= -22404,

	MMRC_LOG_FLASH_HAL_BusInitErr									= -22501,
	MMRC_LOG_FLASH_HAL_FPGA1Err									= -22502,
	MMRC_LOG_FLASH_HAL_FPGA2Err									= -22503,
	MMRC_LOG_FLASH_HAL_FPGA3Err									= -22504,
	MMRC_LOG_FLASH_HAL_IdErr										= -22505,
	MMRC_LOG_FLASH_HAL_EraseErr									= -22506,
	MMRC_LOG_FLASH_HAL_TimeoutErr									= -22507,
	MMRC_LOG_FLASH_HAL_Reset1TimeoutErr							= -22508,
	MMRC_LOG_FLASH_HAL_Reset2TimeoutErr							= -22509,
	MMRC_LOG_FLASH_HAL_EraseTimeoutErr							= -22510,
	MMRC_LOG_FLASH_HAL_ReadTimeoutErr							= -22511,
	MMRC_LOG_FLASH_HAL_WriteTimeoutErr							= -22512,
	MMRC_LOG_FLASH_HAL_DeviceNotInitialized               = -22513,


	MMRC_LOG_FLASH_DEV_UnknownErr                         = -22530,
	MMRC_LOG_FLASH_DEV_BusInitErr									= -22531,
	MMRC_LOG_FLASH_DEV_FPGAErr										= -22532,
	MMRC_LOG_FLASH_DEV_FlashIdErr									= -22533,
	MMRC_LOG_FLASH_DEV_FlashErr									= -22534,
	MMRC_LOG_FLASH_DEV_FatalErr									= -22535,
	MMRC_LOG_FLASH_DEV_DataNotFound								= -22536,
	MMRC_LOG_FLASH_DEV_WrongFlashArea							= -22537,
	MMRC_LOG_FLASH_DEV_InvalidOffset 							= -22538,

	MMRC_SEC_IF_BusInitErr											= -22601,
	MMRC_SEC_IF_FpgaErr											   = -22602,
	MMRC_SEC_IF_BusErr											   = -22603,
	MMRC_SEC_IF_WriteErr											   = -22604,
	MMRC_SEC_IF_FPGASelftestNotReady                      = -22605,
	MMRC_SEC_IF_FPGASelftestAES128CBCErr                  = -22606,
	MMRC_SEC_IF_FPGASelftestHMACSHA1Err                   = -22607,
	MMRC_SEC_IF_FPGASelftestSHA1Err			               = -22608,
	MMRC_SEC_IF_SelftestDRNGErr									= -22609,


	MMRC_SelftestError                                    = -22630,
	MMRC_SelftestPending                                  = -22631,

	MMRC_Zlib_NeedDict												= -22650,		//!< #define Z_NEED_DICT     2
	MMRC_Zlib_StreamEnd												= -22651,		//!< #define Z_STREAM_END    1
	MMRC_Zlib_Reserved												= -22652,      //!< no return code, only used for error translation
	MMRC_Zlib_Errno													= -22653,		//!< #define Z_ERRNO        (-1)
	MMRC_Zlib_StreamErr												= -22654,		//!< #define Z_STREAM_ERROR (-2)
	MMRC_Zlib_DataErr													= -22655,		//!< #define Z_DATA_ERROR   (-3)
	MMRC_Zlib_MemErr													= -22656,		//!< #define Z_MEM_ERROR    (-4)
	MMRC_Zlib_BufErr													= -22657,		//!< #define Z_BUF_ERROR    (-5)
	MMRC_Zlib_VersionErr												= -22658,		//!< #define Z_VERSION_ERROR (-6)
	

	MMRC_FUSE_WriteFailed											= -22700,
	MMRC_FUSE_WriteProtect											= -22701,
	MMRC_FUSE_OverriteProtect										= -22702,
	MMRC_FUSE_WriteLocked											= -22703,
	MMRC_FUSE_ReadFailed												= -22704,
	MMRC_FUSE_VerifyFailed											= -22705,

	MMRC_PROJECTOR_NetworkConfigFailed							= -22800,
	MMRC_PROJECTOR_ConnectionFailed								= -22801,
	MMRC_PROJECTOR_SendFailed										= -22802,
	MMRC_PROJECTOR_RecvFailed										= -22803,
	MMRC_PROJECTOR_NotConnected									= -22804,
	MMRC_PROJECTOR_CommandChecksumError							= -22805,
	MMRC_PROJECTOR_NACK_SystemBusy								= -22806,
	MMRC_PROJECTOR_NACK_UnknownCommand							= -22807,
	MMRC_PROJECTOR_NACK_FunctionNotAvailable					= -22808,
	MMRC_PROJECTOR_NACK_ChecksumError							= -22809,
	MMRC_PROJECTOR_NACK_SoftOverrun								= -22810,
	MMRC_PROJECTOR_NACK_InvalidAccessPrivileges				= -22811,
	MMRC_PROJECTOR_NACK_MessageTimeout							= -22812,
	MMRC_PROJECTOR_NACK_ParityError								= -22813,
	MMRC_PROJECTOR_NACK_OverrunError								= -22814,
	MMRC_PROJECTOR_NACK_FramingError								= -22815,
	MMRC_PROJECTOR_NACK_IncorrectLength							= -22816,
	MMRC_PROJECTOR_NACK_LoadInProgress							= -22817,
	MMRC_PROJECTOR_NACK_InstallInProgress						= -22818,
	MMRC_PROJECTOR_NACK_MessageCorrupt							= -22819,
	MMRC_PROJECTOR_RecvCommandNoResponds						= -22820,		//!< the received command should be a responds
	MMRC_PROJECTOR_InvalidParameter								= -22821,
	MMRC_PROJECTOR_OperationFailed								= -22822,
	MMRC_PROJECTOR_MarriageFailed_ComError						= -22823,
	MMRC_PROJECTOR_MarriageFailed_CertStoreError				= -22824,
	MMRC_PROJECTOR_NotMarried_NoCert								= -22825,
	MMRC_PROJECTOR_NotMarried_CertReadError					= -22826,
	MMRC_PROJECTOR_NotMarried_CertBroken						= -22827,
	MMRC_PROJECTOR_NotMarried_VerifyFailed						= -22828,
	MMRC_PROJECTOR_NotMarried_ComError							= -22829,
	MMRC_PROJECTOR_WaitingForNetwork								= -22830,

   // error codes for interaction with smartcards Global Platform Manager follows
	MMRC_GPM_SelectISDExecFails									= -22901, //!< error selecting the GPM Issuer Security Domain 
	MMRC_GPM_InitializeUpdateExecFails                    = -22902,
	MMRC_GPM_InitializeUpdateCheckFails							= -22903, 
	MMRC_GPM_CardCryptogramMismatch								= -22904,
	MMRC_GPM_ExternalAuthExecFails								= -22905,
	MMRC_GPM_DeleteExecFails										= -22906,

	// error code that are specific for main/mediablock processor bootloader selftests
	MMRC_MP_BOOT_SelftestCRCError                         = -23001, //!< crc self-test of main/mediablock processor fails
	MMRC_MP_BOOT_SelftestAES128CBCEncError                = -23002, //!< AES-128 CBC encryption CAT (compare answer test, known answer) fails at self-test of main/mediablock processor bootloader
	MMRC_MP_BOOT_SelftestAES128CBCDecError                = -23003, //!< AES-128 CBC decryption CAT fails at self-test of main/mediablock processor bootloader
	MMRC_MP_BOOT_SelftestAES256CBCEncError                = -23004, //!< AES-256 CBC encryption CAT fails at self-test of main/mediablock processor bootloader
	MMRC_MP_BOOT_SelftestAES256CBCDecError                = -23005, //!< AES-256 CBC decryption CAT fails at self-test of main/mediablock processor bootloader
	MMRC_MP_BOOT_SelftestSHA256Error                      = -23006, //!< SHA-256 CAT fails at self-test of main/mediablock processor
	MMRC_MP_BOOT_SelftestMD5Error									= -23007, //!< MD5 CAT fails at self-test

	// error code that are specific for security processor bootloader selftests
	MMRC_SecP_BOOT_SelftestCRCError                        = -23051, //!< crc self-test of security processor fails
	MMRC_SecP_BOOT_SelftestAES128CBCEncError               = -23052, //!< AES-128 CBC encryption CAT fails at self-test of security processor bootloader
	MMRC_SecP_BOOT_SelftestAES128CBCDecError               = -23053, //!< AES-128 CBC decryption CAT fails at self-test of security processor bootloader
	MMRC_SecP_BOOT_SelftestAES256CBCEncError               = -23054, //!< AES-256 CBC encryption CAT fails at self-test of security processor bootloader
	MMRC_SecP_BOOT_SelftestAES256CBCDecError               = -23055, //!< AES-256 CBC decryption CAT fails at self-test of security processor bootloader
	
	MMRC_SecP_BOOT_Selftest3TDesCBCEncError                = -23060, //!< 3TDES CBC encryption CAT fails at self-test of security processor bootloader
	MMRC_SecP_BOOT_Selftest3TDesCBCDecError                = -23061, //!< 3TDES CBC decryption CAT fails at self-test of security processor bootloader	
	MMRC_SecP_BOOT_SelftestSHA256Error                     = -23062, //!< SHA-256 CAT fails at self-test of security processor
	MMRC_SecP_BOOT_SelftestSHA512Error                     = -23063, //!< SHA-512 CAT fails at self-test of security processor

	MMRC_SecP_BOOT_SelftestRSASignVerifyError              = -23070, //!< RSA Signature Verification CAT fails at self-test of security processor

	// error code for firmware integrity test (FWIT)
	// the testing entity depends on system and is not mentioned here
   MMRC_FWIT_Fpga1Boot_TestFails                          = -23080, //!< BootFPGA 1 verify fails
	MMRC_FWIT_Fpga1_TestFails                              = -23081, //!< FPGA 1/Main FPGA verify fails
	MMRC_FWIT_Fpga2_TestFails                              = -23082, //!< FPGA 2 FPGA verify fails
	MMRC_FWIT_Fpga3_TestFails                              = -23083, //!< FPGA 3 FPGA verify fails
	MMRC_FWIT_Fpga4_TestFails                              = -23084, //!< FPGA 4 FPGA verify fails
	
	MMRC_FWIT_SecP_Boot_TestFails                          = -23090, //!< security processor bootloader verify fails
	MMRC_FWIT_SecP_TestFails                               = -23091, //!< security processor firmware verify fails
	MMRC_FWIT_SysP_Boot_TestFails                          = -23092, //!< system processor bootloader verify fails
	MMRC_FWIT_SysP_TestFails                               = -23093, //!< system processor firmware verify fails
	MMRC_FWIT_MP_Boot_TestFails                            = -23094, //!< main/mediablock processor bootloader verify fails
	MMRC_FWIT_MP_TestFails                                 = -23095, //!< main/mediablock processor firmware verify fails
	MMRC_FWIT_SecP_SelfCrcErr										 = -23096, //!< security processor firmware crc selfcheck fails


	MMRC_FWIT_DSP1_TestFails                               = -23101, //!< DSP 1 verify fails
	
	MMRC_FWLT_SecP_UpdateStartAuthFails							 = -23170, //!< partial security update authentication fails
	MMRC_FWLT_SecP_UpdateAuthFails								 = -23171, //!< security update authentication fails
	MMRC_FWLT_MP_FirmwareAuthFails                         = -23172, //!< firmware authentication fails

	// Security Manager error codes
	MMRC_SM_KeyStoreFull												= -24000,
	MMRC_SM_KeyDecryptionFailed									= -24001,
	MMRC_SM_KeyDecryptionFailed_WrongSize						= -24002,
	MMRC_SM_KeyDecryptionFailed_WrongStructureId				= -24003,
	MMRC_SM_KeyDecryption_WrongCPL								= -24004,
	MMRC_SM_KeyDecryption_ThumbPrintInconsistent				= -24005,
	MMRC_SM_SSL_ConnectFailed										= -24006,
	MMRC_SM_SSL_LoadCertChainFailed								= -24007,
	MMRC_SM_SSL_SetCipherFailed									= -24008,
	MMRC_SM_SSL_ConnectInitFailed									= -24009,
	MMRC_SM_SSL_LoadPrivateKeyFailed								= -24010,
	MMRC_SM_XML_CreateSignatureContextFailed					= -24011,
	MMRC_SM_XML_SignatureVerifyFailed							= -24012,
	MMRC_SM_XML_SignatureInvalid									= -24013,
	MMRC_SM_SuiteLost													= -24014,
	MMRC_SM_SpbListTooLong											= -24015,
	MMRC_SM_SuiteAlreadyStarted									= -24016, //!< suite is already started, it must be stopped before it can be started again
	MMRC_SM_SuiteDCIError											= -24017, //!< suite configuration breaks DCI �9.4.3.6.5 or �9.4.4 requirements
	MMRC_SM_ElementExists											= -24018, //!< CPL/KDM already exists
	MMRC_SM_ElementNotSMPTECompliant								= -24019, //!< CPL not SMPTE 429-7 complaint or KDM not SMPTE 430-2 compliant (wrong Schema)
	MMRC_SM_XML_SignChainError										= -24020, //!< signer chain not SMPTE 430-2 compliant
	MMRC_SM_CPLMissingAsset											= -24021, //!< CPL has at least one missing asset
	MMRC_SM_OutOfElementSpace										= -24022, //!< no more space for CPL/KDM
	MMRC_SM_TooManyEssenceKeys										= -24023, //!< CPL has more than 256 essence keys
	MMRC_SM_NoCPLforKDM												= -24024, //!< corresponding CPL for a KDM is missing
	MMRC_SM_KDMKeylistMismatch										= -24025, //!< KDM key list does not match CPL key list
	MMRC_SM_OutputTableOverflow									= -24026, //!< output table is to small
	MMRC_SM_ElementNotFound											= -24027, //!< CPL or KDM not found
	MMRC_SM_InvalidCheckTime										= -24028, //!< the check time is beyond SMs clock resolution
	MMRC_SM_InvalidCPLList											= -24029, //!< CPL list is invalid (to small or big)
	MMRC_SM_MissingKDM												= -24030, //!< at least one KDM for the requested CPL is missing
	MMRC_SM_KDMInFuture												= -24031, //!< at least one KDM is in the future
	MMRC_SM_KDMExpired												= -24032, //!< at least one KDM is expired
	MMRC_SM_ShowAlreadyPrepared									= -24033,
	MMRC_SM_SuiteError												= -24034, //!< the auditorium suite is either not started, not healthy or at least one suite device is not authorized (i.e. on a KDM device list)
	MMRC_SM_SuiteKeyingFailed										= -24035, //!< the keying of at least one suite device failed
	MMRC_SM_OutOfTimeRange											= -24036,
	MMRC_SM_LogRangeInvalid											= -24037,
	MMRC_SM_LogRangeEmpty											= -24038,
	MMRC_SM_LogRecordLimitExceeded								= -24039,
	MMRC_SM_UnknownCertificateType								= -24040,
	MMRC_SM_CertificateNotFound									= -24041,
	MMRC_SM_OutputBufferTruncated									= -24042,
	MMRC_SM_MasterKeyWrong											= -24043,
	MMRC_SM_PrivateKeyNotFound										= -24044,
	MMRC_SM_CertificateAlreadyInstalled							= -24045,
	MMRC_SM_CertificateInstallFailed								= -24046,
	MMRC_SM_CPUIdMismatch											= -24047,
	MMRC_SM_UUIDMismatch												= -24048,
	MMRC_SM_ProductCodeMismatch									= -24049,
	MMRC_SM_SecurityCheckPending									= -24050,
	MMRC_SM_CertificateMalformed									= -24051,
	MMRC_SM_XML_SigningFailed										= -24052,
	MMRC_SM_CertificateChainIncomplete							= -24053,
	MMRC_SM_CertificateChainVerifyFailed						= -24054,
	MMRC_SM_SignatureDigestWrong									= -24055,
	MMRC_SM_SSL_CreateFailed										= -24056,
	MMRC_SM_PublicPrivateKeyMismatch								= -24057,
	MMRC_SM_SuiteNotStarted											= -24058,
	MMRC_SM_KDMContentAuthenticatorMismatch					= -24059,
	MMRC_SM_KDMDeviceListMismatch									= -24060,
	MMRC_SM_RequiresForensicMarking								= -24061,	//!< forensic marking is required by KDM but no core is activated (missing feature?)
	MMRC_SM_TLSContextCreationFailed								= -24062,
	MMRC_SM_CertificateChainNotSelected							= -24063,
	MMRC_SM_ShowNotPlaying											= -24064,
	MMRC_SM_ShowAlreadyPlaying										= -24065,
	MMRC_SM_CplAlreadyPlaying										= -24066,
	MMRC_SM_UnknownKeyType											= -24067,
	MMRC_SM_XML_ReadingSchemaFailed								= -24068,
	MMRC_SM_XML_ParsingSchemaFailed								= -24069,
	MMRC_SM_XML_SchemaCtxFailed									= -24070,
	MMRC_SM_XML_SchemaValidationFailed							= -24071,
	MMRC_SM_XML_SchemaNotFound										= -24072,
	MMRC_SM_CertificateNotValidBefore							= -24073,
	MMRC_SM_CertificateNotValidAfter								= -24074,
	MMRC_SM_CertificateChainVerifyFailed_Sign					= -24075,	//!< denotes the signing chain from root to leaf is incorrect
	MMRC_SM_CertificateChainVerifyFailed_NotBefore			= -24076,	//!< certificate is valid in the future
	MMRC_SM_CertificateChainVerifyFailed_NotAfter			= -24077,	//!< certificate is valid in the past
	MMRC_SM_KDMSignerThumbMismatch								= -24078,
	MMRC_SM_KDMKeyTimeInvalid										= -24079,
	MMRC_SM_KDMIssuerTimeInvalid									= -24080,
	MMRC_SM_KDMBadMessageType										= -24081,
	MMRC_SM_KDMKeyInfoMismatch										= -24082,
	MMRC_SM_KDMKeyTypeMismatch										= -24083,	//!< key type mismatch between encrypted and unecrypted KDM section
	MMRC_SM_XML_WrongSignatureMethod								= -24084,
	MMRC_SM_KDMWrongMediaType										= -24085,	//!< key type and media type mismatch
	MMRC_SM_CPLUserAssetException									= -24086,
	MMRC_SM_RequiresHFRForensicMarking							= -24087,
	MMRC_SM_KDMContentAuthenticatorSignatureWrong			= -24088,	//!< Signature of the reference

	MMRC_TAMPER_EmptyEvent											= -24100,
	MMRC_TAMPER_WrongVersion										= -24101,
	MMRC_TAMPER_CommunicationError								= -24102,
	MMRC_TAMPER_MasterKeyCorrupt									= -24103,
	MMRC_TAMPER_ServiceDoorOpen									= -24104,
	MMRC_TAMPER_MissingServiceDoorTamperTermination			= -24105,
	MMRC_TAMPER_Divorced												= -24106,

	MMRC_DATABASE_InitErr										   = -24200,
	MMRC_DATABASE_NotConnected										= -24201,
	MMRC_DATABASE_BadQueryResponse                        = -24202,
	MMRC_DATABASE_QueryFailed                             = -24203,

	// error code for hermes subsystem 
	MMRC_HERMES_SUB_DeviceNotInitialized                  = -24400, // the function that initializes the AV device (usually MVC20x) was not called
	MMRC_HERMES_SUB_DatabaseNotInitialized                = -24401, // the function that initializes database access was not called
	MMRC_HERMES_SUB_3rdPartyLibNotInitialized             = -24402, // the function that initializes third party libraries was not called
	MMRC_HERMES_SUB_ControlUnitNotInitialized             = -24403, // the function that initializes hermes control unit was not called

	MMRC_JOB_DECODER_UnexpectedException                  = -24500,
	MMRC_JOB_DECODER_XMLException		                     = -24501,
	MMRC_JOB_DECODER_DOMException									= -24502,

	// error code for testing, should not be used in regular software but only in test software
	// compare answer test (CAT) codes
	MMRC_CAT_Test0Fails												= -30000,
	MMRC_CAT_Test1Fails												= -30010,
	MMRC_CAT_Test2Fails												= -30020,
	MMRC_CAT_Test3Fails												= -30030,
	MMRC_CAT_Test4Fails												= -30040,
	MMRC_CAT_Test5Fails												= -30050,
	MMRC_CAT_Test6Fails												= -30060,
	MMRC_CAT_Test7Fails												= -30070,
	MMRC_CAT_Test8Fails												= -30080,
	MMRC_CAT_Test9Fails												= -30090,


	MMRC_LastErrorCode
};

#endif
