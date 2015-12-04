#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "debug.h"
#include "UIMovieObj.h"
#include "AppLib.h"
#include "MediaRecAPI.h"
#include "PrjCfg.h"
#include "UIStorageCheck.h"
#if(WIFI_AP_RTSP_FUNC==ENABLE)
#include "RtspNvtApi.h"
#endif
#include "NvtSystem.h"

#define __MODULE__          UIMovieRecObj
//#define __DBGLVL__ 0        //OFF mode, show nothing
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
//#define __DBGLVL__ 2        //TRACE mode, show err, wrn, ind, msg and func and ind, msg and func can be filtering by __DBGFLT__ settings
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"

#define UIMovRecObj_LibStartRec(x)                  MediaRec_Record(x)
#define UIMovRecObj_LibStopRec(x)                   MediaRec_Stop(x)
#define UIMovRecObj_LibDisk2Second(x)               MediaRec_Disk2SecondWithType(MEDIAREC_COUNTTYPE_FS,0)
#define UIMovRecObj_LibSetAudioVolume(x)            MedaiRec_SetAudioVolume(x)

#if (_UIMOVOBJ_DBG_LVL_ > _UIMOVOBJ_DBG_CHK_)

void UIMovRecObj_DumpInitParam(UINT32 paramAddr)
{
}

#else

#define UIMovRecObj_DumpInitParam(x)

#endif


INT32 UIMovRecObj_Close(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    return NVTEVT_CONSUME;
}
INT32 UIMovRecObj_StartRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UIMovRecObj_LibStartRec();
    return NVTEVT_CONSUME;
}
INT32 UIMovRecObj_StopRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UIMovRecObj_LibStopRec(MEDIAREC_WAIT_END);
    return NVTEVT_CONSUME;
}
INT32 UIMovRecObj_StartRtsp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if(WIFI_AP_RTSP_FUNC==ENABLE)
    RTSPNVT_OPEN Open={0};
    Open.uiApiVer = RTSPNVT_API_VERSION;
    Open.Type = RTSPNVT_TYPE_MEDIA_REC;
    Open.uiPortNum = 554;
    RtspNvt_Open(&Open);
    DscMovie_RegRTSPSetBSCB(RtspNvt_RTSPSetBS);
#endif
    return NVTEVT_CONSUME;
}
INT32 UIMovRecObj_StopRtsp(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if(WIFI_AP_RTSP_FUNC==ENABLE)
    DscMovie_RegRTSPSetBSCB(NULL);
    RtspNvt_Close();
#endif
    return NVTEVT_CONSUME;
}
UINT32 UIMovRecObj_GetData(RECMOVIE_GET_DATATYPE dataType)
{
    UINT32 retV = 0;

    switch(dataType)
    {
        case RECMOVIE_MAXSECOND:
            //#NT#2014/4/29#Philex Lin-begin
            // It is reasonable to get max recording time if SD card is OK. Otherwise, return 0 directly
            if(UIStorageCheck(STORAGE_CHECK_ERROR, NULL) == FALSE)
            {
                MediaRec_UpdateSetting2CaluSec();
                MediaRec_SetDisk2SecRestriction(MEDIAREC_RESTR_INDEXBUF, 0);
                MediaRec_SetDisk2SecRestriction(MEDIAREC_RESTR_FSLIMIT, 0);
                retV = UIMovRecObj_LibDisk2Second();
            } else
                retV = 0;
            //#NT#2014/4/29#Philex Lin-end
            DBG_IND("max sec %d\r\n", retV);
            break;

        default:
            DBG_IND("not support type %d\r\n", dataType);
            break;
    }

    return retV;
}
void UIMovRecObj_WaitReady(void)
{
}
void UIMovRecObj_SetData(RECMOVIE_SET_DATATYPE dataType, UINT32 data)
{
    switch (dataType)
    {
    case RECMOVIE_AUD_VOLUME:
        UIMovRecObj_LibSetAudioVolume(data);
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////

EVENT_ENTRY UIMovieRecObjCmdMap[] =
{
    {NVTEVT_CLOSE_REC_MOVIE,                UIMovRecObj_Close},
    {NVTEVT_START_REC_MOVIE,                UIMovRecObj_StartRec},
    {NVTEVT_STOP_REC_MOVIE,                 UIMovRecObj_StopRec},
    {NVTEVT_START_RTSP,                     UIMovRecObj_StartRtsp},
    {NVTEVT_STOP_RTSP,                      UIMovRecObj_StopRtsp},

    {NVTEVT_NULL,                           0},  //End of Command Map
};

CREATE_APP(UIMovieRecObj,APP_SETUP)

//#NT#2010/09/16#Lily Kao -end

