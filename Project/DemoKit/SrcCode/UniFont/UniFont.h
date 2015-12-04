
#ifndef _UNIFONT_H
#define _UNIFONT_H

//#pragma pack(1)

#include "Type.h"
#include "Debug.h"
#include "GxGfx.h"

#define GLYPHDATA_LENGTH        320


// 掃描到的列資料列
typedef struct _SCANLINE{
   UINT32  scanData[34];
}SCANLINE;


// 提供個別 Glyph 的尺寸資料
typedef struct tagUFN_GLYPHINFO {
    CHAR        wOrgX;              // Glyph 區塊左上角和原點的偏移 X，往右為正
    CHAR        wOrgY;              // Glyph 區塊左上角和基準線的偏移 Y，往上為正
    UINT8       wBoxX;              // 能夠包含住 Glyph 區塊的最小寬度
    UINT8       wBoxY;              // 能夠包含住 Glyph 區塊的最小高度
} UFN_GLYPHINFO, *PUFN_GLYPHINFO;

//  Glyph在UFN的資料結構
typedef struct _GLYPH {
    UFN_GLYPHINFO GLYPHINFO;
    SCANLINE      SCAN;
}UFN_GLYPH,*PUFN_GLYPH;

typedef struct
{
  UFN_GLYPH     glyph;
  UINT8         GlyphData[GLYPHDATA_LENGTH];
} LINGUAL_INFO,*PLINGUAL_INFO;


// 區段表中的區段內容
typedef struct tagUFN_RANGE {
    UINT16      uwFirst;            // 區段的啟始值
    UINT16      uwLast;             // 區段的結束值
} UFN_RANGE, *PUFN_RANGE;

// UniFont 檔頭
typedef _PACKED_BEGIN struct tagUFN_FILE_HEADER {
    UINT32       cbThis;             /* 檔頭大小 = sizeof(UFN_FILE_HEADER)  */
    CHAR         cFaceName[28];      /* 字形名稱  */
    short        wHeight;            /* 字形高度，最大 32 */
    short        wDescent;           /* 基準線 pixel, 由最下面往上算，例如 5 pixel 高  */
    UINT16       wGlyphsCount;       /* 總共有多少個 Glyph  */
    UINT16       cRanges;            /* 總共有多少個區段  */
    UINT32       FileHandle;         /* 存放字形的檔案代碼  */
    UINT32       FontHandle;         /* 存放字形的 FontMap 代碼 */
    UFN_RANGE*   RangeTable;         /* 指向區段表，每個區段以 UFN_RANGE 結構表示  */
    UINT16*      IndexTable;         /* 指向索引表，對應每個區段的啟始 Glyph，型別 USHORT  */
    UINT8*       WidthTable;         /* 指向寬度表，對應每個 Glyph 的建議寬度，型別 BYTE  */
    UINT32*      OffsetTable;        /* 指向偏移表，對應每個 Glyph 資料的偏移位址，型別 ULONG  */
    UINT32       GlyphData;          /* 第 0 個 Glyph 資料的啟始位址，從檔案開頭啟算  */
} _PACKED_END UFN_FILE_HEADER, *PUFN_FILE_HEADER;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// UFN 檔案格式: <檔頭>, <區段表>, <索引表>, <寬度表>, <偏移表>, <Glyph 0>, <Glyph 1>, ... <Glyph N-1>
//----------------------------------------------------------------------------------------------
//
// <UFN_FILE_HEADER>            : 檔頭資訊
//
// <UFN_RANGE[cRanges]>         : 區段表共有 wRanges 個區段
//
// <INDEX_TABLE[cRanges]>       : 索引表用於將區段對應到偏移表的啟始
//
// <WIDTH_TABLE[wGlyphsCount]>  : 記錄每個 Glyph 的建議寬度
//
// <OFFSET_TABLE[wGlyphsCount]> : Glyph 偏移表，以檔案開頭啟算，共有 wGlyphsCount 個偏移位址
//                                偏移量的最高 BYTE 用於表示 Glyph 的資料長度(bytes)，
//                                最大偏移量 16 Mbytes
//
//  [0]<UFN_GLYPHINFO>          : 第 0 個 Glyph 的資訊，最小包含區塊大小、原點偏移
//     <Scan Line Data>         : 第 0 個 Glyph 的掃描線資料，每條掃描線為 4 byte，最多表示
//                                32 個 pixel，每 byte 中的每位元從左至右對應一個 pixel
//                                BYTE_0[7:0] -> pixel  0 ..  7, pixel 0 為左上角
//                                BYTE_1[7:0] -> pixel  8 .. 15
//                                BYTE_2[7:0] -> pixel 16 .. 23
//                                BYTE_3[7:0] -> pixel 24 .. 31
//                                每個 Glyph 的掃描線不固定，可由 wBoxY 來決定，最多 32 條
//
//                                UFN 檔案中描繪的 Glyph 皆以左上角開始，應用程式可由
//                                UFN_GLYPHINFO 提示的資訊還原出完整的資料
//
//  [1]<UFN_GLYPHINFO>          : 第 1 個 Glyph 的資訊，最小包含區塊大小、原點偏移
//     <Scan Line Data>         : 第 1 個 Glyph 的掃描線資料
//  .
//  .
//  .
//
//  <直到第 wGlyphsCount-1 個 Glyph>
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

// UFN 檔中 Glyph 資料範例
// --------------------------------------------------------------------------------------------------
// U+0057, Index=54, Box(15,21), Org(1,22), Width=16
// Print font: 4 bytes by 21 lines, align=1
// [ 0] E3 8E 00 00   '■■■□□□■■■□□□■■■□□□□□□□□□□□□□□□□□'
// [ 1] E3 8E 00 00   '■■■□□□■■■□□□■■■□□□□□□□□□□□□□□□□□'
// [ 2] E3 8E 00 00   '■■■□□□■■■□□□■■■□□□□□□□□□□□□□□□□□'
// [ 3] E3 8E 00 00   '■■■□□□■■■□□□■■■□□□□□□□□□□□□□□□□□'
// [ 4] 73 9C 00 00   '□■■■□□■■■□□■■■□□□□□□□□□□□□□□□□□□'
// [ 5] 72 9C 00 00   '□■■■□□■□■□□■■■□□□□□□□□□□□□□□□□□□'
// [ 6] 72 9C 00 00   '□■■■□□■□■□□■■■□□□□□□□□□□□□□□□□□□'
// [ 7] 76 DC 00 00   '□■■■□■■□■■□■■■□□□□□□□□□□□□□□□□□□'
// [ 8] 76 DC 00 00   '□■■■□■■□■■□■■■□□□□□□□□□□□□□□□□□□'
// [ 9] 76 DC 00 00   '□■■■□■■□■■□■■■□□□□□□□□□□□□□□□□□□'
// [10] 76 DC 00 00   '□■■■□■■□■■□■■■□□□□□□□□□□□□□□□□□□'
// [11] 36 D8 00 00   '□□■■□■■□■■□■■□□□□□□□□□□□□□□□□□□□'
// [12] 36 D8 00 00   '□□■■□■■□■■□■■□□□□□□□□□□□□□□□□□□□'
// [13] 3E F8 00 00   '□□■■■■■□■■■■■□□□□□□□□□□□□□□□□□□□'
// [14] 3E F8 00 00   '□□■■■■■□■■■■■□□□□□□□□□□□□□□□□□□□'
// [15] 3E F8 00 00   '□□■■■■■□■■■■■□□□□□□□□□□□□□□□□□□□'
// [16] 3C 78 00 00   '□□■■■■□□□■■■■□□□□□□□□□□□□□□□□□□□'
// [17] 3C 78 00 00   '□□■■■■□□□■■■■□□□□□□□□□□□□□□□□□□□'
// [18] 1C 70 00 00   '□□□■■■□□□■■■□□□□□□□□□□□□□□□□□□□□'
// [19] 1C 70 00 00   '□□□■■■□□□■■■□□□□□□□□□□□□□□□□□□□□'
// [20] 1C 70 00 00   '□□□■■■□□□■■■□□□□□□□□□□□□□□□□□□□□'

// 應用程式應可以轉換出正常的資料，以下是 Coach 7 1bpp Font 格式
// --------------------------------------------------------------------------------------------------
// Print font: 2 bytes by 32 lines, align=2
// [ 0] 00 00   '□□□□□□□□□□□□□□□□'
// [ 1] 00 00   '□□□□□□□□□□□□□□□□'
// [ 2] 00 00   '□□□□□□□□□□□□□□□□'
// [ 3] 00 00   '□□□□□□□□□□□□□□□□'
// [ 4] 00 00   '□□□□□□□□□□□□□□□□'
// [ 5] 00 00   '□□□□□□□□□□□□□□□□'
// [ 6] C7 71   '□■■■□□□■■■□□□■■■'
// [ 7] C7 71   '□■■■□□□■■■□□□■■■'
// [ 8] C7 71   '□■■■□□□■■■□□□■■■'
// [ 9] C7 71   '□■■■□□□■■■□□□■■■'
// [10] CE 39   '□□■■■□□■■■□□■■■□'
// [11] 4E 39   '□□■■■□□■□■□□■■■□'
// [12] 4E 39   '□□■■■□□■□■□□■■■□'
// [13] 6E 3B   '□□■■■□■■□■■□■■■□'
// [14] 6E 3B   '□□■■■□■■□■■□■■■□'
// [15] 6E 3B   '□□■■■□■■□■■□■■■□'
// [16] 6E 3B   '□□■■■□■■□■■□■■■□'
// [17] 6C 1B   '□□□■■□■■□■■□■■□□'
// [18] 6C 1B   '□□□■■□■■□■■□■■□□'
// [19] 7C 1F   '□□□■■■■■□■■■■■□□'
// [20] 7C 1F   '□□□■■■■■□■■■■■□□'
// [21] 7C 1F   '□□□■■■■■□■■■■■□□'
// [22] 3C 1E   '□□□■■■■□□□■■■■□□'
// [23] 3C 1E   '□□□■■■■□□□■■■■□□'
// [24] 38 0E   '□□□□■■■□□□■■■□□□'
// [25] 38 0E   '□□□□■■■□□□■■■□□□'
// [26] 38 0E   '□□□□■■■□□□■■■□□□'
// [27] 00 00   '□□□□□□□□□□□□□□□□'
// [28] 00 00   '□□□□□□□□□□□□□□□□'
// [29] 00 00   '□□□□□□□□□□□□□□□□'
// [30] 00 00   '□□□□□□□□□□□□□□□□'
// [31] 00 00   '□□□□□□□□□□□□□□□□'


typedef struct tagStringTable {
  UINT16    GX_signature1;
  UINT8      Format1;
  UINT8      BPS;
  UINT16    GX_signature2;
  UINT8      Format2;
  UINT8      Revd;
  UINT16    Start;
  UINT16    Count;
} StringTable,*PStringTable;

typedef struct tagStringIndexChunk {
 UINT32     Offset;
 UINT32     Size;
} StringIndexChunk,*PStringIndexChunk;

extern void     UniFont_Init(void);
extern void     UniFont_SetStringTable(const STRING_TABLE* pUniFontStringTbl);
extern void     UniFont_DrawString(UINT32 Line,UINT32 StrID);
extern void     UniFont_GetGlyphFromUnicode(UINT16 unicode,LINGUAL_INFO *pLingualInfo);


#endif // _UNIFONT_H
