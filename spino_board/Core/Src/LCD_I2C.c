#include "LCD_I2C.h"
#include <string.h>

void LCD_Command(I2C_HandleTypeDef *hi2c, uint8_t cmd) {
    uint8_t data[2] = {0x80, cmd};
    HAL_I2C_Master_Transmit(hi2c, LCD_ADDR, data, 2, HAL_MAX_DELAY);
}

void LCD_Write(I2C_HandleTypeDef *hi2c, uint8_t data_byte) {
    uint8_t data[2] = {0x40, data_byte};
    HAL_I2C_Master_Transmit(hi2c, LCD_ADDR, data, 2, HAL_MAX_DELAY);
}

void LCD_SetRGB(I2C_HandleTypeDef *hi2c, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t data[][2] = {
        {0x00, 0x00},
        {0x01, 0x00},
        {0x08, 0xAA},
        {0x04, r},
        {0x03, g},
        {0x02, b}
    };
    for (int i = 0; i < 6; i++) {
        HAL_I2C_Mem_Write(hi2c, RGB_ADDR, data[i][0], I2C_MEMADD_SIZE_8BIT, &data[i][1], 1, HAL_MAX_DELAY);
    }
}

void LCD_SetText(I2C_HandleTypeDef *hi2c, const char *text, uint8_t line) {
    if (line == 0) {
        LCD_Command(hi2c, 0x80);
    } else if (line == 1) {
        LCD_Command(hi2c, 0xC0);
    }

    for (int i = 0; i < 16 && text[i] != '\0'; i++) {
        LCD_Write(hi2c, text[i]);
    }
}

void LCD_Init(I2C_HandleTypeDef *hi2c) {
    HAL_Delay(50);

    LCD_Command(hi2c, 0x38); // Function set: 8-bit, 2 lines, 5x8 dots
    HAL_Delay(1);
    LCD_Command(hi2c, 0x39); // Function set: extension command set
    HAL_Delay(1);
    LCD_Command(hi2c, 0x14); // Internal OSC frequency
    HAL_Delay(1);
    LCD_Command(hi2c, 0x70); // Contrast set (low byte)
    HAL_Delay(1);
    LCD_Command(hi2c, 0x56); // Power/icon/contrast (high byte)
    HAL_Delay(1);
    LCD_Command(hi2c, 0x6C); // Follower control (internal power)
    HAL_Delay(200);

    LCD_Command(hi2c, 0x38); // Function set
    LCD_Command(hi2c, 0x0C); // Display ON, cursor OFF, blink OFF
    LCD_Command(hi2c, 0x01); // Clear display
    HAL_Delay(2);
}

