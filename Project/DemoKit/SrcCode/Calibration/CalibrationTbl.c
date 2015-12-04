/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       CalibrationTbl.c
    @ingroup    mIPRJAPTest

    @brief      Calibration item table
                Calibration item table

    @note       Nothing.

    @date       2006/01/02
*/

/** \addtogroup mIPRJAPTest */
//@{

#include "CalibrationInt.h"
#include "CalibrationAPI.h"
#include "CalBurnIn.h"
#include "CalLCD.h"
#include "CalCIPA.h"
#include "CalMISC.h"
#include "UIFramework.h"
#include "UIFlow.h"
#include "Debug.h"
#include "CalSensor.h"
#include "CalLens.h"
#include "UIConfig.h"

#if (_CALIBRATION_MODE_ == ENABLE)

// -------------------------------------------------------------------
// Internal Global variables
// -------------------------------------------------------------------
// Declare all global variables used in Calibration task here.
// Don't reference these variables in your code.

void Cal_Test_1(void)
{
    //FLGPTN  uiFlag = 0;
    IRECT    StringRect = {20, 100, 300, 30};

    //UI_ClearOSD(_OSD_INDEX_TRANSPART); // clear OSD
    Cal_ShowString("Press UP key to exit", &StringRect, TRUE);

    //while (1)
    {
        //wai_flg(&uiFlag, FLG_ID_KEY, FLGKEY_UP, TWF_ORW | TWF_CLR);
        //break;
    }
}

// Calibration item table, please keep at least one item to make calibration task
// works correctly
CAL_TERM g_CalItem[]    =
{
    // Display Name           Function pointer
    {"Format SPI",            Cal_FormatPstoreSPI   },
    {"ECS",                   Cal_ECS               },
    {"Bright Defect Pixel",   Cal_DefectPixel_Bright},
    {"Dark Defect Pixel",     Cal_DefectPixel_Dark  },
    {"ISO",                   Cal_ISO               },
    {"AWB",                   Cal_AWB               },
    {"WriteAWBGS",            Cal_WriteAWBGS        },
    {"Cal Status",            Cal_ReadCalStatus     },
};

UINT32 g_CalMaxItemNum = sizeof(g_CalItem) / sizeof(CAL_TERM) - 1;

UINT32 Cal_GetMaxItemNum(void)
{
    return g_CalMaxItemNum;
}

#endif

//@}
