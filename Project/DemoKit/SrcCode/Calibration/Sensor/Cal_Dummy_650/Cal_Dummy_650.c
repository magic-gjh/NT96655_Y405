#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    "kernel.h"
#include    "sensor.h"
#include    "debug.h"
#include    "utility.h"
#include    "CalibrationInt.h"
#include    "Cal_Dummy_650.h"
#include    "Syscfg.h"
#include    "UIFlow.h"
#include    "Phototask.h"

ER Cal_PS2MemPool(void)
{
    return E_OK;
}

UINT32 Cal_WiteDebug_Info(UINT32 Addr)
{
    return Addr;
}


ER Cal_SenISO(void)
{
    return E_OK;
}

ER Cal_SenMShutter(void)
{
    return E_OK;
}

ER Cal_SenDP(void)
{
    return E_OK;
}


ER Cal_SenDP_Bright(void)
{
    return E_OK;
}

ER Cal_SenDP_Dark(void)
{
    return E_OK;
}


ER Cal_SenAWB(void)
{
    return E_OK;
}

ER Cal_SenAWBGS(void)
{
    return E_OK;
}

ER Cal_SenECS(void)
{
    return E_OK;
}

ER Cal_Get_DNP_Status(void)
{
    return E_OK;
}

