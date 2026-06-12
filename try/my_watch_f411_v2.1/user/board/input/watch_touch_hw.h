#ifndef WATCH_TOUCH_HW_H
#define WATCH_TOUCH_HW_H

#include <stdint.h>

typedef enum {
    WATCH_TOUCH_GESTURE_NONE = 0,
    WATCH_TOUCH_GESTURE_SLIDE_DOWN = 1,
    WATCH_TOUCH_GESTURE_SLIDE_UP = 2,
    WATCH_TOUCH_GESTURE_SLIDE_LEFT = 3,
    WATCH_TOUCH_GESTURE_SLIDE_RIGHT = 4,
    WATCH_TOUCH_GESTURE_CLICK = 5,
    WATCH_TOUCH_GESTURE_DOUBLE_CLICK = 11,
    WATCH_TOUCH_GESTURE_LONG_PRESS = 12
} watch_touch_gesture_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t finger_num;
    watch_touch_gesture_t gesture;
} watch_touch_sample_t;

void watch_touch_hw_init(void);
uint8_t watch_touch_hw_read(watch_touch_sample_t *sample);

#endif /* WATCH_TOUCH_HW_H */
