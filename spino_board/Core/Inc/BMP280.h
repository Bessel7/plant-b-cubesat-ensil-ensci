#ifndef BMP280_H
#define BMP280_H

#include "stm32l4xx_hal.h"

// Adresse I2C 8 bits (0x77 << 1 = 0xEE)
#define BMP280_ADDR (0x77 << 1)

void BMP280_Init(I2C_HandleTypeDef *hi2c);
float BMP280_ReadTemperature(void);
float BMP280_ReadPressure(void);       // Renvoie la pression en Pascal (Pa)
float BMP280_ReadAltitude(float seaLevelhPa);  // seaLevelhPa typiquement 1013.25

#endif // BMP280_H
