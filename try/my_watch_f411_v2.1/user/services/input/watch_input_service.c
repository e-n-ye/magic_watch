#include "watch_input_service.h"

#include "board/input/watch_encoder_hw.h"
#include "board/input/watch_key_hw.h"

#define INPUT_FIFO_SIZE          12U
#define KEY_DEBOUNCE_TICKS      5U
#define KEY_LONG_PRESS_TICKS    100U

typedef struct {
    uint8_t stable_pressed;
    uint8_t filter;
    uint16_t press_ticks;
    uint8_t long_sent;
} key_state_t;

static key_state_t s_keys[WATCH_KEY_COUNT];
static watch_input_event_t s_fifo[INPUT_FIFO_SIZE];
static uint8_t s_read;
static uint8_t s_write;

static void input_put(watch_input_event_t event)
{
    uint8_t next = (uint8_t)((s_write + 1U) % INPUT_FIFO_SIZE);

    if (next == s_read) {
        return;
    }

    s_fifo[s_write] = event;
    s_write = next;
}

static watch_input_event_t short_event_for_key(watch_key_id_t key)
{
    switch (key) {
    case WATCH_KEY_BACK:
        return WATCH_INPUT_BACK_SHORT;
    case WATCH_KEY_WAKE:
        return WATCH_INPUT_WAKE_SHORT;
    case WATCH_KEY_ENCODER:
        return WATCH_INPUT_ENCODER_PRESS;
    default:
        return WATCH_INPUT_NONE;
    }
}

static watch_input_event_t long_event_for_key(watch_key_id_t key)
{
    switch (key) {
    case WATCH_KEY_BACK:
        return WATCH_INPUT_BACK_LONG;
    case WATCH_KEY_WAKE:
        return WATCH_INPUT_WAKE_LONG;
    case WATCH_KEY_ENCODER:
        return WATCH_INPUT_ENCODER_LONG;
    default:
        return WATCH_INPUT_NONE;
    }
}

static void scan_key(watch_key_id_t key)
{
    key_state_t *state = &s_keys[key];
    uint8_t raw_pressed = watch_key_hw_is_pressed(key);
    uint8_t was_long_sent;

    if (raw_pressed != state->stable_pressed) {
        if (++state->filter >= KEY_DEBOUNCE_TICKS) {
            was_long_sent = state->long_sent;
            state->stable_pressed = raw_pressed;
            state->filter = 0U;
            state->press_ticks = 0U;
            state->long_sent = 0U;

            if ((raw_pressed == 0U) && (was_long_sent == 0U)) {
                input_put(short_event_for_key(key));
            }
        }
        return;
    }

    state->filter = 0U;

    if (state->stable_pressed != 0U) {
        if (state->press_ticks < KEY_LONG_PRESS_TICKS) {
            state->press_ticks++;
        }
        if ((state->press_ticks >= KEY_LONG_PRESS_TICKS) && (state->long_sent == 0U)) {
            state->long_sent = 1U;
            input_put(long_event_for_key(key));
        }
    }
}

void watch_input_service_init(void)
{
    uint32_t i;

    s_read = 0U;
    s_write = 0U;

    for (i = 0U; i < (uint32_t)WATCH_KEY_COUNT; ++i) {
        s_keys[i].stable_pressed = watch_key_hw_is_pressed((watch_key_id_t)i);
        s_keys[i].filter = 0U;
        s_keys[i].press_ticks = 0U;
        s_keys[i].long_sent = 0U;
    }

    watch_encoder_hw_init();
}

void watch_input_service_scan_10ms(void)
{
    int16_t encoder_delta;

    scan_key(WATCH_KEY_BACK);
    scan_key(WATCH_KEY_WAKE);
    scan_key(WATCH_KEY_ENCODER);

    encoder_delta = watch_encoder_hw_get_delta();
    if (encoder_delta > 0) {
        input_put(WATCH_INPUT_ENCODER_CW);
    } else if (encoder_delta < 0) {
        input_put(WATCH_INPUT_ENCODER_CCW);
    }
}

watch_input_event_t watch_input_service_get_event(void)
{
    watch_input_event_t event;

    if (s_read == s_write) {
        return WATCH_INPUT_NONE;
    }

    event = s_fifo[s_read];
    s_read = (uint8_t)((s_read + 1U) % INPUT_FIFO_SIZE);
    return event;
}
