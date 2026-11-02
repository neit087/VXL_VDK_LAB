#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

/* Buttons are active-low with pull-up on GPIOC. */
typedef enum
{
    BUTTON_MODE = 0,  /* PC13 */
    BUTTON_INC  = 1,  /* PC14 */
    BUTTON_SET  = 2,  /* PC15 */
    BUTTON_COUNT = 3
} ButtonId;

void Button_Init(void);
/* Call exactly once every 10 ms from TIM2 callback. */
void Button_Scan10ms(void);
/* Return 1 once per debounced press, otherwise 0. */
uint8_t Button_WasPressed(ButtonId id);
/* Current debounced physical level: 1 = pressed. */
uint8_t Button_IsPressed(ButtonId id);

#endif /* BUTTON_H */
