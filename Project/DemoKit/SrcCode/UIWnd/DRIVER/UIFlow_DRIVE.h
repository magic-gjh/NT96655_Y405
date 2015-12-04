#ifndef _UIFLOW_TURNKEY_H
#define _UIFLOW_TURNKEY_H

//UI Display
#include "Resource/D320x240_DRIVER/BGDB/BG_Images.h"
#include "Resource/D320x240_DRIVER/BGDB/BG_Opening.h"
#include "Resource/D320x240_DRIVER/BGDB/screen_160x120.h"

//UI Info
#include "UIPlayObj.h"
#include "UISetup.h"
#include "UIStorageCheck.h"
#include "UICommon.h"
#include "SoundData.h"

#include "UIInfo/UIInfo.h"
//#include "DRIVER/UIInfo/UICfgDefault.h"
//#include "DRIVER/UIInfo/UISystemStatusSettings.h"
//#include "DRIVER/UIInfo/UIMenuPrintSettings.h"


//UIFlow common
#include "UIFlow/UIFlowCommon/UIFlowWndWrnMsg.h"
#include "UIFlow/UIFlowCommon/UIFlowWndWrnMsgAPI.h"
#include "UIFlow/UIFlowCommon/UIFlowWndWaitMoment.h"
#include "UIFlow/UIFlowCommon/UIFlowWndProcessing.h"
#include "UIFlow/UIFlowCommon/UIFlowWndUSB.h"
#include "UIFlow/UIFlowCommon/UIFlowWndUSBAPI.h"


#if 0
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowCommonIcons.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndFwVersion.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndUSB.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndUSBAPI.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndWaitMoment.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndWaitMomentAPI.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndWrnMsg.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndWrnMsgAPI.h"
#include "DRIVE/UIFlow/UIFlowCommon/UIFlowWndWrnStrgHotPlug.h"
#endif

//UIFlow movie
#include "UIFlow/UIFlowMovie/UIFlowMovieFuncs.h"
#include "UIFlow/UIFlowMovie/UIFlowMovieIcons.h"
#include "UIFlow/UIFlowMovie/UIFlowWndMovie.h"


//UIFlow photo
#include "UIFlow/UIFlowPhoto/UIFlowPhotoFuncs.h"
#include "UIFlow/UIFlowPhoto/UIFlowPhotoIcons.h"
#include "UIFlow/UIFlowPhoto/UIFlowWndPhoto.h"

//UIFlow play
#include "UIFlow/UIFlowPlay/UIFlowWndPlay.h"
//#include "DRIVER/UIFlow/UIFlowPlay/UIFlowWndPlayNoFile.h"
#include "UIFlow/UIFlowPlay/UIFlowPlayFuncs.h"
#include "UIFlow/UIFlowPlay/UIFlowPlayIcons.h"
//#include "DRIVER/UIFlow/UIFlowPlay/UIFlowWndPlayMagnify.h"
#include "UIFlow/UIFlowPlay/UIFlowWndPlayThumb.h"

//UIFlow printer
//#include "DRIVE/UIFlow/UIFlowPrint/UIFlowWndPrint.h"

//UIMenu Common
#include "UIMenu/UIMenuCommon/TabMenu.h"
#include "UIMenu/UIMenuCommon/MenuId.h"
#include "UIMenu/UIMenuCommon/MenuCommonItem.h"   //Item
#include "UIMenu/UIMenuCommon/MenuCommonOption.h" //Option

//UIMenu calibration
#include "UIMenu/UIMenuCalibration/UIMenuWndCalibration.h"

//UIMenu movie
#include "UIMenu/UIMenuMovie/MenuMovie.h"

//UIMenu photo
#include "UIMenu/UIMenuPhoto/MenuPhoto.h"
#include "UIMenu/UIMenuPhoto/UIMenuWndPhotoColor.h"
#include "UIMenu/UIMenuPhoto/UIMenuWndPhotoExposure.h"
#include "UIMenu/UIMenuPhoto/UIMenuWndPhotoQuickSetting.h"
#include "UIMenu/UIMenuPhoto/UIMenuWndPhotoWB.h"


//UIMenu playback
#include "UIMenu/UIMenuPlay/MenuPlayback.h"
#include "UIMenu/UIMenuPlay/UIMenuWndPlaySlideShow.h"
#include "UIMenu/UIMenuPlay/UIMenuWndPlaySlideShowCB.h"
#include "UIMenu/UIMenuPlay/UIMenuWndPlayDel.h"
#include "UIMenu/UIMenuPlay/UIMenuWndPlayConfirmDel.h"

#if 0
#include "DRIVE/UIMenu/UIMenuPlay/MenuPlayback.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayConfirmDel.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayCopyToCard.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayDel.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayDPOF.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayProtect.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayQuickConfirmDel.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlayQuickDel.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlaySetDPOF.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlaySlideShow.h"
#include "DRIVE/UIMenu/UIMenuPlay/UIMenuWndPlaySlideShowCB.h"
#endif

//UIMenu printer
#if 0
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_Confirm.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_Crop.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_PrintAll.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_PrintDPOF.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_PrintProgress.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_PrintSetting.h"
#include "DRIVE/UIMenu/UIMenuPrint/UIMenuWndPrint_SelectImages.h"
#endif

//UIMenu Setup
#include "UIMenu/UIMenuSetup/MenuSetup.h"
#include "UIMenu/UIMenuSetup/MenuSetup.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupDateTime.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupDefaultSetting.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupFormat.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupFormatConfirm.h"
//#include "DRIVE/UIMenu/UIMenuSetup/UIMenuWndSetupLoaderVersion.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupVersion.h"

#include "UIMenu/UIMenuSetup/UIMenuWndSetupCarNumber.h"
#include "UIMenu/UIMenuSetup/UIMenuWndSetupCommonCarNumber.h"

//UIMenu USB
//#include "DRIVE/UIMenu/UIMenuUSB/UIMenuWndUSB.h"



#endif
