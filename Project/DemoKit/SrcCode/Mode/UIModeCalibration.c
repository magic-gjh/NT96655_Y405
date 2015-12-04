////////////////////////////////////////////////////////////////////////////////
#include "SysCommon.h"
//#include "AppCommon.h"
////////////////////////////////////////////////////////////////////////////////
#include "AppLib.h"
#include "UIMode.h"
#include "UIModeCalibration.h"
#include "UISetup.h"
//UIControl
#include "UIFrameworkExt.h"

//UIInfo
#include "UIInfo.h"
#if (UI_STYLE==UI_STYLE_DRIVER)
#include "UIFlow.h"
#endif
#include "DeviceCtrl.h"

extern BOOL     g_bPreviewFirstTime;

int PRIMARY_MODE_CALIBRATION = -1;      ///< calibration


void ModeCalibration_Open(void);
void ModeCalibration_Close(void);

BOOL ModeCalibration_CheckEng(void)
{
  BOOL ret = FALSE;
  FST_FILE filehdl = NULL;

     filehdl = FileSys_OpenFile(ENG_MODE_FILE,FST_OPEN_READ);
     if (filehdl!=NULL)
     {
        FileSys_CloseFile(filehdl);
        ret = TRUE;
     }

  return ret;
}

void ModeCalibration_Open(void)
{
      Input_ResetMask();

#if (UI_STYLE==UI_STYLE_DRIVER)
      //_OpenWindow((VControl *)(&UIMenuWndCalibrationCtrl), 0);
#endif

    // Init Flow Window config for different DSC modes
    Ux_SetActiveApp(&CustomPhotoObjCtrl);
//#NT#2010/04/15#Jeah Yen -FASTBOOT- -begin
    //#NT#2010/03/02#JeahYen -begin
    if(g_bPreviewFirstTime)
    {
        //skip
        //GxSystem_Mark("APP-SKIP\r\n");
    }
    else
    {
        //TM_Begin(DEVCTRL_CCD);
        // Control CCD/LCD power on/off, lens, shutter...
        DevCtrl_ModePhoto();
        //TM_End(DEVCTRL_CCD);
        //TM_Begin(APP_OPEN);
        // Init Photo app and open
        Ux_SendEvent(0, NVTEVT_EXE_OPEN, 0);
        //TM_End(APP_OPEN);

    }
    if(!g_bPreviewFirstTime)
    {
        DevCtrl_ModePhoto_WaitFinish();
    }

    //#NT#2013/08/08#KS Hung -begin
    GxPower_SetControl(GXPWR_CTRL_AUTOPOWEROFF_EN, FALSE);
    //#NT#2013/08/08#KS Hung -end
    Ux_OpenWindow((VControl *)(&UIMenuWndCalibrationCtrl), 0);
    g_bPreviewFirstTime = FALSE; //turn off fast path
    //TM_End(OFF_TO_CAPTURE);
//#NT#2010/04/15#Jeah Yen -FASTBOOT- -end

}
void ModeCalibration_Close(void)
{
#if (UI_STYLE==UI_STYLE_DRIVER)
      Ux_CloseWindowClear((VControl *)(&UIMenuWndCalibrationCtrl), 0);
      Ux_SendEvent(0, NVTEVT_EXE_CLOSE, 0);
#endif
}


SYS_MODE gModeCalibration =
{
    "CALIBRATION",
    ModeCalibration_Open,
    ModeCalibration_Close,
    NULL,
    NULL,
    NULL,
    NULL
};
