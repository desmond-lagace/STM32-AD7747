#include <AD7747.hpp>

UART_HandleTypeDef uart;
I2C_HandleTypeDef hi2c;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART_Init();
static void MX_I2C_Init();

#define _DEBUG(x) HAL_UART_Transmit(&uart,(uint8_t *) x,strlen(x),HAL_MAX_DELAY)

int main ( void ) {

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_UART_Init();
	MX_I2C_Init();

	AD7747 ad7747 = AD7747();
		if ( !ad7747.begin(&hi2c) ) {
			_DEBUG("Init failed\r\n");
			while(1);
		}
	_DEBUG("Init successful\r\n");

	char buffer[128];
	float C;
	float T;

	while ( 1 ) {

		C = ad7747.getCapacitance(&hi2c);
		T = ad7747.getTempC(&hi2c);
		sprintf(buffer,"%.4lf pF\t%.2lf degC\r\n",C,T);
		_DEBUG(buffer);
		HAL_Delay(500);

	}

	return 0;
}

void SystemClock_Config(void)
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  RCC_OscInitStruct.PLL.PLLM = 8;
	  RCC_OscInitStruct.PLL.PLLN = 84;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 4;
	  HAL_RCC_OscConfig(&RCC_OscInitStruct);


	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();


/* USER CODE BEGIN MX_GPIO_Init_2 */

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

static void MX_UART_Init() {
	uart.Instance = USART2;
	uart.Init.BaudRate = 115200;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.Mode = UART_MODE_TX_RX;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&uart);
}

static void MX_I2C_Init() {
		 hi2c.Instance = I2C1;
		 hi2c.Init.ClockSpeed = 100000;
		 hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
		 hi2c.Init.OwnAddress1 = 0;
		 hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		 hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		 hi2c.Init.OwnAddress2 = 0;
		 hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		 hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		 HAL_I2C_Init(&hi2c);
}


