/**
 * \file experimentalMode.c
 * \brief manage experimental mode
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *  
 */

#include <stdio.h>
#include <string.h>
#include "core/setup.h"
#include "drivers/modem.h"
#include "mailboxMode/mailbox.h"
#include "errorMngt/error.h"
#include "experimentalMode/experimentalMode.h"


unsigned long long lv_spino_expe_timeStampPrevious;
s_ax25_header gv_headerExp;

t_tc_response respexpBeacon;
s_tle gv_tle;
s_experimental_beacon expBeacon;

void initExpe() {

	expBeacon.id = (long) 0x0F55F0AA0;
	expBeacon.delay = 10;
	expBeacon.idf = (short)0x0F55;
	expBeacon.expereceived =0;
	expBeacon.expereceivedfail =0;
	respexpBeacon.header.responseType = EXPEBEACON;
	respexpBeacon.header.error_code = 0;
	respexpBeacon.header.cmd_id = CMD_BEACON;
	respexpBeacon.size = sizeof(s_experimental_beacon);
	respexpBeacon.header.size = TC_REPONSE_HEADER_SIZE + respexpBeacon.size ;

	gv_headerExp.pid = 255;

	convertToAX25Header(&gv_headerExp, gv_spinoConfig.spinoDesCallsign,
			SSID_SPINO_EXPERIMENTAL, gv_spinoConfig.spinoSrcCallsign,
			SSID_SPINO_EXPERIMENTAL);

}

static unsigned char expDownloadTLE(t_tc_response *resp) {

	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerExp;

	unsigned short taille_message = SIZE_TLE;
	resp->header.cmd_id = CMD_DOWNLOAD_TLE_1;
	resp->parameter[0] = 0;
	strncat(resp->parameter, gv_tle.tleLine1, SIZE_TLE);
	resp->size = taille_message;
	resp->header.size = TC_REPONSE_HEADER_SIZE +resp->size;
	memcpy(ax25Frame.data, resp, TC_REPONSE_HEADER_SIZE + resp->size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp->size);
	resp->header.cmd_id = CMD_DOWNLOAD_TLE_2;
	resp->parameter[0] = 0;
	strncat(resp->parameter, gv_tle.tleLine2, SIZE_TLE);
	resp->size = taille_message;
	resp->header.size = TC_REPONSE_HEADER_SIZE +resp->size;
	memcpy(ax25Frame.data, resp, TC_REPONSE_HEADER_SIZE + resp->size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp->size);


	return SUCCESS;
}

t_tc_response interpretExperimentalCommmand(s_command cmd,
		unsigned char *callsign) {

	t_tc_response resp;

	resp.header.responseType = RESULT_CMD;
	resp.header.timeStamp = gv_spino.timestamps;
	resp.header.cmd_id = cmd.id;
	resp.header.error_code = SUCCESS;

	switch (cmd.id) {

	case CMD_EXPE_INIT:
		logger(LOG_LEVEL_INFO, "Commande EXP  RESET");
		resp.header.error_code = initialise();
		resp.size = 0;
		break;

	case CMD_SET_EXPE_BEACON_DELAY:
		logger(LOG_LEVEL_INFO, "Commande CMD_SET_EXPE_BEACON_DELAY");
		s_set_value setvalue;
		memcpy(&setvalue, cmd.parameter, cmd.size);
		expBeacon.delay = (unsigned short) setvalue.value[0];
		resp.header.error_code = SUCCESS;
		resp.size = 0;
		break;

	case CMD_LOAD_TLE_1:
		logger(LOG_LEVEL_INFO, "Commande CMD_LOAD_TLE_1");
		if (cmd.size != SIZE_TLE) {
			logger(LOG_LEVEL_INFO, "Commande CMD_LOAD_TLE_1 SIZE OK");
			gv_tle.tleLine1[0] = 0;
			strncat(gv_tle.tleLine1, cmd.parameter, SIZE_TLE);
		} else {
			resp.header.error_code = ERROR_TLE_WRONG_SIZE;

		}
		resp.size = 0;
		break;
	case CMD_LOAD_TLE_2:
		logger(LOG_LEVEL_INFO, "Commande CMD_LOAD_TLE_2");
		if (cmd.size != SIZE_TLE) {
			gv_tle.tleLine2[0] = 0;
			strncat(gv_tle.tleLine2, cmd.parameter, SIZE_TLE);
		} else {
			resp.header.error_code = ERROR_TLE_WRONG_SIZE;
		}
		resp.size = 0;
		break;

	case CMD_DOWNLOAD_TLE:
		logger(LOG_LEVEL_INFO, "Commande CMD_DOWNLOAD_TLE");
		resp.header.error_code = expDownloadTLE(&resp);
		resp.header.cmd_id = CMD_DOWNLOAD_TLE;
		resp.size = 0;
		break;
	case CMD_EXP_ADD_DATA:
		logger(LOG_LEVEL_INFO, "Commande CMD_EXP_ADD_DATA");
		if (cmd.size > MAX_LENGHT_MESSAGE) {
			resp.header.error_code = ERROR_ADD_MSG_EXCED_SIZE;
		} else {
			char message[MAX_LENGHT_MESSAGE];
			memcpy(message, &cmd.parameter, cmd.size);
			logger(LOG_LEVEL_INFO, message);
			resp.header.error_code = addMessage(callsign, message, cmd.size);
		}
		resp.size = 0;
		break;
	case CMD_EXPE_GET_LIST:
		logger(LOG_LEVEL_INFO, "Commande CMD_EXPE_GET_LIST");
		resp.header.error_code = getListMailbox(&resp);
		break;
	case CMD_EXPE_DELETTE_ALL:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX DELETE MAILBOX");
		resp.header.error_code = deleteMailBox(callsign );
		resp.size = 0;
		break;
	case CMD_EXPE_GET_ALL_DATA:
		resp.header.error_code = getAllMesage(callsign);
		resp.size = 0;
		break;

	default:
		sprintf(gvLogMsg, "erreur Experimental cmd %d \r\n", cmd.id);
		logger(LOG_LEVEL_CRITICAL, gvLogMsg);
		resp.header.error_code = ERROR_COMMAND_UNKNOW;
		resp.size = 0;
		break;
	}

	if(resp.header.error_code!= SUCCESS)
	{
		expBeacon.expereceivedfail++;
	}

	 resp.header.size = TC_REPONSE_HEADER_SIZE + resp.size;
	return resp;
}

void sendBeaconExpe() {

	if (gv_spino.timestamps
			> (lv_spino_expe_timeStampPrevious
					+ (unsigned long long) (expBeacon.delay * 1000))) {
		s_ax25_packet ax25Frame;
		lv_spino_expe_timeStampPrevious = gv_spino.timestamps;
		respexpBeacon.header.timeStamp = gv_spino.timestamps;
		respexpBeacon.header.cmd_id = CMD_BEACON;
		memcpy(respexpBeacon.parameter, &expBeacon,
				sizeof(s_experimental_beacon));
		ax25Frame.header = gv_headerExp;
		memcpy(ax25Frame.data, &respexpBeacon,
				TC_REPONSE_HEADER_SIZE + respexpBeacon.size);
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + respexpBeacon.size);
		logger(LOG_LEVEL_INFO, "Envoie BEACON");
	}

}

static void processExperimental(s_ax25_packet data_ax25) {

	t_tc_response result;
	s_command cmd;

	gv_spino.nbExperiemntalCommand++;


	memcpy(&cmd, data_ax25.data, sizeof(s_command));
	result = interpretExperimentalCommmand(cmd, data_ax25.header.sourceAdress);
	if (result.header.error_code != SUCCESS) {
		gv_spino.nbExperiemntalCommandError++;

	}
	s_ax25_packet ax25Frame;

	ax25Frame.header = gv_headerExp;
	memcpy(ax25Frame.data, &result, TC_REPONSE_HEADER_SIZE + result.size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + result.size);
	sprintf(gvLogMsg, "RESULT EXPE COMMAND  %x %x ", result.header.cmd_id,
			result.header.error_code);
	logger(LOG_LEVEL_CRITICAL, gvLogMsg);

}

unsigned short experimentalMode() {

	s_ax25_packet data_ax25;

	logger(LOG_LEVEL_CRITICAL, "EXPERIMENTAL MODE ");
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
				logger(LOG_LEVEL_CRITICAL, "TRT COMMANDE NORMAL");
				processCommand(data_ax25);
			} else if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_EXPERIMENTAL) {
				logger(LOG_LEVEL_CRITICAL, "TRT COMMANDE EXPE");
				expBeacon.expereceived++;
				processExperimental(data_ax25);
			} else {
				// Message not awaited   -  message dropped
				processDropMessage((char *)gv_data_rx, (unsigned short) nbc);
			}
		} else {
			processDropMessage((char *) gv_data_rx, (unsigned short) nbc);
		}
	}

	sendBeaconExpe();

	return gv_spino.currentState;
}
