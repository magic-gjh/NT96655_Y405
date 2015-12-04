#ifndef _UIINFO_H_
#define _UIINFO_H_

#include "GxLib.h"
#include "PrjCfg.h"
/**
    @brief UISysInfo is the data structure
    for part 1 qulickly load from Nand

    Store in NAND Reserved Area [MAX: 512 bytes]
*/
typedef struct _UISysInfo
{
    UINT32 uhInfoSize;
    UINT32 ubWallpaperStartupIndex;
    UINT32 ubBeepVolumnIndex;
    UINT32 ubBeepPowerIndex;
    UINT32 ulmodeIndex ; //Record Mode Index in SysInfo
}
UISysInfo;

// Maximum system flag number
#define MAX_SYS_FLAG_NUM            256

//PStore
#define VER_STR_MAX     32

/**
    @brief UIMenuStoreInfo is the data structure
    for keep data in pstore;permanent storage
*/
typedef struct _UIMenuUIMenuStoreInfo
{
    //Common
    CHAR        strMakerString[VER_STR_MAX];
    CHAR        strModelString[VER_STR_MAX];
    CHAR        strSoftwareVer[VER_STR_MAX];
    CHAR        strMakeNote[VER_STR_MAX];
    CHAR        strImageDescription[VER_STR_MAX];
    CHAR        ubCarNumber[VER_STR_MAX];	
    CHAR        strZHCarNoStamp[VER_STR_MAX];	
    UINT32      uhInfoSize;
    UINT32      UIParameter[MAX_SYS_FLAG_NUM];

//#NT#2010/09/17#Jeah Yen -begin
    //system
    UINT32 ubWallpaperStartupIndex;
    UINT32 ubBeepVolumnIndex;
    UINT32 ubBeepPowerIndex;
    UINT32 ulmodeIndex ; //Record Last Mode Index
//#NT#2010/09/17#Jeah Yen -end

    //setup
    UINT32 ubBeepShutterIndex;
    UINT32 ubBeepKeyIndex;
    UINT32 ubBeepSelftimerIndex;
    UINT32 ubSlideShowEffectIndex;
    UINT32 ubSlideShowIntervalIndex;
    UINT32 ubSlideShowRepeatIndex;
    //#NT#2011/01/12#Steven feng -begin
    #if ( SLIDESHOW_MUSIC_OPTION ==   ENABLE )
    UINT32 ubSlideShowMusicIndex;
    //#NT#2011/02/23#Steven feng -begin
    UINT32 ubSlideShowWAVStatus;
    //#NT#2011/02/23#Steven feng -end
    #endif
    //#NT#2011/01/12#Steven feng -end
    //#NT#2010/12/13#Ben Wang -begin
    //Add playback i key setting
    UINT32 ubPlayIKeyModeIndex;
    //#NT#2010/12/13#Ben Wang -end
    UINT32 ubPlayEffectIndex;
    UINT32 ubLanguageIndex;
    UINT32 ubWallpaperMenuIndex;
    UINT32 ubWallpaperBackIndex;
    UINT32 ubLcdOffIndex;
    UINT32 ubDelayOffIndex;
    UINT32 ubPowerOffIndex;
    UINT32 ubBrightAdjIndex;
    UINT32 ubBrightIndex;
    UINT32 ubDateFormat;
    UINT32 ubTimeFormat;
    UINT32 ubTVFormat;
    UINT32 ubDirNumber;
    UINT32 ubFileNumber;
    UINT32 ubUSBMode;
    UINT32 ubResetAll;
    UINT32 ubTimeLocaleIndex;
    UINT32 ubHomeZoneIndex;
    UINT32 ubTravalZoneIndex;

    //photo menu
    UINT32 ubAFWindowIndex;
    UINT32 ubAFBeamIndex;
    UINT32 ubContAF;
    UINT32 ubAEmode;
    UINT32 ubQviewIndex;
    UINT32 ubDzoom;
    UINT32 ubBlinkDet;
    UINT32 ubSlowShutter;
    UINT32 ubContshotIndex;
    UINT32 ubTimelapseIndex;
    UINT32 ubDatePrint;
    UINT32 ubPanModeIndex;

    // photo Qfunc
    UINT32 ubMacroIndex;
    UINT32 ubSelftimerIndex;
    UINT32 ubFlashIndex;
    UINT32 ubEVIndex;
    UINT32 ubSizeIndex;
    UINT32 ubQualityIndex;
    UINT32 ubWBIndex;
    UINT32 ubMwbRGB;
    UINT32 ubColorIndex;
    UINT32 ubISOIndex;
    UINT32 ubSCNIndex;

    // movie
    UINT32 ubAVIVolumnIndex;
    UINT32 ubMovieContAF;
    UINT32 ubMovieMetering;
    UINT32 ubMovieDis;
    UINT32 ubMovieMCTF;
    //#NT#2010/12/16#Photon Lin -begin
    //#Enhance DIS flow
    UINT32 ubMovieDisEnable;
    //#NT#2010/12/16#Photon Lin -end
    UINT32 ubMovieGdc;
    UINT32 ubMovieSmear;
    UINT32 ubMovieSize;
    UINT32 ubMovieQuality;
    UINT32 ubMovieGSensorIndex;
    UINT32 ubMovieGPSIndex;

    UINT32 ubTakePictCnt;
//#NT#2010/09/17#Jeah Yen -begin
/*
    //#NT# record mode
    UINT32 ulmodeIndex ;
*/
//#NT#2010/09/17#Jeah Yen -end
    //#NT#Store FD index after power off
    UINT32 ubFDIndex ;
    UINT32 ubPhotoFrameIndex ;
    //#NT#2011/02/18#Brad Chen -begin
    //#NT# add autoRunISO
    #if(_MSDC_MODE_==_MSDC_ALL_NAND2_|| _MSDC_MODE_ == _MSDC_SINGLE_NAND2_)
    UINT32 ubAutoRunISO;
    #endif
    //#NT#2011/02/18#Brad Chen -end
    UINT32 ubPhotoDispMode;

}UIMenuStoreInfo;



typedef enum
{
    // << system setting flags region (should be stored in PStore and read back while system initial) >>
    FL_NULL,

    // Photo
    FL_PHOTO_CAPTURE,           // Photo capture
    FL_PHOTO_SIZE,              // Photo size
    FL_QUALITY,                 // Photo quality
    FL_COLOR_EFFECT,            // Photo color
    FL_SCENE,                   // Scene mode
    FL_EV,                      // Exposure value
    FL_PHOTO_HDR,               // Photo HDR
    FL_ISO,                     // ISO value
    FL_WB,                      // White balance mode
    FL_METERING,                // Exposure metering
    FL_SHARPNESS,               // Image sharpness
    FL_SATURATION,              // Image saturation
    FL_FACE,                    // Face detection
    FL_SMILE,                   // Smile detection
    FL_FD,                      // FD/SD detection
    FL_ANTISHAKE,               // Anti-shaking on/off
    FL_EXIT,                    // EXIT
    FL_QUICK_REVIEW,            // Quick review time
    FL_DATE_STAMP,              // Date-time stamp OFF/DATE/DATE TIME
    FL_SELFTIMER,               // Self-timer time
    FL_FLASH_MODE,              // Flash light mode
    FL_PHOTO_VIVILINK,          // Photo mode Vivilink setting
    FL_CONTINUE_SHOT,           // Continue shot on/off
    FL_QUICKVIEW,

    // Movie
    FL_MOVIE_SIZE,              // Movie size
    FL_MOVIE_SIZE_DUAL,         // Movie size for dual recording
    FL_MOVIE_DUAL_REC,          // Movie dual recording
    FL_MOVIE_EV,                // Movie EV
    FL_MOVIE_HDR,               // Movie HDR
    FL_MOVIE_FRAMERATE,         // Movie frame rate  15fps, 30fps
    FL_MOVIE_QUALITY,           // Movie quality
    FL_MOVIE_COLOR,             // Movie color
    FL_MOVIE_SELFTIMER,         // Movie mode self-timer time
    FL_MOVIE_VIVILINK,          // Movie mode Vivilink setting
    FL_MOVIE_CYCLIC_REC,        // Movie cyclic recording
    FL_MOVIE_TIMELAPSE_REC,     // Movie cyclic recording
    FL_MOVIE_MOTION_DET,        // Movie motion detect
    FL_MOVIE_AUDIO,             // Movie audio enable
    FL_MOVIE_DATEIMPRINT,       // Movie dateimprint
    FL_MOVIE_USBRETURN,         // Movie usb return to movie mode
    FL_MOVIE_GSENSOR,           // Movie GSensor
    FL_MOVIE_GPS,               // Movie GPS
    FL_MOVIE_PARKING,
    FL_MOVIE_YESHI_DELAY,
    FL_AUTO_UPDATE_TIME,
    // Playback
    FL_DELETE_PHOTO,            // Delete Still
    FL_DELETE_MOVIE,            // Delete Video
    FL_PROTECT,                 // File protect operation (pseudo flag)
    FL_SLIDE_SHOW,              // Slide show time
    FL_MAKE_MOVIE,              // Make movie option
    FL_PLAY_STILL_VIVILINK,     // Playback mode Vivilink value (depends on file content)
    FL_PLAY_VIDEO_VIVILINK,     // Playback mode Vivilink value (depends on file content)
    FL_CROP,
    FL_ROTATE,
    FL_PHOTO_PROTECT,
    FL_MOVIE_PROTECT,
    FL_PICTBRIDGE,

    // System
    FL_AUTO_POWER_OFF,          // Auto power off time
    FL_CAR_NUM,
    FL_DATE_TIME,
    FL_BEEP,                    // Beep sound on/off
    FL_SILENT,                  // definition is opposite with FL_BEEP,
    FL_LANGUAGE,                // Language
    FL_FREQUENCY,               // Frequency (50Hz/60Hz)
    FL_LCD_BACKLIGHT,           // Lcd backlight
    FL_TV_MODE,                 // TV mode (NTSC/PAL)
    FL_LEDSET,                     // Led
    FL_LCDOFF,
    FL_DELAYOFF,
    FL_HDMI_MODE,               // HDMI mode
    FL_FORMAT,                  // Format storage
    FL_DEFAULT,                 // Reset Default
    FL_DATE_FORMAT,             // Date format (DD/MM/YY, MM/DD/YY, YY/MM/DD)
    FL_TIME_FORMAT,             // Time format (AM/PM)
    FL_HOUR_FORMAT,             // Hour format (12 Hours/24 Hours)
    FL_DATE_TIME_SETUP,
    FL_OPENING_LOGO,            // Opening logo display (on/off)
    FL_SENSOR_ROTATE,           // Sensor Roate on/off
    FL_SETUP,                   // Setup menu

    //Init Setup flag after reset default
    FL_INIT_SETUP,

    FL_DCF_DIR_ID,              // DCF directory ID
    FL_DCF_FILE_ID,             // DCF file ID
    FL_LCD_DISPLAY,             // LCD display status (OSD normal, full, off, LCD power down)
    FL_MACRO,                   // Macro mode on/off
    FL_LCD_BRIGHTNESS,          // LCD brightness level
    FL_USB_MODE,                // USB mode (MSDC, PCC, PictBridge, ...)


    //Common
    FL_ModeIndex,
    FL_PreMode,
    FL_NextMode,
    FL_ModeTransParam,
    FL_ModeTransDir,

    // Photo
    FL_SceneModeIndex,
    FL_ASCNIndex,
    // QFunc
    FL_MacroIndex,
    FL_SelftimerIndex,
    FL_FlashIndex,
    FL_EVIndex,
    FL_SizeIndex,
    FL_QualityIndex,
    FL_WBIndex,
    FL_MwbRGB,
    FL_ColorIndex,
    FL_ISOIndex,
    // Photo menu
    FL_IsInitSettingFlag,
    FL_AFWindowIndex,
    FL_ContAFIndex,
    FL_AFBeamIndex,
    FL_MeteringIndex,
    FL_CapModeIndex,
    FL_DatePrintIndex,
    FL_PreviewIndex,
    FL_Dzoom,
    FL_DzoomReset,
    FL_ZoomIFIndex,
    FL_ZoomIsDigital,
    FL_ZoomOzmDzmChange,
    FL_BShootIndex,
    FL_BlinkDetIndex,
    FL_SmileDetIndex,//for menu setting
    FL_PanModeIndex,
    FL_DzoomIndex,
    FL_FDIndex,
    FL_ObjTrackIndex,
    FL_ContShotStateIndex,
    FL_ContShotIndex,
    FL_IsSingleCapture,

    //Play
    FL_SlideIndex,
    FL_VMIndex,
    FL_DPOFPrtIndex,
    FL_DPOFDateOnIndex,

    //Movie
    FL_MovieSizeIndex,
    FL_MovieQualityIndex,
    FL_MovieColorIndex,
    FL_MovieSelftimer,
    FL_MovieEVIndex,
    FL_MovieWBIndex,
    FL_MovieMeteringIndex,
    FL_MovieContAFIndex,
    FL_MovieDisIndex,
    FL_MovieMCTFIndex,
    FL_MovieRSCIndex,
    //#NT#2010/12/16#Photon Lin -begin
    //#Enhance DIS flow
    FL_MovieDisEnableIndex,
    //#NT#2010/12/16#Photon Lin -end
    FL_MovieGdcIndex,
    FL_MovieSmearIndex,
    FL_MovieAudioPlayIndex,
    FL_MovieAudioRecIndex,
    //#NT#2010/11/25#Photon Lin -begin
    //#Add feature of movie key
    FL_MovieAutoRecIndex,
    FL_MovieAutoRecLastMode,
    FL_Movie_GPSIndex,
    FL_Movie_GSensorIndex,
    //#NT#2010/11/25#Photon Lin -end
    //UI_MovieHotKeyRec,
    //Setup
    FL_LanguageIndex,
    FL_PowerOffIndex,
    FL_LCDOffIndex,
    FL_BrightAdjIndex,
    FL_BrightLvlIndex,
    FL_TimeLocaleIndex,
    FL_HomeZoneIndex,
    FL_TravalZoneIndex,
    FL_DateFormatIndex,
    FL_TimeFormatIndex,
    FL_USBModeIndex,
    FL_DualDisp,
    FL_DisplayDevIndex,
    FL_TVFormatIndex,
    FL_DisplayModeIndex,
    FL_LedIndex,
//#NT#2010/11/29#Ben Wang -begin
//#NT#add i key playback mode
    FL_PlayIKeyModeIndex,
//#NT#2010/11/29#Ben Wang -end
//#NT#2010/12/10#Ben Wang -begin
//#NT#add for _IKEY_DATEVIEW_MODE_
    FL_LastPlayDate,
//#NT#2010/12/10#Ben Wang -end
    FL_PhotoDispModeIndex,
    FL_TimelapseIndex,
    FL_BeepVolumnIndex,
    FL_BeepShutterIndex,
    FL_BeepKeyIndex,
    FL_BeepSelftimerIndex,
    FL_BeepPowerIndex,
    FL_SlideShowEffectIndex,
    FL_SlideShowIntervalIndex,
    FL_SlideShowRepeatIndex,
    //#NT#2011/01/12#Steven feng -begin
#if ( SLIDESHOW_MUSIC_OPTION ==   ENABLE )
    FL_SlideShowMusicIndex,
#endif
    //#NT#2011/01/12#Steven feng -end
    FL_WallpaperMenuIndex,
    FL_WallpaperStartupIndex,
    FL_WallpaperBackIndex,
    FL_PlayEffectIndex,

    //Media Status
    FL_FileNumbering,
    FL_FilesysDirId,
    FL_FilesysFileId,
    FL_DualCardMain,
    FL_CardStatus,
    FL_Card2Status,
    FL_CardChange,
    FL_FSStatus,

    //Info Status
    FL_ResetAllStatus,
    FL_RTCPowerFlag,
    FL_LensOpen,
    FL_CalSDinsert,
    FL_ShowADC,
    FL_IsPlaybackInSCN,
    FL_TakePictCnt,
    FL_IsDCIMReadOnly,
    //#NT#For menu reset flag
    FL_ResetFlag,
    FL_PhotoFrameIndex,
    FL_PhotoFrameWriteSts,
//#NT#2011/02/14#Brad Chen -begin
//#NT#  AutoRunISO
    FL_AutoRunISOWriteSts,
//#NT#2011/02/14#Brad Chen -end
    //#NT#2011/02/16#Steven feng -begin
    FL_SSWAVWriteSts,
    //#NT#2011/02/16#Steven feng -end
    //#NT#2011/03/16#Lincy Lin -begin
    //#NT#
    FL_IsCIPARunning,
    FL_IsEmptyBatt,
    //#NT#2011/03/16#Lincy Lin -end
    FL_IslensStopping,
    FL_IsInMuseum,
    FL_IsStopCharge,
    FL_IsCopyToCarding,
    FL_IsUseFileDB,
    FL_NetWorkMode,
    FL_WIFI_LINK_STA,
    FL_WIFI_MOVIE_FMT,
    FL_LOW_POWER_TIMELAPSE_REC,
    FL_TIMELAPSE_REC_MERGEYUV,
    FL_IndexInfoMAX
}UI_IndexInfo;


typedef enum
{
    CLEAR_RESULT,
    EXE_OK,
    COPY_NUM_FULL
}EXE_RESULT;


//UI_ResetAllStatus,
typedef enum
{
    RESET_ALL_OFF = 0,
    RESET_ALL_ON,
    RESET_ALL_SETTING_MAX
}RESET_ALL_SETTING;


//UI_CardStatus,
typedef enum
{
    CARD_REMOVED = 0,
    CARD_INSERTED,
    CARD_LOCKED
} CARD_STATUS;

//-----------------------------------------------------------------------------
// Main card for dual card
//-----------------------------------------------------------------------------
typedef enum
{
    MAIN_CARD_NONE = 0,
    MAIN_CARD_1,
    MAIN_CARD_2
} MAIN_CARD;

//-----------------------------------------------------------------------------
// card change for dual card
//-----------------------------------------------------------------------------
typedef enum
{
    CARD_CHANGE_NO = 0,
    CARD_CHANGE_YES
} CARD_CHANGE;

//UI_FSStatus,
typedef enum
{
    FS_DISK_ERROR = 0,
    FS_UNKNOWN_FORMAT,
    FS_UNFORMATTED,
    FS_NOT_INIT,
    FS_INIT_OK,
    FS_NUM_FULL
} FS_STATUS;

//-----------------------------------------------------------------------------
// Battery Status
//-----------------------------------------------------------------------------
typedef enum
{
    BATTERY_FULL = 0,
    BATTERY_MED,
    BATTERY_LOW,
    BATTERY_EMPTY,
    BATTERY_EXHAUSTED,
    BATTERY_CHARGE,
    BATTERY_STATUS_TOTAL_NUM
} BATTERY_STATUS;


typedef enum
{
    BEEP_ATTR_VOLUME = 0,
    BEEP_ATTR_SHUTTER,
    BEEP_ATTR_KEY,
    BEEP_ATTR_SELFTIMER,
    BEEP_ATTR_POWER,
    BEEP_ATTR_SETTING_MAX
}BEEP_ATTR_SETTING;


//UI_BeepVolumnIndex,
typedef enum
{
    BEEP_VOL_MUTE = 0,
    BEEP_VOL_1,
    BEEP_VOL_2,
    BEEP_VOL_3,
    BEEP_VOL_SETTING_MAX
}BEEP_VOL_SETTING;

//UI_BeepShutterIndex,
//UI_BeepKeyIndex,
//UI_BeepSelftimerIndex,
//UI_BeepPowerIndex,
typedef enum
{
    BEEP_TONE_1 = 0,
    BEEP_TONE_2,
    BEEP_TONE_3,
    BEEP_TONE_4,
    BEEP_TONE_SETTING_MAX
}BEEP_TONE_SETTING;

typedef enum
{
    SLIDESHOW_ATTR_EFFECT = 0,
    SLIDESHOW_ATTR_INTERVAL,
    SLIDESHOW_ATTR_REPEAT,
    SLIDESHOW_ATTR_SETTING_MAX
}SLIDESHOW_ATTR_SETTING;


//UI_LCDOffIndex
typedef enum
{
    LCDOFF_ON = 0,
    LCDOFF_30SEC,
    LCDOFF_1MIN,
    LCDOFF_2MIN,
    LCDOFF_SETTING_MAX
}LCDOFF_SETTING;

//UI_WallpaperMenuIndex,
typedef enum
{
    WALLPAPER_MENU_USER = 0,
    WALLPAPER_MENU_NATURE,
    WALLPAPER_MENU_SPARKLING,
    WALLPAPER_MENU_SETTING_MAX
}WALLPAPER_MENU_SETTING;

//#NT#2011/02/18#Steven feng -begin
//UI_SlideshowMusicIndex
typedef enum
{
    SLIDESHOW_TYPE1 = 0,
    SLIDESHOW_TYPE2,
    SLIDESHOW_TYPE3,
    SLIDESHOW_TYPE4,
    SLIDESHOW_TYPE_SETTING_MAX
}SLIEDSHOW_TYPE_MENU_SETTING;
//#NT#2011/02/18#Steven feng -end

//UI_WallpaperStartupIndex,
typedef enum
{
    WALLPAPER_STARTUP_USER = 0,
    WALLPAPER_STARTUP_GE,
    WALLPAPER_STARTUP_SETTING_MAX
}WALLPAPER_STARTUP_SETTING;

//UI_WallpaperBackIndex,
typedef enum
{
    WALLPAPER_BACK_YES = 0,
    WALLPAPER_BACK_NO,
    WALLPAPER_BACK_SETTING_MAX
}WALLPAPER_BACK_SETTING;

//UI_PlayEffectIndex,
typedef enum
{
    PLAYEFFECT_NORAML = 0,
    PLAYEFFECT_VIVID,
    PLAYEFFECT_SETTING_MAX
}PLAYEFFECT_SETTING;

//UI_BrightAdjIndex,
typedef enum
{
    LCDBRT_ADJ_AUTO = 0,
    LCDBRT_ADJ_MANUAL,
    LCDBRT_ADJ_SETTING_MAX
}LCDBRT_ADJ_SETTING;

//UI_DateFormatIndex,
typedef enum
{
    DATE_YMD = 0,
    DATE_DMY,
    DATE_MDY,
    DATE_SETTING_MAX
}DATE_SETTING;

//UI_TimeFormatIndex,
typedef enum
{
    TIME_24HR = 0,
    TIME_12HR,
    TIME_SETTING_MAX
}TIME_SETTING;

//FL_TV_MODE,
typedef enum
{
    VIDEOOUT_NTSC = 0,
    VIDEOOUT_PAL,
    VIDEOOUT_SETTING_MAX
}VIDEOOUT_SETTING;

//FL_HDMI_MODE,
typedef enum
{
    VIDEOOUT2_FHD = 0,
    VIDEOOUT2_HD,
    VIDEOOUT2_SETTING_MAX
}VIDEOOUT2_SETTING;

//#NT#2010/11/29#Ben Wang -begin
//#NT#add i key playback mode
//UI_PlayIKeyModeIndex,
typedef enum
{
    PLAY_BY_NAME = 0,
    PLAY_BY_DATE,
    PLAY_BY_XX_MAX
}PLAY_BY_SETTING;
//#NT#2010/11/29#Ben Wang -end
typedef enum
{
    FILE_NUMBER_MODE_CONT = 0,
    FILE_NUMBER_MODE_RESET,
    FILE_NUMBER_MODE_SETTING_MAX
}FILE_NUMBER_MODE_SETTING;


//#NT#2012/8/10#Philex - begin
// add menu flag enumeration
//-----------------------------------------------------------------------------
// Photo Size
//-----------------------------------------------------------------------------
enum _PHOTO_SIZE
{
    PHOTO_SIZE_12M,
    PHOTO_SIZE_10M,
    PHOTO_SIZE_8M,
    PHOTO_SIZE_5M,
    PHOTO_SIZE_3M,
    PHOTO_SIZE_2MHD,
    PHOTO_SIZE_VGA,
    PHOTO_SIZE_1M,
    PHOTO_SIZE_ID_MAX,

    PHOTO_SIZE_15M,
    PHOTO_SIZE_14M,
    PHOTO_SIZE_7M,
    PHOTO_SIZE_7MHD,
    PHOTO_SIZE_4M,
    PHOTO_SIZE_2M,
};

//-----------------------------------------------------------------------------
// Photo Quality
//-----------------------------------------------------------------------------
enum _QUALITY
{
    QUALITY_SUPER,
    QUALITY_FINE,
    QUALITY_NORMAL,
    QUALITY_ID_MAX
};

//-----------------------------------------------------------------------------
// Photo Color
//-----------------------------------------------------------------------------
enum _PHOTO_COLOR
{
    COLOR_EFFECT_STANDARD,
    COLOR_EFFECT_MONOCHROME,
    COLOR_EFFECT_SEPIA,
    COLOR_EFFECT_ID_MAX,
    COLOR_EFFECT_VIVID
};

//-----------------------------------------------------------------------------
// Scene Mode
//-----------------------------------------------------------------------------
enum _SCENE_MODE
{
    SCENE_AUTO,
    SCENE_SCENERY,
    SCENE_PORTRAIT,
    SCENE_NIGHTSCENERY,
    SCENE_NIGHTPORTRAIT,
    SCENE_HIGHSENSITIVITY,
    SCENE_BEACH,
    SCENE_ID_MAX,
    SCENE_SPORT,
    SCENE_PARTY
};

//-----------------------------------------------------------------------------
// Exposure Value
//-----------------------------------------------------------------------------
enum _EXPOSURE
{
    EV_P20,
    EV_P16,
    EV_P13,
    EV_P10,
    EV_P06,
    EV_P03,
    EV_00,
    EV_N03,
    EV_N06,
    EV_N10,
    EV_N13,
    EV_N16,
    EV_N20,
    EV_SETTING_MAX,

    EV_P30,
    EV_P15,
    EV_N15,
    EV_N30
};

//-----------------------------------------------------------------------------
// Photo HDR State
//-----------------------------------------------------------------------------
enum _PHOTO_HDR
{
    PHOTO_HDR_OFF,
    PHOTO_HDR_ON,
    PHOTO_HDR_ID_MAX
};

//-----------------------------------------------------------------------------
// ISO Value
//-----------------------------------------------------------------------------
enum _ISO
{
    ISO_AUTO,
    ISO_100,
    ISO_200,
    ISO_400,
    ISO_ID_MAX,
    ISO_800,
    ISO_1600,
    ISO_3200
} ;

//-----------------------------------------------------------------------------
// White Balance Mode
//-----------------------------------------------------------------------------
enum _WB_MODE
{
    WB_AUTO,
    WB_DAYLIGHT,
    WB_CLOUDY,
    WB_TUNGSTEN,
    WB_FLUORESCENT,
    WB_ID_MAX,
    // The following are for compiling issue
    WB_FLUORESCENT1,
    WB_FLUORESCENT2,
    WB_INCAND,
    WB_MANUAL
};

//-----------------------------------------------------------------------------
// Exposure Metering Method
//-----------------------------------------------------------------------------
enum _METERING
{
    METERING_CENTER,
    METERING_MULTI,
    METERING_SPOT,
    METERING_AIAE,
    METERING_ID_MAX,
};

//-----------------------------------------------------------------------------
// Sharpness Level
//-----------------------------------------------------------------------------
enum _SHARPNESS
{
    SHARPNESS_SHARP,
    SHARPNESS_NORMAL,
    SHARPNESS_SOFT,
    SHARPNESS_ID_MAX
};

//-----------------------------------------------------------------------------
// Saturation Level
//-----------------------------------------------------------------------------
enum _SATURATION
{
    SATURATION_HIGH,
    SATURATION_NORMAL,
    SATURATION_LOW,
    SATURATION_ID_MAX
};

//-----------------------------------------------------------------------------
// Face Detection State
//-----------------------------------------------------------------------------
enum _FACE_DETECTION
{
    FD_OFF,
    FD_ON,
    FD_SMILE,
    FD_ID_MAX
};

//-----------------------------------------------------------------------------
// AntiShaking State
//-----------------------------------------------------------------------------
enum _ANTISHAKING
{
    ANTISHAKE_OFF,
    ANTISHAKE_ON,
    ANTISHAKE_ID_MAX
};

//-----------------------------------------------------------------------------
// Quick Review Time
//-----------------------------------------------------------------------------
enum _QUICK_REVIEW
{
    QUICK_REVIEW_0SEC,
    QUICK_REVIEW_2SEC,
    QUICK_REVIEW_5SEC,
    QUICK_REVIEW_ID_MAX
};

//-----------------------------------------------------------------------------
// Date Imprint Mode
//-----------------------------------------------------------------------------
enum _DATE_STAMP
{
    DATE_STAMP_OFF,
    DATE_STAMP_DATE,
    DATE_STAMP_DATE_TIME,
    DATE_STAMP_ID_MAX
};

//-----------------------------------------------------------------------------
// Photo Selftimer Time
//-----------------------------------------------------------------------------
enum _SELFTIMER
{
    SELFTIMER_OFF,
    SELFTIMER_2SEC,
    SELFTIMER_5SEC,
    SELFTIMER_10SEC,
    SELFTIMER_ID_MAX
};

//-----------------------------------------------------------------------------
// Flash Light Mode
//-----------------------------------------------------------------------------
enum _FLASH_MODE
{
    FLASH_AUTO,
    FLASH_ON,
    FLASH_RED_EYE,
    FLASH_OFF,
    FLASH_ID_MAX,
    // The following are for compiling issue
    FLASH_SLOW_FLASH_EYE,
    FLASH_SLOW_FLASH
};

//-----------------------------------------------------------------------------
// Vivilink
//-----------------------------------------------------------------------------
// Photo mode Vivilink items
enum _PHOTO_VIVILINK
{
    PHOTO_VIVILINK_FACEBOOK,
    PHOTO_VIVILINK_TWITGOO,
    PHOTO_VIVILINK_TWITPIC,
    PHOTO_VIVILINK_PHOTOBUCKET,
    PHOTO_VIVILINK_FLICKR,
    PHOTO_VIVILINK_MYSPACE,
    PHOTO_VIVILINK_PICASA_ALBUMS,
    PHOTO_VIVILINK_NONE,
    PHOTO_VIVILINK_ID_MAX
};

// Movie mode Vivilink items
enum _MOVIE_VIVILINK
{
    MOVIE_VIVILINK_YOUTUBE,
    MOVIE_VIVILINK_PHOTOBUCKET,
    MOVIE_VIVILINK_FLICKR,
    MOVIE_VIVILINK_NONE,
    MOVIE_VIVILINK_ID_MAX
};

// Playback mode Vivilink items for still
enum _PLAY_STILL_VIVILINK
{
    PLAY_STILL_VIVILINK_FACEBOOK,
    PLAY_STILL_VIVILINK_TWITGOO,
    PLAY_STILL_VIVILINK_TWITPIC,
    PLAY_STILL_VIVILINK_PHOTOBUCKET,
    PLAY_STILL_VIVILINK_FLICKR,
    PLAY_STILL_VIVILINK_MYSPACE,
    PLAY_STILL_VIVILINK_PICASA_ALBUMS,
    PLAY_STILL_VIVILINK_NONE,
    PLAY_STILL_VIVILINK_ID_MAX
};

// Playback mode Vivilink items for video
enum _PLAY_VIDEO_VIVILINK
{
    PLAY_VIDEO_VIVILINK_YOUTUBE,
    PLAY_VIDEO_VIVILINK_PHOTOBUCKET,
    PLAY_VIDEO_VIVILINK_FLICKR,
    PLAY_VIDEO_VIVILINK_NONE,
    PLAY_VIDEO_VIVILINK_ID_MAX
};

// real Vivilink value
enum _VIVILINK
{
    VIVILINK_NONE,
    VIVILINK_FACEBOOK,
    VIVILINK_TWITGOO,
    VIVILINK_TWITPIC,
    VIVILINK_PHOTOBUCKET,
    VIVILINK_FLICKR,
    VIVILINK_MYSPACE,
    VIVILINK_PICASA_ALBUMS,
    VIVILINK_YOUTUBE,
    VIVILINK_ID_MAX
};

// Vivilink setting operation
enum _VIVILINK_SETTING
{
    VIVILINK_SET_ONE,
    VIVILINK_SET_ALL,
    VIVILINK_SET_ID_MAX
};

//-----------------------------------------------------------------------------
// Continue shot State
//-----------------------------------------------------------------------------
enum
{
    CONTINUE_SHOT_OFF,
    CONTINUE_SHOT_ON,
    CONTINUE_SHOT_SETTING_MAX
};

//-----------------------------------------------------------------------------
// Movie Size
//-----------------------------------------------------------------------------
enum _MOVIE_SIZE
{
    MOVIE_SIZE_1080P,   // 1920 x 1080
    MOVIE_SIZE_720P,    // 1280 x 720
    MOVIE_SIZE_WVGA,    // 854 x 480
    MOVIE_SIZE_VGA,     // 640 x 480
    MOVIE_SIZE_ID_MAX,
    MOVIE_SIZE_QVGA,    // 320 x 240
    MOVIE_SIZE_1080P_FULLRES    // 1920 x 1080 with full resolution sensor input (for compiling issue)
};

enum _MOVIE_SIZE_DUAL
{
    MOVIE_SIZE_1080P_720P,  // 1920 x 1080 + 1280 x 720
    MOVIE_SIZE_720P_720P,   // 1280 x 720 + 1280 x 720
    MOVIE_SIZE_DUAL_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie Quality
//-----------------------------------------------------------------------------
enum _MOVIE_QUALITY
{
    MOVIE_QUALITY_FINE,
    MOVIE_QUALITY_STANDARD,
    MOVIE_QUALITY_ECONOMY,
    MOVIE_QUALITY_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie Color
//-----------------------------------------------------------------------------
enum _MOVIE_COLOR
{
    MOVIE_COLOR_NORMAL,
    MOVIE_COLOR_MONO,
    MOVIE_COLOR_SEPIA,
    MOVIE_COLOR_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie Selftimer
//-----------------------------------------------------------------------------
enum _MOVIE_SELFTIMER
{
    MOVIE_SELFTIMER_OFF,
    MOVIE_SELFTIMER_ON_2S,
    MOVIE_SELFTIMER_ON_10S,
    MOVIE_SELFTIMER_ON_10_2S,
    MOVIE_SELFTIMER_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie cyclic record
//-----------------------------------------------------------------------------
enum _MOVIE_CYCLICREC
{
    MOVIE_CYCLICREC_OFF,
    MOVIE_CYCLICREC_3MIN,
    MOVIE_CYCLICREC_5MIN,
    MOVIE_CYCLICREC_10MIN,
    MOVIE_CYCLICREC_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie time lapse record
//-----------------------------------------------------------------------------
enum _MOVIE_TIMELAPSEREC
{
    MOVIE_TIMELAPSEREC_OFF,
    MOVIE_TIMELAPSEREC_100MS,
    MOVIE_TIMELAPSEREC_200MS,
    MOVIE_TIMELAPSEREC_500MS,
    MOVIE_TIMELAPSEREC_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie HDR State
//-----------------------------------------------------------------------------
enum
{
    MOVIE_HDR_OFF = 0,
    MOVIE_HDR_ON,
    MOVIE_HDR_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie Motion Detect State
//-----------------------------------------------------------------------------
enum
{
    MOVIE_MOTIONDET_OFF = 0,
    MOVIE_MOTIONDET_ON,
    MOVIE_MOTIONDET_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie Audio enable State
//-----------------------------------------------------------------------------
enum
{
    MOVIE_AUDIO_OFF = 0,
    MOVIE_AUDIO_ON,
    MOVIE_AUDIO_ID_MAX
};

//-----------------------------------------------------------------------------
// Movie DateImprint State
//-----------------------------------------------------------------------------
enum
{
    MOVIE_DATEIMPRINT_OFF = 0,
    MOVIE_DATEIMPRINT_ON,
    MOVIE_DATEIMPRINT_ID_MAX
};

//-----------------------------------------------------------------------------
// Date Format
//-----------------------------------------------------------------------------
enum _DATE_FORMAT
{
    DATE_FORMAT_YMD,
    DATE_FORMAT_MDY,
    DATE_FORMAT_DMY,
    DATE_FORMAT_ID_MAX
};

//-----------------------------------------------------------------------------
// File Protect
//-----------------------------------------------------------------------------
enum _FILE_PROTECT
{
    PROTECT_ONE = 0,
    UNPROTECT_ONE,
    PROTECT_ALL,
    UNPROTECT_ALL,
    FILE_PROTECT_ID_MAX
};

//-----------------------------------------------------------------------------
// Slide Show Time
//-----------------------------------------------------------------------------
enum _SLIDE_SHOW
{
    SLIDE_SHOW_2SEC,
    SLIDE_SHOW_5SEC,
    SLIDE_SHOW_8SEC,
    SLIDE_SHOW_ID_MAX
};

//-----------------------------------------------------------------------------
// Make Movie
//-----------------------------------------------------------------------------
enum _MAKE_MOVIE
{
    MAKE_MOVIE_NO,
    MAKE_MOVIE_YES,
    MAKE_MOVIE_ID_MAX
};

//-----------------------------------------------------------------------------
// Auto Power Off Time
//-----------------------------------------------------------------------------
//FL_AUTO_POWER_OFF
typedef enum
{
    POWER_ON = 0,
    POWER_3MIN,
    POWER_5MIN,
    POWER_10MIN,
    POWEROFF_SETTING_MAX
}POWEROFF_SETTING;

//-----------------------------------------------------------------------------
// Car number
//-----------------------------------------------------------------------------
enum _CAR_NUM
{
    CAR_NUM_OFF,
    CAR_NUM_ON,
    CAR_NUM_ID_MAX
};
//-----------------------------------------------------------------------------
// Beep Sound
//-----------------------------------------------------------------------------
enum _BEEP
{
    BEEP_OFF,
    BEEP_ON,
    BEEP_ID_MAX
};

//-----------------------------------------------------------------------------
// Silent Mode
//-----------------------------------------------------------------------------
enum _SILENT
{
    SILENT_ON,
    SILENT_OFF,
    SILENT_ID_MAX
};

//-----------------------------------------------------------------------------
// Language Setting
//-----------------------------------------------------------------------------
enum _LANGUAGE
{
    LANG_EN,
    LANG_FR,
    LANG_ES,
    LANG_PO,
    LANG_DE,
    LANG_IT,
    LANG_SC,
    LANG_TC,
    LANG_RU,
    LANG_JP,
    LANG_ID_MAX,

    LANG_DU,
    LANG_TU,
    LANG_PB,
    LANG_YI,
    LANG_PL,
    LANG_DE_JAY
};


enum _MOVIE_YESHI_DELAY
{
    MOVIE_YESHI_DELAY_OFF,
    MOVIE_YESHI_DELAY_5SEC,
    MOVIE_YESHI_DELAY_10SEC,
    MOVIE_YESHI_DELAY_15SEC,
    MOVIE_YESHI_DELAY_20SEC,  
    MOVIE_YESHI_DELAY_ID_MAX
};
//-----------------------------------------------------------------------------
// Power Frequency
//-----------------------------------------------------------------------------
enum _FREQUENCY
{
    FREQUENCY_50HZ,
    FREQUENCY_60HZ,
    FREQUENCY_ID_MAX
};

//-----------------------------------------------------------------------------
// GSENSOR
//-----------------------------------------------------------------------------
enum _GSENSOR
{
	GSENSOR_OFF=0,
	GSENSOR_LOW,
	GSENSOR_MED,
	GSENSOR_HIGH,
	GSENSOR_ID_MAX,
	GSENSOR_ON
};

//-----------------------------------------------------------------------------
// TV Mode
//-----------------------------------------------------------------------------
enum _TV_MODE
{
    TV_MODE_NTSC,
    TV_MODE_PAL,
    TV_MODE_ID_MAX
};

//-----------------------------------------------------------------------------
// Sensor rotate Mode
//-----------------------------------------------------------------------------
enum _SENSOR_ROTATE_MODE
{
    SEN_ROTATE_OFF,
    SEN_ROTATE_ON,
    SEN_ROTATE_MAX
};

//-----------------------------------------------------------------------------
// Opening Logo Display
//-----------------------------------------------------------------------------
enum _OPENING_LOGO
{
    OPENING_LOGO_OFF,
    OPENING_LOGO_ON,
    OPENING_LOGO_ID_MAX
};

//-----------------------------------------------------------------------------
// LCD Display Status
//-----------------------------------------------------------------------------
enum _LCD_DISPLAY
{
    DISPLAY_NORMAL,
    DISPLAY_ALL,
    DISPLAY_OFF,
    DISPLAY_ID_MAX,
    DISPLAY_OFFPANEL
};

//-----------------------------------------------------------------------------
// LCD Brightness Adjust Level
//-----------------------------------------------------------------------------
enum _LCD_BRIGHTNESS_LEVEL
{
    LCDBRT_LVL_01 = 1,
    LCDBRT_LVL_02,
    LCDBRT_LVL_03,
    LCDBRT_LVL_04,
    LCDBRT_LVL_05,
    LCDBRT_LVL_06,
    LCDBRT_LVL_07,
    LCDBRT_LVL_08,
    LCDBRT_LVL_09,
    LCDBRT_LVL_ID_MAX
};

//-----------------------------------------------------------------------------
// Macro State
//-----------------------------------------------------------------------------
enum _MACRO
{
    MACRO_OFF,
    MACRO_ON,
    MACRO_ID_MAX
};

//-----------------------------------------------------------------------------
// USB Mode
//-----------------------------------------------------------------------------
enum _USB_MODE
{
    USB_MODE_MSDC,
    USB_MODE_PICTBRIDGE,
    USB_MODE_PCC,
    USB_MODE_PTP,
    USB_MODE_YOUTUBE,
    USB_MODE_ID_MAX,
    USB_MODE_CHARGE // for future use
};

//-----------------------------------------------------------------------------
// Date Time Setting
//-----------------------------------------------------------------------------
enum _DATE_TIME
{
    DATE_TIME_ON ,
    DATE_TIME_OFF,
    DATE_TIME_SETUP,
    DATE_TIME_ID_MAX
};
//#NT#2012/8/10#Philex - end

//-----------------------------------------------------------------------------
// HDMI/TV plug
//-----------------------------------------------------------------------------
typedef enum
{
    PLUG_OFF,
    PLUG_TV,
    PLUG_HDMI,
    PLUG_MAX,
    ENUM_DUMMY4WORD(PLUG_DEV)
} PLUG_DEV;


//-----------------------------------------------------------------------------
// Wi-Fi link OK/NG
//-----------------------------------------------------------------------------
enum _WIFI_LINK
{
    WIFI_LINK_NG,
    WIFI_LINK_OK,
    WIFI_LINK_LINKING,
    WIFI_LINK_ID_MAX
};

enum _WIFI_RTSP
{
    WIFI_RTSP_LIVEVIEW,
    WIFI_RTSP_REC,
    WIFI_RTSP_MAX
};

//-----------------------------------------------------------------------------
// POWER SAVE TIME LAPSE REC
//-----------------------------------------------------------------------------
//FL_LOW_POWER_TIMELAPSE_REC
typedef enum
{
    LOW_POWER_TIMELAPSE_REC_OFF = 0,
    LOW_POWER_TIMELAPSE_REC_5SEC,
    LOW_POWER_TIMELAPSE_REC_10SEC,
    LOW_POWER_TIMELAPSE_REC_1MIN,
    LOW_POWER_TIMELAPSE_REC_5MIN,
    LOW_POWER_TIMELAPSE_REC_10MIN,
    LOW_POWER_TIMELAPSE_REC_30MIN,
    LOW_POWER_TIMELAPSE_REC_60MIN,
    LOW_POWER_TIMELAPSE_REC_SETTING_MAX
}LOW_POWER_TIMELAPSE_REC_SETTING;

typedef enum
{
    TIMELAPSE_REC_MERGEYUV_END = 0,
    TIMELAPSE_REC_MERGEYUV_START,
    TIMELAPSE_REC_MERGEYUV_START_SETTING_MAX
}TIMELAPSE_REC_MERGEYUV_SETTING;


//-----------------------------------------------------------------------------
// Led
//-----------------------------------------------------------------------------
enum _LED
{
    LED_OFF,
    LED_ON,		
    LED_ID_MAX
};
extern CHAR previewEVStr[EV_SETTING_MAX][5];

//-----------------------------------------------------------------------------
// Get/Set Photo Mode User Defined Items
//-----------------------------------------------------------------------------
typedef enum
{
    PHOTO_USR_FLASH         = 0,
    PHOTO_USR_EV,
    PHOTO_USR_AFWINDOW,
    PHOTO_USR_AFBEAM,
    PHOTO_USR_METERING,
    PHOTO_USR_TIMELAPSE,
    PHOTO_USR_CONTSHOT,
    PHOTO_USR_CONTAF,
    PHOTO_USR_QREVIEW,
    PHOTO_USR_DZOOMSTATUS,
    PHOTO_USR_SLOWSHUTTER,
    PHOTO_USR_DATEIMPRINT,
    PHOTO_USR_BLINK,
    PHOTO_USR_PANDET,
    PHOTO_USR_COLOR,
    PHOTO_USR_SIZE,
    PHOTO_USR_QUALITY,
    PHOTO_USR_ISO,
    PHOTO_USR_WB,
    PHOTO_USR_SATURATION,
    PHOTO_USR_SHARPNESS,
    PHOTO_USR_CONTRAST,
    PHOTO_USR_FD,
    PHOTO_USR_DISPMODE
}PHOTO_USR_ITEM_T;
//-----------------------------------------------------------------------------
enum _DELAYOFF
{
    DELAYOFF_OFF,
    DELAYOFF_10SEC,
    DELAYOFF_20SEC,
    DELAYOFF_30SEC,    
    DELAYOFF_ID_MAX
};
// GPS
//-----------------------------------------------------------------------------
enum _GPS
{
    GPS_OFF,
    GPS_ON,
    GPS_ID_MAX
};


enum _AUTO_UPDATE_TIME
{
	AUTO_UPDATE_TIME_OFF,
	//AUTO_UPDATE_TIME_ON,
	TIME_ZONE_GMTM12,
	TIME_ZONE_GMTM11,
	TIME_ZONE_GMTM10,
	TIME_ZONE_GMTM9,
	TIME_ZONE_GMTM8,
	TIME_ZONE_GMTM7,
	TIME_ZONE_GMTM6,
	TIME_ZONE_GMTM5,
	TIME_ZONE_GMTM430,
	TIME_ZONE_GMTM4,
	TIME_ZONE_GMTM330,
	TIME_ZONE_GMTM3,
	TIME_ZONE_GMTM2,
	TIME_ZONE_GMTM1,
	TIME_ZONE_GMT0,
	TIME_ZONE_GMT1,
	TIME_ZONE_GMT2,
	TIME_ZONE_GMT3,
	TIME_ZONE_GMT330,
	TIME_ZONE_GMT4,
	TIME_ZONE_GMT430,
	TIME_ZONE_GMT5,
	TIME_ZONE_GMT530,
	TIME_ZONE_GMT545,	
	TIME_ZONE_GMT6,
	TIME_ZONE_GMT630,
	TIME_ZONE_GMT7,
	TIME_ZONE_GMT8,	
	TIME_ZONE_GMT9,	
	TIME_ZONE_GMT930,
	TIME_ZONE_GMT10,
	TIME_ZONE_GMT11,
	TIME_ZONE_GMT12,
	TIME_ZONE_GMT13,		
	AUTO_UPDATE_TIME_ID_MAX
};

//-----------------------------------------------------------------------------
// MOVIE MONITOR
//-----------------------------------------------------------------------------
enum _MOVIE_MONITOR
{
    MOVIE_PARKING_OFF,
    MOVIE_PARKING_ON,
    MOVIE_PARKING_MAX
};

extern void     Load_SysInfo(void);
extern void     Save_SysInfo(void);
extern void     Init_SysInfo(void);

//UI Setting Info
extern void     Load_MenuInfo(void); //Load from PST
extern void     Save_MenuInfo(void); //Save to PST
extern void     Init_MenuInfo(void);
extern void     Reset_MenuInfo(void); //reset

//#NT#2012/10/17#Philex Lin-begin
// add this API to check ACIn and naming is backward for NT96220
extern BOOL KeyScan_IsACIn(void);
// add Auto Poweroff/USB detect/Enable Key sound scan items
extern void KeyScan_EnableMisc(BOOL bEn);
// add get battery level function
extern UINT32   GetBatteryLevel(void);
//#NT#2012/10/17#Philex Lin-end

extern void     SysSetFlag(UINT32 uiFlag, UINT32 uiValue);
extern UINT32   SysGetFlag(UINT32 uiFlag);

extern void     SysCheckFlag(void);
extern void     SysSetFixedFlag(void);
extern void     SysResetFlag(void);
extern void     SysExeMenuSettingFuncs(void);

extern UINT32   UI_GetData(UINT32 attribute);
extern void     UI_SetData(UINT32 attribute,UINT32 value);
extern void     Photo_SetUserIndex(PHOTO_USR_ITEM_T uiItem, UINT32 uhIndex);
extern UINT32   Photo_GetUserIndex(PHOTO_USR_ITEM_T uiItem);

extern PLUG_DEV KeyScan_GetPlugDev(void);

extern void FlowDCF_UpdateName(void);

extern void UI_SetInitVolume(UINT32 volumeIndex);
extern void UI_SaveRTCDate(void);
extern void UI_SetFileDBFileID(void);
#endif //_UIINFO_H_
