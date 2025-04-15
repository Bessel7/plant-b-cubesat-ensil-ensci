/**
 * \file setup.c
 * \brief  initialise all global variables needed for SPINO embeded software.
 * \author Xtophe
 * \version 0.1
 * \date 01/08/2022
 * 
 * \todo  [ ] split set in different function
 * \todo  [ ] Set define for number and Callsign 
 * 
 */

/*=========  INCLUDES ==========================================================================*/

#include <string.h>

#include "core/setup.h"
#include "dropMsgMngt/DropMessage.h"
#include "core/informationMessage.h"
#include "core/control.h"
#include "core/informationMessage.h"

extern void initExpe();
extern void  setupControl();

/*=========  GLOBAL VARIABLES  ===================================================================*/

s_configuration_spino gv_spinoConfig;
s_globalVariable gv_spino;
s_ax25_header gv_headerTlm;
s_unprocessedmessageList gv_unprocess_messages;
s_prog_mngt gv_prog;

unsigned char gv_data_rx[SIZE_RCV_DATA_MAX];


char mem_prg[MAX_MEM_PRG];

unsigned short gv_version = (unsigned short) 0x0104;

unsigned short gv_spino_cmd_key = SPINO_CMD_KEY;

/*=========  FUNCTIONS  ========================================================================*/

/*---------------------------------------------------------------------------------------------------------
 *
 *  setupGlobalVariable :  
 *  \brief       Setup SPINO  global variable 
 *  \details
 *  \param    none
 *  \return void
 * 
 ---------------------------------------------------------------------------------------------------------*/
/**
 * \fn void setupGlobalVariable(void)
 * \brief initialise all global variable 
 *
 */

void setupGlobalVariable() {

	char cs[6] = { 'S', 'P', 'I', 'N', 'O', 'S' };
	char cd[6] = { 'S', 'P', 'I', 'N', 'O', 'D' };
	char lt[6] = { 'L', 'A', 'T', 'M', 'O', 'S' };
	memcpy(gv_spinoConfig.spinoSrcCallsign, cs, 6);
	memcpy(gv_spinoConfig.spinoDesCallsign, cd, 6);
	memcpy(gv_spinoConfig.payloadCallsign, lt, 6);
	gv_spino.timestamps =0;
	gv_spino.nbCommandeReceived =0;
	gv_spino.nbCommandeWithError = 0;
	gv_spino.lastResetCause =0;
	gv_spino.nbDigipeaterMesssageProcessed =0;
	gv_spino.nbFrameNotprocessed = 0;
	gv_spino.currentState = STATE_SURVEY;
	gv_spino.nbExperiemntalCommand=0;
	gv_spino.nbExperiemntalCommandError=0;
	gv_spino.nbMailboxCommandeReceived = 0;
	gv_spino.nbMailboxErrorCommandeReceived = 0;
	gv_spino.nbPayloadCommandeWithError=0;
	gv_spino.nbPayloadCommandeReceived=0;
	gv_spino.spare = 0xFF00;
	gv_spino.gv_nb_I2CMAXDATA =0;


	gv_spinoConfig.telemetryDelay = 20;
	gv_spinoConfig.spinoTxFrequency = 435000;
	gv_spinoConfig.spinoTXModemMode = 0;
	gv_spinoConfig.spinoTxModemSpeed = 1200;
	gv_spinoConfig.spinoRxFrequency = 145000;
	gv_spinoConfig.spinoRXModemMode = 0;
	gv_spinoConfig.spinoRxModemSpeed = 9600;
	gv_spinoConfig.delay_info_message = 200;
	gv_spinoConfig.info_message_actif = INFO_MSG_NOT_USED;
	gv_spinoConfig.multimode=0;

	gv_headerTlm.pid = 255;

	convertToAX25Header(&gv_headerTlm, gv_spinoConfig.spinoDesCallsign,
			gv_headerTlm.ssidDestination, gv_spinoConfig.spinoSrcCallsign,
			gv_headerTlm.ssidSource);

	gv_prog.indexCourrant = 0;
	gv_prog.memory = mem_prg;


	gv_unprocess_messages.index = 0;
	int i = 0;
	for (i = 0; i < MAX_UNPROCESSED_MESSAGE_LIST_SIZE; i++) {

		gv_unprocess_messages.message[i].timestamps = 0;
		gv_unprocess_messages.message[i].size = 0;
	}

	setupInfoMessage();
	initExpe();
    setupControl();

}
