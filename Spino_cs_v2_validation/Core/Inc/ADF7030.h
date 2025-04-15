/*
 * ADF7030.h
 *
 *  Created on: 24 sept. 2022
 *      Author: f4hda
 */

#ifndef INC_ADF7030_H_
#define INC_ADF7030_H_



/*
 ***********   Local snapshot of key parameters   ***********
 *
 *            Please update these values according
 *                    to cfg file evolutions...
 */

// Part of GENERIC_PKT_BUFF_CFG0 :
#define PTR_TX_BASE						0
//Tx base buffer offset pointer. The base address of the Tx payload is 0x2000000 + (PTR_TX_BASE × 4).

// Part of GENERIC_PKT_BUFF_CFG1 :
#define PAYLOAD_SIZE					240
//should be a multiple of 4

#define SYNC_WORD						0x2EFC9827


void config_ADF7030(uint8_t mode);
void init_uhf_txbuffer(void);
uint8_t adf_tx_data(void);
uint8_t adf_read_temp(float* temp);

void adf_test_cw_mode(void);
void adf_test_cw_off(void);





#endif /* INC_ADF7030_H_ */
