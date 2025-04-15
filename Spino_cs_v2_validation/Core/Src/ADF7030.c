/*
 * ADF7030.c
 *
 *  Created on: 25 juil. 2022
 *      Author: nats
 */
#include "main.h"
#include "ADF7030.h"

/*
 * All ADF7030 function are blocking because TX
 * spi transfer can be interrupted
 */

/*
 * ADF7030 format
 * MSB = 1 : radio command
 * MSB = 0 : Memory command
 * UG1002 page 14
 */

#define ADF_PHY_OFF		0x81
#define ADF_PHY_ON		0x82
#define ADF_PHY_TX		0x84
#define ADF_PHY_CONF	0x85
#define ADF_PHY_MON		0x8A

#define ADF_NOP			0xFF


//ADF7030_STATES
#define STATE_PHY_SLEEP		0x00
#define STATE_PHY_OFF		0x01
#define STATE_PHY_ON		0x02
#define STATE_PHY_RX		0x03
#define STATE_PHY_TX		0x04
#define STATE_PHY_CONF		0x05
#define STATE_PHY_CCA		0x06
#define STATE_PHY_CAL		0x09
#define STATE_PHY_MON		0x0A

#define RETRY_END_TX		20
#define RETRY_END_MON		100

/*
 * For this set of memory access command see UG-1002 p. 18
 */
#define ADF_READ_REG32_CMD		0x78
#define ADF_WRITE_REG32_CMD		0x38

/*
 * ADF7030 Flags
 */
#define ADF_CMD_RDY_FLAG		0x20

/*
 * GPIO Configuration register
 */
#define PROFILE_RADIO_DIG_TX_CFG1		0x20000308
#define ADDR_GENERIC_PKT_TEST_MODES0	0x20000548
#define PROFILE_GPCON4_7				0x20000398
#define MISC_FW							0x400042B4

#define PACKET_MEM_BASE_ADDR			0x20000AF0

//MONITOR READBACK REGISTER
#define PROFILE_MONITOR1				0x2000038C

// Trying something crappy with a lot of enthusiasm and hope
#define CRAP_FAST_REG_GPIO4_7			0x40000800

#define EXT_PA_PIN_SEL_MASK				0x000E0000
#define EXT_PA_PIN_SEL_SHIFT			17
#define EXT_PA_FRAMING_EN_MASK			0x00010000

#define POS_TX_TEST						16
#define MASK_TX_TEST					0x000F0000

#define TEST_DISABLED					0x00
#define TEST_TX_CARRIER					0x01
#define TEST_TX_PREAMBLE_PATTERN		0x06

#define GPIO6							6
#define GPIO7							7

#define GPIO6_SHIFT						16
#define GPIO7_SHIFT						24

#define GPIO7_OUTPUT					0x1F
#define GPIO6_OUTPUT					0x1E

#define GPIO6_MASK						0x3F0000
#define GPIO7_MASK						0x3F000000

#define SPI3_TOUT		1000

uint8_t ar_conf_adf7030_mode0[] = {
	#include "spino_ADF_conf_2400_FSK.cfg"
};

uint8_t ar_conf_adf7030_mode1[] = {
	#include "spino_ADF_conf_9600_GFSK.cfg"
};
uint8_t ar_conf_adf7030_mode2[] = {
	#include "spino_ADF_conf_10800_4GFSK.cfg"
};

uint8_t ar_conf_adf7030_mode3[] = {
	#include "spino_ADF_conf_12800_4GFSK.cfg"
};

/********* WARNING 1 *********
 *
 * ISSUE IN AUTO ADI GONF GEN TOOL
 * A 128 bytes max TX length is configured
 * independently of the packet size...
 * We need 260bytes for this project
 *
 * MODIFY THE CONF SECTION ACCORDINGLY
 */
/*
0x04010080

 ? ?  ?
 | |  |
0000 0100 0000 0010 0000 1001 0000 0100 -> 0x040209C4
| |  | |         |          |----------
| |  | |         |          RX_SIZE (220)
| |  | |         |----------
| |  | |         TX_SIZE (260)
| |  | |---------
| |  | TRX_BLOCK_SIZE
| |  Tx raw mode is disabled
| Return to PHY_ON following Rx.
Transition to PHY_ON on completion of Tx.
*/
/***** CONF FILE SECTION MODIFICATION EXAMPLE ****/
/* 0x200004F4 */
/*0x00, 0x00, 0x74, 0x38, 0x20, 0x00, 0x04, 0xF4,*/
/*0x00, 0x15, 0xE3, 0x06, 0x04, 0x02, 0x09, 0xC4,*/
/*                        ---------------------- */

/********* WARNING 2 *********
 *
 * ISSUE IN AUTO ADI GONF GEN TOOL
 * Bitrate is actually symbol rate...
 * For 4(G)FSK, divide value by 2
 *
 */


extern SPI_HandleTypeDef hspi3;

void SPI_Write(SPI_HandleTypeDef * hspi, uint8_t * data, uint16_t len) {
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_SPI_Transmit(hspi, data, len, SPI3_TOUT);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_SET);
}

void SPI_ReadWrite(SPI_HandleTypeDef * hspi, uint8_t * data, uint8_t * rbuff, uint16_t len) {
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(hspi, data, rbuff, len, SPI3_TOUT);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_SET);
}

void adf_check_ready(void)
{
	uint8_t tx_data = ADF_NOP;
	uint8_t rx_buff = 0;

	do {
		SPI_ReadWrite(&hspi3, &tx_data, &rx_buff, 1);
	} while((rx_buff & ADF_CMD_RDY_FLAG) == 0);
}

uint32_t adf_read_reg(uint32_t reg_addr)
{
	uint8_t tx_buff[11];
	uint8_t rx_buff[11];
	uint8_t length = 0;

	tx_buff[length++] = ADF_READ_REG32_CMD;
	tx_buff[length++] = (reg_addr >> 24) & 0xFF;
	tx_buff[length++] = (reg_addr >> 16) & 0xFF;
	tx_buff[length++] = (reg_addr >> 8) & 0xFF;
	tx_buff[length++] = reg_addr & 0xFF;
	tx_buff[length++] = 0xFF; //wait
	tx_buff[length++] = 0xFF; //wait
	tx_buff[length++] = 0xFF; //read data
	tx_buff[length++] = 0xFF; //read data
	tx_buff[length++] = 0xFF; //read data
	tx_buff[length++] = 0xFF; //read data

	adf_check_ready();

	SPI_ReadWrite(&hspi3, tx_buff, rx_buff, length);

	uint32_t rval = ((uint32_t) rx_buff[7] << 24) |  ((uint32_t) rx_buff[8] << 16) |  (uint32_t) (rx_buff[9] << 8) | ((uint32_t) rx_buff[10]);

	return rval;
}

void adf_write_reg(uint32_t reg_addr, uint32_t reg_val)
{
	uint8_t tx_buff[9];
	uint8_t length = 0;

	tx_buff[length++] = ADF_WRITE_REG32_CMD;
	tx_buff[length++] = (uint8_t) ((reg_addr >> 24) & 0xFF);
	tx_buff[length++] = (uint8_t) ((reg_addr >> 16) & 0xFF);
	tx_buff[length++] = (uint8_t) ((reg_addr >> 8) & 0xFF);
	tx_buff[length++] = (uint8_t) (reg_addr & 0xFF);
	tx_buff[length++] = (uint8_t) ((reg_val >> 24) & 0xFF);
	tx_buff[length++] = (uint8_t) ((reg_val >> 16) & 0xFF);
	tx_buff[length++] = (uint8_t) ((reg_val >> 8) & 0xFF);
	tx_buff[length++] = (uint8_t) (reg_val & 0xFF);

	adf_check_ready();

	SPI_Write(&hspi3, tx_buff, length);
}

void adf_conf_pin() {
	uint32_t reg_val = 0;

	reg_val = adf_read_reg(PROFILE_GPCON4_7);

	uint32_t gp6_mask = GPIO6_MASK;
	uint32_t gp7_mask = GPIO7_MASK;

	reg_val = (reg_val & ~gp6_mask) | (reg_val & ~gp7_mask);
	reg_val = reg_val | GPIO7_OUTPUT << GPIO7_SHIFT | GPIO6_OUTPUT << GPIO6_SHIFT;

	adf_write_reg(PROFILE_GPCON4_7, reg_val);

}

void adf_test_cw_mode(void)
{
	uint32_t reg_val;

	// Config GPIO7 to drive PA while tx is on
	reg_val = adf_read_reg(PROFILE_RADIO_DIG_TX_CFG1);
//	reg_val = (reg_val & ~EXT_PA_PIN_SEL_MASK) | (GPIO7 << EXT_PA_PIN_SEL_SHIFT) | EXT_PA_FRAMING_EN_MASK;
	reg_val = (reg_val & ~EXT_PA_PIN_SEL_MASK) | ((uint32_t)GPIO6 << EXT_PA_PIN_SEL_SHIFT) | EXT_PA_FRAMING_EN_MASK;
	adf_write_reg(PROFILE_RADIO_DIG_TX_CFG1, reg_val);

	// Activate test mode
	reg_val = adf_read_reg(ADDR_GENERIC_PKT_TEST_MODES0);
	reg_val = (reg_val & ~MASK_TX_TEST) | ((uint32_t) TEST_TX_CARRIER << POS_TX_TEST);
	adf_write_reg(ADDR_GENERIC_PKT_TEST_MODES0, reg_val);

	uint8_t on = ADF_PHY_ON;
	uint8_t tx = ADF_PHY_TX;

	SPI_Write(&hspi3, &on, 1);

	HAL_Delay(10);

	SPI_Write(&hspi3, &tx, 1);

}

void adf_test_cw_off(void)
{
	uint32_t regVal;

	uint8_t on = ADF_PHY_ON;
	uint8_t off = ADF_PHY_OFF;

	SPI_Write(&hspi3, &on, 1);

	HAL_Delay(10);

	SPI_Write(&hspi3, &off, 1);

	//unconfig test mode
	regVal = adf_read_reg(ADDR_GENERIC_PKT_TEST_MODES0);
	regVal = (regVal & ~MASK_TX_TEST) | ((uint32_t) TEST_DISABLED << POS_TX_TEST);
	adf_write_reg(ADDR_GENERIC_PKT_TEST_MODES0, regVal);
}

uint8_t adf_send_confblob(uint8_t mode) {

    uint32_t array_position = 0;

    uint8_t* ar_conf_adf7030=NULL;

    uint8_t nb_step = 11;
    uint8_t cur_step = 0;

    switch ( mode )
    {
        case 0:
        	ar_conf_adf7030=ar_conf_adf7030_mode0;
            break;
        case 1:
        	ar_conf_adf7030=ar_conf_adf7030_mode1;
            break;
        case 2:
            ar_conf_adf7030=ar_conf_adf7030_mode2;
            break;
        case 3:
            ar_conf_adf7030=ar_conf_adf7030_mode3;
            break;
        default:
        	ar_conf_adf7030=ar_conf_adf7030_mode0;
    }

    do
    {
      // Calculate the number of bytes to write
      uint32_t length =  (*(ar_conf_adf7030 + array_position ) << 16) |
                         (*(ar_conf_adf7030 + array_position + 1) << 8) |
                         (*(ar_conf_adf7030 + array_position + 2));

      if(length > 0xFFFF)
      {
         return 0;
      }

      // Write the SPI data pointed to location (MEMORY_FILE + array_position) with specified length (length)
      uint8_t * pSeqData = (ar_conf_adf7030 + array_position + 3);

      SPI_Write(&hspi3, pSeqData, length-3);

      // Update the array position to point to the next block
      array_position += length;
      cur_step++;

    } while(cur_step < nb_step); //while(array_position < size); // Continue operation until full data file has been written

    return 1;
}

void config_ADF7030(uint8_t mode) {

	// Power the UHF rail
	HAL_GPIO_WritePin(GPIOA, UHF_PWR_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, UHF_FE_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(50);

	// Reset ADF7030
	HAL_GPIO_WritePin(GPIOA, UHF_NRST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, UHF_NRST_Pin, GPIO_PIN_SET);

	// Need to wait the MISO signal before starting
	// Need to add a max retry to avoid being stuck
	uint8_t started = 0;
	uint8_t tx = 0xFF;

	do {
		SPI_ReadWrite(&hspi3, &tx, &started, 1);
	} while (!started);

	adf_check_ready();

	// Put the ADF7030 in "OFF State" see DS p44
	uint8_t off = ADF_PHY_OFF;
	SPI_Write(&hspi3, &off, 1);
	// Set GPIO
	adf_conf_pin();

	adf_check_ready();

	// Send configuration file
	adf_send_confblob(mode);

	adf_check_ready();

	// Issue CMD_CFG_DEV
	uint8_t conf = ADF_PHY_CONF;
	SPI_Write(&hspi3, &conf, 1);
	adf_check_ready();

	// Got back to "OFF State"
	//HAL_SPI_Transmit(&hspi3, ADF_PHY_OFF, 1, SPI3_TOUT);

}

uint8_t adf_get_status() {
	uint8_t rx_data[2];
	uint8_t tx_data[] = { 0xFF, 0xFF }; // Send NOP to get status byte
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_SPI_TransmitReceive(&hspi3, tx_data, rx_data, 2, SPI3_TOUT);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, UHF_SPI_NSS_Pin, GPIO_PIN_SET);

	return (rx_data[1] >> 5) & 0x01;
}


/************************ crap from the adf7030 TX experimenter *************
*********************** Should become a driver at some point... *************
*
*  Created on: 24 sept. 2022
*      Author: f4hda
*/

/*
Guessed from ADI material:
Generic packet configuration is in the 0x2000 04F4 to 0x2000 060C region (280bytes).
Packet memory (for the buffers) are in the 0x2000 0AF0 to 0x2000 0FFC region (1292bytes).

Retrieved from UG-1002, p27:
<< The host programs the ADF7030-1 to transmit and receive variable or fixed length packets.
In preparation for transmitting a packet, the host writes payload data into the ADF7030-1
Tx payload buffer and configures programmable fields in the generic packet memory region.
The ADF7030-1 transmits a packet in response to a CMD_PHY_TX command. >>

<< The ADF7030-1 packet memory is reserved for Rx and Tx payload buffers.
The host configures a Tx payload buffer and an Rx payload buffer within the packet memory. >>

<< The start locations of the Rx and Tx payload buffers are set via the PTR_RX_BASE and
PTR_TX_BASE bits, respectively, in the GENERIC_PKT_BUFF_CFG0 register.
By default, these fields both point to the start of the packet memory.
The sizes of the Rx and Tx payload buffers are set via the RX_SIZE and TX_SIZE bits,
respectively, in the GENERIC_PKT_BUFF_CFG1 register.
By default, these bits are both 256 bytes.
The host is responsible for ensuring that the Rx and Tx payload buffers do not extend outside
the ADF7030-1 packet memory.
The maximum size of the Tx payload buffer is 511 bytes.
The maximum size of the Rx payload buffer is 511 bytes.
See the Rolling Buffers Mode section for payloads greater than 511 bytes. >>

For generic packet configuration structure, see p29
*/

uint8_t uhf_txbuffer[PAYLOAD_SIZE];

void adf_read_state(uint8_t* err_code,uint8_t* currr_state,uint8_t* status)
{
	uint32_t value=0;
	value=adf_read_reg(MISC_FW);
	*err_code=(value >> 24) & 0xFF;
	*currr_state=(value >> 8) & 0x1F;
	*status=value & 0x03;
}

uint8_t adf_read_temp(float* temp)
{
	//2022_09_28 :	WARNING ! Bug in "adf_read_temp" function..
	//				State machine behaves as described in UG-1002
	//				but PROFILE_MONITOR1 register readback is
	//				always ZERO !!!
	//				Note :	ADF7030 stays in the MONITORING state
	//						for a LOOOOOONG time (about 4 seconds)
	//						before going back to "PHY_ON"...

	uint8_t err_code=0;
	uint8_t currr_state=0;
	uint8_t status=0;

	//Goto PHY_ON state
	uint8_t adf_state_cmd = ADF_PHY_ON;
	SPI_Write(&hspi3, &adf_state_cmd, 1);

	//Check status
	adf_read_state(&err_code,&currr_state,&status);
	if (currr_state!=STATE_PHY_ON)
		return 0xff; //Error, state not expected

	//Goto PHY_MON state
	adf_state_cmd = ADF_PHY_MON;
	SPI_Write(&hspi3, &adf_state_cmd, 1);

	//Check status
		int retry_cptr=0;
		adf_read_state(&err_code,&currr_state,&status);
		while ((currr_state!=STATE_PHY_ON) && (retry_cptr<RETRY_END_MON))
			{
			retry_cptr++;
			HAL_Delay(100);
			adf_read_state(&err_code,&currr_state,&status);
			}
		if (retry_cptr==RETRY_END_MON)
				return 0xff; //Error, state not expected

	//read temp...
	uint32_t val=0;
	val=adf_read_reg(PROFILE_MONITOR1);
	//Temperature as a signed 12-bit number in units of 0.0625°C.
	//val=value & 0xFFF;
	if(val>2048)
		*temp=(val-2048)*0.0625;
	else
		*temp=val*0.0625;

	///Goto PHY_OFF state
	adf_state_cmd = ADF_PHY_OFF;
	SPI_Write(&hspi3, &adf_state_cmd, 1);

	return  0x00; //READ TEMP SUCESSFULL
}


void init_uhf_txbuffer_ramp(void)
{
	int i;
	for (i = 0; i < PAYLOAD_SIZE; i++)
		uhf_txbuffer[i] = (uint8_t)i;   //i value 8 LSBs are loaded into buffer
}

void init_uhf_txbuffer(void)
{
	uint32_t* ptr=NULL;
	ptr = (uint32_t *)&uhf_txbuffer;

	int i;
	for (i=0;i<(PAYLOAD_SIZE>>2);i++)
		{
		*(ptr+i)=SYNC_WORD;
		}
}


uint8_t adf_tx_data(void) {
	uint8_t err_code=0;
	uint8_t currr_state=0;
	uint8_t status=0;

	//Check status
	adf_read_state(&err_code,&currr_state,&status);
	if (currr_state!=STATE_PHY_OFF)
		return 0xff; //Error, state not expected

	// Put the ADF7030 in "On State"
	uint8_t adf_state_cmd = ADF_PHY_ON;
	SPI_Write(&hspi3, &adf_state_cmd, 1);

	//Check status
	adf_read_state(&err_code,&currr_state,&status);
		if (currr_state!=STATE_PHY_ON)
			return 0xff; //Error, state not expected

	// Config GPIO7 to drive PA while tx is on
	uint32_t reg_val;
	reg_val = adf_read_reg(PROFILE_RADIO_DIG_TX_CFG1);
//	reg_val = (reg_val & ~EXT_PA_PIN_SEL_MASK) | (GPIO7 << EXT_PA_PIN_SEL_SHIFT) | EXT_PA_FRAMING_EN_MASK;
	reg_val = (reg_val & ~EXT_PA_PIN_SEL_MASK) | ((uint32_t)GPIO6 << EXT_PA_PIN_SEL_SHIFT) | EXT_PA_FRAMING_EN_MASK;
	adf_write_reg(PROFILE_RADIO_DIG_TX_CFG1, reg_val);

	//Transfer host local tx buffer into ADF7030 tx buffer region :

	//TX buffer has a dedicated memory region called "Packet memory"
	//Area : 0x2000 0AF0 to 0x2000 0FFC (1292bytes).
	//Host chooses the buffer address in this in the area.
	//Address setting is setted in PTR_TX_BASE (part of GENERIC_PKT_BUFF_CFG0 register).
	//The base address of the Tx payload is 0x2000000 + (PTR_TX_BASE × 4).

	uint32_t write_addr = PACKET_MEM_BASE_ADDR+PTR_TX_BASE;

	uint32_t* ptr=NULL;

	ptr = (uint32_t *)&uhf_txbuffer;

	int i;
	for (i=0;i<(PAYLOAD_SIZE>>2);i++)
		{
		adf_write_reg(write_addr+i*4, *(ptr+i));
		*(ptr+i)=SYNC_WORD;
		}

	//goto PHY_TX
	adf_state_cmd = ADF_PHY_TX;
	SPI_Write(&hspi3, &adf_state_cmd, 1);

	//Wait for end of tx...
	//ADF7030 should go back to PHY_ON
	int retry_cptr=0;
	adf_read_state(&err_code,&currr_state,&status);
	while ((currr_state!=STATE_PHY_ON) && (retry_cptr<RETRY_END_TX))
		{
		retry_cptr++;
		HAL_Delay(100);
		adf_read_state(&err_code,&currr_state,&status);
		}
	if (retry_cptr==RETRY_END_TX)
			return 0xff; //Error, state not expected

	///goto PHY_OFF
	adf_state_cmd = ADF_PHY_OFF;
	SPI_Write(&hspi3, &adf_state_cmd, 1);


	return 0x00; //TX SUCESSFULL
}



