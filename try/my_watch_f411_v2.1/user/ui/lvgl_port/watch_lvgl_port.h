#ifndef WATCH_LVGL_PORT_H
#define WATCH_LVGL_PORT_H

#include "app/input/watch_input_intent.h"

void watch_lvgl_port_init(void);
void watch_lvgl_port_feed_input_intent(watch_input_intent_t intent);
void watch_lvgl_port_task(void);

#endif /* WATCH_LVGL_PORT_H */
