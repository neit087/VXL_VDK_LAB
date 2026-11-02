#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H
#include <stdint.h>

void SoftwareTimer_Init(void);
void SoftwareTimer_TickISR(uint32_t delta_ms);
uint32_t SoftwareTimer_Millis(void);

#endif
