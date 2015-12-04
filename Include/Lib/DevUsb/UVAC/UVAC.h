/**
    UVAC module

    USB Video and Audio device class module.

    @file       UVAC.h
    @ingroup    mIUVAC

    Copyright   Novatek Microelectronics Corp. 2013.  All rights reserved.

*/

#ifndef _UVAC_H
#define _UVAC_H

/*-----------------------------------
    Includes
-----------------------------------*/
#include "Type.h"

/**
    @addtogroup mIUVAC
*/
//@{

//Define fields of [Class spec Frame ] descriptor
#define UVC_VSFRM_INDEX1                    0x01        // frame index
#define UVC_VSFRM_INDEX1_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX2                    0x02        // frame index
#define UVC_VSFRM_INDEX2_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX3                    0x03        // frame index
#define UVC_VSFRM_INDEX3_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX4                    0x04        // frame index
#define UVC_VSFRM_INDEX4_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX5                    0x05        // frame index
#define UVC_VSFRM_INDEX5_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX6                    0x06        // frame index
#define UVC_VSFRM_INDEX6_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX7                    0x07        // frame index
#define UVC_VSFRM_INDEX7_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX8                    0x08        // frame index
#define UVC_VSFRM_INDEX8_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX9                    0x09        // frame index
#define UVC_VSFRM_INDEX9_CAP                0x03        // D0: still image  D1:Fix frame rate
#define UVC_VSFRM_INDEX10                   0x0A        // frame index
#define UVC_VSFRM_INDEX11                   0x0B        // frame index
#define UVC_VSFRM_INDEX12                   0x0C        // frame index

#define UVAC_OUT_SIZE_1280_720      UVC_VSFRM_INDEX1
#define UVAC_OUT_SIZE_1024_768      UVC_VSFRM_INDEX2
#define UVAC_OUT_SIZE_640_480       UVC_VSFRM_INDEX3
#define UVAC_OUT_SIZE_320_240       UVC_VSFRM_INDEX4
#define UVAC_OUT_SIZE_1024_576      UVC_VSFRM_INDEX5
#define UVAC_OUT_SIZE_800_600       UVC_VSFRM_INDEX6
#define UVAC_OUT_SIZE_800_480       UVC_VSFRM_INDEX7
#define UVAC_OUT_SIZE_640_360       UVC_VSFRM_INDEX8
#define UVAC_OUT_SIZE_160_120       UVC_VSFRM_INDEX10    //not in configuration descriptor
#define UVAC_OUT_SIZE_128_96        UVC_VSFRM_INDEX11   //not in configuration descriptor
#define UVAC_OUT_SIZE_352_288       UVC_VSFRM_INDEX12
#define UVAC_OUT_SIZE_1920_1080     UVC_VSFRM_INDEX9

//#NT#2014/06/16#Hideo Lin -begin
//#NT#To improve UVC size setting flexibility
#define UVC_SIZE_DEFAULT                    0x00000000
#define UVC_SIZE_320x240                    0x00000001
#define UVC_SIZE_640x480                    0x00000002
#define UVC_SIZE_640x360                    0x00000004
#define UVC_SIZE_720x480                    0x00000008
#define UVC_SIZE_800x600                    0x00000010
#define UVC_SIZE_848x480                    0x00000020
#define UVC_SIZE_960x720                    0x00000040
#define UVC_SIZE_1024x768                   0x00000080
#define UVC_SIZE_1024x576                   0x00000100
#define UVC_SIZE_1280x960                   0x00000200
#define UVC_SIZE_1280x720                   0x00000400
#define UVC_SIZE_1440x1080                  0x00000800
#define UVC_SIZE_1920x1080                  0x00001000
#define UVC_SIZE_MAX                        UVC_SIZE_1920x1080
#define UVC_FRAME_NUM_MAX                   13
//#NT#2014/06/16#Hideo Lin -end

/**
    Video format for UVC.
*/
typedef enum _UVAC_VIDEO_FORMAT
{
    UVAC_VIDEO_FORMAT_422,                 ///< YCbCr 422, UV packed.
    UVAC_VIDEO_FORMAT_420,                 ///< YCbCr 420, UV packed.
    ENUM_DUMMY4WORD(UVAC_VIDEO_FORMAT)
} UVAC_VIDEO_FORMAT;

/**
    Video frame triggered type.
*/
typedef enum _UVAC_TRIG_GETVID_TYPE
{
    UVAC_TRIG_GETVID_TYPE_AUD,              ///< Video frame trigged by audio driver.
    UVAC_TRIG_GETVID_TYPE_TIMER,            ///< Video frame trigged by timer.
    UVAC_TRIG_GETVID_TYPE_OTHER,            ///< Reserved.
    ENUM_DUMMY4WORD(UVAC_TRIG_GETVID_TYPE)
}UVAC_TRIG_GETVID_TYPE;

/**
    UVAC configuration identifier.

    This definition is used in UVAC_SetConfig() to assgin new configuration to specified ID function.
*/
typedef enum _UVAC_CONFIG_ID
{
    UVAC_CONFIG_H264_TARGET_SIZE,           ///< H.264 stream target size(Byte/Sec). Default value is 0x100000.
    UVAC_CONFIG_H264_3DNR_LEVEL,            ///< H.264 3DNR settings (0 to 7). 0 is disabled and 7 is strongest. Default value is 0.
    UVAC_CONFIG_MJPG_TARGET_SIZE,           ///< Motion JPEG stream target size(Byte/Sec). Default value is 0x300000.
    UVAC_CONFIG_VID_TRIG_TYPE,              ///< Video trigger type, refer to UVAC_TRIG_GETVID_TYPE. Default value is UVAC_TRIG_GETVID_TYPE_AUD.
    UVAC_CONFIG_VENDOR_CALLBACK,            ///< The callback function for USB Vendor Request
    ENUM_DUMMY4WORD(UVAC_CONFIG_ID)
} UVAC_CONFIG_ID;

/**
    Callback functions prototype that UVAC task used.

    These definitions are used in the structure UVAC_INFO and would be used during UVAC_Open() to specify the
    UVAC_Task needed information.
*/
//@{
typedef UINT32 (*UVAC_STARTVIDEOCB)(UINT32 VideoWidth, UINT32 VideoHeight, UVAC_VIDEO_FORMAT VideoFmt); ///< Callback for starting video.
typedef UINT32 (*UVAC_GETVIDEOBUFCB)(UINT32 *yAddr, UINT32 *cbAddr, UINT32 *crAddr, UINT32 *lineOfsY);  ///< Callback for getting video buffer.
typedef UINT32 (*UVAC_STOPVIDEOCB)(void);                                                               ///< Callback for stopping video.
//@}

/**
    Structure used for UVAC Device Vendor Request Callback API.

    This structure is used in the callback prototype UVAC_VENDOR_REQ_CB.
*/
typedef struct
{
    BOOL      bHostToDevice;      ///< Data direction.
                                  ///< - @b TRUE: USB Host sent data to USB Device.
                                  ///< - @b FALSE: USB Host will get data from USB Device.
    UINT8     uiReguest;          ///< The bRequest field in USB Device Requests.
    UINT16    uiValue;            ///< The wValue field in USB Device Requests.
    UINT16    uiIndex;            ///< The wIndex field in USB Device Requests.
    UINT32    uiDataAddr;         ///< Data address.
    UINT32    uiDataSize;         ///< Data size, in byte, maximun is 64. If there is no data to transfer, this should be zero.
} UVAC_VENDOR_PARAM, *PUVAC_VENDOR_PARAM;

/**
    Callback function prototype for UVAC Device Vendor Request.
*/
typedef void (*UVAC_VENDOR_REQ_CB)(PUVAC_VENDOR_PARAM pParam);




/**
    String Descriptor
*/
typedef  struct _UVAC_STRING_DESC
{
    UINT8       bLength;                ///< size of String Descriptor
    UINT8       bDescriptorType;        ///< String Descriptor type
    UINT8       bString[254];           ///< UNICODE stirng
} UVAC_STRING_DESC;

/**
    Information needed for UVAC module.

    This structure is used in UVAC_Open() to specify the UVAC task needed information for normal working
    and user customization callback.
*/
typedef struct {

    UVAC_STRING_DESC    *pManuStringDesc;    ///< USB Manufacturer String Descriptor ptr
    UVAC_STRING_DESC    *pProdStringDesc;    ///< USB Product String Descriptor ptr
    UVAC_STRING_DESC    *pSerialStringDesc;  ///< USB Serial Number String Descriptor ptr
    UINT16              VID;                 ///< Customized USB Vendor ID
    UINT16              PID;                 ///< Customized USB Product ID
    UINT32              UvacMemAdr;          ///< Buffer address for USB operation
    UINT32              UvacMemSize;         ///< Buffer size for USB operation
    UVAC_STARTVIDEOCB   fpStartVideoCB;      ///< Callback function to start video
    UVAC_GETVIDEOBUFCB  fpGetVideoBufCB;     ///< Callback function to get video buffer
    UVAC_STOPVIDEOCB    fpStopVideoCB;       ///< Callback function to stop video buffer
} UVAC_INFO;

/**
    Open UVAC(USB VIDEO/AUDIO-Device-Class) module.

    Open UVAC(USB VIDEO/AUDIO-Device-Class) module. Before using this API to open UVAC task, the user must use UVAC_InstallID()
    to retrieve the system task ID and the flag ID for UVAC Task usage to keep the task normal working.

    @param[in] pClassInfo Information needed for opening UVAC Task. The user must prepare all the information needed.

    @return
     - @b E_OK:  The UVAC Task open done and success.
     - @b E_SYS: Buffer allocation failed.
     - @b E_PAR: Parameter error.
*/
extern UINT32   UVAC_Open(UVAC_INFO *pClassInfo);

/**
    Close UVAC module.
*/
extern void     UVAC_Close(void);

/**
    Suspend UVAC when it is running
*/
extern INT32 UVAC_Suspend(void);


/**
    Resume UVAC when it be suspended.
*/
extern INT32 UVAC_Resume(void);

//#NT#2013/09/26#Calvin Chang#Drop frame when UVC is resumed -begin
/**
    Resume UVAC and drop some frames when it be suspended.
*/
extern INT32 UVAC_ResumeAndDrop(void);
//#NT#2013/09/26#Calvin Chang -end

/**
    Get needed buffer size for UVAC module.
*/
extern UINT32 UVAC_GetNeedMemSize(void);

//#NT#2014/06/16#Hideo Lin -begin
//#NT#To improve UVC size setting flexibility
extern void UVAC_SetMjpgOnly(BOOL bValue);
extern void UVAC_UvcSizeConfig(UINT32 uiSize);
extern void UVAC_SetUvcDefaultSize(UINT32 uiSize);
//#NT#2014/06/16#Hideo Lin -end

/**
    Set UVAC configuration

    Assign new configuration of the specified ConfigID.

    @param[in] ConfigID         Configuration identifier
    @param[in] Value            Configuration context for ConfigID
*/
extern void UVAC_SetConfig(UVAC_CONFIG_ID ConfigID, UINT32 Value);

/**
     Install UVAC task,flag and semaphore id

     This is a default setting API. Users can create their own task id, semphore id,
     flag id to adjust own stack size and task priority as the following:

     @code
     {
        #define PRI_UVACVIDEO          10
        #define PRI_UVCISOIN           (PRI_UVACVIDEO + 2)
        #define PRI_UACISOIN           (PRI_UVACVIDEO + 1)
        #define STKSIZE_UVACVIDEO           2048
        #define STKSIZE_UVCISOIN            1024
        #define STKSIZE_UACISOIN            1024

        UINT32 FLG_ID_UVAC = 0;
        UINT32 UVACVIDEOTSK_ID = 0;
        UINT32 UVCISOINTSK_ID = 0;
        UINT32 UACISOINTSK_ID = 0;
        extern void UVAC_IsoInAudTsk(void);
        extern void UVAC_VideoTsk(void);
        extern void UVAC_IsoInVidTsk(void);

        void UVAC_InstallID(void)
        {
            OS_CONFIG_FLAG(FLG_ID_UVAC);
            OS_CONFIG_TASK(UVACVIDEOTSK_ID,   PRI_UVACVIDEO,  STKSIZE_UVACVIDEO,  UVAC_VideoTsk);
            OS_CONFIG_TASK(UVCISOINTSK_ID,    PRI_UVCISOIN,   STKSIZE_UVCISOIN,   UVAC_IsoInVidTsk);
            OS_CONFIG_TASK(UACISOINTSK_ID,    PRI_UACISOIN,   STKSIZE_UACISOIN,   UVAC_IsoInAudTsk);
        }
     }
     @endcode
*/
extern void UVAC_InstallID(void) _SECTION(".kercfg_text");


/**
     Version

*/
extern CHAR *UVAC_GetVerInfo(void);

/**
     Build Date

*/
extern CHAR *UVAC_GetBuildDate(void);


//@}

#endif  // _UVAC_H

