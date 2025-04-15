#include "VEML7700.h"

static void VEML7700_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t value) {
    uint8_t data[3] = {reg, value & 0xFF, (value >> 8) & 0xFF};
    HAL_I2C_Master_Transmit(hi2c, VEML7700_ADDR, data, 3, HAL_MAX_DELAY);
}

static uint16_t VEML7700_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg) {
    uint8_t data[2];
    HAL_I2C_Mem_Read(hi2c, VEML7700_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
    return (data[1] << 8) | data[0];
}

void VEML7700_Init(I2C_HandleTypeDef *hi2c) {
    VEML7700_WriteRegister(hi2c, 0x00, 0x0000);
    HAL_Delay(10);
}

float VEML7700_GetLux(I2C_HandleTypeDef *hi2c) {
    uint16_t raw = VEML7700_ReadRegister(hi2c, 0x04);
    return raw * 0.0036f;
}
