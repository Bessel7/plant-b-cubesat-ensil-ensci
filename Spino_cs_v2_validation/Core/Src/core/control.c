/**
 * \file control.c
 * \brief manage spino mode 
 * \author Xtophe
 * \version 0.2
 * \date 01/08/2022
 *  
 */

#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
//#include <windows.h>
#include "core/setup.h"
#include "errorMngt/error.h"
#include "drivers/modem.h"
#include "core/informationMessage.h"
#include "core/control.h"

#define SLEEP_TIME 500  /* sleep time */

extern unsigned short survey();
extern unsigned short digipeater();
extern unsigned short modeMailbox();
extern unsigned short payloadMode();
extern unsigned short experimentalMode();
extern void initExpe();
extern void initPayload();

static unsigned long long lv_spino_timeStampPrevious = 0;
static unsigned int lv_previousTick;

t_tc_response resptlm;


void  setupControl()
{
	lv_spino_timeStampPrevious =0;
	lv_previousTick =0;
}

void inittlm() {
	resptlm.header.responseType = TELEMETRY;
	resptlm.header.error_code = 0;
	resptlm.header.cmd_id = 0;
	resptlm.header.spare = (short) 0x5555AAAA;
	resptlm.header.size = TC_REPONSE_HEADER_SIZE + sizeof(s_globalVariable) ;
	resptlm.size = sizeof(s_globalVariable);
}

void sendTLMWithoutDelay()
{
	s_ax25_packet ax25Frame;
		lv_spino_timeStampPrevious = gv_spino.timestamps;
		resptlm.header.timeStamp = gv_spino.timestamps;
		memcpy(resptlm.parameter, &gv_spino, sizeof(s_globalVariable));
		ax25Frame.header = gv_headerTlm;
		memcpy(ax25Frame.data, &resptlm, TC_REPONSE_HEADER_SIZE + resptlm.size);
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resptlm.size);
		logger(LOG_LEVEL_INFO, "Envoie TLM");
}
static void sendTLM() {
	if (gv_spino.timestamps
			> (lv_spino_timeStampPrevious + gv_spinoConfig.telemetryDelay * 1000)) {

		s_ax25_packet ax25Frame;
		lv_spino_timeStampPrevious = gv_spino.timestamps;
		resptlm.header.timeStamp = gv_spino.timestamps;
		memcpy(resptlm.parameter, &gv_spino, sizeof(s_globalVariable));
		ax25Frame.header = gv_headerTlm;
		memcpy(ax25Frame.data, &resptlm, TC_REPONSE_HEADER_SIZE + resptlm.size);

		if(gv_spinoConfig.multimode==0 )
		{
		writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resptlm.size);
		logger(LOG_LEVEL_INFO, "Envoie TLM");
		}
		else
		{
			uint8_t j=0;

			for(j=0; j<MAX_MODE;j++)
			{
				setModemMode(j );
				writeData(ax25Frame, TC_REPONSE_HEADER_SIZE + resptlm.size);
				HAL_Delay(DELAY_MULTIMODE);
			}
			setModemMode(gv_spinoConfig.spinoTXModemMode );
		}

	}



}

unsigned char changeState(int state) {

	unsigned char response = SUCCESS;

	switch (state) {

	case STATE_INIT:
		gv_spino.currentState = STATE_INIT;
		break;
	case STATE_SURVEY:
		gv_spino.currentState = STATE_SURVEY;
		break;

	case STATE_DIGIPEATER:
		if (gv_spino.currentState == STATE_SURVEY) {
			gv_spino.currentState = STATE_DIGIPEATER;
		} else {
			logger(LOG_LEVEL_CRITICAL, "ERROR_WRONG_STATE");
			response = ERROR_WRONG_STATE;
		}
		break;

	case STATE_MAILBOX:
		if (gv_spino.currentState == STATE_SURVEY) {
			gv_spino.currentState = STATE_MAILBOX;
		} else {
			logger(LOG_LEVEL_CRITICAL, "ERROR_WRONG_STATE");
			response = ERROR_WRONG_STATE;
		}
		break;

	case STATE_EXPE_DATA:
		if (gv_spino.currentState == STATE_SURVEY) {
			gv_spino.currentState = STATE_EXPE_DATA;
			initExpe();
		} else {
			logger(LOG_LEVEL_CRITICAL, "ERROR_WRONG_STATE");
			response = ERROR_WRONG_STATE;
		}
		break;

	case STATE_MAIN_PAYLOAD:
		if (gv_spino.currentState == STATE_SURVEY) {
			gv_spino.currentState = STATE_MAIN_PAYLOAD;
		} else {
			logger(LOG_LEVEL_CRITICAL, "ERROR_WRONG_STATE");
			response = ERROR_WRONG_STATE;
		}
		break;

	default:
		/* remplacer par reinit */

		logger(LOG_LEVEL_CRITICAL, "ERROR_WRONG_STATE");
		response = ERROR_WRONG_STATE;
		break;
	}

	return response;
}

void control() {
//	inittlm();

//	while (1) {
		switch (gv_spino.currentState) {

		case STATE_INIT:

			logger(LOG_LEVEL_INFO, "STATE INIT");
			setupGlobalVariable();
			break;
		case STATE_SURVEY:
			logger(LOG_LEVEL_INFO, "STATE SURVEY");
			gv_spino.currentState = survey();
			break;

		case STATE_DIGIPEATER:
			logger(LOG_LEVEL_INFO, "STATE DIGIPEATER");
			gv_spino.currentState = digipeater();
			break;

		case STATE_MAILBOX:

			logger(LOG_LEVEL_INFO, "STATE MAILBOX");
			gv_spino.currentState = modeMailbox();
			break;

		case STATE_EXPE_DATA:
			logger(LOG_LEVEL_INFO, "STATE EXPE");
			gv_spino.currentState = experimentalMode();
			break;

		case STATE_MAIN_PAYLOAD:
			logger(LOG_LEVEL_INFO, "STATE_MAIN_PAYLOAD");
			initPayload();
			gv_spino.currentState = payloadMode();
			break;

		default:
			/* remplacer par reinit */
			sprintf(gvLogMsg, "State default %d \r\n", gv_spino.currentState);
			logger(LOG_LEVEL_CRITICAL, "STATE DEFAULT");
			gv_spino.currentState = STATE_INIT;
			gv_spino.lastResetCause = RESET_CAUSE_STATE_UNKNOWN;
			break;
		}


		uint32_t tim =  HAL_GetTick();

		int deltaTime = tim - lv_previousTick;
		if (deltaTime <0)
		{
			deltaTime = tim + (0xFFFFFFFF - lv_previousTick);
		}
		lv_previousTick = tim;

//		Sleep(SLEEP_TIME);
		gv_spino.timestamps = gv_spino.timestamps + deltaTime;
		sendTLM();
		sendInfoMessage();
//	}

}
