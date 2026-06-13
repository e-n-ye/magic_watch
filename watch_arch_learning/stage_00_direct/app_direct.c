#include "app_direct.h"

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

#define TAP_MOVE_LIMIT_PX 12

typedef enum {
    FEATURE_HEART = 0,
    FEATURE_STEPS = 1
} feature_t;

typedef struct {
    lv_obj_t *pressed_object;
    lv_point_t pressed_point;
    bool tracking;
    bool moved;
} tap_guard_t;

static lv_obj_t *s_home_panel;
static lv_obj_t *s_detail_panel;
static lv_obj_t *s_detail_title;
static tap_guard_t s_tap_guard;

static void show_home(void);
static void show_detail(feature_t feature);
static void on_heart_clicked(lv_event_t *event);
static void on_steps_clicked(lv_event_t *event);
static void on_back_clicked(lv_event_t *event);
static void add_tap_guard(lv_obj_t *object, lv_event_cb_t callback);
static bool tap_guard_accept(lv_event_t *event);

static int32_t absolute_distance(lv_coord_t first, lv_coord_t second)
{
    int32_t distance = (int32_t)first - (int32_t)second;
    return distance < 0 ? -distance : distance;
}

static bool tap_moved_too_far(const lv_point_t *point)
{
    return absolute_distance(point->x, s_tap_guard.pressed_point.x) > TAP_MOVE_LIMIT_PX ||
           absolute_distance(point->y, s_tap_guard.pressed_point.y) > TAP_MOVE_LIMIT_PX;
}

static void add_tap_guard(lv_obj_t *object, lv_event_cb_t callback)
{
    lv_obj_add_event_cb(object, callback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(object, callback, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(object, callback, LV_EVENT_PRESS_LOST, NULL);
    lv_obj_add_event_cb(object, callback, LV_EVENT_RELEASED, NULL);
}

static bool tap_guard_accept(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *object = lv_event_get_current_target(event);
    lv_indev_t *indev = lv_indev_active();
    lv_point_t point;

    if (object == NULL || indev == NULL) {
        return false;
    }

    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &s_tap_guard.pressed_point);
        s_tap_guard.pressed_object = object;
        s_tap_guard.tracking = true;
        s_tap_guard.moved = false;
        return false;
    }

    if (code == LV_EVENT_PRESS_LOST) {
        if (s_tap_guard.pressed_object == object) {
            s_tap_guard.tracking = false;
            s_tap_guard.moved = true;
            s_tap_guard.pressed_object = NULL;
        }
        return false;
    }

    if (!s_tap_guard.tracking || s_tap_guard.pressed_object != object) {
        return false;
    }

    lv_indev_get_point(indev, &point);
    if (tap_moved_too_far(&point)) {
        s_tap_guard.moved = true;
    }

    if (code == LV_EVENT_RELEASED) {
        bool accepted = !s_tap_guard.moved;
        s_tap_guard.tracking = false;
        s_tap_guard.pressed_object = NULL;
        return accepted;
    }

    return false;
}

static lv_obj_t *create_card(
    lv_obj_t *parent,
    const char *title,
    const char *value,
    lv_coord_t y,
    lv_event_cb_t callback)
{
    lv_obj_t *card = lv_button_create(parent);
    lv_obj_t *title_label;
    lv_obj_t *value_label;

    lv_obj_set_size(card, 200, 72);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x243247), LV_PART_MAIN);
    lv_obj_set_style_radius(card, 16, LV_PART_MAIN);
    add_tap_guard(card, callback);

    title_label = lv_label_create(card);
    lv_label_set_text(title_label, title);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 12, -12);

    value_label = lv_label_create(card);
    lv_label_set_text(value_label, value);
    lv_obj_align(value_label, LV_ALIGN_LEFT_MID, 12, 14);

    return card;
}

void app_direct_init(void)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_t *header;
    lv_obj_t *back_button;
    lv_obj_t *back_label;

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101722), LV_PART_MAIN);

    header = lv_label_create(screen);
    lv_label_set_text(header, "Stage 00: Direct");
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 16);

    s_home_panel = lv_obj_create(screen);
    lv_obj_set_size(s_home_panel, 232, 210);
    lv_obj_align(s_home_panel, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_style_bg_opa(s_home_panel, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_home_panel, 0, LV_PART_MAIN);
    lv_obj_clear_flag(s_home_panel, LV_OBJ_FLAG_SCROLLABLE);

    (void)create_card(
        s_home_panel,
        "Heart",
        "78 bpm",
        8,
        on_heart_clicked);
    (void)create_card(
        s_home_panel,
        "Steps",
        "2,640",
        90,
        on_steps_clicked);

    s_detail_panel = lv_obj_create(screen);
    lv_obj_set_size(s_detail_panel, 220, 190);
    lv_obj_align(s_detail_panel, LV_ALIGN_BOTTOM_MID, 0, -16);
    lv_obj_set_style_bg_color(s_detail_panel, lv_color_hex(0x1B2637), LV_PART_MAIN);
    lv_obj_set_style_radius(s_detail_panel, 18, LV_PART_MAIN);
    lv_obj_clear_flag(s_detail_panel, LV_OBJ_FLAG_SCROLLABLE);

    s_detail_title = lv_label_create(s_detail_panel);
    lv_label_set_text(s_detail_title, "Detail");
    lv_obj_set_style_text_color(s_detail_title, lv_color_hex(0xF4F7FB), LV_PART_MAIN);
    lv_obj_align(s_detail_title, LV_ALIGN_TOP_MID, 0, 28);

    back_button = lv_button_create(s_detail_panel);
    lv_obj_set_size(back_button, 120, 44);
    lv_obj_align(back_button, LV_ALIGN_BOTTOM_MID, 0, -18);
    add_tap_guard(back_button, on_back_clicked);

    back_label = lv_label_create(back_button);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    lv_obj_add_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
}

static void show_home(void)
{
    /* LEARN: make the home panel visible and the detail panel hidden. */
    lv_obj_clear_flag(s_home_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
}

static void show_detail(feature_t feature)
{
    /*
     * LEARN: choose the detail title from feature, then switch panel visibility.
     * Keep this direct; do not add a navigation layer.
     */
    // s_detail_title
    // s_home_panel
    // s_detail_panel
    if (feature == FEATURE_HEART)
    {
        // 标题设为 Heart Detail
        lv_obj_clear_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_home_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_detail_title, "Heart Detail");
    }
    else
    {
        // 标题设为 Steps Detail
        lv_obj_clear_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_home_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_detail_title, "Steps Detail");
    }

    (void)feature;
}

static void on_heart_clicked(lv_event_t *event)
{
    /* LEARN: directly open FEATURE_HEART. */
    if (!tap_guard_accept(event)) {
        return;
    }

    show_detail(FEATURE_HEART);
}

static void on_steps_clicked(lv_event_t *event)
{
    /* LEARN: directly open FEATURE_STEPS. */
    if (!tap_guard_accept(event)) {
        return;
    }

    show_detail(FEATURE_STEPS);
}

static void on_back_clicked(lv_event_t *event)
{
    /* LEARN: directly return to the home panel. */
    if (!tap_guard_accept(event)) {
        return;
    }

    show_home();
}
