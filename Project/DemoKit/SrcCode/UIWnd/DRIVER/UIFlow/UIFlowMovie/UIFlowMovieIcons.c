
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIFlow.h"
#include "JpgEnc.h"
#include "Exif.h"
#include "Utility.h"
#include "audio.h"
#include "RTC.h"
#include "PipView.h"
//#include "PhotoTsk.h"

static CHAR    g_RecMaxTimeStr[20] = {0};
static UINT32  g_RecMaxTime=0;
static CHAR    g_RecCurrTimeStr[20] = {0};
static UINT32  g_RecCurrTime=0;
static CHAR    date_str[20] = {0};
static CHAR    time_str[20] = {0};

static CHAR    resolution_Buf[MOVIE_SIZE_ID_MAX][10]=
{
    "1080FHD",
    "720P",
    "WVGA",
    "VGA"
};

static CHAR    resolution_dual_Buf[MOVIE_SIZE_DUAL_ID_MAX][16]=
{
    "1080P + VGA",//"1080P + 720P",
    "720P + VGA"//"720P + 720P"
};

void FlowMovie_IconDrawDscMode(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Static_cameraCtrl, TRUE);
}

void FlowMovie_IconHideDscMode(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Static_cameraCtrl, FALSE);
}

void FlowMovie_IconDrawMaxRecTime(void)
{
    memset((void *)g_RecMaxTimeStr, 0, sizeof(g_RecMaxTimeStr));
    g_RecMaxTime = UIMovRecObj_GetData(RECMOVIE_MAXSECOND);

    if (g_RecMaxTime <= 2)
    {
        FlowMovie_SetRecMaxTime(0);
    }
    else if (g_RecMaxTime > 86399)
    {   ///23:59:59
        FlowMovie_SetRecMaxTime(86399);
    }

    #if (_DUAL_CARD_FUNC_ == ENABLE)
    if ((UI_GetData(FL_CardStatus) == CARD_REMOVED) && (UI_GetData(FL_Card2Status) == CARD_REMOVED))
    #else
    if (UI_GetData(FL_CardStatus) == CARD_REMOVED)
    #endif
    {
        sprintf(g_RecMaxTimeStr,"%02d:%02d:%02d", 0, 0, 0);
    }
    else
    {
        sprintf(g_RecMaxTimeStr,"%02d:%02d:%02d", g_RecMaxTime/3600,(g_RecMaxTime%3600)/60, (g_RecMaxTime%3600)%60);
    }

    UxStatic_SetData(&UIFlowWndMovie_Static_maxtimeCtrl,STATIC_VALUE,Txt_Pointer(g_RecMaxTimeStr));
    if(PipView_GetStyle() == PIP_STYLE_2T2F)
        UxCtrl_SetShow(&UIFlowWndMovie_Static_maxtimeCtrl, FALSE);
        else
    UxCtrl_SetShow(&UIFlowWndMovie_Static_maxtimeCtrl, TRUE);
}

void FlowMovie_IconHideMaxRecTime(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Static_maxtimeCtrl, FALSE);
}

void FlowMovie_IconDrawRecTime(void)
{
    switch (gMovData.State)
    {
    case MOV_ST_REC:
    case MOV_ST_REC | MOV_ST_ZOOM:
        g_RecCurrTime = FlowMovie_GetRecCurrTime();
        memset((void *)g_RecCurrTimeStr, 0, sizeof(g_RecCurrTimeStr));
        sprintf(g_RecCurrTimeStr,"%02d:%02d:%02d", g_RecCurrTime/3600,(g_RecCurrTime%3600)/60, (g_RecCurrTime%3600)%60);
        UxStatic_SetData(&UIFlowWndMovie_Static_timeCtrl,STATIC_VALUE,Txt_Pointer(g_RecCurrTimeStr));
        if(PipView_GetStyle() == PIP_STYLE_2T2F)
            UxCtrl_SetShow(&UIFlowWndMovie_Static_timeCtrl, FALSE);
        else
        UxCtrl_SetShow(&UIFlowWndMovie_Static_timeCtrl, TRUE);
        break;
    }
}

void FlowMovie_IconDrawDateTime(void)
{
 RTC_DATE    Date;
 RTC_TIME    Time;

    // display Date/Time string in movie mode
    Date = rtc_getDate();
    Time = rtc_getTime();
    sprintf(date_str,"%04d/%02d/%02d",Date.s.year,Date.s.month,Date.s.day);
    sprintf(time_str,"%02d:%02d:%02d",Time.s.hour,Time.s.minute,Time.s.second);
    UxStatic_SetData(&UIFlowWndMovie_YMD_StaticCtrl,STATIC_VALUE,Txt_Pointer(date_str));
    UxStatic_SetData(&UIFlowWndMovie_HMS_StaticCtrl,STATIC_VALUE,Txt_Pointer(time_str));
    if(PipView_GetStyle() == PIP_STYLE_2T2F)
        {
        UxCtrl_SetShow(&UIFlowWndMovie_YMD_StaticCtrl,FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_HMS_StaticCtrl,FALSE);
        }
        else
            {
    UxCtrl_SetShow(&UIFlowWndMovie_YMD_StaticCtrl,TRUE);
    UxCtrl_SetShow(&UIFlowWndMovie_HMS_StaticCtrl,TRUE);
            }
}

void FlowMovie_IconHideDateTime(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_YMD_StaticCtrl,FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_HMS_StaticCtrl,FALSE);
}

void FlowMovie_IconDrawSize(void)
{
 VControl *pCtrl = &UIFlowWndMovie_Static_resolutionCtrl;

    if (SysGetFlag(FL_MOVIE_DUAL_REC))
    {
        UxStatic_SetData(pCtrl,STATIC_VALUE,Txt_Pointer(resolution_dual_Buf[SysGetFlag(FL_MOVIE_SIZE_DUAL)]));
    }
    else
    {
        UxStatic_SetData(pCtrl,STATIC_VALUE,Txt_Pointer(resolution_Buf[SysGetFlag(FL_MOVIE_SIZE)]));
    }
    UxCtrl_SetShow(pCtrl, FALSE);
}

void FlowMovie_IconHideSize(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Static_resolutionCtrl, FALSE);
}

void FlowMovie_IconDrawEV(void)
{
    UxState_SetData(&UIFlowWndMovie_StatusICN_EVCtrl,STATE_CURITEM,SysGetFlag(FL_EV));
    UxCtrl_SetShow(&UIFlowWndMovie_StatusICN_EVCtrl,TRUE);
}

void FlowMovie_IconHideEV(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_StatusICN_EVCtrl,FALSE);
}

void FlowMovie_IconDrawStorage(void)
{
 VControl *pCtrl=&UIFlowWndMovie_Status_StorageCtrl;

    /* Update status item data */
    #if (_DUAL_CARD_FUNC_ == ENABLE)
    if ((UI_GetData(FL_CardStatus) == CARD_REMOVED) && (UI_GetData(FL_Card2Status) == CARD_REMOVED))
    {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_INTERNAL_FLASH);
    } else if ((UI_GetData(FL_CardStatus) == CARD_LOCKED) || (UI_GetData(FL_Card2Status) == CARD_LOCKED)) {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_SD_LOCK);
    } else {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_SD_CARD);
    }
    #else
    if (UI_GetData(FL_CardStatus) == CARD_REMOVED)
    {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_INTERNAL_FLASH);
    } else if (UI_GetData(FL_CardStatus) == CARD_LOCKED) {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_SD_LOCK);
    } else {
        UxState_SetData(pCtrl,STATE_CURITEM,UIFlowWndMovie_Status_Storage_ICON_SD_CARD);
    }
    #endif

    UxCtrl_SetShow(pCtrl, TRUE);
}

void FlowMovie_IconHideStorage(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_StorageCtrl, FALSE);
}

void FlowMovie_IconDrawCyclicRec(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_CyclicRecCtrl,STATE_CURITEM,SysGetFlag(FL_MOVIE_CYCLIC_REC));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_CyclicRecCtrl, TRUE);
}

void FlowMovie_IconHideCyclicRec(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_CyclicRecCtrl, FALSE);
}

void FlowMovie_IconDrawHDR(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_HDRCtrl,STATE_CURITEM,SysGetFlag(FL_MOVIE_HDR));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_HDRCtrl, TRUE);
}

void FlowMovie_IconHideHDR(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_HDRCtrl, FALSE);
}

void FlowMovie_IconDrawMotionDet(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_MotionDetCtrl,STATE_CURITEM,SysGetFlag(FL_MOVIE_MOTION_DET));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_MotionDetCtrl, TRUE);
}

void FlowMovie_IconHideMotionDet(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_MotionDetCtrl, FALSE);
}

void FlowMovie_IconDrawBattery(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_batteryCtrl,STATE_CURITEM,GetBatteryLevel());
    UxCtrl_SetShow(&UIFlowWndMovie_Status_batteryCtrl,TRUE);
}

void FlowMovie_IconHideBattery(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_batteryCtrl,FALSE);
}

void FlowMovie_IconDrawDZoom(void)
{
 BOOL   bShow;
 VControl *pCtrl = &UIFlowWndMovie_Zoom_StaticCtrl;

    UxStatic_SetData(pCtrl,STATIC_VALUE,Txt_Pointer(Get_DZoomRatioString()));
    bShow = (DZOOM_IDX_GET() > DZOOM_IDX_MIN())? TRUE : FALSE;
    UxCtrl_SetShow(pCtrl,bShow);
}

void FlowMovie_IconHideDZoom(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Zoom_StaticCtrl,FALSE);
}

void FlowMovie_DrawPIM(BOOL bDraw)
{
    UxCtrl_SetShow(&UIFlowWndMovie_StaticIcon_PIMCCtrl, bDraw);
}

void FlowMovie_IconDrawLED(void)
{
    if(SysGetFlag(FL_LEDSET) == LED_OFF)
    UxState_SetData(&UIFlowWndMovie_Status_LEDCtrl,STATE_CURITEM,UIFlowWndMovie_Status_LED_ICON_IRLED_OFF);
    else
    UxState_SetData(&UIFlowWndMovie_Status_LEDCtrl,STATE_CURITEM,UIFlowWndMovie_Status_LED_ICON_IRLED_ON);
    UxCtrl_SetShow(&UIFlowWndMovie_Status_LEDCtrl, TRUE);
}
void FlowMovie_IconHideLED(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_LEDCtrl, FALSE);
}
void FlowMovie_IconDrawGPS(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_GPSCtrl, TRUE);
}

void FlowMovie_IconHideGPS(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_GPSCtrl, FALSE);
}
void FlowMovie_IconDrawAudioOnOff(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_AudioCtrl,STATE_CURITEM,UI_GetData(FL_MOVIE_AUDIO));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_AudioCtrl, TRUE);
}

void FlowMovie_IconHideAudioOnOff(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_AudioCtrl, FALSE);
}

void FlowMovie_IconDrawLockFile(void)
{
    debug_msg("FlowMovie_IconDrawLockFile..\r\n");
    UxCtrl_SetShow(&UIFlowWndMovie_Static_LockCtrl, TRUE);
}

void FlowMovie_IconHideLockFile(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Static_LockCtrl, FALSE);
}

void FlowMovie_IconDrawGsensor(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_GSENSORCtrl,STATE_CURITEM,UI_GetData(FL_MOVIE_GSENSOR));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_GSENSORCtrl, TRUE);
}

void FlowMovie_IconHideGsensor(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_GSENSORCtrl, FALSE);
}

void FlowMovie_IconDrawPlateNumber(void)
{

	static char CarNoBuf[16] = {0};
	if (SysGetFlag(FL_CAR_NUM) == CAR_NUM_ON) {
		if (SysGetFlag(FL_LANGUAGE) == LANG_SC) { 
			strncpy(CarNoBuf, SysGetZHCarNoStamp(), 16);
			if (CarNoBuf[0]<1 || CarNoBuf[0]>32)
			{
				CarNoBuf[0] = 2;
				strcpy(CarNoBuf+1, SysGetNZHCarNoStamp());
				SysSetZHCarNoStamp(CarNoBuf);
			}
			UxState_SetData(&UIFlowWndMovie_Status_ZHCarNoCtrl,STATE_CURITEM,CarNoBuf[0]);
			UxCtrl_SetShow(&UIFlowWndMovie_Status_ZHCarNoCtrl,TRUE);

                    UxStatic_SetData(&UIFlowWndMovie_CarNo_StaticCtrl,STATIC_VALUE,Txt_Pointer(&CarNoBuf[1]));
                    UxCtrl_SetShow(&UIFlowWndMovie_CarNo_StaticCtrl,TRUE);
		}
		else
		{
              UxStatic_SetData(&UIFlowWndMovie_CarNo_StaticCtrl,STATIC_VALUE,Txt_Pointer(SysGetNZHCarNoStamp()));
    		UxCtrl_SetShow(&UIFlowWndMovie_CarNo_StaticCtrl,TRUE);
              UxCtrl_SetShow(&UIFlowWndMovie_Status_ZHCarNoCtrl,FALSE);
		}
	}
	else
	{
        UxCtrl_SetShow(&UIFlowWndMovie_Status_ZHCarNoCtrl,FALSE);
        UxCtrl_SetShow(&UIFlowWndMovie_CarNo_StaticCtrl,FALSE);
	}
	
}


void FlowMovie_IconDrawTimeLapes(void)
{
    if(UI_GetData(FL_MOVIE_TIMELAPSE_REC)!=MOVIE_TIMELAPSEREC_OFF){
    		UxState_SetData(&UIFlowWndMovie_Status_TimeLapesCtrl,STATE_CURITEM,UIFlowWndMovie_Status_TimeLapes_ICON_TIMELAPSE_REC_STATUS);
    		UxCtrl_SetShow(&UIFlowWndMovie_Status_TimeLapesCtrl, TRUE);
    }
    else
    {
    		UxCtrl_SetShow(&UIFlowWndMovie_Status_TimeLapesCtrl, FALSE);
	}
}

void FlowMovie_IconHidePlateNumber(void)
{

    UxCtrl_SetShow(&UIFlowWndMovie_Status_ZHCarNoCtrl,FALSE);
    UxCtrl_SetShow(&UIFlowWndMovie_CarNo_StaticCtrl,FALSE);
    
}
void FlowMovie_IconDrawParkingMonitor(void)
{
    UxState_SetData(&UIFlowWndMovie_Status_ParkingMonitorCtrl,STATE_CURITEM,UI_GetData(FL_MOVIE_PARKING));
    UxCtrl_SetShow(&UIFlowWndMovie_Status_ParkingMonitorCtrl, TRUE);
}

void FlowMovie_IconHideParkingMonitor(void)
{
    UxCtrl_SetShow(&UIFlowWndMovie_Status_ParkingMonitorCtrl, FALSE);
}

void FlowMovie_UpdateIcons(BOOL bShow)
{

    if (bShow == FALSE || SysGetFlag(FL_LCD_DISPLAY) == DISPLAY_OFF||PipView_GetStyle() == PIP_STYLE_2T2F)
    {
        FlowMovie_IconHideDscMode();
        FlowMovie_IconHideSize();
        FlowMovie_IconHideMaxRecTime();
        UxState_SetData(&UIFlowWndMovie_Status_RECCtrl,STATE_CURITEM,UIFlowWndMovie_Status_REC_ICON_TRANSPAENT);
        FlowMovie_IconHideStorage();
        FlowMovie_IconHideCyclicRec();
        FlowMovie_IconHideMotionDet();
        FlowMovie_IconHideDZoom();
        FlowMovie_IconHideBattery();
        FlowMovie_IconHideEV();
        FlowMovie_IconHideDateTime();
        FlowMovie_DrawPIM(FALSE);
	    FlowMovie_IconHideLockFile();
	    FlowMovie_IconHideAudioOnOff();
	    FlowMovie_IconHideGPS();
        FlowMovie_IconHideLED();	
        FlowMovie_IconHideHDR();
        FlowMovie_IconHideGsensor();
        FlowMovie_IconHidePlateNumber();
        FlowMovie_IconHideParkingMonitor();
        UxCtrl_SetShow(&UIFlowWndMovie_Static_timeCtrl, FALSE);
    }
    else
    {
        if(PipView_GetStyle() !=  PIP_STYLE_2T2F)
        {
        FlowMovie_IconDrawDscMode();
        FlowMovie_IconDrawSize();
        FlowMovie_IconDrawMaxRecTime();
        UxState_SetData(&UIFlowWndMovie_Status_RECCtrl,STATE_CURITEM,UIFlowWndMovie_Status_REC_ICON_TRANSPAENT);
        FlowMovie_IconDrawStorage();
        FlowMovie_IconDrawCyclicRec();
        FlowMovie_IconDrawHDR();
        FlowMovie_IconDrawMotionDet();
        FlowMovie_IconDrawDZoom();
        FlowMovie_IconDrawBattery();
        FlowMovie_IconDrawEV();
        FlowMovie_IconDrawDateTime();
        FlowMovie_DrawPIM(FALSE);
        FlowMovie_IconDrawGsensor();	
		
        if(UIFlowWndMovie_IsGsensorTrig()==TRUE)
	  FlowMovie_IconDrawLockFile();			
	  else	
	  FlowMovie_IconHideLockFile();
	  
        FlowMovie_IconDrawAudioOnOff();	  
#if(GPS_FUNCTION == ENABLE)  	  
	  FlowMovie_IconDrawGPS();
#else
	  FlowMovie_IconHideGPS();
#endif		
        //FlowMovie_IconHideLED();	
        FlowMovie_IconDrawLED();
        if(SysGetFlag(FL_CAR_NUM)==CAR_NUM_ON)
            FlowMovie_IconDrawPlateNumber();
        else
            FlowMovie_IconHidePlateNumber();

        FlowMovie_IconDrawTimeLapes();

	  FlowMovie_IconDrawParkingMonitor();		
     // UxCtrl_SetShow(&UIFlowWndMovie_daocheCtrl, FALSE);
        }
    }
}

