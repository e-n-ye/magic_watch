#include "watch_encoder_hw.h"

#include "tim.h"

static uint16_t s_last_count;
static uint32_t s_last_tick;

void watch_encoder_hw_init(void)
{
    (void)HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    watch_encoder_hw_reset();
}

int16_t watch_encoder_hw_get_delta(void)
{
    uint16_t now_count;
    int16_t delta;
    uint32_t now_tick = HAL_GetTick();

    if ((now_tick - s_last_tick) < 30U) {
        return 0;
    }

    now_count = (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
    delta = (int16_t)(now_count - s_last_count);

    if ((delta > -2) && (delta < 2)) {
        return 0;
    }

    s_last_count = now_count;
    s_last_tick = now_tick;

    return (delta > 0) ? 1 : -1;
}

void watch_encoder_hw_reset(void)
{
    __HAL_TIM_SET_COUNTER(&htim4, 32768U);
    s_last_count = 32768U;
    s_last_tick = HAL_GetTick();
}
