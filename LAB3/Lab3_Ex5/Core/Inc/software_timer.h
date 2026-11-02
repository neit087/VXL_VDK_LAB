#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H
#include <stdint.h>

void SoftwareTimer_Init(void);
void SoftwareTimer_TickISR(uint32_t elapsed_ms);
uint32_t SoftwareTimer_Millis(void);
uint8_t SoftwareTimer_Elapsed(uint32_t *last_ms, uint32_t interval_ms);

#endif /* SOFTWARE_TIMER_H */
