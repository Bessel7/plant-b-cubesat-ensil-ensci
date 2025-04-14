#include "BMP280.h"
#include <math.h>
#include <string.h>

// Adresse I2C 8 bits
#define BMP280_I2C_ADDR BMP280_ADDR

// Variables globales
static I2C_HandleTypeDef *bmp_hi2c;

static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

static int32_t t_fine;

// Lecture des registres de calibration
static void BMP280_ReadCalibrationData(void) {
    uint8_t calib[24];
    HAL_I2C_Mem_Read(bmp_hi2c, BMP280_I2C_ADDR, 0x88, I2C_MEMADD_SIZE_8BIT, calib, 24, HAL_MAX_DELAY);

    dig_T1 = (uint16_t)(calib[1] << 8 | calib[0]);
    dig_T2 = (int16_t)(calib[3] << 8 | calib[2]);
    dig_T3 = (int16_t)(calib[5] << 8 | calib[4]);

    dig_P1 = (uint16_t)(calib[7] << 8 | calib[6]);
    dig_P2 = (int16_t)(calib[9] << 8 | calib[8]);
    dig_P3 = (int16_t)(calib[11] << 8 | calib[10]);
    dig_P4 = (int16_t)(calib[13] << 8 | calib[12]);
    dig_P5 = (int16_t)(calib[15] << 8 | calib[14]);
    dig_P6 = (int16_t)(calib[17] << 8 | calib[16]);
    dig_P7 = (int16_t)(calib[19] << 8 | calib[18]);
    dig_P8 = (int16_t)(calib[21] << 8 | calib[20]);
    dig_P9 = (int16_t)(calib[23] << 8 | calib[22]);
}

void BMP280_Init(I2C_HandleTypeDef *hi2c) {
    bmp_hi2c = hi2c;

    // Configuration : normal mode, oversampling x1
    uint8_t ctrl_meas[2] = {0xF4, 0x27}; // Temp x1, Press x1, mode normal
    uint8_t config[2] = {0xF5, 0xA0};    // Standby 1000ms, filter off

    HAL_I2C_Mem_Write(bmp_hi2c, BMP280_I2C_ADDR, ctrl_meas[0], I2C_MEMADD_SIZE_8BIT, &ctrl_meas[1], 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Write(bmp_hi2c, BMP280_I2C_ADDR, config[0], I2C_MEMADD_SIZE_8BIT, &config[1], 1, HAL_MAX_DELAY);

    BMP280_ReadCalibrationData();
}

float BMP280_ReadTemperature(void) {
    uint8_t data[3];
    HAL_I2C_Mem_Read(bmp_hi2c, BMP280_I2C_ADDR, 0xFA, I2C_MEMADD_SIZE_8BIT, data, 3, HAL_MAX_DELAY);
    int32_t adc_T = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4));

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
                    ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T = (t_fine * 5 + 128) >> 8;
    return T / 100.0f;
}

float BMP280_ReadPressure(void) {
    uint8_t data[3];
    HAL_I2C_Mem_Read(bmp_hi2c, BMP280_I2C_ADDR, 0xF7, I2C_MEMADD_SIZE_8BIT, data, 3, HAL_MAX_DELAY);
    int32_t adc_P = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4));

    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

    if (var1 == 0) return 0; // avoid division by zero

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (float)p / 256.0f; // En Pa
}

float BMP280_ReadAltitude(float seaLevelhPa) {
    float pressure = BMP280_ReadPressure() / 100.0f; // Convert to hPa
    return 44330.0f * (1.0f - powf(pressure / seaLevelhPa, 0.1903f));
}
