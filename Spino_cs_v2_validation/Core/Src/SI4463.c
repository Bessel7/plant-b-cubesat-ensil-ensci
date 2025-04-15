/*
 * SI4463.c
 *
 *  Created on: 31 août 2022
 *      Author: nats
 */

#include "main.h"
#include "SI4463.h"

extern SPI_HandleTypeDef hspi2;

void radio_comm_SendCmd(SPI_HandleTypeDef * hspi, uint8_t * data, uint16_t len) {
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	//HAL_Delay(10);
	HAL_SPI_Transmit(hspi, data, len, SPI2_TOUT);
	//HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
}

uint8_t radio_comm_GetResp(SPI_HandleTypeDef * hspi, uint8_t * rbuff, uint16_t len) {
	uint8_t err_cnt = 10;

	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);

	uint8_t cts_cmd[2] = { 0x44, 0x00 };
	uint8_t ctsbuff[2] = { 0 };

	uint8_t i = 0;
	for(i = 0; i <= err_cnt; i++) {
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(hspi, cts_cmd, ctsbuff, 2, SPI2_TOUT);
		if(ctsbuff[1] != 0xFF) {
			HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
			HAL_Delay(1);
		} else {
			break;
		}
	}

	if(err_cnt <= i) {
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
		return 0;
	}

	if(len != 0) {
		HAL_SPI_TransmitReceive(hspi, zero_buff, rbuff, len, SPI2_TOUT);
	}
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
	return ctsbuff[1];
}

uint8_t radio_comm_SendCmdGetResp(SPI_HandleTypeDef * hspi, uint8_t * data, uint8_t tx_len, uint8_t * rbuff, uint16_t rx_len, uint8_t waitms) {
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, data, tx_len, SPI2_TOUT);
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);

	HAL_Delay(waitms);

	return radio_comm_GetResp(hspi, rbuff, rx_len);
}

uint8_t si446x_poll_cts(SPI_HandleTypeDef * hspi) {
	uint8_t err_cnt = 10;

	uint8_t cts_cmd[2] = { 0x44, 0x00 };
	uint8_t ctsbuff[2] = { 0 };

	while(ctsbuff[1] != 0xFF && err_cnt != 0) {
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive(hspi, cts_cmd, ctsbuff, 2, SPI2_TOUT);
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
		err_cnt--;
	}

	if(err_cnt == 0) {
		return 0;
	}
	return 1;
}

uint8_t radio_comm_ReadData(SPI_HandleTypeDef * hspi, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	if(pollCts) {
		if(!si446x_poll_cts(hspi)) {
			return 0;
		}
	}
	uint8_t cmd_back = 0;
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(hspi, &cmd, &cmd_back, 1, SPI2_TOUT);
	HAL_SPI_TransmitReceive(hspi, zero_buff, pData, byteCount, SPI2_TOUT);
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
	return 1;
}

uint8_t config_si4463() {
	uint16_t dataI = 0;
	while(cmdArray[dataI] != 0) {
		uint8_t *dataP = &cmdArray[dataI+1];
		uint8_t dataLen = cmdArray[dataI];

		if(dataLen > 16) {
			return SI446X_COMMAND_ERROR;
		}

		if(radio_comm_SendCmdGetResp(&hspi2, dataP, dataLen, 0, 0, 10) != 0xFF) {
			return SI446X_CTS_TIMEOUT;
		}
		dataI += dataLen+1;
	}
	return SI446X_SUCCESS;
}

void si446x_power_up(uint8_t BOOT_OPTIONS, uint8_t XTAL_OPTIONS, uint32_t XO_FREQ)
{
	uint8_t Pro2Cmd[7];

    Pro2Cmd[0] = SI446X_CMD_ID_POWER_UP;
    Pro2Cmd[1] = BOOT_OPTIONS;
    Pro2Cmd[2] = XTAL_OPTIONS;
    Pro2Cmd[3] = (uint8_t)(XO_FREQ >> 24);
    Pro2Cmd[4] = (uint8_t)(XO_FREQ >> 16);
    Pro2Cmd[5] = (uint8_t)(XO_FREQ >> 8);
    Pro2Cmd[6] = (uint8_t)(XO_FREQ);

    radio_comm_SendCmd(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_POWER_UP);
}

uint8_t si446x_part_info(void)
{
	uint8_t Pro2Cmd[8];

    Pro2Cmd[0] = SI446X_CMD_ID_PART_INFO;
    Pro2Cmd[1] = 0;
    Pro2Cmd[2] = 0;
    Pro2Cmd[3] = 0;
    Pro2Cmd[4] = 0;
    Pro2Cmd[5] = 0;
    Pro2Cmd[6] = 0;
    Pro2Cmd[6] = 0;

    radio_comm_SendCmdGetResp(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_PART_INFO,
                              Pro2Cmd,
                              SI446X_CMD_REPLY_COUNT_PART_INFO, 0);

    return Pro2Cmd[0];
}

void si446x_get_int_status(uint8_t PH_CLR_PEND, uint8_t MODEM_CLR_PEND, uint8_t CHIP_CLR_PEND)
{
	uint8_t Pro2Cmd[8];

    Pro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;
    Pro2Cmd[1] = PH_CLR_PEND;
    Pro2Cmd[2] = MODEM_CLR_PEND;
    Pro2Cmd[3] = CHIP_CLR_PEND;
    Pro2Cmd[4] = 0;
    Pro2Cmd[5] = 0;
    Pro2Cmd[6] = 0;
    Pro2Cmd[7] = 0;

    radio_comm_SendCmdGetResp(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_GET_INT_STATUS,
                              Pro2Cmd, SI446X_CMD_REPLY_COUNT_GET_INT_STATUS, 0);
}

void si446x_gpio_pin_cfg(U8 GPIO0, U8 GPIO1, U8 GPIO2, U8 GPIO3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG)
{
	uint8_t Pro2Cmd[8];
    Pro2Cmd[0] = SI446X_CMD_ID_GPIO_PIN_CFG;
    Pro2Cmd[1] = GPIO0;
    Pro2Cmd[2] = GPIO1;
    Pro2Cmd[3] = GPIO2;
    Pro2Cmd[4] = GPIO3;
    Pro2Cmd[5] = NIRQ;
    Pro2Cmd[6] = SDO;
    Pro2Cmd[7] = GEN_CONFIG;

    radio_comm_SendCmdGetResp(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_GPIO_PIN_CFG,
                              Pro2Cmd, SI446X_CMD_REPLY_COUNT_GPIO_PIN_CFG, 0 );
}

void si446x_start_rx(U8 CHANNEL, U8 CONDITION, U16 RX_LEN, U8 NEXT_STATE1, U8 NEXT_STATE2, U8 NEXT_STATE3)
{
	uint8_t Pro2Cmd[8];
    Pro2Cmd[0] = SI446X_CMD_ID_START_RX;
    Pro2Cmd[1] = CHANNEL;
    Pro2Cmd[2] = CONDITION;
    Pro2Cmd[3] = (U8)(RX_LEN >> 8);
    Pro2Cmd[4] = (U8)(RX_LEN);
    Pro2Cmd[5] = NEXT_STATE1;
    Pro2Cmd[6] = NEXT_STATE2;
    Pro2Cmd[7] = NEXT_STATE3;

    radio_comm_SendCmd(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_START_RX);
}

void si446x_read_rx_fifo(U8 numBytes, U8* pRxData)
{
  radio_comm_ReadData(&hspi2, SI446X_CMD_ID_READ_RX_FIFO, 0, numBytes, pRxData );
}

void si446x_fifo_info(U8 FIFO)
{
	uint8_t Pro2Cmd[2];
    Pro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;
    Pro2Cmd[1] = FIFO;

    radio_comm_SendCmdGetResp(&hspi2, Pro2Cmd, SI446X_CMD_ARG_COUNT_FIFO_INFO,
                                  Pro2Cmd, SI446X_CMD_REPLY_COUNT_FIFO_INFO, 0 );

}

void reset_si4463() {
	HAL_GPIO_WritePin(GPIOC, VHF_SDN_Pin, 1);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, VHF_SDN_Pin, 0);
}

void power_vhf(uint8_t b) {
	if(b) {
		HAL_GPIO_WritePin(GPIOC, VHF_PWR_EN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
		HAL_Delay(500);
	} else {
		HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, VHF_PWR_EN_Pin, GPIO_PIN_RESET);
		HAL_Delay(500);
	}
}

void init_rx_irq() {
	// Clear EXTI0 interrupt before arming
	//HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_All);
	// PB0 interrupt is linked to EXTI0
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void VHF_InterruptFetch(FETCH_OP op, uint8_t size) {
	// Chip Select is driven through the state
	// yes it's ugly
	HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);

	switch(op) {
		/*case FETCH_SIZE:
			uint8_t fifo_info[SI446X_CMD_ARG_COUNT_FIFO_INFO] = { SI446X_CMD_ID_FIFO_INFO,
			HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, 1);
			break;*/
		case FETCH_DATA:
			zero_buff[0] = SI446X_CMD_ID_READ_RX_FIFO;
			HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, size);
			break;
		case FETCH_STATUS:
			zero_buff[0] = SI446X_CMD_ID_GET_INT_STATUS;
			zero_buff[1] = 0;
			zero_buff[2] = 0;
			zero_buff[3] = 0;
			HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, size);
			break;
	}

}

void si446x_cts_it_poll() {
	  zero_buff[0] = 0x44;
	  zero_buff[1] = 0x00;
	  HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	  HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, 2);
}

void VHF_rstfifo_startrx() {
	// Reset FIFO count
	vhf_overflow = 0;
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_FIFO_RX_BIT);

	si446x_start_rx(0, 0, 240,
		  SI446X_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_RX,
		  SI446X_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
		  SI446X_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_READY );
	// Start RX, if timeout stay in RX, if packet received goes back to READY, (third is CRC error but we don't use CRC)
}
