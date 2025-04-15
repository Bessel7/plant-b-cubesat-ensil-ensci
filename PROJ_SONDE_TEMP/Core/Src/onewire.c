/*
 * onewire.c
 *
 *  Created on: Mar 13, 2025
 *      Author: nour
 */
#include "onewire.h"
#include "main.h" // Pour delay_us()

void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);
}

uint8_t Onewire_Reset(void) {
    uint8_t response = 1;
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(480);
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    delay_us(70);

    if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN) == GPIO_PIN_RESET) {
        response = 0;
    }

    delay_us(410);
    return response;
}

void Onewire_WriteBit(uint8_t bit) {
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(bit ? 10 : 60);
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    delay_us(bit ? 55 : 5);
}

uint8_t Onewire_ReadBit(void) {
    uint8_t bit = 0;
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(3);
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    delay_us(10);
    bit = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
    delay_us(50);
    return bit;
}

void Onewire_WriteByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        Onewire_WriteBit(byte & (1 << i));
    }
}

uint8_t Onewire_ReadByte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (Onewire_ReadBit()) {
            byte |= (1 << i);
        }
    }
    return byte;
}


