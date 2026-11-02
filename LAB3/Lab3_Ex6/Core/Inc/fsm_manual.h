#ifndef FSM_MANUAL_H
#define FSM_MANUAL_H

#include <stdint.h>

typedef enum
{
    LAB3_MODE_NORMAL    = 1,
    LAB3_MODE_SET_RED   = 2,
    LAB3_MODE_SET_AMBER = 3,
    LAB3_MODE_SET_GREEN = 4
} Lab3Mode;

void FSM_Manual_Init(void);
void FSM_Manual_Run(void);
Lab3Mode FSM_Manual_GetMode(void);

#endif /* FSM_MANUAL_H */
