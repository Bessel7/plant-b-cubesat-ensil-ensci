/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file : main.c
 * @brief : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "SI4463.h"
#include "ADF7030.h"
#include "core/control.h"
#include "core/setup.h"
#include "drivers/modem.h"
#include "payloadMode/payloadMode.h"
#include "SpinoDriver/spUART.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//SPINO FUNCTIONAL
#define DELAY_SPINO 1000

//bessel start
#define VEML7700_ADDR 0x10 << 1 // Adresse I2C du VEML7700
//bessel end


#define SYSTICK_LOAD (SystemCoreClock/1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c4;

UART_HandleTypeDef hlpuart1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C4_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
extern void ModemOpen();
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

//bessel start
void VEML7700_WriteRegister(uint8_t reg, uint16_t value);
uint16_t VEML7700_ReadRegister(uint8_t reg);
void VEML7700_Init(void);
float VEML7700_GetLux(void);
void AfficherLumiere(float lux);
void I2C_Scan();
//bessel end

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Gestion code Erreur au démarrage */
int gv_error_code;
int gv_error_subcode;
extern char gv_spino_data_avalable;
SI44_STATES current_vhf_state = IDLE;
extern uint8_t uhf_txbuffer[PAYLOAD_SIZE];
extern s_I2C_data gv_I2C_Write_Data[I2CMAXDATA];
volatile uint8_t CSKB_I2C_TX_BUFFER[I2C_BUFFER_SIZE];
volatile unsigned long long lv_RX_FETCH=0;


//bessel start

float lux = 0.0;

void I2C_Scan() {
    char msg[50]; // Augmentation de la taille du tampon pour snprintf
    uint8_t found = 0;

    snprintf(msg, sizeof(msg), "Scanning I2C...\r\n"); // Utilisation de snprintf
    writeUART((unsigned char*)msg, strlen(msg)); // Utilisation de strlen pour la taille

    for (uint8_t i = 0x10; i <=0x11 ; i++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 1, 100) == HAL_OK) {
            snprintf(msg, sizeof(msg), "Device found at 0x%X\r\n", i); // Utilisation de snprintf
            writeUART((unsigned char*)msg, strlen(msg)); // Utilisation de strlen pour la taille
            found = 1;
        }
    }

    if (!found) {
        snprintf(msg, sizeof(msg), "No I2C device found!\r\n"); // Utilisation de snprintf
        writeUART((unsigned char*)msg, strlen(msg)); // Utilisation de strlen pour la taille
    }

    snprintf(msg, sizeof(msg), "Scan Complete\r\n"); // Utilisation de snprintf
    writeUART((unsigned char*)msg, strlen(msg)); // Utilisation de strlen pour la taille
}

// Fonction pour écrire une valeur dans un registre du capteur VEML7700
// Paramètres :
//   - reg : adresse du registre où écrire
//   - value : valeur 16 bits à écrire
// Fonction pour écrire une valeur dans un registre du capteur VEML7700
void VEML7700_WriteRegister(uint8_t reg, uint16_t value) {
    uint8_t data[3];
    data[0] = reg;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;
}

// Fonction pour lire un registre
uint16_t VEML7700_ReadRegister(uint8_t reg) {
    uint8_t data[2] = {0};
    return (data[1] << 8) | data[0]; // Conversion Little Endian
}

// Fonction pour initialiser le capteur VEML7700
// Configure le capteur avec un gain et un temps d'intégration par défaut
void VEML7700_Init(void) {
	VEML7700_WriteRegister(0x00, 0x0000);
    HAL_Delay(10);
}

// Fonction pour lire la valeur de luminosité mesurée par le VEML7700
// Retourne : la valeur en lux
float VEML7700_GetLux(void) {
	uint16_t raw = VEML7700_ReadRegister(0x04); // Lecture de la valeur brute du capteur
	return raw * 0.0036; // Conversion de la val eur brute en lux selon la documentation du capteur
}

// Fonction pour afficher la valeur de luminosité via l'UART
// Paramètre :
//   - lux : valeur de luminosité en lux
void AfficherLumiere(float lux) {
	char buffer[50]; // Tampon pour stocker le message formaté
	snprintf(buffer, sizeof(buffer), "Lux: %.2f lx\r\n", lux); // Formatage de la valeur
	writeUART((unsigned char*)buffer, strlen(buffer)); // Utilisation de strlen pour la taille
}
//bessel end


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C4_Init();
  MX_LPUART1_UART_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

	/* Init vhf packet access */
	for(uint8_t i = 0; i < NB_FETCH; i++) {
		vhf_packet_accessor[i] = &vhf_packet[1 + i * (RX_FIFO_THRES+1)];
		// vhf_packet_accessor[i] = vhf_packet[1 + i * (RX_FIFO_THRES+1)];
	}

	/* Init Timer */
	HAL_TIM_Base_Start_IT(&htim2);

	/* Init OBC I2C Interruption */
	HAL_I2C_EnableListen_IT(&hi2c1);

	/* Init ADF7030 with blocking SPI *
	 * *
	 */
	uint8_t mode=0;
	config_ADF7030(mode);
	init_uhf_txbuffer();

	/* Init SI4461 with blocking
	 *
	 */
	MX_SPI2_Init();
	power_vhf(1);
	reset_si4463();
	HAL_Delay(20);

	uint8_t conf_stat = config_si4463();

	uint8_t chiprev = si446x_part_info();

	// Clearing all interrupt in the SI446x
	si446x_get_int_status(0, 0, 0);

	//init_rx_irq();
	HAL_Delay(1);

	VHF_rstfifo_startrx();

	current_vhf_state = RX_PENDING;

	/********** SPINO Functional Initialisation ********/

	setupGlobalVariable();
	inittlm();
	ModemOpen();
	setModemMode(gv_spinoConfig.spinoTXModemMode);

	/* Mode UART - A virer */
	// envoie version :)
	//unsigned char message[] = "SPINO UART 0.1";
	//writeUART(message, sizeof(message) );

	sendTLMWithoutDelay();

	//bessel start
	I2C_Scan();
	VEML7700_Init();
	//bessel end

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint8_t ReArm_I2C = 0;

	while (1)
	{
		/** SPINO*/

		if(CSKB_I2C_TX_Complete || CSKB_I2C_RX_Complete) {
			ReArm_I2C = 1;
			if(CSKB_I2C_TX_Complete) {
				// Do something here

				CSKB_I2C_TX_Complete = 0;
			} else {
				// Use your data here
				gv_I2C_Write_Data[gv_spino.gv_nb_I2CMAXDATA].size = CSKB_I2C_RX_Size;
				for(uint8_t i = 0; i < CSKB_I2C_RX_Size; i++) {
					gv_I2C_Write_Data[gv_spino.gv_nb_I2CMAXDATA].data[i] = CSKB_I2C_RX_BUFFER[i];
				}
				gv_spino.gv_nb_I2CMAXDATA = ((gv_spino.gv_nb_I2CMAXDATA+1) % I2CMAXDATA );

				// Use CSKB_I2C_NXT_FrameSize if you want the slave to stop transmission after byte count
				// Set to zero if you want the slave to continue answering up to I2C_BUFFER_SIZE
				CSKB_I2C_NXT_FrameSize = CSKB_I2C_RX_Size;

				CSKB_I2C_RX_Complete = 0;
			}

		}
		if(ReArm_I2C && HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY) {
			ReArm_I2C = 0;
			HAL_I2C_EnableListen_IT(&hi2c1);
		}

		/** SPINO */
		if(VHF_Data_Ready == 1) {
			// Process your data from VHF radio
			lv_RX_FETCH =0;
			gv_spino_data_avalable=1;
			readDataRX(vhf_packet_accessor);
			VHF_Data_Ready = 0;
			si446x_get_int_status(0, 0, 0);
			VHF_fifo_data_count = 0;
			VHF_rstfifo_startrx();
			current_vhf_state = RX_PENDING;
		} else if(vhf_overflow) {
			// Problem to fetch data

		}

		// BLocking uart transmit
		//char test_uart[] = "test uart 115200\n";
		//HAL_UART_Transmit(&huart1, test_uart, sizeof(test_uart), HAL_MAX_DELAY);
		//writeUART("Hello world", 100);

		//*****SPINO Functional State Machine
		control();
		HAL_Delay(500);
		// Reset en cas de Rx bloque
		if((lv_RX_FETCH !=0) && ( (gv_spino.timestamps - lv_RX_FETCH) > 200 ) )
		{
			reset_si4463();
			HAL_Delay(20);

			uint8_t conf_stat = config_si4463();

			uint8_t chiprev = si446x_part_info();

			// Clearing all interrupt in the SI446x
			si446x_get_int_status(0, 0, 0);

			VHF_rstfifo_startrx();
			current_vhf_state = RX_PENDING;
			VHF_fifo_data_count =0;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		//bessel start
		float lux = VEML7700_GetLux();
		AfficherLumiere(lux);
		HAL_Delay(2000);
		//bessel end

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00B07CB4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C4_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x006039F7;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 32000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin|FRAM_PWR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, VHF_SDN_Pin|VHF_PWR_EN_Pin|EXT_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, UHF_PWR_EN_Pin|UHF_FE_EN_Pin|UHF_NRST_Pin|UHF_SPI_NSS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : VHF_SPI_NSS_Pin FRAM_PWR_Pin */
  GPIO_InitStruct.Pin = VHF_SPI_NSS_Pin|FRAM_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : VHF_NIRQ_Pin */
  GPIO_InitStruct.Pin = VHF_NIRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VHF_NIRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : VHF_SDN_Pin VHF_PWR_EN_Pin EXT_PWR_EN_Pin */
  GPIO_InitStruct.Pin = VHF_SDN_Pin|VHF_PWR_EN_Pin|EXT_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : EXT_PWR_NFLT_Pin UHF_INT_Pin */
  GPIO_InitStruct.Pin = EXT_PWR_NFLT_Pin|UHF_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : UHF_PWR_EN_Pin UHF_FE_EN_Pin UHF_NRST_Pin UHF_SPI_NSS_Pin */
  GPIO_InitStruct.Pin = UHF_PWR_EN_Pin|UHF_FE_EN_Pin|UHF_NRST_Pin|UHF_SPI_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
 * @brief This function handles EXTI line[9:5] interrupts.
 */
void EXTI9_5_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI9_5_IRQn 0 */
	if(__HAL_GPIO_EXTI_GET_FLAG(VHF_NIRQ_Pin)){
		if(current_vhf_state != IDLE) {
			switch(current_vhf_state) {
			case RX_PENDING:
				// Fetch interrupt status to decide what to do
				// Expected interrupt are:
				// RX_FIFO_THRESHOLD => fetch an other 48byte of data
				// RX_PACKET => Validate the packet
				current_vhf_state = FETCHING_STATUS;
				lv_RX_FETCH = gv_spino.timestamps;
				HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);

				packet_done = 0;
				zero_buff[0] = SI446X_CMD_ID_GET_INT_STATUS;
				zero_buff[1] = 0;
				zero_buff[2] = 0;
				zero_buff[3] = 0;
				HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, 4);

				break;
			default:
				vhf_overflow = 1;
				break;
			}
		}
	}

	/* USER CODE END EXTI9_5_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(VHF_NIRQ_Pin);
	HAL_GPIO_EXTI_IRQHandler(EXT_PWR_NFLT_Pin);
	/* USER CODE BEGIN EXTI9_5_IRQn 1 */

	/* USER CODE END EXTI9_5_IRQn 1 */
}

/* I2C slave callback */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	/* TX transfer is complete */

	//CSKB_I2C_TX_Complete = 1;
	//CSKB_I2C_TX_Size = I2C_BUFFER_SIZE;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	/* RX transfer is complete */

	//CSKB_I2C_RX_Complete = 1;
	//CSKB_I2C_RX_Size = I2C_BUFFER_SIZE;
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode) {
	/* Slave matched his address */
	if (TransferDirection != 0) {
		/* I2C Transmit buffer */
		CSKB_I2C_Dir = I2C_TX;
		uint8_t framesize = (CSKB_I2C_NXT_FrameSize == 0) ? I2C_BUFFER_SIZE : CSKB_I2C_NXT_FrameSize;
		if (HAL_I2C_Slave_Seq_Transmit_IT(hi2c, (uint8_t *)CSKB_I2C_TX_BUFFER, framesize, I2C_FIRST_AND_LAST_FRAME) != HAL_OK) {
			/* Transfer error in transmission process */
			Error_Handler();
		}
	} else {
		CSKB_I2C_Dir = I2C_RX;
		/* I2C Receive on the rx buffer*/
		if (HAL_I2C_Slave_Seq_Receive_IT(hi2c, (uint8_t *)CSKB_I2C_RX_BUFFER, I2C_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK) {
			/* Transfer error in reception process */
			Error_Handler();
		}
	}

}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	/* Handling here incomplete RX/TX with dirty hack */
	if (CSKB_I2C_Dir == I2C_TX) {
		CSKB_I2C_TX_Complete = 1;
		CSKB_I2C_TX_Size = (CSKB_I2C_NXT_FrameSize == 0) ? I2C_BUFFER_SIZE - hi2c->XferSize : CSKB_I2C_NXT_FrameSize - hi2c->XferSize;
	} else if (CSKB_I2C_Dir == I2C_RX) {
		CSKB_I2C_RX_Complete = 1;
		CSKB_I2C_RX_Size = I2C_BUFFER_SIZE - hi2c->XferSize;
	}
	CSKB_I2C_Dir = I2C_NONE;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
	/** Error_Handler() function is called when error occurs.
	 * 1- When Slave doesn't acknowledge its address, Master restarts communication.
	 * 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
	 */
	/*if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
{
Error_Handler();
}*/
	// Really dirty trick no warranties
	if ((HAL_I2C_GetError(I2cHandle) == HAL_I2C_ERROR_AF) && (HAL_I2C_GetState(I2cHandle) != HAL_I2C_STATE_BUSY)) {
		return; // no real error!!!
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim->Instance == htim2.Instance) {
		ms_counter++;
	}
}

void SPI2_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi2);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	if(hspi->Instance == SPI2) {
		if (current_vhf_state == FETCHING_STATUS) {
			HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
			current_vhf_state = CTS_POLLING;
			si446x_cts_it_poll();
		} else if(current_vhf_state == CTS_POLLING) {
			if(rx_data[1] != 0xFF) {
				HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
				//HAL_Delay(1);
				__NOP();
				__NOP();
				__NOP();
				si446x_cts_it_poll();
			} else {
				current_vhf_state = STATUS_PENDING;
				HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, rx_data, 8);
			}
		} else if(current_vhf_state == STATUS_PENDING) {
			HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);

			current_vhf_state = STATUS_OK;

			/*uint8_t INT_PEND = rx_data[0];
uint8_t INT_STATUS = rx_data[1];*/
			uint8_t PH_PEND = rx_data[2];
			/*uint8_t PH_STATUS = rx_data[3];
uint8_t MODEM_PEND = rx_data[4];
uint8_t MODEM_STATUS = rx_data[5];
uint8_t CHIP_PEND = rx_data[6];
uint8_t CHIP_STATUS = rx_data[7];*/

			uint8_t fifo_thresh = (PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_STATUS_RX_FIFO_ALMOST_FULL_BIT);
			uint8_t rx_packet = ((PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT) << 1);

			if(fifo_thresh) {
				HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_RESET);
				zero_buff[0] = SI446X_CMD_ID_READ_RX_FIFO;

				current_vhf_state = FETCHING_DATA;
				VHF_fifo_data_count++;

				// Need to accomodate for the extra byte from the command
				HAL_SPI_TransmitReceive_IT(&hspi2, zero_buff, &vhf_packet[(VHF_fifo_data_count-1) * (RX_FIFO_THRES+1)], RX_FIFO_THRES + 1);
			}
		} else if (current_vhf_state == FETCHING_DATA) {
			HAL_GPIO_WritePin(GPIOB, VHF_SPI_NSS_Pin, GPIO_PIN_SET);
			if(VHF_fifo_data_count == NB_FETCH) {
				VHF_Data_Ready = 1;
				current_vhf_state = IDLE;
			} else {
				current_vhf_state = RX_PENDING;
			}
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	/* WTF ? ! We shouldn't be here ! */
	NVIC_SystemReset();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
