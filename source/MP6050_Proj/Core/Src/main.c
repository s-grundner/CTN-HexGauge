/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mpu6050.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEG_NUM_MASK (SEGA_Pin | SEGB_Pin | SEGC_Pin | SEGD_Pin | SEGE_Pin | SEGF_Pin | SEGG_Pin)
#define SEG_DP_MASK  (SEGDP_Pin)
#define SEG_MAX_NUM  (9)
#define SEG_MIN_NUM  (0)
#define SEG_N_NUMS   (SEG_MAX_NUM - SEG_MIN_NUM + 1)

#define SEG_Port GPIOB

#define THRESH 0.2

#define N_SET(y) (((y) < -THRESH) * (SEGA_Pin))
#define S_SET(y) (((y) >  THRESH) * (SEGD_Pin))
#define O_SET(x) (((x) < -THRESH) * (SEGB_Pin | SEGC_Pin))
#define W_SET(x) (((x) >  THRESH) * (SEGF_Pin | SEGE_Pin))

#define N_RST(y) (((y) < -THRESH) * (SEGE_Pin | SEGC_Pin))
#define S_RST(y) (((y) >  THRESH) * (SEGB_Pin | SEGF_Pin))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
mpu6050_handle_t hmpu;

const uint16_t segments[SEG_N_NUMS] = { // Index is the displayed number
	(SEG_NUM_MASK & ~SEGG_Pin),                             // 0
	(SEGB_Pin | SEGC_Pin),                                  // 1
	(SEGA_Pin | SEGB_Pin | SEGD_Pin | SEGE_Pin | SEGG_Pin), // 2
	(SEGA_Pin | SEGB_Pin | SEGC_Pin | SEGD_Pin | SEGG_Pin), // 3
	(SEGB_Pin | SEGC_Pin | SEGF_Pin | SEGG_Pin),            // 4
	(SEGA_Pin | SEGC_Pin | SEGD_Pin | SEGF_Pin | SEGG_Pin), // 5
	(SEG_NUM_MASK & ~SEGB_Pin),                             // 6
	(SEGA_Pin | SEGB_Pin | SEGC_Pin),                       // 7
	(SEG_NUM_MASK),                                         // 8
	(SEG_NUM_MASK & ~SEGE_Pin),                             // 9
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == MPU_INT_Pin)
	{
		MPU6050_SetIntrFlag(hmpu);
	}
}

#define USR_SEG_WriteReg(x) MODIFY_REG(SEG_Port->ODR, SEG_NUM_MASK, x)
#define USR_SEG_WriteNum(x) USR_SEG_WriteReg(segments[x])

void USR_SEG_Direction(float_vector3_t dir)
{
	uint8_t set = N_SET(dir.y) | O_SET(dir.x) | S_SET(dir.y) | W_SET(dir.x);
	if (set == 0) // No direction
	{
		USR_SEG_WriteReg(SEGG_Pin);
	} else // Any Direction, Clear opposite E/W bits if direction is diagonal
	{
		USR_SEG_WriteReg(set & ~(N_RST(dir.y) | S_RST(dir.y)));
	}
}

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
	MX_TIM2_Init();
	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim2);

	uint32_t usb_timer = __HAL_TIM_GET_COUNTER(&htim2);
	uint32_t display_timer = __HAL_TIM_GET_COUNTER(&htim2);

	char usb_buf[64];

	float_vector3_t data = { .x = 0, .y = 0, .z = 0 };
	mpu6050_config_t mpu_cfg = {
		.accel_range = MPU6050_ACCEL_RANGE_2G,
		.gyro_range = MPU6050_GYRO_RANGE_250DPS,
		.hi2c = &hi2c1
	};

	MPU6050_Init(&hmpu, &mpu_cfg);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (MPU6050_DataReady(hmpu))
		{
			data = MPU6050_ReadAcceleration(hmpu);
		}

		if (__HAL_TIM_GET_COUNTER(&htim2) - usb_timer >= 1000)
		{
			HAL_GPIO_TogglePin(SEGDP_GPIO_Port, SEGDP_Pin);
			uint8_t buf_len = snprintf(usb_buf, 64, "%.2f, %.2f, %.2f\r\n", data.x, data.y, data.z);
			CDC_Transmit_FS(usb_buf, buf_len);
			usb_timer = __HAL_TIM_GET_COUNTER(&htim2);
		}

		if (__HAL_TIM_GET_COUNTER(&htim2) - display_timer >= 100)
		{
			USR_SEG_Direction(data);
			display_timer = __HAL_TIM_GET_COUNTER(&htim2);
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

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

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 8400;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
		!= HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
		SEGA_Pin | SEGB_Pin | SEGC_Pin | SEGD_Pin | SEGE_Pin | SEGF_Pin
		| SEGG_Pin | SEGDP_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : MPU_INT_Pin */
	GPIO_InitStruct.Pin = MPU_INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(MPU_INT_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : KEY_Pin */
	GPIO_InitStruct.Pin = KEY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : SEGA_Pin SEGB_Pin SEGC_Pin SEGD_Pin
	 SEGE_Pin SEGF_Pin SEGG_Pin SEGDP_Pin */
	GPIO_InitStruct.Pin = SEGA_Pin | SEGB_Pin | SEGC_Pin | SEGD_Pin | SEGE_Pin
		| SEGF_Pin | SEGG_Pin | SEGDP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	   /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
