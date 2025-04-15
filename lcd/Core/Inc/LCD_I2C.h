#ifndef __LCD_I2C_H__
#define __LCD_I2C_H__

#include "stm32l1xx_hal.h"  // Adapté à ta carte, modifie si nécessaire
#include <stdint.h>

#define LCD_ADDR (0x3E << 1)
#define RGB_ADDR (0x60 << 1)

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_SetText(I2C_HandleTypeDef *hi2c, const char *text, uint8_t line);
void LCD_SetRGB(I2C_HandleTypeDef *hi2c, uint8_t r, uint8_t g, uint8_t b);

#endif
