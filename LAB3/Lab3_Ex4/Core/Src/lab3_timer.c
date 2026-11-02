#include "lab3_timer.h"
#define BLINK_TOGGLE_MS 250U
static uint32_t accumulated_ms = 0U;

void Lab3Timer_Init(void)
{
    accumulated_ms = 0U;
}

uint8_t Lab3Timer_Update(uint32_t elapsed_ms)
{
    uint8_t toggle = 0U;
    accumulated_ms += elapsed_ms;
    while (accumulated_ms >= BLINK_TOGGLE_MS)
    {
        accumulated_ms -= BLINK_TOGGLE_MS;
        toggle ^= 1U;
    }
    return toggle;
}
