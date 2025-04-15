/**
 * \file log.c
 * \brief manage log 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *  
 */

#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "core/setup.h"
#include "errorMngt/error.h"
#include "drivers/modem.h"

//extern void writeData(s_ax25_packet ax25Frame, int length);

unsigned char gv_SelectedLogLevel = 0;

s_logs logs[MAX_LOG];
int gvLogIndex = 0;
char gvLogMsg[MAX_SIZE_MSG_LOG];

/**
 * \fn void logger(char level, char *message)
 * \brief log message in log Table
 * \param level  define type of log
 *  \param message Ascii message
 * \return  void
 *
 */
void logger(char level, char *message) {

	if (level >= gv_SelectedLogLevel) {
		gvLogIndex = (gvLogIndex + 1) % MAX_LOG;
		logs[gvLogIndex].priority = level;
		logs[gvLogIndex].timeStamps = gv_spino.timestamps;
		logs[gvLogIndex].log[0] = 0;
		strncat(logs[gvLogIndex].log, message, MAX_SIZE_MSG_LOG - 1);
		printf("LOG : ");
		printf(message);
		printf("\r\n");
	}

}

/**
 * \fn int getLastLog(t_tc_response *resp)
 * \brief set to response the last log
 * \param *resp output structure with result of command
 * \return  SUCCESS 
 * 
 * \todo : optimise message lenght in response
 *
 */

unsigned char getLastLog(t_tc_response *resp) {

	logger(LOG_LEVEL_INFO, " GET_LAST_LOG_MESSAGE");
	unsigned short taille_message = (unsigned short) strlen(
			logs[gvLogIndex].log) + SIZE_S_LOG + 1; // +1 pour inclure le 0
	memcpy(resp->parameter, &logs[gvLogIndex], taille_message);
	resp->size = taille_message;
	return SUCCESS;

}

/**
 * \fn int getAllLogs(t_tc_response *resp)
 * \brief send  all logs
 * \param *resp output structure with result of command
 * \return  SUCCESS 
 * 
 * \todo : suprimer répétition dernier logs 
 * 
 */

unsigned char getAllLogs(t_tc_response *resp) {
	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerTlm;
	resp->header.error_code = SUCCESS;
	resp->header.cmd_id =  GET_LOG;
	resp->header.responseType = RESULT_CMD;
	resp->header.size =  TC_REPONSE_HEADER_SIZE;
	resp->header.spare = (short) 0x01020304;

	logger(LOG_LEVEL_CRITICAL, " GET_ALL_LOG_MESSAGE");
	int i = 0;

	for (i = 0; i < MAX_LOG; i++) {

		unsigned short taille_message = (unsigned short) strlen(logs[i].log)
				+ SIZE_S_LOG + 1; // +1 pour inclure le 0
		memcpy(resp->parameter, &logs[i], taille_message);
		resp->size = taille_message;
		resp->header.size =  TC_REPONSE_HEADER_SIZE+taille_message;
		resp->header.timeStamp =  gv_spino.timestamps;
		memcpy(ax25Frame.data, resp, TC_REPONSE_HEADER_SIZE + resp->size);
		/* envoyer la reponse de la  commande */
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp->size);

		HAL_Delay(1000);
	}
	/* return success of the command */
	resp->header.cmd_id =  GET_ALL_LOG;
	resp->size = 0;
	return SUCCESS;

}
