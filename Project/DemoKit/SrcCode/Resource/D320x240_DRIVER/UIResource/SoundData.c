/**
    Copyright   Novatek Microelectronics Corp. 2005.  All rights reserved.

    @file       SoundData.c
    @ingroup    mIPRJAPKey

    @brief      Keypad tone, Startup... sound data
                This file contain the PCM (ADPCM) data of keypad tone, Startup...

    @note       Nothing.

    @date       2005/01/23
*/

/** \addtogroup mIPRJAPKey */
//@{
#include "Type.h"
#include "SoundData.h"
#include "GxSound.h"
#include "debug.h"
#include "UIInfo.h"
#include "Audio.h"
#include "SoundData_Key.c"
#include "SoundData_Shutter.c"
#include "SoundData_PowerOn.c"

const SOUND_DATA gDemo_Sound[DEMOSOUND_SOUND_MAX_CNT]=
{
    { 0 },
    { g_SoundData_Key,      sizeof(g_SoundData_Key),        AUDIO_SR_32000, FALSE,  DEMOSOUND_SOUND_KEY_TONE        },
    { g_SoundData_Shutter,  sizeof(g_SoundData_Shutter),    AUDIO_SR_32000, FALSE,  DEMOSOUND_SOUND_SHUTTER_TONE    },
    { g_SoundData_PowerOn,  sizeof(g_SoundData_PowerOn),    AUDIO_SR_32000, FALSE,  DEMOSOUND_SOUND_POWERON_TONE    },
};

static volatile BOOL        bKeyToneEn = TRUE;

void UISound_RegTable(void)
{
    ER retV = E_OK;
    retV = GxSound_SetSoundTable
    ((UINT32)DEMOSOUND_SOUND_MAX_CNT, (PSOUND_DATA)&gDemo_Sound[0]);
    if (retV != E_OK)
        debug_err(("Set SoundData Fail:%d\r\n", retV));
}

void UISound_EnableKey(BOOL bEn)
{
    bKeyToneEn = bEn;
}

void UISound_Play(UINT32 index)
{
    if (bKeyToneEn == FALSE)
    {
        return;
    }

    if (UI_GetData(FL_BEEP)== BEEP_ON)
    {
        GxSound_Play(index);
    }
}
//@}
