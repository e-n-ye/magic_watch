#include "app/input/watch_input_intent.h"

watch_input_intent_t watch_input_intent_from_event(watch_input_event_t event)
{
    switch (event) {
    case WATCH_INPUT_ENCODER_CW:
        return WATCH_INPUT_INTENT_CROWN_CLOCKWISE;
    case WATCH_INPUT_ENCODER_CCW:
        return WATCH_INPUT_INTENT_CROWN_COUNTERCLOCKWISE;
    case WATCH_INPUT_ENCODER_PRESS:
        return WATCH_INPUT_INTENT_CONFIRM;
    case WATCH_INPUT_ENCODER_LONG:
        return WATCH_INPUT_INTENT_LONG_PRESS;
    default:
        return WATCH_INPUT_INTENT_NONE;
    }
}
