
#include "UIAppMovie.h"
#include "UIAppPhoto.h"
#include "NVTUserCommand.h"
#include "UIMovieMapping.h"
#include "UIPhotoMapping.h"
#include "SysCfg.h" //for memory POOL_ID
#include "UIAppPlay.h"
#include "GxDisplay.h"
#include "IPL_AlgInfor.h"
#include "IPL_Display.h"
#include "IPL_Display2.h"
#include "IPL_Cmd.h"
#include "IQS_Utility.h"
#include "ImgCaptureAPI.h"
#include "ImgCapInfor.h"
#include "PhotoTask.h"
#include "Sensor.h"
#include "DxSensor.h"
#include "NvtSystem.h"
#include "MediaRecAPI.h"
#include "MovieStamp.h"
#include "UIMode.h"
#include "caf_sample.h"
#include "RawEncApi.h"
#include "PrjCfg.h"
#include "UICommon.h"
#include "BG_Images.h"
//#NT#2013/05/15#Calvin Chang#Add customer's user data to MOV/MP4 file format -begin
#include "MovieUdtaVendor.h"
#include "GSensor.h"

//#NT#2013/05/15#Calvin Chang -end
#if PIP_VIEW_FUNC
#include "PipView.h"
#endif
#if (UI_STYLE==UI_STYLE_DRIVER)
#include "IQS_Utility.h"
#endif
#if _LDWS_FUNC_
//#NT#2013/04/25#adrian jiang -begin
#include "ldws_lib.h"
extern void Photo_LdwsCB(LDWS_CBMSG uiMsgID, void *Data);

extern UINT32      gLDWScount ;//for test
extern PHOTO_FUNC_INFO PhotoFuncInfo_dummy;
extern PHOTO_FUNC_INFO PhotoFuncInfo_ldws;

//#NT#2013/04/25#adrian jiang -end
#endif
#if (UVC_RECORD_FUNC == ENABLE)
#include "UIAppUsbCam.h"
#endif

// Test codes
#define MOVIE_TEST_ENABLE       DISABLE
#define MOVIE_YUV_SIZE_MAX      (1920*1080*2)   // maximum YUV raw data size
#define MOVIE_YUV_NUM           8   // 8 source images for fixed YUV mode

//#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
#define MOVIE_AUDIO_NR_PATTERN_ENABLE    DISABLE
//#NT#2012/10/30#Calvin Chang -end

extern int  UVAC_SetImageSize(UINT16 uhSize);
extern void UIMovie_3DNRReset(void);
extern BOOL Sensor_CheckExtSensor(void);
ER MovieExe_SetGetReadyCB2_Sample(void);//2013/09/23 Meg


#define __MODULE__          UIMovieExe
//#define __DBGLVL__ 0        //OFF mode, show nothing
//#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGLVL__ 2        //TRACE mode, show err, wrn, ind, msg and func and ind, msg and func can be filtering by __DBGFLT__ settings
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"


static IPL_IME_INFOR            g_ImeInfo = {0};
static IPL_SET_IMGRATIO_DATA    g_ImgRatio = {0};

#if (UI_STYLE==UI_STYLE_DRIVER)
static BOOL    bSDIsSlow    = FALSE;
#endif
static volatile BOOL   g_bMovieRecYUVMergeEnable = 0;  // Isiah, implement YUV merge mode of recording func.
static volatile UINT32 g_uiMovieRecYUVMergeInterval = 1, g_uiMovieRecYUVMergeCounter = 0;
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
static BOOL g_bSensor2En = FALSE;
#endif

//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
#if (_FAST_BOOT_REC_FUNC_)
extern DX_HANDLE gDevLCDObj;
#endif
//#NT#2015/01/21#KS Hung -end

//-------------------------------------------------------------------------------------------------
// Set (load) YUV test pattern from JPEG files
#if (MOVIE_TEST_ENABLE == ENABLE)
#define YUV_RAW_PATH_FHD    "A:\\YUV_RAW\\FHD\\CLOCK\\"
#define YUV_RAW_PATH_HD     "A:\\YUV_RAW\\HD\\CLOCK\\"
#define YUV_RAW_PATH_VGA    "A:\\YUV_RAW\\VGA\\CLOCK\\"
#define YUV_RAW_PATH_QVGA   "A:\\YUV_RAW\\QVGA\\CLOCK\\"

static MEDIAREC_TESTMODE g_MediaRecTestMode = {0};
static char     sDirPath[40];
static char     sFileName[40];
static UINT32   g_uiFixedYUVFrameCount = 0;

extern void MovRec_SetTestMode(MEDIAREC_TESTMODE *pTestMode);
extern void DscMovie_RegIPLChangeCB(MediaRecIPLChangeCB *pIPLChangeCB);
extern void DscMovie_RegGetReadyBufCB(MediaRecIPLGetReadyCB *pGetReadyBufCB);
extern void DscMovie_RegGetReadyBufCB2(MediaRecIPLGetReadyCB *pGetReadyBufCB);

static void Movie_SetYUV(UINT32 uiMovieSize)
{
    UINT32      i;
    UINT32      uiPoolAddr, uiRawAddr, uiFileSize, uiHeight;
    UINT32      uiYLineOffset, uiUVLineOffset;
    char        *pDirPath;
    ER          err = E_OK;
    FST_FILE    pFile;

    uiPoolAddr = OS_GetMempoolAddr(POOL_ID_APP);
    uiRawAddr = uiPoolAddr;

    // ----------------------------------- <--- APP buffer starting address
    // | YUV buffer 1 for media recorder |
    // -----------------------------------
    // | YUV buffer 2 for media recorder |
    // -----------------------------------
    // | YUV buffer 3 for media recorder |
    // -----------------------------------
    // | ...                             |
    // ----------------------------------- <--- media recorder starting address
    //
    // Note: Be sure the buffer is enough for storing the necessary YUV patterns!

    //pDirPath = UserMedia_GetRawDirPath();

    switch (uiMovieSize)
    {
    case MOVIE_SIZE_1080P_FULLRES: // 1080p
    case MOVIE_SIZE_1080P: // 1080p
        uiYLineOffset  = 1920;
        uiUVLineOffset = 1920; // UV pack
        uiHeight = 1088;
        //if (*pDirPath == 0) // never set directory path, use default
        {
            sprintf(sDirPath, YUV_RAW_PATH_FHD, sizeof(YUV_RAW_PATH_FHD));
            pDirPath = sDirPath;
        }
        break;

    case MOVIE_SIZE_VGA:
    //case MOVIE_SIZE_WVGA: // no WVGA pattern, use VGA instead
        uiYLineOffset  = 640;
        uiUVLineOffset = 640; // UV pack
        uiHeight = 480;
        //if (*pDirPath == 0) // never set directory path, use default
        {
            sprintf(sDirPath, YUV_RAW_PATH_VGA, sizeof(YUV_RAW_PATH_VGA));
            pDirPath = sDirPath;
        }
        break;

    case MOVIE_SIZE_QVGA:
        uiYLineOffset  = 320;
        uiUVLineOffset = 320; // UV pack
        uiHeight = 240;
        //if (*pDirPath == 0) // never set directory path, use default
        {
            sprintf(sDirPath, YUV_RAW_PATH_QVGA, sizeof(YUV_RAW_PATH_QVGA));
            pDirPath = sDirPath;
        }
        break;

    case MOVIE_SIZE_720P:
    default:
        uiYLineOffset  = 1280;
        uiUVLineOffset = 1280; // UV pack
        uiHeight = 720;
        //if (*pDirPath == 0) // never set directory path, use default
        {
            sprintf(sDirPath, YUV_RAW_PATH_HD, sizeof(YUV_RAW_PATH_HD));
            pDirPath = sDirPath;
        }
        break;
    }

    g_MediaRecTestMode.YUVInfo.uiTotalNum = MOVIE_YUV_NUM;

    for (i = 0; i < g_MediaRecTestMode.YUVInfo.uiTotalNum; i++)
    {
        // 1. Read Y data from file
        sprintf(sFileName, "%sY%02d.RAW", pDirPath, i+1);
        uiFileSize = MOVIE_YUV_SIZE_MAX;
        pFile = FileSys_OpenFile(sFileName, FST_OPEN_READ);
        if (pFile)
        {
            FileSys_ReadFile(pFile, (UINT8 *)uiRawAddr, &uiFileSize, 0, NULL);
        }
        FileSys_CloseFile(pFile);
        DBG_IND("File %s, size %d\r\n", sFileName, uiFileSize);

        if (err != E_OK)
        {
            DBG_ERR("Open %s for reading failed!\r\n", sFileName);
            return;
        }
        else if (uiFileSize == MOVIE_YUV_SIZE_MAX)
        {
            DBG_ERR("Read buffer is not enough!\r\n");
            return;
        }

        // Set Y source address to media recorder
        g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiYAddr = uiRawAddr;
        //uiRawAddr += uiFileSize;
        uiRawAddr += (uiYLineOffset * uiHeight);

        // 2. Read UV data from file
        sprintf(sFileName, "%sUV%02d.RAW", pDirPath, i+1);
        uiFileSize = MOVIE_YUV_SIZE_MAX;
        pFile = FileSys_OpenFile(sFileName, FST_OPEN_READ);
        if (pFile)
        {
            FileSys_ReadFile(pFile, (UINT8 *)uiRawAddr, &uiFileSize, 0, NULL);
        }
        FileSys_CloseFile(pFile);
        DBG_IND("File %s, size %d\r\n", sFileName, uiFileSize);

        if (err != E_OK)
        {
            DBG_ERR("Open %s for reading failed!\r\n", sFileName);
            return;
        }
        else if (uiFileSize == MOVIE_YUV_SIZE_MAX)
        {
            DBG_ERR("Read buffer is not enough!\r\n");
            return;
        }

        // Set UV source address to media recorder
        g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiCbAddr = uiRawAddr;
        g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiCrAddr = uiRawAddr;
        //uiRawAddr += uiFileSize;
        uiRawAddr += (uiUVLineOffset * uiHeight) / 2;

        // Set Y line offset
        g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiYLineOffset  = uiYLineOffset;
        // Set UV line offset
        g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiUVLineOffset = uiUVLineOffset;

        DBG_MSG("g_UserMediaYUVInfo.YUVSrc[%d].uiYAddr        0x%x\r\n", i, g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiYAddr);
        DBG_MSG("g_UserMediaYUVInfo.YUVSrc[%d].uiCbAddr       0x%x\r\n", i, g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiCbAddr);
        DBG_MSG("g_UserMediaYUVInfo.YUVSrc[%d].uiCrAddr       0x%x\r\n", i, g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiCrAddr);
        DBG_MSG("g_UserMediaYUVInfo.YUVSrc[%d].uiYLineOffset  %d\r\n",   i, g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiYLineOffset);
        DBG_MSG("g_UserMediaYUVInfo.YUVSrc[%d].uiUVLineOffset %d\r\n",   i, g_MediaRecTestMode.YUVInfo.YUVSrc[i].uiUVLineOffset);
    }
}

void MovieExe_FakeIPLChangeCB(UINT32 mode, UINT32 param)
{
    UINT32  APP_RESERVE_SIZE = 1920 * 1080 * 15;
    //UINT32  APP_RESERVE_SIZE = 1440 * 1080 * 15 + 1280 * 720 * 15; // path 1 + path 2 (maximum)
    UINT32  uiPoolAddr;

    uiPoolAddr = OS_GetMempoolAddr(POOL_ID_APP);

    switch (mode)
    {
    case MEDIAREC_IPLCHG_VIDEO:
        // Re-allocate buffer for Media Recorder
        MediaRec_ReAllocBuf2VA(uiPoolAddr + APP_RESERVE_SIZE, POOL_SIZE_APP - APP_RESERVE_SIZE);
        break;
    }
}

void MovieExe_FakeGetReadyBufCB(UINT32 mode, UINT32 param)
{
    MEDIAREC_READYBUF_INFO readyInfo;

    switch (mode)
    {
    case MEDIAREC_IPLGET_READYBUF:
        readyInfo.y         = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiYAddr;
        readyInfo.cb        = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiCbAddr;
        readyInfo.cr        = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiCrAddr;
        readyInfo.y_lot     = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiYLineOffset;
        readyInfo.uv_lot    = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiUVLineOffset;

        g_uiFixedYUVFrameCount++;
        if (g_uiFixedYUVFrameCount >= g_MediaRecTestMode.YUVInfo.uiTotalNum)
            g_uiFixedYUVFrameCount = 0;

        MediaRec_SetReadyBufWithID(0, &readyInfo); // set ready buffer of image source 0
        break;

    default:
        break;
    }
}

void MovieExe_FakeGetReadyBufCB2(UINT32 mode, UINT32 param)
{
    MEDIAREC_READYBUF_INFO readyInfo;

    switch (mode)
    {
    case MEDIAREC_IPLGET_READYBUF:
        // use path 1 image source temporarily
        readyInfo.y         = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiYAddr;
        readyInfo.cb        = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiCbAddr;
        readyInfo.cr        = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiCrAddr;
        readyInfo.y_lot     = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiYLineOffset;
        readyInfo.uv_lot    = g_MediaRecTestMode.YUVInfo.YUVSrc[g_uiFixedYUVFrameCount].uiUVLineOffset;

        //g_uiFixedYUVFrameCount++;
        //if (g_uiFixedYUVFrameCount >= g_MediaRecTestMode.YUVInfo.uiTotalNum)
        //    g_uiFixedYUVFrameCount = 0;

        MediaRec_SetReadyBufWithID(1, &readyInfo); // set ready buffer of image source 0
        break;

    default:
        break;
    }
}
#endif
//-------------------------------------------------------------------------------------------------

#if (UI_STYLE==UI_STYLE_DRIVER)
void Movie_SetSDSlow(BOOL IsSlow)
{
    bSDIsSlow = IsSlow;
}
#endif

#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
void Movie_SetDualRec(BOOL bOn)
{
    if (bOn)
    {
        UI_SetData(FL_MOVIE_DUAL_REC, TRUE);
    }
    else
    {
        UI_SetData(FL_MOVIE_DUAL_REC, FALSE);
        DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_DUAL_SENSOR_2NDOFF);
    }
}
#endif

void Movie_SetRecParam(void)
{
    UINT32  uiRecFormat, uiFileType;
    UINT32  uiWidth, uiHeight, uiMovieSize, uiQuality;
    UINT32  uiVidCodec, uiVidFrameRate, uiTargetBitrate, uiH264GopType, ui3DNRLevel, uiVidRotate, uiDispAspectRatio;
    UINT32  uiAudCodec, uiAudSampleRate, uiAudChannelNum, uiAudChannelType;
    BOOL    bAudFilterEn;
    UINT32  uiWidth2, uiHeight2, uiTargetBitrate2, uiDispAspectRatio2, uiVidFrameRate2;

    uiMovieSize         = UI_GetData(FL_MOVIE_DUAL_REC) ?
                          UI_GetData(FL_MOVIE_SIZE_DUAL) :
                          UI_GetData(FL_MOVIE_SIZE);            // movie size

    uiQuality           = UI_GetData(FL_MOVIE_QUALITY);         // movie quality
    // Isiah, implement YUV merge mode of recording func.
    if (!FlowMovie_RecGetYUVMergeMode())
    {
        if (UI_GetData(FL_WIFI_LINK_STA) != WIFI_LINK_OK)
        {
            if (SysGetFlag(FL_MOVIE_TIMELAPSE_REC)==MOVIE_TIMELAPSEREC_OFF)
                uiRecFormat     = MEDIAREC_AUD_VID_BOTH;            // recording type
            else
                uiRecFormat     = MEDIAREC_TIMELAPSE;               // recording type
        } else
            uiRecFormat         = Get_MovieFormatValue(UI_GetData(FL_WIFI_MOVIE_FMT));            //rtsp live or record mode
    }
    else // Change to YUV Merge mode.
    {
        uiRecFormat         = MEDIAREC_MERGEYUV;
    }

    uiFileType          = MEDIAREC_MOV;                         // file type
    //uiFileType          = MEDIAREC_MP4;                         // file type
    //uiFileType          = MEDIAREC_AVI;                         // file type

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        uiWidth             = GetMovieSizeWidth_2p(0, uiMovieSize);         // image width
        uiHeight            = GetMovieSizeHeight_2p(0, uiMovieSize);        // image height
        uiVidFrameRate      = GetMovieFrameRate_2p(0, uiMovieSize);         // video frame rate
        uiTargetBitrate     = GetMovieTargetBitrate_2p(0, uiMovieSize);     // enable rate control and set target rate
        uiDispAspectRatio   = GetMovieDispAspectRatio_2p(0, uiMovieSize);   // display aspect ratio
    }
    else
    {
        uiWidth             = GetMovieSizeWidth(uiMovieSize);               // image width
        uiHeight            = GetMovieSizeHeight(uiMovieSize);              // image height
        uiVidFrameRate      = GetMovieFrameRate(uiMovieSize);               // video frame rate
        uiTargetBitrate     = GetMovieTargetBitrate(uiMovieSize, uiQuality);// enable rate control and set target rate
        uiDispAspectRatio   = GetMovieDispAspectRatio(uiMovieSize);         // display aspect ratio
    }
    uiH264GopType       = MEDIAREC_H264GOP_IPONLY;              // H.264 GOP type (IP only)
    //uiH264GopType       = MEDIAREC_H264GOP_IPB;                 // H.264 GOP type (IPB)

    if (UI_GetData(FL_MovieMCTFIndex) == MOVIE_MCTF_OFF)
    {
        ui3DNRLevel     = H264_3DNR_DISABLE;                    // H.264 3DNR disable
    }
    else
    {
        //ui3DNRLevel     = H264_3DNR_NORMAL;                     // H.264 3DNR enable (normal)
        //ui3DNRLevel     = H264_3DNR_WEAK;                       // H.264 3DNR enable (weak)
        ui3DNRLevel     = H264_3DNR_STRONGEST;                  // H.264 3DNR enable (strongest)
    }

    uiAudSampleRate     = 32000;                                // audio sampling rate
    #if (UI_STYLE==UI_STYLE_DRIVER)
    uiAudChannelType    = MEDIAREC_AUDTYPE_LEFT;//MEDIAREC_AUDTYPE_RIGHT;               // audio channel type
    uiAudChannelNum     = 1;                                    // audio channel number
    bAudFilterEn        = FALSE;                                // enable/disable audio filter
    #else
    uiAudChannelType    = MEDIAREC_AUDTYPE_STEREO;              // audio channel type
    uiAudChannelNum     = 2;                                    // audio channel number
    bAudFilterEn        = TRUE;                                 // enable/disable audio filter
    #endif

    switch (uiFileType)
    {
    case MEDIAREC_AVI: // AVI format, audio should be PCM, video can be H.264 or MJPEG
        uiAudCodec  = MEDIAREC_ENC_PCM;
        uiVidCodec  = MEDIAREC_ENC_H264;
        //uiVidCodec  = MEDIAREC_ENC_JPG;
        break;

    case MEDIAREC_MP4: // MP4 format, audio should be AAC, video should be H.264
        uiAudCodec  = MEDIAREC_ENC_AAC;
        uiVidCodec  = MEDIAREC_ENC_H264;
        break;

    case MEDIAREC_MOV: // MOV format, audio can be PCM or AAC, video can be H.264 or MJPEG
    default:
        //uiAudCodec  = MEDIAREC_ENC_AAC;
        uiAudCodec  = MEDIAREC_ENC_PCM;
        uiVidCodec  = MEDIAREC_ENC_H264;
        //uiVidCodec  = MEDIAREC_ENC_JPG;
        break;
    }

    //#NT#2013/04/17#Calvin Chang#Support Rotation information in Mov/Mp4 File format -begin
    uiVidRotate = MEDIAREC_MOV_ROTATE_0;
    //#NT#2013/04/17#Calvin Chang -end

#if  (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)

    uiH264GopType = MEDIAREC_H264GOP_IPONLY; // forced set to IP-only for dual recording due to buffer issue

    #if (DRAM_SIZE == DRAM_SIZE_256MB)
        IPL_Display_SetBufferNum(8);
        IPL_Display_SetBufferNum2(8);
    #else
        IPL_Display_SetBufferNum(6);
        IPL_Display_SetBufferNum2(5);
    #endif

    IPL_Display_SetEncodeLag(2);//jacky lan 
    if (Sensor_CheckExtSensor())
    {
        if (UI_GetData(FL_MOVIE_DUAL_REC) && g_bSensor2En)
        {
            DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_DUAL_SENSOR);
        }
        else
        {
            DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_DUAL_SENSOR_2NDINACT);
        }
    }
    else
    {
        DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_DUAL_SENSOR_2NDOFF);
    }

    MediaRec_EnableEndChar4SingleRec(TRUE);
    MediaRec_EnableEndChar4MultiRec(TRUE);

#else

    if (uiH264GopType == MEDIAREC_H264GOP_IPB) // IPB needs 9 buffers at least, set 10 for insurance
    {
        IPL_Display_SetBufferNum(10);
    }
    else
    {
        #if (UVC_RECORD_FUNC == ENABLE)
        IPL_Display_SetBufferNum(6); // IPP needs 6 buffers at least
        #else
        IPL_Display_SetBufferNum(8); // IPP needs 6 buffers at least, set 8 for insurance
        #endif
    }

    MediaRec_EnableEndChar4SingleRec(FALSE);
    MediaRec_EnableEndChar4MultiRec(FALSE);

#endif

    //---------------------------------------------------------------------------------------------
    // set media recording control parameters
    //---------------------------------------------------------------------------------------------
    // disable golf shot recording
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_GOLFSHOT_ON, FALSE, 0, 0);
    // disable flash recording
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_FLASH_ON, FALSE, 0, 0);
    //#NT#2012/11/20#Philex Lin-begin
    // enable/disable power off protection
    #if (UI_STYLE==UI_STYLE_DRIVER)
    if ((bSDIsSlow == TRUE) || (uiFileType == MEDIAREC_MP4)) // slow card or MP4 doesn't support power off protection
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_PWROFFPT, FALSE, 0, 0);
    else
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_PWROFFPT, TRUE, 0, 0);
    #else
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_PWROFFPT, FALSE, 0, 0);
    #endif
    //#NT#2012/11/20#Philex Lin-end
    // disable time lapse recording => not necessary now, since it can be set by using MEDIAREC_RECPARAM_RECFORMAT
    //MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE, FALSE, 0, 0);
    // set recording format (MEDIAREC_VID_ONLY, MEDIAREC_AUD_VID_BOTH, MEDIAREC_TIMELAPSE, ...)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_RECFORMAT, uiRecFormat, 0, 0);
    // set customized user data on/off
    //MediaRec_ChangeParameter(MEDIAREC_RECPARAM_EN_CUSTOMUDATA, FALSE, 0, 0);
    // set file type (MEDIAREC_AVI, MEDIAREC_MOV, MEDIAREC_MP4)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_FILETYPE, uiFileType, 0, 0);

    // set recording end type (MEDIAREC_ENDTYPE_NORMAL, MEDIAREC_ENDTYPE_CUTOVERLAP, MEDIAREC_ENDTYPE_CUT_TILLCARDFULL)
    // MP4 cannot support seamless recording
    if ((UI_GetData(FL_MOVIE_CYCLIC_REC) != MOVIE_CYCLICREC_OFF) && (uiFileType != MEDIAREC_MP4))
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_ENDTYPE, MEDIAREC_ENDTYPE_CUTOVERLAP, 0, 0);
    }
    else
    {
        #if 1
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_ENDTYPE, MEDIAREC_ENDTYPE_NORMAL, 0, 0);
        #else
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_ENDTYPE, MEDIAREC_ENDTYPE_CUT_TILLCARDFULL, 0, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_CUTSEC, 1500, 0, 0); // 25 min
        #endif
    }

    // enable/disable time lapse recording
    if (uiRecFormat == MEDIAREC_TIMELAPSE)
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE, TRUE, 0, 0);
        switch(SysGetFlag(FL_MOVIE_TIMELAPSE_REC))
        {
          case MOVIE_TIMELAPSEREC_100MS:
          default:
            MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE_TIME, 100, 0, 0);
            break;
          case MOVIE_TIMELAPSEREC_200MS:
            MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE_TIME, 200, 0, 0);
            break;
          case MOVIE_TIMELAPSEREC_500MS:
            MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE_TIME, 500, 0, 0);
            break;
        }
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_ENDTYPE, MEDIAREC_ENDTYPE_NORMAL, 0, 0); // should be normal end for time lapse recording
    }
    else
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TIMELAPSE, FALSE, 0, 0);
    }

    //---------------------------------------------------------------------------------------------
    // set video parameters
    //---------------------------------------------------------------------------------------------
    // set image width
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_WIDTH, uiWidth, 0, 0);
    // set image height
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_HEIGHT, uiHeight, 0, 0);
    // set video frame rate
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_FRAMERATE, uiVidFrameRate, 0, 0);
    // set target data rate (bytes per second)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TARGETRATE, uiTargetBitrate, 0, 0);
    // set video codec (MEDIAREC_ENC_H264, MEDIAREC_ENC_JPG)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_VIDEOCODEC, uiVidCodec, 0, 0);
    // set H.264 GOP type (MEDIAREC_H264GOP_IPONLY, MEDIAREC_H264GOP_IPB)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H264GOPTYPE, uiH264GopType, 0, 0);
    // set H.264 3DNR level (H264_3DNR_DISABLE, H264_3DNR_WEAKEST, H264_3DNR_WEAK, ...)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H2643DNRLEVEL, ui3DNRLevel, 0, 0);
    //#NT#2013/04/17#Calvin Chang#Support Rotation information in Mov/Mp4 File format -begin
    // set rotation information
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_MOV_ROTATE, uiVidRotate, 0, 0);
    //#NT#2013/04/17#Calvin Chang -end
    //#NT#2013/09/17#Calvin Chang#add for YUV fromat setting -begin
    if (uiVidCodec == MEDIAREC_ENC_JPG)
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_JPG_YUVFORMAT, MEDIAREC_JPEG_FORMAT_420, 0, 0);
    //#NT#2010/09/17#Calvin Chang -end
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_DAR, uiDispAspectRatio, 0, 0);

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        uiWidth2            = GetMovieSizeWidth_2p(1, uiMovieSize);         // image width
        uiHeight2           = GetMovieSizeHeight_2p(1, uiMovieSize);        // image height
        uiVidFrameRate2     = GetMovieFrameRate_2p(1, uiMovieSize);         // video frame rate
        uiTargetBitrate2    = GetMovieTargetBitrate_2p(1, uiMovieSize);     // enable rate control and set target rate
        uiDispAspectRatio2  = GetMovieDispAspectRatio_2p(1, uiMovieSize);   // display aspect ratio

        DscMovie_Config(MOVIE_CFG_MULTIREC, 1);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_MULTIREC_ON, TRUE, 0, 0);
        //MovieExe_SetGetReadyCB2_Sample();
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_FILETYPE_2, MEDIAREC_MOV, 0, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_VIDEOCODEC, MEDIAREC_ENC_H264, 1, 0);
        //MediaRec_ChangeParameter(MEDIAREC_RECPARAM_VIDEOCODEC, MEDIAREC_ENC_JPG, 1, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_JPG_YUVFORMAT, MEDIAREC_JPEG_FORMAT_420, 0, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_WIDTH, uiWidth2, 1, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_HEIGHT, uiHeight2, 1, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_FRAMERATE, uiVidFrameRate2, 1, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TARGETRATE, uiTargetBitrate2, 1, 0);
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_DAR, uiDispAspectRatio2, 1, 0);
        //MediaRec_ChangeParameter(MEDIAREC_RECPARAM_PWROFFPT, FALSE, 0, 0);

        #if  (_SENSORLIB2_ == _SENSORLIB2_DUMMY_)
        DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_DUAL_REC_TEST); // just for dual recording test!!!
        MediaRec_EnableEndChar4SingleRec(TRUE);
        MediaRec_EnableEndChar4MultiRec(TRUE);
        #endif
    }
    else
    {
        #if (_MULTI_RECORD_FUNC_)
        DscMovie_Config(MOVIE_CFG_MULTIREC, 1); // always enable ready buffer CB2
        #else
        DscMovie_Config(MOVIE_CFG_MULTIREC, 0);
        #endif
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_MULTIREC_ON, FALSE, 0, 0);
    }

    //---------------------------------------------------------------------------------------------
    // set audio parameters
    //---------------------------------------------------------------------------------------------
    // set audio sampling rate (AUDIO_SR_8000, AUDIO_SR_11025, AUDIO_SR_12000, ...)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_AUD_SAMPLERATE, uiAudSampleRate, 0, 0);
    // set audio source (AUDIO_CH_RIGHT, AUDIO_CH_LEFT, AUDIO_CH_STEREO)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_AUD_SRC, uiAudChannelType, 0, 0);
    // set audio channel number (1 or 2; if audio channel is mono but channel number is 2, the audio data will be duplicated)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_AUD_CHS, uiAudChannelNum, 0, 0);
    // set audio codec (MEDIAREC_ENC_PCM, MEDIAREC_ENC_AAC)
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_AUD_CODEC, uiAudCodec, 0, 0);
    // set audio filter on/off
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_EN_AUDFILTER, bAudFilterEn, 0, 0);
    // Enable Tiny mode test
    //MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TINYMODE_ON, TRUE, 0, 0);
}

void MovieExe_CalcDispSize(ISIZE *pDispSize, ISIZE *pDevSize, ISIZE *pDevRatio, ISIZE *pImgRatio)
{
    if (((pDevRatio->w * 100) / pDevRatio->h) > ((pImgRatio->w * 100) / pImgRatio->h)) // device aspect ratio > image ratio
    {
        pDispSize->h = pDevSize->h;
        pDispSize->w = ALIGN_ROUND_16((pDevSize->w * pImgRatio->w * pDevRatio->h) / (pImgRatio->h * pDevRatio->w));
    }
    else
    {
        pDispSize->w = pDevSize->w;
        pDispSize->h = ALIGN_ROUND_4((pDevSize->h * pImgRatio->h * pDevRatio->w) / (pImgRatio->w * pDevRatio->h));
    }
}

INT32 MovieExe_OnDraw(DISPSRV_DRAW* pDraw)
{
    ER er;
    UINT32 uiLockIdx;
    IMG_BUF* pSrc=NULL;
    IMG_BUF* pDst=pDraw->pImg[0];

    IRECT rcSrc={0},rcDst={0};

    if((er=pDraw->fpLock[DISPSRV_SRC_IDX_PRIMARY](&uiLockIdx,DISPSRV_LOCK_NEWEST))!=E_OK)
    {//locked fail indicate skip this draw
        return er;
    }

    pSrc = &pDraw->pDS[DISPSRV_SRC_IDX_PRIMARY]->pSrc[uiLockIdx];

    //--------------------Customer Draw Begin-----------------------------------
    //start to user draw
    rcSrc.x = 0;
    rcSrc.y = 0;
    rcSrc.w = pSrc->Width;
    rcSrc.h = pSrc->Height;
    rcDst.x = 0;
    rcDst.w = pDst->Width;
    rcDst.y = (pDst->Height - pSrc->Height) >> 1;
    rcDst.h = pSrc->Height;

    //scale src to dst
    GxImg_FillData(pDst,NULL,COLOR_YUV_BLACK);
    GxImg_ScaleData(pSrc,&rcSrc,pDst,&rcDst);

    pDraw->fpUnlock[DISPSRV_SRC_IDX_PRIMARY](uiLockIdx);

    if(pDraw->bDualHandle)
    {
        if((er=pDraw->fpLock[DISPSRV_SRC_IDX_SECONDARY](&uiLockIdx,DISPSRV_LOCK_NEWEST))!=E_OK)
        {//locked fail indicate skip this draw
            return er;
        }

        pDst=pDraw->pImg[1];
        pSrc = &pDraw->pDS[DISPSRV_SRC_IDX_SECONDARY]->pSrc[uiLockIdx];

        rcSrc.x = 0;
        rcSrc.y = 0;
        rcSrc.w = pSrc->Width;
        rcSrc.h = pSrc->Height;
        rcDst.x = 0;
        rcDst.y = 0;
        rcDst.w = pDst->Width;
        rcDst.h = pDst->Height;
        //scale src to dst
        GxImg_FillData(pDst,NULL,COLOR_YUV_BLACK);
        GxImg_ScaleData(pSrc,&rcSrc,pDst,&rcDst);
        pDraw->fpUnlock[DISPSRV_SRC_IDX_SECONDARY](uiLockIdx);
    }
    //--------------------Customer Draw End-------------------------------------

    pDraw->fpFlip(TRUE);

    return E_OK;
}

extern PHOTO_FUNC_INFO PhotoFuncInfo_dummy;
extern PHOTO_FUNC_INFO PhotoFuncInfo_dis;
ISIZE _QueryDeviceModeAspect(UINT32 NewDevObj, UINT32 mode);

/**
  Initialize application for AVI mode

  Initialize application for AVI mode.

  @param void
  @return void
*/
INT32 MovieExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    MEM_RANGE Pool;
    ISIZE BufferSize;
    ISIZE DeviceSize;
    ISIZE AspectSize;
    ISIZE DispSize;
    ISIZE ImgRatio;
    UINT32 CurrDevObj;
    IPL_SENCROPRATIO CropRatio;
    UINT32 useFileDB = 0;
    BOOL IsRotatePanel = (VDO_USE_ROTATE_BUFFER==TRUE||OSD_USE_ROTATE_BUFFER==TRUE);

    PHOTO_FUNC_INFO* phead = NULL;

    //call default
    Ux_DefaultEvent(pCtrl,NVTEVT_EXE_OPEN,paramNum,paramArray);

    #if (UVC_RECORD_FUNC == ENABLE)
    {
        UINT32  uiUvcMemSize;

        UVAC_SetMjpgOnly(TRUE); // only support MJPEG to minimize buffer usage
        uiUvcMemSize = UVAC_GetNeedMemSize();

        Pool.Addr = OS_GetMempoolAddr(POOL_ID_APP) + uiUvcMemSize;
        Pool.Size = POOL_SIZE_APP - uiUvcMemSize;
    }
    #else
    {
        Pool.Addr = OS_GetMempoolAddr(POOL_ID_APP);
        Pool.Size = POOL_SIZE_APP;
    }
    #endif
    DscMovie_Config(MOVIE_CFG_POOL, (UINT32)&Pool);

    #if (MOVIE_PIM == ENABLE)
    {
        DscMovie_Config(MOVIE_CFG_PIMSIZE, 88*1024*1024); // 88MB for picture in movie function (can capture 2 14M images)
    }
    #endif

    //1.get current device size (current mode)
    DeviceSize = GxVideo_GetDeviceSize(DOUT1);
    BufferSize = DeviceSize;
    #if (_FPGA_EMULATION_ == ENABLE)
    //overwrite buffer size to reduce BW
    BufferSize.w = 320;
    BufferSize.h = 240;
    #endif
    #if((RTSP_LIVEVIEW_FUNC==ENABLE)||(WIFI_AP_RTSP_FUNC==ENABLE))
    BufferSize.w = 80;
    BufferSize.h = 60;
    #endif
    //3.get its Aspect Ratio
    //#NT#2015/01/21#KS Hung -begin
    //#NT#Fast boot recording.
    #if (_FAST_BOOT_REC_FUNC_)
    if (FlowMovie_GetFirstBootRecFlag())
    {
        CurrDevObj = (UINT32)gDevLCDObj;
    }
    else
    {
        CurrDevObj = GxVideo_GetDevice(DOUT1);
    }
    #else
    CurrDevObj = GxVideo_GetDevice(DOUT1);
    #endif
    //#NT#2015/01/21#KS Hung -end
    AspectSize = _QueryDeviceModeAspect(CurrDevObj, GxVideo_QueryDeviceLastMode(CurrDevObj));

    phead = &PhotoFuncInfo_dummy;
    phead->pNext = NULL; //mark end

//#NT#2013/04/25#adrian jiang -begin
    #if _LDWS_FUNC_
    {
        PHOTO_FUNC_INFO* pfunc = &PhotoFuncInfo_ldws;
        PHOTO_FUNC_INFO* pcurr = 0;
        pcurr = phead; while(pcurr->pNext) pcurr = pcurr->pNext; //find last func
        pcurr->pNext = pfunc; //append this func
        pfunc->pNext = NULL; //mark end
    }
//#NT#2013/04/29#adrian jiang -begin
    LDWS_RegisterCB(UIMovie_LDWSCB);
//#NT#2013/04/29#adrian jiang -end
    DBG_IND("\r\n LDWS::Movie LDWS  RegisterCB \r\n" );
    #endif
//#NT#2013/04/25#adrian jiang -end

    #if (MOVIE_TEST_ENABLE != ENABLE)
    {// DIS function
        PHOTO_FUNC_INFO* pfunc = &PhotoFuncInfo_dis;
        PHOTO_FUNC_INFO* pcurr = 0;
        pcurr = phead; while(pcurr->pNext) pcurr = pcurr->pNext; //find last func
        pcurr->pNext = pfunc; //append this func
        pfunc->pNext = NULL; //mark end
    }
    #endif

    DscMovie_Config(MOVIE_CFG_FUNCINFO, (UINT32)phead);
    DscMovie_Config(MOVIE_CFG_CACHESIZE, 0);
    DscMovie_Config(MOVIE_CFG_IMGPATH,MOVIE_NORMAL);

    #if (MOVIE_TEST_ENABLE == ENABLE) // test mode: using fixed YUV patterns
    {
        g_MediaRecTestMode.bEnable = TRUE;
        g_MediaRecTestMode.uiVidSrcType = MEDIAREC_SRC_FIXED_YUV;
        //g_MediaRecTestMode.uiAudSrcType = MEDIAREC_SRC_NORMAL;
        g_MediaRecTestMode.uiAudSrcType = MEDIAREC_SRC_FIXED_PCM;

        // Set YUV information if using fixed YUV as video source (MEDIAREC_SRC_FIXED_YUV)
        if (g_MediaRecTestMode.uiVidSrcType == MEDIAREC_SRC_FIXED_YUV)
        {
            //Movie_SetYUV(UI_GetData(FL_MOVIE_SIZE));
        }

        MediaRec_SetTestMode(&g_MediaRecTestMode);
    }
    #else // real sensor
    {
        SENSOR_INIT_OBJ SenObj;
        SENSOR_DRVTAB *SenTab;
        //open sensor
        SenObj = DrvSensor_GetObj1st();
        SenTab = DrvSensor_GetTab1st();
        Sensor_Open(SENSOR_ID_1, &SenObj, SenTab);

#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
        if (Sensor_CheckExtSensor())
        {
            SENSOR_INIT_OBJ SenObj;
            SENSOR_DRVTAB *SenTab;

            Movie_SetDualRec(ON);
            SenObj = DrvSensor_GetObj2nd();
            SenTab = DrvSensor_GetTab2nd();
            Sensor_Open(SENSOR_ID_2, &SenObj, SenTab);
            g_bSensor2En = TRUE;
        }
        else
        {
            // forced disable dual recording if no 2nd sensor
            Movie_SetDualRec(OFF);
            g_bSensor2En = FALSE;
        }
#endif
    }

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        switch (UI_GetData(FL_MOVIE_SIZE_DUAL))
        {
            case MOVIE_SIZE_1080P_720P:
            case MOVIE_SIZE_720P_720P:
            default:
                CropRatio = IPL_SENCROPRATIO_16_9;
                ImgRatio.w = 16;
                ImgRatio.h = 9;
                break;
        }
    }
    else
    {
        switch (UI_GetData(FL_MOVIE_SIZE))
        {
            case MOVIE_SIZE_VGA:
            case MOVIE_SIZE_QVGA:
                CropRatio = IPL_SENCROPRATIO_4_3;
                ImgRatio.w = 4;
                ImgRatio.h = 3;
                break;

            case MOVIE_SIZE_1080P_FULLRES:
            case MOVIE_SIZE_1080P:
            case MOVIE_SIZE_720P:
            //case MOVIE_SIZE_WVGA:
            default:
                CropRatio = IPL_SENCROPRATIO_16_9;
                ImgRatio.w = 16;
                ImgRatio.h = 9;
                break;
        }
    }
    DscMovie_Config(MOVIE_CFG_RATIO, CropRatio);

    //config view for primary display device
    MovieExe_CalcDispSize(&DispSize, &DeviceSize, &AspectSize, &ImgRatio);
    AppView_ConfigEnable(0, TRUE);
    AppView_ConfigDevice(0, DeviceSize.w, DeviceSize.h, AspectSize.w, AspectSize.h);
    AppView_ConfigBuffer(0, BufferSize.w, BufferSize.h, GX_IMAGE_PIXEL_FMT_YUV420_PACKED);
    AppView_ConfigWindow(0, 0, 0, DeviceSize.w, DeviceSize.h);
    AppView_ConfigImgSize(0, DispSize.w, DispSize.h, ImgRatio.w, ImgRatio.h);
    #if PIP_VIEW_FUNC
    PipView_SetPrimaryImgRatio((USIZE*)&ImgRatio);
    #endif

    //config view for secondary display device
    CurrDevObj = GxVideo_GetDevice(DOUT2);
    if (CurrDevObj)
    {
        DeviceSize = GxVideo_GetDeviceSize(DOUT2);
        BufferSize = DeviceSize;
        AspectSize = _QueryDeviceModeAspect(CurrDevObj, GxVideo_QueryDeviceLastMode(CurrDevObj));
        MovieExe_CalcDispSize(&DispSize, &DeviceSize, &AspectSize, &ImgRatio);

        AppView_ConfigEnable(1, TRUE);
        AppView_ConfigDevice(1, DeviceSize.w, DeviceSize.h, AspectSize.w, AspectSize.h);
        AppView_ConfigBuffer(1, BufferSize.w, BufferSize.h, GX_IMAGE_PIXEL_FMT_YUV420_PACKED);
        AppView_ConfigWindow(1, 0, 0, DeviceSize.w, DeviceSize.h);
        AppView_ConfigImgSize(1, DispSize.w, DispSize.h, ImgRatio.w, ImgRatio.h);
        DscMovie_Config(MOVIE_CFG_IMGPATH,MOVIE_DUAL_DISPLAY);
    }
    else
    {
        AppView_ConfigEnable(1, FALSE);
    }

    #if (UVC_RECORD_FUNC == ENABLE)
    {
        UvcRec_SetImgID(UVCREC_IMG_ID_2);
        PUVCREC_IMG_DATA pUvcImg = UvcRec_GetImgData();

        AppView_ConfigEnable(1, TRUE);
        AppView_ConfigBuffer(1, pUvcImg->uiBufW, pUvcImg->uiBufH, GX_IMAGE_PIXEL_FMT_YUV422_PACKED);

        if(CropRatio == IPL_SENCROPRATIO_16_9)
        {
            AppView_ConfigImgSize(1, pUvcImg->uiImgW_16_9, pUvcImg->uiImgH_16_9, pUvcImg->uiImgW_16_9, pUvcImg->uiImgH_16_9);
            UVAC_SetImageSize(pUvcImg->uiUvcSizeId_16_9);
        }
        else
        {
            AppView_ConfigImgSize(1, pUvcImg->uiImgW_4_3, pUvcImg->uiImgH_4_3, pUvcImg->uiImgW_4_3, pUvcImg->uiImgH_4_3);
            UVAC_SetImageSize(pUvcImg->uiUvcSizeId_4_3);
        }

        DscMovie_Config(MOVIE_CFG_IMGPATH, MOVIE_UVC_RECORD);
    }
    #endif

    //config src
    #if PIP_VIEW_FUNC
    gMovie_src.bDirectMode = FALSE;
    DscPhoto_RegChgDispSizeInforCB(PipView_ActivePrimaryImgRatio);
    #endif

    //#NT#2014/10/30#KS Hung -begin
    if (IsRotatePanel == TRUE)
    {
        if (System_IsPlugTVHDMI() == TRUE)
    {
            IsRotatePanel=FALSE; // plug in TV or HDMI and current display object is not a rotate panel.
            #if !PIP_VIEW_FUNC
            gMovie_src.bDirectMode = TRUE;
            #endif			
    }
        else
    {
        gMovie_src.bDirectMode = FALSE;
    }
    }
    //#NT#2014/10/30#KS Hung -end

    AppView_ConfigSource(&gMovie_src);
    //open
    AppView_Open();
    #if PIP_VIEW_FUNC
    {
        DISPSRV_CMD Cmd={0};
        DISPSRV_FP_DRAW_CB fpOnDraw = PipView_OnDraw;
        Cmd.Idx = DISPSRV_CMD_IDX_SET_DRAW_CB;
        Cmd.In.pData = &fpOnDraw;
        Cmd.In.uiNumByte = sizeof(fpOnDraw);
        Cmd.Prop.bExitCmdFinish = TRUE;
        DispSrv_Cmd(&Cmd);
    }
    #else
    if (IsRotatePanel==TRUE)
    {
        DISPSRV_CMD Cmd={0};
        DISPSRV_FP_DRAW_CB fpOnDraw = MovieExe_OnDraw;
        Cmd.Idx = DISPSRV_CMD_IDX_SET_DRAW_CB;
        Cmd.In.pData = &fpOnDraw;
        Cmd.In.uiNumByte = sizeof(fpOnDraw);
        Cmd.Prop.bExitCmdFinish = TRUE;
        DispSrv_Cmd(&Cmd);
    }
    #endif

    #endif // MOVIE_TEST_ENABLE
    Movie_SetRecParam();

    // DIS can only be enabled as RSC off
    if (UI_GetData(FL_MovieDisIndex) == MOVIE_DIS_ON &&
        UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_OFF)
    {
        IPL_AlgSetUIInfo(IPL_SEL_DISCOMP, SEL_DISCOMP_ON);
    }
    else
    {
        UI_SetData(FL_MovieDisIndex, MOVIE_DIS_OFF);
        IPL_AlgSetUIInfo(IPL_SEL_DISCOMP, SEL_DISCOMP_OFF);
    }

    //#NT#2013/02/27#Lincy Lin -begin
    //#NT#For capture in movie write file naming

    #if USE_FILEDB
    UI_SetData(FL_IsUseFileDB, 1);
    #else
    UI_SetData(FL_IsUseFileDB, 0);
    #endif
    useFileDB = UI_GetData(FL_IsUseFileDB);
    if (useFileDB)
    {
        //DscPhoto_Config(PHOTO_CFG_USE_FILEDB, 1);
        //Pool.Addr = OS_GetMempoolAddr(POOL_ID_FILEDB);
        //Pool.Size = POOL_SIZE_FILEDB;
        //DscPhoto_Config(PHOTO_CFG_FILEDB_MEM, (UINT32)&Pool);

        DscMovie_Config(MOVIE_CFG_USE_FILEDB, 1);
        Pool.Addr = OS_GetMempoolAddr(POOL_ID_FILEDB);
        Pool.Size = POOL_SIZE_FILEDB;
        DscMovie_Config(MOVIE_CFG_FILEDB_MEM, (UINT32)&Pool);
        DscPhoto_Config(PHOTO_CFG_USE_FILEDB, 1);
        // config FDB root
        DscMovie_SetFDBFolder(FILEDB_CARDV_ROOT);
        DscMovie_SetFDB_MovieFolder("MOVIE");
        DscMovie_SetFDB_ROFolder("MOVIE\\RO");
        DscPhoto_SetFDBFolder(FILEDB_CARDV_ROOT);
        DscPhoto_SetFDBPhotoFolder("PHOTO");
        //sample
        //DscMovie_SetFDBFolder("CARDV");
        //DscMovie_SetFDB_MovieFolder("MOVIE");
        //DscMovie_SetFDB_ROFolder("MOVIE\\RO");
        //set photo to RawEncTsk
        //DscPhoto_SetFDBFolder("CARDV");
        //DscPhoto_SetFDBPhotoFolder("PHOTO");
    }
    else
    {
        DscMovie_Config(MOVIE_CFG_USE_FILEDB, 0);
        DscPhoto_Config(PHOTO_CFG_USE_FILEDB, 0);
    }
    //#NT#2013/02/27#Lincy Lin -end

    AF_RegisterCB(UIMovie_AFCB);                    // register AF callback function
    AFTsk_RegisterCAF((AF_PROC_CAF)caf_AutoFocusProcess);   // register CAF function
    DscMovie_RegDrawCB(MovieStamp_CopyData);        // register movie stamp callback function
    DscMovie_RegAudFilterCB(UIMovie_AudFilterProc); // register audio filtering callback function
    DscMovie_RegImmProcCB(UIMovie_ImmProc);         // register instant events processing callback function
#if (MOVIE_TEST_ENABLE == ENABLE)
    DscMovie_RegIPLChangeCB(MovieExe_FakeIPLChangeCB);
    DscMovie_RegGetReadyBufCB(MovieExe_FakeGetReadyBufCB);
    DscMovie_RegGetReadyBufCB2(MovieExe_FakeGetReadyBufCB2);
    DscMovie_Config(MOVIE_CFG_FIXED_YUV, TRUE);
#endif
    //#NT#2015/01/21#KS Hung -begin
    //#NT#Fast boot recording.
    #if (_FAST_BOOT_REC_FUNC_)
    if (FlowMovie_GetFirstBootRecFlag())
    {
        Movie_FDB_SetNOTInit(1);
    }
    else
    {
        Movie_FDB_SetNOTInit(0);
    }
    #endif
	
    IPL_Display_InitSkipFrame(4);   //jacky lan 
    IPL_Display_InitSkipFrame2(2);	//jacky lan 
	
    //#NT#2015/01/21#KS Hung -end
    DscMovie_Open();

    #if (UI_STYLE==UI_STYLE_DRIVER)
    #if(_GYRO_EXT_==_GYRO_EXT_NONE_)
    IPL_AlgSetUIInfo(IPL_SEL_3DNR, SEL_3DNR_ON);
    #else
    IPL_AlgSetUIInfo(IPL_SEL_3DNR, SEL_3DNR_OFF);
    //C H Lin - RSC_Modification: FOV flow
    if (SysGetFlag(FL_MovieRSCIndex)==MOVIE_RSC_ON)
        IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_ON);
    #endif
    //IPL_AlgSetUIInfo(IPL_SEL_IMAGEEFFECT, SEL_IMAGEEFFECT_WDR);
    IPL_AlgSetUIInfo(IPL_SEL_WDR, SEL_WDR_LV6);
    #else
    // disable IME 3DNR since it will conflict with RSC
    IPL_AlgSetUIInfo(IPL_SEL_3DNR, SEL_3DNR_OFF);
    #endif

    #if (UVC_RECORD_FUNC == ENABLE)
    UvcRec_Open();
    #endif

    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UvcRec_EnStopVideoCB(FALSE);    // disable "stop video callback" (do not set UVC active to FALSE)
        UVAC_Suspend();                 // suspend UVC to avoid seeing garbage as changing sensor mode
    }
    #endif

    DscMovie_Close();
    AppView_Close();
    {
      DISPSRV_CMD DispCmd= {0};

      memset(&DispCmd,0,sizeof(DISPSRV_CMD));
      //show last frame of live view
      DispCmd.Idx = DISPSRV_CMD_IDX_DIRECT_FRM_TO_TEMP;
      DispCmd.Prop.bExitCmdFinish=TRUE;
      DispSrv_Cmd(&DispCmd);
      //SwTimer_DelayMs(5000);
    }
    AppView_ConfigSource(NULL); // set AppView source to NULL!

#if (MOVIE_TEST_ENABLE == DISABLE)
    Sensor_Close(SENSOR_ID_1);
#if  (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
    if (g_bSensor2En)
    {
        Sensor_Close(SENSOR_ID_2);
        g_bSensor2En = FALSE;
    }
#endif
#endif

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMovieSize(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;
    UINT32 uiQuality;
    UINT32 uiFrameRate;

    if(paramNum > 0)
        uiSelect = paramArray[0];

    uiQuality = UI_GetData(FL_MOVIE_QUALITY);
    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        UI_SetData(FL_MOVIE_SIZE_DUAL, uiSelect);
        uiFrameRate = GetMovieFrameRate_2p(0, uiSelect);
    }
    else
    {
        UI_SetData(FL_MOVIE_SIZE, uiSelect);
        uiFrameRate = GetMovieFrameRate(uiSelect);
    }

    // need to set media record parameters in order to calculate remaining time
    Movie_SetRecParam();

    // enable RSC if necessary (only support frame rate smaller than 30fps)
    if ((UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_ON) && (uiFrameRate <= 30))
    {
        DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_ON);
    }
    else
    {
        DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_OFF);
    }

    // set IPL parameters
    if (uiSelect == MOVIE_SIZE_1080P_FULLRES) // for full resolution input 1080p30
    {
        IPL_AlgSetUIInfo(IPL_SEL_VIDEO_FPS,SEL_VIDEOFPS_FULLRES);
    }
    else
    {
        switch (uiFrameRate)
        {
        case 60: // 60fps
            IPL_AlgSetUIInfo(IPL_SEL_VIDEO_FPS,SEL_VIDEOFPS_60);
            break;

        case 120: // 120fps
            IPL_AlgSetUIInfo(IPL_SEL_VIDEO_FPS,SEL_VIDEOFPS_120);
            break;

        default: // 30fps
            IPL_AlgSetUIInfo(IPL_SEL_VIDEO_FPS,SEL_VIDEOFPS_30);
            break;
        }
    }

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnImageRatio(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    IPL_SENCROPRATIO CropRatio;
    UINT32  uiSelect = 0;
    UINT32  CurrDevObj;
    ISIZE   DispSize = {0};
    ISIZE   ImgRatio = {0};
    ISIZE   DeviceSize = {0};
    ISIZE   DeviceRatio = {0};

    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UvcRec_EnStopVideoCB(FALSE);    // disable "stop video callback" (do not set UVC active to FALSE)
        UVAC_Suspend();                 // suspend UVC to avoid seeing garbage as changing sensor mode
    }
    #endif

    if (paramNum > 0)
        uiSelect = paramArray[0]; // parameter 0: image size

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        switch (uiSelect)
        {
        case MOVIE_SIZE_1080P_720P:
        case MOVIE_SIZE_720P_720P:
        default:
            CropRatio = IPL_SENCROPRATIO_16_9;
            ImgRatio.w = 16;
            ImgRatio.h = 9;
            break;
        }
    }
    else
    {
        switch (uiSelect)
        {
        case MOVIE_SIZE_VGA:
        case MOVIE_SIZE_QVGA:
            CropRatio = IPL_SENCROPRATIO_4_3;
            ImgRatio.w = 4;
            ImgRatio.h = 3;
            break;

        case MOVIE_SIZE_1080P_FULLRES:
        case MOVIE_SIZE_1080P:
        case MOVIE_SIZE_720P:
        //case MOVIE_SIZE_WVGA:
        default:
            CropRatio = IPL_SENCROPRATIO_16_9;
            ImgRatio.w = 16;
            ImgRatio.h = 9;
            break;
        }
    }
    DscMovie_Config(MOVIE_CFG_RATIO, CropRatio);

#if (MOVIE_TEST_ENABLE != ENABLE)
    CurrDevObj = GxVideo_GetDevice(DOUT1);
    DeviceSize = GxVideo_GetDeviceSize(DOUT1);
    DeviceRatio = _QueryDeviceModeAspect(CurrDevObj, GxVideo_QueryDeviceLastMode(CurrDevObj));
    MovieExe_CalcDispSize(&DispSize, &DeviceSize, &DeviceRatio, &ImgRatio);
    AppView_ConfigImgSize(0, DispSize.w, DispSize.h, ImgRatio.w, ImgRatio.h);
    #if PIP_VIEW_FUNC
    PipView_SetPrimaryImgRatio((USIZE*)&ImgRatio);
    #endif

    //2. Set IPL setting
    g_ImeInfo.Id = IPL_ID_1;
    IPL_GetCmd(IPL_GET_IME_INFOR, (void *)&g_ImeInfo);

    g_ImgRatio.Id = IPL_ID_1;
    g_ImgRatio.PathId = IPL_IME_PATH_NONE;
    g_ImgRatio.PathId |= IPL_IME_PATH2;
    g_ImgRatio.CropRatio = CropRatio;
    g_ImgRatio.ImeP2.OutputEn = TRUE;
    g_ImgRatio.ImeP2.ImgSizeH = DispSize.w;
    g_ImgRatio.ImeP2.ImgSizeV = DispSize.h;
    g_ImgRatio.ImeP2.ImgSizeLOfs = DispSize.w;
    g_ImgRatio.ImeP2.ImgFmt = g_ImeInfo.ImeP2[0].type;

    CurrDevObj = GxVideo_GetDevice(DOUT2);
    if (CurrDevObj)
    {
        DeviceSize = GxVideo_GetDeviceSize(DOUT2);
        DeviceRatio = _QueryDeviceModeAspect(CurrDevObj, GxVideo_QueryDeviceLastMode(CurrDevObj));
        MovieExe_CalcDispSize(&DispSize, &DeviceSize, &DeviceRatio, &ImgRatio);
        AppView_ConfigImgSize(1, DispSize.w, DispSize.h, ImgRatio.w, ImgRatio.h);

        g_ImgRatio.PathId |= IPL_IME_PATH3;
        g_ImgRatio.ImeP3.OutputEn = TRUE;
        g_ImgRatio.ImeP3.ImgSizeH = DispSize.w;
        g_ImgRatio.ImeP3.ImgSizeV = DispSize.h;
        g_ImgRatio.ImeP3.ImgSizeLOfs = DispSize.w;
        g_ImgRatio.ImeP3.ImgFmt = g_ImeInfo.ImeP3[0].type;
    }

    #if (UVC_RECORD_FUNC == ENABLE)
    {
        PUVCREC_IMG_DATA pUvcImg = UvcRec_GetImgData();

        g_ImgRatio.PathId |= IPL_IME_PATH3;
        g_ImgRatio.ImeP3.OutputEn = TRUE;
        g_ImgRatio.ImeP3.ImgFmt = IPL_IMG_Y_PACK_UV422;

        if(CropRatio == IPL_SENCROPRATIO_16_9)
        {
            g_ImgRatio.ImeP3.ImgSizeH = pUvcImg->uiImgW_16_9;
            g_ImgRatio.ImeP3.ImgSizeV = pUvcImg->uiImgH_16_9;
            g_ImgRatio.ImeP3.ImgSizeLOfs = pUvcImg->uiImgW_16_9;
            AppView_ConfigImgSize(1, pUvcImg->uiImgW_16_9, pUvcImg->uiImgH_16_9, pUvcImg->uiImgW_16_9, pUvcImg->uiImgH_16_9);
            UVAC_SetImageSize(pUvcImg->uiUvcSizeId_16_9);
        }
        else
        {
            g_ImgRatio.ImeP3.ImgSizeH = pUvcImg->uiImgW_4_3;
            g_ImgRatio.ImeP3.ImgSizeV = pUvcImg->uiImgH_4_3;
            g_ImgRatio.ImeP3.ImgSizeLOfs = pUvcImg->uiImgW_4_3;
            AppView_ConfigImgSize(1, pUvcImg->uiImgW_4_3, pUvcImg->uiImgH_4_3, pUvcImg->uiImgW_4_3, pUvcImg->uiImgH_4_3);
            UVAC_SetImageSize(pUvcImg->uiUvcSizeId_4_3);
        }
    }
    #endif

    IPL_SetCmd(IPL_SET_IMGRATIO, (void *)&g_ImgRatio);
    IPL_WaitCmdFinish();

    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UVAC_Resume();                  // resume UVC
    }
    #endif
#endif

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMovieQuality(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiQuality = 0;
    UINT32 uiSize;

    if(paramNum > 0)
        uiQuality = paramArray[0];

    UI_SetData(FL_MOVIE_QUALITY, uiQuality);
    uiSize = UI_GetData(FL_MOVIE_SIZE);

    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_TARGETRATE, GetMovieTargetBitrate(uiSize, uiQuality), 0, 0);

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnWB(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_WB, uiSelect);
    IPL_AlgSetUIInfo(IPL_SEL_WBMODE, Get_WBValue(uiSelect));

    return NVTEVT_CONSUME;
}

//#NT#2011/04/15#Photon Lin -begin
//#NT#Add AV sync mechanism
INT32 MovieExe_OnAVSyncCnt(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum > 0)
        uhSelect = paramArray[0];
#if _DEMO_TODO
    DbgMsg_UIMovExeIO(("+-MovieExe_OnAVSyncCnt:idx=%d,val=%d\r\n",uhSelect,GetMovieQualityValue(uhSelect)));

    UIMovRecObj_SetData(RECMOVIE_AYSYNCCNT, uhSelect);
#endif
    return NVTEVT_CONSUME;
}
//#NT#2011/04/15#Photon Lin -end

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnColor(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_COLOR_EFFECT, uiSelect);
    IPL_AlgSetUIInfo(IPL_SEL_IMAGEEFFECT, Get_ColorValue(uiSelect));

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnEV(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_EV, uiSelect);
    IPL_AlgSetUIInfo(IPL_SEL_AEEV, Get_EVValue(uiSelect));

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMovieAudio(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_AUDIO, uiSelect);

    if (uiSelect == MOVIE_AUDIO_OFF)
    {
        UIMovRecObj_SetData(RECMOVIE_AUD_VOLUME, RECMOVIE_AUD_OFF);
    }
    else
    {
        UIMovRecObj_SetData(RECMOVIE_AUD_VOLUME, RECMOVIE_AUD_ON);
    }

    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnAudPlayVolume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum > 0)
        uhSelect = paramArray[0];
    DbgMsg_UIMovExeIO(("+-MovieExe_OnAudPlayVolume:idx=%d,val=%d\r\n",uhSelect,GetMovieAudioVolumeValue(uhSelect)));
    //if (UI_GetData(FL_MovieAudioPlayIndex) != uhSelect)
    {
        UI_SetData(FL_MovieAudioPlayIndex,uhSelect);
        //#NT#2012/09/19#Calvin Chang#Porting Media Play Demo1 Flow -begin
        UIMovObj_SetAudPlayVolume(GetMovieAudioVolumeValue(uhSelect));
//#if _DEMO_TODO
//        UIMovObj_SetDataPlayMovie(PLAYMOVIE_AUD_VOLUME, GetMovieAudioVolumeValue(uhSelect));
//#endif
        //#NT#2012/09/19#Calvin Chang -end
    }
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnAudRecVolume(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum > 0)
        uhSelect = paramArray[0];
    DbgMsg_UIMovExeIO(("+-MovieExe_OnAudRecVolume:idx=%d\r\n",uhSelect));
    if (UI_GetData(FL_MovieAudioRecIndex) != uhSelect)
    {
        UI_SetData(FL_MovieAudioRecIndex,uhSelect);
    }
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnDigitalZoom(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum>0)
        uhSelect= paramArray[0];
    DbgMsg_UIMovExeIO(("+-MovieExe_OnDigitalZoom:idx=%d\r\n",uhSelect));
    UI_SetData(FL_Dzoom, uhSelect);
    if (uhSelect == DZOOM_OFF)
    {
        UI_SetData(FL_DzoomIndex, DZOOM_10X);
    }
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnContAF(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum>0)
        uhSelect= paramArray[0];

    UI_SetData(FL_MovieContAFIndex,uhSelect);
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMetering(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum>0)
        uhSelect= paramArray[0];

    UI_SetData(FL_MovieMeteringIndex,uhSelect);
#if _DEMO_TODO
    //set to photo obj
    AppPhoto_SetData(&UIPhotoObjCtrl,_AEMode,Get_MeteringValue(uhSelect));
#endif
    IPL_AlgSetUIInfo(IPL_SEL_AEMODE,Get_MeteringValue(uhSelect));
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnDis(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
    {
        uiSelect = paramArray[0];
    }

    DBG_IND("idx=%d\r\n", uiSelect);

    UI_SetData(FL_MovieDisIndex, uiSelect);

    if (UI_GetData(FL_ModeIndex) == DSC_MODE_MOVIE)
    {
        if (uiSelect == MOVIE_DIS_ON)
        {
            // disable RSC
            UI_SetData(FL_MovieRSCIndex, MOVIE_RSC_OFF);
            DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_OFF);
            IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_OFF);

            // enable DIS
            IPL_AlgSetUIInfo(IPL_SEL_DISCOMP, SEL_DISCOMP_ON);
        }
        else
        {
            IPL_AlgSetUIInfo(IPL_SEL_DISCOMP, SEL_DISCOMP_OFF);
        }
    }
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnMCTF(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MovieMCTFIndex, uiSelect);

    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnRSC(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;
    UINT32 uiFrameRate;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MovieRSCIndex, uiSelect);

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        uiFrameRate = GetMovieFrameRate_2p(0, UI_GetData(FL_MOVIE_SIZE));
    }
    else
    {
        uiFrameRate = GetMovieFrameRate(UI_GetData(FL_MOVIE_SIZE));
    }

    if (uiSelect == MOVIE_RSC_ON)
    {
        // disable DIS
        UI_SetData(FL_MovieDisIndex, MOVIE_DIS_OFF);
        IPL_AlgSetUIInfo(IPL_SEL_DISCOMP, SEL_DISCOMP_OFF);

        // enable RSC => move to IPL mode change to VIDEOREC
        //IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_ON);

        // enable RSC if necessary (only support frame rate smaller than 30fps)
        if ((UI_GetData(FL_MovieRSCIndex) == MOVIE_RSC_ON) && (uiFrameRate <= 30))
        {
            DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_ON);
        }
        else
        {
            DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_OFF);
        }
    }
    else
    {
        DscMovie_Config(MOVIE_CFG_RSC, SEL_RSC_OFF);
        IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_OFF);
    }

    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnHDR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    if (uiSelect == MOVIE_HDR_ON)
    {
        UI_SetData(FL_MOVIE_HDR, MOVIE_HDR_ON);
        IPL_AlgSetUIInfo(IPL_SEL_IMAGEEFFECT, SEL_IMAGEEFFECT_WDR);
    }
    else
    {
        UI_SetData(FL_MOVIE_HDR, MOVIE_HDR_OFF);
        IPL_AlgSetUIInfo(IPL_SEL_IMAGEEFFECT, SEL_IMAGEEFFECT_OFF);
    }

    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnGdc(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum>0)
        uhSelect= paramArray[0];
    DbgMsg_UIMovExeIO(("+-MovieExe_OnGdc:idx=%d\r\n",uhSelect));

    UI_SetData(FL_MovieGdcIndex,uhSelect);
#if(MOVIE_GDC_FUNC ==ENABLE)
    //#NT#2010/09/02#JeahYen -begin
    AppPhoto_SetData(&UIPhotoObjCtrl, _Prv2DGDC,
        (uhSelect==MOVIE_GDC_ON)?_Prv_2D_GDC_On:_Prv_2D_GDC_Off);
    //#NT#2010/09/02#JeahYen -end
#endif
    return NVTEVT_CONSUME;
}

INT32 MovieExe_OnSmear(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0;
    if(paramNum>0)
        uhSelect= paramArray[0];
    DbgMsg_UIMovExeIO(("+-MovieExe_OnSmear:idx=%d\r\n",uhSelect));

    UI_SetData(FL_MovieSmearIndex,uhSelect);
#if(MOVIE_SMEAR_R_FUNC ==ENABLE)
    AppPhoto_SetData(&UIPhotoObjCtrl, _SmearModeEn,
        (uhSelect==MOVIE_SMEAR_ON)?_SmearMode_on:_SmearMode_off);
#endif
    return NVTEVT_CONSUME;
}

//#NT#2010/04/14#Photon Lin -begin
//#Add DIS event
/*
1. Turn ON movie DIS: after IPL is ready, call
      PhotoDisplay_SetMode(DISPLAY_MODE_DISVIEW, TRUE);
      PhotoDis_setMode(PHOTODIS_MODE_START, TRUE);

2. Turn OFF movie DIS: before switching IPL off, call
      PhotoDis_setMode(PHOTODIS_MODE_IDLE, TRUE);
      PhotoDisplay_SetMode(DISPLAY_MODE_VIDEO, TRUE);
*/
INT32 MovieExe_OnDisEnable(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhSelect = 0xFFFFFFFF;
    if(paramNum>0)
        uhSelect= paramArray[0];

    DbgMsg_UIMovExeIO(("+-MovieExe_OnDisEnable:idx=%d\r\n",uhSelect));
    //if (UI_GetData(FL_MovieDisIndex) == MOVIE_DIS_ON)
    {
        //#NT#2010/12/16#Photon Lin -begin
        //#Enhance DIS flow
        UI_SetData(FL_MovieDisEnableIndex,uhSelect);
#if _DEMO_TODO
        //#NT#2010/12/16#Photon Lin -end
        if (uhSelect == 1)
        {
            //#NT#2011/04/15#Photon Lin -begin
            //#NT#Add AV sync mechanism
            if (UI_GetData(FL_MovieGdcIndex)==1)
            {
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEAVSYNC, 1, 4);
            }
            else
            {
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEAVSYNC, 1, 3);
            }
            //#NT#2011/04/15#Photon Lin -end
            //#NT#2010/12/15#Jay -begin
//            PhotoDisplay_SetMode(DISPLAY_MODE_DISVIEW, TRUE);
//            PhotoDis_setMode(PHOTODIS_MODE_START, TRUE);
            PhotoDisplay_SetMode(DISPLAY_MODE_DISVIEW, FALSE);
            PhotoDis_setMode(PHOTODIS_MODE_START, FALSE);
            //#NT#2010/12/15#Jay -end

        }
        else if (uhSelect == 0)
        {
            //#NT#2011/04/15#Photon Lin -begin
            //#NT#Add AV sync mechanism
            if (UI_GetData(FL_MovieGdcIndex)==1)
            {
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEAVSYNC, 1, 3);
            }
            else
            {
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEAVSYNC, 1, 2);
            }
            //#NT#2011/04/15#Photon Lin -end
            idec_setV1En(IDE_ID_1, FALSE);
            PhotoDis_setMode(PHOTODIS_MODE_IDLE, TRUE);
            PhotoDisplay_SetMode(DISPLAY_MODE_VIDEO, TRUE);
        }
#endif
    }
    return NVTEVT_CONSUME;
}
//#NT#2010/04/14#Photon Lin -end

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnDateImprint(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_DATEIMPRINT, uiSelect);

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnGSENSOR(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_GSENSOR, uiSelect);

    switch (UI_GetData(FL_MOVIE_GSENSOR))
    {
    case GSENSOR_OFF:
        GSensor_SetSensitivity(GSENSOR_SENSITIVITY_OFF);
        break;
    case GSENSOR_LOW:
        GSensor_SetSensitivity(GSENSOR_SENSITIVITY_LOW);
        break;
    case GSENSOR_MED:
        GSensor_SetSensitivity(GSENSOR_SENSITIVITY_MED);
        break;
    case GSENSOR_HIGH:
        GSensor_SetSensitivity(GSENSOR_SENSITIVITY_HIGH);
        break;
    default:
        GSensor_SetSensitivity(GSENSOR_SENSITIVITY_OFF);
        break;
    }

    //debug_err(("MovieExe_OnGSENSOR =%d\r\n ",FL_MOVIE_GSENSOR));
    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnDualRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_DUAL_REC, uiSelect);

    // TO DO: set IPL
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
    // forced set dual recording to FALSE if no 2nd sensor
    if (Sensor_CheckExtSensor() == FALSE)
    {
        UI_SetData(FL_MOVIE_DUAL_REC, FALSE);
    }
    else
    {
        if (uiSelect) // enable dual recording
        {
            IPL_Display_OpenTsk2();
            IPL_ResetDisplayCtrlFlow2();
        }
        else // disable dual recording
        {
            IPL_Display_CloseTsk2();
        }
    }
#endif

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
UINT32 Movie_GetCyclicRecTime(void)
{
    UINT32 uiCyclicRecTime = 0;

    switch (UI_GetData(FL_MOVIE_CYCLIC_REC))
    {
    case MOVIE_CYCLICREC_3MIN:
        uiCyclicRecTime = 180;
        break;

    case MOVIE_CYCLICREC_5MIN:
        uiCyclicRecTime = 300;
        break;

    case MOVIE_CYCLICREC_10MIN:
        uiCyclicRecTime = 600;
        break;

    case MOVIE_CYCLICREC_OFF:
        break;

    default:
        uiCyclicRecTime = 300;
        break;
    }

    return uiCyclicRecTime;
}

INT32 MovieExe_OnCyclicRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;
    UINT32 uiCyclicRecTime;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_CYCLIC_REC, uiSelect);

    uiCyclicRecTime = Movie_GetCyclicRecTime();

    if (uiSelect != MOVIE_CYCLICREC_OFF)
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_CUTSEC, uiCyclicRecTime, 0, 0);
    }
    else
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_CUTSEC, 1500, 0, 0); // 25 min
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_ENDTYPE, MEDIAREC_ENDTYPE_CUT_TILLCARDFULL, 0, 0);
    }

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnMotionDet(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_MOTION_DET, uiSelect);

    return NVTEVT_CONSUME;
}

//#NT2014/03/28# Sun chen for AR0330 high temperature issue start
INT32 MovieExe_OnSenOB(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;
    SENSOR_CMD SenCmd;

    if (paramNum)
        uiSelect = paramArray[0];

    SenCmd.uiAddr = 0x3180;  //Bit13:Trig OB to auto calibration
    SenCmd.uiDataLen = 2;
    SenCmd.uiData[0] = 0x20F0;
    SenCmd.uiData[1] = 0;

    Sensor_ReadReg(0x00, &SenCmd);
    SenCmd.uiData[0] |= 0x2000;
    Sensor_WriteReg(0x00, &SenCmd);

    return NVTEVT_CONSUME;
}
//#NT2014/03/28# Sun chen for AR0330 high temperature issue end

//#NT#2010/11/25#Photon Lin -begin
//#Add feature of movie key
INT32 MovieExe_OnAutoRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uhParam = 0xFFFFFFFF;
    UINT32 uhLastMode = 0xFFFFFFFF;
    if(paramNum>0)
    {
        uhParam = paramArray[0];
        uhLastMode = paramArray[1];
        DbgMsg_UIMovExeIO(("+-MovieExe_OnAutoRec:idx=%d\r\n",uhParam));

        UI_SetData(FL_MovieAutoRecIndex, uhParam);
        UI_SetData(FL_MovieAutoRecLastMode, uhLastMode);

    }
    else
    {
        DBG_ERR("parameters error\r\n");
    }

    return NVTEVT_CONSUME;
}
//#NT#2010/11/25#Photon Lin -end
//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
INT32 MovieExe_OnFastBootRec(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if (_FAST_BOOT_REC_FUNC_)
    Movie_FDB_FastInit();

    UIFlowWndMovie_OnFastBootRecord(NULL, 0, NULL);
#endif
    return NVTEVT_CONSUME;
}
//#NT#2015/01/21#KS Hung -end

//#NT#2012/07/31#Hideo Lin -begin
//-------------------------------------------------------------------------------------------------
void Movie_SetQPInitLevel(void)
{
    UINT32  uiQPLevel = H264_QP_LEVEL_3;

    // To do: set H.264 QP initial level according to ISO value

    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H264QPLEVEL, uiQPLevel, 0, 0);
}

#if (UI_STYLE==UI_STYLE_DRIVER)
extern GPSDATA    gpsdata;
#endif
INT32 MovieExe_OnRecStart(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32  uiWidth, uiHeight, uiMovieSize;
    UINT32  uiUserDataAddr, uiUserDataSize = 0;
    UINT32  uiWidth2 = 0, uiHeight2 = 0;

    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UvcRec_EnStartVideoCB(FALSE);   // disable "start video callback"
        UvcRec_EnStopVideoCB(FALSE);    // disable "stop video callback" (do not set UVC active to FALSE)
        UVAC_Suspend();                 // suspend UVC to avoid seeing garbage as changing sensor mode
    }
    #endif

    if (SysGetFlag(FL_MOVIE_DUAL_REC))
    {
        uiMovieSize = UI_GetData(FL_MOVIE_SIZE_DUAL);           // movie size
        uiWidth     = GetMovieSizeWidth_2p(0, uiMovieSize);     // image width
        uiHeight    = GetMovieSizeHeight_2p(0, uiMovieSize);    // image height
        uiWidth2    = GetMovieSizeWidth_2p(1, uiMovieSize);     // image width
        uiHeight2   = GetMovieSizeHeight_2p(1, uiMovieSize);    // image height
    }
    else
    {
        uiMovieSize = UI_GetData(FL_MOVIE_SIZE);        // movie size
        uiWidth     = GetMovieSizeWidth(uiMovieSize);   // image width
        uiHeight    = GetMovieSizeHeight(uiMovieSize);  // image height
    }

    //#NT#2013/05/15#Calvin Chang#Add customer's user data to MOV/MP4 file format -begin
    if(MovieUdta_MakeVendorUserData(&uiUserDataAddr, &uiUserDataSize))
    {
        // Enable custom user data
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_EN_CUSTOMUDATA, TRUE, 0, 0);
        // Set user data
        MediaRec_SetUserData((UINT32)uiUserDataAddr, uiUserDataSize);
    }
    //#NT#2013/05/15#Calvin Chang -end

    Movie_SetRecParam();
    Movie_SetQPInitLevel();

    #if (MOVIE_TEST_ENABLE == ENABLE)
    {// test codes for fixed YUV patterns mode (no IPL)!!!
        if (g_MediaRecTestMode.bEnable && g_MediaRecTestMode.uiVidSrcType == MEDIAREC_SRC_FIXED_YUV)
        {
            // set YUV information if using fixed YUV as video source
            Movie_SetYUV(uiMovieSize);
            MediaRec_SetTestMode(&g_MediaRecTestMode);

            // register callback
            MediaRec_SetIPLChangeCB(MovieExe_FakeIPLChangeCB);
        }
    }
    #endif

    #if (UI_STYLE==UI_STYLE_DRIVER)
    // for GPS data recording test
    MediaRec_ChangeParameter(MEDIAREC_RECPARAM_GPSON, TRUE, 0, 0);
    strcpy(gpsdata.IQVer,IQS_GetVerInfo());
    strcpy(gpsdata.IQBuildDate,IQS_GetBuildDate());
    MediaRec_SetGPSData((UINT32)&gpsdata, sizeof(GPSDATA));
    #endif

    //---------------------------------------------------------------------------------------------
    // setup movie date stamp if necessary
    //---------------------------------------------------------------------------------------------
    if (UI_GetData(FL_MOVIE_DATEIMPRINT) == MOVIE_DATEIMPRINT_ON)
    {
        UINT32      uiStampAddr;
        STAMP_COLOR StampColorBg = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp background color
        STAMP_COLOR StampColorFr = {RGB_GET_Y( 16,  16,  16), RGB_GET_U( 16,  16,  16), RGB_GET_V( 16,  16,  16)}; // date stamp frame color
        STAMP_COLOR StampColorFg = {RGB_GET_Y(224, 224, 192), RGB_GET_U(224, 224, 192), RGB_GET_V(224, 224, 192)}; // date stamp foreground color

        // use POOL_ID_DATEIMPRINT as movie data stamp buffer
        uiStampAddr = OS_GetMempoolAddr(POOL_ID_DATEIMPRINT);
        MovieStamp_SetDataAddr(0, uiStampAddr);

        MovieStamp_SetColor(0, &StampColorBg, &StampColorFr, &StampColorFg);

        MovieStamp_Setup(
            0,
            STAMP_ON |
            STAMP_AUTO |
            STAMP_DATE_TIME |
            STAMP_BOTTOM_RIGHT |
            STAMP_POS_NORMAL |
            STAMP_BG_TRANSPARENT |
            STAMP_YY_MM_DD |
            STAMP_IMG_420UV,
            uiWidth,
            uiHeight,
#if (WATERLOGO_FUNCTION == ENABLE)
            (WATERLOGO_BUFFER *)&g_WaterLogo);
#else
            NULL);
#endif

        if (SysGetFlag(FL_MOVIE_DUAL_REC))
        {
            MovieStamp_SetDataAddr(1, uiStampAddr + POOL_SIZE_DATEIMPRINT/2);

            MovieStamp_SetColor(1, &StampColorBg, &StampColorFr, &StampColorFg);

            MovieStamp_Setup(
                1,
                STAMP_ON |
                STAMP_AUTO |
                STAMP_DATE_TIME |
                STAMP_BOTTOM_RIGHT |
                STAMP_POS_NORMAL |
                STAMP_BG_TRANSPARENT |
                STAMP_YY_MM_DD |
#if  (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
                STAMP_IMG_422UV,
#else
                STAMP_IMG_420UV,
#endif
                uiWidth2,
                uiHeight2,
#if (WATERLOGO_FUNCTION == ENABLE)
                (WATERLOGO_BUFFER *)&g_WaterLogo);
#else
                NULL);
#endif
        }
    }
    else
    {
        MovieStamp_Setup(
            0,
            STAMP_OFF,
            uiWidth,
            uiHeight,
            NULL);

        MovieStamp_Setup(
            1,
            STAMP_OFF,
            uiWidth2,
            uiHeight2,
            NULL);
    }

    // send command to UIMovieRecObj to start movie recording
    Ux_SendEvent(&UIMovieRecObjCtrl, NVTEVT_START_REC_MOVIE, 0);

    //#NT#2012/10/30#Calvin Chang#Generate Audio NR pattern by noises of zoom, focus and iris -begin
    #if (MOVIE_AUDIO_NR_PATTERN_ENABLE == ENABLE)
    BKG_PostEvent(NVTEVT_BKW_ANR_LENS_ACTION);
    #endif
    //#NT#2012/10/30#Calvin Chang -end

    //#NT#2012/10/23#Philex Lin - begin
    // disable auto power off/USB detect timer
    #if (UI_STYLE==UI_STYLE_DRIVER)
    KeyScan_EnableMisc(FALSE);
    #endif
    //#NT#2012/10/23#Philex Lin - end

    //#NT#2014/01/14#Spark Chou - begin
	//remove 3dnr dirty frame
    UIMovie_3DNRReset();
    //#NT#2014/01/14#Spark Chou - end

    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UVAC_Resume();                  // resume UVC
    }
    #endif

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
static UINT32 g_uiDoPIM = FALSE;
INT32 MovieExe_OnRecStop(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UVAC_Suspend();                 // suspend UVC to avoid seeing garbage as changing sensor mode
    }
    #endif

    // send command to UIMovieRecObj to stop movie recording
    Ux_SendEvent(&UIMovieRecObjCtrl, NVTEVT_STOP_REC_MOVIE, 0);
    //#NT#2012/10/23#Philex Lin - begin
    // Enable auto power off/USB detect timer
    #if (UI_STYLE==UI_STYLE_DRIVER)
    KeyScan_EnableMisc(TRUE);
    #endif
    //#NT#2012/10/23#Philex Lin - end

    if (g_uiDoPIM)
    {
        DscMovie_CloseIPL();
        ImgCap_StopCapture();
        ImgCap_WaitFinish();
        ImgCap_Close();
        DscMovie_OpenIPL();
        g_uiDoPIM = FALSE;
    }

    #if (UVC_RECORD_FUNC == ENABLE)
    if (UvcRec_IsUvcActive())           // if UVC is active
    {
        UVAC_Resume();                  // resume UVC
        UvcRec_EnStartVideoCB(TRUE);    // enable "start video callback"
    }
    #endif

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnRecPIM(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    // do picture in movie (capture full size still image as movie recording; still image will be processed after recording end)
    DBG_IND("Capture full size image!\r\n");
    g_uiDoPIM = TRUE;
    ImgCap_SetUIInfo(CAP_SEL_DRIVEMODE, SEL_DRIVEMODE_INVID);
    ImgCap_SetUIInfo(CAP_SEL_PICNUM, 1);
    ImgCap_SetUIInfo(CAP_SEL_FILEFMT, SEL_FILEFMT_JPG);
    ImgCap_SetUIInfo(CAP_SEL_JPGFMT, SEL_JPGFMT_422);
    ImgCap_SetUIInfo(CAP_SEL_JPG_IMG_H_SIZE, GetPhotoSizeWidth(0));  // 0 is max still image size
    ImgCap_SetUIInfo(CAP_SEL_JPG_IMG_V_SIZE, GetPhotoSizeHeight(0)); // 0 is max still image size
    ImgCap_SetUIInfo(CAP_SEL_RAW_BUF_NUM, 2); // 2 raw image buffer with 12-bit resolution for 128MB DRAM
    ImgCap_SetUIInfo(CAP_SEL_JPG_BUF_NUM, 1); // 1 JPEG buffer
    ImgCap_StartCapture();

    return NVTEVT_CONSUME;
}

//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnRecRawEnc(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    #define MAX_ENC_PATH    2

    USIZE   JpegSize[MAX_ENC_PATH], SrcSize[MAX_ENC_PATH];
    USIZE   *pSize;
    UINT32  i, uiDAR[MAX_ENC_PATH], uiMovieSize, uiEncPath = 1;

    // do raw encode (capture still image as movie recording; still image size must be equal to or smaller than video size;
    // still image can be processed during movie recording)
    DBG_IND("Capture image!\r\n");

    if (paramNum)
    {
        pSize = (USIZE *)paramArray[0];
        JpegSize[0].w = pSize->w;
        JpegSize[0].h = pSize->h;

        if (paramNum > 1)
        {
            pSize = (USIZE *)paramArray[1];
            JpegSize[1].w = pSize->w;
            JpegSize[1].h = pSize->h;
        }
    }

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        #if 0
        // only support 1st sensor for raw encoding
        uiMovieSize = UI_GetData(FL_MOVIE_SIZE_DUAL);
        SrcSize[0].w = GetMovieSizeWidth_2p(0, uiMovieSize);
        SrcSize[0].h = GetMovieSizeHeight_2p(0, uiMovieSize);
        uiDAR[0] = GetMovieDispAspectRatio_2p(0, uiMovieSize);
        if (uiDAR[0] == VIDENC_DAR_16_9)
        {
            SrcSize[0].w = (SrcSize[0].h * 16) / 9;
        }
        #else
        // support 1st & 2nd path image capture as dual recording
        uiMovieSize = UI_GetData(FL_MOVIE_SIZE_DUAL);
        for (i = 0; i < MAX_ENC_PATH; i++)
        {
            SrcSize[i].w = GetMovieSizeWidth_2p(i, uiMovieSize);
            SrcSize[i].h = GetMovieSizeHeight_2p(i, uiMovieSize);
            uiDAR[i] = GetMovieDispAspectRatio_2p(i, uiMovieSize);
            if (uiDAR[i] == VIDENC_DAR_16_9)
            {
                SrcSize[i].w = (SrcSize[i].h * 16) / 9;
            }
        }
        uiEncPath = MAX_ENC_PATH;
        #endif
    }
    else
    {
        uiMovieSize = UI_GetData(FL_MOVIE_SIZE);
        SrcSize[0].w = GetMovieSizeWidth(uiMovieSize);
        SrcSize[0].h = GetMovieSizeHeight(uiMovieSize);
        uiDAR[0] = GetMovieDispAspectRatio(uiMovieSize);
        if (uiDAR[0] == VIDENC_DAR_16_9)
        {
            SrcSize[0].w = (SrcSize[0].h * 16) / 9;
        }
    }

    for (i = 0; i < uiEncPath; i++)
    {
        // limit JPEG width
        if (JpegSize[i].w > SrcSize[i].w)
        {
            JpegSize[i].w = SrcSize[i].w;
        }
        if (JpegSize[i].w < 320)
        {
            JpegSize[i].w = 320;
        }

        // limit JPEG height
        if (JpegSize[i].h > SrcSize[i].h)
        {
            JpegSize[i].h = SrcSize[i].h;
        }
        if (JpegSize[i].h < 240)
        {
            JpegSize[i].h = 240;
        }

        // set JPEG size to raw encode
        RawEnc_ChangeParameter(RAWENC_JPEG_WIDTH,  JpegSize[i].w, i, 0);
        RawEnc_ChangeParameter(RAWENC_JPEG_HEIGHT, JpegSize[i].h, i, 0);

        DBG_IND("RawEnc Path%d, w %d, h %d\r\n", i, JpegSize[i].w, JpegSize[i].h);
    }

    if (uiEncPath == MAX_ENC_PATH)
    {
        // support 1st & 2nd path image capture as dual recording
        RawEnc_TriggerEncodeWithType(RAW_FORMAT_ALLPATH, 0);
    }
    else
    {
        // only support 1st sensor for raw encoding
        RawEnc_TriggerEncodeWithType(RAW_FORMAT_ONEPATH, RAW_FORMAT_PATH1);
    }

    return NVTEVT_CONSUME;
}
//#NT#2012/07/31#Hideo Lin -end


//-------------------------------------------------------------------------------------------------
INT32 MovieExe_OnParkingMonitor(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UINT32 uiSelect = 0;

    if (paramNum)
        uiSelect = paramArray[0];

    UI_SetData(FL_MOVIE_PARKING, uiSelect);

    return NVTEVT_CONSUME;

}
//#NT#2010/10/05#Lily Kao -begin
//#Changeable Manufacture,Product names by project
INT32 MovieExe_UtilSetMovCompareCompManuName(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
#if _DEMO_TODO
    MOVUtil_SetCompareCompManu(gMovieCompManuStr, MOV_COMP_MANU_STRING_LEN);
    MOVUtil_SetCompareCompName(gMovieCompNameStr, MOV_COMP_NAME_STRING_LEN);
#endif
    return NVTEVT_CONSUME;
}
//#NT#2010/10/05#Lily Kao -end

void MovieExe_IPLGetReadyCB2_Sample(UINT32 mode, UINT32 param);
NVT_MEDIAOBJ gMediaobj;

ER MovieExe_SetGetReadyCB2_Sample(void)
{
    NVT_MEDIAOBJ *pMobj;

    pMobj = &gMediaobj;
    pMobj->GetReadyCb2 = MovieExe_IPLGetReadyCB2_Sample;
    DscMovie_SetGetReadyCb2(pMobj);
    return E_OK;
}

void MovieExe_IPLGetReadyCB2_Sample(UINT32 mode, UINT32 param)
{
    MEDIAREC_READYBUF_INFO readyInfo;
    IPL_IME_BUF_ADDR CurInfo;

    switch (mode)
    {
    case MEDIAREC_IPLGET_READYBUF:

        CurInfo.Id = IPL_ID_1;
        IPL_GetCmd(IPL_GET_IME_CUR_BUF_ADDR, (void *)&CurInfo);

        readyInfo.y = CurInfo.ImeP1.PixelAddr[0];
        readyInfo.cb = CurInfo.ImeP1.PixelAddr[1];
        //readyInfo.cr = CurInfo.ImeP1.PixelAddr[2];
        readyInfo.cr = CurInfo.ImeP1.PixelAddr[1]; // UV pack
        readyInfo.y_lot = CurInfo.ImeP1.Ch[0].LineOfs;
        readyInfo.uv_lot = CurInfo.ImeP1.Ch[1].LineOfs;
        readyInfo.uiBufID = CurInfo.ImeP1.PixelAddr[2]; // for IME ready buffer check
        if (readyInfo.y == 0)
        {
            DBG_DUMP("not ready..\r\n");
            return;
        }
        if (MediaRec_PutReadyBufWithID(1, &readyInfo)!= E_OK)
        {
            DBG_DUMP("not ready22..\r\n");
            return;
        }

        //DBG_IND("ImeP1: y 0x%x, cb 0x%x, cr 0x%x\r\n", CurInfo.ImeP1.PixelAddr[0], CurInfo.ImeP1.PixelAddr[1], CurInfo.ImeP1.PixelAddr[2]);
        MediaRec_GetReadyBufWithID(1, &readyInfo);
        MediaRec_SetReadyBufWithID(1, &readyInfo);

        //DBG_DUMP("^G gg22\r\n");
        break;

    default:
        break;
    }
}

//#NT#2013/10/29#Isiah Chang -begin
//#Implement YUV merge mode of recording func.
void FlowMovie_RecSetYUVMergeMode(BOOL bEnable)
{
    g_bMovieRecYUVMergeEnable = bEnable;
}

BOOL FlowMovie_RecGetYUVMergeMode(void)
{
    return g_bMovieRecYUVMergeEnable;
}

void FlowMovie_RecSetYUVMergeRecInterval(UINT32 uiSec)
{
    g_uiMovieRecYUVMergeInterval = uiSec;
}

UINT32 FlowMovie_RecGetYUVMergeRecInterval(void)
{
    return g_uiMovieRecYUVMergeInterval;
}

void FlowMovie_RecSetYUVMergeRecCounter(UINT32 uiCount)
{
    g_uiMovieRecYUVMergeCounter = uiCount;
}

UINT32 FlowMovie_RecGetYUVMergeRecCounter(void)
{
    return g_uiMovieRecYUVMergeCounter;
}

void FlowMovie_RecYUVMergeCounterInc(void)
{
    g_uiMovieRecYUVMergeCounter++;
}
//#NT#2013/10/29#Isiah Chang -end

////////////////////////////////////////////////////////////

EVENT_ENTRY CustomMovieObjCmdMap[] =
{
    {NVTEVT_EXE_OPEN,               MovieExe_OnOpen                     },
    {NVTEVT_EXE_CLOSE,              MovieExe_OnClose                    },
    {NVTEVT_EXE_MOVIESIZE,          MovieExe_OnMovieSize                },
    {NVTEVT_EXE_IMAGE_RATIO,        MovieExe_OnImageRatio               },
    {NVTEVT_EXE_MOVIEQUALITY,       MovieExe_OnMovieQuality             },
    {NVTEVT_EXE_WB,                 MovieExe_OnWB                       },
    //#NT#2011/04/15#Photon Lin -begin
    //#NT#Add AV sync mechanism
    {NVTEVT_EXE_MOVIEAVSYNC,        MovieExe_OnAVSyncCnt                },
    //#NT#2011/04/15#Photon Lin -end
    {NVTEVT_EXE_MOVIECOLOR,         MovieExe_OnColor                    },
    {NVTEVT_EXE_MOVIE_EV,           MovieExe_OnEV                       },
    {NVTEVT_EXE_MOVIE_AUDIO,        MovieExe_OnMovieAudio               },
    {NVTEVT_EXE_MOVIEAUDPLAYVOLUME, MovieExe_OnAudPlayVolume            },
    {NVTEVT_EXE_MOVIEAUDRECVOLUME,  MovieExe_OnAudRecVolume             },
    {NVTEVT_EXE_MOVIEDZOOM,         MovieExe_OnDigitalZoom              },
    {NVTEVT_EXE_MOVIECONTAF,        MovieExe_OnContAF                   },
    {NVTEVT_EXE_MOVIEMETERING,      MovieExe_OnMetering                 },
    {NVTEVT_EXE_MOVIEDIS,           MovieExe_OnDis                      },
    {NVTEVT_EXE_MOVIE_MCTF,         MovieExe_OnMCTF                     },
    {NVTEVT_EXE_MOVIE_RSC,          MovieExe_OnRSC                      },
    {NVTEVT_EXE_MOVIE_HDR,          MovieExe_OnHDR                      },
    {NVTEVT_EXE_MOVIEGDC,           MovieExe_OnGdc                      },
    {NVTEVT_EXE_MOVIESMEAR,         MovieExe_OnSmear                    },
    {NVTEVT_EXE_MOVIEDIS_ENABLE,    MovieExe_OnDisEnable                },
    {NVTEVT_EXE_MOVIE_DATE_IMPRINT, MovieExe_OnDateImprint              },
    {NVTEVT_EXE_DUAL_REC,           MovieExe_OnDualRec                  },
    {NVTEVT_EXE_CYCLIC_REC,         MovieExe_OnCyclicRec                },
    {NVTEVT_EXE_MOTION_DET,         MovieExe_OnMotionDet                },
    {NVTEVT_EXE_GSENSOR,            MovieExe_OnGSENSOR                  },
    {NVTEVT_EXE_MOVIEPARKING,       MovieExe_OnParkingMonitor},    
    
    //#NT#2010/11/16#Photon Lin -begin
    //#Add feature of movie key
    {NVTEVT_EXE_MOVIE_AUTO_REC,     MovieExe_OnAutoRec                  },
    //#NT#2010/11/16#Photon Lin -end
    {NVTEVT_EXE_MOVIE_REC_START,    MovieExe_OnRecStart                 },
    {NVTEVT_EXE_MOVIE_REC_STOP,     MovieExe_OnRecStop                  },
    {NVTEVT_EXE_MOVIE_REC_PIM,      MovieExe_OnRecPIM                   },
    {NVTEVT_EXE_MOVIE_REC_RAWENC,   MovieExe_OnRecRawEnc                },
    //#NT#2010/10/05#Lily Kao -begin
    //#Changeable Manufacture,Product names by project
    {NVTEVT_EXE_SET_MOV_MANUNAME,   MovieExe_UtilSetMovCompareCompManuName},
    //#NT#2010/10/05#Lily Kao -end
    {NVTEVT_EXE_SET_MOV_SENOB,      MovieExe_OnSenOB                    },//#NT2014/03/28# Sun chen for AR0330 high temperature issue
//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
    {NVTEVT_EXE_FASTBOOT_REC,       MovieExe_OnFastBootRec              },
//#NT#2015/01/21#KS Hung -end
    {NVTEVT_NULL,                   0},  //End of Command Map
};

CREATE_APP(CustomMovieObj,APP_SETUP)

