#ifndef WATCH_INPUT_SERVICE_H
#define WATCH_INPUT_SERVICE_H

#include <stdint.h>

typedef enum {
    WATCH_INPUT_NONE = 0,
    WATCH_INPUT_BACK_SHORT,
    WATCH_INPUT_BACK_LONG,
    WATCH_INPUT_WAKE_SHORT,
    WATCH_INPUT_WAKE_LONG,
    WATCH_INPUT_ENCODER_PRESS,
    WATCH_INPUT_ENCODER_LONG,
    WATCH_INPUT_ENCODER_CW,
    WATCH_INPUT_ENCODER_CCW,
} watch_input_event_t;

void watch_input_service_init(void);
void watch_input_service_scan_10ms(void);
watch_input_event_t watch_input_service_get_event(void);

#endif /* WATCH_INPUT_SERVICE_H */
