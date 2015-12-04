/**
    UI Interface of MN34110 Calibration.

    Processing MSG & Display on LCD.

    @file            Cal_UICB_MN34110_650.c
    @brief          Processing MSG & Display
    @ingroup    Calibration
    @note          UI Layer
    @author      Randy Ho
    @Date

    Copyright   Novatek Microelectronics Corp. 2013.  All rights reserved.
*/
#include    "stdio.h"

#include    "Type.h"
#include    "GxDC.h"
#include    "GxGfx.h"
#include    "GxVideo.h"
#include    "GxInput.h"
#include    "KeyDef.h"
#include    "Delay.h"

#include    "IPL_Calibration.h"

#include    "UIDisplay.h"
#include    "UIGraphics.h"
#include    "UIResource.h"
#include    "CalibrationAPI.h"
#include    "DxInput.h"

static void Cal_ClrOSD(void);
static void Cal_DisplayString(CHAR *pStr, UINT16 X, UINT16 Y);
static void Cal_WaitKey(UINT32 Key);
static void UICB_Common(CalUICB UICB,CHAR* pstr);

static void Cal_ClrOSD(void)
{
    Cal_ClearOSD(_OSD_INDEX_TRANSPART);

    return;
}

static void Cal_DisplayString(CHAR *pStr, UINT16 X, UINT16 Y)
{
    Cal_ShowStringWithColor(pStr, X, Y, 3);

    return;
}

static void Cal_WaitKey(UINT32 Key)
{
    do
    {
        Delay_DelayMs(10);
    }while(DrvKey_DetNormalKey()!=Key);

    return;
}

static void UICB_Common(CalUICB UICB,CHAR* pstr)
{
    switch(UICB)
    {
         case UICB_START:
             // Processing this msg means starting to inital calibration item

            // Clear screen first
            Cal_ClrOSD();

            // Display op instruction
            if(pstr)
            {
                Cal_DisplayString(pstr, 20, 30);
            }
            break;

        case UICB_WAITKEY:
            if(pstr)
            {
                Cal_DisplayString(pstr, 20, 150);
            }

            Cal_WaitKey(FLGKEY_SHUTTER1);
            Cal_ClrOSD();

            break;

        case UICB_PROCESSING:
            if(pstr)
            {
                Cal_DisplayString(pstr, 20, 80);
            }

            // Cal_ClrOSD();
            break;

        case UICB_NONE:
        case UICB_END:
        default:
            break;
    }

    return;
}

static void UICB_DP_W(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_DP_D(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_ECS(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_ISO(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_AWB(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_MSHUT(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    UICB_Common(UICB, pstr);

    return;
}

static void UICB_CAL_STATUS(CalUICB UICB,CHAR* pstr, UINT32 TextColor)
{
    switch(UICB)
    {
         case UICB_START:
             // Processing this msg means starting to initialize calibration item

            // Clear screen first
            Cal_ClrOSD();

            Cal_DisplayString("CAL STATUS", 20, 10);

            break;

        case UICB_WAITKEY:
            Cal_DisplayString("Press Shutter to EXIT", 20, 210);

            Cal_WaitKey(FLGKEY_SHUTTER1);

            break;

        case UICB_PROCESSING:
            /*
                   The first character says the calibration item...
            */
            if(pstr)
            {
                switch(*pstr)
                {
                    case '1':
                        Cal_DisplayString(pstr+1, 20, 95);
                        break;

                    case '2':
                        Cal_DisplayString(pstr+1, 20, 115);
                        break;

                    case '3':
                        Cal_DisplayString(pstr+1, 20, 135);
                        break;

                    case '4':
                        Cal_DisplayString(pstr+1, 20, 155);
                        break;

                    default:
                        Cal_DisplayString(pstr, 20, 175);
                        break;
                }

            }
            break;

        case UICB_NONE:
        case UICB_END:
        default:
            break;

    return;
}
}

CAL_UICB_OBJ Cal_UI =
{
    UICB_AWB,
    UICB_ISO,
    UICB_ECS,
    UICB_MSHUT,
    UICB_DP_W,
    UICB_DP_D,
    UICB_CAL_STATUS,
};

pCAL_UICB_OBJ CAL_UICB(void)
{
    return &Cal_UI;

}
