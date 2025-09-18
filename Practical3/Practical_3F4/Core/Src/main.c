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
/* Task 5: FPU Impact Analysis - compare float vs double, FPU enabled vs disabled */
static const uint32_t kMaxIter = 100u;
static const uint32_t kNumResolutions = 5;
static const uint16_t kWidths[5]  = {128, 160, 192, 224, 256};  /* Practical 1B sizes */
static const uint16_t kHeights[5] = {128, 160, 192, 224, 256};

/* Memory management for large images */
#define MAX_TILE_SIZE 256  /* Maximum tile size that fits in available SRAM */
#define TILE_OVERLAP 0     /* No overlap needed for Mandelbrot calculation */

/* Live Expressions: current benchmark metrics */
volatile uint32_t g_current_width = 0u;
volatile uint32_t g_current_height = 0u;
volatile uint32_t g_current_cycles = 0u;
volatile uint32_t g_current_checksum = 0u;
volatile double g_current_execution_time = 0.0;
volatile double g_current_throughput = 0.0;

/* Task 5: FPU Impact Analysis Results */
/* Float precision results */
volatile uint32_t checksum_float[5] = {0u, 0u, 0u, 0u, 0u};
volatile double execution_time_ms_float[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
volatile uint32_t cpu_cycles_float[5] = {0u, 0u, 0u, 0u, 0u};
volatile double throughput_pps_float[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

/* Double precision results */
volatile uint32_t checksum_double[5] = {0u, 0u, 0u, 0u, 0u};
volatile double execution_time_ms_double[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
volatile uint32_t cpu_cycles_double[5] = {0u, 0u, 0u, 0u, 0u};
volatile double throughput_pps_double[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

/* Accuracy comparison */
volatile double accuracy_diff_percent[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
volatile double speedup_factor[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

/* Current test configuration */
volatile uint32_t g_current_test_mode = 0u; /* 0=float, 1=double */
volatile uint32_t g_fpu_enabled = 1u; /* 1=FPU enabled, 0=FPU disabled */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
//TODO: Define any function prototypes you might need such as the calculate Mandelbrot function among others
static uint32_t generate_mandelbrot_checksum_float(uint16_t width, uint16_t height, uint32_t max_iter);
static uint32_t generate_mandelbrot_checksum_double(uint16_t width, uint16_t height, uint32_t max_iter);
static void dwt_cycle_counter_init(void);
static inline uint32_t dwt_get_cycles(void);
static void log_fpu_benchmark(uint16_t width, uint16_t height, uint32_t cycles, uint32_t checksum, double throughput, const char* precision);
static void log_fpu_comparison(uint16_t width, uint16_t height, uint32_t checksum_float, uint32_t checksum_double, double speedup);
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
      /* Visual indicator: LED0 ON */
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

      /* Task 5: FPU Impact Analysis - Test both float and double precision */
      
      /* Test 1: Float precision */
      g_current_test_mode = 0u; /* float */
      printf("\r\n=== FPU Impact Analysis: FLOAT PRECISION ===\r\n");
      for (uint32_t size_index = 0; size_index < kNumResolutions; ++size_index)
      {
        uint16_t w = kWidths[size_index];
        uint16_t h = kHeights[size_index];
        g_current_width = (uint32_t)w;
        g_current_height = (uint32_t)h;

        DWT->CYCCNT = 0; // reset cycle counter
        uint32_t start_cycles = dwt_get_cycles();
        uint32_t result_checksum = generate_mandelbrot_checksum_float(w, h, kMaxIter);
        uint32_t end_cycles = dwt_get_cycles();
        uint32_t elapsed_cycles = end_cycles - start_cycles;
        double elapsed_seconds = (double)elapsed_cycles / (double)SystemCoreClock;
        double total_pixels = (double)w * (double)h;
        double throughput = total_pixels / elapsed_seconds;

        /* Update Live Expressions */
        g_current_cycles = elapsed_cycles;
        g_current_checksum = result_checksum;
        g_current_execution_time = elapsed_seconds * 1000.0;
        g_current_throughput = throughput;

        /* Store float results */
        checksum_float[size_index] = result_checksum;
        execution_time_ms_float[size_index] = elapsed_seconds * 1000.0;
        cpu_cycles_float[size_index] = elapsed_cycles;
        throughput_pps_float[size_index] = throughput;

        log_fpu_benchmark(w, h, elapsed_cycles, result_checksum, throughput, "FLOAT");
      }
      
      /* Test 2: Double precision */
      g_current_test_mode = 1u; /* double */
      printf("\r\n=== FPU Impact Analysis: DOUBLE PRECISION ===\r\n");
      for (uint32_t size_index = 0; size_index < kNumResolutions; ++size_index)
      {
        uint16_t w = kWidths[size_index];
        uint16_t h = kHeights[size_index];
        g_current_width = (uint32_t)w;
        g_current_height = (uint32_t)h;

        DWT->CYCCNT = 0; // reset cycle counter
        uint32_t start_cycles = dwt_get_cycles();
        uint32_t result_checksum = generate_mandelbrot_checksum_double(w, h, kMaxIter);
        uint32_t end_cycles = dwt_get_cycles();
        uint32_t elapsed_cycles = end_cycles - start_cycles;
        double elapsed_seconds = (double)elapsed_cycles / (double)SystemCoreClock;
        double total_pixels = (double)w * (double)h;
        double throughput = total_pixels / elapsed_seconds;

        /* Update Live Expressions */
        g_current_cycles = elapsed_cycles;
        g_current_checksum = result_checksum;
        g_current_execution_time = elapsed_seconds * 1000.0;
        g_current_throughput = throughput;

        /* Store double results */
        checksum_double[size_index] = result_checksum;
        execution_time_ms_double[size_index] = elapsed_seconds * 1000.0;
        cpu_cycles_double[size_index] = elapsed_cycles;
        throughput_pps_double[size_index] = throughput;

        log_fpu_benchmark(w, h, elapsed_cycles, result_checksum, throughput, "DOUBLE");
      }
      
      /* Calculate and log comparisons */
      printf("\r\n=== FPU Impact Analysis: COMPARISON ===\r\n");
      for (uint32_t size_index = 0; size_index < kNumResolutions; ++size_index)
      {
        uint16_t w = kWidths[size_index];
        uint16_t h = kHeights[size_index];
        
        /* Calculate accuracy difference */
        double accuracy_diff = 0.0;
        if (checksum_double[size_index] != 0) {
          accuracy_diff = ((double)checksum_float[size_index] - (double)checksum_double[size_index]) / (double)checksum_double[size_index] * 100.0;
        }
        accuracy_diff_percent[size_index] = accuracy_diff;
        
        /* Calculate speedup factor */
        double speedup = 0.0;
        if (execution_time_ms_float[size_index] != 0) {
          speedup = execution_time_ms_double[size_index] / execution_time_ms_float[size_index];
        }
        speedup_factor[size_index] = speedup;
        
        log_fpu_comparison(w, h, checksum_float[size_index], checksum_double[size_index], speedup);
      }

      /* Visual indicator: LED1 ON, keep ON 2s, then turn both OFF */
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
      HAL_Delay(2000);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);
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

static uint32_t generate_mandelbrot_checksum_float(uint16_t width, uint16_t height, uint32_t max_iter)
{
	uint32_t mandelbrot_sum = 0u;
	for (uint16_t y = 0; y < height; ++y)
	{
		float y0 = ((float)y / (float)height) * 2.0f - 1.0f;
		for (uint16_t x = 0; x < width; ++x)
		{
			float x0 = ((float)x / (float)width) * 3.5f - 2.5f;
			float xi = 0.0f;
			float yi = 0.0f;
			uint32_t iteration = 0u;
			while (iteration < max_iter && (xi*xi + yi*yi) <= 4.0f)
			{
				float tmp = xi*xi - yi*yi + x0;
				yi = 2.0f*xi*yi + y0;
				xi = tmp;
				++iteration;
			}
			mandelbrot_sum += iteration;
		}
	}
	return mandelbrot_sum;
}

static uint32_t generate_mandelbrot_checksum_double(uint16_t width, uint16_t height, uint32_t max_iter)
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

static void log_fpu_benchmark(uint16_t width, uint16_t height, uint32_t cycles, uint32_t checksum, double throughput, const char* precision)
{
	double seconds = (double)cycles / (double)SystemCoreClock;
	printf("%s: %ux%u, cycles %lu, time %.6f s, throughput %.0f pps, checksum %lu\r\n",
			precision, (unsigned)width, (unsigned)height, (unsigned long)cycles, seconds, throughput, (unsigned long)checksum);
}

static void log_fpu_comparison(uint16_t width, uint16_t height, uint32_t checksum_float, uint32_t checksum_double, double speedup)
{
	printf("COMPARISON %ux%u: Float checksum %lu, Double checksum %lu, Speedup %.2fx\r\n",
			(unsigned)width, (unsigned)height, (unsigned long)checksum_float, (unsigned long)checksum_double, speedup);
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
