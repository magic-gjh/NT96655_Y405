/**
    YUV Format Font to be generated by INDEX8 font.

    For date imprint to generate yuv string to paste on picture.

    @file       FontToYuv.h
    @ingroup    mFONTTOYUV

    Copyright   Novatek Microelectronics Corp. 2012.  All rights reserved.
*/

#ifndef _FONTTOYUV_H
#define _FONTTOYUV_H
#include "GxGfx.h"
#include "GxImage.h"

/**
    @addtogroup mFONTTOYUV
*/
//@{

/**
     Font rotation degree

     For date imprint with rotation.
*/
typedef enum _FONT_TO_YUV_DEG{
    FONT_TO_YUV_DEG_0,      ///< no rotation
    FONT_TO_YUV_DEG_90,     ///< 90 degree rotation
    FONT_TO_YUV_DEG_180,    ///< 180 degree rotation
    FONT_TO_YUV_DEG_270,    ///< 270 degree rotation
    ENUM_DUMMY4WORD(FONT_TO_YUV_DEG)
}FONT_TO_YUV_DEG;

/**
     String font translate to YUV format information

     An input structure for FontToYuv
*/
typedef void (*FontToYuvCB)(UINT32,UINT32,UINT32,UINT32);

typedef struct _FONT_TO_YUV_IN
{
    UINT32 MemAddr;             ///< Working Memory Address
    UINT32 MemSize;             ///< Working Memory Size
    FONT_TO_YUV_DEG Deg;        ///< degree
    CHAR*  pStr;                ///< Source String
    FONT*  pFont;               ///< Font Data
    COLOR_ITEM ciSolid;         ///< Solid Color of Font
    COLOR_ITEM ciFrame;         ///< Frame Color of Font
    COLOR_ITEM ciTransparet;    ///< Transparent Color of Font
    UINT32 ScaleFactor;         ///< Scale Factor base on scale = ScaleFactor/65536
    GX_IMAGE_PIXEL_FMT Format;  ///< Only Support to GX_IMAGE_PIXEL_FMT_YUV422_PACKED currently
    BOOL   bEnableSmooth;       ///< Set TRUE for Font with Frame Smooth
    FontToYuvCB  pStringCB;     ///< user draw string/icon callback
    PALETTE_ITEM* pPalette ;    ///< global palette for callback
    ISIZE StampSize;
}FONT_TO_YUV_IN,*PFONT_TO_YUV_IN;

/**
     String font translate to YUV format information

     An output structure for FontToYuv
*/
typedef struct _FONT_TO_YUV_OUT
{
    IMG_BUF GenImg;             ///< Generated Image
    UINT32 ColorKeyY;           ///< Color Key Y
    UINT32 ColorKeyCb;          ///< Color Key Cb
    UINT32 ColorKeyCr;          ///< Color Key Cr
    UINT32 UsedMemSize;         ///< Used Memory Size
    UINT32 UsedMaxMemSize;      ///< Used Max Usage Memory Size
}FONT_TO_YUV_OUT,*PFONT_TO_YUV_OUT;

/**
     Font data to Packed UV 422

     A source with INDEX8 trans to YUV format.
*/
extern ER FontToYuv(FONT_TO_YUV_IN* pIn,FONT_TO_YUV_OUT* pOut);

//@}
#endif