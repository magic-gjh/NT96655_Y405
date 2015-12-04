/**
    Calibration item

    @file       CalibrationItem.c
    @ingroup    calibration
    @note       Nothing (or anything need to be mentioned).

    Copyright   Novatek Microelectronics Corp. 2011.  All rights reserved.
*/

#include "CalibrationInt.h"
#include "debug.h"
#include "CalLens.h"
#include "Cal_Dummy_650.h"
UINT32 Cal_RunItem(CAL_ITEM AlgCal_item)
{
    ER ErrorCode = CAL_ERR_OK;

    switch(AlgCal_item)
    {
        case CAL_ITEM_ISO:
            ErrorCode = Cal_SenISO();
            break;

        case CAL_ITEM_MSHUTTER:
            ErrorCode = Cal_SenMShutter();
            break;

        case CAL_ITEM_DEFECTPIXEL:
            ErrorCode = Cal_SenDP();
            break;

        case CAL_ITEM_DEFECTPIXEL_WHITE:
            ErrorCode = Cal_SenDP_Bright();
            break;

        case CAL_ITEM_DEFECTPIXEL_DARK:
            ErrorCode = Cal_SenDP_Dark();
            break;

        case CAL_ITEM_AWB:
            ErrorCode = Cal_SenAWB();
            break;

        case CAL_ITEM_SAVEAWBGS:
            ErrorCode = Cal_SenAWBGS();
            break;

        case CAL_ITEM_ECS:
            ErrorCode = Cal_SenECS();
            break;

        case CAL_ITEM_OB:
            break;

        case CAL_ITEM_READSTATUS:
            ErrorCode = Cal_Get_DNP_Status();
            break;

        case CAL_ITEM_AF_NEAR:
            //ErrorCode = CalLens_NearFocusCalibration();
            break;

        case CAL_ITEM_AF_FAR:
            //ErrorCode = CalLens_FarFocusCalibration();
            break;

        case CAL_ITEM_FOCUS_STEPMISS_TEST:
            //ErrorCode = CalLens_FocusStepMissTest();
            break;

        case CAL_ITEM_LENS_QUALITY_TEST:
            //ErrorCode = CalLens_LensQualityTest();
            break;

        case CAL_ITEM_FLASH_AWB:
            break;

        default:
            ErrorCode = CAL_ERR_NULL;
            debug_ind(("No this item\r\n"));
            break;
    }
    return ErrorCode;
}
//@}
