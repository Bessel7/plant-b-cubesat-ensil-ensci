#include "main.h"
#include "SpinoDriver/spUART.h"


#define TIMEOUT_RX_MAX 5000
#define TIMEOUT_RX_MIN 50

#define ERROR_RCV_UART 4

extern  int gv_error_code;
extern  int gv_error_subcode;

extern UART_HandleTypeDef hlpuart1;


int readUART(unsigned char *data, int size)
{
	int ret=0;
	unsigned char in;
	int endrcv=1;
	int cpt =0;

	// attente 1er car
	HAL_StatusTypeDef  status = HAL_UART_Receive (&hlpuart1,(uint8_t *) &in, 1, TIMEOUT_RX_MAX);

	if ( status != HAL_OK )
	{
	   gv_error_code = ERROR_RCV_UART;
	   gv_error_subcode = status;
	   ret=0;
	} else
	{
		data[cpt++] = in;
		while (endrcv != 0)
		{
			status = HAL_UART_Receive (&hlpuart1, (uint8_t  *) &in, 1, TIMEOUT_RX_MIN);
			if ( status == HAL_OK )
			{
				data[cpt++] = in;
				endrcv=1;

			}
			else
			{
				endrcv=0;
				if (cpt!=0)
				{
					ret = cpt;
				}
				else
				{
					endrcv=0;
					gv_error_code = ERROR_RCV_UART;
					gv_error_subcode = status;
					ret=0;
				}

			}

		}
	}

	return ret;

}


/**
  * @brief write
  * @param None
  * @retval None
  */
  
  void writeUART(unsigned char *message, int size )
  {
	  
	  	  // BLocking uart transmit
	  //char test_uart[] = "test uart 115200\n";
	HAL_UART_Transmit(&hlpuart1, message, (size_t)size, HAL_MAX_DELAY);
	
  }

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OverSampling = UART_OVERSAMPLING_16;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
