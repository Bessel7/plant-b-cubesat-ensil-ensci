/*
 * SI4463.h
 *
 *  Created on: 31 août 2022
 *      Author: nats
 */

#ifndef INC_SI4463_H_
#define INC_SI4463_H_

#include <radio_config.h>
#include <stdint.h>

#include "si446x_cmd.h"

#include "stm32l4xx_hal.h"

#define SPI2_TOUT		1000

#define RX_FIFO_THRES	20
#define PACKET_SIZE		240
#define NB_FETCH		PACKET_SIZE/RX_FIFO_THRES

typedef enum {
	IDLE,
	RX_PENDING,
	FETCHING_STATUS,
	FETCHING_SIZE,
	FETCHING_DATA,
	CTS_POLLING,
	STATUS_PENDING,
	STATUS_OK

}  SI44_STATES;

typedef enum {
	FETCH_SIZE,
	FETCH_STATUS,
	FETCH_DATA
}  FETCH_OP;

// Dirty hack for SPI read
#define MAX_SPI_BUFFER_READ		255
static volatile uint8_t zero_buff[MAX_SPI_BUFFER_READ] = { 0 };
static volatile uint8_t vhf_packet[MAX_SPI_BUFFER_READ] = { 0 };

// For faster spi transaction we get an extra byte at the beggining of each fetch
// This accessor directly point toward spi frame start
static volatile uint8_t * vhf_packet_accessor[NB_FETCH];
//static volatile uint8_t vhf_packet_accessor[NB_FETCH];


static volatile uint8_t rx_data[32] = { 0 };
static volatile uint8_t vhf_overflow = 0;

static uint8_t cmdArray[] = {\
		SI446X_PATCH_CMDS, \
		0x07, RF_POWER_UP, \
		0x08, RF_GPIO_PIN_CFG, \
		0x06, RF_GLOBAL_XO_TUNE_2, \
		0x05, RF_GLOBAL_CONFIG_1, \
		0x08, RF_INT_CTL_ENABLE_4, \
		0x08, RF_FRR_CTL_A_MODE_4, \
		0x0D, RF_PREAMBLE_TX_LENGTH_9, \
		0x0A, RF_SYNC_CONFIG_6, \
		0x10, RF_PKT_CRC_CONFIG_12, \
		0x10, RF_PKT_RX_THRESHOLD_12, \
		0x10, RF_PKT_FIELD_3_CRC_CONFIG_12, \
		0x10, RF_PKT_RX_FIELD_1_CRC_CONFIG_12, \
		0x09, RF_PKT_RX_FIELD_4_CRC_CONFIG_5, \
		0x08, RF_PKT_CRC_SEED_31_24_4, \
		0x10, RF_MODEM_MOD_TYPE_12, \
		0x05, RF_MODEM_FREQ_DEV_0_1, \
		0x10, RF_MODEM_TX_RAMP_DELAY_12, \
		0x10, RF_MODEM_BCR_NCO_OFFSET_2_12, \
		0x07, RF_MODEM_AFC_LIMITER_1_3, \
		0x05, RF_MODEM_AGC_CONTROL_1, \
		0x10, RF_MODEM_AGC_WINDOW_SIZE_12, \
		0x0E, RF_MODEM_RAW_CONTROL_10, \
		0x06, RF_MODEM_RAW_SEARCH2_2, \
		0x06, RF_MODEM_SPIKE_DET_2, \
		0x05, RF_MODEM_RSSI_MUTE_1, \
		0x09, RF_MODEM_DSA_CTRL1_5, \
		0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12, \
		0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12, \
		0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12, \
		0x05, RF_PA_TC_1, \
		0x0B, RF_SYNTH_PFDCP_CPFF_7, \
		0x10, RF_MATCH_VALUE_1_12, \
		0x0C, RF_FREQ_CONTROL_INTE_8, \
		0x00 \
	};

void radio_comm_SendCmd(SPI_HandleTypeDef * hspi, uint8_t * data, uint16_t len);
uint8_t radio_comm_GetResp(SPI_HandleTypeDef * hspi, uint8_t * rbuff, uint16_t len);
uint8_t radio_comm_SendCmdGetResp(SPI_HandleTypeDef * hspi, uint8_t * data, uint8_t tx_len, uint8_t * rbuff, uint16_t rx_len, uint8_t waitms);
uint8_t si446x_poll_cts(SPI_HandleTypeDef * hspi);
uint8_t radio_comm_ReadData(SPI_HandleTypeDef * hspi, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
uint8_t config_si4463();
void si446x_power_up(uint8_t BOOT_OPTIONS, uint8_t XTAL_OPTIONS, uint32_t XO_FREQ);
uint8_t si446x_part_info(void);
void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND);
void si446x_gpio_pin_cfg(U8 GPIO0, U8 GPIO1, U8 GPIO2, U8 GPIO3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG);
void si446x_start_rx(U8 CHANNEL, U8 CONDITION, U16 RX_LEN, U8 NEXT_STATE1, U8 NEXT_STATE2, U8 NEXT_STATE3);
void si446x_read_rx_fifo(U8 numBytes, U8* pRxData);
void si446x_fifo_info(U8 FIFO);
void reset_si4463();
void power_vhf(uint8_t b);
void init_rx_irq();
void VHF_InterruptFetch(FETCH_OP op, uint8_t size);
void si446x_cts_it_poll();
void VHF_rstfifo_startrx();


#endif /* INC_SI4463_H_ */
