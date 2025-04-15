/**
 * \file InformationMessage.c
 * \brief manage information message
 * \author Xtophe
 * \version 0.2
 * \date 21/08/2022
 *
 */
#include <stdio.h>
#include <string.h>
#include "core/setup.h"
#include "core/informationMessage.h"
#include "errorMngt/error.h"
#include "logMngt/log.h"
#include "drivers/modem.h"

static s_inf_msg gv_information_msg[MAX_INF_MESSAGE];

static unsigned long long lv_spino_timeStampInfoMsgPrevious = 0;
static unsigned char lv_index_message_actif = 0;

/**
 * \fn  void setupInfoMessage()
 * \brief initialise information global variable
 * \return  void
 *
 */
void setupInfoMessage() {
	int i = 0;
	int j = 0;
	for (i = 0; i < MAX_INF_MESSAGE; i++) {
		gv_information_msg[i].used = INFO_MSG_NOT_USED;
		for (j = 0; j < MAX_SIZE_INF_MSG; j++) {
			gv_information_msg[i].message[j] = 0;
		}

	}
}

/**
 * \fn  int setInfoMessage(char *data,t_tc_response *resp)
 * \param information data in raw => to be set in s_add_inf_msg structure
 * \param *resp output structure with result of command
 *
 * \brief add information message
 *
 * \return  SUCCESS or ERROR_INFO_MSG_INDEX_OUT_OF_BOUND
 *
 */
unsigned char setInfoMessage(char *data, t_tc_response *resp) {

	unsigned char reponse = SUCCESS;
	int index;
	s_add_inf_msg info_msg;

	memcpy(&info_msg, data, sizeof(s_add_inf_msg));
	index = info_msg.index;
	logger(LOG_LEVEL_INFO, "SET INFO MESSAGE");

	if (info_msg.index < MAX_INF_MESSAGE) {
		gv_information_msg[index].message[0] = 0;
		gv_information_msg[index].used = INFO_MSG_USED;
		strncat(gv_information_msg[index].message, info_msg.message,
				MAX_SIZE_INF_MSG);
	} else {
		reponse = ERROR_INFO_MSG_INDEX_OUT_OF_BOUND;
	}

	resp->size = 0;
	return reponse;

}

/**
 * \fn    int  delInfoMessage(char index ,t_tc_response *resp)
 * \param index of message to remove
 * \param *resp output structure with result of command
 *
 * \brief add information message
 *
 * \return  SUCCESS or ERROR_INFO_MSG_INDEX_OUT_OF_BOUND or ERROR_INFO_MSG_ALREADY_NOT_USED
 *
 */

unsigned char delInfoMessage(char index, t_tc_response *resp)

{
	unsigned char reponse = SUCCESS;
	int ind = (int) index;

	if (index < MAX_INF_MESSAGE) {
		if (gv_information_msg[ind].used == INFO_MSG_NOT_USED) {
			reponse = ERROR_INFO_MSG_ALREADY_NOT_USED;
		}
		gv_information_msg[ind].used = (char) INFO_MSG_NOT_USED;
		gv_information_msg[ind].message[0] = (char) 0;

	} else {
		reponse = ERROR_INFO_MSG_INDEX_OUT_OF_BOUND;
	}
	resp->size = 0;
	return reponse;
}

/**
 * \fn void sendInfoMessage()
 *
 * \brief Send information message when information mode is actif
 *
 * \return  void
 *
 */

void sendInfoMessage() {
	int find = 0;
	int cpt = 0;

	if (gv_spinoConfig.info_message_actif == INFO_MSG_USED) {

		if (gv_spino.timestamps
				> (lv_spino_timeStampInfoMsgPrevious
						+ gv_spinoConfig.delay_info_message * 1000)) {

			lv_spino_timeStampInfoMsgPrevious = gv_spino.timestamps;

			t_tc_response resp;
			s_send_inf_msg infMsg;

			resp.header.responseType = INFORMATION_MSG;
			resp.header.error_code = 0;
			resp.header.cmd_id = 0;
			resp.header.timeStamp = gv_spino.timestamps;

			// recherche message actif
			while (find == 0) {
				cpt++;

				if (gv_information_msg[lv_index_message_actif].used
						== INFO_MSG_USED) {

					find = 1;
					infMsg.index = lv_index_message_actif;
					infMsg.used =
							gv_information_msg[lv_index_message_actif].used;
					resp.size = (unsigned short) strlen(
							gv_information_msg[lv_index_message_actif].message)
							+ 2;
					infMsg.message[0] = 0;

					strncat(infMsg.message,
							gv_information_msg[lv_index_message_actif].message,
							MAX_SIZE_INF_MSG);

					//  strcpy(resp.parameter, gv_information_msg[lv_index_message_actif].message);
					memcpy(resp.parameter, &infMsg, resp.size);
					s_ax25_packet ax25Frame;
					ax25Frame.header = gv_headerTlm;
					memcpy(ax25Frame.data, &resp,
							TC_REPONSE_HEADER_SIZE + resp.size);
					/* envoyer la reponse de la  commande */
					writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resp.size);

					logger(LOG_LEVEL_INFO, "MSG INFO SENT");

				} else if (cpt > MAX_INF_MESSAGE) {
					find = 1;
					logger(LOG_LEVEL_CRITICAL, "NO MESSAGE INFO AVAILABLE");
					// stop information message transmission
					gv_spinoConfig.info_message_actif = INFO_MSG_NOT_USED;

				}
				lv_index_message_actif =
						(unsigned char) ((lv_index_message_actif + 1)
								% MAX_INF_MESSAGE);

			}

		}
	}  // do Nothing

}

