/* EX3 - GPIO and TIM2 interrupt; blink PA4 to distinguish from EX4 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    : main.c
 * @brief   : Lab 3 - Exercise 3
 *            GPIO configuration and TIM2 interrupt every 10 ms.
 *            Test LED on PA4 blinks at 1 Hz (500 ms ON, 500 ms OFF).
 *            Other traffic LEDs remain OFF for this test.
 ******************************************************************************
 */
/* USER CODE END Header */

#include "main.h"

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DIGIT_ENABLE_PINS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define TRAFFIC_LED_PINS  (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
                           GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#define SEGMENT_PINS      (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)

#define EX3_TEST_LED_PIN      GPIO_PIN_4
#define EX3_TOGGLE_EVERY_ISR 50U  /* 50 x 10 ms = 500 ms */
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile uint32_t timer_ticks = 0U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();

    /* USER CODE BEGIN 2 */
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE END 2 */

    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        /* USER CODE BEGIN 3 */
        /* Exercise 3: TIM2 callback counts interrupts and blinks PA4 only. */
    }
    /* USER CODE END 3 */
}

/**
 * @brief  System Clock Configuration: HSI = 8 MHz.
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

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                   RCC_CLOCKTYPE_SYSCLK |
                                   RCC_CLOCKTYPE_PCLK1 |
                                   RCC_CLOCKTYPE_PCLK2;
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
 * @brief TIM2 at 10 ms: TIM2 clock 8 MHz; PSC = 7999; ARR = 9.
 */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9;
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
}

/**
 * @brief Configure PA4-PA15 traffic LEDs, PA0-PA3 digit enables,
 *        PB0-PB6 segments, and PC13-PC15 buttons.
 * @note  This test assumes the traffic LEDs are ACTIVE HIGH, matching
 *        the observed behavior where setting all PA pins HIGH lit them.
 *        If your schematic has ACTIVE-LOW LEDs, invert those pin levels.
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    /* PA13/PA14/PA15 are also SWJ pins. Free them for traffic LEDs.
       This disables both SWD and JTAG debug while this firmware runs. */
    __HAL_AFIO_REMAP_SWJ_DISABLE();

    /* Four common-anode digits through PNP: HIGH means OFF. */
    HAL_GPIO_WritePin(GPIOA, DIGIT_ENABLE_PINS, GPIO_PIN_SET);

    /* Traffic LEDs in the current active-high wiring: LOW means OFF.
       Only PA4 is later toggled by the timer interrupt. */
    HAL_GPIO_WritePin(GPIOA, TRAFFIC_LED_PINS, GPIO_PIN_RESET);

    /* Common-anode seven-segment segments: HIGH means OFF. */
    HAL_GPIO_WritePin(GPIOB, SEGMENT_PINS, GPIO_PIN_SET);

    /* PC13-PC15: MODE / INC / SET, released = HIGH (pull-up). */
    GPIO_InitStruct.Pin = BTN_MODE_Pin | BTN_INC_Pin | BTN_SET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PA0-PA3: enable pins for seven-segment digit transistors. */
    GPIO_InitStruct.Pin = DIGIT_ENABLE_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA4-PA15: traffic LED outputs. */
    GPIO_InitStruct.Pin = TRAFFIC_LED_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PB0-PB6: seven-segment outputs. */
    GPIO_InitStruct.Pin = SEGMENT_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/**
 * @brief TIM2 interrupt callback: 10 ms each, PA4 toggle every 500 ms.
 *        PA4 blinking period = 1 s = 1 Hz.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        timer_ticks++;
        if ((timer_ticks % EX3_TOGGLE_EVERY_ISR) == 0U)
        {
            HAL_GPIO_TogglePin(GPIOA, EX3_TEST_LED_PIN);
        }
    }
}
/* USER CODE END 4 */

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
#endif

/************************ END OF FILE ************************/
