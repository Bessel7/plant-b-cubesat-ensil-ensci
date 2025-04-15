/**
 * \file ax25.c
 * \brief ax25 driver 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 */

#include <string.h>
#include <stdint.h>
#include "errorMngt/error.h"
#include "ax25/ax25.h"

/**
 * \fn void convertToAX25Header ( t_ax25_header *data, char *dest, char ssidDest, char *src, char ssidSrc)
 * \brief convert header structure to AX25 Header 
 * \param header structure
 * \param destination callsign
 * \param SSID destionation
 * \param source callsign
 * \param SSID source
 *
 * \return  void 
 * 
 */

void convertToAX25Header(s_ax25_header *data, unsigned char *dest,
		unsigned char ssidDest, unsigned char *src, unsigned char ssidSrc) {
	int i;
	unsigned char c;

	data->ssidDestination = (unsigned char) (ssidDest << 1 & 0xFE);
	data->ssidSource = (unsigned char) (ssidSrc << 1 & 0xFE);
	for (i = 0; i < 6; i++) {
		c = (unsigned char) dest[i];
		data->destinationAdress[i] = (unsigned char) (c << 1 & 0xFE);
		;
	}

	for (i = 0; i < 6; i++) {
		c = (unsigned char) src[i];
		data->sourceAdress[i] = (unsigned char) (c << 1 & 0xFE);
		;
	}

}
/**
 * \fn void encodeAX25Header(t_ax25_header *data)
 * \brief encode callsign  
 * \param SSID source
 *
 * \return  void 
 * 
 */

void encodeAX25Header(s_ax25_header *data) {
	int i = 0;
	unsigned char c;
	c = (unsigned char) (data->ssidDestination << 1 & 0xFE);
	data->ssidDestination = c;
	c = (unsigned char) (data->ssidSource << 1 & 0xFE);
	data->ssidSource = c;
	for (i = 0; i < 6; i++) {
		c = (unsigned char) data->destinationAdress[i];
		data->destinationAdress[i] = (unsigned char) (c << 1 & 0xFE);
		;
	}

	for (i = 0; i < 6; i++) {
		c = (unsigned char) data->sourceAdress[i];
		data->sourceAdress[i] = (unsigned char) (c << 1 & 0xFE);
		;
	}
}

/**
 * \fn int convertDataToAx25 (t_ax25_packet *data, char *rawdata, unsigned int size )
 * \brief Decode calssign from AX25  
 * \param ax25 structure 
 * \param raw data 
 * \param raw data size 
 *
 * \return  error value SUCCES or  ERROR_AX25_EXCEED_MAX_LENGH
 * 
 */
int convertDataToAx25(s_ax25_packet *data,char *rawdata, int size) {

	int error = SUCCESS;
	int i;
	unsigned char c;

	if (size < (int) sizeof(s_ax25_packet)) {
		memcpy(data, rawdata, (size_t) size);

		/* Convert */
		c = data->header.ssidDestination;
		data->header.ssidDestination = c >> 1 & 0x7F;
		c = data->header.ssidSource;
		data->header.ssidSource = c >> 1 & 0x7F;

		for (i = 0; i < 6; i++) {
			c = data->header.destinationAdress[i];
			data->header.destinationAdress[i] = c >> 1 & 0x7F;

		}

		for (i = 0; i < 6; i++) {
			c = data->header.sourceAdress[i];
			data->header.sourceAdress[i] = c >> 1 & 0x7F;
		}

	} else {
		error = ERROR_AX25_EXCEED_MAX_LENGH;
	}
	return error;

}



uint16_t calculateCRC(unsigned char  *data, int offset, int length) {
		int crc = 0x000;
		for (int i = offset; i < (offset + length); ++i) {
			crc ^= data[i] << 8;
			for (int j = 0; j < 8; ++j) {
				if ((crc & 0x8000) > 0) {
					crc = (crc << 1) ^ 0x1021;
				} else {
					crc = crc << 1;
				}
			}
		}
		return (uint16_t) crc & 0xFFFF;
	}








