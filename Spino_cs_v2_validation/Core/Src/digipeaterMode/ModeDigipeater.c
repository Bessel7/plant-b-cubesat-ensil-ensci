/**
 * \file digipeater.c
 * \brief MOde Digipeater 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *
 */

#include <stdio.h>
#include <string.h>
#include "core/setup.h"
#include "drivers/modem.h"
#include "errorMngt/error.h"



/**
 * \fn unsigned short digipeater ()
 * \brief manage DIGIPEATER mode
 * \return  void
 * 
 */
unsigned short digipeater() {

	s_ax25_packet data_ax25;

	int nbc = readData(gv_data_rx);

	if(nbc <0)
	{
		logger(LOG_LEVEL_CRITICAL, "ERROR DATA RECEIVED");
	}
	else  if (nbc != 0) {
		/* traitement des donnees recues */
		int res = convertDataToAx25(&data_ax25, (char *) gv_data_rx, nbc);
		if (res != SUCCESS) {
			logger(LOG_LEVEL_CRITICAL, "AX25 CONVESTION ISSUE");
		}
		if (memcmp(gv_spinoConfig.spinoDesCallsign,
				data_ax25.header.destinationAdress, 6) == 0) {

			if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_TMTC) {
				processCommand(data_ax25);
			} else if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_DIGIPEATER) {
				// renvoie la trame
				memcpy(data_ax25.header.destinationAdress,
						data_ax25.header.sourceAdress, 6);
				memcpy(data_ax25.header.sourceAdress,
						gv_spinoConfig.spinoSrcCallsign, 6);
				data_ax25.header.ssidSource = SSID_SPINO_DIGIPEATER;
				data_ax25.header.ssidDestination = data_ax25.header.ssidSource;
				encodeAX25Header(&data_ax25.header);
				nbc = nbc - (int) sizeof(s_ax25_header);
				writeData(data_ax25, nbc);
				gv_spino.nbDigipeaterMesssageProcessed++;
			} else {
				logger(LOG_LEVEL_CRITICAL, "WRONG SSID");
				processDropMessage((char *)gv_data_rx, (unsigned short) nbc);
			}
		} else {
			// Message not awaited   -  message dropped
			processDropMessage((char *) gv_data_rx, (unsigned short) nbc);
		}

	}
	return gv_spino.currentState;
}
