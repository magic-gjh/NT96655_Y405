
#include "Type.h"
#include "PrjCfg.h"

#if (MULTILINGUAL_FUNCTION == ENABLE)

#include "UniFont.h"
#include "ArUnicodeMS_H27.h"
#include "mi.h"
#include "DemoKit_String.h"


#define UniFont_Debug   DISABLE

#define LEFT_MARGIN          2
#define RIGHT_MARGIN         2
#define TOP_MARGIN           2
#define BOTTOM_MARGIN        2

UFN_FILE_HEADER g_UFN_Header;
static  UINT8   *g_pGlyphData=0;
static  UINT32  g_pixelCount = 0;
static  UINT8   g_pixeldata = 0;
static  UINT8   g_pixelMaxH = 14; // initial value, define yourself


void UniFont_Init(void)
{
   memcpy(&g_UFN_Header,g_ArUnicodeMS_H27,sizeof(UFN_FILE_HEADER));

#if (UniFont_Debug==ENABLE)
   DBGD(g_UFN_Header.cbThis);
   debug_msg("g_UFN_Header.cFaceName:%s\r\n",g_UFN_Header.cFaceName);
   DBGD(g_UFN_Header.wHeight);
   DBGD(g_UFN_Header.wDescent);
   DBGD(g_UFN_Header.wGlyphsCount);
   DBGD(g_UFN_Header.cRanges);
   DBGD(g_UFN_Header.FileHandle);
   DBGD(g_UFN_Header.FontHandle);
   DBGH(g_UFN_Header.RangeTable);
   DBGH(g_UFN_Header.IndexTable);
   DBGH(g_UFN_Header.WidthTable);
   DBGH(g_UFN_Header.OffsetTable);
   DBGD(g_UFN_Header.GlyphData);
#endif
}

static UINT16 UniFont_UnicodeToIndex(UINT16 unicode)
{
  UINT16 range_no;
  UINT16 index;
  UINT16 UnicodeIndex;
  UFN_RANGE RangeTab;
  UINT32  RangeTabIndex;
  UINT32  IndexTabIndex;


    // 由低到高走訪每個區段，用區段的結尾碼來找出 unicode 可能會落在那個區段
    for(range_no = 0; range_no < g_UFN_Header.cRanges; range_no++)
    {
        RangeTabIndex = (UINT32)(g_UFN_Header.RangeTable+range_no);
        memcpy(&RangeTab,&g_ArUnicodeMS_H27[RangeTabIndex],sizeof(UFN_RANGE));

        if( unicode <= RangeTab.uwLast )
        {
            if( unicode < RangeTab.uwFirst )
                return 0xFFFF;  // 區段不包含此 unicode

            // unicode 和區段頭碼的差量再加上本區段對應的啟始索引就可得到
            // 此 unicode 是第幾個 Glyph
            IndexTabIndex = (UINT32)(g_UFN_Header.IndexTable+range_no);
            memcpy(&UnicodeIndex,&g_ArUnicodeMS_H27[IndexTabIndex],sizeof(UINT16));
            index = UnicodeIndex + unicode - RangeTab.uwFirst;

            return index;
        }
    }
    return 0xFFFF;
}

static void UniFont_UniCodeGetGlyph(UFN_GLYPH *pglyph,UINT16 wGlyphIndex)
{
 UINT16  glyph_data_len,glyph_total_len;
 UINT32  offset;
 UINT32  buffer[34];  // UFN_GLYPHINFO data size+scan 32 lines*4 bytes
 UFN_GLYPHINFO *pgi;
 UINT32 OffsetTabIndex;


    if( wGlyphIndex >= g_UFN_Header.wGlyphsCount )
    {
        debug_msg("Invalided wGlyphIndex\n\r");
        return ;
    }

    OffsetTabIndex = (UINT32)(g_UFN_Header.OffsetTable+wGlyphIndex);
    memcpy(&offset,&g_ArUnicodeMS_H27[OffsetTabIndex],sizeof(UINT32));
    glyph_data_len = (UINT16)(offset>>24);
    glyph_total_len = glyph_data_len+sizeof(UFN_GLYPHINFO);
    offset &= 0x0FFFFFF;
    offset += g_UFN_Header.GlyphData;

    // read Glyph Data
    memcpy(buffer,&g_ArUnicodeMS_H27[offset],glyph_total_len);
    pgi = (UFN_GLYPHINFO *)buffer;
    pglyph->GLYPHINFO = *pgi;
    memcpy(&pglyph->SCAN,&pgi[1],glyph_data_len);
}

static void UniFont_GlyphConcatenate2BitData(UINT8 data)
{

     g_pixeldata |= ((data&0x03)<<(6-g_pixelCount%4*2));
     g_pixelCount++;
     if (g_pixelCount%4==0)
     {
       *g_pGlyphData++ = g_pixeldata;
       g_pixeldata=0;
       //g_pGlyphData++;
     }
}

static void UniFont_GlyphScanDataTo2BitData(UFN_GLYPH *pglyph,UINT8 data)
{
 UINT8 bitindex = 0x80;
 int index;
 static int printNum=1;

    for (index=0;index<8;index++)
    {
        if (data&bitindex)
            UniFont_GlyphConcatenate2BitData(0x2); // foreground data
        else
            UniFont_GlyphConcatenate2BitData(0x0); // background data

        if (printNum>=pglyph->GLYPHINFO.wBoxX)
        {
            printNum=1;
            break;
        } else
            printNum++;
        bitindex >>= 1;
    }
}

static void UniFont_Glyph1bitDataTo2bit(UFN_GLYPH *pglyph)
{
  UINT16  row,col,margin;
  UINT8   data;
  UINT32  top_dummy,bottom_dummy;


  debug_ind(("wOrg(%d,%d),wBox(%d,%d)\r\n",\
      pglyph->GLYPHINFO.wOrgX,\
      pglyph->GLYPHINFO.wOrgY,\
      pglyph->GLYPHINFO.wBoxX,\
      pglyph->GLYPHINFO.wBoxY));

  top_dummy = 0;
  bottom_dummy = 0;
  if ((pglyph->GLYPHINFO.wBoxY+TOP_MARGIN+BOTTOM_MARGIN)>g_pixelMaxH)
  {
     g_pixelMaxH = pglyph->GLYPHINFO.wBoxY+TOP_MARGIN+BOTTOM_MARGIN;
  } else {
     // add another dummy row for align all lingual character's height
     bottom_dummy = (g_pixelMaxH-(pglyph->GLYPHINFO.wBoxY+TOP_MARGIN+BOTTOM_MARGIN)) >> 1;
     top_dummy = g_pixelMaxH - (pglyph->GLYPHINFO.wBoxY+TOP_MARGIN+BOTTOM_MARGIN) - bottom_dummy;
  }

  // Insert top blank into dummy data
  for (row=0;row<(TOP_MARGIN+top_dummy);row++)
  {
    for (col=0;col<(pglyph->GLYPHINFO.wBoxX+LEFT_MARGIN+RIGHT_MARGIN);col++)
    {
        UniFont_GlyphConcatenate2BitData(0x00);
    }
  }

  for (row=0;row<pglyph->GLYPHINFO.wBoxY;row++)
  {
    // Insert left blank into dummy data
    for (margin=0;margin<LEFT_MARGIN;margin++)
         UniFont_GlyphConcatenate2BitData(0x00);

    // here is real Glyph real scan data
    for (col=0;col<pglyph->GLYPHINFO.wBoxX;col+=8)
        if (pglyph->GLYPHINFO.wBoxX>col)
        {
          data = (UINT8)(pglyph->SCAN.scanData[row]>>col)&0xFF;
          UniFont_GlyphScanDataTo2BitData(pglyph,data);
        }

    // Insert right blank into dummy data
    for (margin=0;margin<RIGHT_MARGIN;margin++)
        UniFont_GlyphConcatenate2BitData(0x00);
  }

  // Insert bottom blank into dummy data
  for (row=0;row<(BOTTOM_MARGIN+bottom_dummy);row++)
  {
    for (col=0;col<(pglyph->GLYPHINFO.wBoxX+LEFT_MARGIN+RIGHT_MARGIN);col++)
    {
        UniFont_GlyphConcatenate2BitData(0x00);
    }
  }
}

static UINT8 UniFont_GlyphGet2bitData(UINT32 bitsOneRow,UINT32 x,UINT32 y)
{
 UINT32  bitsOffset,bytesOffset,bitsRemainder;

    bitsOffset = bitsOneRow*y+x*2;
    bitsRemainder = (bitsOffset%8)>>1;
    bytesOffset = bitsOffset/8;

    return (g_pGlyphData[bytesOffset]>>(6-(bitsRemainder%4*2)))&0x03;
}

static void UniFont_GlyphSet2bitData(UINT32 bitsOneRow,UINT32 x,UINT32 y,UINT8 frame)
{
 UINT32  bitsOffset,bytesOffset,bitsRemainder;

    bitsOffset = bitsOneRow*y+x*2;
    bitsRemainder = (bitsOffset%8)>>1;
    bytesOffset = bitsOffset/8;

    g_pGlyphData[bytesOffset] &= ~(0x03<<(6-(bitsRemainder%4*2)));
    g_pGlyphData[bytesOffset] |= (frame&0x03)<<(6-bitsRemainder%4*2);
}


static void UniFont_Glyph2bitDataForFrame(UFN_GLYPH *pglyph)
{
  UINT32  width,height;
  UINT32  col,row;
  UINT32  bitsOneRow;
  UINT8   up,down,right,left;

   pglyph->GLYPHINFO.wBoxX += (LEFT_MARGIN+RIGHT_MARGIN);
   pglyph->GLYPHINFO.wBoxY = g_pixelMaxH;

//   DBGD(pglyph->GLYPHINFO.wBoxX);
   width = pglyph->GLYPHINFO.wBoxX;
   height = pglyph->GLYPHINFO.wBoxY;

   bitsOneRow = width*2;
   for (row=0;row<height;row++)
     for (col=0;col<width;col++)
       {
          if (UniFont_GlyphGet2bitData(bitsOneRow,col,row)==0x02)
          {
            //printf("(row,col)=(%d,%d)\r\n",row,col);
            continue;
          }

          if (row==0)
           {
             up = 0;
             if (col==0)
             {
               left =0;
               right = UniFont_GlyphGet2bitData(bitsOneRow,col+1,row);
               down = UniFont_GlyphGet2bitData(bitsOneRow,col,row+1);
             } else if (col==(width-1))
             {
               right=0;
               left = UniFont_GlyphGet2bitData(bitsOneRow,col-1,row);
               down = UniFont_GlyphGet2bitData(bitsOneRow,col,row+1);
             }
             else {
               left = UniFont_GlyphGet2bitData(bitsOneRow,col-1,row);
               right = UniFont_GlyphGet2bitData(bitsOneRow,col+1,row);
               down = UniFont_GlyphGet2bitData(bitsOneRow,col,row+1);
             }
           }
          else if (row == (height-1))
          {
            down=0;
            if (col==0)
            {
               left =0;
               right = UniFont_GlyphGet2bitData(bitsOneRow,col+1,row);
               up = UniFont_GlyphGet2bitData(bitsOneRow,col,row-1);
            } else if (col==(width-1))
            {
               left = UniFont_GlyphGet2bitData(bitsOneRow,col-1,row);
               right = 0;
               up = UniFont_GlyphGet2bitData(bitsOneRow,col,row-1);
            }else {
               left = UniFont_GlyphGet2bitData(bitsOneRow,col-1,row);
               right = UniFont_GlyphGet2bitData(bitsOneRow,col+1,row);
               up = UniFont_GlyphGet2bitData(bitsOneRow,col,row-1);
            }
          } else {
            up = UniFont_GlyphGet2bitData(bitsOneRow,col,row-1);
            down = UniFont_GlyphGet2bitData(bitsOneRow,col,row+1);
            left = UniFont_GlyphGet2bitData(bitsOneRow,col-1,row);
            right = UniFont_GlyphGet2bitData(bitsOneRow,col+1,row);
          }

          // update frame value to pixel data
          if (up==2||down==2||left==2||right==2)
          {
              //printf("(row,col)=(%d,%d),up:%d,down:%d,left:%d,right:%d\r\n",row,col,up,down,left,right);
              UniFont_GlyphSet2bitData(bitsOneRow,col,row,0x01);
          }
       } // end for loop
}

void UniFont_GetGlyphFromUnicode(UINT16 unicode,LINGUAL_INFO *pLingualInfo)
{
 UINT16 wGlyphIndex;

    memset(pLingualInfo->GlyphData,0,GLYPHDATA_LENGTH);
    g_pixelCount = 0;
    g_pixeldata = 0;
    g_pGlyphData = (UINT8 *)pLingualInfo->GlyphData;

     // get Glyphs index in UFN file
     wGlyphIndex = UniFont_UnicodeToIndex(unicode);
     if (wGlyphIndex==0xFFFF)
     {
         debug_msg("unicode %x not found in this UFN file\r\n",unicode);
         return;
     } else {
//         DBGD(wGlyphIndex);
     }


     // get Glyphs data in UFN file
     UniFont_UniCodeGetGlyph(&pLingualInfo->glyph, wGlyphIndex);

     // Convert to 2 bits from 1 bit per pixel in unifont's glyph scan data.
     UniFont_Glyph1bitDataTo2bit(&pLingualInfo->glyph);

     // reset g_pGlyphData buffer pointer after converting 2 bits per pixel.
     g_pGlyphData = (UINT8 *)pLingualInfo->GlyphData;

     // search for Frame pixel in glyph scan data.
     UniFont_Glyph2bitDataForFrame(&pLingualInfo->glyph);
}
#endif

