#include "software_timer.h"
static volatile uint32_t system_time_ms = 0U;

void SoftwareTimer_Init(void)
{
    system_time_ms = 0U;
}

void SoftwareTimer_TickISR(uint32_t delta_ms)
{
    system_time_ms += delta_ms;
}

uint32_t SoftwareTimer_Millis(void)
{
    return system_time_ms;
}
