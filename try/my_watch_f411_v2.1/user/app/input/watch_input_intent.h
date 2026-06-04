#ifndef WATCH_INPUT_INTENT_H
#define WATCH_INPUT_INTENT_H

#include "services/input/watch_input_service.h"

typedef enum {
    WATCH_INPUT_INTENT_NONE = 0,
    WATCH_INPUT_INTENT_CROWN_CLOCKWISE,
    WATCH_INPUT_INTENT_CROWN_COUNTERCLOCKWISE,
    WATCH_INPUT_INTENT_CONFIRM,
    WATCH_INPUT_INTENT_LONG_PRESS,
} watch_input_intent_t;

watch_input_intent_t watch_input_intent_from_event(watch_input_event_t event);

#endif /* WATCH_INPUT_INTENT_H */
