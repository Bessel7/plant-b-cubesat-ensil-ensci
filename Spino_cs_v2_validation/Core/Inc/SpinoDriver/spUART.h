#ifndef SP_UART_H
#define SP_UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


void MX_USART1_UART_Init(void);


/*

//Nous n'avons pas réussi à avoir le fonctionnement souhaité.

1*** Nous essayons d'afficher des messages dans le port série en utilisant l'UART.
Comme ceci : 
//HAL_UART_Transmit(&hlpuart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
Résultat : Rien ne s'affiche
2*** Cette manière de faire ne marchant pas, nous nous sommes tourner vers les fontions dans "SpinoDriver/spUART.h", notamment la fonction 
Pour cela on a testé le bout de code suivant qui était dans le main.c :
// unsigned char message[] = "SPINO UART 0.1";
// writeUART(message, sizeof(message) );
Résultat : Rien ne s'affiche
3*** En analysant cette fois le code que vous nous avez fourni 

*/





void writeUART(unsigned char *message, int size );
int readUART(unsigned char *data, int size);

#endif // SP_UART_H
