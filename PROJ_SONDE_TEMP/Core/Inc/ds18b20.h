#ifndef DS18B20_H
#define DS18B20_H

#include "onewire.h"

void DS18B20_Start(void);
int16_t DS18B20_ReadTemperature(void);

#endif
