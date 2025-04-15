/**
 * \file command.c
 * \brief perform treatment of standard SPINO Command
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *
 *  \todo   renforcer l'analyse par verfication de la taille des parametres pour la gestion des commandes 
 */

#include <stdio.h>
#include <string.h>
#include "core/setup.h"
#include "core/command.h"
#include "core/informationMessage.h"
#include "logMngt/log.h"
#include "dropMsgMngt/DropMessage.h"
#include "errorMngt/error.h"
#include "drivers/modem.h"


extern int changeState(short state);
/**
 * \fn int reset()
 * \brief réinitialise le logiciel SPINO 
 *
 * \return  SUCCESS if ok else 
 *          ERROR_COMMAND_NOT_IMPLEMENTED  
 * 
 * \todo : int reset() is to be implemented 
 */

static unsigned char reset() {

	HAL_NVIC_SystemReset();
	return SUCCESS;
}

/**
 * \fn int setValue(t_set_value  value,t_tc_response *resp)
 * \brief set value accordting to information in t_set_value strruture
 * \param value structure with data to set
 * \param *resp output structure with result of command
 * \return  SUCCESS if ok else 
 *          ERROR_COMMAND_NOT_IMPLEMENTED  
 * 
 * \todo : int setValie()  renforcer l'analyse par verfication de la taille des parametres
 */
static unsigned char setValue(const s_set_value value, t_tc_response *resp) {

	unsigned char returnValue = SUCCESS;

	resp->size = 1;
	resp->parameter[0] = value.fied_id;

	switch (value.fied_id) {

	case VALUE_MODEM_TX_MODE:
		gv_spinoConfig.spinoTXModemMode = (unsigned char) value.value[0];
		setModemMode(gv_spinoConfig.spinoTXModemMode);
		break;

	case VALUE_MODEM_TX_MULTIMODE :
		gv_spinoConfig.multimode = (unsigned char) value.value[0];
		setModemMode(gv_spinoConfig.spinoTXModemMode);
		break;

	case VALUE_SPINO_DELAY:
		gv_spinoConfig.telemetryDelay = (unsigned char) value.value[0];
		break;
	case VALUE_DELAY_INFO_MESSAGE:
		memcpy(&gv_spinoConfig.delay_info_message, value.value,
				sizeof(gv_spinoConfig.delay_info_message));
		break;

	case VALUE_ACTIVE_INFO_MESSAGE:
		memcpy(&gv_spinoConfig.info_message_actif, value.value,
				sizeof(gv_spinoConfig.info_message_actif));
		break;

	case VALUE_CALLSIGN_SRC_SPINO:
		memcpy(&gv_spinoConfig.spinoSrcCallsign, value.value, CALLSIGN_SIZE);
		break;

	case VALUE_CALLSIGN_DES_SPINO:
		memcpy(&gv_spinoConfig.spinoDesCallsign, value.value, CALLSIGN_SIZE);
		break;

	case VALUE_CALLSIGN_PAYLOAD_SPINO:
		memcpy(&gv_spinoConfig.payloadCallsign, value.value, CALLSIGN_SIZE);
		break;

	case VALUE_TIMESTAMP:
		logger(LOG_LEVEL_INFO, "Commande VALUE_TIMESTAMP");
		memcpy(&gv_spino.timestamps, value.value, sizeof(gv_spino.timestamps));
		break;

	case VALUE_LOG_LEVEL:
		logger(LOG_LEVEL_INFO, "Commande VALUE_LOG_LEVEL");
		gv_SelectedLogLevel = (unsigned char) value.value[0];
		break;
	default:
		// generation code erreur
		returnValue = ERROR_VALUE_FIELD_UNKNOW;
		break;
	}
	//    memcpy (resp->parameter,&response,resp->size );


	return returnValue;

}
/**
 * \fn int getValue(t_get_value value, t_tc_response *resp)
 * \brief get value accordting to information in t_set_value strruture
 * \param value structure with data to get.
 * \param *resp output structure with result of command
 * \return  SUCCESS if ok else 
 *          ERROR_COMMAND_NOT_IMPLEMENTED  
 * 
 * \todo : int getValue()  renforcer l'analyse par verfication de la taille des parametres
 */
static unsigned char getValue(const s_get_value value, t_tc_response *resp) {
	unsigned char returnValue = SUCCESS;
	s_field response;

	response.field_id = value.field_id;

	switch (value.field_id) {

	case VALUE_SPINO_VERSION:
		response.size = sizeof(gv_version);
		memcpy(response.field_value, &gv_version, response.size);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_SPINO_DELAY:

		response.size = sizeof(gv_spinoConfig.telemetryDelay);
		response.field_value[0] = gv_spinoConfig.telemetryDelay;
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_CALLSIGN_SRC_SPINO:
		response.size = 6;
		memcpy(response.field_value, gv_spinoConfig.spinoSrcCallsign, 6);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_CALLSIGN_DES_SPINO:

		response.size = 6;
		memcpy(response.field_value, gv_spinoConfig.spinoDesCallsign, 6);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_CALLSIGN_PAYLOAD_SPINO:
		response.size = 6;
		memcpy(response.field_value, gv_spinoConfig.payloadCallsign, 6);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_LOG_LEVEL:
		response.size = sizeof(gv_SelectedLogLevel);
		memcpy(response.field_value, &gv_SelectedLogLevel, response.size);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_ACTIVE_INFO_MESSAGE:
		response.size = sizeof(gv_spinoConfig.info_message_actif);
		memcpy(response.field_value, &gv_spinoConfig.info_message_actif,
				response.size);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_DELAY_INFO_MESSAGE:
		response.size = sizeof(gv_spinoConfig.delay_info_message);
		memcpy(response.field_value, &gv_spinoConfig.delay_info_message,
				response.size);
		resp->size = SIZE_HEADER_FIELD + response.size;
		memcpy(resp->parameter, &response, resp->size);
		break;

	case VALUE_TIMESTAMP: /*! Non implemented !*/
	default:
		// generation code erreur
		sprintf(gvLogMsg, "valeur inconnue %d \r\n", value.field_id);
		logger(LOG_LEVEL_CRITICAL, gvLogMsg);
		returnValue = ERROR_VALUE_FIELD_UNKNOW;
		response.size = 0;
		resp->size = SIZE_HEADER_FIELD;
		memcpy(resp->parameter, &response, resp->size);
		break;

	}

	return returnValue;

}
/**
 * \fn t_tc_response interpretcommand(const t_command cmd)
 * \brief interpretCommand
 * \param cmd structure
 * \return  SUCCESS if ok else 
 *          ERROR_COMMAND_NOT_IMPLEMENTED  
 * \todo implementer LOAD PROG
 */

t_tc_response interpretcommand(s_command cmd) {

	t_tc_response resp;
	unsigned char reponse = SUCCESS;


	resp.header.timeStamp = gv_spino.timestamps;


	switch (cmd.id) {
	case CMD_RESET:
		logger(LOG_LEVEL_INFO, "Commande RESET");
		reponse = reset();
		break;
	case SET_VALUE: /*  modify configuration value */
		logger(LOG_LEVEL_INFO, "Commande SET VALUE");
		s_set_value setvalue;
		memcpy(&setvalue, cmd.parameter, cmd.size);
		reponse = setValue(setvalue, &resp);
		//resp.size = 0;
		break;

	case SET_STATE: /*modify mode */
		logger(LOG_LEVEL_INFO, "Commande SET STATE");
		//if (cmd.size == 7) {
			//  gv_spino.currentState = cmd.parameter[0];
			reponse = changeState((int) cmd.parameter[0]);
	//	}
		resp.size = 0;
		break;

	case GET_VALUE: // return value of field

		logger(LOG_LEVEL_INFO, "Commande GET VALUE");
		s_get_value getvalue;
		memcpy(&getvalue, cmd.parameter, cmd.size);
		reponse = getValue(getvalue, &resp);
		break;

	case GET_CONGIG: // return Configuration structure
		logger(LOG_LEVEL_INFO, "Commande GET CONFIG");
		resp.size = sizeof(s_configuration_spino);
		memcpy(resp.parameter, &gv_spinoConfig, sizeof(s_configuration_spino));
		break;

	case PROG_INIT:  // initialise memory prog structure
		logger(LOG_LEVEL_INFO, "Commande PROG INIT");
		gv_prog.indexCourrant = 0;
		for (int i = 0; i < MAX_MEM_PRG; i++) {
			gv_prog.memory[i] = 0;
		}
		resp.size = 0;
		break;

	case PROG_LOAD:
		logger(LOG_LEVEL_INFO, "Commande PROG LOAD");
		s_load_prg load_prg;
		memcpy(&load_prg, cmd.parameter, sizeof(s_load_prg));
		// check index
		if (load_prg.index + MAX_MEM_PRG_LOAD > MAX_MEM_PRG) {
			reponse = ERROR_PROG_INDEX_OUT_OF_BOUND;
		} else if (gv_prog.indexCourrant == load_prg.index) {
			if (memcmp(load_prg.mem1, load_prg.mem2, MAX_MEM_PRG_LOAD) == 0) {
				memcpy(&gv_prog.memory[gv_prog.indexCourrant], load_prg.mem1,
						MAX_MEM_PRG_LOAD);
				gv_prog.indexCourrant += MAX_MEM_PRG_LOAD;

				s_field response;
				response.field_id = PROG_INDEX;
				response.size = sizeof(s_configuration_spino);
				memcpy(resp.parameter, &gv_spinoConfig,
						sizeof(s_configuration_spino));
				resp.size = SIZE_HEADER_FIELD + response.size;
			} else {
				reponse = ERROR_PROG_MEM1_MEM2_NOT_EQUAL;

			}

		} else {
			reponse = ERROR_PROG_INDEX_NOT_EQUAL;
		}
		break;
	case PROG_CHECK:
		logger(LOG_LEVEL_INFO, "Commande PROG CHECK");
		reponse = ERROR_COMMAND_NOT_IMPLEMENTED;
		resp.size = 0;
		break;
	case PROG_SET_ADDRESS:
		logger(LOG_LEVEL_INFO, "Commande SET  ADRESS");
		logger(LOG_LEVEL_CRITICAL, gvLogMsg);
		reponse = ERROR_COMMAND_NOT_IMPLEMENTED;
		resp.size = 0;
		break;
	case GET_LAST_DROPED_MESSAGE:

		logger(LOG_LEVEL_CRITICAL, "Commande GET_LAST_DROPED_MESSAGE");
		reponse = getLastDroppedMessage(&resp);
		break;
	case GET_ALL_DROPED_MESSAGE:
		logger(LOG_LEVEL_INFO, "Commande GET_ALL_DROPED_MESSAGE");
		reponse = getAllDroppedMessage(&resp);
		break;
	case GET_LAST_LOG:

		logger(LOG_LEVEL_CRITICAL, "Commande GET_LAST_LOG");
		reponse = getLastLog(&resp);

		break;
	case GET_ALL_LOG:
		logger(LOG_LEVEL_INFO, "Commande GET_ALL_LOG");
		reponse = getAllLogs(&resp);
		break;

	case SET_INFO_MESSAGE:
		logger(LOG_LEVEL_INFO, "Commande SET_INFO_MESSAGE");
		reponse = setInfoMessage(cmd.parameter, &resp);
		break;
	case DEL_INFO_MESSAGE:
		logger(LOG_LEVEL_INFO, "Commande DEL_INFO_MESSAGE");
		reponse = delInfoMessage(cmd.parameter[0], &resp);
		break;

	default:
		// generation code erreur
		logger(LOG_LEVEL_CRITICAL, "erreur cmd %d ");
		reponse = ERROR_COMMAND_UNKNOW;
		resp.size = 0;

		break;
	}

	if (reponse != SUCCESS) {
		resp.header.error_code = reponse;
		logger(LOG_LEVEL_INFO, "ERREUR COMMAND");
	} else {
		resp.header.error_code = SUCCESS;
	}


	resp.header.size = TC_REPONSE_HEADER_SIZE;
	resp.header.responseType = RESULT_CMD;
//	resp.header.spare= 0x0FF05555;
	resp.header.cmd_id = cmd.id;

	return resp;
}

/**
 * \fn void processCommand(t_ax25_packet data_ax25)
 * \brief process command ax25 packet
 * \param  Ax25 packet
 * \return  SUCCESS if or Error code
 *
 */
void processCommand(s_ax25_packet data_ax25) {

	t_tc_response result;
	s_command cmd;
//	result.header.size = sizeof(s_tm_tc_header);

	gv_spino.nbCommandeReceived++;
	memcpy(&cmd, data_ax25.data, sizeof(s_command));
	if (cmd.key != gv_spino_cmd_key) {
		result.header.size = sizeof(s_tm_tc_header);
//		result.header.spare= 0x0FF05555;
		result.header.responseType = RESULT_CMD;
		result.header.timeStamp = gv_spino.timestamps;
		result.header.cmd_id = cmd.id;
		result.header.error_code = ERROR_COMMAND_WITH_WRONG_KEY;
		result.size = 0;
	} else {
		result = interpretcommand(cmd);
	}

	if (result.header.error_code != SUCCESS) {
		gv_spino.nbCommandeWithError++;
	}
	s_ax25_packet ax25Frame;
	ax25Frame.header = gv_headerTlm;
	result.header.size =result.header.size + result.size;
	memcpy(ax25Frame.data, &result, TC_REPONSE_HEADER_SIZE + result.size);
	/* envoyer la reponse de la  commande */
	writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + result.size);
	sprintf(gvLogMsg, "RESULT COMMAND  %x %x ", result.header.cmd_id,
			result.header.error_code);
	logger(LOG_LEVEL_CRITICAL, gvLogMsg);
}

