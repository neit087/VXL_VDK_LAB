/*EX6*/
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    : main.c
 * @brief   : Lab 3 - Exercise 6
 *            Mode display on 4 seven-segment digits and 2 Hz mode lamps.
 *            TIM2 is 10 ms for button debounce, SysTick drives 7-segment scan.
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "button.h"
#include "fsm_manual.h"
#include "traffic_light.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Read/debounce buttons every 10 ms using TIM2. */
#define TIMER_INTERRUPT_MS 10U

/* TIM2 input clock = 8 MHz; PSC = 799 -> 10,000 counts per second. */
#define TIMER_COUNTS_PER_MS 10U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile uint32_t timer_ticks = 0U;
/* Observe this variable in debugger Live Expressions. */
volatile uint8_t mode_debug = 1U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
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

  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  SoftwareTimer_Init();
  Button_Init();
  FSM_Manual_Init();
  /* PA13-PA15 are used as traffic LEDs; disable SWJ in Proteus.
   * This also disables SWD/JTAG debugging on real hardware. */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE();
  TrafficLight_Init();

  mode_debug = (uint8_t)FSM_Manual_GetMode();

  /* Change ARR without rewriting the LED timing logic. */
  __HAL_TIM_SET_AUTORELOAD(
      &htim2,
	  TIMER_INTERRUPT_MS * TIMER_COUNTS_PER_MS - 1U);
 __HAL_TIM_SET_COUNTER(&htim2, 0U);
 __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
 if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE END 2 */

   /* Infinite loop */
   /* USER CODE BEGIN WHILE */
   while (1)
   {
     /* USER CODE END WHILE */

     /* USER CODE BEGIN 3 */
     /* Button MODE selects 1 -> 2 -> 3 -> 4 -> 1 (debounced in ISR). */
     FSM_Manual_Run();
     mode_debug = (uint8_t)FSM_Manual_GetMode();

     /* Ex6 output: refresh 7-segment display and color blink non-blockingly. */
     TrafficLight_Run(mode_debug);
     /* USER CODE END 3 */
   }
 }

 /**
  * @brief System Clock Configuration: HSI 8 MHz, no PLL.
  */
 void SystemClock_Config(void)
 {
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
     Error_Handler();
   }

   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
   {
     Error_Handler();
   }
 }

 /**
  * @brief TIM2 Initialization Function
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
   htim2.Init.Prescaler = 799;
   htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim2.Init.Period = 99;
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
    */
   static void MX_GPIO_Init(void)
   {
     GPIO_InitTypeDef GPIO_InitStruct = {0};

     __HAL_RCC_GPIOC_CLK_ENABLE();
     __HAL_RCC_GPIOA_CLK_ENABLE();
     __HAL_RCC_GPIOB_CLK_ENABLE();

     /* PNP digit enables are active-low: HIGH = disabled. */
     HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin,
                       GPIO_PIN_SET);

     /* Traffic LEDs OFF: three bottom lamps are active-low. */
     HAL_GPIO_WritePin(GPIOA,
                       GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                       GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_13 |
                       GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
     HAL_GPIO_WritePin(GPIOA,
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_SET);

     /* Common-anode segment lines are active-low: HIGH = disabled. */
     HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin |
                              SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_SET);

     GPIO_InitStruct.Pin = BTN_MODE_Pin | BTN_INC_Pin | BTN_SET_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
     GPIO_InitStruct.Pull = GPIO_PULLUP;
     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

     GPIO_InitStruct.Pin = EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

     GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                           GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

     GPIO_InitStruct.Pin = SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin |
                           SEG4_Pin | SEG5_Pin | SEG6_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   }

   /* USER CODE BEGIN 4 */
   void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
   {
     if (htim->Instance == TIM2)
     {
       timer_ticks++;
       SoftwareTimer_TickISR(TIMER_INTERRUPT_MS);
       Button_Scan10ms();
     }
   }
   /* USER CODE END 4 */

   /**
    * @brief Error Handler
    */
   void Error_Handler(void)
   {
     /* USER CODE BEGIN Error_Handler_Debug */
     __disable_irq();
     while (1)
     {
     }
     /* USER CODE END Error_Handler_Debug */
   }

   #ifdef USE_FULL_ASSERT
   void assert_failed(uint8_t *file, uint32_t line)
   {
     /* USER CODE BEGIN 6 */
     (void)file;
     (void)line;
     /* USER CODE END 6 */
   }
   #endif /* USE_FULL_ASSERT */

