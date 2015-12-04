
//#NT#2010/07/29#Lily Kao -begin

/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       AppInitUSB.c
    @ingroup    mIPRJAPCfg

    @brief      Appication initialization for USB mode
                Get memory and set parameters for USB mode

    @note       Nothing.

    @date       2010/07/29
*/

/** \addtogroup mIPRJAPCfg */
//@{

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
//#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "GxDisplay.h"
#include "UIAppUsbDisk.h"
#include "AppLib.h"
#include "UIInfo.h"

#include "PlaybackTsk.h"
#include "DxType.h"
#include "DxStorage.h"
#include "MsdcNvtCb.h"

//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UIAppUsbDiskExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////

//#NT#2009/07/01#Brad Chen -begin
//#NT#Add to support MSDC CDROM function
static DX_HANDLE pLastStrgDev = 0;
//#NT#2009/07/01#Brad Chen -end

//#NT#2010/10/21#Lily Kao -begin
#if (UI_STYLE!=UI_STYLE_DRIVER)
extern UINT16 gUSIDCSerialStrDesc3[];
#endif
extern void USBMakerInit_SerialNumber(void);

extern BOOL DrvCARD_DetStrgCard(void);
extern BOOL DrvCARD_DetStrgCardWP(void);

_ALIGNED(4) const static UINT8 gMSDCManuStrDesc[] =
{
    USB_VENDER_DESC_STRING_LEN*2+2, // size of String Descriptor = 6 bytes
    0x03,                       // 03: String Descriptor type
    USB_VENDER_DESC_STRING
};

_ALIGNED(4) const static UINT8 gMSDCProdStrDesc[] =
{
    USB_PRODUCT_DESC_STRING_LEN*2+2, // size of String Descriptor = 6 bytes
    0x03,                       // 03: String Descriptor type
    USB_PRODUCT_DESC_STRING
};
_ALIGNED(16) UINT8 gMSDCInquiryData[36]=
{
    0x00, 0x80, 0x05, 0x02, 0x20, 0x00, 0x00, 0x00,
    //Vendor identification,
    USB_VENDER_STRING,/*'\0',*/
    USB_PRODUCT_STRING,'\0',
    USB_PRODUCT_REVISION
};
//#NT#2010/10/21#Lily Kao -end

void USBMakerInit_UMSD(USB_MSDC_INFO *pUSBMSDCInfo)
{
    pUSBMSDCInfo->pManuStringDesc = (USB_STRING_DESC *)gMSDCManuStrDesc;
    pUSBMSDCInfo->pProdStringDesc = (USB_STRING_DESC *)gMSDCProdStrDesc;

    //USBMakerInit_SerialNumber();
    #if (UI_STYLE!=UI_STYLE_DRIVER)
    pUSBMSDCInfo->pSerialStringDesc = (USB_STRING_DESC *)gUSIDCSerialStrDesc3;
    #else
    pUSBMSDCInfo->pSerialStringDesc = NULL;
    #endif

    pUSBMSDCInfo->VID = USB_VID;
    pUSBMSDCInfo->PID = USB_PID_MSDC;

    pUSBMSDCInfo->pInquiryData = (UINT8*)&gMSDCInquiryData[0];
}

/**
  Initialize application for USB MSDC mode

  Initialize application for USB MSDC mode.

  @param void
  @return void
*/
//#NT#2010/12/13#Lily Kao -begin
ER AppInit_ModeUSBMSDC(void)
//#NT#2010/12/13#Lily Kao -end
{
#if 0 // original
    char* pDxName=NULL;
    USB_MSDC_INFO       MSDCInfo={0};
    UINT32              uiPoolAddr;
    DX_HANDLE pStrgDev = 0;
    ER retV = E_OK;
    // Get buffer memory for MSDC task, MSDC share the same buffer memory with SIDC,
    // make sure the buffer memory size is enough for both
    uiPoolAddr = OS_GetMempoolAddr(POOL_ID_APP);

    MSDCInfo.uiMsdcBufAddr = uiPoolAddr;
    MSDCInfo.uiMsdcBufSize = MSDC_MIN_BUFFER_SIZE;
    USBMakerInit_UMSD(&MSDCInfo);
    //The callback functions for the MSDC Vendor command.
    //If project doesn't need the MSDC Vendor command, set this callback function as NULL.
    MSDCInfo.msdc_check_cb = NULL;
    MSDCInfo.msdc_vendor_cb = NULL;

#if (POWERON_TESTFLOW == ENABLE)
    if(gIsUSBChargePreCheck)
    {
    if(DrvCARD_DetStrgCard())
    pStrgDev = Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_CARD1);
    else
    pStrgDev = Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_NAND1);
    }
    else
#endif
    {
    pStrgDev = (DX_HANDLE)GxStrg_GetDevice(0);
    }

    MSDCInfo.pStrgHandle[0] = pStrgDev;

    if(Dx_GetInfo(pStrgDev, DX_INFO_NAME,&pDxName)!=DX_OK)
    {
        pDxName = NULL;
    }

    if(pDxName==NULL || strcmp(pDxName, "Storage_Nand1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = NULL;
        MSDCInfo.msdc_strgLock_detCB[0] = NULL;
    }
    else if(strcmp(pDxName,"Storage_Card1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = (MSDC_StorageDet_CB)DrvCARD_DetStrgCard;
        MSDCInfo.msdc_strgLock_detCB[0] = (MSDC_StrgLockDet_CB)DrvCARD_DetStrgCardWP;
    }
    MSDCInfo.msdc_type[0] = MSDC_STRG;
    MSDCInfo.LUNs = 1;

    //#NT#2009/08/29#Ben Wang -Begin
    // Close File System task before running USB MSDC
    //GxFile_Exit();
    FileSys_Close(FST_TIME_INFINITE);
    //#NT#2009/08/29#Ben Wang -end
    Msdc_SetConfig(USBMSDC_CONFIG_ID_SELECT_POWER, USBMSDC_POW_BUSPOWER);

#if(MSDCVENDOR_NVT==ENABLE)
    MsdcNvtCb_Attach(&MSDCInfo);
#endif

    retV = Msdc_Open(&MSDCInfo);
    if (retV != E_OK)
    {
        debug_err(("Error open USB MSDC task:%d\r\n",retV));
    }
    return retV;

#else // _MSDC_ALL_, _MSDC_SINGLE_NAND2_, _MSDC_ALL_NAND2_

    char*           pDxName = NULL;
    USB_MSDC_INFO   MSDCInfo = {0};
    UINT32          uiPoolAddr;
    DX_HANDLE       pStrgDev1 = 0;
    DX_HANDLE       pStrgDev2 = 0;
    DX_HANDLE       pStrgDev3 = 0;
    ER              retV = E_OK;

    // Get buffer memory for MSDC task, MSDC share the same buffer memory with SIDC,
    // make sure the buffer memory size is enough for both
    uiPoolAddr = OS_GetMempoolAddr(POOL_ID_APP);

    MSDCInfo.uiMsdcBufAddr = uiPoolAddr;
    MSDCInfo.uiMsdcBufSize = MSDC_MIN_BUFFER_SIZE;
    USBMakerInit_UMSD(&MSDCInfo);
    //The callback functions for the MSDC Vendor command.
    //If project doesn't need the MSDC Vendor command, set this callback function as NULL.
    MSDCInfo.msdc_check_cb = NULL;
    MSDCInfo.msdc_vendor_cb = NULL;

#if (_MSDC_MODE_ == _MSDC_SINGLE_)

    pStrgDev1 = (DX_HANDLE)GxStrg_GetDevice(0);

    MSDCInfo.pStrgHandle[0] = pStrgDev1;

    if(Dx_GetInfo(pStrgDev1, DX_INFO_NAME, &pDxName) != DX_OK)
    {
        pDxName = NULL;
    }

    if(pDxName==NULL || strcmp(pDxName, "Storage_Nand1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = NULL;
        MSDCInfo.msdc_strgLock_detCB[0] = NULL;
    }
    else if(strcmp(pDxName, "Storage_Card1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = (MSDC_StorageDet_CB)DrvCARD_DetStrgCard;
        MSDCInfo.msdc_strgLock_detCB[0] = (MSDC_StrgLockDet_CB)DrvCARD_DetStrgCardWP;
    }
    MSDCInfo.msdc_type[0] = MSDC_STRG;
    MSDCInfo.LUNs = 1;

#elif (_MSDC_MODE_ == _MSDC_SINGLE_NAND2_)

    pStrgDev1 = (DX_HANDLE)GxStrg_GetDevice(0);
    pStrgDev2 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_NAND2);

    MSDCInfo.pStrgHandle[0] = pStrgDev1;
    MSDCInfo.pStrgHandle[1] = pStrgDev2;

    if(Dx_GetInfo(pStrgDev1, DX_INFO_NAME, &pDxName) != DX_OK)
    {
        pDxName = NULL;
    }

    if(pDxName==NULL || strcmp(pDxName, "Storage_Nand1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = NULL;
        MSDCInfo.msdc_strgLock_detCB[0] = NULL;
    }
    else if(strcmp(pDxName, "Storage_Card1")==0)
    {
        MSDCInfo.msdc_storage_detCB[0] = (MSDC_StorageDet_CB)DrvCARD_DetStrgCard;
        MSDCInfo.msdc_strgLock_detCB[0] = (MSDC_StrgLockDet_CB)DrvCARD_DetStrgCardWP;
    }

    MSDCInfo.msdc_type[0] = MSDC_STRG;
    MSDCInfo.msdc_type[1] = MSDC_CDROM;
    MSDCInfo.LUNs = 2;

#elif (_MSDC_MODE_ == _MSDC_ALL_NAND2_)

    pStrgDev1 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_CARD1);
    pStrgDev2 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_NAND1);
    pStrgDev3 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_NAND2);

    MSDCInfo.pStrgHandle[0] = pStrgDev1;
    MSDCInfo.pStrgHandle[1] = pStrgDev2;
    MSDCInfo.pStrgHandle[2] = pStrgDev3;

    // Card
    MSDCInfo.msdc_storage_detCB[0] = (MSDC_StorageDet_CB)DrvCARD_DetStrgCard;
    MSDCInfo.msdc_strgLock_detCB[0] = (MSDC_StrgLockDet_CB)DrvCARD_DetStrgCardWP;
    // NAND1
    MSDCInfo.msdc_storage_detCB[1] = NULL;
    MSDCInfo.msdc_strgLock_detCB[1] = NULL;
    // NAND2
    MSDCInfo.msdc_storage_detCB[2] = NULL;
    MSDCInfo.msdc_strgLock_detCB[2] = NULL;

    MSDCInfo.msdc_type[0] = MSDC_STRG;
    MSDCInfo.msdc_type[1] = MSDC_STRG;
    MSDCInfo.msdc_type[2] = MSDC_CDROM;
    MSDCInfo.LUNs = 3;

#else // _MSDC_ALL_

    pStrgDev1 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_CARD1);
    pStrgDev2 = (DX_HANDLE)Dx_GetObject(DX_CLASS_STORAGE_EXT|DX_TYPE_NAND1);

    MSDCInfo.pStrgHandle[0] = pStrgDev1;
    MSDCInfo.pStrgHandle[1] = pStrgDev2;

    // Card
    MSDCInfo.msdc_storage_detCB[0] = (MSDC_StorageDet_CB)DrvCARD_DetStrgCard;
    MSDCInfo.msdc_strgLock_detCB[0] = (MSDC_StrgLockDet_CB)DrvCARD_DetStrgCardWP;
    // NAND1
    MSDCInfo.msdc_storage_detCB[1] = NULL;
    MSDCInfo.msdc_strgLock_detCB[1] = NULL;

    MSDCInfo.msdc_type[0] = MSDC_STRG;
    MSDCInfo.msdc_type[1] = MSDC_STRG;
    MSDCInfo.LUNs = 2;

#endif

    //#NT#2009/08/29#Ben Wang -Begin
    // Close File System task before running USB MSDC
    //GxFile_Exit();
    FileSys_Close(FST_TIME_INFINITE);
    //#NT#2009/08/29#Ben Wang -end
    Msdc_SetConfig(USBMSDC_CONFIG_ID_SELECT_POWER, USBMSDC_POW_BUSPOWER);

#if(MSDCVENDOR_NVT==ENABLE)
    MsdcNvtCb_Attach(&MSDCInfo);
#endif

    retV = Msdc_Open(&MSDCInfo);
    if (retV != E_OK)
    {
        debug_err(("Error open USB MSDC task:%d\r\n",retV));
    }
    return retV;

#endif
}

INT32 MSDCExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    #if (UVC_RECORD_FUNC == ENABLE)
    UvcRec_Close();
    #endif

    Ux_DefaultEvent(pCtrl,NVTEVT_EXE_OPEN,paramNum,paramArray);
//#NT#2009/07/01#Brad Chen -begin
//#NT#Add to support MSDC CDROM function
    pLastStrgDev =GxStrg_GetDevice(0);
//#NT#2009/07/01#Brad Chen -end
    //#NT#2010/12/28#Jeah Yen -begin
    if(pLastStrgDev == 0)
    {
        //PowerOn_CB() POWERON_CB_SXOPEN 沒有提早啟動File System
        debug_err(("MSDC: Wait for FS start...\r\n"));
        FileSys_WaitFinish();

        //update
        pLastStrgDev = GxStrg_GetDevice(0);
    }
    //#NT#2010/12/28#Jeah Yen -end

    UI_SetData(FL_FSStatus,FS_NOT_INIT);

    return (AppInit_ModeUSBMSDC());
}

INT32 MSDCExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Msdc_Close();
    GxStrg_OpenDevice(0, pLastStrgDev);
    Delay_DelayMs(500); // delay some time to wait for Gx storage open ready?
                        // (to fix MSDC open then open again failed issue)

    Ux_DefaultEvent(pCtrl,NVTEVT_EXE_CLOSE,paramNum,paramArray);
    return NVTEVT_CONSUME;
}

EVENT_ENTRY CustomMSDCObjCmdMap[] =
{
    {NVTEVT_EXE_OPEN,               MSDCExe_OnOpen},
    {NVTEVT_EXE_CLOSE,              MSDCExe_OnClose},
    {NVTEVT_NULL,                   0},  //End of Command Map
};

CREATE_APP(CustomMSDCObj,APP_MSDC)

//#NT#2010/07/29#Lily Kao -end

