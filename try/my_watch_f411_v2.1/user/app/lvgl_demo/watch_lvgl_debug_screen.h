#ifndef WATCH_LVGL_DEBUG_SCREEN_H
#define WATCH_LVGL_DEBUG_SCREEN_H

#include "app/input/watch_input_intent.h"

void watch_lvgl_debug_screen_init(void);
void watch_lvgl_debug_screen_on_input_intent(watch_input_intent_t intent);
void watch_lvgl_debug_screen_task(void);

#endif /* WATCH_LVGL_DEBUG_SCREEN_H */
