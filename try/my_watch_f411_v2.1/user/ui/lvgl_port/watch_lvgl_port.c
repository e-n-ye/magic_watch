#include "watch_lvgl_port.h"

#include <stdint.h>

#include "board/display/watch_lcd.h"
#include "config/user_config.h"
#include "lvgl.h"

#define WATCH_LVGL_DRAW_BUF_LINES 20U
#define WATCH_LVGL_DMA_BUF_BYTES (WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES * 2U)

static lv_color_t s_draw_buf_1[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
static lv_color_t s_draw_buf_2[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
/* The pending DMA flush owns this byte stream until flush_ready.
 * LVGL must not submit a second flush for the same draw buffer before that. */
static uint8_t s_flush_dma_bytes[WATCH_LVGL_DMA_BUF_BYTES];
static lv_disp_draw_buf_t s_draw_buf;
static lv_disp_drv_t s_disp_drv;
static lv_indev_drv_t s_encoder_drv;
static int16_t s_encoder_diff;
static uint8_t s_encoder_press_pulse;
static uint8_t s_lvgl_port_initialized;
static uint8_t s_flush_waiting_dma;
static uint32_t s_perf_window_start_ms;
static uint32_t s_refresh_count_accum;
static uint32_t s_flush_count_accum;
static uint32_t s_pixels_accum;
static uint32_t s_handler_count_accum;
static uint32_t s_max_flush_pixels_accum;
static uint32_t s_max_convert_ms_accum;
static uint32_t s_blocking_count_accum;
static uint32_t s_dma_count_accum;
static uint32_t s_failed_count_accum;
static uint32_t s_dma_fallback_count_accum;
static uint32_t s_max_dma_wait_ms_accum;
static uint32_t s_pending_flush_start_ms;
static uint32_t s_pending_flush_pixels;
static uint16_t s_pending_flush_x;
static uint16_t s_pending_flush_y;
static uint16_t s_pending_flush_width;
static uint16_t s_pending_flush_height;
static uint16_t s_pending_flush_byte_count;
static lv_disp_drv_t *s_pending_flush_drv;
static watch_lvgl_perf_snapshot_t s_perf_snapshot;

#define WATCH_LVGL_FULL_PIXELS ((uint32_t)WATCH_LCD_WIDTH * (uint32_t)WATCH_LCD_HEIGHT)

static uint32_t scale_to_per_sec(uint32_t value, uint32_t elapsed_ms)
{
    uint64_t scaled;

    if (elapsed_ms == 0U) {
        elapsed_ms = 1U;
    }

    scaled = ((uint64_t)value * 1000ULL) / (uint64_t)elapsed_ms;
    return (scaled > UINT32_MAX) ? UINT32_MAX : (uint32_t)scaled;
}

static uint32_t scale_to_permille(uint32_t value, uint32_t total)
{
    uint64_t scaled;

    if (total == 0U) {
        return 0U;
    }

    scaled = ((uint64_t)value * 1000ULL) / (uint64_t)total;
    return (scaled > UINT32_MAX) ? UINT32_MAX : (uint32_t)scaled;
}

static void watch_lvgl_monitor(lv_disp_drv_t *disp_drv, uint32_t time, uint32_t px)
{
    (void)disp_drv;
    (void)px;

    s_refresh_count_accum++;
    s_perf_snapshot.last_refresh_ms = time;
}

static void watch_lvgl_finish_flush(lv_disp_drv_t *disp_drv, uint32_t start_ms, uint32_t pixels)
{
    s_perf_snapshot.last_flush_ms = lv_tick_elaps(start_ms);
    s_perf_snapshot.last_flush_pixels = pixels;
    s_perf_snapshot.last_flush_area_permille = scale_to_permille(pixels, WATCH_LVGL_FULL_PIXELS);
    s_perf_snapshot.dma_flush_pending = 0U;
    s_flush_count_accum++;
    s_pixels_accum += pixels;
    if (pixels > s_max_flush_pixels_accum) {
        s_max_flush_pixels_accum = pixels;
    }
    lv_disp_flush_ready(disp_drv);
}

static void watch_lvgl_try_complete_pending_flush(void)
{
    uint32_t dma_wait_ms;

    if ((s_flush_waiting_dma == 0U) || watch_lcd_dma_is_busy()) {
        return;
    }

    s_flush_waiting_dma = 0U;
    s_perf_snapshot.dma_flush_pending = 0U;
    dma_wait_ms = lv_tick_elaps(s_pending_flush_start_ms);
    s_perf_snapshot.last_dma_wait_ms = dma_wait_ms;
    if (dma_wait_ms > s_max_dma_wait_ms_accum) {
        s_max_dma_wait_ms_accum = dma_wait_ms;
    }
    /* If SPI DMA aborted after the LCD window was armed, replay the same byte
     * stream synchronously from defaultTask before releasing LVGL's draw buffer. */
    if (watch_lcd_dma_consume_error()) {
        s_perf_snapshot.last_transfer_result = WATCH_LCD_TRANSFER_FAILED;
        s_dma_fallback_count_accum++;
        watch_lcd_draw_rgb565_bytes_blocking(
            s_pending_flush_x,
            s_pending_flush_y,
            s_pending_flush_width,
            s_pending_flush_height,
            s_flush_dma_bytes,
            s_pending_flush_byte_count);
    }

    watch_lvgl_finish_flush(s_pending_flush_drv, s_pending_flush_start_ms, s_pending_flush_pixels);
    s_pending_flush_drv = 0;
    s_pending_flush_start_ms = 0U;
    s_pending_flush_pixels = 0U;
    s_pending_flush_x = 0U;
    s_pending_flush_y = 0U;
    s_pending_flush_width = 0U;
    s_pending_flush_height = 0U;
    s_pending_flush_byte_count = 0U;
}

static void watch_lvgl_wait_cb(lv_disp_drv_t *disp_drv)
{
    (void)disp_drv;

    /* LVGL calls wait_cb from inside its internal flushing wait loop.
     * Completing the pending DMA flush here avoids deadlocking on
     * lv_timer_handler() -> while(draw_buf->flushing). */
    watch_lvgl_try_complete_pending_flush();
}

static uint16_t watch_lvgl_clip_start(lv_coord_t value)
{
    return (value < 0) ? 0U : (uint16_t)value;
}

static uint16_t watch_lvgl_clip_end(lv_coord_t value, uint16_t max_value)
{
    if (value < 0) {
        return 0U;
    }

    if (value > (lv_coord_t)max_value) {
        return max_value;
    }

    return (uint16_t)value;
}

static uint16_t watch_lvgl_prepare_flush_bytes(
    const lv_area_t *area,
    const lv_color_t *color_p,
    uint16_t *out_x,
    uint16_t *out_y,
    uint16_t *out_width,
    uint16_t *out_height)
{
    uint16_t clipped_x1;
    uint16_t clipped_y1;
    uint16_t clipped_x2;
    uint16_t clipped_y2;
    uint16_t width;
    uint16_t height;
    uint16_t src_stride;
    uint16_t src_offset_x;
    uint16_t src_offset_y;
    uint16_t row;
    uint16_t column;
    uint16_t byte_index;
    uint16_t pixel;
    const lv_color_t *src_row;

    clipped_x1 = watch_lvgl_clip_start(area->x1);
    clipped_y1 = watch_lvgl_clip_start(area->y1);
    clipped_x2 = watch_lvgl_clip_end(area->x2, (uint16_t)(WATCH_LCD_WIDTH - 1U));
    clipped_y2 = watch_lvgl_clip_end(area->y2, (uint16_t)(WATCH_LCD_HEIGHT - 1U));

    if ((clipped_x2 < clipped_x1) || (clipped_y2 < clipped_y1)) {
        return 0U;
    }

    width = (uint16_t)(clipped_x2 - clipped_x1 + 1U);
    height = (uint16_t)(clipped_y2 - clipped_y1 + 1U);
    src_stride = (uint16_t)(area->x2 - area->x1 + 1);
    src_offset_x = (uint16_t)(clipped_x1 - area->x1);
    src_offset_y = (uint16_t)(clipped_y1 - area->y1);

    if (((uint32_t)width * (uint32_t)height * 2U) > (uint32_t)WATCH_LVGL_DMA_BUF_BYTES) {
        return 0U;
    }

    byte_index = 0U;
    for (row = 0U; row < height; ++row) {
        src_row = &color_p[(uint32_t)(src_offset_y + row) * src_stride + src_offset_x];
        for (column = 0U; column < width; ++column) {
            pixel = src_row[column].full;
            s_flush_dma_bytes[byte_index++] = (uint8_t)(pixel >> 8);
            s_flush_dma_bytes[byte_index++] = (uint8_t)(pixel & 0xFFU);
        }
    }

    *out_x = clipped_x1;
    *out_y = clipped_y1;
    *out_width = width;
    *out_height = height;
    return byte_index;
}

static void watch_lvgl_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    watch_lcd_transfer_result_t result;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t byte_count;
    uint32_t pixels;
    uint32_t convert_start_ms;
    uint32_t convert_ms;
    uint32_t start_ms;

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

    convert_start_ms = lv_tick_get();
    byte_count = watch_lvgl_prepare_flush_bytes(area, color_p, &x, &y, &width, &height);
    convert_ms = lv_tick_elaps(convert_start_ms);
    s_perf_snapshot.last_convert_ms = convert_ms;
    if (convert_ms > s_max_convert_ms_accum) {
        s_max_convert_ms_accum = convert_ms;
    }
    if (byte_count == 0U) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    pixels = (uint32_t)width * (uint32_t)height;
    s_perf_snapshot.last_flush_pixels = pixels;
    s_perf_snapshot.last_flush_bytes = byte_count;
    start_ms = lv_tick_get();
    result = watch_lcd_draw_rgb565_bytes(
        x,
        y,
        width,
        height,
        s_flush_dma_bytes,
        byte_count,
        0,
        0);

    s_perf_snapshot.last_transfer_result = (uint32_t)result;
    if (result == WATCH_LCD_TRANSFER_DMA_STARTED) {
        s_flush_waiting_dma = 1U;
        s_perf_snapshot.dma_flush_pending = 1U;
        s_dma_count_accum++;
        s_pending_flush_drv = disp_drv;
        s_pending_flush_start_ms = start_ms;
        s_pending_flush_pixels = pixels;
        s_pending_flush_x = x;
        s_pending_flush_y = y;
        s_pending_flush_width = width;
        s_pending_flush_height = height;
        s_pending_flush_byte_count = byte_count;
        return;
    }

    if (result == WATCH_LCD_TRANSFER_FAILED) {
        s_perf_snapshot.dma_flush_pending = 0U;
        s_failed_count_accum++;
        lv_disp_flush_ready(disp_drv);
        return;
    }

    s_blocking_count_accum++;
    watch_lvgl_finish_flush(disp_drv, start_ms, pixels);
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
    s_disp_drv.wait_cb = watch_lvgl_wait_cb;
    s_disp_drv.monitor_cb = watch_lvgl_monitor;
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

    watch_lvgl_try_complete_pending_flush();
    (void)lv_timer_handler();
    s_handler_count_accum++;
    watch_lvgl_try_complete_pending_flush();

    elapsed_ms = lv_tick_elaps(s_perf_window_start_ms);
    if (elapsed_ms >= 1000U) {
        s_perf_snapshot.refresh_per_sec = scale_to_per_sec(s_refresh_count_accum, elapsed_ms);
        s_perf_snapshot.flush_per_sec = scale_to_per_sec(s_flush_count_accum, elapsed_ms);
        s_perf_snapshot.pixels_per_sec = scale_to_per_sec(s_pixels_accum, elapsed_ms);
        s_perf_snapshot.handler_per_sec = scale_to_per_sec(s_handler_count_accum, elapsed_ms);
        s_perf_snapshot.max_flush_pixels_per_sec_window = s_max_flush_pixels_accum;
        s_perf_snapshot.max_flush_area_permille_per_sec_window =
            scale_to_permille(s_max_flush_pixels_accum, WATCH_LVGL_FULL_PIXELS);
        s_perf_snapshot.max_convert_ms_per_sec_window = s_max_convert_ms_accum;
        s_perf_snapshot.blocking_count_per_sec = scale_to_per_sec(s_blocking_count_accum, elapsed_ms);
        s_perf_snapshot.dma_count_per_sec = scale_to_per_sec(s_dma_count_accum, elapsed_ms);
        s_perf_snapshot.failed_count_per_sec = scale_to_per_sec(s_failed_count_accum, elapsed_ms);
        s_perf_snapshot.dma_fallback_count_per_sec =
            scale_to_per_sec(s_dma_fallback_count_accum, elapsed_ms);
        s_perf_snapshot.max_dma_wait_ms_per_sec_window = s_max_dma_wait_ms_accum;

        s_refresh_count_accum = 0U;
        s_flush_count_accum = 0U;
        s_pixels_accum = 0U;
        s_handler_count_accum = 0U;
        s_max_flush_pixels_accum = 0U;
        s_max_convert_ms_accum = 0U;
        s_blocking_count_accum = 0U;
        s_dma_count_accum = 0U;
        s_failed_count_accum = 0U;
        s_dma_fallback_count_accum = 0U;
        s_max_dma_wait_ms_accum = 0U;
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
