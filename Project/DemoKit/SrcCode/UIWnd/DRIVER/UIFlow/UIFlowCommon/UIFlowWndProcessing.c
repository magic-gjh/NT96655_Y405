//This source code is generated by UI Designer Studio.

#include "UIFramework.h"
#include "UIFrameworkExt.h"
#include "UIGraphics.h"
#include "NVTToolCommand.h"
#include "UIFlowWndProcessingRes.c"
#include "UIFlowWndProcessing.h"
#include "UIFlow.h"

//---------------------UIFlowWndProcessingCtrl Debug Definition -----------------------------
#define _UIFLOWWNDPROCESSING_ERROR_MSG        1
#define _UIFLOWWNDPROCESSING_TRACE_MSG        0

#if (_UIFLOWWNDPROCESSING_ERROR_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_ERR))
#define UIFlowWndProcessingErrMsg(...)            debug_msg ("^R UIFlowWndProcessing: "__VA_ARGS__)
#else
#define UIFlowWndProcessingErrMsg(...)
#endif

#if (_UIFLOWWNDPROCESSING_TRACE_MSG&(PRJ_DBG_LVL>=PRJ_DBG_LVL_TRC))
#define UIFlowWndProcessingTraceMsg(...)          debug_msg ("^B UIFlowWndProcessing: "__VA_ARGS__)
#else
#define UIFlowWndProcessingTraceMsg(...)
#endif

//---------------------UIFlowWndProcessingCtrl Global Variables -----------------------------
static CHAR     g_cProgress[6] = {0};
static UINT32   g_uiProgress = 0;

//---------------------UIFlowWndProcessingCtrl Prototype Declaration  -----------------------

//---------------------UIFlowWndProcessingCtrl Public API  ----------------------------------

//---------------------UIFlowWndProcessingCtrl Private API  ---------------------------------
//---------------------UIFlowWndProcessingCtrl Control List---------------------------
CTRL_LIST_BEGIN(UIFlowWndProcessing)
CTRL_LIST_ITEM(UIFlowWndProcessing_StaticText_Msg)
CTRL_LIST_ITEM(UIFlowWndProcessing_StaticText_Progress)
CTRL_LIST_END

//----------------------UIFlowWndProcessingCtrl Event---------------------------
INT32 UIFlowWndProcessing_OnOpen(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndProcessing_OnClose(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndProcessing_OnUpdateInfo(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndProcessing_OnTimer(VControl *, UINT32, UINT32 *);
INT32 UIFlowWndProcessing_OnBackgroundDone(VControl *, UINT32, UINT32 *);
EVENT_BEGIN(UIFlowWndProcessing)
EVENT_ITEM(NVTEVT_OPEN_WINDOW,UIFlowWndProcessing_OnOpen)
EVENT_ITEM(NVTEVT_CLOSE_WINDOW,UIFlowWndProcessing_OnClose)
EVENT_ITEM(NVTEVT_UPDATE_INFO,UIFlowWndProcessing_OnUpdateInfo)
EVENT_ITEM(NVTEVT_TIMER,UIFlowWndProcessing_OnTimer)
EVENT_ITEM(NVTEVT_BACKGROUND_DONE,UIFlowWndProcessing_OnBackgroundDone)
EVENT_END

INT32 UIFlowWndProcessing_OnOpen(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    g_uiProgress = 0;
    sprintf(g_cProgress, "0 %");
    UxStatic_SetData(&UIFlowWndProcessing_StaticText_ProgressCtrl, STATIC_VALUE, Txt_Pointer(g_cProgress));
    //UxCtrl_SetShow(&UIFlowWndProcessing_StaticText_ProgressCtrl,TRUE);

    Ux_DefaultEvent(pCtrl,NVTEVT_OPEN_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 UIFlowWndProcessing_OnClose(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_DefaultEvent(pCtrl,NVTEVT_CLOSE_WINDOW,paramNum,paramArray);
    return NVTEVT_CONSUME;
}
INT32 UIFlowWndProcessing_OnUpdateInfo(VControl * pCtrl, UINT32 paramNum, UINT32 * paramArray)
{
    UINT32 uiProgress = 0;

    if (paramNum)
    {
        uiProgress = paramArray[0];
        LIMIT(uiProgress, 0, 100);
    }

    if (g_uiProgress != uiProgress)
    {
        g_uiProgress = uiProgress;
        sprintf(g_cProgress, "%d %", uiProgress);
        UxStatic_SetData(&UIFlowWndProcessing_StaticText_ProgressCtrl, STATIC_VALUE, Txt_Pointer(g_cProgress));
        //Ux_Redraw();
    }
    return NVTEVT_CONSUME;
}
INT32 UIFlowWndProcessing_OnTimer(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    return NVTEVT_CONSUME;
}
INT32 UIFlowWndProcessing_OnBackgroundDone(VControl *pCtrl, UINT32 paramNum, UINT32 *paramArray)
{
    Ux_CloseWindow(pCtrl, 2, NVTRET_PROCESSING, NVTRET_OK);
    return NVTEVT_CONSUME;
}
//----------------------UIFlowWndProcessing_StaticText_MsgCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndProcessing_StaticText_Msg)
EVENT_END

//----------------------UIFlowWndProcessing_StaticText_ProgressCtrl Event---------------------------
EVENT_BEGIN(UIFlowWndProcessing_StaticText_Progress)
EVENT_END

