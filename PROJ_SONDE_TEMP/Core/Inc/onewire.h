#ifndef ONEWIRE_H
#define ONEWIRE_H
#include "stm32f4xx_hal.h"

#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_1
extern TIM_HandleTypeDef htim3;
void Onewire_Init(void);
uint8_t Onewire_Reset(void);
void Onewire_WriteBit(uint8_t bit);
uint8_t Onewire_ReadBit(void);
void Onewire_WriteByte(uint8_t byte);
uint8_t Onewire_ReadByte(void);

#endif
