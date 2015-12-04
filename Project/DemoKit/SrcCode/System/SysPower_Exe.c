/*
    System Power Callback

    System Callback for Power Module.

    @file       SysPower_Exe.c
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
#include "GxInput.h"
#include "DxPower.h"
#include "DxLens.h"
#include "GxFlash.h"
#include "GxPower.h"
#include "DbgUtApi.h"
#include "PipView.h"

#if (UI_STYLE==UI_STYLE_DRIVER)
#include "UsbDevDef.h"
#endif

//global debug level: PRJ_DBG_LVL
#include "PrjCfg.h"
//local debug level: THIS_DBGLVL
#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          SysPwrExe
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"

#include "NVTUserCommand.h"
#include "UIFlowMovieFuncs.h"
#include "MediaRecAPI.h"
#include "Gsensor.h"
extern INT32 Get_GSensorSensitivity(void);
extern BOOL UIFlowWndMovie_IsGsensorTrig(void);
extern void UIFlowWndMovie_SetGsensorTrigFlag(BOOL En);


#define KEYSCAN_AUTOLCDOFF_DISABLED   0
#define KEYSCAN_DELAYOFF_DISABLED   0
#define KEYSCAN_GPS_DISABLED  0

static volatile UINT32  g_uiKeyScanAutoLcdoffCnt;
static volatile BOOL    g_bKeyScanAutoLcdoffEn = FALSE;
static volatile UINT32  g_uiKeyScanDelayoffCnt;
static volatile BOOL    g_bKeyScanDelayoffEn = TRUE;

static volatile UINT32  g_uiKeyScanGpsoffCnt;
static volatile BOOL    g_bKeyScanGpsoffEn = FALSE;

static UINT32   uiSystemAutoLcdoffTime     = KEYSCAN_AUTOLCDOFF_DISABLED;
static UINT32   uiSystemDelayoffTime     = KEYSCAN_DELAYOFF_DISABLED;
static UINT32   uiSystemGpsoffTime     = KEYSCAN_GPS_DISABLED;

int SX_TIMER_DET_SLEEP_ID = -1;
int SX_TIMER_DET_AUTOPOWEROFF_ID = -1;
int SX_TIMER_DET_BATT_ID = -1;
int SX_TIMER_DET_CHARGE_ID = -1;
int SX_TIMER_DET_SHOWADC = -1;
int SX_TIMER_DET_BATTINS_ID = -1;
int SX_TIMER_DET_AUTOLCDOFF_ID = -1;
int SX_TIMER_DET_DELAYOFF_ID = -1;
int SX_TIMER_DET_GSENSOR_ID = -1;

void UI_DetAutoSleep(void);
void UI_DetAutoPoweroff(void);
void UI_DetBattery(void);
void UI_DetBatteryInsert(void);
void UI_DetAutoLCDOff(void);
void UI_DetDelayPowerOff(void);
void UI_DetGsensor(void);
void System_DetShowADC(void);

UINT32  GxCustom_GetControl(GXCUSTOM_CTRL CustomCtrl);
void GxCustom_SetControl(GXCUSTOM_CTRL CustomCtrl, UINT32 value);

#if (AUTOSLEEP_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetSleep, UI_DetAutoSleep, 50, TRUE)
#endif
#if (AUTOPWROFF_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetAutoPoweroff, UI_DetAutoPoweroff, 50, TRUE)
#endif
#if (PWRLEVEL_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetBattery, UI_DetBattery, 25, TRUE)
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetCharge, GxPower_DetCharge, 50, TRUE)
#endif
#if (SHOW_ADC_ON_SCREEN == ENABLE)
SX_TIMER_ITEM(System_DetShowADC, System_DetShowADC, 15, FALSE)
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
SX_TIMER_ITEM(GxPower_DetBatteryIns, UI_DetBatteryInsert, 25, TRUE)
#endif
SX_TIMER_ITEM(UI_DetAutoLCDOff, UI_DetAutoLCDOff, 50, TRUE)
SX_TIMER_ITEM(UI_DetDelayPowerOff, UI_DetDelayPowerOff, 50, FALSE)
SX_TIMER_ITEM(UI_DetGsensor, UI_DetGsensor, 1, FALSE)

#if (POWERON_TESTFLOW == ENABLE)
BOOL System_Is_USBPC_DeadBattery(void);
#endif

UINT32 DxPower_CB(UINT32 event, UINT32 param1, UINT32 param2)
{
    #if LENS_FUNCTION
    switch (event)
    {
        case DRVPWR_CB_IS_LENS_MOVING:
            return GxFlash_GetData(FLASH_SUSPEND_CALLER_CNT);
        case DRVPWR_CB_DUMMY_LOAD_START:
            {
                PMOTOR_TAB pMotorObj = motor_getObject();
                PLENS_DEVICE_OBJ pLensDevice =DrvLens_GetDevice();


                //first time,need to init motor dirver gpio
                pMotorObj->init(pLensDevice);
                pMotorObj->setSignal(MD_SIGNAL_STANDBY,ON);
                //Open shutter
                pMotorObj->shutter_setState(MOTOR_SHUTTER_DUMMYLOAD,SHUTTER_DUMMYLOAD_START);
            }
            return 0;
        case DRVPWR_CB_DUMMY_LOAD_END:
            {
                PMOTOR_TAB pMotorObj = motor_getObject();
                pMotorObj->shutter_setState(MOTOR_SHUTTER_DUMMYLOAD,SHUTTER_DUMMYLOAD_END);
                pMotorObj->setSignal(MD_SIGNAL_STANDBY,OFF);
            }
            return 0;
        default:
            DBG_ERR("event(%d)\r\n",event);
            return 0;

    }
    #else
    return 0;
    #endif
}

void System_OnPowerPreInit(void)
{
    // register DxPower Callback function
    DrvPower_RegCB(DxPower_CB);

#if (POWERON_TESTFLOW == ENABLE)
    gIsUSBChargePreCheck = System_Is_USBPC_DeadBattery();
#endif

    if(GxKey_GetData(GXKEY_NORMAL_KEY) & FLGKEY_UP)
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, TRUE);
    else
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, FALSE);
#if (POWERON_TESTFLOW == ENABLE)
    if(gIsUSBChargePreCheck)
        GxPower_SetControl(GXPWR_CTRL_IGNORE_BATT_CHECK, TRUE);
#else
    //GxPower_OnSystem(SYSTEM_CMD_POWERON); //Do Dummy Load
    GxPower_PowerON();
#endif

    #if _MIPS_TODO
    HwClock_EnablePowerAlarm(FALSE);
    #endif

    //If press up key, enter "Battery Calibration" mode.
    #if 0
    if(GxKey_GetData(GXKEY_NORMAL_KEY) & FLGKEY_UP)
    #else
    if(GxPower_GetControl(GXPWR_CTRL_IGNORE_BATT_CHECK))
    #endif
    {
        //"battery calibration" path
        DBG_IND("USER force to ignore low-battery check.\r\n");

        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
    }
    else
    {
        //normal path
        DBG_IND("Start low-battery check (dummyload).\r\n");

        //if under low-battery condition
        if(!GxPower_DummyLoad())
        {
            System_SetState(SYS_STATE_POWERON, SYS_POWERON_LOWPOWER);
            System_SetState(SYS_STATE_POWEROFF, SYS_POWEROFF_LOWPOWER);
        }
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
}

extern void Power_CB(UINT32 event, UINT32 param1, UINT32 param2);

void System_OnPowerInit(void)
{
     //PHASE-1 : Init & Open Drv or DrvExt
    {
    GxPower_RegCB(Power_CB);     //Register CB function of GxPower
    }
    //PHASE-2 : Init & Open Lib or LibExt
    {
    //1.設定init值
    //2.設定CB值,

    //3.註冊SxJob服務 ---------> System Job
    //4.註冊SxTimer服務 ---------> Detect Job
    #if (AUTOSLEEP_FUNCTION == ENABLE)
    SX_TIMER_DET_SLEEP_ID = SxTimer_AddItem(&Timer_GxPower_DetSleep);
    #endif
    #if (AUTOPWROFF_FUNCTION == ENABLE)
    SX_TIMER_DET_AUTOPOWEROFF_ID = SxTimer_AddItem(&Timer_GxPower_DetAutoPoweroff);
    #endif
    #if (PWRLEVEL_FUNCTION == ENABLE)
    SX_TIMER_DET_BATT_ID = SxTimer_AddItem(&Timer_GxPower_DetBattery);
    #endif
    #if (USB_CHARGE_FUNCTION == ENABLE)
    SX_TIMER_DET_CHARGE_ID = SxTimer_AddItem(&Timer_GxPower_DetCharge);
    #endif
    #if (SHOW_ADC_ON_SCREEN == ENABLE)
    SX_TIMER_DET_SHOWADC = SxTimer_AddItem(&Timer_System_DetShowADC);
    #endif
    #if (USB_CHARGE_FUNCTION == ENABLE)
    SX_TIMER_DET_BATTINS_ID = SxTimer_AddItem(&Timer_GxPower_DetBatteryIns);
    #endif
 //LCD Auto OFF
    SX_TIMER_DET_AUTOLCDOFF_ID = SxTimer_AddItem(&Timer_UI_DetAutoLCDOff);

    SX_TIMER_DET_DELAYOFF_ID = SxTimer_AddItem(&Timer_UI_DetDelayPowerOff);

    SX_TIMER_DET_GSENSOR_ID = SxTimer_AddItem(&Timer_UI_DetGsensor);

    //5.註冊SxCmd服務 ---------> Cmd Function
    //GxPower_InstallCmd();
    //DbgUt_InstallCmd();
    }
}

void System_OnPowerExit(void)
{
    //PHASE-2 : Close Lib or LibExt
    {
    }
    //PHASE-1 : Close Drv or DrvExt
    {
    }
}

///////////////////////////////////////////////////////////////////////////////

#if (AUTOSLEEP_FUNCTION == ENABLE)
void UI_DetAutoSleep(void)
{
    if((!UI_IsForceLock()) && (!UI_IsForceLockAutoSleep()))
    {
        //debug_msg("DetAutoSleep\r\n");
        GxPower_DetSleep();
    }
}
#endif
#if (AUTOPWROFF_FUNCTION == ENABLE)
void UI_DetAutoPoweroff(void)
{
    if((!UI_IsForceLock()) && (!UI_IsForceLockAutoPWROff()))
    {
        GxPower_DetAutoPoweroff();
    }
}
#endif
#if (PWRLEVEL_FUNCTION == ENABLE)
void UI_DetBattery(void)
{
    if(!UI_IsForceLock())
    {
        //#NT#2012/10/23#Philex Lin - begin
        // send NVTEVT_BATTERY event to flash power charging icon in power charging mode
        #if (UI_STYLE==UI_STYLE_DRIVER)
         // power charging case
         if (GxUSB_GetConnectType() == USB_CONNECT_CHARGER)
         {
            Ux_PostEvent(NVTEVT_BATTERY, 0);
         } else {
            // normal battery detect case
            GxPower_DetBattery();
         }
        #else
        GxPower_DetBattery();
        #endif
        //#NT#2012/10/23#Philex Lin - end
    }
}
#endif
#if (USB_CHARGE_FUNCTION == ENABLE)
BOOL gIsBattInsert = TRUE;
BOOL gIsBattDead = FALSE;
void UI_DetBatteryInsert(void)
{
    if(!UI_IsForceLock())
    {
        BOOL bInsert;
        #if (USB_CHARGE_FUNCTION == ENABLE)
        bInsert = GxPower_GetControl(GXPWR_CTRL_IS_BATT_INSERT);
        #else
        bInsert = TRUE;
        #endif
        if(bInsert != gIsBattInsert)
        {
            gIsBattInsert = bInsert;
            debug_msg("  Battery Insert = %s\r\n", gIsBattInsert?"YES":"NO");
        }
    }
}
#endif
#if (SHOW_ADC_ON_SCREEN == ENABLE)
void System_DetShowADC(void)
{
    IRECT  StringRect = { 200, 162, 340, 40 };
    IRECT*  pRect = &StringRect;
    static char ADCStr[20];

    //UIDebug_Sys("aa  = %d\r\n",UI_GetData(UI_ShowADC));
    if (UI_GetData(UI_ShowADC) == TRUE)
    {
        DC**     pDCList;
        UI_SetDisplayDirty(TRUE); //set TRUE to force dirty current begin/end

        pDCList = (DC**)UI_BeginScreen();
        sprintf(ADCStr, "AD0-%ld,AD1-%ld,AD2-%ld", adc_readData(0), adc_readData(1),adc_readData(2));
        GxGfx_SetTextStroke((const FONT*)gDemo_Font, FONTSTYLE_NORMAL, SCALE_1X);
        GxGfx_SetTextColor(_OSD_INDEX_TRANSPART, _OSD_INDEX_TRANSPART, 0);
        GxGfx_FillRect(((DC**)pDCList)[GxGfx_OSD],pRect->uiLeft,pRect->uiTop,pRect->uiLeft+pRect->uiWidth,pRect->uiTop+pRect->uiHeight);
        GxGfx_SetShapeColor(_OSD_INDEX_TRANSPART, _OSD_INDEX_TRANSPART, 0);
        GxGfx_SetTextColor(_OSD_INDEX_RED, _OSD_INDEX_GRAY, 0);
        GxGfx_Text(((DC**)pDCList)[GxGfx_OSD], pRect->uiLeft, pRect->uiTop, ADCStr);
        UI_EndScreen((UINT32)pDCList);
    }

}
#endif


///////////////////////////////////////////////////////////////////////////////

INT32 System_OnBattChange(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_PostEvent(NVTEVT_BATTERY, 0);
    debug_msg("battery changed %d**********\r\n",GxPower_GetControl(GXPWR_CTRL_BATTERY_LEVEL));

    return NVTEVT_CONSUME;
}

INT32 System_OnBattEmpty(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    if (UI_GetData(FL_IsCIPARunning))
    {
        if (UI_GetData(FL_IsEmptyBatt) == FALSE)
        {
            UI_SetData(FL_IsEmptyBatt, TRUE);
        }
    }
    #if (EMPTYPWROFF_FUNCTION == ENABLE)
    else
    {
        #if (UI_STYLE==UI_STYLE_DRIVER)
        // set battery low event to ui window
        Ux_PostEvent(NVTEVT_BATTERY_LOW, 0);
        #else
        System_PowerOff(SYS_POWEROFF_BATTEMPTY);
        #endif
    }
    #endif
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeOK(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeSuspend(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    return NVTEVT_CONSUME;
}

INT32 System_OnPwrChargeResume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    //GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,TRUE);
    return NVTEVT_CONSUME;
}





void System_ResetPowerSaveCount(void)
{
    //reset auto power off and sleep counter
    #if (AUTOPWROFF_FUNCTION == ENABLE)
    GxPower_SetControl(GXPWR_CTRL_AUTOPOWEROFF_RESET,0);
    #endif
    #if (AUTOSLEEP_FUNCTION == ENABLE)
    GxPower_SetControl(GXPWR_CTRL_SLEEP_RESET,0);
    #endif
    GxCustom_SetControl(GXCUSTOM_CTRL_AUTOLCDOFF_RESET,0);
}

#if (USB_CHARGE_FUNCTION == ENABLE)
void Power_StartUSBCharge(void)
{
    if (!gIsBattInsert)
    {
        DBG_DUMP("  NO Battery, Ignore START\r\n");
        return;
    }

    if (GxPower_GetControl(GXPWR_CTRL_BATTERY_CHARGE_EN))
    {
        DBG_DUMP("  Already UNDER Charge, Ignore START\r\n");
        return;
    }

    if((USB_GetSource() == USB_SRC_USB_ADAPTER))
    {
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_HIGH);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_HIGH);
    }
#if _MIPS_TODO
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_ADAPTER) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Do not charge, just stop check low-battery
        DBG_DUMP("GxPower: Stop low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
        return;
    }
    //#NT#2010/12/10#Jeah Yen -end
#endif
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_PC))
    {
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_MEDIUM);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_MEDIUM);
    }
    //#NT#2010/12/10#Jeah Yen -end

    //#NT#2010/12/10#Jeah Yen -begin
    DBG_DUMP("GxPower: Stop low-battery check.\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, FALSE);
    DBG_DUMP("GxPower: Start to Charge Battery\r\n");
#if (USB_CHARGE_VERIFY == ENABLE)
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
#endif
    GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_EN,TRUE);
    //#NT#2010/12/10#Jeah Yen -end
#if (USB_CHARGE_VERIFY == ENABLE)
#else
    //GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,FALSE);
#endif
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,TRUE);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SETLED_SPEED,GXLED_1SEC_LED_TOGGLE_CNT);
    GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,TRUE);

    // disable flash re-charge
    SxTimer_SetFuncActive(SX_TIMER_DET_RECHARGE_ID,FALSE);
}
void Power_StopUSBCharge(void)
{
    //DBG_MSG("  gIsUSBInsert=%d,gIsBattInsert=%d,gIsBattDead=%d,gIsUSBAdapter=%d\r\n",gIsUSBInsert,gIsBattInsert,gIsBattDead,gIsUSBAdapter);
    if (!gIsBattInsert)
    {
        DBG_DUMP("  NO Battery, Ignore STOP\r\n");
        return;
    }
#if _MIPS_TODO
    //#NT#2010/12/10#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_USB_ADAPTER) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Not charge yet, just start check low-battery
        DBG_DUMP("GxPower: Start low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
    //#NT#2010/12/10#Jeah Yen -end

    //#NT#2011/3/30#Jeah Yen -begin
    if((USB_GetSource() == USB_SRC_NONE) && (Power_GetSource() != POWER_SRC_USB_ADAPTER))
    {
        //Cannot charge, start check low-battery
        DBG_DUMP("GxPower: Start low-battery check.\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    }
    //#NT#2011/3/30#Jeah Yen -end

    if((USB_GetSource() == USB_SRC_USB_ADAPTER))
    {
        //DBG_MSG("  USB Src = Adaptor\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_LOW);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_LOW);
    }
#endif
    if((USB_GetSource() == USB_SRC_USB_PC))
    {
        //DBG_MSG("  USB Src = PC\r\n");
        GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_CURRENT,BATT_CHARGE_CURRENT_LOW);
        DBG_MSG("  Charge Current = %d\r\n", BATT_CHARGE_CURRENT_LOW);
    }

    if (!GxPower_GetControl(GXPWR_CTRL_BATTERY_CHARGE_EN))
    {
        DBG_DUMP("  Already NOT Charge, Ignore STOP\r\n");
        return;
    }

    //#NT#2010/12/10#Jeah Yen -begin
    DBG_DUMP("GxPower: Stop to Charge Battery\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_CHARGE_EN,FALSE);
#if _MIPS_TODO
    if(!gIsUSBChargePreCheck
        && (GxSystem_GetState(SYSTEM_STATE_POWERON) != SYSTEM_POWERON_CHARGE)
        )
    {
#if (USB_CHARGE_VERIFY == ENABLE)
#else
        GxLED_SetCtrl(KEYSCAN_LED_GREEN,SET_TOGGLE_LED,FALSE);
        GxLED_SetCtrl(KEYSCAN_LED_GREEN,TURNON_LED,TRUE);
#endif
        //GxLED_SetCtrl(KEYSCAN_LED_RED,SET_TOGGLE_LED,FALSE);
        //GxLED_SetCtrl(KEYSCAN_LED_RED,TURNON_LED,FALSE);
    }
#endif
    DBG_DUMP("GxPower: Start low-battery check.\r\n");
    GxPower_SetControl(GXPWR_CTRL_BATTERY_DETECT_EN, TRUE);
    // enable flash re-charge
    SxTimer_SetFuncActive(SX_TIMER_DET_RECHARGE_ID,TRUE);
    //#NT#2010/12/10#Jeah Yen -end

}
#endif



UINT32  GxCustom_GetControl(GXCUSTOM_CTRL CustomCtrl)
{
    UINT32 getv = 0;

    switch (CustomCtrl)
    {
        case GXCUSTOM_CTRL_AUTOLCDOFF_EN:
            getv = (UINT32)g_bKeyScanAutoLcdoffEn;
            break;
        case GXCUSTOM_CTRL_AUTOLCDOFF_TIME:
            getv = uiSystemAutoLcdoffTime;
            break;
        case GXCUSTOM_CTRL_DELAYOFF_EN:
            getv = (UINT32)g_bKeyScanDelayoffEn;
            break;
        case GXCUSTOM_CTRL_DELAYOFF_TIME:
            getv = (UINT32)uiSystemDelayoffTime;
            break;
        case GXCUSTOM_CTRL_GPS_TIME:
            getv = (UINT32)uiSystemGpsoffTime;
            break;
        default:
            DBG_ERR("CustomCtrl(%d)\r\n",CustomCtrl);
            getv = 0;
            break;
    }
    return getv;
}

void GxCustom_SetControl(GXCUSTOM_CTRL CustomCtrl, UINT32 value)
{
    DBG_IND("cmd=(%d), value=%d\r\n",CustomCtrl,value);
    switch (CustomCtrl)
    {
        case GXCUSTOM_CTRL_AUTOLCDOFF_EN:
            if (g_bKeyScanAutoLcdoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanAutoLcdoffCnt = 0;
            }
            g_bKeyScanAutoLcdoffEn = (INT32)value;
            break;
        case GXCUSTOM_CTRL_AUTOLCDOFF_TIME:
            uiSystemAutoLcdoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_AUTOLCDOFF_TIME cnt = %d\r\n", uiSystemAutoLcdoffTime);
            break;
	 case GXCUSTOM_CTRL_AUTOLCDOFF_RESET:
	 	g_uiKeyScanAutoLcdoffCnt=0;
		break;
        case GXCUSTOM_CTRL_DELAYOFF_EN:
            if (g_bKeyScanDelayoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanDelayoffCnt = 0;
            }
            g_bKeyScanDelayoffEn = value;
            DBG_IND("g_bKeyScanDelayoffEn cnt = %d\r\n", g_bKeyScanDelayoffEn);			
            break;
        case GXCUSTOM_CTRL_DELAYOFF_TIME:
            uiSystemDelayoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_DELAYOFF_TIME cnt = %d\r\n", uiSystemDelayoffTime);
            break;

	 case GXCUSTOM_CTRL_DELAYOFF_RESET:
	 	g_uiKeyScanDelayoffCnt = 0;
	 	break;
        case GXCUSTOM_CTRL_GPS_EN:
            if (g_bKeyScanGpsoffEn == FALSE && value == TRUE)
            {
                g_uiKeyScanGpsoffCnt = 0;
            }
            g_bKeyScanGpsoffEn = (INT32)value;
            break;
        case GXCUSTOM_CTRL_GPS_TIME:
            uiSystemGpsoffTime = value;
            DBG_IND("GXCUSTOM_CTRL_GPS_TIME cnt = %d\r\n", uiSystemGpsoffTime);
            break;

        default:
            DBG_ERR("CustomCtrl(%d)\r\n",CustomCtrl);
            break;
    }
}

void UI_DetAutoLCDOff(void)
{
    if ((GxCustom_GetControl(GXCUSTOM_CTRL_AUTOLCDOFF_TIME) != KEYSCAN_AUTOLCDOFF_DISABLED)
        && (g_bKeyScanAutoLcdoffEn == TRUE))
    {
        DBG_IND("Auto Lcd Off Cnt = %d\r\n", g_uiKeyScanAutoLcdoffCnt);

        g_uiKeyScanAutoLcdoffCnt++;

        if (g_uiKeyScanAutoLcdoffCnt >= GxCustom_GetControl(GXCUSTOM_CTRL_AUTOLCDOFF_TIME))
        {
            DBG_IND("Do Auto Lcd Off!\r\n");
            //System_PowerOff(SYS_POWEROFF_NORMAL);
            if(GPIOMap_IsLCDBacklightOn()==TRUE)
            {
			GPIOMap_TurnOffLCDBacklight();
	      }     
        }
    }
}


void UI_DetDelayPowerOff(void)
{
    if ((GxCustom_GetControl(GXCUSTOM_CTRL_DELAYOFF_TIME) != KEYSCAN_DELAYOFF_DISABLED)
        && (g_bKeyScanDelayoffEn == TRUE))
    {
        //debug_msg("Auto DelayPwrOff Cnt = %d\r\n", g_uiKeyScanDelayoffCnt);
        g_uiKeyScanDelayoffCnt++;

        if (g_uiKeyScanDelayoffCnt >=GxCustom_GetControl(GXCUSTOM_CTRL_DELAYOFF_TIME))
        {
             DBG_IND("Do AutoDelayPwrOff!\r\n");
	        if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
	        {
	        	switch(gMovData.State)	
	        	{
	        	     case MOV_ST_REC:
			     case MOV_ST_REC|MOV_ST_ZOOM:
				//MediaRec_Stop(MEDIAREC_WAIT_END);  	
                		FlowMovie_StopRec();				
			     break;
	        	}
	        }			 
		Ux_PostEvent(NVTEVT_SYSTEM_SHUTDOWN, 1, 0);
            
        }
    }
}

void UI_DetGsensor(void)
{
#if 1
    Gsensor_Data GS_Data;
    INT32 gsensor_trig;
    static UINT32 X,Y,Z;
    static UINT32 counter=0;
    if(counter<5)
    {
	GSensor_GetStatus(&GS_Data);
	counter++;
    }
    else
    {
        if (System_GetState(SYS_STATE_CURRMODE)==PRIMARY_MODE_MOVIE)
        {
        	switch(gMovData.State)	
        	{
        	     case MOV_ST_REC:
		     case MOV_ST_REC|MOV_ST_ZOOM:
		        if(UI_GetData(FL_MOVIE_GSENSOR)!=GSENSOR_OFF)
			  {
				if(GSensor_GetStatus(&GS_Data)==TRUE)
		            {
		            #if 1
                        		Ux_PostEvent(NVTEVT_GSENSOR_TRIG, 0);
				#else					
		            		gsensor_trig=Get_GSensorSensitivity();
		                   if((abs(GS_Data.Axis.Xacc-X)>gsensor_trig)||(abs(GS_Data.Axis.Yacc-Y)>gsensor_trig)
						||(abs(GS_Data.Axis.Zacc-Z)>gsensor_trig))
		                   	{
		            			debug_msg("gsensor trig TRUE ...%d,%d,%d, threshold:%d\r\n",GS_Data.Axis.Xacc,GS_Data.Axis.Yacc,GS_Data.Axis.Zacc,gsensor_trig);
						if(UIFlowWndMovie_IsGsensorTrig()==FALSE)
						{
							//UIFlowWndMovie_SetGsensorTrigFlag(TRUE);
                        				Ux_PostEvent(NVTEVT_GSENSOR_TRIG, 0);
						}	
		                   	}
					X=GS_Data.Axis.Xacc;
					Y=GS_Data.Axis.Yacc;
					Z=GS_Data.Axis.Zacc;		
                         #endif					
		            }		
			    }
			break;
        	}
        }
    }
#endif	
}

