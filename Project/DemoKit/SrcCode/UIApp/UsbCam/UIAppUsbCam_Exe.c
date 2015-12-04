/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       AppInitUSB.c
    @ingroup    mIPRJAPCfg

    @brief      Appication initialization for USB mode
                Get memory and set parameters for USB mode

    @note       Nothing.

    @date       2005/12/09
*/

/** \addtogroup mIPRJAPCfg */
//@{

#include "SysCfg.h"
#include "UIAppUsbCam.h"
#include "GxSystem.h"
#include <string.h>
#include "AppLib.h"
#include "NvtAppPhotoApi.h"
#include "UIAppPhoto.h"
#include "PrjCfg.h"
#include "GxDisplay.h"
#include "Sensor.h"
#include "DxSensor.h"
#include "IPL_Cmd.h"
#include "IPL_Display.h"
#include "UIInfo.h"
#include "UIMovieInfo.h"

#define THIS_DBGLVL         1 //0=OFF, 1=ERROR, 2=TRACE
///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          UIApp_UsbCam
#define __DBGLVL__          ((THIS_DBGLVL>=PRJ_DBG_LVL)?THIS_DBGLVL:PRJ_DBG_LVL)
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////


#include "usb_desc.h"
#include "UVAC.h"
#define PCCAM_VIDEO_FORMAT          UVAC_VIDEO_FORMAT
#define PCCAM_VIDEO_FORMAT_422      UVAC_VIDEO_FORMAT_422
#define PCCAM_VIDEO_FORMAT_420      UVAC_VIDEO_FORMAT_420
#define UVAC_HD_HEIGHT              720
#define UVAC_FULL_HD_HEIGHT         1080
#define UVAC_MJPG_TARGET_SIZE       (100*1024*30)  //96650 ISO bandwidth 4M
#define UVC_SIZE_SETTING_TEST       DISABLE//ENABLE

static MEM_RANGE m_IPLworkBuf;

#define SOURCE_IPL    1
#if (SOURCE_IPL == 0)
#include "JpgDec.h"
#include "DCF.h"

#define     TEMP_FILE_BUF_SIZE  0xA00000
#define     TEMP_DECODED_BUF_SIZE   (0x2000000)//16MB x 2

static UINT32 m_uiVideoWidth = 640, m_uiVideoHeight = 480;
static UINT32 m_uiImgStartIdx = 1, m_uiImgNum = 3;

static void xUPccSetTestPattern(PMEM_RANGE pWorkBuf, UINT32 ImgStartIndex, UINT32 ImgNum, UINT32 Width, UINT32 Height,UINT32 videoFmt)
{
    UINT32 uiFileNum, uiDirNum, uiFileType;
    CHAR    chaFullName[30] = { 0 };
    UINT32 RawBuf, RawBufSize, FileBuf, FileBufSize;
    UINT32 i;
    ER Ret;
    JPG_DEC_INFO  DecOneJPGInfo = {0};
    JPGHEAD_DEC_CFG     JPGCfg ={0};
    UINT32  LineOff[3], PxlAddr[3];
    GX_IMAGE_PIXEL_FMT PxlFmt, PxlFmtOut;
    IMG_BUF DecodedBuf = {0},DstImgBuf = {0};
    UINT32  ImgBuf, ImgSize;

    DBG_IND(":buf=0x%x,%d,num=%d,w=%d,%d,fmt=%d ++\r\n",pWorkBuf, ImgStartIndex, ImgNum, Width, Height, videoFmt);
    FileBufSize = TEMP_FILE_BUF_SIZE;
    FileBuf =  ALIGN_FLOOR_4(pWorkBuf->Addr + pWorkBuf->Size- FileBufSize);

    RawBufSize = TEMP_DECODED_BUF_SIZE;
    RawBuf = ALIGN_FLOOR_4(FileBuf - RawBufSize);

    ImgBuf = pWorkBuf->Addr;
    ImgSize = Width*Height*2;
    for(i=ImgStartIndex; i<ImgStartIndex+ImgNum; i++)
    {
        DCF_GetObjInfo(i,&uiDirNum,&uiFileNum,&uiFileType);
        DCF_GetObjPath(i,uiFileType,chaFullName);

        {
        FST_FILE  filehdl;
        filehdl = FileSys_OpenFile(chaFullName,FST_OPEN_READ);
        FileSys_ReadFile(filehdl, (UINT8 *)FileBuf, &FileBufSize, 0, NULL);
        FileSys_CloseFile(filehdl);
        }

        DecOneJPGInfo.pDstAddr          = (UINT8 *)RawBuf;
        DecOneJPGInfo.jdcfg_p           = &JPGCfg;
        DecOneJPGInfo.DecodeType     = DEC_PRIMARY;
        DecOneJPGInfo.bEnableTimeOut = FALSE;
        DecOneJPGInfo.bEnableCrop = FALSE;
        DecOneJPGInfo.pSrcAddr      = (UINT8 *)FileBuf;
        DecOneJPGInfo.JPGFileSize   = FileBufSize;
        Ret = Jpg_DecOneJPG(&DecOneJPGInfo);
        if(E_OK == Ret)
        {
            LineOff[0]= DecOneJPGInfo.jdcfg_p->lineoffsetY;
            LineOff[1]= DecOneJPGInfo.jdcfg_p->lineoffsetUV;
            PxlAddr[0]= DecOneJPGInfo.uiOutAddrY;
            PxlAddr[1]= DecOneJPGInfo.uiOutAddrCb;
            if(DecOneJPGInfo.jdcfg_p->fileformat == JPG_FMT_YUV211)
                PxlFmt = GX_IMAGE_PIXEL_FMT_YUV422_PACKED;
            else
                PxlFmt = GX_IMAGE_PIXEL_FMT_YUV420_PACKED;

            if (videoFmt == UVAC_VIDEO_FORMAT_422)
                PxlFmtOut = GX_IMAGE_PIXEL_FMT_YUV422_PACKED;
            else
                PxlFmtOut = GX_IMAGE_PIXEL_FMT_YUV420_PACKED;

            GxImg_InitBufEx(&DecodedBuf,DecOneJPGInfo.jdcfg_p->imagewidth,DecOneJPGInfo.jdcfg_p->imageheight,PxlFmt,LineOff,PxlAddr);
            GxImg_InitBuf(&DstImgBuf,Width,Height,PxlFmtOut,GXIMAGE_LINEOFFSET_ALIGN(16),ImgBuf+(i-ImgStartIndex)*ImgSize,ImgSize);
            GxImg_ScaleDataFine(&DecodedBuf, REGION_MATCH_IMG, &DstImgBuf, REGION_MATCH_IMG);
        }
        else
        {
            DBG_ERR("ret=%d;i=%d,buf=0x%x,%d,num=%d,w=%d,%d,fmt=%d\r\n",Ret, i, pWorkBuf, ImgStartIndex, ImgNum, Width, Height, videoFmt);
        }

    }
    DBG_IND(":buf=0x%x,%d,num=%d,w=%d,%d,fmt=%d --\r\n",pWorkBuf, ImgStartIndex, ImgNum, Width, Height, videoFmt);

}
#endif

static void  xUPccOpenIPL(PMEM_RANGE pWorkBuf)
{
    DBG_IND(":buf=0x%x ++\r\n",pWorkBuf);
    m_IPLworkBuf = *pWorkBuf;

    #if SOURCE_IPL
    SENSOR_INIT_OBJ SenObj;
    SENSOR_DRVTAB *SenTab;

    SenObj = DrvSensor_GetObj1st();
    SenTab = DrvSensor_GetTab1st();
    Sensor_Open(SENSOR_ID_1, &SenObj, SenTab);
    IPL_Open();
    IPL_Display_OpenTsk();
    #endif

    DBG_IND(":buf=0x%x --\r\n",pWorkBuf);

}
static void xUPccCloseIPL(void)
{
    #if SOURCE_IPL
    IPL_Close();
    IPL_Display_CloseTsk();
    Sensor_Close(SENSOR_ID_1);
    #endif
}

typedef struct
{
    UINT32 width;
    UINT32 height;
    IPL_SENCROPRATIO ratio;
} IMG_SIZE_RATIO;

IMG_SIZE_RATIO ImgSizeRatio[] =
{
    {4,  3, IPL_SENCROPRATIO_4_3},
    {3,  2, IPL_SENCROPRATIO_3_2},
    {16, 9, IPL_SENCROPRATIO_16_9}
};

static UINT32 xUPccStartVideoCB(UINT32 videoWidth, UINT32 videoHeight, PCCAM_VIDEO_FORMAT videoFmt)
{
    DBG_IND(":w=%d,%d,fmt=%d ++\r\n",videoWidth,videoHeight,videoFmt);
    #if SOURCE_IPL
    IPL_CHGMODE_DATA CmdData = {0};

    CmdData.Mode = IPL_MODE_PREVIEW;
#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
    CmdData.SenNum = 2;
#else
    CmdData.SenNum = 1;
#endif
#if 0
    if((videoHeight == UVAC_HD_HEIGHT) || (videoHeight == UVAC_FULL_HD_HEIGHT))
    {
        CmdData.CropRatio = IPL_SENCROPRATIO_16_9;
    }
    else
    {
        CmdData.CropRatio = IPL_SENCROPRATIO_4_3;
    }
#else
    {
        UINT32  i, num, ratio;

        num = sizeof(ImgSizeRatio) / sizeof(IMG_SIZE_RATIO);

        CmdData.CropRatio = IPL_SENCROPRATIO_16_9; // default
        for (i = 0; i < num; i++)
        {
            ratio = (videoWidth * ImgSizeRatio[i].height * 100) / (videoHeight * ImgSizeRatio[i].width);
            if ((ratio > 99) && (ratio < 101)) // +- 1%
            {
                CmdData.CropRatio = ImgSizeRatio[i].ratio;
                #if 0
                if (i == 0)
                    DBG_DUMP("^YRatio 4:3\r\n");
                else if (i == 1)
                    DBG_DUMP("^YRatio 3:2\r\n");
                else
                    DBG_DUMP("^YRatio 16:9\r\n");
                #endif
                break;
            }
        }
    }
#endif
    CmdData.BufAddr = m_IPLworkBuf.Addr;
    CmdData.BufSize = m_IPLworkBuf.Size;
    CmdData.CB_FP = NULL;

    CmdData.BufSizeH_Disp = videoWidth;
    CmdData.BufSizeV_Disp = videoHeight;
    CmdData.ImgSizeH_Disp = videoWidth;
    CmdData.ImgSizeV_Disp = videoHeight;

    if (videoFmt == PCCAM_VIDEO_FORMAT_422)
    {
        CmdData.ImgFmt_Disp = IPL_IMG_Y_PACK_UV422;
    }
    else
    {
        CmdData.ImgFmt_Disp = IPL_IMG_Y_PACK_UV420;
    }

    CmdData.ImgP1En = DISABLE;
    CmdData.ImgP3En = DISABLE;

#if (_SENSORLIB2_ != _SENSORLIB2_DUMMY_)
    // only enable sensor 1 for PCC in dual sensor project
    CmdData.ProcSenId[IPL_ID_1] = SENSOR_ID_1;
    CmdData.ProcSenId[IPL_ID_2] = SENSOR_ID_NONE;
#endif

    CmdData.ISR_CB_FP[IPL_ISR_SIE] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_PRE] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_IFE] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_DCE] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_IPE] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_IME] = NULL;
    CmdData.ISR_CB_FP[IPL_ISR_IFE2] = NULL;
    CmdData.FuncEn = IPL_FUNC_NONE;
    IPL_SetCmd(IPL_SET_CHGMODE, (void *)&CmdData);
    IPL_WaitCmdFinish();
    debug_msg("^BvW=%d,vH=%d,vFmt=%d\r\n",videoWidth,videoHeight,videoFmt);
    #else
    m_uiVideoWidth = videoWidth;
    m_uiVideoHeight = videoHeight;
    xUPccSetTestPattern(&m_IPLworkBuf, m_uiImgStartIdx, m_uiImgNum, videoWidth, videoHeight, videoFmt);
    #endif

    DBG_IND(":w=%d,%d,fmt=%d --\r\n",videoWidth,videoHeight,videoFmt);

    #if (UI_STYLE==UI_STYLE_DRIVER)
    #if (_GYRO_EXT_!=_GYRO_EXT_NONE_)
      if (SysGetFlag(FL_MovieRSCIndex)==MOVIE_RSC_ON)
      {
        IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_OFF);
        IPL_AlgSetUIInfo(IPL_SEL_RSC, SEL_RSC_ON);
      }
    #endif
    #endif
    return E_OK;
}

static UINT32 xUPccGetVideoBufCB(UINT32 *yAddr, UINT32 *cbAddr, UINT32 *crAddr, UINT32 *lineOfsY)
{
    //DBG_IND(":y=0x%x,0x%x,0x%x,ofs=0x%x +-\r\n",*yAddr,*cbAddr,*crAddr,*lineOfsY);

    #if SOURCE_IPL
    IPL_IME_BUF_ADDR CurInfo;

    CurInfo.Id = IPL_ID_1;
    IPL_GetCmd(IPL_GET_IME_CUR_BUF_ADDR, (void *)&CurInfo);

    if (CurInfo.P2Enable == ENABLE)
    {
        *yAddr = CurInfo.ImeP2.PixelAddr[0];
        *cbAddr = CurInfo.ImeP2.PixelAddr[1];
        *crAddr = CurInfo.ImeP2.PixelAddr[2];
        *lineOfsY = CurInfo.ImeP2.Ch[0].LineOfs;
    }
    else
    {
        *yAddr = 0;
        *cbAddr = 0;
        *crAddr = 0;
        *lineOfsY = 0;
    }
    #else
    static UINT32 ImgIndex = 0, Cnt = 0;
    ImgIndex %= m_uiImgNum;
    *yAddr = m_IPLworkBuf.Addr +  ImgIndex*m_uiVideoWidth*m_uiVideoHeight*2;
    *cbAddr = *yAddr + m_uiVideoWidth*m_uiVideoHeight;
    if(++Cnt % SKIP_RATE == 0)
    {
        ImgIndex++;
    }
    *lineOfsY = m_uiVideoWidth;
    #endif
    return E_OK;
}

static UINT32 xUPccStopVideoCB(void)
{
    DBG_IND(" ++:%d\r\n",IPL_IsOpen());
    #if SOURCE_IPL
    IPL_CHGMODE_DATA CmdData;

    CmdData.Mode = IPL_MODE_OFF;
    CmdData.BufAddr = 0;
    CmdData.BufSize = 0;
    CmdData.CB_FP = NULL;
    CmdData.ImgSizeH_Disp = 0;
    CmdData.ImgSizeV_Disp = 0;
    CmdData.ImgFmt_Disp = 0;
    CmdData.ImgP1En = DISABLE;
    CmdData.ImgP3En = DISABLE;
    IPL_SetCmd(IPL_SET_CHGMODE, (void *)&CmdData);
    DBG_IND(" -:%d\r\n",IPL_IsOpen());
    IPL_WaitCmdFinish();
    #endif
    DBG_IND(" --\r\n");
    return E_OK;
}
_ALIGNED(4) static const UINT8 gPCCManuStrDesc[] =
{
    USB_VENDER_DESC_STRING_LEN*2+2, // size of String Descriptor = 6 bytes
    0x03,                       // 03: String Descriptor type
    USB_VENDER_DESC_STRING
};

_ALIGNED(4) const static UINT8 gPCCProdStrDesc[] =
{
    USB_PRODUCT_DESC_STRING_LEN*2+2, // size of String Descriptor = 6 bytes
    0x03,                       // 03: String Descriptor type
    USB_PRODUCT_DESC_STRING
};

//Serial number string descriptor, the content should be updated according to serial number
//It's useless since UVC driver restricts serial number string ID to 0
_ALIGNED(4) UINT16 gPCCSerialStrDesc3[] =
{
    0x0320,                             // 20: size of String Descriptor = 32 bytes
                                        // 03: String Descriptor type
    USB_SERIAL_NUM_STRING
};
void USBMakerInit_UPCC(UVAC_INFO *pUSBPCCInfo)
{
    pUSBPCCInfo->pManuStringDesc = (UVAC_STRING_DESC *)gPCCManuStrDesc;
    pUSBPCCInfo->pProdStringDesc = (UVAC_STRING_DESC *)gPCCProdStrDesc;
    pUSBPCCInfo->pSerialStringDesc = (UVAC_STRING_DESC *)gPCCSerialStrDesc3;
    pUSBPCCInfo->VID = USB_VID;
    pUSBPCCInfo->PID = USB_PID_PCCAM;
}

INT32 USBPCCExe_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    UVAC_INFO       UvacInfo = {0};
    UINT32          uiPoolAddr;
    MEM_RANGE       WorkBuf;

    DBG_IND(" ++\r\n");

    Ux_DefaultEvent(pCtrl,NVTEVT_EXE_OPEN,paramNum,paramArray);
    // Get memory for Photo task
    uiPoolAddr = OS_GetMempoolAddr(POOL_ID_DISP_VDO1);

    UvacInfo.UvacMemAdr    = uiPoolAddr;
    UvacInfo.UvacMemSize   = UVAC_GetNeedMemSize();
    UvacInfo.fpStartVideoCB  = xUPccStartVideoCB;
    UvacInfo.fpGetVideoBufCB = xUPccGetVideoBufCB;
    UvacInfo.fpStopVideoCB  = xUPccStopVideoCB;
    USBMakerInit_UPCC(&UvacInfo);
    WorkBuf.Addr = uiPoolAddr + UvacInfo.UvacMemSize;
    WorkBuf.Size = POOL_SIZE_APP - UvacInfo.UvacMemSize;

    xUPccOpenIPL(&WorkBuf);

    #if (UVC_SIZE_SETTING_TEST == ENABLE)
    {
        UVAC_SetMjpgOnly(TRUE); // only support MJPEG

        // sample code for UVC supported size setting
        UVAC_UvcSizeConfig(
            UVC_SIZE_640x480  |
            UVC_SIZE_640x360  |
            UVC_SIZE_720x480  |
            UVC_SIZE_848x480  |
            UVC_SIZE_1024x576 |
            UVC_SIZE_1024x768 |
            UVC_SIZE_1280x720 |
            UVC_SIZE_1280x960 |
            UVC_SIZE_1920x1080
            );
        UVAC_SetUvcDefaultSize(UVC_SIZE_848x480);
    }
    #endif

    UVAC_SetConfig(UVAC_CONFIG_MJPG_TARGET_SIZE, UVAC_MJPG_TARGET_SIZE);
    UVAC_SetConfig(UVAC_CONFIG_VID_TRIG_TYPE, UVAC_TRIG_GETVID_TYPE_AUD);//UVAC_TRIG_GETVID_TYPE_TIMER
    if (UVAC_Open(&UvacInfo) != E_OK)
    {
        DBG_ERR("Error open USB PCC task\r\n");
    }

    DBG_IND(" aud/timer=%d --\r\n",UVAC_TRIG_GETVID_TYPE_AUD);

    return NVTEVT_CONSUME;
}

INT32 USBPCCExe_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    DBG_IND(" +\r\n");
    UVAC_Close();
    xUPccCloseIPL();
    DBG_IND(" -\r\n");
    Ux_DefaultEvent(pCtrl,NVTEVT_EXE_CLOSE,paramNum,paramArray);
    DBG_IND(" ---\r\n");
    return NVTEVT_CONSUME;
}

#if (UVC_RECORD_FUNC == ENABLE)
static BOOL             g_bEnStartVideoCB = TRUE;
static BOOL             g_bEnStopVideoCB = TRUE;
static BOOL             g_bUvcActive = FALSE;
static BOOL             g_bUvcOpen = FALSE;
static UVCREC_IMG_ID    g_UvcRecImgId = UVCREC_IMG_ID_1;

static UVCREC_IMG_DATA  g_UvcRecImgData[UVCREC_IMG_ID_MAX] =
{
    {
        640,                        ///< image buffer width
        480,                        ///< image buffer height
        640,                        ///< image width for 4:3 image ratio
        480,                        ///< image height for 4:3 image ratio
        640,                        ///< image width for 16:9 image ratio
        360,                        ///< image height for 16:9 image ratio
        0x02,                       ///< UVC size ID for 4:3 image ratio (2nd frame index)
        0x01,                       ///< UVC size ID for 16:9 image ratio (1st frame index)
        UVC_SIZE_640x480 | UVC_SIZE_640x360
    },

    {
        848,                        ///< image buffer width
        480,                        ///< image buffer height
        640,                        ///< image width for 4:3 image ratio
        480,                        ///< image height for 4:3 image ratio
        848,                        ///< image width for 16:9 image ratio
        480,                        ///< image height for 16:9 image ratio
        0x02,                       ///< UVC size ID for 4:3 image ratio (1st frame index)
        0x01,                       ///< UVC size ID for 16:9 image ratio (2nd frame index)
        UVC_SIZE_640x480 | UVC_SIZE_848x480
    },

    {
        1280,                       ///< image buffer width
        960,                        ///< image buffer height
        1280,                       ///< image width for 4:3 image ratio
        960,                        ///< image height for 4:3 image ratio
        1280,                       ///< image width for 16:9 image ratio
        720,                        ///< image height for 16:9 image ratio
        0x02,                       ///< UVC size ID for 4:3 image ratio (2nd frame index)
        0x01,                       ///< UVC size ID for 16:9 image ratio (1st frame index)
        UVC_SIZE_1280x960 | UVC_SIZE_1280x720
    },
};

static UINT32 UvcRec_GetVideoBufCB(UINT32 *yAddr, UINT32 *cbAddr, UINT32 *crAddr, UINT32 *lineOfsY)
{
    if (PhotoExe_IsQVForUVC())
    {
        IMG_CAP_QV_DATA *pImgQvData;
        pImgQvData = DscPhoto_GetQvImg();
        *yAddr = pImgQvData->ImgInfo.PixelAddr[0];
        *cbAddr = pImgQvData->ImgInfo.PixelAddr[1];
        *crAddr = pImgQvData->ImgInfo.PixelAddr[2];
        *lineOfsY = pImgQvData->ImgInfo.Ch[0].LineOfs;
    }
    else
    {
        IPL_IME_BUF_ADDR CurInfo;

        CurInfo.Id = IPL_ID_1;
        IPL_GetCmd(IPL_GET_IME_CUR_BUF_ADDR, (void *)&CurInfo);

        if (CurInfo.P3Enable == ENABLE)
        {
            *yAddr = CurInfo.ImeP3.PixelAddr[0];
            *cbAddr = CurInfo.ImeP3.PixelAddr[1];
            *crAddr = CurInfo.ImeP3.PixelAddr[2];
            *lineOfsY = CurInfo.ImeP3.Ch[0].LineOfs;
        }
        else
        {
            *yAddr = 0;
            *cbAddr = 0;
            *crAddr = 0;
            *lineOfsY = 0;
        }
    }

    return E_OK;
}

static UINT32 UvcRec_StartVideoCB(UINT32 videoWidth, UINT32 videoHeight, PCCAM_VIDEO_FORMAT videoFmt)
{
    if (g_bEnStartVideoCB)
    {
        g_bEnStopVideoCB = TRUE;
        g_bUvcActive = TRUE;
    }

    return E_OK;
}

static UINT32 UvcRec_StopVideoCB(void)
{
    if (g_bEnStopVideoCB)
    {
        g_bUvcActive = FALSE;
    }

    return E_OK;
}

void UvcRec_Open(void)
{
    UVAC_INFO       UvacInfo = {0};
    UINT32          uiPoolAddr;

    if (g_bUvcOpen == FALSE)
    {
        g_bUvcOpen = TRUE;

        // Get memory for Photo task
        uiPoolAddr = OS_GetMempoolAddr(POOL_ID_APP);

        UvacInfo.UvacMemAdr      = uiPoolAddr;
        UvacInfo.UvacMemSize     = UVAC_GetNeedMemSize();
        UvacInfo.fpStartVideoCB  = UvcRec_StartVideoCB;
        UvacInfo.fpGetVideoBufCB = UvcRec_GetVideoBufCB;
        UvacInfo.fpStopVideoCB   = UvcRec_StopVideoCB;
        USBMakerInit_UPCC(&UvacInfo);

        g_bEnStartVideoCB = TRUE;
        g_bEnStopVideoCB = TRUE;
        UVAC_SetMjpgOnly(TRUE); // only support MJPEG to minimize buffer usage
        UVAC_UvcSizeConfig(g_UvcRecImgData[g_UvcRecImgId].uiUvcSizeMapping);
        UVAC_SetConfig(UVAC_CONFIG_MJPG_TARGET_SIZE, UVAC_MJPG_TARGET_SIZE);
        UVAC_SetConfig(UVAC_CONFIG_VID_TRIG_TYPE, UVAC_TRIG_GETVID_TYPE_TIMER);
        if (UVAC_Open(&UvacInfo) != E_OK)
        {
            DBG_ERR("Error open USB PCC task\r\n");
        }
    }
}

void UvcRec_Close(void)
{
    if (g_bUvcOpen == TRUE)
    {
        g_bUvcOpen = FALSE;
        g_bEnStartVideoCB = FALSE;
        g_bEnStopVideoCB = FALSE;
        UVAC_Close();
    }
}

void UvcRec_EnStartVideoCB(BOOL bEn)
{
    g_bEnStartVideoCB = bEn;
}

void UvcRec_EnStopVideoCB(BOOL bEn)
{
    g_bEnStopVideoCB = bEn;
}

BOOL UvcRec_IsUvcActive(void)
{
    return g_bUvcActive;
}

void UvcRec_SetImgID(UVCREC_IMG_ID ImgId)
{
    g_UvcRecImgId = ImgId;
}

PUVCREC_IMG_DATA UvcRec_GetImgData(void)
{
    return &g_UvcRecImgData[g_UvcRecImgId];
}
#endif


////////////////////////////////////////////////////////////

EVENT_ENTRY CustomUSBPCCObjCmdMap[] =
{
    {NVTEVT_EXE_OPEN,               USBPCCExe_OnOpen},
    {NVTEVT_EXE_CLOSE,              USBPCCExe_OnClose},
    {NVTEVT_NULL,                   0},  //End of Command Map
};

CREATE_APP(CustomUSBPCCObj,APP_PCC)

//#NT#2010/07/29#Lily Kao -end
//@}
