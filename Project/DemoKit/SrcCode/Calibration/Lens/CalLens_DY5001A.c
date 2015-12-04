/**
    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.

    @file       Cal_Lens_ST2089.c
    @ingroup    mISYSLens

    @brief      Lens calibration library

    @note       Nothing.

    @version    V1.00.000
    @author     iView SA
    @date       2011/02/26
*/

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    "SysCfg.h"
#include    "CalLens.h"
#include    "PhotoTask.h"
#include    "IDE.h"
#include    "sie_lib.h"
#include    "Utility.h"
#include    "UIFlow.h"
#include    "AF_Lib.h"
#include    "Type.h"
#include    "CalibrationAPI.h"
#include    "DxInput.h"
#include    "IPL_Ctrl.h"

#if (_CALIBRATION_MODE_ == ENABLE)

INT16 gCalLens_FocusRangeTable[CALLENS_ZOOM_SECTION_NUM][2]={
    //  Far , Near
    //start ,  end ,
    {100 , -40 }, //      Z1
    {170 , -30 }, // 1.1
    {190 ,  20 }, // 1.2  Z3
    {270 ,  50 }, // 1.3
    {290 , 100 }, // 1.4  Z5
    {350 , 120 }, // 1.5
    {385 , 170 }, // 1.6  Z7
    {420 , 200 }, // 1.7
    {450 , 220 }, // 1.8  Z9
    {475 , 260 }, // 1.9
    {520 , 300 }, //      Z11
    {540 , 340 }, // 2.1
    {555 , 360 }, // 2.2  Z13
    {585 , 380 }, // 2.3
    {600 , 390 }, // 2.4  Z15
    {620 , 400 }, // 2.5
    {630 , 420 }, // 2.6  Z17
    {660 , 430 }, // 2.7
    {680 , 450 }, // 2.8  Z19
    {690 , 450 }, // 2.9
    {710 , 460 }, //      Z21
    {720 , 460 }, // 3.1
    {730 , 470 }, // 3.2  Z23
    {735 , 470 }, // 3.3
    {745 , 470 }, // 3.4  Z25
    {750 , 480 }, // 3.5
    {760 , 480 }, // 3.6  Z27
    {760 , 480 }, // 3.7
    {760 , 480 }, // 3.8  Z29
    {755 , 480 }, // 3.9
    {755 , 500 }, //      Z31
    {755 , 500 }, // 4.1
    {755 , 500 }, // 4.2  Z33
    {740 , 500 }, // 4.3
    {740 , 500 }, // 4.4  Z35
    {715 , 500 }, // 4.5
    {715 , 500 }, // 4.6  Z37
};

static INT32 g_FocusTableFromSpec[CALLENS_ZOOM_SECTION_NUM][CALLENS_FOCUS_TABLE_COLUMN]={
//  Inf,  8.0m,  5.0m,  3.0m,  2.0m,  1.0m,  0.8m,  0.6m,  0.5m,  0.4m,  0.2m
{    -1,   -5 ,   -6 ,   -8 ,  -10 ,  -16 ,  -19 ,  -24 ,  -27 ,   -34,  -34 },
{    60,   60 ,   59 ,   57 ,   54 ,   46 ,   43 ,   37 ,   34 ,    25,   -5 },  // 1.1
{   126,  124 ,  122 ,  120 ,  117 ,  108 ,  103 ,   96 ,   94 ,    83,   49 },  // 1.2
{   181,  177 ,  176 ,  173 ,  170 ,  160 ,  155 ,  148 ,  144 ,   134,   98 },  // 1.3
{   237,  231 ,  229 ,  226 ,  223 ,  212 ,  207 ,  200 ,  195 ,   185,  147 },  // 1.4
{   281,  275 ,  273 ,  270 ,  266 ,  255 ,  250 ,  241 ,  237 ,   226,  186 },  // 1.5
{   325,  318 ,  316 ,  313 ,  309 ,  297 ,  292 ,  283 ,  279 ,   266,  225 },  // 1.6
{   360,  355 ,  353 ,  349 ,  345 ,  332 ,  326 ,  317 ,  313 ,   300,  257 },  // 1.7
{   396,  391 ,  389 ,  385 ,  380 ,  367 ,  361 ,  351 ,  347 ,   333,  289 },  // 1.8
{   426,  421 ,  418 ,  414 ,  410 ,  396 ,  389 ,  379 ,  374 ,   361,  315 },  // 1.9
{   456,  451 ,  448 ,  444 ,  439 ,  425 ,  418 ,  407 ,  401 ,   388,  341 },
{   481,  475 ,  473 ,  469 ,  463 ,  448 ,  441 ,  430 ,  424 ,   410,  410 },  // 2.1
{   506,  500 ,  498 ,  493 ,  488 ,  472 ,  465 ,  453 ,  447 ,   433,  433 },  // 2.2
{   527,  521 ,  518 ,  514 ,  508 ,  492 ,  484 ,  473 ,  466 ,   451,  451 },  // 2.3
{   548,  542 ,  539 ,  534 ,  528 ,  512 ,  504 ,  492 ,  485 ,   470,  470 },  // 2.4
{   566,  560 ,  557 ,  552 ,  545 ,  528 ,  520 ,  508 ,  501 ,   486,  486 },  // 2.5
{   584,  577 ,  574 ,  569 ,  563 ,  545 ,  537 ,  524 ,  517 ,   501,  501 },  // 2.6
{   599,  592 ,  589 ,  583 ,  577 ,  559 ,  550 ,  537 ,  529 ,   514,  514 },  // 2.7
{   614,  607 ,  604 ,  598 ,  591 ,  572 ,  564 ,  551 ,  542 ,   527,  527 },  // 2.8
{   627,  619 ,  616 ,  610 ,  603 ,  584 ,  575 ,  562 ,  553 ,   538,  538 },  // 2.9
{   640,  631 ,  628 ,  622 ,  615 ,  595 ,  586 ,  572 ,  564 ,   548,  548 },
{   650,  641 ,  638 ,  632 ,  624 ,  605 ,  596 ,  582 ,  573 ,   557,  557 },  // 3.1
{   661,  652 ,  648 ,  642 ,  634 ,  614 ,  605 ,  591 ,  582 ,   566,  566 },  // 3.2
{   670,  660 ,  656 ,  650 ,  642 ,  622 ,  613 ,  598 ,  589 ,   573,  573 },  // 3.3
{   678,  668 ,  665 ,  658 ,  651 ,  630 ,  620 ,  606 ,  596 ,   580,  580 },  // 3.4
{   685,  675 ,  671 ,  665 ,  657 ,  636 ,  627 ,  612 ,  602 ,   586,  586 },  // 3.5
{   691,  682 ,  678 ,  672 ,  664 ,  643 ,  633 ,  618 ,  608 ,   592,  592 },  // 3.6
{   696,  687 ,  683 ,  677 ,  669 ,  647 ,  638 ,  623 ,  613 ,   597,  597 },  // 3.7
{   700,  693 ,  689 ,  682 ,  674 ,  652 ,  643 ,  628 ,  618 ,   602,  602 },  // 3.8
{   702,  696 ,  692 ,  685 ,  677 ,  655 ,  646 ,  631 ,  621 ,   605,  605 },  // 3.9
{   705,  699 ,  695 ,  688 ,  680 ,  658 ,  649 ,  634 ,  624 ,   608,  608 },
{   705,  699 ,  695 ,  688 ,  680 ,  659 ,  649 ,  634 ,  625 ,   608,  608 },  // 4.1
{   704,  699 ,  695 ,  688 ,  680 ,  659 ,  650 ,  635 ,  626 ,   609,  609 },  // 4.2
{   701,  695 ,  691 ,  685 ,  677 ,  656 ,  647 ,  632 ,  623 ,   607,  607 },  // 4.3
{   698,  692 ,  688 ,  682 ,  674 ,  654 ,  644 ,  630 ,  620 ,   605,  605 },  // 4.4
{   691,  685 ,  682 ,  676 ,  668 ,  648 ,  639 ,  625 ,  615 ,   600,  600 },  // 4.5
{   685,  679 ,  675 ,  670 ,  662 ,  643 ,  634 ,  620 ,  610 ,   596,  596 },  // 4.6
//{   680,  673 ,  670 ,  665 ,  657 ,  638 ,  629 ,  615 ,  605 ,   591,  591 },  // 4.7
//{   674,  668 ,  665 ,  659 ,  652 ,  633 ,  624 ,  610 ,  600 ,   586,  586 },  // 4.8
//{   668,  663 ,  660 ,  654 ,  647 ,  628 ,  619 ,  605 ,  595 ,   581,  581 },  // 4.9
//{   663,  657 ,  654 ,  648 ,  641 ,  622 ,  613 ,  599 ,  589 ,   575,  575 },  //
};

extern void AE_reset (void);
extern void lensDY5001A_test(void);

UINT8 CalLens_NearFocusCalibration(void)
{
    INT32 maxIdx = 0;
    INT16 StartIdx,EndIdx;
    UINT  AF_Step;
    char str1[32];
    UINT32 maxVA;
    BOOL bFocusNG = FALSE;
    UINT32 StopFunc=0,StartFunc=0;
    UINT32 tempFrequency=0;
    UINT32 zoomSection;

    /************************************************************/
    debug_err(("\n\nPlease let camera facing ISO chart. Then press shutter key\r\n\n"));
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    Cal_ShowStringWithColor("1. Face ISO chart (near(0.5))",0,0,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("2. Press shutter key",0,30,_OSD_INDEX_BLACK);
    #if 0
    clr_flg(FLG_ID_KEY, FLGKEY_SHUTTER2);
    wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_SHUTTER2, TWF_ORW|TWF_CLR);
    #else
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_SHUTTER1);
    #endif
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    /*************************************************************/

    //tempFrequency = IPL_AlgGetUIInfo(IPL_SEL_FREQUENCY);
    //IPL_AlgSetUIInfo(IPL_SEL_FREQUENCY,SEL_FREQUENCY_50HZ);
    #if 1
    StartFunc=0;
    StartFunc |=UIAPP_PHOTO_AE;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);

    AE_reset();
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    AE_Wait_Stable(3,60);

    StopFunc |=UIAPP_PHOTO_AE;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);
    #endif

    StopFunc |=UIAPP_PHOTO_AE;
    StopFunc |=UIAPP_PHOTO_AWB;
    StopFunc |=UIAPP_PHOTO_FD;
    StopFunc |=UIAPP_PHOTO_CAF;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);

    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_NULL);

    Lens_Zoom_SetPPS(300);
    Lens_Focus_SetPPS(500);

    Lens_Init(LENS_INIT_ZOOM_PART1);
    Lens_Init(LENS_INIT_ZOOM_PART2);

    if (gCalLens_Info.bFocusBLCal == FALSE)
    {
        gCalLens_Info.uiLensFocusBL = CalLens_FocusBLCalibration();
    }

    UILens_AFPrepare(TRUE);

    AF_Step=4;
    for(zoomSection=Lens_Zoom_GetSection(ZOOM_MIN_SECTION);zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
    {
        //Lens_Focus_GotoHome();
        Lens_Zoom_GoSection(zoomSection);

        #if 0
        StartFunc=0;
        StartFunc |=UIAPP_PHOTO_AE;
        Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);
        AE_reset();
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        AE_Wait_Stable(3,60);
        StopFunc |=UIAPP_PHOTO_AE;
        Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);
        #endif

        if(CalLens_IsFarAdjust())
        {
            StartIdx = CalLens_GetFarCalCompensation(zoomSection, FALSE);
        }
        else
        {
            StartIdx = gCalLens_FocusRangeTable[zoomSection-1][0];
        }

        if(StartIdx > Lens_Focus_GetPosition(FOCUS_MAX_POSITION))
        {
            StartIdx = Lens_Focus_GetPosition(FOCUS_MAX_POSITION);
        }
        Lens_Focus_Goto(StartIdx);

        EndIdx = gCalLens_FocusRangeTable[zoomSection-1][1];
        if(EndIdx < Lens_Focus_GetPosition(FOCUS_MIN_POSITION))
        {
            EndIdx = Lens_Focus_GetPosition(FOCUS_MIN_POSITION);
        }

        if(AF_Step > 4)
        {
            maxIdx = AF_SearchFullRange(StartIdx, EndIdx, AF_Step, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+AF_Step-4, maxIdx-AF_Step+4, 4, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+3, maxIdx-3, 1, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
        }
        else
        {
            maxIdx = AF_SearchFullRange(StartIdx, EndIdx, 4, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+8, maxIdx-8, 1, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
        }
        debug_msg("^G%s: Section = %d, maxIdx = %d\r\n", __func__, Lens_Zoom_GetSection(ZOOM_CUR_SECTION), maxIdx);
        gCalLens_Info.Near_Focus[zoomSection-1] = maxIdx;

        if(zoomSection==Lens_Zoom_GetSection(ZOOM_MIN_SECTION)){
            snprintf(str1, 32, "S  Step VA  ");
            Cal_ShowStringWithColor(str1,0,16*(zoomSection/2),_OSD_INDEX_BLACK);
            Cal_ShowStringWithColor(str1,100,16*(zoomSection/2),_OSD_INDEX_BLACK);
            Cal_ShowStringWithColor(str1,200,16*(zoomSection/2),_OSD_INDEX_BLACK);
            snprintf(str1, 32, "%02d %03d %d", zoomSection, gCalLens_Info.Near_Focus[zoomSection-1], maxVA);
        }
        else
            snprintf(str1, 32, "%02d %03d %d",zoomSection, gCalLens_Info.Near_Focus[zoomSection-1], maxVA);

        if(maxVA > CALLENS_VA_WARNING)
        {
            if(zoomSection % 3 == 1)
                Cal_ShowStringWithColor(str1,0,16+16*((zoomSection-1)/3),_OSD_INDEX_BLACK);
            else if(zoomSection % 3 == 2)
                Cal_ShowStringWithColor(str1,100,16+16*((zoomSection-1)/3),_OSD_INDEX_RED);
            else
                Cal_ShowStringWithColor(str1,200,16+16*((zoomSection-1)/3),_OSD_INDEX_BLACK);
        }
        else
        {
            if(zoomSection % 3 == 1)
                Cal_ShowStringWithColor(str1,0,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);
            else if(zoomSection % 3 == 2)
                Cal_ShowStringWithColor(str1,100,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);
            else
                Cal_ShowStringWithColor(str1,200,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);

            if(maxVA < CALLENS_VA_NG)
            {
                bFocusNG =TRUE;
                Delay_DelayMs(2000);
                break;
            }
        }
    }

    for(zoomSection=Lens_Zoom_GetSection(ZOOM_MIN_SECTION);zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
    {
        debug_msg("^G%s: gCalLens_Info.Near_Focus[%d] = %d\r\n", __func__,zoomSection-1 , gCalLens_Info.Near_Focus[zoomSection-1]);
    }

    gCalLens_Info.bNearAdjust = TRUE;
    if(CalLens_IsFarAdjust()){
        for(zoomSection=Lens_Zoom_GetSection(ZOOM_MIN_SECTION);zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
        {
            if(CalLens_GetFarFocusCalStep(zoomSection) < CalLens_GetNearFocusCalStep(zoomSection))
                bFocusNG = TRUE;

            if(bFocusNG)
                debug_msg("^RZoom=%2d FarFocusStep=%3d < NearFocusStep=%3d\r\n",zoomSection,CalLens_GetFarFocusCalStep(zoomSection),CalLens_GetNearFocusCalStep(zoomSection));
            else
                debug_msg("^GZoom=%2d FarFocusStep=%3d > NearFocusStep=%3d\r\n",zoomSection,CalLens_GetFarFocusCalStep(zoomSection),CalLens_GetNearFocusCalStep(zoomSection));
        }
    }
    if(bFocusNG)
        gCalLens_Info.bNearAdjust = FALSE;

    UILens_AFPrepare(FALSE);

    CalLens_WriteCalData();
    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_DEFAULT);

    IPL_AlgSetUIInfo(IPL_SEL_FREQUENCY,tempFrequency);

    StartFunc=0;
    StartFunc |=UIAPP_PHOTO_AE;
    StartFunc |=UIAPP_PHOTO_AWB;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);

    Lens_Retract(0);
    Delay_DelayMs(1000);

    /************************************************************/
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    if(bFocusNG)
        Cal_ShowStringWithColor("1. Adjust NG (near(0.5))",0,0,_OSD_INDEX_DARKGRAY);
    else
        Cal_ShowStringWithColor("1. Adjust done (near(0.5))",0,0,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("2. Press menu key to leave",0,30,_OSD_INDEX_BLACK);
    #if 0
    clr_flg(FLG_ID_KEY, FLGKEY_ENTER);
    wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_ENTER, TWF_ORW|TWF_CLR);
    #else
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_MENU);
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()==FLGKEY_MENU);
    Ux_FlushEvent();
    #endif
    /*************************************************************/

    return CALLENS_STA_OK;
}

UINT8 CalLens_FarFocusCalibration(void)
{
    INT16 maxIdx = 0;
    INT16 StartIdx,EndIdx;
    UINT  AF_Step;
    char str1[32];
    UINT32 maxVA;
    BOOL bFocusNG = FALSE;
    UINT32 StopFunc=0,StartFunc=0;
    UINT32 tempFrequency=0;
    UINT32 zoomSection;

    /************************************************************/
    debug_err(("\n\nPlease let camera facing ISO chart. Then press shutter key\r\n\n"));
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    Cal_ShowStringWithColor("1. Face ISO chart (far(3m))",0,0,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("2. Press shutter key",0,30,_OSD_INDEX_BLACK);
    #if 0
    clr_flg(FLG_ID_KEY, FLGKEY_SHUTTER2);
    wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_SHUTTER2, TWF_ORW|TWF_CLR);
    #else
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_SHUTTER1);
    #endif
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    /*************************************************************/

    //tempFrequency = IPL_AlgGetUIInfo(IPL_SEL_FREQUENCY);
    //IPL_AlgSetUIInfo(IPL_SEL_FREQUENCY,SEL_FREQUENCY_50HZ);
    #if 1
    StartFunc=0;
    StartFunc |=UIAPP_PHOTO_AE;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);

    AE_reset();
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    IPC_WaitVD(TRUE,IPL_ID_1);
    AE_Wait_Stable(3,60);

    StopFunc |=UIAPP_PHOTO_AE;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);
    #endif

    StopFunc |=UIAPP_PHOTO_AE;
    StopFunc |=UIAPP_PHOTO_AWB;
    StopFunc |=UIAPP_PHOTO_FD;
    StopFunc |=UIAPP_PHOTO_CAF;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);

    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_NULL);

    Lens_Zoom_SetPPS(300);
    Lens_Focus_SetPPS(500);

    Lens_Init(LENS_INIT_ZOOM_PART1);
    Lens_Init(LENS_INIT_ZOOM_PART2);

    if (gCalLens_Info.bFocusBLCal == FALSE)
    {
        gCalLens_Info.uiLensFocusBL = CalLens_FocusBLCalibration();
    }

    UILens_AFPrepare(TRUE);

    AF_Step=4;
    for(zoomSection=Lens_Zoom_GetSection(ZOOM_MIN_SECTION);zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
    {
        //Lens_Focus_GotoHome();
        Lens_Zoom_GoSection(zoomSection);

        #if 0
        StartFunc=0;
        StartFunc |=UIAPP_PHOTO_AE;
        Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);
        AE_reset();
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        IPC_WaitVD(TRUE,IPL_ID_1);
        AE_Wait_Stable(3,60);
        StopFunc |=UIAPP_PHOTO_AE;
        Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);
        #endif

        StartIdx = gCalLens_FocusRangeTable[zoomSection-1][0];
        if(StartIdx > Lens_Focus_GetPosition(FOCUS_MAX_POSITION))
        {
            StartIdx = Lens_Focus_GetPosition(FOCUS_MAX_POSITION);
        }
        Lens_Focus_Goto(StartIdx);

        if(CalLens_IsNearAdjust())
        {
            EndIdx = CalLens_GetNearCalCompensation(zoomSection, FALSE);
        }
        else
        {
            EndIdx = gCalLens_FocusRangeTable[zoomSection-1][1];
        }

        if(EndIdx < Lens_Focus_GetPosition(FOCUS_MIN_POSITION))
        {
            EndIdx = Lens_Focus_GetPosition(FOCUS_MIN_POSITION);
        }

        if(AF_Step > 4)
        {
            maxIdx = AF_SearchFullRange(StartIdx, EndIdx, AF_Step, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+AF_Step-4, maxIdx-AF_Step+4, 4, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+3, maxIdx-3, 1, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
        }
        else
        {
            maxIdx = AF_SearchFullRange(StartIdx, EndIdx, 4, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
            maxIdx = AF_SearchFullRange(maxIdx+8, maxIdx-8, 1, &maxVA, AF_VA_THRESHOLD_70PERCENTAGE);
        }
        debug_msg("^G%s: Section = %d, maxIdx = %d\r\n", __func__, Lens_Zoom_GetSection(ZOOM_CUR_SECTION), maxIdx);
        gCalLens_Info.Far_Focus[zoomSection-1] = maxIdx;

        if(zoomSection==1){
            snprintf(str1, 32, "S  Step VA  ");
            Cal_ShowStringWithColor(str1,0,16*(zoomSection/2),_OSD_INDEX_GREEN);
            Cal_ShowStringWithColor(str1,100,16*(zoomSection/2),_OSD_INDEX_GREEN);
            Cal_ShowStringWithColor(str1,200,16*(zoomSection/2),_OSD_INDEX_GREEN);
            snprintf(str1, 32, "%02d %03d %d", zoomSection, gCalLens_Info.Far_Focus[zoomSection-1], maxVA);
        }
        else
            snprintf(str1, 32, "%02d %03d %d", zoomSection, gCalLens_Info.Far_Focus[zoomSection-1], maxVA);

        if(maxVA > CALLENS_VA_WARNING)
        {
            if(zoomSection % 3 == 1)
                Cal_ShowStringWithColor(str1,0,16+16*((zoomSection-1)/3),_OSD_INDEX_BLACK);
            else if(zoomSection % 3 == 2)
                Cal_ShowStringWithColor(str1,100,16+16*((zoomSection-1)/3),_OSD_INDEX_RED);
            else
                Cal_ShowStringWithColor(str1,200,16+16*((zoomSection-1)/3),_OSD_INDEX_BLACK);
        }
        else
        {
            if(zoomSection % 3 == 1)
                Cal_ShowStringWithColor(str1,0,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);
            else if(zoomSection % 3 == 2)
                Cal_ShowStringWithColor(str1,100,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);
            else
                Cal_ShowStringWithColor(str1,200,16+16*((zoomSection-1)/3),_OSD_INDEX_DARKGRAY);

            if(maxVA < CALLENS_VA_NG)
            {
                bFocusNG =TRUE;
                Delay_DelayMs(2000);
                break;
            }
        }
    }

    for(zoomSection=Lens_Zoom_GetSection(ZOOM_MIN_SECTION);zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
    {
        debug_msg("^G%s: gCalLens_Info.Far_Focus[%d] = %d\r\n", __func__,zoomSection-1 , gCalLens_Info.Far_Focus[zoomSection-1]);
    }

    gCalLens_Info.bFarAdjust = TRUE;
    if(CalLens_IsNearAdjust()){
        for(zoomSection=1;zoomSection<=Lens_Zoom_GetSection(ZOOM_MAX_SECTION);zoomSection++)
        {
            if(CalLens_GetFarFocusCalStep(zoomSection) < CalLens_GetNearFocusCalStep(zoomSection))
                bFocusNG = TRUE;

            if(bFocusNG)
                debug_msg("^RZoom=%2d FarFocusStep=%3d < NearFocusStep=%3d\r\n",zoomSection,CalLens_GetFarFocusCalStep(zoomSection),CalLens_GetNearFocusCalStep(zoomSection));
            else
                debug_msg("^GZoom=%2d FarFocusStep=%3d > NearFocusStep=%3d\r\n",zoomSection,CalLens_GetFarFocusCalStep(zoomSection),CalLens_GetNearFocusCalStep(zoomSection));
        }
    }
    if(bFocusNG)
        gCalLens_Info.bFarAdjust = FALSE;

    UILens_AFPrepare(FALSE);

    CalLens_WriteCalData();
    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_DEFAULT);

    IPL_AlgSetUIInfo(IPL_SEL_FREQUENCY,tempFrequency);

    StartFunc = 0;
    StartFunc |=UIAPP_PHOTO_AE;
    StartFunc |=UIAPP_PHOTO_AWB;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);

    Lens_Retract(0);
    Delay_DelayMs(1000);

    /************************************************************/
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    if(bFocusNG)
        Cal_ShowStringWithColor("1. Adjust NG (far(3m))",0,0,_OSD_INDEX_DARKGRAY);
    else
        Cal_ShowStringWithColor("1. Adjust done (far(3m))",0,0,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("2. Press menu key to leave",0,30,_OSD_INDEX_BLACK);
    #if 0
    clr_flg(FLG_ID_KEY, FLGKEY_ENTER);
    wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_ENTER, TWF_ORW|TWF_CLR);
    #else
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_MENU);
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()==FLGKEY_MENU);
    Ux_FlushEvent();
    #endif
    /*************************************************************/
    return CALLENS_STA_OK;
}

INT32 CalLens_GetNearFocusCalStep(UINT32 zoomSection)
{
    INT32 retValue = 0;

    if(zoomSection > CALLENS_ZOOM_SECTION_NUM)
        return 0;

    if(CalLens_IsNearAdjust())
    {
        retValue = gCalLens_Info.Near_Focus[zoomSection-1];
    }
    else
    {
        retValue = gCalLens_FocusRangeTable[zoomSection-1][1];
    }

    return retValue;
}


INT32 CalLens_GetFarFocusCalStep(UINT32 zoomSection)
{
    INT32 retValue = 0;

    if(zoomSection > CALLENS_ZOOM_SECTION_NUM)
        return 0;

    if(CalLens_IsFarAdjust())
    {
        retValue = gCalLens_Info.Far_Focus[zoomSection-1];
    }
    else
    {
        retValue = gCalLens_FocusRangeTable[zoomSection-1][0];
    }

    return retValue;
}

INT32 CalLens_GetNearCalCompensation(UINT32 zoomSection, BOOL bCompensate)
{
    INT32 retValue = 0;
    UINT32 uiCalandSpecRatio=0, uiCalDiff=0, uiSpecDiff=0;
    UINT32 uiMacroSpecDiff=0;

    if(CalLens_IsNearAdjust())
        retValue = gCalLens_Info.Near_Focus[zoomSection-1];
    else
        retValue = gCalLens_FocusRangeTable[zoomSection-1][1];

    if(bCompensate)
    {
        if (SysGetFlag(FL_MACRO) == MACRO_ON)
        {
            uiCalDiff = CalLens_GetFarFocusCalStep(zoomSection) - CalLens_GetNearFocusCalStep(zoomSection);
            uiSpecDiff = g_FocusTableFromSpec[zoomSection-1][3] - g_FocusTableFromSpec[zoomSection-1][8]; //3m & 0.5m
            uiCalandSpecRatio = abs(uiCalDiff - uiSpecDiff)*100/uiSpecDiff;
            uiMacroSpecDiff = g_FocusTableFromSpec[zoomSection-1][8] - g_FocusTableFromSpec[zoomSection-1][10]; //0.5m & 0.2m
            if (uiCalDiff > uiSpecDiff)
            {
                retValue -= (uiMacroSpecDiff + uiMacroSpecDiff*uiCalandSpecRatio/100 + 20);
            }
            else if (uiCalDiff < uiSpecDiff)
            {
                retValue -= (uiMacroSpecDiff - uiMacroSpecDiff*uiCalandSpecRatio/100 + 20);
            }
            else
            {
                retValue -= (uiMacroSpecDiff + 20);
            }
        }
        else
        {
            retValue -= 10;
        }
    }

    if (retValue < Lens_Focus_GetPosition(FOCUS_MIN_POSITION))
    {
        retValue = Lens_Focus_GetPosition(FOCUS_MIN_POSITION);
    }

    return retValue;
}

INT32 CalLens_GetFarCalCompensation(UINT32 zoomSection, BOOL bCompensate)
{
    INT32 retValue = 0;
    UINT32 uiCalandSpecRatio=0, uiCalDiff=0, uiSpecDiff=0;
    UINT32 uiInfSpecDiff=0;

    if(CalLens_IsFarAdjust())
        retValue = gCalLens_Info.Far_Focus[zoomSection-1];
    else
        retValue = gCalLens_FocusRangeTable[zoomSection-1][0];

    if(bCompensate)
    {
        uiCalDiff = CalLens_GetFarFocusCalStep(zoomSection) - CalLens_GetNearFocusCalStep(zoomSection); //3m & 0.5m
        uiSpecDiff = g_FocusTableFromSpec[zoomSection-1][3] - g_FocusTableFromSpec[zoomSection-1][8];   //3m & 0.5m
        uiCalandSpecRatio = abs(uiCalDiff - uiSpecDiff)*100/uiSpecDiff;
        uiInfSpecDiff = g_FocusTableFromSpec[zoomSection-1][0] - g_FocusTableFromSpec[zoomSection-1][3]; //infinite & 3m
        if (uiCalDiff > uiSpecDiff)
        {
            retValue += (uiInfSpecDiff + uiInfSpecDiff*uiCalandSpecRatio/100 + 4);
        }
        else if (uiCalDiff < uiSpecDiff)
        {
            retValue += (uiInfSpecDiff - uiInfSpecDiff*uiCalandSpecRatio/100 + 4);
        }
        else
        {
            retValue += (uiInfSpecDiff + 4);
        }
    }

    if (retValue > Lens_Focus_GetPosition(FOCUS_MAX_POSITION))
    {
        retValue = Lens_Focus_GetPosition(FOCUS_MAX_POSITION);
    }

    return retValue;
}

INT32 CalLens_GetCapCompensiteStep(UINT32 zoomSection)
{
    return 0;
}

UINT32 CalLens_ZoomBLCalibration(void)
{
    return 0;
}

UINT32 CalLens_FocusBLCalibration(void)
{
    UINT32 i, loop=2;
    UINT32 curBL, maxBL=0;

    for(i=0; i<loop; i++)
    {
        curBL = Lens_Focus_GetBL();
        Delay_DelayMs(100);
        if(curBL > maxBL)
            maxBL = curBL;
    }

    debug_msg("%s: maxBL = %d\r\n",__func__ ,maxBL);
    if (maxBL == 0)
    {
        maxBL = 1;
    }
    else if (maxBL == 1)
    {
        maxBL += 1;
    }

    gCalLens_Info.bFocusBLCal = TRUE;

    return maxBL;
}

UINT32 CalLens_FocusStepMissTest(void)
{
    UINT32 i;
    CHAR str[32];
    extern UINT32 focusHomeDiff;
    extern UINT32 zoomHomeDiff;
    UINT32 StopFunc=0,StartFunc=0;

    /************************************************************/
    debug_err(("\n\nRunning FocusMiss Test,Please wait..\r\n\n"));
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    Cal_ShowStringWithColor("1. Run Focus Miss Test...",0,0,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("2. Press shutter key",0,30,_OSD_INDEX_BLACK);
    #if 0
    clr_flg(FLG_ID_KEY, FLGKEY_SHUTTER2);
    wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_SHUTTER2, TWF_ORW|TWF_CLR);
    #else
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_SHUTTER1);
    #endif
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    /*************************************************************/

    StopFunc |=UIAPP_PHOTO_AE;
    StopFunc |=UIAPP_PHOTO_AWB;
    StopFunc |=UIAPP_PHOTO_FD;
    StopFunc |=UIAPP_PHOTO_CAF;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_STOP_FUNC,2,StopFunc ,UIAPP_PHOTO_WAITIDLE);

    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_NULL);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_NULL);

    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    Cal_ShowStringWithColor("1.Running Focus Miss Test...",0,0,_OSD_INDEX_BLACK);

    Lens_Zoom_SetPPS(300);
    Lens_Focus_SetPPS(500);

    Lens_Retract(0);
    Delay_DelayMs(500);
    Lens_Init(LENS_INIT_ZOOM_PART1);
    Lens_Init(LENS_INIT_ZOOM_PART2);

    for(i=0;i<2;i++)
    {
        lensDY5001A_test();

        UILens_WaitZoomFinish();

        Delay_DelayMs(500);
    }

    Lens_Retract(1);
    Delay_DelayMs(500);
    Lens_Init(LENS_INIT_ZOOM_PART1);
    Lens_Init(LENS_INIT_ZOOM_PART2);

    StartFunc = 0;
    StartFunc |=UIAPP_PHOTO_AE;
    StartFunc |=UIAPP_PHOTO_AWB;
    Ux_SendEvent(&CustomPhotoObjCtrl,NVTEVT_EXE_START_FUNC,2,StartFunc ,UIAPP_PHOTO_NOWAITIDLE);

    snprintf(str, 32, "2. Diff Zoom(%d),Focus(%d)", zoomHomeDiff, focusHomeDiff);
    Cal_ShowStringWithColor(str,0,30,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("3. Press menu key to leave",0,60,_OSD_INDEX_BLACK);

    //CalLens_WaitKey(FLGKEY_MENU);
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_MENU);
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()==FLGKEY_MENU);
    Delay_DelayMs(100);
    Ux_FlushEvent();
    Input_SetKeyMask(KEY_PRESS, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_RELEASE, FLGKEY_KEY_MASK_DEFAULT);
    Input_SetKeyMask(KEY_CONTINUE, FLGKEY_KEY_MASK_DEFAULT);

    return 0;
}

UINT32 CalLens_LensQualityTest(void)
{
    //FLGPTN  uiFlag;
    UINT32  i;//, startIdx, endIdx, maxIdx, maxVA;
    char str1[40];
    extern UINT32 focusHomeDiff;
    extern UINT32 zoomHomeDiff;
    UINT32 focusmissstep[3]={0};
    UINT32 zoommissstep[3]={0};

    /************************************************************/
    debug_err(("\n\nRunning FocusMiss Test,Please wait..\r\n\n"));
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);
    Cal_ShowStringWithColor("1. Please wait..",0,0,_OSD_INDEX_BLACK);
    /*************************************************************/

    Lens_Zoom_SetPPS(300);
    Lens_Focus_SetPPS(500);

    Lens_Retract(0);
    Delay_DelayMs(500);

    for(i=1;i<10000;i++){

        Lens_Init(LENS_INIT_ZOOM_PART1);
        Lens_Init(LENS_INIT_ZOOM_PART2);
        UILens_WaitZoomFinish();

        Lens_Zoom_GoSection(Lens_Zoom_GetSection(ZOOM_MIN_SECTION));

        Lens_Focus_Goto(10);//Lens_Focus_GetPosition(FOCUS_MIN_POSITION));
        Lens_Focus_Goto(715);//Lens_Focus_GetPosition(FOCUS_MAX_POSITION));
        //Lens_Focus_GotoHome();

        Lens_Zoom_GoSection(Lens_Zoom_GetSection(ZOOM_MAX_SECTION));

        Lens_Retract(1);
        Delay_DelayMs(100);

        Cal_ClearOSD(_OSD_INDEX_TRANSPART);
        Cal_ShowStringWithColor("1. Running..",0,0,_OSD_INDEX_BLACK);
        Cal_ShowStringWithColor("2. Press menu key to leave",0,30,_OSD_INDEX_BLACK);
        snprintf(str1, 40, "3. %d times Diff Zoom(%d) Focus(%d)", i,zoomHomeDiff, focusHomeDiff);
        Cal_ShowStringWithColor(str1,0,60,_OSD_INDEX_BLACK);

        if((zoomHomeDiff<4)&&(zoomHomeDiff>0))zoommissstep[zoomHomeDiff-1] = zoomHomeDiff;
        if((focusHomeDiff<4)&&(focusHomeDiff>0))focusmissstep[focusHomeDiff-1] = focusHomeDiff;

        if((zoomHomeDiff>=4)||(focusHomeDiff>=4)){
            Cal_ShowStringWithColor("4. Press movie key to continue",0,90,_OSD_INDEX_BLACK);
            do{
                Delay_DelayMs(10);
            }while(DrvKey_DetNormalKey()!=FLGKEY_MOVIE);
            do{
                Delay_DelayMs(10);
            }while(DrvKey_DetNormalKey()==FLGKEY_MOVIE);
            Ux_FlushEvent();
        }
        if((DrvKey_DetNormalKey() & FLGKEY_MENU))
        {
            do{
                Delay_DelayMs(10);
            }while(DrvKey_DetNormalKey()==FLGKEY_MENU);
            Ux_FlushEvent();
            break;
        }
        zoomHomeDiff = 0;
        focusHomeDiff = 0;
    }

    snprintf(str1, 40, "3. zoom  %d  %d  %d focus  %d  %d  %d", zoommissstep[0],zoommissstep[1], zoommissstep[2],focusmissstep[0],focusmissstep[1],focusmissstep[2]);
    Cal_ShowStringWithColor(str1,0,60,_OSD_INDEX_BLACK);
    Cal_ShowStringWithColor("4. Test finish!Press menu key to leave",0,90,_OSD_INDEX_BLACK);

    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=FLGKEY_MENU);
    do{
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()==FLGKEY_MENU);
    Ux_FlushEvent();

    return 0;
}
#endif
