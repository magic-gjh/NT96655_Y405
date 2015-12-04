/**
    Copyright   Novatek Microelectronics Corp. 2009.  All rights reserved.

    @file       DetKey.c
    @ingroup    mIPRJAPKeyIO

    @brief      Scan key, modedial
                Scan key, modedial

    @note       Nothing.

    @date       2009/04/22
*/

/** \addtogroup mIPRJAPKeyIO */
//@{

#include "DxCfg.h"
#include "IOCfg.h"

#include "DxInput.h"
#include "Debug.h"
#include "KeyDef.h"
#include "Rtc.h"
#include "IrRx.h"
#include "IrRxParser.h"

#define GPIO_KEY            DISABLE
#define ADC_KEY             ENABLE
#define IRRX_KEY            ENABLE//DISABLE

///////////////////////////////////////////////////////////////////////////////
#define __MODULE__          DxDrv
#define __DBGLVL__          1 // 0=OFF, 1=ERROR, 2=TRACE
#define __DBGFLT__          "*" //*=All, [mark]=CustomClass
#include "DebugModule.h"
///////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// ADC related
////////////////////////////////////////////////////////////////////////////////
// ADC related
//#NT#2009/11/16#Lincy -begin
#if (ADC_KEY == ENABLE)
//#define R_ERR                       (0.05)              // error of resister in unit of %
#define R_ERR                       (0.10)              // error of resister in unit of %
#define R_MAX                       (1 + R_ERR)         // resister max scale
#define R_MIN                       (1 - R_ERR)         // resister min scale
#define ADC_MAX                     1023                // ADC max value (3.3V)

// resister for ADC1 key
#define ADC1_RB                     (15)                // base resister in unit of K-Ohm
#define ADC1_R1                     (0)                 // resister for ADC key 1 in unit of K-Ohm
#define ADC1_R2                     (4.7 + ADC1_R1)     // resister for ADC key 2 in unit of K-Ohm
#define ADC1_R3                     (12  + ADC1_R2)     // resister for ADC key 3 in unit of K-Ohm
#define ADC1_R4                     (30  + ADC1_R3)     // resister for ADC key 4 in unit of K-Ohm

// ADC1 level for key detection
#define VOLDET_KEY_ADC1_LVL1_L      (UINT32)((ADC1_R1*R_MIN*ADC_MAX)/(ADC1_R1*R_MIN+ADC1_RB*R_MAX))
#define VOLDET_KEY_ADC1_LVL1_H      (UINT32)((ADC1_R1*R_MAX*ADC_MAX)/(ADC1_R1*R_MAX+ADC1_RB*R_MIN))
#define VOLDET_KEY_ADC1_LVL2_L      (UINT32)((ADC1_R2*R_MIN*ADC_MAX)/(ADC1_R2*R_MIN+ADC1_RB*R_MAX))
#define VOLDET_KEY_ADC1_LVL2_H      (UINT32)((ADC1_R2*R_MAX*ADC_MAX)/(ADC1_R2*R_MAX+ADC1_RB*R_MIN))
#define VOLDET_KEY_ADC1_LVL3_L      (UINT32)((ADC1_R3*R_MIN*ADC_MAX)/(ADC1_R3*R_MIN+ADC1_RB*R_MAX))
#define VOLDET_KEY_ADC1_LVL3_H      (UINT32)((ADC1_R3*R_MAX*ADC_MAX)/(ADC1_R3*R_MAX+ADC1_RB*R_MIN))
#define VOLDET_KEY_ADC1_LVL4_L      (UINT32)((ADC1_R4*R_MIN*ADC_MAX)/(ADC1_R4*R_MIN+ADC1_RB*R_MAX))
#define VOLDET_KEY_ADC1_LVL4_H      (UINT32)((ADC1_R4*R_MAX*ADC_MAX)/(ADC1_R4*R_MAX+ADC1_RB*R_MIN))

// resister for ADC2 key
#define ADC2_RB                     (10)                // base resister in unit of K-Ohm
#define ADC2_R1                     (2.4)               // resister for ADC key 1 in unit of K-Ohm

// ADC2 level for key detection
#define VOLDET_KEY_ADC2_LVL1_L      (UINT32)((ADC2_R1*R_MIN*ADC_MAX)/(ADC2_R1*R_MIN+ADC2_RB*R_MAX))
#define VOLDET_KEY_ADC2_LVL1_H      (UINT32)((ADC2_R1*R_MAX*ADC_MAX)/(ADC2_R1*R_MAX+ADC2_RB*R_MIN))

#define VOLDET_KEY_LVL_UNKNOWN      0xFFFFFFFF
#define VOLDET_KEY_LVL_MODE         0
#define VOLDET_KEY_LVL_MENU         1
#define VOLDET_KEY_LVL_UP           2
#define VOLDET_KEY_LVL_DOWN         3
#define VOLDET_KEY_LVL_SHUTTER2     4
#endif

#define VOLDET_MS_ADC_RANGE         (60)
#define VOLDET_MS_ADC_LVL0          (0)
#define VOLDET_MS_ADC_LVL1          (262-VOLDET_MS_ADC_RANGE)
#define VOLDET_MS_ADC_LVL2          (525-VOLDET_MS_ADC_RANGE)
#define VOLDET_MS_ADC_LVL3          (788-VOLDET_MS_ADC_RANGE)
#define VOLDET_MS_ADC_TH            (788+VOLDET_MS_ADC_RANGE)

#define VOLDET_MS_LVL_UNKNOWN       0xFFFFFFFF
#define VOLDET_MS_LVL_0             0
#define VOLDET_MS_LVL_1             1
#define VOLDET_MS_LVL_2             2
#define VOLDET_MS_LVL_3             3
#define VOLDET_MS_LVL_4             4
#define VOLDET_MS_LVL_5             5

#if (ADC_KEY == ENABLE)
static UINT32 VolDet_GetKey1ADC(void)
{
#if (VOLDET_ADC_CONT_MODE == DISABLE)
    UINT32 uiADCValue;

    uiADCValue = adc_readData(ADC_CH_VOLDET_KEY1);
    // One-Shot Mode, trigger one-shot
    adc_triggerOneShot(ADC_CH_VOLDET_KEY1);

    return uiADCValue;
#else
    return adc_readData(ADC_CH_VOLDET_KEY1);
#endif
}

static UINT32 VolDet_GetKey2ADC(void)
{
#if (VOLDET_ADC_CONT_MODE == DISABLE)
    UINT32 uiADCValue;

    uiADCValue = adc_readData(ADC_CH_VOLDET_KEY2);
    // One-Shot Mode, trigger one-shot
    adc_triggerOneShot(ADC_CH_VOLDET_KEY2);

    return uiADCValue;
#else
    return adc_readData(ADC_CH_VOLDET_KEY2);
#endif
}


/**
  Get ADC key voltage level

  Get  ADC key  2 voltage level.

  @param void
  @return UINT32 key level, refer to VoltageDet.h -> VOLDET_MS_LVL_XXXX
*/
static UINT32 VolDet_GetKey1Level(void)
{
    static UINT32   uiRetKey1Lvl;
    UINT32          uiKey1ADC, uiCurKey1Lvl;
    UINT32          uiKey1ADCLvl1H;

    uiKey1ADC = VolDet_GetKey1ADC();
    if (uiKey1ADC < 1000)
        DBG_IND("uiKey1ADC %d \r\n", uiKey1ADC);

    if (VOLDET_KEY_ADC1_LVL1_L == 0) // short to ground
        uiKey1ADCLvl1H = 10;         // ADC level error
    else
        uiKey1ADCLvl1H = VOLDET_KEY_ADC1_LVL1_H;

    if (uiKey1ADC >= VOLDET_KEY_ADC1_LVL1_L && uiKey1ADC <= uiKey1ADCLvl1H)
    {
        uiCurKey1Lvl = VOLDET_KEY_LVL_MENU;
        DBG_MSG("MENU %d (%d ~ %d)\r\n", uiKey1ADC, VOLDET_KEY_ADC1_LVL1_L, uiKey1ADCLvl1H);
    }
    else if (uiKey1ADC >= VOLDET_KEY_ADC1_LVL2_L && uiKey1ADC <= VOLDET_KEY_ADC1_LVL2_H)
    {
        uiCurKey1Lvl = VOLDET_KEY_LVL_SHUTTER2;
        DBG_MSG("SHUTTER2 %d (%d ~ %d)\r\n", uiKey1ADC, VOLDET_KEY_ADC1_LVL2_L, VOLDET_KEY_ADC1_LVL2_H);
    }
    else if (uiKey1ADC >= VOLDET_KEY_ADC1_LVL3_L && uiKey1ADC <= VOLDET_KEY_ADC1_LVL3_H)
    {
        uiCurKey1Lvl = VOLDET_KEY_LVL_UP;
        DBG_MSG("UP %d (%d ~ %d)\r\n", uiKey1ADC, VOLDET_KEY_ADC1_LVL3_L, VOLDET_KEY_ADC1_LVL3_H);
    }
    else if (uiKey1ADC >= VOLDET_KEY_ADC1_LVL4_L && uiKey1ADC <= VOLDET_KEY_ADC1_LVL4_H)
    {
        uiCurKey1Lvl = VOLDET_KEY_LVL_DOWN;
        DBG_MSG("DOWN %d (%d ~ %d)\r\n", uiKey1ADC, VOLDET_KEY_ADC1_LVL4_L, VOLDET_KEY_ADC1_LVL4_H);
    }
    else
    {
        uiCurKey1Lvl = VOLDET_KEY_LVL_UNKNOWN;
        if (uiKey1ADC < 1000)
        {
            DBG_MSG("^RUNKNOWN1 %d\r\n", uiKey1ADC);
        }
    }

    uiRetKey1Lvl = uiCurKey1Lvl;

    return uiRetKey1Lvl;
}

static UINT32 VolDet_GetKey2Level(void)
{
    static UINT32   uiRetKey2Lvl;
    UINT32          uiKey2ADC, uiCurKey2Lvl;
    UINT32          uiKey2ADCLvl1H;

    uiKey2ADC = VolDet_GetKey2ADC();
    if (uiKey2ADC < 1000)
        DBG_IND("uiKey2ADC %d \r\n", uiKey2ADC);

    if (VOLDET_KEY_ADC2_LVL1_L == 0) // short to ground
        uiKey2ADCLvl1H = 10;         // ADC level error
    else
        uiKey2ADCLvl1H = VOLDET_KEY_ADC2_LVL1_H;

    if (uiKey2ADC >= VOLDET_KEY_ADC2_LVL1_L && uiKey2ADC <= uiKey2ADCLvl1H)
    {
        uiCurKey2Lvl = VOLDET_KEY_LVL_MODE;
        DBG_MSG("MODE %d (%d ~ %d)\r\n", uiKey2ADC, VOLDET_KEY_ADC2_LVL1_L, uiKey2ADCLvl1H);
    }
    else
    {
        uiCurKey2Lvl = VOLDET_KEY_LVL_UNKNOWN;
        if (uiKey2ADC < 1000)
        {
            DBG_MSG("^RUNKNOWN2 %d\r\n", uiKey2ADC);
        }
    }

    uiRetKey2Lvl = uiCurKey2Lvl;

    return uiRetKey2Lvl;
}

#endif
/**
  Get IrRx key

  Get IrRx key.

  @param void
  @return UINT32 key level, refer to VoltageDet.h -> VOLDET_MS_LVL_XXXX
*/
#if (IRRX_KEY == ENABLE)
static BOOL IrRxDet_Key(UINT32 *uiKey)
{
  IRRX_DATA_QUEUE IrRxGetDataQ = {0, 0, 0};
  static UINT32 uiIrRxData = 0;
  static UINT32 uiPreIrRxData = 0;

    if(irrx_dequeueData((PIRRX_DATA_QUEUE)&IrRxGetDataQ))
    {
        uiIrRxData = IrRxGetDataQ.uiRxData;
        debug_ind(("IrRx(%010lu): CODE: 0x%08X\r\n", IrRxGetDataQ.uiRxTime, uiIrRxData));

        *uiKey = IrRx_ParseKey(g_pIrCodeTb, uiIrRxData);
        uiPreIrRxData = *uiKey;
         return TRUE;

    } else if (uiPreIrRxData!=0)
    {
        // return IR key one more time because keyscan time interval is smaller than IR key scan time interval
        // and this will get zero value when keyscan task get data the sencondary time
        *uiKey = uiPreIrRxData;
        // clear uiPreIrRxData
        uiPreIrRxData = 0;
        return TRUE;
    }
    *uiKey = 0;
    return FALSE;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// GPIO related

//static BOOL g_bIsShutter2Pressed = FALSE;



/**
  Delay between toggle GPIO pin of input/output

  Delay between toggle GPIO pin of input/output

  @param void
  @return void
*/
static void DrvKey_DetKeyDelay(void)
{
    gpio_readData(0);
    gpio_readData(0);
    gpio_readData(0);
    gpio_readData(0);
}


void DrvKey_Init(void)
{
}
/**
  Detect normal key is pressed or not.

  Detect normal key is pressed or not.
  Return key pressed status (refer to KeyDef.h)

  @param void
  @return UINT32
*/
UINT32 DrvKey_DetNormalKey(void)
{
    UINT32 uiKeyCode = 0;
#if (IRRX_KEY==ENABLE)
    static UINT32 uiIrRxKey = 0;
#endif

#if (ADC_KEY == ENABLE)
    if (VolDet_GetKey1Level() == VOLDET_KEY_LVL_UP)
    {
        uiKeyCode |= FLGKEY_UP;
    }
    if (VolDet_GetKey1Level() == VOLDET_KEY_LVL_DOWN)
    {
        uiKeyCode |= FLGKEY_DOWN;
    }
    if (VolDet_GetKey1Level() == VOLDET_KEY_LVL_SHUTTER2)
    {
        uiKeyCode |= FLGKEY_SHUTTER2;
    }
    if (VolDet_GetKey1Level() == VOLDET_KEY_LVL_MENU)
    {
        uiKeyCode |= FLGKEY_MENU;
    }
    if (VolDet_GetKey2Level() == VOLDET_KEY_LVL_MODE)
    {
        uiKeyCode |= FLGKEY_MODE;
    }
#endif
#if (GPIO_KEY == ENABLE)
    if(!gpio_getPin(GPIO_KEY_SHUTTER2))
    {
        uiKeyCode |= FLGKEY_SHUTTER2;
    }
#endif
/*
    //detect if power-on by press playback key
    if (!rtc_getPWR2Status())
    {
        uiKeyCode |= FLGKEY_PLAYBACK;
    }
*/
#if (IRRX_KEY==ENABLE)
        if (IrRxDet_Key(&uiIrRxKey)==TRUE)
        {
            uiKeyCode |= uiIrRxKey;
        }
#endif
    DBG_IND("KEY=%08x\r\n", uiKeyCode);

    DrvKey_DetKeyDelay();
    return uiKeyCode;
}

/**
  Detect power key is pressed or not.

  Detect power key is pressed or not.
  Return key pressed status (refer to KeyDef.h)

  @param void
  @return UINT32
*/
UINT32 DrvKey_DetPowerKey(void)
{
    UINT32 uiKeyCode = 0;

    if(rtc_getPWRStatus())
    {
        uiKeyCode = FLGKEY_KEY_POWER;
        // Reset shutdown timer
        rtc_resetShutdownTimer();
    }

    return uiKeyCode;
}

UINT32 DrvKey_DetStatusKey(DX_STATUS_KEY_GROUP KeyGroup)
{
    UINT32 uiReturn = STATUS_KEY_LVL_UNKNOWN;
    switch (KeyGroup)
    {
        case DX_STATUS_KEY_GROUP1:
            break;

        case DX_STATUS_KEY_GROUP2:
            break;

        case DX_STATUS_KEY_GROUP3:
            break;

        case DX_STATUS_KEY_GROUP4:
            break;

        case DX_STATUS_KEY_GROUP5:
            break;

        default:
            DBG_ERR("[StatusKey]no this attribute");
            break;
    }
    return uiReturn;
}
