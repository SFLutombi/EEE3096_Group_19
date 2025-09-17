/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <stdio.h>
#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//TODO: Define variables you think you might need
// - Performance timing variables (e.g execution time, throughput, pixels per second, clock cycles)
static const uint32_t kMaxIter = 100; /* Match Practical 1B default */
static const uint32_t kNumResolutions = 5;
static const uint16_t kWidths[5]  = {128, 160, 192, 224, 256};
static const uint16_t kHeights[5] = {128, 160, 192, 224, 256};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
//TODO: Define any function prototypes you might need such as the calculate Mandelbrot function among others
static uint32_t generate_mandelbrot_checksum(uint16_t width, uint16_t height, uint32_t max_iter);
static void dwt_cycle_counter_init(void);
static inline uint32_t dwt_get_cycles(void);
static void log_benchmark(uint16_t width, uint16_t height, uint32_t cycles, uint32_t checksum);
int __io_putchar(int ch);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  /* USER CODE BEGIN 2 */
  dwt_cycle_counter_init();
  printf("\r\nEEE3096S Practical 3 - STM32F4 Mandelbrot Benchmark\r\n");
  printf("SYSCLK: %lu Hz\r\n", SystemCoreClock);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //TODO: Visual indicator: Turn on LED0 to signal processing start
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);


	  //TODO: Benchmark and Profile Performance
	  for (uint32_t i = 0; i < kNumResolutions; ++i)
	  {
	  	uint16_t w = kWidths[i];
	  	uint16_t h = kHeights[i];
	  	DWT->CYCCNT = 0; // reset cycle counter
	  	uint32_t start_cycles = dwt_get_cycles();
	  	uint32_t checksum = generate_mandelbrot_checksum(w, h, kMaxIter);
	  	uint32_t end_cycles = dwt_get_cycles();
	  	uint32_t elapsed_cycles = end_cycles - start_cycles;
	  	log_benchmark(w, h, elapsed_cycles, checksum);
	  }


	  //TODO: Visual indicator: Turn on LED1 to signal processing start
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);


	  //TODO: Keep the LEDs ON for 2s
	  HAL_Delay(2000);

	  //TODO: Turn OFF LEDs
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//TODO: Function signatures you defined previously , implement them here

static uint32_t generate_mandelbrot_checksum(uint16_t width, uint16_t height, uint32_t max_iter)
{
	uint32_t mandelbrot_sum = 0u;
	for (uint16_t y = 0; y < height; ++y)
	{
		double y0 = ((double)y / (double)height) * 2.0 - 1.0;
		for (uint16_t x = 0; x < width; ++x)
		{
			double x0 = ((double)x / (double)width) * 3.5 - 2.5;
			double xi = 0.0;
			double yi = 0.0;
			uint32_t iteration = 0u;
			while (iteration < max_iter && (xi*xi + yi*yi) <= 4.0)
			{
				double tmp = xi*xi - yi*yi + x0;
				yi = 2.0*xi*yi + y0;
				xi = tmp;
				++iteration;
			}
			mandelbrot_sum += iteration;
		}
	}
	return mandelbrot_sum;
}

static void dwt_cycle_counter_init(void)
{
	/* Enable TRC */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	/* Unlock DWT (if locked) by writing the key to LAR when present */
#ifdef DWT_LAR
	DWT->LAR = 0xC5ACCE55;
#endif
	/* Reset the cycle counter */
	DWT->CYCCNT = 0;
	/* Enable the cycle counter */
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t dwt_get_cycles(void)
{
	return DWT->CYCCNT;
}

static void log_benchmark(uint16_t width, uint16_t height, uint32_t cycles, uint32_t checksum)
{
	double seconds = (double)cycles / (double)SystemCoreClock;
	printf("Resolution %ux%u, cycles %lu, time %.6f s, checksum %lu\r\n",
			(unsigned)width, (unsigned)height, (unsigned long)cycles, seconds, (unsigned long)checksum);
}

int __io_putchar(int ch)
{
	/* Route to ITM stimulus port 0 for SWV view. Requires debugger and SWO enabled. */
	if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
	{
		while (ITM->PORT[0].u32 == 0) { __NOP(); }
		ITM->PORT[0].u8 = (uint8_t)ch;
		return ch;
	}
	/* Fallback: do nothing */
	return ch;
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
