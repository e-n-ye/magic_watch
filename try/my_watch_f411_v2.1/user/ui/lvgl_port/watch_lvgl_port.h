#ifndef WATCH_LVGL_PORT_H
#define WATCH_LVGL_PORT_H

#include <stdint.h>

#include "app/input/watch_input_intent.h"

typedef struct {
    uint32_t refresh_per_sec;
    uint32_t flush_per_sec;
    uint32_t pixels_per_sec;
    uint32_t handler_per_sec;
    uint32_t last_refresh_ms;
    uint32_t last_flush_ms;
} watch_lvgl_perf_snapshot_t;

void watch_lvgl_port_init(void);
void watch_lvgl_port_feed_input_intent(watch_input_intent_t intent);
void watch_lvgl_port_task(void);
void watch_lvgl_port_get_perf_snapshot(watch_lvgl_perf_snapshot_t *snapshot);

#endif /* WATCH_LVGL_PORT_H */
