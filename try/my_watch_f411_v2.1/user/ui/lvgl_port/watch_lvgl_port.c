#include "watch_lvgl_port.h"

#include <stdint.h>

#include "board/display/watch_lcd.h"
#include "config/user_config.h"
#include "lvgl.h"

#define WATCH_LVGL_DRAW_BUF_LINES 20U

static lv_color_t s_draw_buf_1[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
static lv_color_t s_draw_buf_2[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
static lv_disp_draw_buf_t s_draw_buf;
static lv_disp_drv_t s_disp_drv;
static lv_indev_drv_t s_encoder_drv;
static int16_t s_encoder_diff;
static uint8_t s_encoder_press_pulse;
static uint8_t s_lvgl_port_initialized;
static uint32_t s_perf_window_start_ms;
static uint32_t s_flush_count_accum;
static uint32_t s_pixels_accum;
static uint32_t s_handler_count_accum;
static watch_lvgl_perf_snapshot_t s_perf_snapshot;

static uint32_t scale_to_per_sec(uint32_t value, uint32_t elapsed_ms)
{
    uint64_t scaled;

    if (elapsed_ms == 0U) {
        elapsed_ms = 1U;
    }

    scaled = ((uint64_t)value * 1000ULL) / (uint64_t)elapsed_ms;
    return (scaled > UINT32_MAX) ? UINT32_MAX : (uint32_t)scaled;
}

static void watch_lvgl_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint32_t start_ms;

    (void)disp_drv;

    if ((area == 0) || (color_p == 0)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    if ((area->x2 < 0) || (area->y2 < 0) ||
        (area->x1 >= (lv_coord_t)WATCH_LCD_WIDTH) ||
        (area->y1 >= (lv_coord_t)WATCH_LCD_HEIGHT)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    x = (area->x1 < 0) ? 0U : (uint16_t)area->x1;
    y = (area->y1 < 0) ? 0U : (uint16_t)area->y1;
    width = (uint16_t)(area->x2 - area->x1 + 1);
    height = (uint16_t)(area->y2 - area->y1 + 1);

    start_ms = lv_tick_get();
    watch_lcd_draw_rgb565(x, y, width, height, (const uint16_t *)color_p);
    s_perf_snapshot.last_flush_ms = lv_tick_elaps(start_ms);
    s_flush_count_accum++;
    s_pixels_accum += (uint32_t)width * (uint32_t)height;
    lv_disp_flush_ready(disp_drv);
}

static void watch_lvgl_encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    (void)indev_drv;

    data->enc_diff = s_encoder_diff;
    s_encoder_diff = 0;

    if (s_encoder_press_pulse != 0U) {
        data->state = LV_INDEV_STATE_PR;
        s_encoder_press_pulse = 0U;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void watch_lvgl_port_init(void)
{
    if (s_lvgl_port_initialized != 0U) {
        return;
    }

    lv_init();

    lv_disp_draw_buf_init(&s_draw_buf,
                          s_draw_buf_1,
                          s_draw_buf_2,
                          (uint32_t)WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res = WATCH_LCD_WIDTH;
    s_disp_drv.ver_res = WATCH_LCD_HEIGHT;
    s_disp_drv.flush_cb = watch_lvgl_flush;
    s_disp_drv.draw_buf = &s_draw_buf;
    (void)lv_disp_drv_register(&s_disp_drv);

    lv_indev_drv_init(&s_encoder_drv);
    s_encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    s_encoder_drv.read_cb = watch_lvgl_encoder_read;
    (void)lv_indev_drv_register(&s_encoder_drv);

    s_perf_window_start_ms = lv_tick_get();
    s_lvgl_port_initialized = 1U;
}

void watch_lvgl_port_feed_input_intent(watch_input_intent_t intent)
{
    switch (intent) {
    case WATCH_INPUT_INTENT_CROWN_CLOCKWISE:
        if (s_encoder_diff < INT16_MAX) {
            s_encoder_diff++;
        }
        break;
    case WATCH_INPUT_INTENT_CROWN_COUNTERCLOCKWISE:
        if (s_encoder_diff > INT16_MIN) {
            s_encoder_diff--;
        }
        break;
    case WATCH_INPUT_INTENT_CONFIRM:
    case WATCH_INPUT_INTENT_LONG_PRESS:
        s_encoder_press_pulse = 1U;
        break;
    default:
        break;
    }
}

void watch_lvgl_port_task(void)
{
    uint32_t elapsed_ms;

    if (s_lvgl_port_initialized == 0U) {
        return;
    }

    (void)lv_timer_handler();
    s_handler_count_accum++;

    elapsed_ms = lv_tick_elaps(s_perf_window_start_ms);
    if (elapsed_ms >= 1000U) {
        s_perf_snapshot.flush_per_sec = scale_to_per_sec(s_flush_count_accum, elapsed_ms);
        s_perf_snapshot.pixels_per_sec = scale_to_per_sec(s_pixels_accum, elapsed_ms);
        s_perf_snapshot.handler_per_sec = scale_to_per_sec(s_handler_count_accum, elapsed_ms);

        s_flush_count_accum = 0U;
        s_pixels_accum = 0U;
        s_handler_count_accum = 0U;
        s_perf_window_start_ms = lv_tick_get();
    }
}

void watch_lvgl_port_get_perf_snapshot(watch_lvgl_perf_snapshot_t *snapshot)
{
    if (snapshot == 0) {
        return;
    }

    *snapshot = s_perf_snapshot;
}
