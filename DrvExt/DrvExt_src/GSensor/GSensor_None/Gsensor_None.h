
#ifndef _GSENSOR_NONE_H
#define _GSENSOR_NONE_H
#include "Type.h"
#include "IOInit.h"
#include "pad.h"
#include "top.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"

extern BOOL GSensor_NONE_open(void);
extern BOOL GSensor_NONE_close(void);
extern BOOL GSensor_NONE_GetStatus(Gsensor_Data *GS_Data);
extern BOOL GSensor_NONE_ParkingMode(void);
extern BOOL GSensor_NONE_CrashMode(void);
extern void GSensor_NONE_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity);

#endif
