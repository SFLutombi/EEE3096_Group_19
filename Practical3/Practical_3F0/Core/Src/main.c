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

/* USER CODE END Includes */

/* Task 6: Global variables for Live Expressions - Must be declared here for debugger access */
uint32_t g_total_program_cycles = 0u;
double g_total_program_time_ms = 0.0;
double g_total_time_sum = 0.0;
uint32_t g_test_counter = 0u;
uint32_t g_simple_test = 0u;  /* Simple test variable */

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
/* Task 6: Compiler Optimization Analysis - Practical 1B sizes */
static const uint32_t kMaxIter = 100u;
static const uint32_t kNumResolutions = 5;
static const uint16_t kWidths[5]  = {128, 160, 192, 224, 256};  /* Practical 1B sizes */
static const uint16_t kHeights[5] = {128, 160, 192, 224, 256};

/* Memory management for large images */
#define MAX_TILE_SIZE 128  /* Smaller tile size for F0 due to limited SRAM */
#define TILE_OVERLAP 0     /* No overlap needed for Mandelbrot calculation */

/* Live Expressions: current benchmark metrics */
volatile uint32_t g_current_width = 0u;
volatile uint32_t g_current_height = 0u;
volatile uint32_t g_current_cycles = 0u;
volatile uint32_t g_current_checksum = 0u;
volatile double g_current_execution_time = 0.0;
volatile double g_current_throughput = 0.0;

/* Live Expressions: per-size results */
volatile uint32_t checksum[5] = {0u, 0u, 0u, 0u, 0u};
volatile double execution_time_ms[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
volatile uint32_t cpu_cycles[5] = {0u, 0u, 0u, 0u, 0u};
volatile double throughput_pps[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

/* Task 4: Tiling information */
volatile uint32_t g_num_tiles_x = 0u;
volatile uint32_t g_num_tiles_y = 0u;
volatile uint32_t g_total_tiles = 0u;
volatile uint32_t g_current_tile = 0u;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t generate_mandelbrot_checksum(uint16_t width, uint16_t height, uint32_t max_iter);
static uint32_t generate_mandelbrot_checksum_tiled(uint16_t width, uint16_t height, uint32_t max_iter);
static uint32_t estimate_cpu_cycles_from_ms(uint32_t elapsed_ms);
static void log_tiling_info(uint16_t width, uint16_t height, uint32_t tiles_x, uint32_t tiles_y);

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
  
  /* Initialize Task 6 timing variables */
  g_total_program_cycles = 0u;
  g_total_program_time_ms = 0.0;
  g_total_time_sum = 0.0;
  g_test_counter = 0u;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      /* Increment test counters to verify Live Expressions work */
      g_test_counter++;
      g_simple_test++;
      
      /* Start total program timing */
      uint32_t program_start_ms = HAL_GetTick();
      
      /* Visual indicator: LED0 ON */
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

      /* Task 4: Scalability test up to Full HD with tiling for large images */
      for (uint32_t size_index = 0; size_index < kNumResolutions; ++size_index)
      {
        uint16_t w = kWidths[size_index];
        uint16_t h = kHeights[size_index];
        g_current_width = (uint32_t)w;
        g_current_height = (uint32_t)h;

        uint32_t start_ms = HAL_GetTick();
        
        /* Use tiled approach for large images that exceed memory constraints */
        uint32_t result_checksum;
        if (w > MAX_TILE_SIZE || h > MAX_TILE_SIZE) {
          result_checksum = generate_mandelbrot_checksum_tiled(w, h, kMaxIter);
        } else {
          result_checksum = generate_mandelbrot_checksum(w, h, kMaxIter);
        }
        
        uint32_t end_ms = HAL_GetTick();
        uint32_t elapsed_ms = end_ms - start_ms;
        
        /* Estimate CPU cycles from elapsed time (F0 doesn't have DWT) */
        uint32_t estimated_cycles = estimate_cpu_cycles_from_ms(elapsed_ms);
        double elapsed_seconds = (double)elapsed_ms / 1000.0;
        double total_pixels = (double)w * (double)h;
        double throughput = total_pixels / elapsed_seconds;

        /* Update Live Expressions */
        g_current_cycles = estimated_cycles;
        g_current_checksum = result_checksum;
        g_current_execution_time = (double)elapsed_ms;
        g_current_throughput = throughput;

        /* Store results */
        checksum[size_index] = result_checksum;
        execution_time_ms[size_index] = (double)elapsed_ms;
        cpu_cycles[size_index] = estimated_cycles;
        throughput_pps[size_index] = throughput;
      }

      /* End total program timing */
      uint32_t program_end_ms = HAL_GetTick();
      uint32_t total_elapsed_ms = program_end_ms - program_start_ms;
      g_total_program_time_ms = (double)total_elapsed_ms;
      
      /* Calculate total time sum from individual tests */
      g_total_time_sum = 0.0;
      for (uint32_t i = 0; i < kNumResolutions; ++i) {
        g_total_time_sum += execution_time_ms[i];
      }
      
      /* Debug: Store raw values for debugging */
      g_total_program_cycles = total_elapsed_ms;  /* Store raw ms value for debugging */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

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

static uint32_t generate_mandelbrot_checksum_tiled(uint16_t width, uint16_t height, uint32_t max_iter)
{
  uint32_t total_checksum = 0u;
  
  /* Calculate number of tiles needed */
  uint32_t tiles_x = (width + MAX_TILE_SIZE - 1) / MAX_TILE_SIZE;
  uint32_t tiles_y = (height + MAX_TILE_SIZE - 1) / MAX_TILE_SIZE;
  uint32_t total_tiles = tiles_x * tiles_y;
  
  /* Update Live Expressions with tiling info */
  g_num_tiles_x = tiles_x;
  g_num_tiles_y = tiles_y;
  g_total_tiles = total_tiles;
  
  log_tiling_info(width, height, tiles_x, tiles_y);
  
  /* Process each tile */
  for (uint32_t tile_y = 0; tile_y < tiles_y; ++tile_y)
  {
    for (uint32_t tile_x = 0; tile_x < tiles_x; ++tile_x)
    {
      g_current_tile = tile_y * tiles_x + tile_x + 1;
      
      /* Calculate tile boundaries */
      uint16_t start_x = tile_x * MAX_TILE_SIZE;
      uint16_t start_y = tile_y * MAX_TILE_SIZE;
      uint16_t end_x = (start_x + MAX_TILE_SIZE > width) ? width : start_x + MAX_TILE_SIZE;
      uint16_t end_y = (start_y + MAX_TILE_SIZE > height) ? height : start_y + MAX_TILE_SIZE;
      uint16_t tile_width = end_x - start_x;
      uint16_t tile_height = end_y - start_y;
      
      /* Process this tile */
      for (uint16_t y = 0; y < tile_height; ++y)
      {
        uint16_t global_y = start_y + y;
        double y0 = ((double)global_y / (double)height) * 2.0 - 1.0;
        for (uint16_t x = 0; x < tile_width; ++x)
        {
          uint16_t global_x = start_x + x;
          double x0 = ((double)global_x / (double)width) * 3.5 - 2.5;
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
          total_checksum += iteration;
        }
      }
    }
  }
  
  return total_checksum;
}

static void log_tiling_info(uint16_t width, uint16_t height, uint32_t tiles_x, uint32_t tiles_y)
{
  /* Note: F0 doesn't have printf capability, so this is a placeholder */
  /* In a real implementation, you might use SWO or UART for logging */
  (void)width; (void)height; (void)tiles_x; (void)tiles_y; /* Suppress unused warnings */
}

static uint32_t estimate_cpu_cycles_from_ms(uint32_t elapsed_ms)
{
  /* Estimate CPU cycles based on SystemCoreClock and elapsed milliseconds */
  /* This is an approximation since F0 doesn't have DWT cycle counter */
  return elapsed_ms * (SystemCoreClock / 1000u);
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
