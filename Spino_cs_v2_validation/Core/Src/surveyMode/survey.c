#include"stdio.h"
#include <string.h>
#include "core/setup.h"
#include "drivers/modem.h"
#include "errorMngt/error.h"
#include "dropMsgMngt/DropMessage.h"

unsigned short survey() {

	s_ax25_packet data_ax25;
 int nbc = readData(gv_data_rx);

	if(nbc <0)
	{
		logger(LOG_LEVEL_CRITICAL, "ERROR DATA RECEIVED");
	}
	else 	if (nbc != 0) {
		/* traitement des donnees recues */
		int res = convertDataToAx25(&data_ax25, (char *) gv_data_rx, nbc);
		if (res != SUCCESS) {
			logger(LOG_LEVEL_CRITICAL, "AX25 CONVERSION ISSUE");
		}

		if (memcmp(gv_spinoConfig.spinoDesCallsign,
				data_ax25.header.destinationAdress, 6) == 0) {

			if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_TMTC) {
				processCommand(data_ax25);
			}
		} else {
			// Message not awaited   -  message dropped
			processDropMessage((char *) gv_data_rx, (unsigned short) nbc);

		}

	}
	return gv_spino.currentState;
}
