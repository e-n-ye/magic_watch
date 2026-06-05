#include "watch_lvgl_port.h"

#include "board/display/watch_lcd.h"
#include "config/user_config.h"
#include "lvgl.h"

#define WATCH_LVGL_DRAW_BUF_LINES 20U

static lv_color_t s_draw_buf_1[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
static lv_color_t s_draw_buf_2[WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES];
static lv_disp_draw_buf_t s_draw_buf;
static lv_disp_drv_t s_disp_drv;
static uint8_t s_lvgl_port_initialized;

static void watch_lvgl_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;

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

    watch_lcd_draw_rgb565(x, y, width, height, (const uint16_t *)color_p);
    lv_disp_flush_ready(disp_drv);
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

    s_lvgl_port_initialized = 1U;
}

void watch_lvgl_port_task(void)
{
    if (s_lvgl_port_initialized == 0U) {
        return;
    }

    (void)lv_timer_handler();
}
