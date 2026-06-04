#ifndef WATCH_KEY_HW_H
#define WATCH_KEY_HW_H

#include <stdint.h>

typedef enum {
    WATCH_KEY_BACK = 0,
    WATCH_KEY_WAKE,
    WATCH_KEY_ENCODER,
    WATCH_KEY_COUNT
} watch_key_id_t;

uint8_t watch_key_hw_is_pressed(watch_key_id_t key);

#endif /* WATCH_KEY_HW_H */
