/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VHF_SPI_NSS_Pin GPIO_PIN_12
#define VHF_SPI_NSS_GPIO_Port GPIOB
#define VHF_SPI_SCK_Pin GPIO_PIN_13
#define VHF_SPI_SCK_GPIO_Port GPIOB
#define VHF_SPI_MISO_Pin GPIO_PIN_14
#define VHF_SPI_MISO_GPIO_Port GPIOB
#define VHF_SPI_MOSI_Pin GPIO_PIN_15
#define VHF_SPI_MOSI_GPIO_Port GPIOB
#define VHF_NIRQ_Pin GPIO_PIN_6
#define VHF_NIRQ_GPIO_Port GPIOC
#define VHF_NIRQ_EXTI_IRQn EXTI9_5_IRQn
#define VHF_SDN_Pin GPIO_PIN_7
#define VHF_SDN_GPIO_Port GPIOC
#define VHF_PWR_EN_Pin GPIO_PIN_8
#define VHF_PWR_EN_GPIO_Port GPIOC
#define EXT_PWR_EN_Pin GPIO_PIN_9
#define EXT_PWR_EN_GPIO_Port GPIOC
#define EXT_PWR_NFLT_Pin GPIO_PIN_8
#define EXT_PWR_NFLT_GPIO_Port GPIOA
#define EXT_PWR_NFLT_EXTI_IRQn EXTI9_5_IRQn
#define UHF_PWR_EN_Pin GPIO_PIN_9
#define UHF_PWR_EN_GPIO_Port GPIOA
#define UHF_FE_EN_Pin GPIO_PIN_10
#define UHF_FE_EN_GPIO_Port GPIOA
#define UHF_NRST_Pin GPIO_PIN_11
#define UHF_NRST_GPIO_Port GPIOA
#define UHF_INT_Pin GPIO_PIN_12
#define UHF_INT_GPIO_Port GPIOA
#define UHF_INT_EXTI_IRQn EXTI15_10_IRQn
#define UHF_SPI_NSS_Pin GPIO_PIN_15
#define UHF_SPI_NSS_GPIO_Port GPIOA
#define UHF_SPI_SCK_Pin GPIO_PIN_10
#define UHF_SPI_SCK_GPIO_Port GPIOC
#define UHF_SPI_MISO_Pin GPIO_PIN_11
#define UHF_SPI_MISO_GPIO_Port GPIOC
#define UHF_SPI_MOSI_Pin GPIO_PIN_12
#define UHF_SPI_MOSI_GPIO_Port GPIOC
#define FRAM_PWR_Pin GPIO_PIN_5
#define FRAM_PWR_GPIO_Port GPIOB
#define INT_I2C_SCL_Pin GPIO_PIN_6
#define INT_I2C_SCL_GPIO_Port GPIOB
#define INT_I2C_SDA_Pin GPIO_PIN_7
#define INT_I2C_SDA_GPIO_Port GPIOB
#define EXT_I2C_SCL_Pin GPIO_PIN_8
#define EXT_I2C_SCL_GPIO_Port GPIOB
#define EXT_I2C_SDA_Pin GPIO_PIN_9
#define EXT_I2C_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
static volatile uint64_t ms_counter = 0;


/* I2C link */
#define I2C_BUFFER_SIZE			255
#define I2C_NONE				0
#define I2C_TX					1
#define I2C_RX					2

static volatile uint8_t CSKB_I2C_Dir = I2C_NONE;

static volatile uint8_t CSKB_I2C_TX_Complete = 0;
static volatile uint8_t CSKB_I2C_RX_Complete = 0;

static volatile uint8_t CSKB_I2C_TX_Size = 0;
static volatile uint8_t CSKB_I2C_RX_Size = 0;

static volatile uint8_t CSKB_I2C_NXT_FrameSize = 0;

static volatile uint8_t CSKB_I2C_RX_BUFFER[I2C_BUFFER_SIZE];

/* VHF Link */
#define VHF_MAX_LEN					255

#define VHF_BUF0_STATUS_MASK		1
#define VHF_BUF1_STATUS_MASK		2

static volatile uint8_t current_vhf_buffer = 0;

static volatile uint8_t vhf_buffer_status = 0x00;

static volatile uint8_t VHF_Data_Ready = 0;

static volatile uint8_t VHF_fifo_data_count = 0;

static volatile uint8_t packet_done = 0;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
