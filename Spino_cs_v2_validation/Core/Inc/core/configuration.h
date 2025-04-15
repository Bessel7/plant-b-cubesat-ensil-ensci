
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../ax25/ax25.h"


#define SIZE_RCV_DATA_MAX 240
/* 
 * Define Spino configuration
 */

#define SSID_SPINO_TMTC 15
#define SSID_SPINO_DIGIPEATER 3 
#define SSID_SPINO_MAILBOX 2
#define SSID_SPINO_CUBESAT 4
#define SSID_SPINO_EXPERIMENTAL 5


#define RESET_CAUSE_STATE_UNKNOWN 1

typedef struct configuration {

	long spinoTxFrequency;
	long spinoRxFrequency;
	unsigned short spinoTxModemSpeed;
	unsigned short spinoRxModemSpeed;
	unsigned char spinoRXModemMode;
	unsigned char spinoTXModemMode;
	unsigned char telemetryDelay; /* delay in seconde */
	unsigned char multimode; // envoie des TLM en multimode
	unsigned char info_message_actif;
	unsigned char delay_info_message;/* delay in seconde */
	unsigned char spinoSrcCallsign[CALLSIGN_SIZE];
	unsigned char spinoDesCallsign[CALLSIGN_SIZE];
	unsigned char payloadCallsign[CALLSIGN_SIZE];

} s_configuration_spino;

typedef struct globalVariable {

	unsigned long nbCommandeReceived;
	unsigned long nbCommandeWithError;
	unsigned long nbFrameNotprocessed;
	unsigned long nbDigipeaterMesssageProcessed;
	unsigned long nbExperiemntalCommand;
	unsigned long nbExperiemntalCommandError;
	unsigned long nbMailboxCommandeReceived;
	unsigned long nbMailboxErrorCommandeReceived;
	unsigned long nbPayloadCommandeReceived;
	unsigned long nbPayloadCommandeWithError;
	unsigned long long timestamps;
	unsigned short lastResetCause;
	unsigned short currentState;
	unsigned short gv_nb_I2CMAXDATA;
	unsigned short  spare;
} s_globalVariable;


extern unsigned char gv_data_rx[SIZE_RCV_DATA_MAX];

#endif // CONFIGURATION_H

