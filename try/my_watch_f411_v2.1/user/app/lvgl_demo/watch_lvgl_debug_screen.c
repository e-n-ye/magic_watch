#include "app/lvgl_demo/watch_lvgl_debug_screen.h"

#include <stdint.h>

#include "lvgl.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

static lv_obj_t *s_status_label;
static uint32_t s_input_count;
static watch_input_intent_t s_last_intent = WATCH_INPUT_INTENT_NONE;
static watch_lvgl_perf_snapshot_t s_perf_snapshot;
static uint32_t s_last_perf_refresh_ms;
static char s_label_text[192];

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

static void render_label(void)
{
    char *cursor = s_label_text;

    if (s_status_label == 0) {
        return;
    }

    cursor = append_text(cursor, "F411 LVGL\n");
    cursor = append_text(cursor, "debug screen\n");
    cursor = append_text(cursor, "events: ");
    cursor = append_u32(cursor, s_input_count);
    cursor = append_text(cursor, "\nlast: ");
    cursor = append_text(cursor, intent_text(s_last_intent));
    cursor = append_text(cursor, "\nflush/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.flush_per_sec);
    cursor = append_text(cursor, "\npx/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.pixels_per_sec);
    cursor = append_text(cursor, "\nlast ms: ");
    cursor = append_u32(cursor, s_perf_snapshot.last_flush_ms);
    cursor = append_text(cursor, "\nhandler/s: ");
    cursor = append_u32(cursor, s_perf_snapshot.handler_per_sec);
    *cursor = '\0';

    lv_label_set_text(s_status_label, s_label_text);
    lv_obj_center(s_status_label);
}

void watch_lvgl_debug_screen_init(void)
{
    if (s_status_label != 0) {
        return;
    }

    s_status_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_align(s_status_label, LV_TEXT_ALIGN_CENTER, 0);
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

    if (lv_tick_elaps(s_last_perf_refresh_ms) < 500U) {
        return;
    }

    watch_lvgl_port_get_perf_snapshot(&s_perf_snapshot);
    s_last_perf_refresh_ms = lv_tick_get();
    render_label();
}
