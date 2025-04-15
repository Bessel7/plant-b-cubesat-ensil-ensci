/**
 * \file mailbox.c
 * \brief mailbox management
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *  
 */

#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "core/setup.h"
#include "drivers/modem.h"
#include "mailboxMode/mailbox.h"
#include "errorMngt/error.h"
#include "ax25/ax25.h"

s_mailboxes gvMailboxes;

/**
 * \fn int  initialise()
 * \brief initialise mailboxes 
 *
 * \return  SUCCESS 
 * 
 */

unsigned char initialise() {
	int i = 0;
	int j = 0;
	int k = 0;

	gvMailboxes.indexfreeMailbox = 0;
	gvMailboxes.usedMailboxNumber = 0;


	for (i = 0; i < MAX_MAILBOX; i++) {
		gvMailboxes.mailbox[i].indexFreeMessage = MAX_MAILBOX - 1;
		gvMailboxes.mailbox[i].indexNextMessage = 0;
		gvMailboxes.mailbox[i].messageNumber = 0;
		gvMailboxes.mailbox[i].timestampCreation = 0;

		for (j = 0; j < CALLSIGN_SIZE; j++) {
			gvMailboxes.mailbox[i].callsign[j] = ' ';
		}
		for (j = 0; j < MAX_MESSAGE; j++) {
			for (k = 0; k < MAX_LENGHT_MESSAGE; k++) {
				gvMailboxes.mailbox[i].messages[j].message[k] = 0;
			}

			gvMailboxes.mailbox[i].messages[j].size = 0;
			gvMailboxes.mailbox[i].messages[j].timestamps = 0;
		}
	}
	return SUCCESS;
}

/**
 * \fn int  addMessage (char *callsign, char *message,int sizemMessage)
 * \brief add message to the mailbox if the mailbox doesnot exist a new mailbox is created 
 *
 * \return  SUCCESS else ERROR_MAILBOX_FULL, 
 * 
 * 
 */
unsigned char addMessage(unsigned char *callsign, char *message,
		const unsigned short sizemMessage) {
	unsigned char reponse = SUCCESS;
	s_mailbox *pMailbox;
	// create 1st mailbox
	if (gvMailboxes.usedMailboxNumber == 0) {

		pMailbox = &gvMailboxes.mailbox[gvMailboxes.indexfreeMailbox];
		// creation de la mailbox
		memcpy(pMailbox->callsign, callsign, 6);

		memcpy(pMailbox->messages[pMailbox->indexNextMessage].message, message,
				sizemMessage);
		pMailbox->messages[pMailbox->indexNextMessage].timestamps =
				gv_spino.timestamps;
		pMailbox->messages[pMailbox->indexNextMessage].size = sizemMessage;

		pMailbox->indexNextMessage =
				(unsigned char) ((pMailbox->indexNextMessage + 1) % MAX_MAILBOX);
		pMailbox->messageNumber++;
		pMailbox->timestampCreation = gv_spino.timestamps;

		gvMailboxes.usedMailboxNumber++;
		gvMailboxes.indexfreeMailbox++;
	} else { /* recherche si BAL existe */

		int i = 0;
		char find = 0;

		while ((i < MAX_MAILBOX) & (find == 0)) {
			//if (gvMailboxes.mailbox[i].messageNumber != 0)
			//{
			if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0))

			{  // ajout de message

				pMailbox = &gvMailboxes.mailbox[i];
				// verifie place dispo
				if (pMailbox->messageNumber < MAX_MESSAGE) {
					memcpy(
							pMailbox->messages[pMailbox->indexNextMessage].message,
							message, sizemMessage);
					pMailbox->messages[pMailbox->indexNextMessage].timestamps =
							gv_spino.timestamps;
					pMailbox->messages[pMailbox->indexNextMessage].size =
							sizemMessage;

					pMailbox->indexNextMessage =
							(unsigned char) ((pMailbox->indexNextMessage + 1)
									% MAX_MAILBOX);
					pMailbox->messageNumber++;

				} else {
					reponse = ERROR_MAILBOX_FULL;
				}
				find = 1;
			}
			//}
			i++;
		}

		if (find == 0) {
			// creation BAL + ajout Message

			if (gvMailboxes.usedMailboxNumber < MAX_MAILBOX) {
				// reste de la place
				pMailbox = &gvMailboxes.mailbox[gvMailboxes.indexfreeMailbox];
				// creation de la mailbox
				memcpy(pMailbox->callsign, callsign, 6);

				memcpy(pMailbox->messages[pMailbox->indexNextMessage].message,
						message, sizemMessage);
				pMailbox->messages[pMailbox->indexNextMessage].timestamps =
						gv_spino.timestamps;
				pMailbox->messages[pMailbox->indexNextMessage].size =
						sizemMessage;
				pMailbox->indexNextMessage =
						(unsigned char) ((pMailbox->indexNextMessage + 1)
								% MAX_MAILBOX);

				pMailbox->messageNumber++;
				pMailbox->timestampCreation = gv_spino.timestamps;

				// recher de la nouvelle mailbox libre
				while ((i < MAX_MAILBOX) & (find == 0)) {
					if (gvMailboxes.mailbox[i].messageNumber == 0) {
						find = 1;
						gvMailboxes.indexfreeMailbox = i;
					} else {
						i++;
					}
				}

			}

		}
	}
	return reponse;
}

/**
 * \fn int  deletteMessage ( char *callsign)
 * \brief  delete the first message 
 *
 * \return  SUCCESS or ERROR_MAILBOX_EMPTY
 * 
 */

static unsigned char deletteMessage(unsigned char *callsign,
		t_tc_response *resp) {
	int i = 0;
	int find = 0;
	s_mailbox *pMailbox;
	unsigned char reponse = SUCCESS;
	while ((i < MAX_MAILBOX) & (find == 0)) {

		if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0)) { // delete message
			find = 1;
			pMailbox = &gvMailboxes.mailbox[i];
			if (gvMailboxes.mailbox->messageNumber > 0) {
				pMailbox->indexFreeMessage =
						(unsigned char) ((pMailbox->indexFreeMessage + 1)
								% MAX_MESSAGE);
				pMailbox->messageNumber--;

			} else {
				reponse = ERROR_MAILBOX_EMPTY;

			}

		}
		i++;

	}

	if (find == 0) {
		reponse = ERROR_MAILBOX_NOT_FOUND;
	}

	resp->size = 0;

	return reponse;

}
/**
 * \fn int getListMailbox(t_tc_response *resp)
 * \brief return the list of mailboxes  
 *
 * \return  SUCCESS else ERROR_MAILBOX_FULL, 
 * 
 * 
 */

unsigned char getListMailbox(t_tc_response *resp) {
	s_list_mailbox list[MAX_MAILBOX];
	unsigned char response = SUCCESS;
	int i;

	for (i = 0; i < MAX_MAILBOX; i++) {

		memcpy(list[i].callsign, gvMailboxes.mailbox[i].callsign, (int) 6);
		list[i].nb_message = gvMailboxes.mailbox[i].messageNumber;

	}

	resp->size = sizeof(s_list_mailbox) * MAX_MAILBOX;
	memcpy(resp->parameter, list, resp->size);

	return response;
}

/**
 * \fn int deleteMailBox (char *callsign,t_tc_response *resp)
 * \brief delette Mailbox 
 *
 * \return  SUCCESS else ERROR_MAILBOX_NOT_FOUND, 
 * 
 * 
 */
unsigned char deleteMailBox(unsigned char *callsign) {
	int i = 0;
	int j = 0;
	int find = 0;
	s_mailbox *pMailbox;
	unsigned char reponse = SUCCESS;

	while ((i < MAX_MAILBOX) & (find == 0)) {

		if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0)) { // delete message

			pMailbox = &gvMailboxes.mailbox[i];

			pMailbox->indexFreeMessage = MAX_MESSAGE - 1;
			pMailbox->indexNextMessage = 0;
			pMailbox->messageNumber = 0;
			pMailbox->timestampCreation = 0;

			gvMailboxes.indexfreeMailbox = i;
			gvMailboxes.usedMailboxNumber--;
			for (j = 0; j < CALLSIGN_SIZE; j++) {
				pMailbox->callsign[j] = ' ';
			}

			find = 1;

		}

		i++;

	}

	if (find == 0) {
		reponse = ERROR_MAILBOX_NOT_FOUND;

	}

	return reponse;
}

/**
 * \fn int getLastMessage (char * callsign,t_tc_response *resp)
 * \brief return the last message 
 *
 * \return  SUCCESS else ERROR_MAILBOX_NOT_FOUND, 
 * 
 * 
 */

static unsigned char getLastMessage(unsigned char *callsign,
		t_tc_response *resp) {

	int i = 0;
	int find = 0;
	s_mailbox *pMailbox;
	unsigned char reponse = SUCCESS;
	s_get_message messagelue;

	while ((i < MAX_MAILBOX) & (find == 0)) {

		if (gvMailboxes.mailbox[i].messageNumber != 0) {

			if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0)) { // retourn  message

				pMailbox = &gvMailboxes.mailbox[i];
				int message_index = ((pMailbox->indexNextMessage - 1) % MAX_MESSAGE);
				if (message_index < 0) {
					message_index = MAX_MESSAGE - 1;
				}

				messagelue.index = (unsigned char) message_index;
				memcpy(messagelue.callsign, callsign, 6);
				messagelue.timestamps =
						pMailbox->messages[message_index].timestamps;

				memcpy(messagelue.message,
						pMailbox->messages[message_index].message,
						pMailbox->messages[message_index].size);

				resp->size = pMailbox->messages[message_index].size
						+ SIZE_T_GET_MESSAGE;
				memcpy(resp->parameter, &messagelue, resp->size);
				find = 1;

			}

		}
		i++;

	}

	if (find == 0) {
		logger(LOG_LEVEL_INFO, "Mesage not find ");
		reponse = ERROR_MAILBOX_NOT_FOUND;
		resp->size = 0;
		resp->header.error_code = ERROR_MAILBOX_NOT_FOUND;
	}

	return reponse;

}

/**
 * \fn int getMessage (char * callsign, unsigned char index, t_tc_response *resp)
 * \brief return message from the callsign  
 * \param callsign 
 * \param index message index 
 * \param response structure 
 * 
 * \return  SUCCESS else ERROR_MAILBOX_NOT_FOUND, 
 * 
 */

static unsigned char getMessage(unsigned char *callsign, unsigned char index,
		t_tc_response *resp) {
	int i = 0;
	int find = 0;
	s_mailbox *pMailbox;
	unsigned char reponse = SUCCESS;
	s_get_message messagelue;

	while ((i < MAX_MAILBOX) & (find == 0)) {

		if (gvMailboxes.mailbox[i].messageNumber != 0) {

			if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0)) { // retourn  message

				pMailbox = &gvMailboxes.mailbox[i];

				sprintf(gvLogMsg, "Taille INDEX %d LAST %d\r\n",
						pMailbox->indexNextMessage, index);
				logger(LOG_LEVEL_CRITICAL, gvLogMsg);
				if (pMailbox->messages[index].size != 0) {
					messagelue.index = index;
					memcpy(messagelue.callsign, callsign, 6);
					messagelue.timestamps =
							pMailbox->messages[index].timestamps;
					memcpy(messagelue.message,
							pMailbox->messages[index].message,
							pMailbox->messages[index].size);
					resp->size = pMailbox->messages[index].size
							+ SIZE_T_GET_MESSAGE;
					memcpy(resp->parameter, &messagelue, resp->size);
				} else {
					resp->size = 0;
					resp->header.error_code = ERROR_MESSAGE_EMPTY;
				}

				find = 1;

			}

		}
		i++;

	}

	if (find == 0) {

		reponse = ERROR_MAILBOX_NOT_FOUND;
		resp->size = 0;
		resp->header.error_code = ERROR_MAILBOX_NOT_FOUND;
	}

	return reponse;
}

/**
 * \fn int getAllMesage (char * callsign)
 * \brief return all message from the callsign  
 * \param callsign 
 * 
 * \return  SUCCESS else ERROR_MAILBOX_NOT_FOUND, 
 * 
 */

unsigned char getAllMesage(unsigned char *callsign) {

	int i = 0;
	int find = 0;
	s_mailbox *pMailbox;
	unsigned char reponse = SUCCESS;
	s_get_message messagelue;

	t_tc_response resp;
	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerTlm;

	logger(LOG_LEVEL_CRITICAL, " GET_ALL_MESSAGE");

	memcpy(messagelue.callsign, callsign, 6);

	while ((i < MAX_MAILBOX) & (find == 0)) {

		if (gvMailboxes.mailbox[i].messageNumber != 0) {

			if ((memcmp(gvMailboxes.mailbox[i].callsign, callsign, 6) == 0)) { // retourn  message

				pMailbox = &gvMailboxes.mailbox[i];

				// envoie des messages de la boite
				unsigned char j;
				resp.header.cmd_id = CMD_MAILBOX_GET_MSG;
				resp.header.error_code = SUCCESS;
				resp.header.responseType = RESULT_CMD;
		//		resp.header.spare = 0;
				resp.header.timeStamp = gv_spino.timestamps;
				for (j = 0; j < (unsigned char) MAX_MESSAGE; j++) {
					if (pMailbox->messages[j].size != 0) {
						messagelue.index = j;
						messagelue.timestamps =
								pMailbox->messages[j].timestamps;
						memcpy(messagelue.message,
								pMailbox->messages[j].message,
								pMailbox->messages[j].size);
						resp.size = pMailbox->messages[j].size
								+ SIZE_T_GET_MESSAGE;
						resp.header.size =  TC_REPONSE_HEADER_SIZE + resp.size;
						memcpy(resp.parameter, &messagelue, resp.size);
						memcpy(ax25Frame.data, &resp,
								TC_REPONSE_HEADER_SIZE + resp.size);
						/* envoyer la reponse de la  commande */
						writeData(ax25Frame,
								TC_REPONSE_HEADER_SIZE + resp.size);
						HAL_Delay(1000);
					}
				}

				find = 1;

			}

		}
		i++;

	}

	if (find == 0) {

		reponse = ERROR_MAILBOX_NOT_FOUND;

	} else
	{
		resp.size=0;
		resp.header.size =  TC_REPONSE_HEADER_SIZE + resp.size;
	}

	return reponse;
}

/**
 * \fn int dumpMailbox ()
 * \brief return all message from all mailbox 
 * \param none
 * \return  SUCCESS else ERROR_MAILBOX_NOT_FOUND, 
 * 
 */

static unsigned char dumpMailbox() {

	int i = 0;
	unsigned char response = SUCCESS;
	for (i = 0; i < MAX_MAILBOX; i++) {
		if (gvMailboxes.mailbox[i].messageNumber != 0) {
			response = getAllMesage(gvMailboxes.mailbox[i].callsign);

		}
	}

	return response;
}

static t_tc_response interpretMailBoxcommand(s_command cmd,
		unsigned char *callsign) {

	t_tc_response resp;
	unsigned char reponse = SUCCESS;


	resp.header.timeStamp = gv_spino.timestamps;


	switch (cmd.id) {

	case CMD_MAILBOX_INIT:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX RESET");
		reponse = initialise();
		resp.size = 0;
		break;

	case CMD_MAILBOX_ADD_MSG:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX ADD MSG");
		if (cmd.size > MAX_LENGHT_MESSAGE) {
			reponse = ERROR_ADD_MSG_EXCED_SIZE;
		} else {
			char message[MAX_LENGHT_MESSAGE];
			logger(LOG_LEVEL_CRITICAL, gvLogMsg);
			memcpy(message, cmd.parameter, cmd.size);
			reponse = addMessage(callsign, message, cmd.size);
		}
		resp.size = 0;
		break;

	case CMD_MAILBOX_DEL_MSG:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX DELETE MSG !!!");
		reponse = deletteMessage(callsign, &resp);
		resp.size = 0;
		break;
	case CMD_MAILBOX_GET_LIST_BOX:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX LIST MAILBOXES");
		reponse = getListMailbox(&resp);
		break;
	case CMD_MAILBOX_DELETTE_BOX:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX DELETE MAILBOX");
		reponse = deleteMailBox(callsign);
		resp.size = 0;
		break;
	case CMD_MAILBOX_GET_LAST_MSG:
		logger(LOG_LEVEL_INFO, "Commande MAILBOX GET LAST MESSAGE");
		if (cmd.size == 12) {
			unsigned char callsignMailbox[6];
			memcpy(callsignMailbox, cmd.parameter, cmd.size);
			reponse = getLastMessage(callsignMailbox, &resp);
		} else {
			reponse = ERROR_GET_LAST_MSG_CALLSIGN_WRONG_SIZE;
			resp.size = 0;
		}

		break;
	case CMD_MAILBOX_GET_MSG:

		logger(LOG_LEVEL_INFO, "Commande MAILBOX GET MESSAGE INDEX");
		reponse = getMessage((unsigned char*) &cmd.parameter[1],
				(unsigned char) cmd.parameter[0], &resp);
		break;

	case CMD_MAILBOX_GET_ALL_MSG:
		reponse = getAllMesage(callsign);
		resp.size = 0;
		break;
	case CMD_MAILBOX_DUMP_MAILBOX:
		reponse = dumpMailbox();
		break;
	default:
		// generation code erreur
		sprintf(gvLogMsg, "erreur mailbox cmd %d \r\n", cmd.id);
		logger(LOG_LEVEL_CRITICAL, gvLogMsg);
		reponse = ERROR_COMMAND_UNKNOW;
		resp.size = 0;
		break;
	}

	resp.header.size = TC_REPONSE_HEADER_SIZE+resp.size;
	resp.header.responseType = RESULT_CMD;
 	resp.header.spare= (short) 0x0FF05555;
	resp.header.cmd_id = cmd.id;
	resp.header.error_code = reponse;
	return resp;
}

void processMailbox(s_ax25_packet data_ax25) {

	t_tc_response result;
	s_command cmd;

	gv_spino.nbMailboxCommandeReceived++;
	memcpy(&cmd, data_ax25.data, sizeof(s_command));
	result = interpretMailBoxcommand(cmd, data_ax25.header.sourceAdress);
	if (result.header.error_code != SUCCESS) {
		gv_spino.nbMailboxErrorCommandeReceived++;
	}
	s_ax25_packet ax25Frame;

	memcpy(ax25Frame.header.sourceAdress, gv_spinoConfig.spinoDesCallsign, 6);
	ax25Frame.header.ssidSource = SSID_SPINO_MAILBOX;
	memcpy(ax25Frame.header.destinationAdress, data_ax25.header.sourceAdress,
			6);
	ax25Frame.header.ssidDestination = SSID_SPINO_MAILBOX;
	encodeAX25Header(&ax25Frame.header);
	memcpy(ax25Frame.data, &result, TC_REPONSE_HEADER_SIZE + result.size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + result.size);

}
