#ifndef AFFICHAGE_UART_H
#define AFFICHAGE_UART_H

#include "stm32l1xx_hal.h"

void UART_Afficher_Lux(UART_HandleTypeDef *huart, float lux);
void UART_Afficher_Mesures(UART_HandleTypeDef *huart, float temperature, float pressure, float altitude, float lux);

#endif // AFFICHAGE_UART_H
