#include "UIAppMovie.h"
#include "NVTUserCommand.h"
#include "uimode.h"
#include "ae_task.h"
#include "awb_task.h"
#include "SysCfg.h"
#include "MediaRecAPI.h"
// Audio NR
#include "AudFilterAPI.h"
#include "UIAppMovie_AudNR.h"
#include "Lens.h"

#define __MODULE__          UIMovieCB
//#define __DBGLVL__ 0        //OFF mode, show nothing
//#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGLVL__ 2        //TRACE mode, show err, wrn, ind, msg and func and ind, msg and func can be filtering by __DBGFLT__ settings
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"

#define AUDNR_DELAYFRAME  2

// Lens State
static LENS_CB_MESSAGE gUIMovieLensState = 0;

UINT32 gAudNrIrisDelayFrame = AUDNR_DELAYFRAME;
UINT32 gAudNrAFDelayFrame = AUDNR_DELAYFRAME;
BOOL   gbAudNrZoomOperating = FALSE;


//-------------------------------------------------------------------------------------------------
//
// Callback function for audio filtering & Noise Reduction
//
//-------------------------------------------------------------------------------------------------
void UIMovie_AudFilterProc(void)
{
    // Audio NR
    UINT32              uiNrStopFreq, uiNrReplaceFreq, uiNrDecreaseLvl, uiNrMixBackgrd;
    //INT32               iNrBandNum;
    AUDNR_NOISE_SPEC    *pNoiseSpec = NULL;

    //
    //  Default Config
    //
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_SAMPLE_RATE,            32000);
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_CHANNEL_NO,             2);

    //
    // AudNR Background Suppress Enabled
    //
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_BACKGROUND_ENABLE,       FALSE);
    //AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_BACKGROUND_DECREASE_LVL, AUDNR_COEF(4.0));
    //AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_BACKGROUND_LOWER_BOUND,  AUDNR_COEF(0.1));

    //
    // Aud NR Enabled
    //
    switch (gUIMovieLensState)
    {
        case LENS_CB_ZOOM_START: // Zoom
        {
            //DBG_IND("Lens state is Zoom Moving!\r\n");
            uiNrStopFreq    = gAudNrZoomStopFreq;
            uiNrReplaceFreq = gAudNrZoomReplaceFreq;
            uiNrDecreaseLvl = gAudNrZoomDecreaseLvl;
            uiNrMixBackgrd  = gAudNrZoomMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Zoom;
            //iNrBandNum      = gAudNrZoomBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrZoomBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrZoomNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrZoomNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrZoomNoiseSpec;

            gAudNrAFDelayFrame = gAudNrIrisDelayFrame = AUDNR_DELAYFRAME;
            gbAudNrZoomOperating = TRUE;
            AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                         TRUE);
        }
        break;

        case LENS_CB_FOCUS_START: // Focus
        {
            //DBG_IND("Lens state is Focus Moving!\r\n");
            if (gbAudNrZoomOperating) return; // Zoom operation is enabling, skip AF event
            uiNrStopFreq    = gAudNrFocusStopFreq;
            uiNrReplaceFreq = gAudNrFocusReplaceFreq;
            uiNrDecreaseLvl = gAudNrFocusDecreaseLvl;
            uiNrMixBackgrd  = gAudNrFocusMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Focus;
            //iNrBandNum      = gAudNrFocusBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrFocusBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrFocusNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrFocusNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrFocusNoiseSpec;

            gAudNrAFDelayFrame = 0; // Reset delay frame count
            AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                         TRUE);
        }
        break;

        case LENS_CB_APERTURE_START: // Iris
        {
            // Support CAF case
            if((gAudNrAFDelayFrame < AUDNR_DELAYFRAME) ||(gbAudNrZoomOperating)) // Zoom operation and Auto Focus is enabling, skip AE event
                return;

            //DBG_IND("Lens state is Iris Moving!\r\n");
            uiNrStopFreq    = gAudNrIrisStopFreq;
            uiNrReplaceFreq = gAudNrIrisReplaceFreq;
            uiNrDecreaseLvl = gAudNrIrisDecreaseLvl;
            uiNrMixBackgrd  = gAudNrIrisMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Iris;
            //iNrBandNum      = gAudNrIrisBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrIrisBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrIrisNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrIrisNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrIrisNoiseSpec;

            gAudNrIrisDelayFrame = 0; // Reset delay frame count
            AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                         TRUE);
        }
        break;


        default:
        case LENS_CB_ZOOM_END:
        {
            //DBG_IND("Lens state is ZOOM end!\r\n");
            uiNrStopFreq    = gAudNrZoomStopFreq;
            uiNrReplaceFreq = gAudNrZoomReplaceFreq;
            uiNrDecreaseLvl = gAudNrZoomDecreaseLvl;
            uiNrMixBackgrd  = gAudNrZoomMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Zoom;
            //iNrBandNum      = gAudNrZoomBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrZoomBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrZoomNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrZoomNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrZoomNoiseSpec;

            gbAudNrZoomOperating = FALSE;
            AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                         FALSE);
        }
        break;

        case LENS_CB_FOCUS_END:
        {
            //DBG_IND("Lens state is Idle!\r\n");
            if (gbAudNrZoomOperating) return; // Zoom operation is enabling, skip AF event

            uiNrStopFreq    = gAudNrFocusStopFreq;
            uiNrReplaceFreq = gAudNrFocusReplaceFreq;
            uiNrDecreaseLvl = gAudNrFocusDecreaseLvl;
            uiNrMixBackgrd  = gAudNrFocusMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Focus;
            //iNrBandNum      = gAudNrFocusBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrFocusBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrFocusNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrFocusNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrFocusNoiseSpec;

            if(gAudNrAFDelayFrame >= AUDNR_DELAYFRAME)
            {
                //DBG_IND("IRIS Lens state is Idle!\r\n");
                AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                     FALSE);
            }
            else
            {
                //DBG_IND("IRIS Lens state is not Idle!\r\n");
                gAudNrAFDelayFrame++;
                AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                     TRUE);
            }
        }
        break;

        case LENS_CB_APERTURE_END:
        {
            // Support CAF case
            if((gAudNrAFDelayFrame < AUDNR_DELAYFRAME) ||(gbAudNrZoomOperating)) // Zoom operation and Auto Focus is enabling, skip AE event
                return;

            uiNrStopFreq    = gAudNrIrisStopFreq;
            uiNrReplaceFreq = gAudNrIrisReplaceFreq;
            uiNrDecreaseLvl = gAudNrIrisDecreaseLvl;
            uiNrMixBackgrd  = gAudNrIrisMixBackgrd;
            pNoiseSpec      = &gpAudNrNoiseSpec_Iris;
            //iNrBandNum      = gAudNrIrisBandNum;
            //NoiseSpec.piFreqBandNo = &gAudNrIrisBandNum;
            //NoiseSpec.piFreqBand   = gpAudNrIrisNoiseFreqBand;
            //NoiseSpec.piNoisePower = gpAudNrIrisNoisePower;
            //NoiseSpec.piNoiseSpec  = gpAudNrIrisNoiseSpec;

            if(gAudNrIrisDelayFrame >= AUDNR_DELAYFRAME)
            {
                //DBG_IND("IRIS Lens state is Idle!\r\n");
                AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                     FALSE);
            }
            else
            {
                //DBG_IND("IRIS Lens state is not Idle!\r\n");
                gAudNrIrisDelayFrame++;
                AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_ENABLE,                     TRUE);
            }
        }
        break;
    }

    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_STOP_FREQUENCY,         uiNrStopFreq);
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_REPLACE_FREQUENCY,   uiNrReplaceFreq);
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_MIX_BACKGROUND,       uiNrMixBackgrd);
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_DECREASE_LVL,        uiNrDecreaseLvl);

    // Noise Pattern Spec.
    AudNoiseReduce_SetupParams(AUDNR_CONFIG_ID_NOISE_SPEC,             (UINT32)pNoiseSpec);

}

//-------------------------------------------------------------------------------------------------
//
// API function for Lens state updating
//
//-------------------------------------------------------------------------------------------------
void UIMovie_UpdateLensState(UINT32 LensState)
{
    gUIMovieLensState = LensState;
}

//-------------------------------------------------------------------------------------------------
//
// Callback function for the events which should be processed immediately (cannot use post event)
//
//-------------------------------------------------------------------------------------------------
void UIMovie_3DNRSetting(void)
{
    UINT32  ui3DNRLevel;

    if (UI_GetData(FL_MovieMCTFIndex) == MOVIE_MCTF_OFF)
    {
        ui3DNRLevel = H264_3DNR_DISABLE;
    }
    else
    {
        ui3DNRLevel = UIMovie_3DNRAdjust();
    }

    if (UI_GetData(FL_MOVIE_DUAL_REC))
    {
        // note: 3DNR of video path 1 and path 2 can be different
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H2643DNRLEVEL, ui3DNRLevel, 0, 0); // video path 1
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H2643DNRLEVEL, ui3DNRLevel, 1, 0); // video path 2
    }
    else
    {
        MediaRec_ChangeParameter(MEDIAREC_RECPARAM_H2643DNRLEVEL, ui3DNRLevel, 0, 0);
    }
}

void UIMovie_ImmProc(UINT32 type, UINT32 p1, UINT32 p2, UINT32 p3)
{
    switch (type)
    {
    case MEDIAREC_IMMPROC_CUTFILE_END:
        #if (UI_STYLE==UI_STYLE_DRIVER)
        // update DCF Folder/File name
        FlowDCF_UpdateName();
        if(UIFlowWndMovie_IsGsensorTrig()==TRUE)
        {
		UIFlowWndMovie_SetGsensorTrigFlag(FALSE);     
                GSensor_GMA301_ClearCrashMode();	     
			 	 
	  	FlowMovie_IconHideLockFile();
        }
        #endif

        // [TO DO]: log GPS data
        DBG_IND("EVENT %d: Cut file end!\r\n", type);
        break;

    case MEDIAREC_IMMPROC_DELETE_FILE:
        // [TO DO]: delete related files, such as GPS data file
        DBG_IND("EVENT %d: Delete file!\r\n", type);
        break;

    // special dynamic encoding parameters setting, such as H.264 3DNR
    case MEDIAREC_IMMPROC_ENC_SETTING:
        UIMovie_3DNRSetting();
        break;

    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------
//
// Callback function for AF events
//
//-------------------------------------------------------------------------------------------------
void UIMovie_AFCB(AF_CBMSG uiMsgID, void *Data)
{
    UINT32 uiData;

    switch (uiMsgID)
    {
        case AF_CBMSG_AF_START:
            uiData = *(UINT32 *)Data;
            DBG_IND("===AF Start! - %d===\r\n", uiData);
            break;
        case AF_CBMSG_AF_DONE:
            uiData = *(UINT32 *)Data;
            DBG_IND("===AF Done! - %d===\r\n", uiData);
            AE_Lock(AE_ID_1,FALSE,FALSE);
            AWB_Lock(AWB_ID_1,FALSE,FALSE);
            Ux_PostEvent(NVTEVT_ALGMSG_FOCUSEND,1, NVTEVT_ALGMSG_FOCUSEND);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//
// Callback function for LDWS events
//
//-------------------------------------------------------------------------------------------------
void UIMovie_LDWSCB(LDWS_CBMSG uiMsgID, void *Data)
{
    LDWS_INFO uiData;

    switch (uiMsgID)
    {
        case LDWS_CBMSG_WARNNING:
            uiData = *(LDWS_INFO *)Data;
            DBG_IND("===UIMovie_LDWSCB ldws_warning=%d ===\r\n",uiData.ldws_warning);
            break;
        default:
            break;
    }



}

