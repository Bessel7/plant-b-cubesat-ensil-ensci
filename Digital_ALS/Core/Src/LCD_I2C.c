#include "LCD_I2C.h"
#include "stdio.h"
#include "string.h"

#define LCD_ADDR (0x27 << 1)
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_COMMAND 0
#define LCD_DATA 1

extern I2C_HandleTypeDef hi2c1;

static void LCD_SendInternal(uint8_t data, uint8_t flags) {
    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;
    uint8_t data_arr[4];
    data_arr[0] = up | flags | LCD_BACKLIGHT | LCD_ENABLE;
    data_arr[1] = up | flags | LCD_BACKLIGHT;
    data_arr[2] = lo | flags | LCD_BACKLIGHT | LCD_ENABLE;
    data_arr[3] = lo | flags | LCD_BACKLIGHT;
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
}

static void LCD_SendCommand(uint8_t cmd) {
    LCD_SendInternal(cmd, LCD_COMMAND);
}

static void LCD_SendData(uint8_t data) {
    LCD_SendInternal(data, LCD_DATA);
}

void LCD_Init(void) {
    HAL_Delay(50);
    LCD_SendCommand(0x30);
    HAL_Delay(5);
    LCD_SendCommand(0x30);
    HAL_Delay(1);
    LCD_SendCommand(0x30);
    HAL_Delay(10);
    LCD_SendCommand(0x20);
    HAL_Delay(10);
    LCD_SendCommand(0x28);
    LCD_SendCommand(0x08);
    LCD_SendCommand(0x01);
    HAL_Delay(2);
    LCD_SendCommand(0x06);
    LCD_SendCommand(0x0C);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    LCD_SendCommand(0x80 | (col + row_offsets[row]));
}

void LCD_Print(char *str) {
    while (*str) {
        LCD_SendData((uint8_t)(*str));
        str++;
    }
}
