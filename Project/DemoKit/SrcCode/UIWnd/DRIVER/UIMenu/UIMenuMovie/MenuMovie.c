#include <stdio.h>
#include "UIFlow.h"

// --------------------------------------------------------------------------
// OPTIONS
// --------------------------------------------------------------------------
#if (_MULTI_RECORD_FUNC_)
// Dual Recording Optiosn
TMDEF_BEGIN_OPTIONS(MOVIE_DUAL_REC)
    TMDEF_OPTION_TEXT(MOVIE_DUAL_REC_OFF)
    TMDEF_OPTION_TEXT(MOVIE_DUAL_REC_ON)
TMDEF_END_OPTIONS()
#endif

// Movie Size Options
#if (UPDATE_CFG == UPDATE_CFG_YES)
TM_OPTION gTM_OPTIONS_MOVIE_SIZE[MOVIE_SIZE_ID_MAX] = {0};
#else
TMDEF_BEGIN_OPTIONS(MOVIE_SIZE)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_1080FHD)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_720P)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_WVGA)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_VGA)
TMDEF_END_OPTIONS()
#endif

TMDEF_BEGIN_OPTIONS(MOVIE_SIZE_DUAL)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_1080P_720P)
    TMDEF_OPTION_TEXT(MOVIE_SIZE_720P_720P)
TMDEF_END_OPTIONS()

// Cyclic recroding Options
TMDEF_BEGIN_OPTIONS(MOVIE_CYCLIC_REC)
    TMDEF_OPTION_TEXT(MOVIE_CYCLIC_REC_OFF)
    TMDEF_OPTION_TEXT(MOVIE_CYCLIC_REC_3MIN)
    TMDEF_OPTION_TEXT(MOVIE_CYCLIC_REC_5MIN)
    TMDEF_OPTION_TEXT(MOVIE_CYCLIC_REC_10MIN)
TMDEF_END_OPTIONS()

// Time lapse recoring Options
TMDEF_BEGIN_OPTIONS(MOVIE_TIMELAPSE_REC)
    TMDEF_OPTION_TEXT(MOVIE_TIMELAPSE_REC_OFF)
    TMDEF_OPTION_TEXT(MOVIE_TIMELAPSE_REC_100MS)
    TMDEF_OPTION_TEXT(MOVIE_TIMELAPSE_REC_200MS)
    TMDEF_OPTION_TEXT(MOVIE_TIMELAPSE_REC_500MS)
TMDEF_END_OPTIONS()
TMDEF_BEGIN_OPTIONS(MOVIE_MOTION_DET)
    TMDEF_OPTION_TEXT(MOVIE_MOTION_DET_OFF)
    TMDEF_OPTION_TEXT(MOVIE_MOTION_DET_ON)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(MOVIE_HDR)
    TMDEF_OPTION_TEXT(MOVIE_HDR_OFF)
    TMDEF_OPTION_TEXT(MOVIE_HDR_ON)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(MOVIE_AUDIO)
    TMDEF_OPTION_TEXT(MOVIE_AUDIO_OFF)
    TMDEF_OPTION_TEXT(MOVIE_AUDIO_ON)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(MOVIE_DATEIMPRINT)
    TMDEF_OPTION_TEXT(MOVIE_DATEIMPRINT_OFF)
    TMDEF_OPTION_TEXT(MOVIE_DATEIMPRINT_ON)
TMDEF_END_OPTIONS()

// Car num
TMDEF_BEGIN_OPTIONS(CAR_NUM)
    TMDEF_OPTION_TEXT(CAR_NUM_OFF)
    TMDEF_OPTION_TEXT(CAR_NUM_ON)
TMDEF_END_OPTIONS()

TMDEF_BEGIN_OPTIONS(MOVIE_GSENSOR)
    TMDEF_OPTION_TEXT(MOVIE_GSENSOR_OFF)
    TMDEF_OPTION_TEXT(MOVIE_GSENSOR_LOW)
    TMDEF_OPTION_TEXT(MOVIE_GSENSOR_MED)
    TMDEF_OPTION_TEXT(MOVIE_GSENSOR_HIGH)
TMDEF_END_OPTIONS()
// GPS Options
TMDEF_BEGIN_OPTIONS(MOVIE_GPS)
    TMDEF_OPTION_TEXT(MOVIE_GPS_OFF)
    TMDEF_OPTION_TEXT(MOVIE_GPS_ON)
TMDEF_END_OPTIONS()

// --------------------------------------------------------------------------
// OPTIONS
// --------------------------------------------------------------------------
// Monitor Options
TMDEF_BEGIN_OPTIONS(MOVIE_PARKING)
    TMDEF_OPTION_TEXT(MOVIE_PARKING_OFF)
    TMDEF_OPTION_TEXT(MOVIE_PARKING_ON)
TMDEF_END_OPTIONS()


TMDEF_BEGIN_OPTIONS(MOVIE_YESHI_DELAY)
    TMDEF_OPTION_TEXT(MOVIE_YESHI_DELAY_OFF)
    TMDEF_OPTION_TEXT(MOVIE_YESHI_DELAY_5SEC)
    TMDEF_OPTION_TEXT(MOVIE_YESHI_DELAY_10SEC)
    TMDEF_OPTION_TEXT(MOVIE_YESHI_DELAY_15SEC)
    TMDEF_OPTION_TEXT(MOVIE_YESHI_DELAY_20SEC)
TMDEF_END_OPTIONS()
// --------------------------------------------------------------------------
// ITEMS
// --------------------------------------------------------------------------
// Movie Menu Items
TMDEF_BEGIN_ITEMS(MOVIE)
    TMDEF_ITEM_TEXTID(MOVIE_SIZE)
#if (_MULTI_RECORD_FUNC_)
    TMDEF_ITEM_TEXTID(MOVIE_DUAL_REC)
#endif
    TMDEF_ITEM_TEXTID(MOVIE_CYCLIC_REC)
    TMDEF_ITEM_TEXTID(MOVIE_TIMELAPSE_REC)
    TMDEF_ITEM_TEXTID(MOVIE_HDR)
    //TMDEF_ITEM_TEXTID(MOVIE_EV)
    TMDEF_ITEM_TEXTID(EV)
    TMDEF_ITEM_TEXTID(MOVIE_MOTION_DET)
    TMDEF_ITEM_TEXTID(MOVIE_AUDIO)
    TMDEF_ITEM_TEXTID(MOVIE_DATEIMPRINT)
    TMDEF_ITEM_TEXTID(CAR_NUM)        
    TMDEF_ITEM_TEXTID(MOVIE_GSENSOR)
    TMDEF_ITEM_TEXTID(MOVIE_PARKING)    
    #if(GPS_FUNCTION == ENABLE)    
    TMDEF_ITEM_TEXTID(MOVIE_GPS)
#endif 
    TMDEF_ITEM_TEXTID(MOVIE_YESHI_DELAY)
TMDEF_END_ITEMS()

// Movie Size Menu Items (for single/dual rec menu item data copy)
TMDEF_BEGIN_ITEMS(MOVIE_SIZE)
    TMDEF_ITEM_TEXTID(MOVIE_SIZE)
    TMDEF_ITEM_TEXTID(MOVIE_SIZE_DUAL)
TMDEF_END_ITEMS()

// --------------------------------------------------------------------------
// PAGES
// --------------------------------------------------------------------------
// Movie Menu Pages
TMDEF_BEGIN_PAGES(MOVIE)
    TMDEF_PAGE_TEXT_ICON(MOVIE)
    TMDEF_PAGE_TEXT_ICON(SETUP)
TMDEF_END_PAGES()

TMDEF_EMNU(gMovieMenu, MOVIE, Movie_MenuCallback)

// --------------------------------------------------------------------------
// Menu Callback
// --------------------------------------------------------------------------
void Movie_MenuSizeConfig(void)
{
#if (_MULTI_RECORD_FUNC_)
    UINT32 i = SysGetFlag(FL_MOVIE_DUAL_REC) ? 1 : 0;

    // copy item "MOVIE_SIZE" or "MOVIE_SIZE_DUAL" to movie menu
    memcpy((void *)&gTM_ITEMS_MOVIE[0], (void *)&gTM_ITEMS_MOVIE_SIZE[i], sizeof(TM_ITEM));
#endif
}

int Movie_MenuCallback(UINT32 uiMessage, UINT32 uiParam)
{
    UINT16  uwItemId;
    UINT16  uwOption;

    if (uiMessage == TMM_CONFIRM_OPTION)
    {
        uwItemId = LO_WORD(uiParam);
        uwOption = HI_WORD(uiParam);

        switch (uwItemId)
        {
        case IDM_MOVIE_SIZE:
        case IDM_MOVIE_SIZE_DUAL:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIESIZE, 1, uwOption);
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_IMAGE_RATIO, 1, uwOption);
            break;

        case IDM_EV:
            Ux_SendEvent(&CustomPhotoObjCtrl, NVTEVT_EXE_EV, 1, uwOption);
            break;

        case IDM_MOVIE_HDR:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_HDR, 1, uwOption);
            break;

#if (_MULTI_RECORD_FUNC_)
        case IDM_MOVIE_DUAL_REC:
            {
                UINT32 uiSelect = (uwOption == 0) ? FALSE : TRUE; // according to option order
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_DUAL_REC, 1, uiSelect);
                Movie_MenuSizeConfig();
            }
            {
                // set image ratio
                UINT32 uiSize;
                uiSize = SysGetFlag(FL_MOVIE_DUAL_REC) ? SysGetFlag(FL_MOVIE_SIZE_DUAL) : SysGetFlag(FL_MOVIE_SIZE);
                Ux_SendEvent(&CustomMovieObjCtrl,NVTEVT_EXE_MOVIESIZE, 1, uiSize);
                Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_IMAGE_RATIO, 1, uiSize);
            }
            break;
#endif

        case IDM_MOVIE_CYCLIC_REC:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_CYCLIC_REC, 1, uwOption);
            break;

        case IDM_MOVIE_TIMELAPSE_REC:
            SysSetFlag(FL_MOVIE_TIMELAPSE_REC,uwOption);
            break;

        case IDM_MOVIE_MOTION_DET:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOTION_DET, 1, uwOption);
            break;

        case IDM_MOVIE_AUDIO:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_AUDIO, 1, uwOption);
            break;
        #if(GPS_FUNCTION == ENABLE)    
        case IDM_MOVIE_GPS:
            
            UI_SetData(FL_MOVIE_GPS, uwOption);
            //Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEGPS, 1, uwOption);
            break;
#endif	
        case IDM_MOVIE_DATEIMPRINT:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIE_DATE_IMPRINT, 1, uwOption);
            break;
			
	  case IDM_CAR_NUM:
	  	if(uwOption==1)
	  	{
	            if (SysGetFlag(FL_LANGUAGE) == LANG_SC) 
	            {
	                Ux_OpenWindow(&UIMenuWndSetupCarNumberCtrl, 1, 0);
	            }
	            else
	            {
	                Ux_OpenWindow(&UIMenuWndSetupCommonCarNumberCtrl, 1, 0);
	            }		
		}
	  	break;
		
        case IDM_MOVIE_YESHI_DELAY:
            SysSetFlag(FL_MOVIE_YESHI_DELAY,uwOption);
            break;
        case IDM_MOVIE_GSENSOR:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_GSENSOR,1,uwOption);
            break;
			
        case IDM_MOVIE_PARKING:
            Ux_SendEvent(&CustomMovieObjCtrl, NVTEVT_EXE_MOVIEPARKING, 1, uwOption);
            break;
			
        default:
            Setup_MenuCallback(uiMessage, uiParam);
            break;
        }
    }
    else if (uiMessage == TMM_ENTER_ITEM)
    {
        Movie_MenuSizeConfig();
    }

    return TMF_PROCESSED;
}
