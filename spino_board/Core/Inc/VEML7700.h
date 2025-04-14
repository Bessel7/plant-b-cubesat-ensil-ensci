#ifndef VEML7700_H
#define VEML7700_H

#include "stm32l4xx_hal.h"

#define VEML7700_ADDR (0x10 << 1)

void VEML7700_Init(I2C_HandleTypeDef *hi2c);
float VEML7700_GetLux(I2C_HandleTypeDef *hi2c);

#endif // VEML7700_H
