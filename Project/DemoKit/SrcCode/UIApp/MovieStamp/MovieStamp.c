#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "SysCfg.h"
#include "Utility.h"
#include "HwClock.h"
#include "GxImage.h"
#include "MovieStamp.h"
#include "DateStampFont10x16.h"
#include "DateStampFont12x20.h"
#include "DateStampFont18x30.h"
#include "DateStampFont20x44.h"
#include "DateStampFont26x44.h"
#include "IQS_Utility.h"
#include "UIInfo.h"
#if (MULTILINGUAL_FUNCTION == ENABLE)
#include "UniFont.h"
#endif
#include "PrjCfg_Demo.h"

#define __MODULE__          MovieStamp
//#define __DBGLVL__ 0        //OFF mode, show nothing
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
//#define __DBGLVL__ 2        //TRACE mode, show err, wrn, ind, msg and func and ind, msg and func can be filtering by __DBGFLT__ settings
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"

#define CHECK_STAMP_TIME    DISABLE
#define CHECK_ENC_INFO      DISABLE

#define COLOR_BG_Y          0x00
#define COLOR_BG_U          0x80
#define COLOR_BG_V          0x80
#define COLOR_FR_Y          0x00
#define COLOR_FR_U          0x80
#define COLOR_FR_V          0x80
#define COLOR_FG_Y          RGB_GET_Y(250, 160, 10)
#define COLOR_FG_U          RGB_GET_U(250, 160, 10)
#define COLOR_FG_V          RGB_GET_V(250, 160, 10)
#define COLOR_ID_BG         0
#define COLOR_ID_FR         1
#define COLOR_ID_FG         2
#define COLORKEY_BG_YUV     0x00000000  // stamp BG data for color key operation
#define COLORKEY_YUV        0x00010101  // color key for transparent background

#define STAMP_WIDTH_TOLERANCE   8   // total font width error tolerance
#define STAMP_LOGO_GAP          8   // date stamp and water logo position gap

#define VIDEO_IN_MAX        2   // max 2 video paths
#define LINGUAL_MAX         32
#define CAR_NUMBER_STAMP_LEN	32
#define MOVIE_STAMP_STRING		""
extern UINT32 uiSpeed;

//variable declare
static STAMP_POS    g_MovieStampPos[VIDEO_IN_MAX] =
{
    {0, 0}, // stamp position for primary image 1
    {0, 0}  // stamp position for primary image 2
};
static char         g_cMovieStampStr[VIDEO_IN_MAX][256];
static UINT32       g_uiMovieStampSetup[VIDEO_IN_MAX] = {STAMP_OFF, STAMP_OFF};
static STAMP_INFO   g_MovieStampInfo[VIDEO_IN_MAX];
static struct tm    g_CurDateTime[VIDEO_IN_MAX];
static UINT32       g_uiMovieStampYAddr[VIDEO_IN_MAX] = {0};
//static UINT32       g_uiMovieStampHeightMax = 44; // maximum height of date stamp font (gDateStampFont26x44)
static BOOL         g_bStampColorSetup[VIDEO_IN_MAX] = {FALSE, FALSE};
static WATERLOGO_BUFFER g_MovieWaterInfo[VIDEO_IN_MAX] = {0};
static UINT32       g_uiWaterLogoYAddr[VIDEO_IN_MAX] = {0}, g_uiWaterLogoUVAddr[VIDEO_IN_MAX] = {0};
static char carnumber_stamp_str[CAR_NUMBER_STAMP_LEN] = {0};

#if (MULTILINGUAL_FUNCTION == ENABLE)
static UINT32       g_uiLingualStampYAddr[VIDEO_IN_MAX] = {0};
static UINT32       g_uiLingualScaleStampYAddr[VIDEO_IN_MAX] = {0};
static STAMP_POS    g_LingualStampPos[VIDEO_IN_MAX] =
{
    {0, 0}, // stamp position for primary image 1
    {0, 0}  // stamp position for primary image 2
};

// sample unicode string
UINT16 unicode[] = {
     0x53f0,0x7063,0x65b0,0x7af9,0x79d1,0x5b78,0x5de5,0x696d,0x5712,0x5340,0x7be4,0x884c,0x4e00,0x8def,0x0032,0x865f,0x0020,
  };

LINGUAL_INFO    LingualInfo[LINGUAL_MAX];
#endif //

//#NT#2014/4/22#Philex Lin -begin
// init water logo's buffer
#if (0)//(WATERLOGO_FUNCTION == ENABLE)
void MovieStamp_WaterLogoSetup(UINT32 uiVidEncId,WATERLOGO_BUFFER *pWaterLogoBuf)
{
    memcpy((void *)&g_MovieWaterInfo[uiVidEncId],(const void *)pWaterLogoBuf,sizeof(WATERLOGO_BUFFER));
    g_MovieWaterInfo[uiVidEncId].uiWaterLogoFormat = GX_IMAGE_PIXEL_FMT_YUV420_PACKED;
}
#endif
//#NT#2014/4/22#Philex Lin -end

void MovieStamp_Setup(UINT32 uiVidEncId, UINT32 uiFlag, UINT32 uiImageWidth, UINT32 uiImageHeight, WATERLOGO_BUFFER *pWaterLogoBuf)
{
    PSTAMP_INFO     pStampInfo;
    ICON_DB const   *pDB;
    UINT32          uiIconID;
    UINT32          uiStrOffset;

    g_uiMovieStampSetup[uiVidEncId] = uiFlag;

    if ((uiFlag & STAMP_SWITCH_MASK) == STAMP_OFF)
    {
        return;
    }
	memset(carnumber_stamp_str, 0, CAR_NUMBER_STAMP_LEN);
	if (SysGetFlag(FL_CAR_NUM) == CAR_NUM_ON)
	{
		if (SysGetFlag(FL_LANGUAGE) == LANG_SC)
		{
			strcpy(carnumber_stamp_str, SysGetZHCarNoStamp());
		}
		else
		{
			strcpy(carnumber_stamp_str, SysGetNZHCarNoStamp());
		}
	}
	else
	{
		sprintf(carnumber_stamp_str," ");
	}
    

    #if (WATERLOGO_FUNCTION == ENABLE)
    {
        // setup water logo
        memcpy((void *)&g_MovieWaterInfo[uiVidEncId], (const void *)pWaterLogoBuf, sizeof(WATERLOGO_BUFFER));
        g_MovieWaterInfo[uiVidEncId].uiWaterLogoYAddr = g_uiWaterLogoYAddr[uiVidEncId];
        g_MovieWaterInfo[uiVidEncId].uiWaterLogoUVAddr = g_uiWaterLogoUVAddr[uiVidEncId];
    }
    #endif

    pStampInfo = &g_MovieStampInfo[uiVidEncId];
    pStampInfo->pi8Str = &g_cMovieStampStr[uiVidEncId][0];

    // set date stamp font data base
    switch (uiImageWidth)
    {
    case 1920:  // 1920x1080
    case 1536:  // 1536x1536
        pStampInfo->pDataBase = &gDateStampFont18x30;//&gDateStampFont26x44;
        break;

    case 1728:  // 1728x1296 (DAR 16:9)
    case 1440:  // 1440x1080 (DAR 16:9)
        pStampInfo->pDataBase = &gDateStampFont18x30;
        g_MovieWaterInfo[uiVidEncId].uiWidth = (g_MovieWaterInfo[uiVidEncId].uiWidth * 3) / 4;
        break;

    case 1280:  // 1280x720
        pStampInfo->pDataBase = &gDateStampFont18x30;
        g_MovieWaterInfo[uiVidEncId].uiWidth = (g_MovieWaterInfo[uiVidEncId].uiWidth * 30) / 44;
        g_MovieWaterInfo[uiVidEncId].uiHeight = (g_MovieWaterInfo[uiVidEncId].uiHeight * 30) / 44;
        break;

    case 320:   // QVGA
        pStampInfo->pDataBase = &gDateStampFont18x30;//&gDateStampFont10x16;
        g_MovieWaterInfo[uiVidEncId].uiWidth = (g_MovieWaterInfo[uiVidEncId].uiWidth * 16) / 44;
        g_MovieWaterInfo[uiVidEncId].uiHeight = (g_MovieWaterInfo[uiVidEncId].uiHeight * 16) / 44;
        break;

    default:    // VGA & others
        pStampInfo->pDataBase = &gDateStampFont18x30;
        g_MovieWaterInfo[uiVidEncId].uiWidth = (g_MovieWaterInfo[uiVidEncId].uiWidth * 20) / 44;
        g_MovieWaterInfo[uiVidEncId].uiHeight = (g_MovieWaterInfo[uiVidEncId].uiHeight * 20) / 44;
        break;
    }

    #if (WATERLOGO_FUNCTION == ENABLE)
    {
        // do water logo scaling
        UINT32      uiLineOffset[2] = {0};
        UINT32      uiPxlAddr[2] = {0};
        IMG_BUF     SrcImg, DstImg;
        IRECT       SrcRegion = {0};
        IRECT       DstRegion = {0};

        g_MovieWaterInfo[uiVidEncId].uiWidth = ALIGN_CEIL_4(g_MovieWaterInfo[uiVidEncId].uiWidth);
        g_MovieWaterInfo[uiVidEncId].uiHeight = ALIGN_CEIL_4(g_MovieWaterInfo[uiVidEncId].uiHeight);

        // Init source buffer
        uiLineOffset[0] = pWaterLogoBuf->uiWaterLogoYLineOffset;
        uiLineOffset[1] = pWaterLogoBuf->uiWaterLogoYLineOffset;
        uiPxlAddr[0]    = pWaterLogoBuf->uiWaterLogoYAddr;
        uiPxlAddr[1]    = pWaterLogoBuf->uiWaterLogoUVAddr;

        GxImg_InitBufEx(&SrcImg,
                        pWaterLogoBuf->uiWidth,
                        pWaterLogoBuf->uiHeight,
                        GX_IMAGE_PIXEL_FMT_YUV422_PACKED,
                        uiLineOffset,
                        uiPxlAddr);

        // Init destination buffer
        uiLineOffset[0] = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYLineOffset;
        uiLineOffset[1] = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYLineOffset;
        uiPxlAddr[0]    = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYAddr;
        uiPxlAddr[1]    = g_MovieWaterInfo[uiVidEncId].uiWaterLogoUVAddr;

        GxImg_InitBufEx(&DstImg,
                        g_MovieWaterInfo[uiVidEncId].uiWidth,
                        g_MovieWaterInfo[uiVidEncId].uiHeight,
                        GX_IMAGE_PIXEL_FMT_YUV422_PACKED,
                        uiLineOffset,
                        uiPxlAddr);

        // Set scale region
        SrcRegion.x = 0;
        SrcRegion.y = 0;
        SrcRegion.w = pWaterLogoBuf->uiWidth;
        SrcRegion.h = pWaterLogoBuf->uiHeight;

        DstRegion.x = 0;
        DstRegion.y = 0;
        DstRegion.w = g_MovieWaterInfo[uiVidEncId].uiWidth;
        DstRegion.h = g_MovieWaterInfo[uiVidEncId].uiHeight;

        // Scale image by ISE
        GxImg_ScaleData(&SrcImg, &SrcRegion, &DstImg, &DstRegion);
    }
    #endif

    // set stamp string (for calculating stamp position)
    switch (uiFlag & STAMP_DATE_TIME_MASK)
    {
    case STAMP_DATE_TIME_AMPM:
        sprintf(pStampInfo->pi8Str, "0000/00/00 00:00:00 AM");
        break;

    case STAMP_DATE: // date only is not suitable for movie stamp (it's suitable for still image stamp)
    case STAMP_TIME:
        sprintf(pStampInfo->pi8Str, "00:00:00");
        break;

    case STAMP_DATE_TIME:
    default:
        sprintf(pStampInfo->pi8Str, "AAAAAAA    0000/00/00 00:00:00   ");
        //sprintf(pStampInfo->pi8Str, "AAAAAAA  %03d KM/H  0000/00/00 00:00:00   ");
        break;
    }

    // set font width and height (use the width of 1st font, so the total width may have some error)
    pDB = pStampInfo->pDataBase;
    uiStrOffset = pDB->uiDrawStrOffset;
    uiIconID = pStampInfo->pi8Str[0] - uiStrOffset; // 1st font
    pStampInfo->ui32FontWidth  = pDB->pIconHeader[uiIconID].uiWidth;
    pStampInfo->ui32FontHeight = pDB->pIconHeader[uiIconID].uiHeight;
    pStampInfo->ui32DstHeight  = pStampInfo->ui32FontHeight; // no scaling

    // Set date stamp position
    if ((uiFlag & STAMP_OPERATION_MASK) == STAMP_AUTO)
    {
        UINT32  uiStampWidth = (pStampInfo->ui32DstHeight * pStampInfo->ui32FontWidth) / pStampInfo->ui32FontHeight;

        switch (uiFlag & STAMP_POSITION_MASK)
        {
        case STAMP_TOP_LEFT:
            if ((uiFlag & STAMP_POS_END_MASK) == STAMP_POS_END)
            {
                g_MovieStampPos[uiVidEncId].uiX = 0;
                g_MovieStampPos[uiVidEncId].uiY = 0;
            }
            else
            {
                #if 0
                g_MovieStampPos[uiVidEncId].uiX = uiStampWidth * 2; // 2 fonts width gap
                g_MovieStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight; // 1 font height gap
                #else
                g_MovieStampPos[uiVidEncId].uiX = uiStampWidth; // 1 font width gap
                g_MovieStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight / 2; // 1/2 font height gap
                #endif
            }
            #if (MULTILINGUAL_FUNCTION == ENABLE)
            {
                g_LingualStampPos[uiVidEncId].uiX = uiStampWidth * (strlen(pStampInfo->pi8Str)+16);
                g_LingualStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight / 2;
            }
            #endif
            #if (WATERLOGO_FUNCTION == ENABLE)
            if (g_MovieWaterInfo[uiVidEncId].uiXPos == WATERLOGO_AUTO_POS && g_MovieWaterInfo[uiVidEncId].uiYPos == WATERLOGO_AUTO_POS)
            {
                g_MovieWaterInfo[uiVidEncId].uiXPos = g_MovieStampPos[uiVidEncId].uiX;
                g_MovieWaterInfo[uiVidEncId].uiYPos = g_MovieStampPos[uiVidEncId].uiY;
                g_MovieStampPos[uiVidEncId].uiX += (g_MovieWaterInfo[uiVidEncId].uiWidth + STAMP_LOGO_GAP);
            }
            #endif
            break;

        case STAMP_TOP_RIGHT:
            if ((uiFlag & STAMP_POS_END_MASK) == STAMP_POS_END)
            {
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * strlen(pStampInfo->pi8Str) - STAMP_WIDTH_TOLERANCE;
                g_MovieStampPos[uiVidEncId].uiY = 0;
            }
            else
            {
                #if 0
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * (strlen(pStampInfo->pi8Str) + 2); // 2 fonts width gap
                g_MovieStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight; // 1 font height gap
                #else
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * (strlen(pStampInfo->pi8Str) + 1); // 1 font width gap
                g_MovieStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight / 2; // 1/2 font height gap
                #endif
            }
            #if (MULTILINGUAL_FUNCTION == ENABLE)
            {
                g_LingualStampPos[uiVidEncId].uiX = uiStampWidth; // 1 font width gap
                g_LingualStampPos[uiVidEncId].uiY = pStampInfo->ui32DstHeight / 2;
            }
            #endif
            #if (WATERLOGO_FUNCTION == ENABLE)
            if (g_MovieWaterInfo[uiVidEncId].uiXPos == WATERLOGO_AUTO_POS && g_MovieWaterInfo[uiVidEncId].uiYPos == WATERLOGO_AUTO_POS)
            {
                g_MovieWaterInfo[uiVidEncId].uiXPos = g_MovieStampPos[uiVidEncId].uiX - g_MovieWaterInfo[uiVidEncId].uiWidth - STAMP_LOGO_GAP;
                g_MovieWaterInfo[uiVidEncId].uiYPos = g_MovieStampPos[uiVidEncId].uiY;
            }
            #endif
            break;

        case STAMP_BOTTOM_LEFT:
            if ((uiFlag & STAMP_POS_END_MASK) == STAMP_POS_END)
            {
                g_MovieStampPos[uiVidEncId].uiX = 0;
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - pStampInfo->ui32DstHeight;
            }
            else
            {
                #if 0
                g_MovieStampPos[uiVidEncId].uiX = uiStampWidth * 2; // 2 fonts width gap
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - pStampInfo->ui32DstHeight * 2; // 1 font height gap
                #else
                g_MovieStampPos[uiVidEncId].uiX = uiStampWidth; // 1 font width gap
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - (pStampInfo->ui32DstHeight * 3) / 2; // 1/2 font height gap
                #endif
            }
            #if (MULTILINGUAL_FUNCTION == ENABLE)
            {
                g_LingualStampPos[uiVidEncId].uiX = uiStampWidth * (strlen(pStampInfo->pi8Str)+16); // 1 font width gap
                g_LingualStampPos[uiVidEncId].uiY = uiImageHeight - (pStampInfo->ui32DstHeight * 3) / 2;
            }
            #endif
            #if (WATERLOGO_FUNCTION == ENABLE)
            if (g_MovieWaterInfo[uiVidEncId].uiXPos == WATERLOGO_AUTO_POS && g_MovieWaterInfo[uiVidEncId].uiYPos == WATERLOGO_AUTO_POS)
            {
                g_MovieWaterInfo[uiVidEncId].uiXPos = g_MovieStampPos[uiVidEncId].uiX;
                g_MovieWaterInfo[uiVidEncId].uiYPos = g_MovieStampPos[uiVidEncId].uiY + pStampInfo->ui32DstHeight - g_MovieWaterInfo[uiVidEncId].uiHeight;
                g_MovieStampPos[uiVidEncId].uiX += (g_MovieWaterInfo[uiVidEncId].uiWidth + STAMP_LOGO_GAP);
            }
            #endif
            break;

        case STAMP_BOTTOM_RIGHT:
        default:
            if ((uiFlag & STAMP_POS_END_MASK) == STAMP_POS_END)
            {
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * strlen(pStampInfo->pi8Str) - STAMP_WIDTH_TOLERANCE;
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - pStampInfo->ui32DstHeight;
            }
            else
            {
                #if 0
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * (strlen(pStampInfo->pi8Str) + 2); // 2 fonts width gap
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - pStampInfo->ui32DstHeight * 2; // 1 font height gap
                #else
                g_MovieStampPos[uiVidEncId].uiX = uiImageWidth - uiStampWidth * (strlen(pStampInfo->pi8Str) + 1); // 1 font width gap
                g_MovieStampPos[uiVidEncId].uiY = uiImageHeight - (pStampInfo->ui32DstHeight * 3) / 2; // 1/2 font height gap
                #endif
            }
            #if (MULTILINGUAL_FUNCTION == ENABLE)
            {
                g_LingualStampPos[uiVidEncId].uiX = uiStampWidth; // 1 font width gap
                g_LingualStampPos[uiVidEncId].uiY = uiImageHeight - (pStampInfo->ui32DstHeight * 3) / 2;
            }
            #endif
            #if (WATERLOGO_FUNCTION == ENABLE)
            if (g_MovieWaterInfo[uiVidEncId].uiXPos == WATERLOGO_AUTO_POS && g_MovieWaterInfo[uiVidEncId].uiYPos == WATERLOGO_AUTO_POS)
            {
                g_MovieWaterInfo[uiVidEncId].uiXPos = g_MovieStampPos[uiVidEncId].uiX - g_MovieWaterInfo[uiVidEncId].uiWidth - STAMP_LOGO_GAP;
                g_MovieWaterInfo[uiVidEncId].uiYPos = g_MovieStampPos[uiVidEncId].uiY + pStampInfo->ui32DstHeight - g_MovieWaterInfo[uiVidEncId].uiHeight;
            }
            #endif
            break;
        }
    }

    g_MovieStampPos[uiVidEncId].uiX = ALIGN_FLOOR_4(g_MovieStampPos[uiVidEncId].uiX);
    g_MovieStampPos[uiVidEncId].uiY = ALIGN_FLOOR_4(g_MovieStampPos[uiVidEncId].uiY);

    #if (MULTILINGUAL_FUNCTION == ENABLE)
    g_LingualStampPos[uiVidEncId].uiX = ALIGN_FLOOR_4(g_LingualStampPos[uiVidEncId].uiX);
    g_LingualStampPos[uiVidEncId].uiY = ALIGN_FLOOR_4(g_LingualStampPos[uiVidEncId].uiY);
    #endif

    #if (WATERLOGO_FUNCTION == ENABLE)
    g_MovieWaterInfo[uiVidEncId].uiXPos = ALIGN_FLOOR_4(g_MovieWaterInfo[uiVidEncId].uiXPos);
    g_MovieWaterInfo[uiVidEncId].uiYPos = ALIGN_FLOOR_4(g_MovieWaterInfo[uiVidEncId].uiYPos);
    #endif

    // set default stamp color if necessary
    if (g_bStampColorSetup[uiVidEncId] == FALSE)
    {
        // Stamp background color
        pStampInfo->Color[COLOR_ID_BG].ucY = COLOR_BG_Y;
        pStampInfo->Color[COLOR_ID_BG].ucU = COLOR_BG_U;
        pStampInfo->Color[COLOR_ID_BG].ucV = COLOR_BG_V;

        // Stamp frame color
        pStampInfo->Color[COLOR_ID_FR].ucY = COLOR_FR_Y;
        pStampInfo->Color[COLOR_ID_FR].ucU = COLOR_FR_U;
        pStampInfo->Color[COLOR_ID_FR].ucV = COLOR_FR_V;

        // Stamp foreground color (text body)
        pStampInfo->Color[COLOR_ID_FG].ucY = COLOR_FG_Y;
        pStampInfo->Color[COLOR_ID_FG].ucU = COLOR_FG_U;
        pStampInfo->Color[COLOR_ID_FG].ucV = COLOR_FG_V;
    }

    // Reset reference time
    g_CurDateTime[uiVidEncId].tm_sec = 61;

    #if (CHECK_STAMP_TIME == ENABLE)
    Perf_Open(); // for performance measurement
    #endif
}

void MovieStamp_SetPos(UINT32 uiVidEncId, UINT32 x, UINT32 y)
{
    g_MovieStampPos[uiVidEncId].uiX = x;
    g_MovieStampPos[uiVidEncId].uiY = y;
}

void MovieStamp_SetColor(UINT32 uiVidEncId, PSTAMP_COLOR pStampColorBg, PSTAMP_COLOR pStampColorFr, PSTAMP_COLOR pStampColorFg)
{
    g_bStampColorSetup[uiVidEncId] = TRUE;

    // Stamp background color
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucY = pStampColorBg->ucY;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucU = pStampColorBg->ucU;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucV = pStampColorBg->ucV;

    // Stamp frame color
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FR].ucY = pStampColorFr->ucY;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FR].ucU = pStampColorFr->ucU;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FR].ucV = pStampColorFr->ucV;

    // Stamp foreground color (text body)
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FG].ucY = pStampColorFg->ucY;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FG].ucU = pStampColorFg->ucU;
    g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_FG].ucV = pStampColorFg->ucV;
}

#if 0
UINT32 MovieStamp_GetMaxFontHeight(void)
{
    return g_uiMovieStampHeightMax;
}
#endif


#if (MULTILINGUAL_FUNCTION == ENABLE)
static UINT32 MovieStamp_GetLingualDataWidth(LINGUAL_INFO *pingualInfo,UINT32 uiStrLen)
{
    UINT32  i;
    UINT32  uiDataWidth;

    uiDataWidth = 0;
    for (i = 0; i < uiStrLen; i++)
    {
        uiDataWidth += pingualInfo[i].glyph.GLYPHINFO.wBoxX;
    }

    return ALIGN_CEIL_4(uiDataWidth);
}

static BOOL MovieStamp_DrawMultiLingualIcon420UV(PSTAMP_INFO pStampInfo,LINGUAL_INFO LingualInfo,UINT32 uiFBY, UINT32 uiFBCb, UINT32 uiFBCr, UINT32 uiLineOffset)
{
    UINT8   *uiIconData;
    UINT8   *pChar, *pCharU, *pCharV;
    UINT32  uiIconWidth;
    UINT32  uiIconHeight;
    UINT32  uiPixelCount;
    UINT32  i, j;
    UINT32  uiIndex;

    uiIconWidth = LingualInfo.glyph.GLYPHINFO.wBoxX;
    uiIconHeight = LingualInfo.glyph.GLYPHINFO.wBoxY;
    uiIconData = &LingualInfo.GlyphData[0];

    uiPixelCount = 0;

    for(i = 0; i < uiIconHeight; i++)
    {
        pChar = (UINT8 *)uiFBY;
        pCharU = (UINT8 *)uiFBCb;
        pCharV = (UINT8 *)uiFBCb+1;

        for(j = 0; j < uiIconWidth; j ++)
        {
            uiIndex = (*uiIconData >> (6 - (uiPixelCount % 4 * 2))) & 0x03;

            if (uiIndex)
            {
                if (uiIndex == 1) // frame color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FR].ucY;
                }
                else // foreground color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FG].ucY;
                }

                if (((j&1)==0)&&((i&1)==0))
                {
                    if (uiIndex == 1) // frame color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FR].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FR].ucV;
                    }
                    else // foreground color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FG].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FG].ucV;
                    }
                }
            }
            pChar++;
            if ((j&1)==0)
            {
                pCharU+=2;
                pCharV+=2;
            }
            uiPixelCount ++;
            if (uiPixelCount % 4 == 0)
                uiIconData ++;

        }
        uiFBY += uiLineOffset;
        if  ((i&1) == 0)
        {
            uiFBCb += uiLineOffset;
            uiFBCr += uiLineOffset;
        }
    }
    return TRUE;

}

static BOOL MovieStamp_DrawMultiLingualIcon422UV(PSTAMP_INFO pStampInfo,LINGUAL_INFO LingualInfo,UINT32 uiFBY, UINT32 uiFBCb, UINT32 uiFBCr, UINT32 uiLineOffset)
{
    UINT8   *uiIconData;
    UINT8   *pChar, *pCharU, *pCharV;
    UINT32  uiIconWidth;
    UINT32  uiIconHeight;
    UINT32  uiPixelCount;
    UINT32  i, j;
    UINT32  uiIndex;

    uiIconWidth = LingualInfo.glyph.GLYPHINFO.wBoxX;
    uiIconHeight = LingualInfo.glyph.GLYPHINFO.wBoxY;
    uiIconData = &LingualInfo.GlyphData[0];

    uiPixelCount = 0;

    for(i = 0; i < uiIconHeight; i++)
    {
        pChar = (UINT8 *)uiFBY;
        pCharU = (UINT8 *)uiFBCb;
        pCharV = (UINT8 *)uiFBCb+1;

        for(j = 0; j < uiIconWidth; j ++)
        {
            uiIndex = (*uiIconData >> (6 - (uiPixelCount % 4 * 2))) & 0x03;

            if (uiIndex)
            {
                if (uiIndex == 1) // frame color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FR].ucY;
                }
                else // foreground color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FG].ucY;
                }

                if ((j&1)==0)
                {
                    if (uiIndex == 1) // frame color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FR].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FR].ucV;
                    }
                    else // foreground color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FG].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FG].ucV;
                    }
                }
            }
            pChar++;
            if ((j&1)==0)
            {
                pCharU+=2;
                pCharV+=2;
            }
            uiPixelCount ++;
            if (uiPixelCount % 4 == 0)
                uiIconData ++;

        }
        uiFBY += uiLineOffset;
//        if  ((i&1) == 0)
        {
            uiFBCb += uiLineOffset;
            uiFBCr += uiLineOffset;
        }
    }
    return TRUE;

}

static UINT32 MovieStamp_DrawMultiLingualString(PSTAMP_INFO pStampInfo, PSTAMP_BUFFER pStampBuffer,UINT16 *pUniCodeArr,UINT32 datalen)
{
 UINT32  i, j;
 UINT32  uiFBAddrY, uiFBAddrCb, uiFBAddrCr;
 UINT32  uiDataWidth;

    uiFBAddrY  = pStampBuffer->uiYAddr;
    uiFBAddrCb = pStampBuffer->uiUAddr;
    uiFBAddrCr = pStampBuffer->uiVAddr;

    uiDataWidth = 0;
    for (i=0 ; i<datalen ; i++)
    {
        j = LingualInfo[i].glyph.GLYPHINFO.wBoxX;
        uiDataWidth += j;

        if (j%2!=0)
           j++;

        if (pStampBuffer->uiFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
            MovieStamp_DrawMultiLingualIcon420UV(pStampInfo,LingualInfo[i], uiFBAddrY, uiFBAddrCb, uiFBAddrCr, pStampBuffer->uiYLineOffset);
        else
            MovieStamp_DrawMultiLingualIcon422UV(pStampInfo,LingualInfo[i], uiFBAddrY, uiFBAddrCb, uiFBAddrCr, pStampBuffer->uiYLineOffset);

        uiFBAddrY  += (j);
        uiFBAddrCb += (j);
        uiFBAddrCr += (j);
    }

    return uiDataWidth;
}
#endif

static BOOL MovieStamp_DrawIcon420UV(PSTAMP_INFO pStampInfo, UINT16 uiIconID, UINT32 uiFBY, UINT32 uiFBCb, UINT32 uiFBCr, UINT32 uiLineOffset)
{
    ICON_HEADER pIconHeader;
    ICON_DB const *pDB;
    UINT8   const *uiIconData;
    UINT8   *pChar, *pCharU, *pCharV;
    UINT32  uiIconWidth;
    UINT32  uiIconHeight;
    UINT32  uiPixelCount;
    UINT32  i, j;
    UINT32  uiIndex;

    pDB = pStampInfo->pDataBase;
    pIconHeader = pDB->pIconHeader[uiIconID];
    uiIconWidth = pIconHeader.uiWidth;
    uiIconHeight = pIconHeader.uiHeight;
    uiIconData = (pDB->pIconData) + (pIconHeader.uiOffset);

    uiPixelCount = 0;

    for(i = 0; i < uiIconHeight; i++)
    {
        pChar = (UINT8 *)uiFBY;
        pCharU = (UINT8 *)uiFBCb;
        pCharV = (UINT8 *)uiFBCb+1;

        for(j = 0; j < uiIconWidth; j ++)
        {
            uiIndex = (*uiIconData >> (6 - (uiPixelCount % 4 * 2))) & 0x03;

            if (uiIndex)
            {
                if (uiIndex == 1) // frame color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FR].ucY;
                }
                else // foreground color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FG].ucY;
                }

                if (((j&1)==0)&&((i&1)==0))
                {
                    if (uiIndex == 1) // frame color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FR].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FR].ucV;
                    }
                    else // foreground color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FG].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FG].ucV;
                    }
                }
            }
            pChar++;
            if ((j&1)==0)
            {
                pCharU+=2;
                pCharV+=2;
            }
            uiPixelCount ++;
            if (uiPixelCount % 4 == 0)
                uiIconData ++;

        }
        uiFBY += uiLineOffset;
        if  ((i&1) == 0)
        {
            uiFBCb += uiLineOffset;
            uiFBCr += uiLineOffset;
        }
    }
    return TRUE;
}

static BOOL MovieStamp_DrawIcon422UV(PSTAMP_INFO pStampInfo, UINT16 uiIconID, UINT32 uiFBY, UINT32 uiFBCb, UINT32 uiFBCr, UINT32 uiLineOffset)
{
    ICON_HEADER pIconHeader;
    ICON_DB const *pDB;
    UINT8   const *uiIconData;
    UINT8   *pChar, *pCharU, *pCharV;
    UINT32  uiIconWidth;
    UINT32  uiIconHeight;
    UINT32  uiPixelCount;
    UINT32  i, j;
    UINT32  uiIndex;

    pDB = pStampInfo->pDataBase;
    pIconHeader = pDB->pIconHeader[uiIconID];
    uiIconWidth = pIconHeader.uiWidth;
    uiIconHeight = pIconHeader.uiHeight;
    uiIconData = (pDB->pIconData) + (pIconHeader.uiOffset);

    uiPixelCount = 0;

    for(i = 0; i < uiIconHeight; i++)
    {
        pChar = (UINT8 *)uiFBY;
        pCharU = (UINT8 *)uiFBCb;
        pCharV = (UINT8 *)uiFBCb+1;

        for(j = 0; j < uiIconWidth; j ++)
        {
            uiIndex = (*uiIconData >> (6 - (uiPixelCount % 4 * 2))) & 0x03;

            if (uiIndex)
            {
                if (uiIndex == 1) // frame color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FR].ucY;
                }
                else // foreground color
                {
                    *pChar = pStampInfo->Color[COLOR_ID_FG].ucY;
                }

                //if (((j&1)==0)&&((i&1)==0))
                if ((j&1)==0)
                {
                    if (uiIndex == 1) // frame color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FR].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FR].ucV;
                    }
                    else // foreground color
                    {
                        *pCharU = pStampInfo->Color[COLOR_ID_FG].ucU;
                        *pCharV = pStampInfo->Color[COLOR_ID_FG].ucV;
                    }
                }
            }
            pChar++;
            if ((j&1)==0)
            {
                pCharU+=2;
                pCharV+=2;
            }
            uiPixelCount ++;
            if (uiPixelCount % 4 == 0)
                uiIconData ++;

        }
        uiFBY += uiLineOffset;
        //if  ((i&1) == 0)
        {
            uiFBCb += uiLineOffset;
            uiFBCr += uiLineOffset;
        }
    }
    return TRUE;
}

static UINT32 MovieStamp_GetStampDataWidth(PSTAMP_INFO pStampInfo)
{
    ICON_DB const *pDB;
    UINT32  i;
    UINT32  uiStrLen;
    UINT32  uiDataWidth;
    UINT32  uiIconID;
    UINT32  uiStrOffset;

    uiStrLen = strlen(pStampInfo->pi8Str);

    pDB = pStampInfo->pDataBase;
    uiStrOffset = pDB->uiDrawStrOffset;

    uiDataWidth = 0;
    for (i = 0; i < uiStrLen; i++)
    {
        //get icon database
        uiIconID = pStampInfo->pi8Str[i] - uiStrOffset;
        uiDataWidth += pDB->pIconHeader[uiIconID].uiWidth;
    }

    return uiDataWidth;
}

static UINT32 MovieStamp_DrawString(UINT32 uiVidEncId, PSTAMP_INFO pStampInfo, PSTAMP_BUFFER pStampBuffer)
{
    ICON_DB const *pDB;
    UINT32  i, j;
    UINT32  uiStrLen;
    UINT32  uiDataWidth;
    UINT32  uiIconID;
    UINT32  uiStrOffset;
    UINT32  uiFBAddrY, uiFBAddrCb, uiFBAddrCr;

    uiStrLen = strlen(pStampInfo->pi8Str);

    pDB = pStampInfo->pDataBase;
    uiStrOffset = pDB->uiDrawStrOffset;

    uiFBAddrY  = pStampBuffer->uiYAddr;
    uiFBAddrCb = pStampBuffer->uiUAddr;
    uiFBAddrCr = pStampBuffer->uiVAddr;

    uiDataWidth = 0;
    for (i = 0; i < uiStrLen; i++)
    {
        //get icon database
        uiIconID = pStampInfo->pi8Str[i] - uiStrOffset;
        j = pDB->pIconHeader[uiIconID].uiWidth;
        uiDataWidth += j;

        if (j % 2 != 0)
        {
            j++;
        }

        if (pStampBuffer->uiFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
            MovieStamp_DrawIcon420UV(pStampInfo, uiIconID, uiFBAddrY, uiFBAddrCb, uiFBAddrCr, pStampBuffer->uiYLineOffset);
        else
            MovieStamp_DrawIcon422UV(pStampInfo, uiIconID, uiFBAddrY, uiFBAddrCb, uiFBAddrCr, pStampBuffer->uiYLineOffset);
        uiFBAddrY  += (j);
        uiFBAddrCb += (j);
        uiFBAddrCr += (j);
    }

    return uiDataWidth;
}

void MovieStamp_SetDataAddr(UINT32 uiVidEncId, UINT32 uiAddr)
{
    g_uiWaterLogoYAddr[uiVidEncId]  = uiAddr;
    g_uiWaterLogoUVAddr[uiVidEncId] = uiAddr + WATERLOGO_BUF_SIZE/2; // YUV422, UV-packed
    g_uiMovieStampYAddr[uiVidEncId] = uiAddr + WATERLOGO_BUF_SIZE;
    #if (MULTILINGUAL_FUNCTION == ENABLE)
    g_uiLingualStampYAddr[uiVidEncId] = uiAddr + MULTILINGUAL_BUF_OFFSET;
    g_uiLingualScaleStampYAddr[uiVidEncId] = uiAddr + MULTILINGUAL_BUF_OFFSET*2;
    #endif
}

#if (CHECK_ENC_INFO == ENABLE)
extern void MediaRec_GetEncInfo(UINT32 puiParam[7]);
#endif
void MovieStamp_CopyData(UINT32 uiVidEncId, UINT32 yAddr, UINT32 cbAddr, UINT32 crAddr, UINT32 lineY, UINT32 imgWidth)
{
    GX_IMAGE_PIXEL_FMT GxImgFormat;
    STAMP_BUFFER    StampBuffer;
    struct tm       CurDateTime;
    IMG_BUF         SrcImg, DstImg;
    UINT32          x, y;
    UINT32          uiYAddrOffset, uiUAddrOffset;
    static UINT32   uiStampDataWidth[VIDEO_IN_MAX] = {0, 0};
    UINT32          uiPreWidth, uiWidth;
    UINT32          uiStampDataHeight;
    UINT32          uiBgData;
    UINT32          uiLineOffset[2] = {0};
    UINT32          uiPxlAddr[2] = {0};
    #if (MULTILINGUAL_FUNCTION == ENABLE)
    STAMP_BUFFER    LingualStampBuffer;
    STAMP_BUFFER    LingualScaleBuffer;
    IMG_BUF         Src2Img, SrcScaleImg;
    UINT32          uiYAddrOffset2, uiUAddrOffset2;
    UINT32          uiLineOffset2[2] = {0};
    UINT32          uiPxlAddr2[2] = {0};
    static UINT32   uiLingualDataWidth[VIDEO_IN_MAX] = {0, 0};
    static UINT32   uiLingualScaleWidth[VIDEO_IN_MAX] = {0, 0};
    UINT32          uiUnicodeLen;
    UINT32          uiLingualDataHeight=0;
    UINT32          uiLingualScaleHeight=0;
    UINT32          uiLineOffsetScale[2] = {0};
    UINT32          uiPxlAddrScale[2] = {0};
    #endif

    #if (CHECK_STAMP_TIME == ENABLE)
    UINT32          uiTime; // for performance measurement
    #endif

    if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_SWITCH_MASK) == STAMP_OFF)
    {
        return;
    }

    if (yAddr == 0 || cbAddr == 0)
    {
        return;
    }

    switch (g_uiMovieStampSetup[uiVidEncId] & STAMP_IMG_FORMAT_MASK)
    {
    case STAMP_IMG_420UV:
        GxImgFormat = GX_IMAGE_PIXEL_FMT_YUV420_PACKED;
        break;

    case STAMP_IMG_422UV:
        GxImgFormat = GX_IMAGE_PIXEL_FMT_YUV422_PACKED;
        break;

    default:
        DBG_ERR("Only support 420/422 UV pack!\r\n");
        return;
    }

    x = g_MovieStampPos[uiVidEncId].uiX;
    y = g_MovieStampPos[uiVidEncId].uiY;

    CurDateTime = HwClock_GetCurrentTime();

    // Y address offset of destination image to put Y stamp data
    uiYAddrOffset = lineY * y + x;
    // UV address offset of destination image to put UV stamp data
    if (GxImgFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
    {
        uiUAddrOffset = lineY * y / 2 + x;
    }
    else
    {
        uiUAddrOffset = lineY * y + x;
    }

    #if (MULTILINGUAL_FUNCTION == ENABLE)
    {
        uiYAddrOffset2 = lineY * g_LingualStampPos[uiVidEncId].uiY + g_LingualStampPos[uiVidEncId].uiX;
        if (GxImgFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
        {
            uiUAddrOffset2 = lineY * g_LingualStampPos[uiVidEncId].uiY / 2 + g_LingualStampPos[uiVidEncId].uiX;
        }
        else
        {
            uiUAddrOffset2 = lineY * g_LingualStampPos[uiVidEncId].uiY + g_LingualStampPos[uiVidEncId].uiX;
        }
    }
    #endif
    uiStampDataHeight = g_MovieStampInfo[uiVidEncId].ui32DstHeight;

    StampBuffer.uiYLineOffset   = imgWidth;
    StampBuffer.uiUVLineOffset  = imgWidth;
    StampBuffer.uiYAddr         = g_uiMovieStampYAddr[uiVidEncId];
    StampBuffer.uiUAddr         = StampBuffer.uiYAddr + StampBuffer.uiYLineOffset * uiStampDataHeight;
    StampBuffer.uiVAddr         = StampBuffer.uiUAddr; // for 420 UV pack
    StampBuffer.uiFormat        = GxImgFormat;

    #if (MULTILINGUAL_FUNCTION == ENABLE)
    // Multi-Lingual buffer
    {

     for (uiUnicodeLen=0;uiUnicodeLen<(sizeof(unicode)/sizeof(UINT16));uiUnicodeLen++)
       {
          UniFont_GetGlyphFromUnicode(unicode[uiUnicodeLen],&LingualInfo[uiUnicodeLen]);

          // get max Lingual data width/height in this unicode string.
          if (LingualInfo[uiUnicodeLen].glyph.GLYPHINFO.wBoxX > uiLingualDataWidth[uiVidEncId])
              uiLingualDataWidth[uiVidEncId] = LingualInfo[uiUnicodeLen].glyph.GLYPHINFO.wBoxX;
          if (LingualInfo[uiUnicodeLen].glyph.GLYPHINFO.wBoxY > uiLingualDataHeight)
              uiLingualDataHeight = LingualInfo[uiUnicodeLen].glyph.GLYPHINFO.wBoxY;
       }

     switch(imgWidth)
     {
        case  1920: // 1920x1080
        case  1536: // 1536x1536, 26x44
           uiLingualScaleHeight = (uiLingualDataHeight*44)>> 4;
           break;

        case  1440: // 1440x1080
        case  1728: // 1728x1296, (DAR 16:9)20x44
           uiLingualScaleHeight = (uiLingualDataHeight*33)>> 4;
           break;
        case  1280: // 1280x720, 18x30
           uiLingualScaleHeight = (uiLingualDataHeight*30)>> 4;
           break;
        case  640: //  640x480, 12x20
        case  848: //  848x480, 12x20
           uiLingualScaleHeight = (uiLingualDataHeight*20)>> 4;
           break;
        default:
           uiLingualScaleHeight = uiLingualDataHeight;
           break;
     }

     LingualStampBuffer.uiYLineOffset   = imgWidth;
     LingualStampBuffer.uiUVLineOffset  = imgWidth;
     LingualStampBuffer.uiYAddr         = g_uiLingualStampYAddr[uiVidEncId];
     LingualStampBuffer.uiUAddr         = LingualStampBuffer.uiYAddr + LingualStampBuffer.uiYLineOffset * uiLingualDataHeight;
     LingualStampBuffer.uiVAddr         = LingualStampBuffer.uiUAddr; // for 420 UV pack
     LingualStampBuffer.uiFormat        = GxImgFormat;

     LingualScaleBuffer.uiYLineOffset = imgWidth;
     LingualScaleBuffer.uiUVLineOffset  = imgWidth;

     LingualScaleBuffer.uiYAddr         = g_uiLingualScaleStampYAddr[uiVidEncId];
     LingualScaleBuffer.uiUAddr         = LingualScaleBuffer.uiYAddr + LingualScaleBuffer.uiYLineOffset * uiLingualScaleHeight;
     LingualScaleBuffer.uiVAddr         = LingualScaleBuffer.uiUAddr; // for 420 UV pack
     LingualScaleBuffer.uiFormat        = GxImgFormat;
    }
    #endif

    #if (CHECK_STAMP_TIME == ENABLE)
    //Perf_Open(); // for performance measurement
    #endif

    #if (CHECK_ENC_INFO == DISABLE)
    // update stamp data if current second is not equal to previous second,
    // or if it's time-lapse mode (in time-lapse mode, current second may be equal to previous one,
    // e.g., time-lapse interval = 60 seconds)
    if ((g_CurDateTime[uiVidEncId].tm_sec != CurDateTime.tm_sec) ||
        (UI_GetData(FL_MOVIE_TIMELAPSE_REC) != MOVIE_TIMELAPSEREC_OFF))
    #endif
    {
        g_CurDateTime[uiVidEncId].tm_sec = CurDateTime.tm_sec;

        // Prepare date-time string
        if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_DATE_TIME_MASK) == STAMP_DATE_TIME)
        {
            switch (g_uiMovieStampSetup[uiVidEncId] & STAMP_DATE_FORMAT_MASK)
            {
            case STAMP_DD_MM_YY:
                                //sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s  %03d KM/H %02d/%02d/%04d %02d:%02d:%02d", carnumber_stamp_str,uiSpeed,CurDateTime.tm_mday, CurDateTime.tm_mon, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);

                sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s   %02d/%02d/%04d %02d:%02d:%02d", carnumber_stamp_str,CurDateTime.tm_mday, CurDateTime.tm_mon, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                break;
            case STAMP_MM_DD_YY:
                                //sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s  %03d KM/H %02d/%02d/%04d %02d:%02d:%02d", carnumber_stamp_str,uiSpeed,CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);

                sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s   %02d/%02d/%04d %02d:%02d:%02d", carnumber_stamp_str,CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                break;
            default:
                               // sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s  %03d KM/H %04d/%02d/%02d %02d:%02d:%02d", carnumber_stamp_str,uiSpeed,CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);

                sprintf(&g_cMovieStampStr[uiVidEncId][0], "%s   %04d/%02d/%02d %02d:%02d:%02d", carnumber_stamp_str,CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                break;
            }
        }
        else if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_DATE_TIME_MASK) == STAMP_DATE_TIME_AMPM)
        {
            if (CurDateTime.tm_hour >= 12)
            {
                if (CurDateTime.tm_hour > 12)
                    CurDateTime.tm_hour -= 12;

                switch (g_uiMovieStampSetup[uiVidEncId] & STAMP_DATE_FORMAT_MASK)
                {
                case STAMP_DD_MM_YY:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%02d/%02d/%04d %02d:%02d:%02d PM", CurDateTime.tm_mday, CurDateTime.tm_mon, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                case STAMP_MM_DD_YY:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%02d/%02d/%04d %02d:%02d:%02d PM", CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                default:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%04d/%02d/%02d %02d:%02d:%02d PM", CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                }
            }
            else
            {
                switch (g_uiMovieStampSetup[uiVidEncId] & STAMP_DATE_FORMAT_MASK)
                {
                case STAMP_DD_MM_YY:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%02d/%02d/%04d %02d:%02d:%02d AM", CurDateTime.tm_mday, CurDateTime.tm_mon, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                case STAMP_MM_DD_YY:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%02d/%02d/%04d %02d:%02d:%02d AM", CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_year, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                default:
                    sprintf(&g_cMovieStampStr[uiVidEncId][0], "%04d/%02d/%02d %02d:%02d:%02d AM", CurDateTime.tm_year, CurDateTime.tm_mon, CurDateTime.tm_mday, CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
                    break;
                }
            }
        }
        else
        {
            sprintf(&g_cMovieStampStr[uiVidEncId][0], "%02d:%02d:%02d", CurDateTime.tm_hour, CurDateTime.tm_min, CurDateTime.tm_sec);
        }

        ////<< spark add AE function
        //IQS_GetLogString(&g_cMovieStampStr[uiVidEncId][0], 256);
        //g_cMovieStampStr[uiVidEncId][0] =" ";
        //g_cMovieStampStr[uiVidEncId][1] =" ";
        #if (CHECK_ENC_INFO == ENABLE)
        {
            UINT32 uiParam[7];

            MediaRec_GetEncInfo(uiParam);
            sprintf(&g_cMovieStampStr[uiVidEncId][0], "%d %d %d %d / %d %d %d",
                uiParam[0], uiParam[1], uiParam[2], uiParam[3], uiParam[4], uiParam[5], uiParam[6]);
        }
        #endif

        uiPreWidth = uiStampDataWidth[uiVidEncId]; // previous stamp data width
        uiStampDataWidth[uiVidEncId] = MovieStamp_GetStampDataWidth(&g_MovieStampInfo[uiVidEncId]);
        uiWidth = (uiPreWidth > uiStampDataWidth[uiVidEncId]) ? uiPreWidth : uiStampDataWidth[uiVidEncId];

        // Set background data
        #if (CHECK_STAMP_TIME == ENABLE)
        uiTime = Perf_GetCurrent();
        #endif

        // Clear source (stamp) buffer
        uiLineOffset[0] = StampBuffer.uiYLineOffset;
        uiLineOffset[1] = StampBuffer.uiUVLineOffset;
        uiPxlAddr[0]    = StampBuffer.uiYAddr;
        uiPxlAddr[1]    = StampBuffer.uiUAddr;

        GxImg_InitBufEx(&SrcImg,
                        uiWidth,
                        uiStampDataHeight,
                        GxImgFormat,
                        uiLineOffset,
                        uiPxlAddr);

        #if (MULTILINGUAL_FUNCTION == ENABLE)
        {

        uiPreWidth = uiLingualDataWidth[uiVidEncId]; // previous stamp data width
        uiLingualDataWidth[uiVidEncId] = MovieStamp_GetLingualDataWidth(&LingualInfo[0],uiUnicodeLen);
        uiWidth = (uiPreWidth > uiLingualDataWidth[uiVidEncId]) ? uiPreWidth : uiLingualDataWidth[uiVidEncId];

        uiLineOffset2[0] = LingualStampBuffer.uiYLineOffset;
        uiLineOffset2[1] = LingualStampBuffer.uiUVLineOffset;
        uiPxlAddr2[0]    = LingualStampBuffer.uiYAddr;
        uiPxlAddr2[1]    = LingualStampBuffer.uiUAddr;

        GxImg_InitBufEx(&Src2Img,
                        uiWidth,
                        uiLingualDataHeight,
                        GxImgFormat,
                        uiLineOffset2,
                        uiPxlAddr2);
        }
        #endif
        if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_BG_FORMAT_MASK) == STAMP_BG_TRANSPARENT)
        {
            //uiBgData = COLORKEY_BG_YUV; // for LE op
            uiBgData = COLORKEY_YUV; // for EQ op
        }
        else
        {
            uiBgData =  ((UINT32)g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucV << 16) |
                        ((UINT32)g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucU << 8)  |
                        g_MovieStampInfo[uiVidEncId].Color[COLOR_ID_BG].ucY;
        }

        GxImg_FillData(&SrcImg, REGION_MATCH_IMG, uiBgData);

        #if (MULTILINGUAL_FUNCTION == ENABLE)
        GxImg_FillData(&Src2Img, REGION_MATCH_IMG, uiBgData);
        #endif

        #if (CHECK_STAMP_TIME == ENABLE)
        DBG_DUMP("time1: %d us\r\n", Perf_GetCurrent() - uiTime);
        #endif

        // Draw string by getting data from date stamp font
        #if (CHECK_STAMP_TIME == ENABLE)
        uiTime = Perf_GetCurrent();
        #endif

        MovieStamp_DrawString(uiVidEncId, &g_MovieStampInfo[uiVidEncId], &StampBuffer);

        #if (MULTILINGUAL_FUNCTION == ENABLE)
        MovieStamp_DrawMultiLingualString(&g_MovieStampInfo[uiVidEncId], &LingualStampBuffer,&unicode[0],uiUnicodeLen);
        #endif

        #if (CHECK_STAMP_TIME == ENABLE)
        DBG_DUMP("time2: %d us\r\n", Perf_GetCurrent() - uiTime);
        #endif
    }

    // copy data from date stamp draw buffer to image buffer
    #if (CHECK_STAMP_TIME == ENABLE)
    uiTime = Perf_GetCurrent();
    #endif
    // Init source (stamp) buffer
    uiLineOffset[0] = StampBuffer.uiYLineOffset;
    uiLineOffset[1] = StampBuffer.uiUVLineOffset;
    uiPxlAddr[0]    = StampBuffer.uiYAddr;
    uiPxlAddr[1]    = StampBuffer.uiUAddr;

    GxImg_InitBufEx(&SrcImg,
                    uiStampDataWidth[uiVidEncId],
                    uiStampDataHeight,
                    GxImgFormat,
                    uiLineOffset,
                    uiPxlAddr);

    // Init destination (image) buffer
    uiLineOffset[0] = lineY;
    uiLineOffset[1] = lineY;
    uiPxlAddr[0]    = yAddr  + uiYAddrOffset;
    uiPxlAddr[1]    = cbAddr + uiUAddrOffset;

    GxImg_InitBufEx(&DstImg,
                    imgWidth,
                    240, // don't care, but should be > 2
                    GxImgFormat,
                    uiLineOffset,
                    uiPxlAddr);

    if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_BG_FORMAT_MASK) == STAMP_BG_TRANSPARENT)
    {
        GxImg_CopyColorkeyData(&DstImg, REGION_MATCH_IMG, &SrcImg, REGION_MATCH_IMG, COLORKEY_YUV, FALSE);
    }
    else
    {
        GxImg_CopyData(&SrcImg, REGION_MATCH_IMG, &DstImg, REGION_MATCH_IMG);
    }

    #if (MULTILINGUAL_FUNCTION == ENABLE)
    // Second line date Stamp
    {
     IRECT       SrcRegion = {0};
     IRECT       DstRegion = {0};

        // Init source (stamp) buffer
        uiLineOffset2[0] = LingualStampBuffer.uiYLineOffset;
        uiLineOffset2[1] = LingualStampBuffer.uiUVLineOffset;
        uiPxlAddr2[0]    = LingualStampBuffer.uiYAddr;
        uiPxlAddr2[1]    = LingualStampBuffer.uiUAddr;

        GxImg_InitBufEx(&Src2Img,
                        uiLingualDataWidth[uiVidEncId],
                        uiLingualDataHeight,
                        GxImgFormat,
                        uiLineOffset2,
                        uiPxlAddr2);


        uiLineOffsetScale[0] = LingualScaleBuffer.uiYLineOffset;
        uiLineOffsetScale[1] = LingualScaleBuffer.uiUVLineOffset;
        uiPxlAddrScale[0]    = LingualScaleBuffer.uiYAddr;
        uiPxlAddrScale[1]    = LingualScaleBuffer.uiUAddr;

         switch(imgWidth)
         {
            case  1920: // 1920x1080
            case  1536: // 1536x1536, 26x44
               uiLingualScaleWidth[uiVidEncId] = (uiLingualDataWidth[uiVidEncId]*44)>> 4;
               break;

            case  1440: // 1440x1080
            case  1728: // 1728x1296, (DAR 16:9)20x44
               uiLingualScaleWidth[uiVidEncId] = (uiLingualDataWidth[uiVidEncId]*33)>> 4;
               break;
            case  1280: // 1280x720, 18x30
               uiLingualScaleWidth[uiVidEncId] = (uiLingualDataWidth[uiVidEncId]*30)>> 4;
               break;
            case  640: //  640x480, 12x20
            case  848: //  848x480, 12x20
               uiLingualScaleWidth[uiVidEncId] = (uiLingualDataWidth[uiVidEncId]*20)>> 4;
               break;
            default: // 320x240, 10x16
               uiLingualScaleWidth[uiVidEncId] = uiLingualDataWidth[uiVidEncId];
               break;
         }

        GxImg_InitBufEx(&SrcScaleImg,
                        uiLingualScaleWidth[uiVidEncId],
                        uiLingualScaleHeight,
                        GxImgFormat,
                        uiLineOffsetScale,
                        uiPxlAddrScale);

        // Set scale region
        SrcRegion.x = 0;
        SrcRegion.y = 0;
        SrcRegion.w = uiLingualDataWidth[uiVidEncId];
        SrcRegion.h = uiLingualDataHeight;

        DstRegion.x = 0;
        DstRegion.y = 0;
        DstRegion.w = uiLingualScaleWidth[uiVidEncId];
        DstRegion.h = uiLingualScaleHeight;

        // Scale image by ISE
        GxImg_SetParm(GXIMG_PARM_SCALE_METHOD,GXIMG_SCALE_NEAREST);
        GxImg_ScaleData(&Src2Img, &SrcRegion, &SrcScaleImg, &DstRegion);
        GxImg_SetParm(GXIMG_PARM_SCALE_METHOD,GXIMG_SCALE_AUTO);

        // Init destination (image) buffer
        uiLineOffset[0] = lineY;
        uiLineOffset[1] = lineY;
        uiPxlAddr[0]    = yAddr  + uiYAddrOffset2;
        uiPxlAddr[1]    = cbAddr + uiUAddrOffset2;

        GxImg_InitBufEx(&DstImg,
                        imgWidth,
                        240, // don't care, but should be > 2
                        GxImgFormat,
                        uiLineOffset,
                        uiPxlAddr);

        if ((g_uiMovieStampSetup[uiVidEncId] & STAMP_BG_FORMAT_MASK) == STAMP_BG_TRANSPARENT)
        {
            GxImg_CopyColorkeyData(&DstImg, REGION_MATCH_IMG, &SrcScaleImg, REGION_MATCH_IMG, COLORKEY_YUV, FALSE);
        }
        else
        {
            GxImg_CopyData(&SrcScaleImg, REGION_MATCH_IMG, &DstImg, REGION_MATCH_IMG);
        }
    }
    #endif

    #if (WATERLOGO_FUNCTION==ENABLE)
    {
        UINT32      x2, y2, uiYAddrOffset2, uiUAddrOffset2;
        IMG_BUF     SrcImg2, DstImg2;

        x2 = g_MovieWaterInfo[uiVidEncId].uiXPos;
        y2 = g_MovieWaterInfo[uiVidEncId].uiYPos;

        uiYAddrOffset2 = lineY * y2 + x2;
        if (GxImgFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
        {
            uiUAddrOffset2 = lineY * y2 / 2 + x2;
        }
        else
        {
            uiUAddrOffset2 = lineY * y2 + x2;
        }

        // Init source (watermark) buffer
        uiLineOffset[0] = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYLineOffset;
        if (GxImgFormat == GX_IMAGE_PIXEL_FMT_YUV420_PACKED)
        {
            uiLineOffset[1] = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYLineOffset * 2; // *2 for YUV422 => YUV420
        }
        else // GX_IMAGE_PIXEL_FMT_YUV422_PACKED
        {
            uiLineOffset[1] = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYLineOffset;
        }
        uiPxlAddr[0]    = g_MovieWaterInfo[uiVidEncId].uiWaterLogoYAddr;
        uiPxlAddr[1]    = g_MovieWaterInfo[uiVidEncId].uiWaterLogoUVAddr;

        GxImg_InitBufEx(&SrcImg2,
                        g_MovieWaterInfo[uiVidEncId].uiWidth,
                        g_MovieWaterInfo[uiVidEncId].uiHeight,
                        GxImgFormat,
                        uiLineOffset,
                        uiPxlAddr);

        // Init destination (image) buffer
        uiLineOffset[0] = lineY;
        uiLineOffset[1] = lineY;
        uiPxlAddr[0]    = yAddr  + uiYAddrOffset2;
        uiPxlAddr[1]    = cbAddr + uiUAddrOffset2;

        GxImg_InitBufEx(&DstImg2,
                        imgWidth,
                        240, // don't care, but should be > 2
                        GxImgFormat,
                        uiLineOffset,
                        uiPxlAddr);

        if (g_MovieWaterInfo[uiVidEncId].uiWaterLogoColorKey == WATERLOGO_FILL_COLOR)
            GxImg_CopyData(&SrcImg2, REGION_MATCH_IMG, &DstImg2, REGION_MATCH_IMG);
        else
            GxImg_CopyColorkeyData(&DstImg2, REGION_MATCH_IMG, &SrcImg2, REGION_MATCH_IMG, g_MovieWaterInfo[uiVidEncId].uiWaterLogoColorKey, FALSE);
    }
    #endif

    #if (CHECK_STAMP_TIME == ENABLE)
    DBG_DUMP("time3: %d us\r\n", Perf_GetCurrent() - uiTime);
    //Perf_Close();
    #endif
}
