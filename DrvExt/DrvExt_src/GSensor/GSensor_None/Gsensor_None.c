#include "GSensor.h"
#include "GSensor_None.h"

static GSENSOR_OBJ g_GsensorNoneObj = {GSensor_NONE_open, GSensor_NONE_close, GSensor_NONE_GetStatus, GSensor_NONE_ParkingMode, GSensor_NONE_CrashMode, GSensor_NONE_SetSensitivity};

PGSENSOR_OBJ GSensor_NONE_getGSensorObj(void)
{
    return &g_GsensorNoneObj;
}

BOOL GSensor_NONE_open(void)
{
    return FALSE;
}

BOOL GSensor_NONE_close(void)
{
    return TRUE;
}

BOOL GSensor_NONE_GetStatus(Gsensor_Data *GS_Data)
{
    return FALSE;
}

BOOL GSensor_NONE_ParkingMode(void)
{
    return FALSE;
}

BOOL GSensor_NONE_CrashMode(void)
{
    return FALSE;
}

void GSensor_NONE_SetSensitivity(GSENSOR_SENSITIVITY GSensorSensitivity)
{

}

