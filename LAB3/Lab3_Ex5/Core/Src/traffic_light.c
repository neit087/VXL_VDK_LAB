#include "traffic_light.h"
#include "main.h"

#define TRAFFIC_TEST_PORT GPIOA
#define TRAFFIC_TEST_PIN  GPIO_PIN_5

void TrafficLight_Init(void)
{
    HAL_GPIO_WritePin(TRAFFIC_TEST_PORT, TRAFFIC_TEST_PIN, GPIO_PIN_RESET);
}

void TrafficLight_ToggleTestLED(void)
{
    HAL_GPIO_TogglePin(TRAFFIC_TEST_PORT, TRAFFIC_TEST_PIN);
}
