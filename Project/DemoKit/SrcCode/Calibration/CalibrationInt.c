/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       CalibrationTsk.c
    @ingroup    mIPRJAPTest

    @brief      Calibration task
                Calibration task

    @note       Nothing.

    @date       2006/01/02
*/

/** \addtogroup mIPRJAPTest */
//@{

#include "Kernel.h"
#include "SysCfg.h"
#include "CalibrationInt.h"
#include "UIFlow.h"
#include "Type.h"

#if (_CALIBRATION_MODE_ == ENABLE)
#include "UIFramework.h"
//static DC**     pDCList;

// -------------------------------------------------------------------
// Internal Global variables
// -------------------------------------------------------------------
// Declare all global variables used in Calibration task here.
// Don't reference these variables in your code.
volatile BOOL               g_bCalbirationOpened = FALSE;
         IRECT               g_CalStringRect;
         IRECT               g_CalString2Rect;
//volatile CAL_APPOBJ         g_CalAppObj = { NULL, NULL };
#endif

//@}
