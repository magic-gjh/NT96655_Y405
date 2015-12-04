/*
    System Usb Callback

    System Callback for USb Module.

    @file       SysUsb_Exe.c
    @ingroup    mIPRJSYS

    @note

    Copyright   Novatek Microelectronics Corp. 2010.  All rights reserved.
*/

////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "UIFrameworkExt.h"
#include "UICommon.h"
#include "AppLib.h"
#include "MsdcNvtCb.h"
#include "UIAppUsbDisk.h"
#include "UsbDevDef.h"
//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysUsbExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
#include "UIFlowMovieFuncs.h"
#include "DxPower.h"

///////////////////////////////////////////////////////////////////////////////

//Using USB Plug-In + ZOOMOUT Key to enter to Msdc Vendor Spy Mode
#if(RTSP_LIVEVIEW_FUNC==ENABLE)
//RTSP is always using msdc vendor for niven to debug.
#define IS_USB_VENDOR() (1)
#else
#define IS_USB_VENDOR() (GxKey_GetData(GXKEY_NORMAL_KEY)==FLGKEY_ZOOMOUT) //!< set to (1) to Force always
#endif

int SX_TIMER_DET_USB_ID = -1;
//#NT#2014/10/29#KS Hung -begin
static UINT32 g_uiUsbScanExistedCnt = 0;
//#NT#2014/10/29#KS Hung -end
static BOOL m_bVendorConnected = FALSE;
UINT32 gUsbSrc = USB_SRC_NONE;

void USB_CB(UINT32 event, UINT32 param1, UINT32 param2);
void UI_DetUSB(void);

#if (USBINSERT_FUNCTION == ENABLE)
SX_TIMER_ITEM(USB_Det, UI_DetUSB, 5, FALSE)
#endif

void System_OnUsbInit(void)
{
    //PHASE-1 : Init & Open Drv or DrvExt
    {
    GxUSB_RegCB(USB_CB);         //Register CB function of GxUSB
    #if (USBINSERT_FUNCTION == ENABLE)
    SX_TIMER_DET_USB_ID = SxTimer_AddItem(&Timer_USB_Det);
    #endif
    }
    //PHASE-2 : Init & Open Lib or LibExt
    {
    #if (USBINSERT_FUNCTION == ENABLE)
    SxTimer_SetFuncActive(SX_TIMER_DET_USB_ID, TRUE);
    #endif
    }
}

void System_OnUsbExit(void)
{
    //PHASE-2 : Close Lib or LibExt
    {
    }
    //PHASE-1 : Close Drv or DrvExt
    {
    }
}

/////////////////////////////////////////////////////////////////////////////
void UI_DetUSB(void)
{
    if(!UI_IsForceLock())
        GxUSB_DetConnect();
}
//---------------------SysUSB Public API  ----------------------------------
void USB_UpdateSource(void)
{
    #if (USB_CHARGE_FUNCTION == ENABLE)
    UINT32 ctype = GxUSB_GetConnectType();
    if(ctype == USB_CONNECT_NONE) //沒插USB
    {
        DBG_MSG("  USB Src = (None)\r\n");
        gUsbSrc = USB_SRC_NONE;
    }
    else
    if(ctype == USB_CONNECT_PC) //USB插著PC
    {
        DBG_MSG("  USB Src = PC\r\n");
        gUsbSrc = USB_SRC_USB_PC;
    }
    else
    if(ctype == USB_CONNECT_CHARGER) //USB插著充電器
    {
        DBG_MSG("  USB Src = Adapter\r\n");
        gUsbSrc = USB_SRC_USB_ADAPTER;
    }
    else
    if(ctype == USB_CONNECT_CHARGING_PC) //USB插著有大電流port的PC
    {
        DBG_MSG("  USB Src = Charging PC\r\n");
        gUsbSrc = USB_SRC_USB_CHARGING_PC;
    }
    else  //USB插著未知 (已經拔除無法判斷)
    {
        DBG_ERR("  USB Src = Unknown?\r\n");
        //#NT#2011/03/11#Jeah Yen -begin
        gUsbSrc = USB_SRC_UNKNOWN;
        //#NT#2011/03/11#Jeah Yen -end
    }
    #else
    BOOL bInsert = GxUSB_GetIsUSBPlug();
    if(!bInsert) //沒插USB
    {
        DBG_MSG("  USB Src = (None)\r\n");
        gUsbSrc = USB_SRC_NONE;
    }
    else //USB插著PC
    {
        DBG_MSG("  USB Src = PC\r\n");
        gUsbSrc = USB_SRC_USB_PC;
    }
    #endif
}
UINT32 USB_GetSource(void)
{
    return gUsbSrc;
}
///////////////////////////////////////////////////////////////////////
//Device flow event
#if (UI_STYLE==UI_STYLE_DRIVER)
BOOL m_bACPlug = FALSE;
#endif
INT32 System_OnUsbInsert(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //usb plug in
    DBG_IND("USB plug - begin\r\n");
    USB_UpdateSource();
    Ux_PostEvent(NVTEVT_BATTERY, 0, 0);
    if(!IS_USB_VENDOR())
    {
        #if (UI_STYLE==UI_STYLE_DRIVER)
        debug_msg("USBConnectType=%d\r\n",GxUSB_GetConnectType());
        //#NT#2014/10/29#KS Hung -begin
        //#NT#Patch that the car USB adapter is recognized to USB mode.
        if ((GxUSB_GetConnectType() == USB_CONNECT_PC) || (GxUSB_GetConnectType() == USB_CONNECT_CHARGING_PC))
        {
            g_uiUsbScanExistedCnt++; //scan DxUSB_UpdateConnectType function every 100ms
            if (g_uiUsbScanExistedCnt >= 20)  //if the D+/D- is stabled more than 2 second, it is sured to enter USB mode.
            {
                g_uiUsbScanExistedCnt = 0;
                Ux_SendEvent(0, NVTEVT_SYSTEM_MODE, 1, PRIMARY_MODE_USBMENU);
                USB_PlugInSetFunc();
            }
            else
            {
                GxUSB_ForceConnect(FALSE); //scan DxUSB_UpdateConnectType function again
            }
        }
        else if (GxUSB_GetConnectType() == USB_CONNECT_CHARGER)
        {
            g_uiUsbScanExistedCnt = 0; // The "if (g_uiUsbScanExistedCnt >= 20)" is recognized once, it is adapter.
            if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
            {
                m_bACPlug = TRUE;
                Ux_PostEvent(NVTEVT_AC_Plug, 0, 0);
            }
	     USB_PlugInSetFunc();
        }
        //#NT#2014/10/29#KS Hung -end
        #else
        if(USB_GetSource() == USB_SRC_UNKNOWN)
        {
            DBG_MSG("^B USB PlugOn unknown, update again\r\n");
            USB_UpdateSource();
        }
        if(USB_GetSource() == USB_SRC_USB_PC)
        {
#if (POWERON_TESTFLOW == ENABLE)
#else
            //#NT#2010/12/2#Jeah Yen -begin
            #if (USB_CHARGE_FUNCTION == ENABLE)
            if (!GxPower_GetControl(GXPWR_CTRL_IS_BATT_INSERT))
            {
                //USB PC plug, but No battry for charge, shutdown now!
                DBG_MSG("^B USB PlugOn,PwrOff 1\r\n");
                System_PowerOff(SYS_POWEROFF_NORMAL);
                return NVTEVT_CONSUME;
            }
            #endif
#endif
            //#NT#2010/12/2#Jeah Yen -end
            USB_PlugInSetFunc();
            FlowMode_OnUsbPlug();
            //NOT start USB charge here! Only wait until USB "configured" OK, then try to do USB charge!
            //Please refer to System_OnUsbChargeCurrent() to complete your charging control.
        }
        #endif
        m_bVendorConnected = FALSE;
    }
    else
    {
        MSDCNVTCB_OPEN Open={0};
        Open.fpUSBMakerInit = USBMakerInit_UMSD;
        MsdcNvtCb_Open(&Open);
        m_bVendorConnected = TRUE;
    }
    DBG_IND("USB plug - end\r\n");
    return NVTEVT_CONSUME;
}

INT32 System_OnUsbRemove(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //usb unplug

    DBG_IND("USB unplug - begin\r\n");

    if(m_bVendorConnected==FALSE)
    {
        #if (UI_STYLE==UI_STYLE_DRIVER)
        if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
        {
            if (m_bACPlug == TRUE)
            {
               m_bACPlug = FALSE;
               Ux_PostEvent(NVTEVT_AC_UnPlug, 0, 0);
            }
        }
        #endif
        FlowMode_OnUsbUnplug();
        if(GxCustom_GetControl(GXCUSTOM_CTRL_DELAYOFF_TIME) == 0||DrvPower_GetControl(DRVPWR_CTRL_BATTERY_LEVEL)==DRVPWR_BATTERY_LVL_EMPTY||DrvPower_GetControl(DRVPWR_CTRL_BATTERY_LEVEL)==DRVPWR_BATTERY_LVL_0)
                {
                GxVideo_SetDeviceCtrl(DOUT1, DISPLAY_DEVCTRL_BRIGHTLVL, 4);
                    Ux_PostEvent(NVTEVT_BATTERY, 0, 0);
                 if(gMovData.State==MOV_ST_REC)
                    {
                    FlowMovie_StopRec();
                   
                    }
                    
				debug_msg("================shutdown!==============\r\n");
                                Ux_PostEvent(NVTEVT_KEY_POWER_REL, 1, NVTEVT_KEY_RELEASE);
                }   
    }
    else
    {
        MsdcNvtCb_Close();
        m_bVendorConnected=FALSE;
    }
    USB_UnPlugSetFunc();
    USB_UpdateSource();
    GxUSB_UpdateConnectType();//jacky lan
	
    Ux_PostEvent(NVTEVT_BATTERY, 0, 0);
    DBG_IND("USB unplug - end\r\n");
    return NVTEVT_CONSUME;
}

INT32 System_OnUsbChargeCurrent(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiChargeCurrent;
    DBG_IND("USB charging current(%d)\r\n",paramArray[0]);
    uiChargeCurrent = paramArray[0];
    switch (uiChargeCurrent)
    {
        case USB_CHARGE_CURRENT_2P5MA:
            #if (USB_CHARGE_FUNCTION == ENABLE)
            Power_StopUSBCharge();
            #endif
            break;
        case USB_CHARGE_CURRENT_100MA:
            break;
        case USB_CHARGE_CURRENT_500MA:
        case USB_CHARGE_CURRENT_1500MA:
            #if (USB_CHARGE_FUNCTION == ENABLE)
            Power_StartUSBCharge();
            #endif
            break;
        default:
            break;
    }

    return NVTEVT_CONSUME;
}

void USB_PlugInSetFunc(void)
{
    GxPower_SetControl(GXPWR_CTRL_AUTOPOWEROFF_RESET,0);
    GxPower_SetControl(GXPWR_CTRL_SLEEP_RESET,0);
    SxTimer_SetFuncActive(SX_TIMER_DET_SLEEP_ID,FALSE);
    //SxTimer_SetFuncActive(SX_TIMER_DET_AUTOPOWEROFF_ID,FALSE);
    SxTimer_SetFuncActive(SX_TIMER_DET_MODE_ID,FALSE);
    SxTimer_SetFuncActive(SX_TIMER_DET_DELAYOFF_ID,FALSE);
    GxCustom_SetControl(GXCUSTOM_CTRL_DELAYOFF_EN,FALSE);	
}

void USB_UnPlugSetFunc(void)
{
    SxTimer_SetFuncActive(SX_TIMER_DET_DELAYOFF_ID,TRUE);
    GxCustom_SetControl(GXCUSTOM_CTRL_DELAYOFF_EN,TRUE);	
}
