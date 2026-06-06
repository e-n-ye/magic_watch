#include "app/lvgl_demo/watch_lvgl_debug_screen.h"

#include <stdint.h>

#include "config/user_config.h"
#include "lvgl.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

#define WATCH_DEBUG_SCREEN_REFRESH_MS 500U
#define WATCH_DEBUG_SCREEN_FULL_PIXELS ((uint32_t)WATCH_LCD_WIDTH * (uint32_t)WATCH_LCD_HEIGHT)

static lv_obj_t *s_status_label;
static uint32_t s_input_count;
static uint32_t s_idle_pulse;
static watch_input_intent_t s_last_intent = WATCH_INPUT_INTENT_NONE;
static watch_lvgl_perf_snapshot_t s_perf_snapshot;
static uint32_t s_last_perf_refresh_ms;
static char s_label_text[224];

static const char *intent_text(watch_input_intent_t intent)
{
    switch (intent) {
    case WATCH_INPUT_INTENT_CROWN_CLOCKWISE:
        return "crown cw";
    case WATCH_INPUT_INTENT_CROWN_COUNTERCLOCKWISE:
        return "crown ccw";
    case WATCH_INPUT_INTENT_CONFIRM:
        return "confirm";
    case WATCH_INPUT_INTENT_LONG_PRESS:
        return "long press";
    default:
        return "none";
    }
}

static char *append_text(char *dst, const char *text)
{
    while (*text != '\0') {
        *dst = *text;
        dst++;
        text++;
    }
    return dst;
}

static char *append_u32(char *dst, uint32_t value)
{
    char digits[10];
    uint8_t count = 0U;

    if (value == 0U) {
        *dst = '0';
        return dst + 1;
    }

    while ((value != 0U) && (count < (uint8_t)sizeof(digits))) {
        digits[count] = (char)('0' + (value % 10U));
        value /= 10U;
        count++;
    }

    while (count != 0U) {
        count--;
        *dst = digits[count];
        dst++;
    }

    return dst;
}

static char *append_u32_x10(char *dst, uint32_t value_x10)
{
    dst = append_u32(dst, value_x10 / 10U);
    *dst = '.';
    dst++;
    *dst = (char)('0' + (value_x10 % 10U));
    return dst + 1;
}

static void render_label(void)
{
    char *cursor = s_label_text;
    uint32_t full_per_sec_x10 = 0U;

    if (s_status_label == 0) {
        return;
    }

    if (WATCH_DEBUG_SCREEN_FULL_PIXELS != 0U) {
        full_per_sec_x10 = (uint32_t)(((uint64_t)s_perf_snapshot.pixels_per_sec * 10ULL) /
                                      (uint64_t)WATCH_DEBUG_SCREEN_FULL_PIXELS);
    }

    cursor = append_text(cursor, "F411 LVGL\n");
    cursor = append_text(cursor, "debug screen\n");
    cursor = append_text(cursor, "events: ");
    cursor = append_u32(cursor, s_input_count);
    cursor = append_text(cursor, "\nlast: ");
    cursor = append_text(cursor, intent_text(s_last_intent));
    cursor = append_text(cursor, "\npulse: ");
    cursor = append_u32(cursor, s_idle_pulse);
    cursor = append_text(cursor, "\ncalls/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.flush_per_sec);
    cursor = append_text(cursor, "\nfull/s: ");
    cursor = append_u32_x10(cursor, full_per_sec_x10);
    cursor = append_text(cursor, "\npixels/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.pixels_per_sec);
    cursor = append_text(cursor, "\nlast ms: ");
    cursor = append_u32(cursor, s_perf_snapshot.last_flush_ms);
    cursor = append_text(cursor, "\nlvgl/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.handler_per_sec);
    *cursor = '\0';

    lv_label_set_text(s_status_label, s_label_text);
    lv_obj_set_pos(s_status_label, 8, 112);
}

static void create_color_block(lv_coord_t x, lv_coord_t y, lv_color_t color)
{
    lv_obj_t *block = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(block, x, y);
    lv_obj_set_size(block, 40, 26);
    lv_obj_set_style_bg_color(block, color, 0);
    lv_obj_set_style_bg_opa(block, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(block, lv_color_hex(0x404040), 0);
    lv_obj_set_style_border_width(block, 1, 0);
    lv_obj_set_style_radius(block, 0, 0);
}

static void create_font_check_label(
    const char *text,
    lv_coord_t y,
    const lv_font_t *font,
    lv_color_t color)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, text);
    lv_obj_set_width(label, WATCH_LCD_WIDTH);
    lv_obj_set_pos(label, 0, y);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, font, 0);
}

static void create_color_and_font_checks(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x081018), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);

    create_color_block(12, 8, lv_color_hex(0xFF0000));
    create_color_block(56, 8, lv_color_hex(0x00FF00));
    create_color_block(100, 8, lv_color_hex(0x0000FF));
    create_color_block(144, 8, lv_color_hex(0xFFFFFF));
    create_color_block(188, 8, lv_color_hex(0x000000));

    create_font_check_label("font 10: crisp small text", 42, &lv_font_montserrat_10, lv_color_hex(0xC8D6E5));
    create_font_check_label("font 14: normal text", 60, &lv_font_montserrat_14, lv_color_hex(0xFFFFFF));
    create_font_check_label("font 20: large text", 80, &lv_font_montserrat_20, lv_color_hex(0xFFD166));
}

void watch_lvgl_debug_screen_init(void)
{
    if (s_status_label != 0) {
        return;
    }

    create_color_and_font_checks();

    s_status_label = lv_label_create(lv_scr_act());
    lv_obj_set_width(s_status_label, WATCH_LCD_WIDTH - 16);
    lv_obj_set_style_text_align(s_status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(s_status_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(s_status_label, &lv_font_montserrat_10, 0);
    watch_lvgl_port_get_perf_snapshot(&s_perf_snapshot);
    s_last_perf_refresh_ms = lv_tick_get();
    render_label();
}

void watch_lvgl_debug_screen_on_input_intent(watch_input_intent_t intent)
{
    if (intent == WATCH_INPUT_INTENT_NONE) {
        return;
    }

    s_input_count++;
    s_last_intent = intent;
    render_label();
}

void watch_lvgl_debug_screen_task(void)
{
    if (s_status_label == 0) {
        return;
    }

    if (lv_tick_elaps(s_last_perf_refresh_ms) < WATCH_DEBUG_SCREEN_REFRESH_MS) {
        return;
    }

    watch_lvgl_port_get_perf_snapshot(&s_perf_snapshot);
    s_idle_pulse++;
    s_last_perf_refresh_ms = lv_tick_get();
    render_label();
}
