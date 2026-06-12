#ifndef WATCH_LVGL_PORT_H
#define WATCH_LVGL_PORT_H

#include <stdint.h>

#include "app/input/watch_input_intent.h"
#include "lvgl.h"

typedef struct {
    uint32_t refresh_per_sec;
    uint32_t flush_per_sec;
    uint32_t pixels_per_sec;
    uint32_t handler_per_sec;
    uint32_t last_refresh_ms;
    uint32_t last_flush_ms;
    uint32_t last_flush_pixels;
    uint32_t max_flush_pixels_per_sec_window;
    uint32_t last_flush_area_permille;
    uint32_t max_flush_area_permille_per_sec_window;
    uint32_t last_convert_ms;
    uint32_t max_convert_ms_per_sec_window;
    uint32_t last_flush_bytes;
    uint32_t last_transfer_result;
    uint32_t dma_flush_pending;
    uint32_t blocking_count_per_sec;
    uint32_t dma_count_per_sec;
    uint32_t failed_count_per_sec;
    uint32_t dma_fallback_count_per_sec;
    uint32_t last_dma_wait_ms;
    uint32_t max_dma_wait_ms_per_sec_window;
} watch_lvgl_perf_snapshot_t;

void watch_lvgl_port_init(void);
void watch_lvgl_port_feed_input_intent(watch_input_intent_t intent);
void watch_lvgl_port_set_group(lv_group_t *group);
void watch_lvgl_port_task(void);
void watch_lvgl_port_get_perf_snapshot(watch_lvgl_perf_snapshot_t *snapshot);

#endif /* WATCH_LVGL_PORT_H */
