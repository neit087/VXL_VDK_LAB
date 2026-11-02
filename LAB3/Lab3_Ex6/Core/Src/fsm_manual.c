#include "fsm_manual.h"
#include "button.h"

static volatile Lab3Mode s_current_mode = LAB3_MODE_NORMAL;

void FSM_Manual_Init(void)
{
    s_current_mode = LAB3_MODE_NORMAL;
}

void FSM_Manual_Run(void)
{
    if (Button_WasPressed(BUTTON_MODE) != 0U)
    {
        s_current_mode = (s_current_mode == LAB3_MODE_SET_GREEN)
                         ? LAB3_MODE_NORMAL
                         : (Lab3Mode)((uint8_t)s_current_mode + 1U);
    }

    /* INC and SET events are reserved for Exercises 7-9. */
}

Lab3Mode FSM_Manual_GetMode(void)
{
    return s_current_mode;
}
