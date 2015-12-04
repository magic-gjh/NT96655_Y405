#ifndef _UIAPPNET_H_
#define _UIAPPNET_H_


#define TXT_WIFI_AP_MODE  "AP mode"
#define TXT_WIFI_ST_MODE  "station mode"
#define TXT_WIFI_CONNECT  "Conntecting"
#if (_NETWORK_ == _NETWORK_SDIO_EVB_WIFI_)
#define TXT_WIFI          "wifi"
#define TXT_WIFI_SET      "wifi Setting"
#define TXT_WIFI_START    "wifi start"
#else
#define TXT_WIFI          "LAN"
#define TXT_WIFI_SET      "LAN Setting"
#define TXT_WIFI_START    "LAN start"

#endif


extern INT32 UINet_WifiInit(UINT32 mode);
extern INT32 UINet_LanInit(UINT32 mode);
extern INT32 UINet_HFSInit(void);
extern INT32 UINet_WifiUnInit(UINT32 mode);
extern INT32 UINet_LanUnInit(UINT32 mode);
extern INT32 UINet_HFSUnInit(void);
extern char* UINet_GetSSID(void);
extern char* UINet_GetPASSPHRASE(void);

#endif //_UIAPPNET_H_
