#ifndef _CALLENS_H
#define _CALLENS_H

#include "PStore.h"

#define PS_CAL_LENS                         "LENS"

#if (0)//(_LENSDRVLIB_ == _LENSDRVLIB_DY5001A_)
#define CALLENS_ZOOM_SECTION_NUM            37
#define CALLENS_FOCUS_TABLE_COLUMN          11
#define CALLENS_VA_NG                       10000
#define CALLENS_VA_WARNING                  30000
#else
#define CALLENS_ZOOM_SECTION_NUM            1
#endif

typedef struct {
    BOOL        bFarAdjust;
    BOOL        bNearAdjust;
    INT32       Far_Focus[CALLENS_ZOOM_SECTION_NUM];
    INT32       Near_Focus[CALLENS_ZOOM_SECTION_NUM];
    BOOL        bZoomBLCal;
    BOOL        bFocusBLCal;
    UINT32      uiLensFocusBL;
    UINT32      uiLensZoomBL;
    INT32       Far_Focus_Small[CALLENS_ZOOM_SECTION_NUM];
    INT32       Near_Focus_Small[CALLENS_ZOOM_SECTION_NUM];
    BOOL        bFocusStepMissTest;
    UINT32      uiZoomMissStep;
    UINT32      uiFocusMissStep;
    INT32        uiFocusCapCompensiteStep[CALLENS_ZOOM_SECTION_NUM];
} CALLENS_INFO, *PCALLENS_INFO;

typedef enum {
    CALLENS_STA_OK,
    CALLENS_STA_FAIL
} _CALLENS_STATUS_;

extern volatile CALLENS_INFO gCalLens_Info;

extern void     Cal_AFFar(void);
extern void     Cal_AFNear(void);
extern void     Cal_FocusOffset(void);
extern void     Cal_LENS(void);
extern void     Cal_MShutterSWTest(void);
extern void     Cal_ApertureSWTest(void);
extern void     Cal_FAETest(void);
extern void     Cal_ZoomSpeedTest(void);
extern void     Cal_ZoomStepMissTest(void);
extern void     Cal_ZoomStop2PR(void);
extern void     Cal_FocusStepMissTest(void);
extern void     Cal_AFTest(void);
extern void     Cal_FocusComp(void);
extern void     Cal_LensQualityTest(void);

extern UINT32   CalLens_GetFocusBL(void);
extern UINT32   CalLens_GetZoomBL(void);
extern INT32    CalLens_ReadCalData(void);
extern INT32    CalLens_WriteCalData(void);
extern void     CalLens_ResetCalData(void);
extern BOOL     CalLens_IsTestMissStep(UINT32 *pZoomMissStep, UINT32 *pFocusMissStep);
extern BOOL     CalLens_IsAdjust(void);
extern BOOL     CalLens_IsFarAdjust(void);
extern BOOL     CalLens_IsNearAdjust(void);
extern UINT8    CalLens_FarFocusCalibration(void);
extern UINT8    CalLens_NearFocusCalibration(void);
extern INT32    CalLens_GetFarFocusCalStep(UINT32 zoomSection);
extern INT32    CalLens_GetNearFocusCalStep(UINT32 zoomSection);
extern INT32    CalLens_GetFarCalCompensation(UINT32 zoomSection, BOOL bCompensate);
extern INT32    CalLens_GetNearCalCompensation(UINT32 zoomSection, BOOL bCompensate);
extern INT32    CalLens_GetCapCompensiteStep(UINT32 zoomSection);
extern UINT32   CalLens_ZoomBLCalibration(void);
extern UINT32   CalLens_FocusBLCalibration(void);
extern UINT32   CalLens_FocusStepMissTest(void);
extern UINT32   CalLens_FocusBLAdjust(void);
extern UINT32   CalLens_LensQualityTest(void);

#endif // EOF
