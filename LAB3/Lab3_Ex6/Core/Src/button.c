#include "button.h"
#include "main.h"

/* Three equal raw samples x 10 ms -> 20-30 ms to accept an edge. */
#define DEBOUNCE_SAMPLES 3U

static const uint16_t s_button_pins[BUTTON_COUNT] =
{
    BTN_MODE_Pin, BTN_INC_Pin, BTN_SET_Pin
};

static uint8_t s_last_raw[BUTTON_COUNT];
static volatile uint8_t s_stable_pressed[BUTTON_COUNT];
static uint8_t s_same_samples[BUTTON_COUNT];
static volatile uint8_t s_pressed_event[BUTTON_COUNT];

static uint8_t Button_ReadRaw(uint8_t i)
{
    return (HAL_GPIO_ReadPin(GPIOC, s_button_pins[i]) == GPIO_PIN_RESET)
           ? 1U : 0U;
}

void Button_Init(void)
{
    for (uint8_t i = 0U; i < (uint8_t)BUTTON_COUNT; i++)
    {
        uint8_t raw = Button_ReadRaw(i);
        s_last_raw[i] = raw;
        s_stable_pressed[i] = raw;
        s_same_samples[i] = 0U;
        s_pressed_event[i] = 0U;
    }
}

void Button_Scan10ms(void)
{
    for (uint8_t i = 0U; i < (uint8_t)BUTTON_COUNT; i++)
    {
        uint8_t raw = Button_ReadRaw(i);

        if (raw != s_last_raw[i])
        {
            s_last_raw[i] = raw;
            s_same_samples[i] = 1U;
        }
        else if (s_same_samples[i] < DEBOUNCE_SAMPLES)
        {
            s_same_samples[i]++;
        }

        if ((s_same_samples[i] >= DEBOUNCE_SAMPLES) &&
            (raw != s_stable_pressed[i]))
        {
            s_stable_pressed[i] = raw;

            /* Only the released -> pressed edge generates an event. */
            if (raw != 0U)
            {
                s_pressed_event[i] = 1U;
            }
        }
    }
}

uint8_t Button_WasPressed(ButtonId id)
{
    if ((uint32_t)id >= (uint32_t)BUTTON_COUNT)
    {
        return 0U;
    }

    /* The timer ISR produces events, main() consumes them. */
    uint32_t irq_state = __get_PRIMASK();
    __disable_irq();

    uint8_t result = s_pressed_event[(uint8_t)id];
    s_pressed_event[(uint8_t)id] = 0U;

    __set_PRIMASK(irq_state);
    return result;
}

uint8_t Button_IsPressed(ButtonId id)
{
    if ((uint32_t)id >= (uint32_t)BUTTON_COUNT)
    {
        return 0U;
    }

    return s_stable_pressed[(uint8_t)id];
}
