#include "affichage_UART.h"
#include "stdio.h"
#include "string.h"

void UART_Afficher_Lux(UART_HandleTypeDef *huart, float lux) {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "Lux: %.2f lx\r\n", lux);
    HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void UART_Afficher_Mesures(UART_HandleTypeDef *huart, float temperature, float pressure, float altitude, float lux) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer),
             "Lux: %.2f lx\r\nT: %.2f C\r\nP: %.2f hPa\r\nAlt: %.2f m\r\n",
             lux, temperature, pressure / 100.0, altitude);
    HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

    snprintf(buffer, sizeof(buffer), " ************************* \r\n", lux);
    HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}
