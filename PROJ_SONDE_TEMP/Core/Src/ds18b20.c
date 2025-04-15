/*
 * ds18b20.c
 *
 *  Created on: Mar 13, 2025
 *      Author: nour
 */
#include "ds18b20.h"

void DS18B20_Start(void) {
    Onewire_Reset();
    Onewire_WriteByte(0xCC); // Skip ROM
    Onewire_WriteByte(0x44); // Start temperature conversion
}

int16_t DS18B20_ReadTemperature(void) {
    uint16_t temp = 0;

    Onewire_Reset();
    Onewire_WriteByte(0xCC);  // Skip ROM
    Onewire_WriteByte(0xBE);  // Read Scratchpad

    temp = Onewire_ReadByte();         // LSB
    temp |= (Onewire_ReadByte() << 8); // MSB

    return (int16_t) temp;
}



