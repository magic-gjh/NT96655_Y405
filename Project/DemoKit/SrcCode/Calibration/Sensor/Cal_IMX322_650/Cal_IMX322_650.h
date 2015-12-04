/** \addtogroup mIPRJAPTest */
//@{

#ifndef _CALIBRATIONITEM_H
#define _CALIBRATIONITEM_H

#include "Type.h"

extern ER Cal_PS2MemPool(void);
extern UINT32 Cal_WiteDebug_Info(UINT32 Addr);
extern ER     Cal_SenDP(void);
extern ER     Cal_SenDP_Bright(void);
extern ER     Cal_SenDP_Dark(void);
extern void     Cal_SenAE_Check(void);
extern ER     Cal_SenISO(void);
extern ER     Cal_SenMShutter(void);
extern ER Cal_SenAWB(void);
extern ER Cal_SenAWBGS(void);
extern ER     Cal_SenECS(void);
extern void     Cal_SenDNP(void);
extern ER     Cal_Get_DNP_Status(void);
extern ER Cal_IplClose(void);
extern ER Cal_IplOpen(UINT32 BufAddr, UINT32 BufSize);
extern SX_CMD_ENTRY* UiCalTest_GetCmdEntry(void);
extern void CalTest_InstallCmd(void);
#endif

//@}
