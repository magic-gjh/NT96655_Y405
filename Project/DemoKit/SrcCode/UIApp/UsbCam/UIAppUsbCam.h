/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       AppInitUSB.h
    @ingroup    mIPRJAPCfg

    @brief      Header file of AppInitUSB.c
                Header file of AppInitUSB.c

    @note       Nothing.

    @version    V1.00.000
    @author     Chris Hsu
    @date       2005/12/09
*/

/** \addtogroup mIPRJAPCfg */
//@{

#ifndef _UIAPPUSBCAM_H
#define _UIAPPUSBCAM_H

#include "UIFramework.h"
#include "Type.h"
#include "UVAC.h"

#if (UVC_RECORD_FUNC == ENABLE)
typedef struct
{
    UINT32  uiBufW;             ///< image buffer width
    UINT32  uiBufH;             ///< image buffer height
    UINT32  uiImgW_4_3;         ///< image width for 4:3 image ratio
    UINT32  uiImgH_4_3;         ///< image height for 4:3 image ratio
    UINT32  uiImgW_16_9;        ///< image width for 16:9 image ratio
    UINT32  uiImgH_16_9;        ///< image height for 16:9 image ratio
    UINT32  uiUvcSizeId_4_3;    ///< UVC size ID for 4:3 image ratio
    UINT32  uiUvcSizeId_16_9;   ///< UVC size ID for 16:9 image ratio
    UINT32  uiUvcSizeMapping;   ///< UVC size bit mapping
} UVCREC_IMG_DATA, *PUVCREC_IMG_DATA;

typedef enum
{
    UVCREC_IMG_ID_1,
    UVCREC_IMG_ID_2,
    UVCREC_IMG_ID_3,
    UVCREC_IMG_ID_MAX,
    ENUM_DUMMY4WORD(UVCREC_IMG_ID)
} UVCREC_IMG_ID;

extern void UvcRec_Open(void);
extern void UvcRec_Close(void);
extern void UvcRec_EnStartVideoCB(BOOL bEn);
extern void UvcRec_EnStopVideoCB(BOOL bEn);
extern BOOL UvcRec_IsUvcActive(void);
extern void UvcRec_SetImgID(UVCREC_IMG_ID ImgId);
extern PUVCREC_IMG_DATA UvcRec_GetImgData(void);
#endif

extern void     AppInit_ModeUSBPCC(void);

extern VControl CustomUSBPCCObjCtrl;

#endif

//@}
