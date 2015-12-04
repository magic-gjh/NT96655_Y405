#ifndef _DRAW_LIB_H
#define _DRAW_LIB_H

#include "Type.h"

#define _ZH_CAR_NUM_		ENABLE
//#define _ZH_CAR_ICON_NUM_ 0
#define _ZH_CAR_ICON_NUM_ 31
#define _0041_ 1 //A
#define _0042_ 1 //B
#define _0043_ 1 //C
#define _0044_ 1 //D
#define _0045_ 1 //E
#define _0046_ 1 //F
#define _0047_ 1 //G
#define _0048_ 1 //H
#define _0049_ 1 //I
#define _004A_ 1 //J
#define _004B_ 1 //K
#define _004C_ 1 //L
#define _004D_ 1 //M
#define _004E_ 1 //N
#define _004F_ 1 //O
#define _0050_ 1 //P
#define _0051_ 1 //Q
#define _0052_ 1 //R
#define _0053_ 1 //S
#define _0054_ 1 //T
#define _0055_ 1 //U
#define _0056_ 1 //V
#define _0057_ 1 //W
#define _0058_ 1 //X
#define _0059_ 1 //Y
#define _005A_ 1 //Z

#define _0061_ 1 //a
#define _0062_ 1 //b
#define _0063_ 1 //c
#define _0064_ 1 //d
#define _0065_ 1 //e
#define _0066_ 1 //f
#define _0067_ 1 //g
#define _0068_ 1 //h
#define _0069_ 1 //i
#define _006A_ 1 //j
#define _006B_ 1 //k
#define _006C_ 1 //l
#define _006D_ 1 //m
#define _006E_ 1 //n
#define _006F_ 1 //o
#define _0070_ 1 //p
#define _0071_ 1 //q
#define _0072_ 1 //r
#define _0073_ 1 //s
#define _0074_ 1 //t
#define _0075_ 1 //u
#define _0076_ 1 //v
#define _0077_ 1 //w
#define _0078_ 1 //x
#define _0079_ 1 //y
#define _007A_ 1 //z

#if (_ZH_CAR_NUM_ == ENABLE)
#define _4EAC_ 1 //¾©
#define _6D25_ 1 //½ò 
#define _6CAA_	1 //»¦ 
#define _6E1D_ 1 //Óå
#define _5180_ 1 //¼½
#define _8C6B_ 1 //Ô¥ 
#define _4E91_ 1 //ÔÆ
#define _8FBD_ 1 // ÁÉ
#define _9ED1_ 1 //ºÚ
#define _6E58_ 1 // Ïæ 
#define _7696_ 1 //Íî
#define _9C81_ 1 //Â³
#define _65B0_ 1 //ĞÂ
#define _82CF_ 1 //ËÕ
#define _6D59_ 1 //Õã
#define _8D63_ 1 //¸Ó 
#define _9102_ 1 //¶õ
#define _6842_ 1 //¹ğ   
#define _7518_ 1 //¸Ê
#define _664B_ 1 //½ú
#define _8499_ 1 //ÃÉ 
#define _9655_ 1 //ÉÂ
#define _5409_ 1 //¼ª
#define _95FD_ 1 //Ãö
#define _8D35_ 1 //¹ó
#define _7CA4_ 1 //ÔÁ
#define _9752_ 1 //Çà
#define _85CF_ 1 //²Ø
#define _5DDD_ 1 //´¨
#define _5B81_ 1 //Äş
#define _743C_ 1 //Çí
#endif
typedef struct {
    UINT32      uiOffset;              ///< offset in database.
    UINT16      uiWidth;               ///< icon width.
    UINT16      uiHeight;              ///< icon height.
}ICON_HEADER, *PICON_HEADER;

typedef struct {
    ICON_HEADER const   *pIconHeader;       ///< point tp icon header
    UINT8       const   *pIconData;         ///< point to icon data
    UINT16              uiNumOfIcon;        ///< how many icon in this icon data base
    UINT8               uiBitPerPixel;      ///< bit perpixel of this icon DB, it can be 1, 2 or 4 bits per pixel
    UINT8               uiDrawStrOffset;    ///< Data base first item offset
}ICON_DB, *PICON_DB;

#endif /* _DRAW_LIB_H */
