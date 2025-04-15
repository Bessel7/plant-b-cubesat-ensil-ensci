/**
 * \file DropMessage.c
 * \brief manage message dropped
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
#include "dropMsgMngt/DropMessage.h"

extern void writeData(s_ax25_packet ax25Frame, int length);

/**
 * \fn void processDropMessage (char* data_ax25, int size)
 * \param message Ax25 dropped message
 * \param size message size of dropped message
 * \brief manage message dropped 
 *
 * \return  void
 * 
 */
extern s_unprocessedmessageList gv_unprocess_messages;

void processDropMessage(char *data_ax25, unsigned short size) {


	logger(LOG_LEVEL_CRITICAL, "MESSAGE DROPPED");
	gv_spino.nbFrameNotprocessed++;

	gv_unprocess_messages.index = (gv_unprocess_messages.index + 1)
			% MAX_UNPROCESSED_MESSAGE_LIST_SIZE;
	gv_unprocess_messages.message[gv_unprocess_messages.index].timestamps =
			gv_spino.timestamps;

	if (size > MAX_UNPROCESSED_MESSAGE_LENGHT) {
		// trunck to MAX_MESSAGE_SIZE
		gv_unprocess_messages.message[gv_unprocess_messages.index].size =
				MAX_UNPROCESSED_MESSAGE_LENGHT;

	} else {

		gv_unprocess_messages.message[gv_unprocess_messages.index].size = size;
	}

	logger(LOG_LEVEL_CRITICAL, data_ax25);
	memcpy(&gv_unprocess_messages.message[gv_unprocess_messages.index].data,
			data_ax25,
			gv_unprocess_messages.message[gv_unprocess_messages.index].size);

}

/**
 * \fn int getLastDroppedMessage(t_tc_response *resp)
 * \brief set to response the last dropped message
 * \param *resp output structure with result of command
 * \return  SUCCESS 
 * 
 */

unsigned char getLastDroppedMessage(t_tc_response *resp) {

	logger(LOG_LEVEL_CRITICAL, " GET_LAST_DROPED_MESSAGE");
	unsigned short taille_message =
			gv_unprocess_messages.message[gv_unprocess_messages.index].size;
	taille_message = taille_message + (unsigned short) SIZE_HEADER_DROP;
	if (taille_message > MAX_DATA_SIZE) {
		taille_message = MAX_DATA_SIZE;
	}
	memcpy(resp->parameter,
			&gv_unprocess_messages.message[gv_unprocess_messages.index],
			(size_t) taille_message);
	resp->size = taille_message;
	return SUCCESS;

}

/**
 * \fn int getLastDroppedMessage(t_tc_response *resp)
 * \brief set to response the last dropped message
 * \param *resp output structure with result of command
 * \return  SUCCESS 
 * 
 * \todo : suprimer répétition dernier message dropped 
 * 
 */

unsigned char getAllDroppedMessage(t_tc_response *resp) {
	s_ax25_packet ax25Frame;




	ax25Frame.header = gv_headerTlm;
	resp->header.error_code = SUCCESS;
	resp->header.cmd_id =  GET_DROP;
	resp->header.responseType = RESULT_CMD;
	resp->header.size =  TC_REPONSE_HEADER_SIZE;
	resp->header.spare = (short) 0x01020304;


	unsigned short taille_message;
	int i = 0;

	for (i = 0; i < MAX_UNPROCESSED_MESSAGE_LIST_SIZE; i++) {

		taille_message = gv_unprocess_messages.message[i].size;
		taille_message = taille_message + SIZE_HEADER_DROP;
		if (taille_message > MAX_DATA_SIZE) {
			taille_message = MAX_DATA_SIZE;
		}
		resp->size = taille_message;
		memcpy(resp->parameter, &gv_unprocess_messages.message[i],
				(size_t) taille_message);
		resp->header.size =  TC_REPONSE_HEADER_SIZE+resp->size;
		memcpy(ax25Frame.data, resp,
				(size_t) (TC_REPONSE_HEADER_SIZE + resp->size));
		/* envoyer la reponse de la  commande */
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp->size);

		HAL_Delay(1000);
	}
	resp->header.cmd_id =  GET_ALL_DROPED_MESSAGE;
	resp->size = 0;
	return SUCCESS;

}
