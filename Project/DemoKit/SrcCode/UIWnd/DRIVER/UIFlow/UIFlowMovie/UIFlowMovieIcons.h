
#ifndef __UIFlowWndMovieIcons_H
#define __UIFlowWndMovieIcons_H
#include "UIFlow.h"
#include "type.h"


extern void FlowMovie_IconDrawDscMode(void);
extern void FlowMovie_IconHideDscMode(void);
extern void FlowMovie_IconDrawMaxRecTime(void);
extern void FlowMovie_IconHideMaxRecTime(void);
extern void FlowMovie_IconDrawRecTime(void);
extern void FlowMovie_IconDrawSize(void);
extern void FlowMovie_IconHideSize(void);
extern void FlowMovie_IconDrawStorage(void);
extern void FlowMovie_IconHideStorage(void);
extern void FlowMovie_IconDrawEV(void);
extern void FlowMovie_IconHideEV(void);
extern void FlowMovie_IconDrawBattery(void);
extern void FlowMovie_IconHideBattery(void);
extern void FlowMovie_DrawPIM(BOOL bDraw);
extern void FlowMovie_IconDrawDZoom(void);
extern void FlowMovie_IconHideDZoom(void);
extern void FlowMovie_IconDrawDateTime(void);
extern void FlowMovie_IconHideDateTime(void);

extern void FlowMovie_UpdateIcons(BOOL bShow);

#endif //__UIFlowWndMovieIcons_H
