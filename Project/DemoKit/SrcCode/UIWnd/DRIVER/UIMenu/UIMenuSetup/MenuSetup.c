#include <stdio.h>
#include "UIFlow.h"


// --------------------------------------------------------------------------
// OPTIONS
// --------------------------------------------------------------------------
// Power Saving Options
TMDEF_BEGIN_OPTIONS(AUTO_POWER_OFF)
    TMDEF_OPTION_TEXT(OFF)
    TMDEF_OPTION_TEXT(3MIN)
    TMDEF_OPTION_TEXT(5MIN)
    TMDEF_OPTION_TEXT(10MIN)
TMDEF_END_OPTIONS()


// Beep Options
TMDEF_BEGIN_OPTIONS(BEEP)
    TMDEF_OPTION_TEXT(BEEP_OFF)
    TMDEF_OPTION_TEXT(BEEP_ON)
TMDEF_END_OPTIONS()


// Language Options
#if (UPDATE_CFG == UPDATE_CFG_YES)
TM_OPTION gTM_OPTIONS_LANGUAGE[LANG_ID_MAX] = {0};
#else
TMDEF_BEGIN_OPTIONS(LANGUAGE)
    TMDEF_OPTION_TEXT(LANG_EN)
    TMDEF_OPTION_TEXT(LANG_FR)
    TMDEF_OPTION_TEXT(LANG_ES)
    TMDEF_OPTION_TEXT(LANG_PO)
    TMDEF_OPTION_TEXT(LANG_DE)
    TMDEF_OPTION_TEXT(LANG_IT)
    TMDEF_OPTION_TEXT(LANG_SC)
    TMDEF_OPTION_TEXT(LANG_TC)
    TMDEF_OPTION_TEXT(LANG_RU)
    TMDEF_OPTION_TEXT(LANG_JP)
TMDEF_END_OPTIONS()
#endif

// TV Mode Options
TMDEF_BEGIN_OPTIONS(TV_MODE)
    TMDEF_OPTION_TEXT(TV_NTSC)
    TMDEF_OPTION_TEXT(TV_PAL)
TMDEF_END_OPTIONS()


// Frequency Options
TMDEF_BEGIN_OPTIONS(FREQUENCY)
    TMDEF_OPTION_TEXT(50HZ)
    TMDEF_OPTION_TEXT(60HZ)
TMDEF_END_OPTIONS()

// Frequency Options
TMDEF_BEGIN_OPTIONS(LEDSET)
    TMDEF_OPTION_TEXT(LEDSET_OFF)
    TMDEF_OPTION_TEXT(LEDSET_ON)
TMDEF_END_OPTIONS()

// Sensor Rotate Options
TMDEF_BEGIN_OPTIONS(SENSOR_ROTATE)
    TMDEF_OPTION_TEXT(SENSOR_ROTATE_OFF)
    TMDEF_OPTION_TEXT(SENSOR_ROTATE_ON)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(LCDOFF)
    TMDEF_OPTION_TEXT(LCDOFF_OFF)
    TMDEF_OPTION_TEXT(LCDOFF_30SEC)
    TMDEF_OPTION_TEXT(LCDOFF_1MIN)
    TMDEF_OPTION_TEXT(LCDOFF_2MIN)    
TMDEF_END_OPTIONS()
// Delay off Options
TMDEF_BEGIN_OPTIONS(DELAYOFF)
    TMDEF_OPTION_TEXT(DELAYOFF_OFF)
    TMDEF_OPTION_TEXT(DELAYOFF_10SEC)
    TMDEF_OPTION_TEXT(DELAYOFF_20SEC)
    TMDEF_OPTION_TEXT(DELAYOFF_30SEC)
TMDEF_END_OPTIONS()
    TMDEF_BEGIN_OPTIONS(AUTO_UPDATE_TIME)
        TMDEF_OPTION_TEXT(AUTO_UPDATE_TIME_OFF)
        TMDEF_OPTION_TEXT(GMTM12)   
        TMDEF_OPTION_TEXT(GMTM11)
        TMDEF_OPTION_TEXT(GMTM10)
        TMDEF_OPTION_TEXT(GMTM9)    
        TMDEF_OPTION_TEXT(GMTM8)
        TMDEF_OPTION_TEXT(GMTM7)
        TMDEF_OPTION_TEXT(GMTM6)
        TMDEF_OPTION_TEXT(GMTM5)    
        TMDEF_OPTION_TEXT(GMTM430)
        TMDEF_OPTION_TEXT(GMTM4)    
        TMDEF_OPTION_TEXT(GMTM330)  
        TMDEF_OPTION_TEXT(GMTM3)    
        TMDEF_OPTION_TEXT(GMTM2)    
        TMDEF_OPTION_TEXT(GMTM1)    
        TMDEF_OPTION_TEXT(GMT0) 
        TMDEF_OPTION_TEXT(GMT1) 
        TMDEF_OPTION_TEXT(GMT2) 
        TMDEF_OPTION_TEXT(GMT3) 
        TMDEF_OPTION_TEXT(GMT330)   
        TMDEF_OPTION_TEXT(GMT4) 
        TMDEF_OPTION_TEXT(GMT430)   
        TMDEF_OPTION_TEXT(GMT5)
        TMDEF_OPTION_TEXT(GMT530)
        TMDEF_OPTION_TEXT(GMT545)
        TMDEF_OPTION_TEXT(GMT6) 
        TMDEF_OPTION_TEXT(GMT630)   
        TMDEF_OPTION_TEXT(GMT7)
        TMDEF_OPTION_TEXT(GMT8)
        TMDEF_OPTION_TEXT(GMT9)
        TMDEF_OPTION_TEXT(GMT930)
        TMDEF_OPTION_TEXT(GMT10)
        TMDEF_OPTION_TEXT(GMT11)
        TMDEF_OPTION_TEXT(GMT12)    
        TMDEF_OPTION_TEXT(GMT13)    
    TMDEF_END_OPTIONS()

// --------------------------------------------------------------------------
// ITEMS
// --------------------------------------------------------------------------
static int MenuCustom_DateTime(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupDateTimeCtrl, 0);
    return TMF_PROCESSED;
}

static int MenuCustom_Format(UINT32 uiMessage, UINT32 uiParam)
{
    //Ux_OpenWindow(&UIMenuWndSetupFormatCtrl, 0);
    Ux_OpenWindow(&UIMenuWndSetupFormatConfirmCtrl, 0);
    return TMF_PROCESSED;
}

static int MenuCustom_Default(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupDefaultSettingCtrl, 0);
    return TMF_PROCESSED;
}

static int MenuCustom_Version(UINT32 uiMessage, UINT32 uiParam)
{
    Ux_OpenWindow(&UIMenuWndSetupVersionCtrl, 0);
    return TMF_PROCESSED;
}

// Setup Menu Items
TMDEF_BEGIN_ITEMS(SETUP)
    TMDEF_ITEM_CUSTOM(DATE_TIME, MenuCustom_DateTime)
    TMDEF_ITEM_TEXTID(AUTO_POWER_OFF)
    TMDEF_ITEM_TEXTID(BEEP)
    TMDEF_ITEM_TEXTID(LANGUAGE)
    TMDEF_ITEM_TEXTID(TV_MODE)
    TMDEF_ITEM_TEXTID(FREQUENCY)
    //TMDEF_ITEM_TEXTID(SENSOR_ROTATE)
    TMDEF_ITEM_CUSTOM(FORMAT, MenuCustom_Format)
    TMDEF_ITEM_TEXTID(LCDOFF)    
   // TMDEF_ITEM_TEXTID(LEDSET)    
    TMDEF_ITEM_TEXTID(DELAYOFF)    
     #if (AUTO_UPDATE_DATE_TIME == ENABLE)
    TMDEF_ITEM_TEXTID(AUTO_UPDATE_TIME)
#endif
    TMDEF_ITEM_CUSTOM(DEFAULT, MenuCustom_Default)
    TMDEF_ITEM_CUSTOM(VERSION, MenuCustom_Version)
TMDEF_END_ITEMS()

// --------------------------------------------------------------------------
// Menu Callback
// --------------------------------------------------------------------------
int Setup_MenuCallback(UINT32 uiMessage, UINT32 uiParam)
{
    UINT16  uwItemId;
    UINT16  uwOption;

    if (uiMessage == TMM_CONFIRM_OPTION)
    {
        uwItemId = LO_WORD(uiParam);
        uwOption = HI_WORD(uiParam);

        switch (uwItemId)
        {
        case IDM_AUTO_POWER_OFF:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_POWEROFF, 1, uwOption);
            break;

        case IDM_BEEP:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_BEEPKEY, 1, uwOption);
            break;

        case IDM_LANGUAGE:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_LANGUAGE, 1, uwOption);
            break;

        case IDM_TV_MODE:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_TVFORMAT, 1, uwOption);
            break;

        case IDM_FREQUENCY:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_FREQ, 1, uwOption);
            break;

        case IDM_SENSOR_ROTATE:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_SENSOR_ROTATE, 1, uwOption);
            break;
        case IDM_LCDOFF:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_LCDOFF, 1, uwOption);
            break;

        case IDM_LEDSET:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_LED, 1, uwOption);
            break;			
        case IDM_DELAYOFF:
            Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_DELAYOFF, 1, uwOption);
            break;
        case IDM_AUTO_UPDATE_TIME:
		UI_SetData(FL_AUTO_UPDATE_TIME,uwOption);
            //Ux_SendEvent(&UISetupObjCtrl, NVTEVT_EXE_AUTO_UPDATE_TIME, 1, uwOption);
	    break;
        }
    }

    return TMF_PROCESSED;
}
