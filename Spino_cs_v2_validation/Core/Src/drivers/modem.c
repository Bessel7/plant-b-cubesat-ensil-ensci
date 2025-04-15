/**
 * \file modem.c
 * \brief modem simulation 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *
 *  \todo   To adapt with embeded target
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "core/setup.h"
#include "ADF7030.h"
#include "SI4463.h"
#include "stm32l4xx_hal.h"
#include "errorMngt/error.h"
#include "ax25/ax25.h"
#include "drivers/modem.h"
#include "SpinoDriver/spUART.h"

#define POSITION_SIZE 16
#define RCV_SIZE_MAX 240
#define RCV_PACKET  20

static int debug = 0;

volatile char gv_spino_data_avalable;
int lv_taille_rx;
extern uint8_t uhf_txbuffer[PAYLOAD_SIZE];

extern uint8_t uhf_txbuffer[PAYLOAD_SIZE];


//here is the radio TX buffer where to write when the frame is cooked.
//Declaration is performed in the ADF7030.c source file.

int setModemMode(uint8_t mode) {
	int rep = SUCCESS;
	if (mode < (MAX_MODE - 1)) {
		config_ADF7030(mode);
	} else {
		rep = ERROR_WRONG_MODEM_MODE;
	}

	return rep;
}

void ModemOpen() {

	gv_spino_data_avalable = 0;

}

int readData(unsigned char *data)
{
	if (debug==1)
	{
		return readDataDebug(data);
	} else
	{
		return  readDataRX2(data);
	}
}


int readDataDebug(unsigned char *data)
{
	int taille;
		char dataSize;
		char dataSize2;
		uint16_t crcCal1;
		uint16_t crcCal2;
		char crc1;
		char crc2;

		taille = readUART(data, 300);
		if (taille != 0) {
			dataSize = data[POSITION_SIZE];
			dataSize2 = data[POSITION_SIZE + 1];

					  crc1 = data[taille-1];
					  crc2 = data[taille-2];
					  crcCal2  =  calculateCRC(data, 0, taille-2);
					  crcCal1 = crc1 + ((crc2 << 8)&0xFF00);
					  if(crcCal2 != crcCal1 )
					  {
						  taille = ERROR_CRC;
					  }

		}

		return taille;


}


int readDataRX2(unsigned char *data) {

	// to do - variable globale

	int size = 0;

	if (gv_spino_data_avalable == 1) {
		size =lv_taille_rx;
		gv_spino_data_avalable =0;
	}

return size ;


}

int readDataRX(uint8_t** access) {


	/** SPINO */
		int taille = 0;
		uint16_t crcCal1;
		uint16_t crcCal2;


		if (gv_spino_data_avalable == 1) {


			//read 1st part

			memcpy(gv_data_rx, &access[0][0], RCV_PACKET);

			lv_taille_rx = gv_data_rx[POSITION_SIZE]+16;
			taille = lv_taille_rx  ;
			// taille = data[POSITION_SIZE]+  (data[POSITION_SIZE+1] >>8 & 0xFF00);

			int crt = RCV_PACKET;

			if (taille <= RCV_SIZE_MAX) {
				// calcul le nb de packet util
				float nbpf = (float)taille / (float) RCV_PACKET;
				int nbp= ceil(nbpf)-1;
				for (int i = 0; i < nbp; i++) {
					memcpy(&gv_data_rx[crt], &access[i + 1][0], RCV_PACKET);
					crt = crt + RCV_PACKET;
				}

			// identification du CRC

				char crc1 = gv_data_rx[taille-1];
				char crc2 = gv_data_rx[taille-2];

				crcCal2  =  calculateCRC(gv_data_rx, 0, taille-2);
				crcCal1 = crc1 + ((crc2 << 8)&0xFF00);
						  if(crcCal2 != crcCal1 )
						  {
							  taille = ERROR_CRC;
						  }

			// todo - Comparaison CRC + code Erreur

			}
			else
			{
				taille = ERROR_WRONG_SIZE_RCV;
			}
			gv_spino_data_avalable =1 ;

		}

		return taille-16;

}

int writeData(const s_ax25_packet ax25Frame, const int length)
{
	if (debug==1)
	{
		return writeDataDebug(ax25Frame,length);
	} else
	{
		return  writeDataTX(ax25Frame,length);
	}
}

int writeDataDebug(const s_ax25_packet ax25Frame, const int length) {

	int rep = SUCCESS;

	int size = length + (int) sizeof(s_ax25_header);
	if (size + 2 < PAYLOAD_SIZE)

	{
		// CRC Compute


		memcpy(&uhf_txbuffer, &ax25Frame, size);
		uhf_txbuffer[POSITION_SIZE] = uhf_txbuffer[POSITION_SIZE]+2;

		uint16_t crc = calculateCRC((unsigned char*) &uhf_txbuffer,0, size);

		// SPINO DEBUG ONLY TO REMOVE


//		writeUART((unsigned char*) &crc, (int) sizeof(crc));

		// SPINO TX


		uhf_txbuffer[size] = crc & 0xFF;
		uhf_txbuffer[size + 1] = (crc >> 8) & 0xFF;

		writeUART((unsigned char*) &uhf_txbuffer, size+2);
		/** SPINO UART

		rep = adf_tx_data();


		 */
		//This function triggers the transmission of the payload
		//contained in the uhf_txbuffer.
		//uhf_txbuffer is flushed and filled again with sync word.
		//If successful, a 0 is returned.
		//Any non-zero value indicates a failure in the process..

	}

	else //ERROR
	{
		rep = ERROR_BUFFER_OVERFLOW;
	}

	return rep;

}

int writeDataTX(const s_ax25_packet ax25Frame, const int length) {

	int rep = SUCCESS;

	int size = length + (int) sizeof(s_ax25_header);
	if (size + 2 < PAYLOAD_SIZE)

	{
		// CRC Compute


		memcpy(&uhf_txbuffer, &ax25Frame, size);

		uhf_txbuffer[POSITION_SIZE] = uhf_txbuffer[POSITION_SIZE]+2;

		uint16_t crc = calculateCRC((unsigned char*) &uhf_txbuffer,0,size);

		// SPINO DEBUG ONLY TO REMOVE


//		writeUART((unsigned char*) &crc, (int) sizeof(crc));

		// SPINO TX


		uhf_txbuffer[size] = crc & 0xFF;
		uhf_txbuffer[size + 1] = (crc >> 8) & 0xFF;

	//	writeUART((unsigned char*) &uhf_txbuffer, size+2);

		rep = adf_tx_data();



		//This function triggers the transmission of the payload
		//contained in the uhf_txbuffer.
		//uhf_txbuffer is flushed and filled again with sync word.
		//If successful, a 0 is returned.
		//Any non-zero value indicates a failure in the process..

	}

	else //ERROR
	{
		rep = ERROR_BUFFER_OVERFLOW;
	}

	return rep;

}
