#ifndef _WIFIAPPCMD_H_
#define _WIFIAPPCMD_H_

#ifdef __ECOS
extern int UIAP_getData(char* path, char* argument, cyg_uint32 bufAddr, cyg_uint32* bufSize, char* mimeType, cyg_uint32 segmentCount);
#endif


/**
    @name command send to wifi for wifiCmd_send().
*/
//@{
#define WIFI_STATUS                      "AT+get status\n"
#define WIFI_SSID                        "AT+get SSID\n"
#define WIFI_PWD                         "AT+get PASSPHRASE\n"
#define WIFI_VERSION                     "AT+get version\n"
#define WIFI_SIZE                        "AT+get size\n"
#define WIFI_SECURITY                    "AT+ref security\n"
#define WIFI_CHMODE_MJPG                 "AT+set mode MJPEG\n"
#define WIFI_CHMODE_H264                 "AT+set mode H.264\n"
#define WIFI_CHSIZE169                   "AT+set size 640x360,30\n"
#define WIFI_CHSIZE43                    "AT+set size 640x480,30\n"
#define WIFI_UPDATEFW                    "AT+updateFW\n"
#define WIFI_USB_MODE                    "AT+set usbMode %d\n"
#define WIFI_RESET_TO_DEFAULT            "AT+set default\n"
#define WIFI_REBOOT                      "AT+reboot\n"
#define WIFI_MACADDR                     "AT+get mac\n"
#define WIFI_CONTROL                     "AT+set WiFi %d\n"
//@}

/**
    @name EXT_CMD_ITEM command from APP.
*/
//@{
#define WIFIAPP_CMD_CAPTURE              1001
#define WIFIAPP_CMD_CAPTURESIZE          1002
#define WIFIAPP_CMD_CAPTUREWB            1003
#define WIFIAPP_CMD_CAPTUREFOV           1004
#define WIFIAPP_CMD_BURSTMODE            1005

#define WIFIAPP_CMD_RECORD               2001
#define WIFIAPP_CMD_MOVIESIZE            2002
#define WIFIAPP_CMD_MOVIEWB              2003
#define WIFIAPP_CMD_MOVIEFOV             2004

#define WIFIAPP_CMD_MODECHANGE           3001
#define WIFIAPP_CMD_DATE                 3002
#define WIFIAPP_CMD_QUERY                3003
#define WIFIAPP_RET_HEARTBEAT            3004
#define WIFIAPP_CMD_GPS                  3005
#define WIFIAPP_CMD_ACK                  3006
//@}

/**
    @name command send to app for wifiCmd_send().
*/
//@{
#define WIFIAPP_NOTIFY_STATUS            "dv status=%d\n"
#define WIFIAPP_NOTIFY_FREEPICNUM        "dv freePicNum=%d\n"
#define WIFIAPP_NOTIFY_MAXRECTIME        "dv maxRecTime=%d\n"
#define WIFIAPP_NOTIFY_BATTERY           "dv battery=%d\n"
#define WIFIAPP_NOTIFY_MOVFOV            "dv movieFOV=%d\n"
#define WIFIAPP_NOTIFY_MOVFOV_MASK       "dv movieFOVMask=%d\n"
#define WIFIAPP_NOTIFY_CAPFOV            "dv capFOV=%d\n"
#define WIFIAPP_NOTIFY_CAPFOV_MASK       "dv capFOVMask=%d\n"
#define WIFIAPP_NOTIFY_CAPCOUNT          "dv capCnt=%d\n"
#define WIFIAPP_ACK                      "dv ack\n"
#define WIFIAPP_GET_HEARTBEAT            "dv heartBeat\n"
#define WIFIAPP_REPLY_MENUSETTING        "dv capSize=%d,capBurst=%d,capWB=%d,capFOV=%d,capFOVM=%d,movSize=%d,movWB=%d,movFOV=%d,movFOVM=%d,freePic=%d,freeTime=%d,mode=%d,bat=%d\n"


//@}


/**
     wifi callback event
*/
typedef enum _WIFI_CBMSG
{
    WIFICB_GET_STATUS = 0,          ///< get status.
    WIFICB_GET_SSID,                ///< get SSID.
    WIFICB_GET_PASSPHRASE,          ///< get PASSPHRASE.
    WIFICB_GET_VERSION,             ///< get version.
    WIFICB_GET_SIZE,                ///< get size.
    WIFICB_CAP,                     ///< capture photo (APP)
    WIFICB_RECORD,                  ///< movie record (APP)
    WIFICB_SET_CAPSIZE,             ///< capture size (APP)
    WIFICB_SET_CAPBURST,            ///< burst mode (APP)
    WIFICB_SET_CAPWB,               ///< capture WB (APP)
    WIFICB_SET_CAPFOV,              ///< capture FOV (APP)
    WIFICB_SET_MOVIESIZE,           ///< movie size (APP)
    WIFICB_SET_MOVIEWB,             ///< movie WB (APP)
    WIFICB_SET_MOVIEFOV,            ///< movie FOV (APP)
    WIFICB_SET_MODE,                ///< set mode (APP)
    WIFICB_SET_DATE,                ///< set date (APP)
    WIFICB_QUERY_SETTING,           ///< query menu setting (APP)
    WIFICB_RET_HB,                  ///< return heart beat (APP)
    WIFICB_SET_GPS,                 ///< set GPS location (APP)
    WIFICB_ACK_STS,                 ///< ack of 'dv status'(APP)
    WIFICB_ATCMD_FINISH,            ///< wi-fi module "AT+set size" cmd finish
    WIFICB_ATCMD_SET_SIZE_FINISH,   ///< wi-fi module "AT+set size" cmd finish
    WIFICB_ATCMD_REFRESH_FINISH,    ///< wi-fi module "AT+ref security" cmd finish
    WIFICB_ATCMD_USBMODE_FINISH,    ///< wi-fi module "AT+set usbMode" cmd finish
    WIFICB_GET_MACADDR,             ///< get mac address
    WIFICB_ATCMD_WIFI_CTRL_REPLY,   ///< set wifi control
    ENUM_DUMMY4WORD(WIFI_CBMSG)
} WIFI_CBMSG;

/**
     Wifi & APP Command Send definition
*/
typedef enum _WIFI_CMDSEND
{
    WIFI_CMDSEND_GETSTATUS = 0,      ///< send command to get wifi status.
    WIFI_CMDSEND_REFRESH,            ///< send command to refresh wifi SSID & PASSPHRASE.
    WIFI_CMDSEND_GETSSID,            ///< send command to get wifi SSID.
    WIFI_CMDSEND_GETPASSPHRASE,      ///< send command to get wifi PASSPHRASE.
    WIFI_CMDSEND_GETVERSION,         ///< send command to get wifi version.
    WIFI_CMDSEND_SETUVCSIZE,         ///< send command to set UVC size (resolution and fps).
    WIFI_CMDSEND_GETUVCSIZE,         ///< send command to get UVC size (resolution and fps).
    WIFI_CMDSEND_UPDATEFW,           ///< send command to update wifi FW.
    WIFI_CMDSEND_USB_MODE,           ///< send command to wifi change USB mode.
    WIFI_CMDSEND_RESET2DEFAULT,      ///< send command to reset wifi parameter to default.
    WIFI_CMDSEND_REBOOT,             ///< send command to reboot wifi module.
    WIFI_CMDSEND_STREAMING_FORMAT,   ///< send Command to set streaming format
    WIFI_CMDSEND_NOTIFY_STATUS,      ///< send command to notify DV status (to APP).
    WIFI_CMDSEND_NOTIFY_FREPICNUM,   ///< send command to notify free picture number (to APP).
    WIFI_CMDSEND_NOTIFY_BATTERY,     ///< send command to notify battery level (to APP).
    WIFI_CMDSEND_NOTIFY_MAXRECTIME,  ///< send command to notify maximum record time (to APP).
    WIFI_CMDSEND_NOTIFY_MOVIEFOV,    ///< send command to movie FOV value (to APP).
    WIFI_CMDSEND_NOTIFY_MOVIEFOV_MASK,///< send command to movie FOV mask (to APP).
    WIFI_CMDSEND_NOTIFY_CAPCOUNT,    ///< send command to notify photo capture count (to APP).
    WIFI_CMDSEND_NOTIFY_PHOTOFOV,    ///< send command to photo FOV value (to APP).
    WIFI_CMDSEND_NOTIFY_PHOTOFOV_MASK,///< send command to photo FOV mask (to APP).
    WIFI_CMDSEND_NOTIFY_RESULT,      ///< send command to result (to APP)
    WIFI_CMDSEND_GETHEARTBEAT,       ///< send command to get heart beat (to APP).
    WIFI_CMDSEND_QUERY_INFO,         ///< send query information (to APP).
    WIFI_CMDSEND_GET_MACADDR,        ///< send command to get mac addresss
    WIFI_CMDSEND_WIFI_CTRL,          ///< send command to control wifi
    ENUM_DUMMY4WORD(WIFI_CMDSEND)
} WIFI_CMDSEND;

/**
    UVC size of wifi module
    command WIFI_CMDSEND_SETUVCSIZE
*/
typedef enum
{
    WIFI_MOD_CHSIZE43 = 0,
    WIFI_MOD_CHSIZE169,
    ENUM_DUMMY4WORD(WIFI_MOD_UVC_SIZE)
}WIFI_MOD_UVC_SIZE;


/**
    wifi module return status
    command WIFI_STATUS
*/
typedef enum
{
    WIFI_MOD_READY = 0,
    WIFI_MOD_CONNECTING,
    WIFI_MOD_CONNECTED,
    WIFI_MOD_STREAMING,
    WIFI_MOD_IDLE,
    WIFI_MOD_SLEEP,
    WIFI_MOD_DISCONNECT,
    ENUM_DUMMY4WORD(WIFI_MOD_RET_STATUS)
}WIFI_MOD_RET_STATUS;


/**
    DV return status
    command WIFI_CMDSEND_NOTIFY_STATUS
*/
typedef enum
{
    WIFI_DV_STA_ILLREQ = -1,
    WIFI_DV_STA_OK = 0,
    WIFI_DV_STA_LOWBATTERY,
    WIFI_DV_STA_CARDFULL,
    WIFI_DV_STA_NUMFULL,
    WIFI_DV_STA_UNFORMAT,
    WIFI_DV_STA_UNKNOWNFMT,
    WIFI_DV_STA_CARDERR,
    WIFI_DV_STA_SHUTDOWN,
    WIFI_DV_STA_STRGCHANGED,
    WIFI_DV_STA_RECORDING,
    WIFI_DV_STA_BUSY,
    WIFI_DV_STA_SLOWCARD,
    WIFI_DV_STA_HIGHTEMP,
    WIFI_DV_STA_STOPREC,
    WIFI_DV_STA_PWRSAVE,
    ENUM_DUMMY4WORD(WIFI_DV_RET_STATUS)
}WIFI_DV_RET_STATUS;


/**
    app capture size command
    command WIFIAPP_CMD_CAPTURESIZE
*/
typedef enum
{
    WIFI_APP_CAPTURE_SIZE_14MP = 0,
    WIFI_APP_CAPTURE_SIZE_10MP,
    WIFI_APP_CAPTURE_SIZE_5MP,
    ENUM_DUMMY4WORD(WIFI_APP_CAPTURE_SIZE_CMD)
}WIFI_APP_CAPTURE_SIZE_CMD;

/**
    app capture burst command
    command WIFIAPP_CMD_BURSTMODE
*/
typedef enum
{
    WIFI_APP_CAPTURE_BURST_OFF = 0,
    WIFI_APP_CAPTURE_BURST_ON,
    ENUM_DUMMY4WORD(WIFI_APP_CAPTURE_BURST_CMD)
}WIFI_APP_CAPTURE_BURST_CMD;

/**
    app movie record command
    command WIFIAPP_CMD_RECORD
*/
typedef enum
{
    WIFI_APP_MOVIE_STOP = 0,
    WIFI_APP_MOVIE_START,
    ENUM_DUMMY4WORD(WIFI_APP_MOVIE_CMD)
}WIFI_APP_MOVIE_CMD;

/**
    app movie size command
    command WIFIAPP_CMD_MOVIESIZE
*/
typedef enum
{
    WIFI_APP_MOVIE_SIZE_1080P30 = 0,
    WIFI_APP_MOVIE_SIZE_960P50,
    WIFI_APP_MOVIE_SIZE_960P30,
    WIFI_APP_MOVIE_SIZE_720P60,
    WIFI_APP_MOVIE_SIZE_720P30,
    WIFI_APP_MOVIE_SIZE_WVGAP60,
    WIFI_APP_MOVIE_SIZE_WVGAP120,
    ENUM_DUMMY4WORD(WIFI_APP_MOVIE_SIZE_CMD)
}WIFI_APP_MOVIE_SIZE_CMD;

/**
    app movie FOV command
    command WIFIAPP_CMD_MOVIEFOV
*/
typedef enum
{
    WIFI_APP_FOV_ULTRAWIDE = 0,
    WIFI_APP_FOV_MEDIUM,
    WIFI_APP_FOV_NARROW,
    WIFI_APP_FOV_CROP,
    ENUM_DUMMY4WORD(WIFI_APP_MOVIE_FOV_CMD)
}WIFI_APP_MOVIE_FOV_CMD;

/**
     app mode change command
     command WIFIAPP_CMD_MODECHANGE
*/
typedef enum
{
    WIFI_APP_MODE_PHOTO = 0,
    WIFI_APP_MODE_MOVIE,
    WIFI_APP_MODE_PLAYBACK,
    ENUM_DUMMY4WORD(WIFI_APP_MODE_CMD)
} WIFI_APP_MODE_CMD;

/**
    app WB setting command
    command WIFIAPP_CMD_CAPTUREWB/WIFIAPP_CMD_MOVIEWB
*/
typedef enum
{
    WIFI_APP_WB_AUTO = 0,
    WIFI_APP_WB_DAYLIGHT,
    WIFI_APP_WB_CLOUDY,
    WIFI_APP_WB_FLUORESCENT,
    WIFI_APP_WB_UNDERWATER,
    ENUM_DUMMY4WORD(WIFI_APP_WB_CMD)
}WIFI_APP_WB_CMD;

/**
    app return heart beat
    command WIFIAPP_RET_HEARTBEAT
*/
typedef enum
{
    WIFI_APP_DISCONNECT = 0,
    WIFI_APP_ALIVE,
    ENUM_DUMMY4WORD(WIFI_APP_HEARTBEAT)
} WIFI_APP_HEARTBEAT;


#define SysCmdWifi_CmdSend(a,b)


#endif //_WIFIAPPCMD_H_



