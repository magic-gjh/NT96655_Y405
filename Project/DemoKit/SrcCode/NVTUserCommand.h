#ifndef NVT_USER_COMMAND_H
#define NVT_USER_COMMAND_H

#include "UIFramework.h"

//typedef UINT32 NVTEVT;

// KEY event class

typedef enum
{
    NVTEVT_KEY_EVT_START    = DEV_KEY_EVENT_BASE,
    NVTEVT_KEY_STATUS_START,    // Status key start
        NVTEVT_KEY_STATUS1,
        NVTEVT_KEY_STATUS2,
        NVTEVT_KEY_STATUS3,
        NVTEVT_KEY_STATUS4,
        NVTEVT_KEY_STATUS5,
        NVTEVT_KEY_STATUS6,
        NVTEVT_KEY_STATUS7,
        NVTEVT_KEY_STATUS8,
        NVTEVT_KEY_STATUS9,
        NVTEVT_KEY_STATUS10,
    NVTEVT_KEY_STATUS_END,    // Status key end
    NVTEVT_KEY_BUTTON_START,    // Button key start
        NVTEVT_KEY_PRESS_START,    // Press key start
            NVTEVT_KEY_PRESS,
            NVTEVT_KEY_POWER,
            NVTEVT_KEY_UP,
            NVTEVT_KEY_DOWN,
            NVTEVT_KEY_LEFT,
            NVTEVT_KEY_RIGHT,
            NVTEVT_KEY_ENTER,
            NVTEVT_KEY_TELE,
            NVTEVT_KEY_WIDE,
            NVTEVT_KEY_DEL,
            NVTEVT_KEY_ZOOMIN,
            NVTEVT_KEY_ZOOMOUT,
            NVTEVT_KEY_SHUTTER1,
            NVTEVT_KEY_SHUTTER2,
            NVTEVT_KEY_FACEDETECT,
            NVTEVT_KEY_MODE,
            NVTEVT_KEY_MOVIE,
            NVTEVT_KEY_I,
            NVTEVT_KEY_PLAYBACK,
            NVTEVT_KEY_MENU,
            NVTEVT_KEY_DISPLAY,
            NVTEVT_KEY_CUSTOM1,
            NVTEVT_KEY_CUSTOM2,
        NVTEVT_KEY_PRESS_END,       // Press key end
        NVTEVT_KEY_CONTINUE_START,    // Continue key start
            NVTEVT_KEY_CONTINUE,
            NVTEVT_KEY_POWER_CONT,
            NVTEVT_KEY_UP_CONT,
            NVTEVT_KEY_DOWN_CONT,
            NVTEVT_KEY_LEFT_CONT,
            NVTEVT_KEY_RIGHT_CONT,
        NVTEVT_KEY_CONTINUE_END,    // Continue key end
        NVTEVT_KEY_RELEASE_START,   // Release key start
            NVTEVT_KEY_RELEASE,
            NVTEVT_KEY_POWER_REL,
            NVTEVT_KEY_UP_REL,
            NVTEVT_KEY_DOWN_REL,
            NVTEVT_KEY_LEFT_REL,
            NVTEVT_KEY_RIGHT_REL,
            NVTEVT_KEY_SHUTTER1_REL,
            NVTEVT_KEY_SHUTTER2_REL,
            NVTEVT_KEY_ZOOMRELEASE,
        NVTEVT_KEY_RELEASE_END,    // Release key end
    NVTEVT_KEY_BUTTON_END,    // Button key end
    NVTEVT_KEY_EVT_END
}
NVT_KEY_EVENT;

// DEVICE event class

typedef enum
{
    NVTEVT_USERCMD_START    = USER_EVENT_MASK,
    NVTEVT_TIMER            = NVTEVT_USERCMD_START ,
    NVTEVT_BATTERY,
    //#NT#2013/01/29#Philex Lin -begin
    NVTEVT_BATTERY_LOW,
    //#NT#2013/01/29#Philex Lin -end
    //NVTEVT_CALLBACK,
    NVTEVT_STORAGE_CHANGE,
    NVTEVT_STORAGE_INIT,
    NVTEVT_MACRO_CHANGE,
    NVTEVT_PRINT_RESP,
    NVTEVT_MODE_CHANGE,
    NVTEVT_DSCMODE_INIT,
    NVTEVT_ANI_STOPED,
    NVTEVT_CB_POWEROFF,
    NVTEVT_PLAY_EFFECT,
    NVTEVT_TV_CHANGE,
    //#NT#2013/01/28#Philex Lin -begin
    NVTEVT_AC_Plug,
    NVTEVT_AC_UnPlug,
    //#NT#2013/01/28#Philex Lin -end
    //#NT#2010/07/13#Lincy Lin -begin
    //#NT#Add update info
    NVTEVT_UPDATE_INFO,            ///< Update window info command
    //#NT#2010/07/13#Lincy Lin -end
    NVTEVT_GSENSOR_TRIG,
    
    NVTEVT_USERCMD_END
}
NVT_USER_CMD;

typedef enum
{
    NVTEVT_ANIMATE_TIMER  = NVTEVT_USERCMD_END + 20,
    NVTEVT_PLAYINFO_TIMER,
    NVTEVT_TIMER_MODE,
    NVTEVT_SELFTIMER,
    NVTEVT_SELFTIMER_TONE,
    NVTEVT_SELFTIMER_OFFSHOWN,
    NVTEVT_FLASHTIMER,
    NVTEVT_PWROFFTIMER,
    NVTEVT_UPDATETIMETIMER,
    NVTEVT_UPDATEHISTTIMER,
    NVTEVT_DIGITAL_ZOOM_TIMER,
    NVTEVT_FOCUS_TIMER,
    NVTEVT_DISPTIMER,
    NVTEVT_QUICKVIEWTIMER,
    NVTEVT_BLINKTIMER,
    NVTEVT_01SEC_TIMER,
    NVTEVT_05SEC_TIMER,
    NVTEVT_1SEC_TIMER,
    NVTEVT_5SEC_TIMER,
    NVTEVT_FLASH_ALARM_TIMER,
    NVTEVT_FUNNEL_TIMER,
    NVTEVT_SCENE_TIMER,
    NVTEVT_PF_TIMER,
    NVTEVT_AUTOROTATE_TIMER,
    NVTEVT_AUTOCAPTURETIMER,
    NVTEVT_PANINIT_TIMER,
    NVTEVT_AFFAILTIMER,
    NVTEVT_PANCAPTURETIMER,
    NVTEVT_TEST_BUTTON_OK,
    NVTEVT_GREEN_SELFTIMER_TIMER,
    NVTEVT_GREEN_FLOWER_TIMER,
    NVTEVT_GREEN_LOGO_TIMER,
    NVTEVT_GREEN_ICON_TIMER,
    NVTEVT_ZOOM_DISP_TIMER,
    NVTEVT_FW_UPDATE_TIMER,
    NVTEVT_HISTOGRAM_TIMER,
    NVTEVT_OBJTRACK_TIMER,
    NVTEVT_MARQUEE_TIMER,
    //#NT#2012/8/1#Philex - begin
    NVTEVT_CB_QVSTART,
    NVTEVT_CB_JPGOK,
    NVTEVT_CB_CAPFSTOK,
    NVTEVT_CB_FOCUSEND,
    NVTEVT_CB_FDEND,
    NVTEVT_CB_SDEND,
    NVTEVT_EXE_QUICKREVIEW,
    //#NT#2012/8/1#Philex - end
    /*
    NVTEVT_ALGMSG_FLASH,
    NVTEVT_ALGMSG_QVSTART,
    NVTEVT_ALGMSG_JPGOK,
    NVTEVT_ALGMSG_RESUMEKEY,
    NVTEVT_ALGMSG_CAPFSTOK,
    NVTEVT_ALGMSG_CAPTUREEND,
    NVTEVT_ALGMSG_SLOWSHUTTER,
    NVTEVT_ALGMSG_MWBCALEND,
    NVTEVT_ALGMSG_PDEND,
    NVTEVT_ALGMSG_PROGRESSEND,
    */
    NVTEVT_WARNING_TIMER,
    NVTEVT_USER_EVT_END
}
NVT_USER_EVT;

typedef enum
{
    NVTRET_CLOSE_PARENT_WND = NVTEVT_USER_EVT_END+1, ///< Close parent window
    NVTRET_CLOSE_PARENT_WITH_PARAM,                  ///< Close parent window with parameters
    NVTRET_CANCEL,
    NVTRET_CONFIRM,
    NVTRET_SET,
    NVTRET_UNSET,
    NVTRET_SET_NUM,
    NVTRET_SINGLEOBJ,
    NVTRET_PLAYBACKKEY,
    NVTRET_DELETE,
    NVTRET_COPY2CARD,
    NVTRET_SLIDE_SHOW,
    NVTRET_PROTECT,
    NVTRET_THUMBNAIL,
    //#NT#2012/7/10#philex - begin
    NVTRET_MAGNIFY,
    //#NT#2012/7/10#philex - end
    NVTRET_DELETEALL,
    NVTRET_ENTER_MENU,
    NVTRET_FORMAT,
    NVTRET_WAITMOMENT,
    NVTRET_WAIT_SCREEN,
    NVTRET_WARNING,
    NVTRET_CARD_INSERT,
    NVTRET_PROCESSING,
    NVTRET_CLOSE_SESSION,
    NVTRET_UPDATE_SCENE_MODE,
    NVTRET_MODE_WIN,
    NVTRET_MENU,
    NVTRET_SCENE_SEL,
    NVTRET_REDRAW,
    NVTRET_MODE_SEL,
    NVTRET_NULL_RET
}
NVT_WND_RETURN_VALUE;

typedef enum
{
    NVTFILE_INFO = NVTRET_NULL_RET+1,
    NVTFILE_DIR_INFO
}NVT_FILE_EVT;

#endif
