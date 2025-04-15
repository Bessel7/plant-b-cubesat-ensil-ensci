#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32l1xx_hal.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(char *str);

#endif // LCD_I2C_H
