#include "software_timer.h"

static volatile uint32_t elapsed_total_ms = 0U;

void SoftwareTimer_Init(void)
{
    elapsed_total_ms = 0U;
}

void SoftwareTimer_TickISR(uint32_t elapsed_ms)
{
    elapsed_total_ms += elapsed_ms;
}

uint32_t SoftwareTimer_Millis(void)
{
    return elapsed_total_ms;
}

uint8_t SoftwareTimer_Elapsed(uint32_t *last_ms, uint32_t interval_ms)
{
    if ((last_ms == 0) || (interval_ms == 0U))
    {
        return 0U;
    }
    uint32_t current_ms = SoftwareTimer_Millis();
    if ((uint32_t)(current_ms - *last_ms) >= interval_ms)
    {
        *last_ms += interval_ms;
        return 1U;
    }

    return 0U;
}
