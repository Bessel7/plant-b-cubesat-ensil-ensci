/**
 * \file experienceMode.c
 * \brief manage Expérience mode
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *
 * \todo gestion de l'I2C
 * \todo faire le .h
 * \todo download data
 */

#include"stdio.h"
#include <string.h>
//#include "main.h"
#include "core/setup.h"
#include "drivers/modem.h"
#include "errorMngt/error.h"
#include "payloadMode/payloadMode.h"

//static unsigned char I2CReadDataStatus;
static unsigned char I2CWriteDataStatus;
//static unsigned char I2CReadData[I2CMAXSIZE];
static unsigned char I2CWriteData[I2CMAXSIZE];

#define I2C_BUFFER_SIZE			255

extern uint8_t CSKB_I2C_TX_BUFFER[I2C_BUFFER_SIZE];
//extern static volatile uint8_t CSKB_I2C_RX_BUFFER[I2C_BUFFER_SIZE];

s_I2C_data gv_I2C_Write_Data[I2CMAXDATA];

short gv_nb_I2CMAXDATA;

void initPayload()
{
//	I2CReadDataStatus = 0;
	I2CWriteDataStatus = 0;
	gv_spino.gv_nb_I2CMAXDATA =0;
}

unsigned char getAllI2Cdata(t_tc_response *resp) {
	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerTlm;
	resp->header.error_code = SUCCESS;
	resp->header.cmd_id =  CMD_PAYLOAD_READ_DATA_ALL;
	resp->header.responseType = RESULT_CMD;
	resp->header.size =  TC_REPONSE_HEADER_SIZE;
	resp->header.spare = (short) 0x01020304;


	int i = 0;

	for (i = 0; i < gv_spino.gv_nb_I2CMAXDATA; i++) {
		resp->size = sizeof(s_I2C_data);
		resp->header.size = TC_REPONSE_HEADER_SIZE + resp->size;
		memcpy(resp->parameter, &gv_I2C_Write_Data[i].data, resp->size);
		memcpy(ax25Frame.data, resp, TC_REPONSE_HEADER_SIZE + resp->size);
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp->size);
		gv_I2C_Write_Data[i].size =0;
		gv_spino.gv_nb_I2CMAXDATA--;


	}
	gv_spino.gv_nb_I2CMAXDATA =0;
	resp->header.cmd_id =  CMD_PAYLOAD_READ_DATA;
	resp->size = 0;
	return SUCCESS;

}

t_tc_response interpretcommandPayload(s_command cmd) {

	t_tc_response resp;

	resp.header.size =TC_REPONSE_HEADER_SIZE;
	resp.header.responseType = RESULT_CMD;
	resp.header.timeStamp = gv_spino.timestamps;
	resp.header.cmd_id = cmd.id;
	resp.header.error_code = SUCCESS;

	switch (cmd.id) {
	case CMD_PAYLOAD_LOAD_DATA:
		logger(LOG_LEVEL_INFO, " CMD_PAYLOAD_LOAD_DATA");
		s_I2C_data data;
		memcpy(&data, cmd.parameter, cmd.size);
		if(cmd.size < I2C_BUFFER_SIZE)
		{
		CSKB_I2C_TX_BUFFER[0]=cmd.size;
		for(uint8_t i = 0; i < cmd.size; i++) {
		   CSKB_I2C_TX_BUFFER[i+1] = cmd.parameter[i];
			}
		resp.header.error_code = SUCCESS;
		  } else
		  {
				resp.header.error_code = ERROR_LOAD_DATA_PAYLOAD_WRONG_SIZE;
		  }




		resp.size = 0;
		break;

	case CMD_PAYLOAD_READ_DATA: /*  modify configuration value */
		logger(LOG_LEVEL_INFO, "  CMD_PAYLOAD_READ_DATA");
		resp.header.error_code = getAllI2Cdata(&resp);
		break;
	default:
		resp.header.error_code = ERROR_COMMAND_UNKNOW;
	}
	return resp;
}

/**
 * \fn void processCommand(t_ax25_packet data_ax25)
 * \brief process command ax25 packet
 * \param  Ax25 packet
 * \return  SUCCESS if or Error code
 *
 */
void processCommandePayload(s_ax25_packet data_ax25) {

	t_tc_response result;
	s_command cmd;

	gv_spino.nbPayloadCommandeReceived++;
	memcpy(&cmd, data_ax25.data, sizeof(s_command));
	if (cmd.key != gv_spino_cmd_key) {
		result.header.size = sizeof(s_tm_tc_header);

		result.header.responseType = RESULT_CMD;
		result.header.timeStamp = gv_spino.timestamps;
		result.header.cmd_id = cmd.id;
		result.header.error_code = ERROR_COMMAND_WITH_WRONG_KEY;
		result.size = 0;

	} else {
		result = interpretcommandPayload(cmd);

	}

	if (result.header.error_code != SUCCESS) {
		gv_spino.nbPayloadCommandeWithError++;
	}
	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerTlm;
	memcpy(ax25Frame.data, &result, TC_REPONSE_HEADER_SIZE + result.size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + result.size);

}

unsigned short payloadMode() {


	s_ax25_packet data_ax25;

	int nbc = readData(gv_data_rx);

	if(nbc <0)
	{
		logger(LOG_LEVEL_CRITICAL, "ERROR DATA RECEIVED");
	}
	else if (nbc != 0) {
		/* traitement des donnees recues */
		int res = convertDataToAx25(&data_ax25,(char *)  gv_data_rx, nbc);
		if (res != SUCCESS) {
			logger(LOG_LEVEL_CRITICAL, "AX25 CONVERSION ISSUE");
		}

		if (memcmp(gv_spinoConfig.spinoDesCallsign,
				data_ax25.header.destinationAdress, 6) == 0) {

			if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_TMTC) {
				processCommand(data_ax25);
			}
		}
		if (memcmp(gv_spinoConfig.payloadCallsign,
				data_ax25.header.destinationAdress, 6) == 0) {

			if (data_ax25.header.ssidDestination
					== (unsigned char) SSID_SPINO_CUBESAT) {
				processCommandePayload(data_ax25);
			} else {
				logger(LOG_LEVEL_CRITICAL, "MESSAGE DROPED");
				processDropMessage((char *) gv_data_rx, (unsigned short) nbc);

			}

		} else {
			// Message not awaited   -  message dropped
			processDropMessage((char *) gv_data_rx, (unsigned short) nbc);
			logger(LOG_LEVEL_CRITICAL, "MESSAGE DROPED");
		}

	}
	return gv_spino.currentState;
}
