/**
 * \file ax25.h
 * \brief ax25 driver 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 */

#ifndef AX25_H
#define AX25_H
#include <stdint.h>
#define MAX_DATA_SIZE 256
#define CALLSIGN_SIZE 6

typedef struct ax25_header {
	unsigned char destinationAdress[6];
	unsigned char ssidDestination;
	unsigned char sourceAdress[6];
	unsigned char ssidSource;
	unsigned char ctrl;
	unsigned char pid;

} s_ax25_header;

typedef struct ax25_packet {
	s_ax25_header header;
	char data[MAX_DATA_SIZE];

} s_ax25_packet;

int convertDataToAx25(s_ax25_packet *data, char *rawdata, int size);
void convertToAX25Header(s_ax25_header *data, unsigned char *dest,
unsigned char ssidDest, unsigned char *src, unsigned char ssidSrc);
void encodeAX25Header(s_ax25_header *data);

uint16_t calculateCRC(unsigned char  *data, int offset, int length);

#endif // AX25_H
