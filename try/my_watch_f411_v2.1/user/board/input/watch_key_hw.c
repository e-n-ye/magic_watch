#include "watch_key_hw.h"

#include "main.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState pressed_level;
} watch_key_pin_t;

static const watch_key_pin_t s_key_pins[WATCH_KEY_COUNT] = {
    {KEY_BACK_GPIO_Port, KEY_BACK_Pin, GPIO_PIN_RESET},
    {KEY_WAKE_GPIO_Port, KEY_WAKE_Pin, GPIO_PIN_SET},
    {ENCODER_KEY_GPIO_Port, ENCODER_KEY_Pin, GPIO_PIN_RESET},
};

uint8_t watch_key_hw_is_pressed(watch_key_id_t key)
{
    if ((uint32_t)key >= (uint32_t)WATCH_KEY_COUNT) {
        return 0U;
    }

    return (HAL_GPIO_ReadPin(s_key_pins[key].port, s_key_pins[key].pin) == s_key_pins[key].pressed_level) ? 1U : 0U;
}
