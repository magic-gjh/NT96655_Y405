#ifndef UI_CFG_DEFAULT_H
#define UI_CFG_DEFAULT_H

#include "UIFlow.h"

// Photo
#define DEFAULT_PHOTO_CAPTURE           SELFTIMER_OFF
#define DEFAULT_PHOTO_SIZE              PHOTO_SIZE_12M//PHOTO_SIZE_5M
#define DEFAULT_PHOTO_QUALITY           QUALITY_FINE
#define DEFAULT_PHOTO_COLOR             COLOR_EFFECT_STANDARD
#define DEFAULT_SCENE                   SCENE_AUTO
#define DEFAULT_EV                      EV_00
#define DEFAULT_ISO                     ISO_AUTO
#define DEFAULT_WB                      WB_AUTO
#define DEFAULT_METERING                METERING_AIAE
#define DEFAULT_SHARPNESS               SHARPNESS_NORMAL
#define DEFAULT_SATURATION              SATURATION_NORMAL
#define DEFAULT_FD                      FD_OFF
#define DEFAULT_ANTISHAKING             ANTISHAKE_OFF
#define DEFAULT_QUICK_REVIEW            QUICK_REVIEW_0SEC
#define DEFAULT_DATE_STAMP              DATE_STAMP_DATE_TIME
#define DEFAULT_SELFTIMER               SELFTIMER_OFF
#define DEFAULT_FLASH_MODE              FLASH_OFF
#define DEFAULT_PHOTO_VIVILINK          VIVILINK_NONE
#define DEFAULT_CONTINUE_SHOT           CONTINUE_SHOT_OFF
#define DEFAULT_LOW_POWER_TIMELAPSE_REC LOW_POWER_TIMELAPSE_REC_OFF

// Movie
#define DEFAULT_MOVIE_SIZE              MOVIE_SIZE_1080P
#define DEFAULT_MOVIE_SIZE_DUAL         MOVIE_SIZE_1080P_720P
#define DEFAULT_MOVIE_QUALITY           MOVIE_QUALITY_FINE
#define DEFAULT_MOVIE_COLOR             MOVIE_COLOR_NORMAL
#define DEFAULT_MOVIE_SELFTIMER         MOVIE_SELFTIMER_OFF
#define DEFAULT_MOVIE_EV                EV_00
#define DEFAULT_MOVIE_VIVILINK          VIVILINK_NONE

#define DEFAULT_MOVIE_CYCLICREC         MOVIE_CYCLICREC_3MIN
#define DEFAULT_MOVIE_TIMELAPSEREC      MOVIE_TIMELAPSEREC_OFF
#define DEFAULT_MOVIE_MOTION_DET        MOVIE_MOTIONDET_OFF
#define DEFAULT_MOVIE_AUDIO             MOVIE_AUDIO_ON
#define DEFAULT_MOVIE_DATEIMPRINT       MOVIE_DATEIMPRINT_ON
#define DEFAULT_MOVIE_HDR               MOVIE_HDR_ON
#define DEFAULT_MOVIE_GPS               GPS_ON
#define DEFAULT_MOVIE_GSENSOR    GSENSOR_MED
#define DEFAULT_MOVIE_PARKING           MOVIE_PARKING_OFF
#define DEFAULT_AUTO_UPDATE_TIME		TIME_ZONE_GMT8//henry
#define DEFAULT_MOVIE_YESHI_DELAY		MOVIE_YESHI_DELAY_10SEC


// Playback
#define DEFAULT_PROTECT                 PROTECT_ONE
#define DEFAULT_SLIDE_SHOW              SLIDE_SHOW_2SEC

// System
#define DEFAULT_AUTO_POWER_OFF          POWER_ON
#define DEFAULT_BEEP                    BEEP_ON
#define DEFAULT_SILENT                  SILENT_OFF
#define DEFAULT_LANGUAGE                LANG_SC
#define DEFAULT_FREQUENCY               FREQUENCY_50HZ
#define DEFAULT_TV_MODE                 TV_MODE_NTSC
#define DEFAULT_SENSOR_ROTATE           SEN_ROTATE_OFF
#define DEFAULT_DATE_FORMAT             DATE_FORMAT_YMD
#define DEFAULT_OPENING_LOGO            OPENING_LOGO_ON
#define DEFAULT_LCD_DISPLAY             DISPLAY_NORMAL
#define DEFAULT_LCD_BRIGHTNESS          LCDBRT_LVL_05
#define DEFAULT_MACRO                   MACRO_OFF
#define DEFAULT_USB_MODE                USB_MODE_MSDC
#define DEFAULT_DATE_TIME               DATE_TIME_ON
#define DEFAULT_LED                     LED_OFF
#define DEFAULT_LCDOFF               LCDOFF_ON
#define DEFAULT_DELAYOFF            DELAYOFF_10SEC
//-----------------------------------------------------------------------------
// System
//-----------------------------------------------------------------------------
#define UIDFT_FS_STATUS                 FS_NOT_INIT
#define UIDFT_CARD_STATUS               CARD_REMOVED
#define UIDFT_BATTERY_LVL               0
#define UIDFT_AUDIO_VOLUME              AUDIO_VOL_7

#endif
