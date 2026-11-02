#include "traffic_light.h"
#include "main.h"
#define RED_PINS    (GPIO_PIN_15 | GPIO_PIN_6  | GPIO_PIN_9  | GPIO_PIN_12)
#define AMBER_PINS  (GPIO_PIN_4  | GPIO_PIN_7  | GPIO_PIN_10 | GPIO_PIN_13)
#define GREEN_PINS  (GPIO_PIN_5  | GPIO_PIN_8  | GPIO_PIN_11 | GPIO_PIN_14)
#define ALL_LAMPS   (RED_PINS | AMBER_PINS | GREEN_PINS)
#define ACTIVE_LOW_LAMPS 0U
#define ALL_DIGITS (EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin)
#define ALL_SEGMENTS (SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | \
                      SEG4_Pin | SEG5_Pin | SEG6_Pin)
#define DIGIT_SCAN_MS 2U
#define BLINK_TOGGLE_MS 250U

static const uint8_t s_num_to_segments[10] = {
    0x3FU, /* 0: abcdef */
    0x06U, /* 1: bc */
    0x5BU, /* 2: abdeg */
    0x4FU, /* 3: abcdg */
    0x66U, /* 4: bcfg */
    0x6DU, /* 5: acdfg */
    0x7DU, /* 6: acdefg */
    0x07U, /* 7: abc */
    0x7FU, /* 8: abcdefg */
    0x6FU  /* 9: abcdfg */
};

static const uint16_t s_digit_pins[4] = {
    EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin
};

static const uint16_t s_segment_pins[7] = {
    SEG0_Pin, SEG1_Pin, SEG2_Pin, SEG3_Pin,
    SEG4_Pin, SEG5_Pin, SEG6_Pin
};

static uint8_t s_display_digits[4] = {0U, 1U, 0U, 0U};
static uint8_t s_scan_pos = 0U;
static uint8_t s_current_mode = 0U;
static uint8_t s_blink_on = 0U;
static uint32_t s_last_blink_ms = 0U;
static uint32_t s_last_scan_ms = 0U;

static uint16_t TrafficLight_ColorMask(uint8_t mode)
{
    switch (mode)
    {
        case 2U: return RED_PINS;
        case 3U: return AMBER_PINS;
        case 4U: return GREEN_PINS;
        default: return 0U;
    }
}

static void TrafficLight_ApplyLamps(uint16_t lit_pins)
{
    /* Turn OFF all lamps based on their physical polarity. */
    HAL_GPIO_WritePin(GPIOA, (uint16_t)(ALL_LAMPS & ~ACTIVE_LOW_LAMPS),
                      GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, (uint16_t)(ALL_LAMPS & ACTIVE_LOW_LAMPS),
                      GPIO_PIN_SET);

    /* Turn ON only the selected color group when the blink is HIGH. */
    uint16_t high_on = (uint16_t)(lit_pins & ~ACTIVE_LOW_LAMPS);
    uint16_t low_on = (uint16_t)(lit_pins & ACTIVE_LOW_LAMPS);
    if (high_on != 0U)
    {
        HAL_GPIO_WritePin(GPIOA, high_on, GPIO_PIN_SET);
    }
    if (low_on != 0U)
    {
        HAL_GPIO_WritePin(GPIOA, low_on, GPIO_PIN_RESET);
    }
}

static void TrafficLight_DisplayOneDigit(void)
{
    uint8_t value = s_display_digits[s_scan_pos];
    uint8_t bitmap = (value <= 9U) ? s_num_to_segments[value] : 0U;
    uint16_t on_segments = 0U;

    /* All digit transistors OFF before changing the shared segment bus. */
    HAL_GPIO_WritePin(GPIOA, ALL_DIGITS, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, ALL_SEGMENTS, GPIO_PIN_SET);

    for (uint8_t bit = 0U; bit < 7U; ++bit)
    {
        if ((bitmap & (uint8_t)(1U << bit)) != 0U)
        {
            on_segments |= s_segment_pins[bit];
        }
    }

    /* Segments ON = LOW. Digit ON = LOW (PNP transistor). */
    HAL_GPIO_WritePin(GPIOB, on_segments, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, s_digit_pins[s_scan_pos], GPIO_PIN_RESET);
    s_scan_pos = (uint8_t)((s_scan_pos + 1U) % 4U);
}

void TrafficLight_Init(void)
{
    uint32_t now = HAL_GetTick();
    s_current_mode = 0U;
    s_blink_on = 0U;
    s_scan_pos = 0U;
    s_last_blink_ms = now;
    s_last_scan_ms = now;
    s_display_digits[0] = 0U;
    s_display_digits[1] = 1U;
    s_display_digits[2] = 0U;
    s_display_digits[3] = 0U;
    HAL_GPIO_WritePin(GPIOA, ALL_DIGITS, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, ALL_SEGMENTS, GPIO_PIN_SET);
    TrafficLight_ApplyLamps(0U);
}

void TrafficLight_Run(uint8_t mode)
{
    uint32_t now = HAL_GetTick();
    if ((mode < 1U) || (mode > 4U)) mode = 1U;

    if (s_current_mode != mode)
    {
        s_current_mode = mode;
        s_blink_on = (mode >= 2U) ? 1U : 0U;
        s_last_blink_ms = now;
        /* Two digits for MODE, two placeholder digits for duration. */
        s_display_digits[0] = 0U;
        s_display_digits[1] = mode;
        s_display_digits[2] = 0U;
        s_display_digits[3] = 0U;
        TrafficLight_ApplyLamps(s_blink_on ? TrafficLight_ColorMask(mode) : 0U);
    }

    if ((mode >= 2U) && ((uint32_t)(now - s_last_blink_ms) >= BLINK_TOGGLE_MS))
    {
        s_last_blink_ms += BLINK_TOGGLE_MS;
        s_blink_on ^= 1U;
        TrafficLight_ApplyLamps(s_blink_on ? TrafficLight_ColorMask(mode) : 0U);
    }

    if ((uint32_t)(now - s_last_scan_ms) >= DIGIT_SCAN_MS)
    {
        s_last_scan_ms = now;
        TrafficLight_DisplayOneDigit();
    }
}
