#include "UIFlow.h"
#include "type.h"
#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "Sdio.h"
#include "MediaRecAPI.h"
#include "IPL_Ctrl.h" // Isiah, implement YUV merge mode of recording func.

#define MOVIE_REC_TIME_MIN  1 // 1 sec

#define MOVIE_REC_MIN_CLUSTER_SIZE      (0x8000)    //32KB
#define MOVIE_REC_SD_CLASS              (4)         // class 4

MOV_TASK_DATA   gMovData = { 0 };
//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
BOOL    g_bMovFirstBootRecFlag = TRUE;
//#NT#2015/01/21#KS Hung -end

static UINT32  g_MovRecMaxTime = 0;
static UINT32  g_MovRecCurrTime = 0;
static UINT32  g_MovRecSelfTimerSec = 0;
static UINT32  g_MovRecSelfTimerID = NULL_TIMER;
extern BOOL    g_uiRecordIngMotionDet;
extern void Movie_IPLChangeCB(UINT32 mode, UINT32 param);

UINT8 FlowMovie_GetMovDataState(void)
{
    return gMovData.State;
}

void FlowMovie_StartRec(void)
{
    //#NT#2015/01/21#KS Hung -begin
    //#NT#Fast boot recording.
    #if (_FAST_BOOT_REC_FUNC_)
    gMovData.State = MOV_ST_REC;
    #endif
    //#NT#2015/01/21#KS Hung -end
    if (g_MovRecSelfTimerSec == 0)
    {
        g_MovRecCurrTime = 0;

        // hide max rec time
        FlowMovie_IconHideMaxRecTime();

        // draw rec
        FlowMovie_IconDrawRecTime();

        // update DCF Folder/File name
        FlowDCF_UpdateName();

        // wait playing sound stop
        //#NT#2015/01/21#KS Hung -begin
        //#NT#Fast boot recording.
        #if (_FAST_BOOT_REC_FUNC_ == DISABLE)
        GxSound_WaitStop();
        #endif
        //#NT#2015/01/21#KS Hung -end

        //#NT#2012/10/23#Philex Lin - begin
        // disable auto power off/USB detect timer
        KeyScan_EnableMisc(FALSE);
        //#NT#2012/10/23#Philex Lin - end

        Ux_SendEvent(&CustomMovieObjCtrl,NVTEVT_EXE_MOVIE_REC_START,NULL);
    }
    else
    {
        debug_err(("FlowMovie_StartRec: Not yet,g_MovSelfTimerSec=%d\r\n", g_MovRecSelfTimerSec));
    }
}

void FlowMovie_StopRec(void)
{
    // Isiah, implement YUV merge mode of recording func.
    IPL_SLEEP_INFO Info;

    UxState_SetData(&UIFlowWndMovie_Status_RECCtrl,STATE_CURITEM,UIFlowWndMovie_Status_REC_ICON_TRANSPAENT);
    Ux_SendEvent(&CustomMovieObjCtrl,NVTEVT_EXE_MOVIE_REC_STOP,NULL);

    //#NT#2012/10/23#Philex Lin - begin
    // enable auto power off/USB detect timer
    KeyScan_EnableMisc(TRUE);
    gMovData.State= MOV_ST_VIEW;
    //#NT#2012/10/23#Philex Lin - end

    //g_uiRecordIngMotionDet = FALSE;

    // Isiah, implement YUV merge mode of recording func.
    if(FlowMovie_RecGetYUVMergeMode())
    {
        Info.Id = 0;
        IPL_SetCmd(IPL_SET_WAKEUP, (void *)&Info);
        IPL_WaitCmdFinish();

        IPC_WaitVD(FALSE, IPL_ID_1);
        IPC_WaitVD(FALSE, IPL_ID_1);
        IPC_WaitVD(FALSE, IPL_ID_1);

        Movie_IPLChangeCB(MEDIAREC_IPLCHG_PREVIEW, 0);
        debug_msg("Change IPL to movie preview mode\r\n");
    }
}

void FlowMovie_StopRecSelfTimer(void)
{
    if (g_MovRecSelfTimerID != NULL_TIMER)
    {
        GxTimer_StopTimer(&g_MovRecSelfTimerID);
        g_MovRecSelfTimerID = NULL_TIMER;
    }
    g_MovRecSelfTimerSec = 0;
}

UINT8 FlowMovie_GetDataState(void)
{
    return gMovData.State;
}

void FlowMovie_SetRecMaxTime(UINT32 RecMaxTime)
{
    g_MovRecMaxTime = RecMaxTime;
}

UINT32 FlowMovie_GetRecMaxTime(void)
{
    return g_MovRecMaxTime;
}

void FlowMovie_SetRecCurrTime(UINT32 RecCurrTime)
{
    g_MovRecCurrTime = RecCurrTime;
}

UINT32 FlowMovie_GetRecCurrTime(void)
{
    return g_MovRecCurrTime;
}

//#NT#2015/01/21#KS Hung -begin
//#NT#Fast boot recording.
void FlowMovie_SetFirstBootRecFlag(BOOL bEn)
{
    g_bMovFirstBootRecFlag = bEn;
}

BOOL FlowMovie_GetFirstBootRecFlag(void)
{
    return g_bMovFirstBootRecFlag;
}
//#NT#2015/01/21#KS Hung -end

BOOL FlowMovie_IsStorageErr(BOOL IsCheckFull)
{
    UINT32  uiClusterSize;
#if (MOVIE_REC_CLASS4 == ENABLE)
    PSDIO_MISC_INFORMATION pMiscInfo;
#endif

    // check card inserted
    #if (_DUAL_CARD_FUNC_ == ENABLE)
    if ((UI_GetData(FL_CardStatus) == CARD_REMOVED) && (UI_GetData(FL_Card2Status) == CARD_REMOVED))
    #else
    if (UI_GetData(FL_CardStatus) == CARD_REMOVED)
    #endif
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_PLEASE_INSERT_SD,FLOWWRNMSG_TIMER_1SEC);
        return TRUE;
    }

    // check storage error
    if(UIStorageCheck(STORAGE_CHECK_ERROR, NULL) == TRUE)
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,FLOWWRNMSG_ISSUE_MEM_ERR,FLOWWRNMSG_TIMER_KEEP);
        return TRUE;
    }


    // check storage lock or directory read only
    if (UIStorageCheck(STORAGE_CHECK_LOCKED, NULL) == TRUE ||
        UIStorageCheck(STORAGE_CHECK_DCIM_READONLY, NULL) == TRUE)
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_LOCKED,FLOWWRNMSG_TIMER_1SEC);
        return TRUE;
    }

    // check storage full
    if (TRUE == IsCheckFull)
    {
        g_MovRecMaxTime = UIMovRecObj_GetData(RECMOVIE_MAXSECOND);
        if (g_MovRecMaxTime <= MOVIE_REC_TIME_MIN)
        {
            g_MovRecMaxTime = 0;
            #if (_DUAL_CARD_FUNC_ == DISABLE)
            Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL,FLOWWRNMSG_TIMER_1SEC);
            #endif
            return TRUE;
        }
    }

    // check folder full
    if (UIStorageCheck(STORAGE_CHECK_FOLDER_FULL, NULL) == TRUE)
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CARD_FULL,FLOWWRNMSG_TIMER_1SEC);
        return TRUE;
    }


    // check sd card whether faster than class 4
#if (MOVIE_REC_CLASS4 == ENABLE)
    pMiscInfo = sdio_getMiscInfo();
    if (pMiscInfo->uiWriteRate<MOVIE_REC_SD_CLASS)
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_SD_CLASS4,FLOWWRNMSG_TIMER_1SEC);
        return TRUE;
    }
#endif

    // Check cluster size, need to be larger than 32KB
    uiClusterSize = FileSys_GetDiskInfo(FST_INFO_CLUSTER_SIZE);
    if (uiClusterSize< MOVIE_REC_MIN_CLUSTER_SIZE)
    {
        Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,UIFlowWndWrnMsg_StatusTXT_Msg_STRID_CLUSTER_WRONG,FLOWWRNMSG_TIMER_1SEC);
        return TRUE;
    }

    return FALSE;
}

UINT32 FlowMovie_GetSelfTimerID(void)
{
    return g_MovRecSelfTimerID;
}

#if 0
BOOL FlowMovie_ChkDrawStoreFullFolderFull(void)
{

    if(UIStorageCheck(STORAGE_CHECK_FULL, &(g_MovRecMaxTime)) == TRUE)
    {
        g_MovRecMaxTime = UIMovRecObj_GetData(RECMOVIE_MAXSECOND);
        #if (_DUAL_CARD_FUNC_ == ENABLE)
        if ((UI_GetData(FL_CardStatus) == CARD_REMOVED) && (UI_GetData(FL_Card2Status) == CARD_REMOVED))
        #else
        if (UI_GetData(FL_CardStatus) == CARD_REMOVED)
        #endif
        {
            Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,FLOWWRNMSG_ISSUE_MEM_FULL,FLOWWRNMSG_TIMER_2SEC);
            return TRUE;
        }
        else
        {
            Ux_OpenWindow(&UIFlowWndWrnMsgCtrl,2,FLOWWRNMSG_ISSUE_CARD_FULL,FLOWWRNMSG_TIMER_2SEC);
            return TRUE;
        }
    }

    return FALSE;
}
#endif

void FlowMovie_GetPictureInMovie(void)
{
#if (0)//(_MOVIE_PIM_MODE_ == ENABLE)
    UINT32  sec, unsafe;

    switch (gMovData.State)
    {
    case MOV_ST_REC:
        sec = MovRec_Disk2Second();
        unsafe = MovRec_CheckUnsavedFileBlocks();

        if ((sec > 3)&&(unsafe == 0))
        {
            RawEnc_SetCaptureOne();
            //UxCtrl_SetShow(&UIFlowWndMovie_Static_pimCtrl, TRUE);
        }
        else
        {
            if (unsafe)
            {
                debug_err(("unsafe to take picture !\r\n"));
            }
            else
            {
                debug_err(("no space to encode RAW !\r\n"));
            }
        }
        break;
    }
#endif
}

void FlowMovie_OnTimer1SecIndex(void)
{

    switch (gMovData.State)
    {
    case MOV_ST_VIEW:
    case MOV_ST_VIEW | MOV_ST_ZOOM:
    case MOV_ST_REC:
    case MOV_ST_REC | MOV_ST_ZOOM:
        gMovData.SysTimeCount++;
        if (UxCtrl_IsShow(&UIFlowWndMovie_YMD_StaticCtrl))
        {
            FlowMovie_IconDrawDateTime();
        }
        break;

    }
}

