
#ifndef _GSENSOR_INT_H
#define _GSENSOR_INT_H

#include "Type.h"
#include "i2c.h"

extern I2C_STS GSensor_I2C_Receive(UINT32 *value, BOOL bNACK, BOOL bStop);
extern I2C_STS GSensor_I2C_Transmit(UINT32 value, BOOL bStart, BOOL bStop);

#endif
