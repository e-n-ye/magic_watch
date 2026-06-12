#include "app/lvgl_demo/watch_lite_view.h"

#include <stdint.h>

#include "lvgl.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

typedef struct {
    WatchCoreHealthFeature feature;
    const char *title;
    const char *hint;
    uint32_t accent_hex;
} watch_health_card_meta_t;

static const watch_health_card_meta_t s_card_meta[WATCH_CORE_HEALTH_CARD_COUNT] = {
    { WATCH_CORE_HEALTH_FEATURE_HEART_RATE, "Heart Rate", "bpm", 0xFF6B6B },
    { WATCH_CORE_HEALTH_FEATURE_SPO2, "SpO2", "%", 0x4ECDC4 },
    { WATCH_CORE_HEALTH_FEATURE_BREATHE, "Breathing", "rpm", 0xFFD166 },
    { WATCH_CORE_HEALTH_FEATURE_STRESS, "Stress", "calm", 0xA78BFA },
};

static const watch_lite_view_callbacks_t *s_callbacks;
static WatchCorePageIntent s_current_page;
static lv_obj_t *s_root;
static lv_group_t *s_focus_group;
static lv_obj_t *s_header_title;
static lv_obj_t *s_header_subtitle;
static lv_obj_t *s_shortcuts_panel;
static lv_obj_t *s_detail_panel;
static lv_obj_t *s_card_objs[WATCH_CORE_HEALTH_CARD_COUNT];
static lv_obj_t *s_card_value_labels[WATCH_CORE_HEALTH_CARD_COUNT];
static lv_obj_t *s_detail_title_label;
static lv_obj_t *s_detail_metric_label;
static lv_obj_t *s_detail_hint_label;
static lv_obj_t *s_detail_back_button;
static lv_obj_t *s_swipe_back_hint;
static lv_obj_t *s_swipe_back_hint_arrow;

static uint32_t feature_index(WatchCoreHealthFeature feature)
{
    uint32_t i;

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        if (s_card_meta[i].feature == feature) {
            return i;
        }
    }

    return 0U;
}

static void style_card_base(lv_obj_t *card, lv_color_t accent)
{
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x13202B), LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, lv_color_hex(0x2D3E50), LV_PART_MAIN);
    lv_obj_set_style_border_color(card, accent, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(card, 2, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_radius(card, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_top(card, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(card, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(card, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(card, 10, LV_PART_MAIN);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
}

static void rebuild_focus_group(void)
{
    uint32_t i;

    if (s_focus_group == 0) {
        return;
    }

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_group_remove_obj(s_card_objs[i]);
    }
    lv_group_remove_obj(s_detail_back_button);

    if (s_current_page.type == WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL) {
        lv_group_add_obj(s_focus_group, s_detail_back_button);
        lv_group_focus_obj(s_detail_back_button);
        return;
    }

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_group_add_obj(s_focus_group, s_card_objs[i]);
    }
    lv_group_focus_obj(s_card_objs[0]);
}

static void set_detail_translate_x(lv_coord_t offset)
{
    lv_obj_set_style_translate_x(s_header_title, offset, LV_PART_MAIN);
    lv_obj_set_style_translate_x(s_header_subtitle, offset, LV_PART_MAIN);
    lv_obj_set_style_translate_x(s_detail_panel, offset, LV_PART_MAIN);
}

static void on_card_clicked(lv_event_t *event)
{
    WatchCoreHealthFeature feature;

    if (!watch_lvgl_port_accept_activation_event(event) || s_callbacks == 0 ||
        s_callbacks->activate_health_card == 0) {
        return;
    }

    feature = (WatchCoreHealthFeature)(uintptr_t)lv_event_get_user_data(event);
    s_callbacks->activate_health_card(feature);
}

static void on_back_clicked(lv_event_t *event)
{
    if (!watch_lvgl_port_accept_activation_event(event) || s_callbacks == 0 ||
        s_callbacks->back == 0) {
        return;
    }

    s_callbacks->back();
}

static void create_shortcuts_view(void)
{
    static const lv_coord_t s_card_pos[WATCH_CORE_HEALTH_CARD_COUNT][2] = {
        { 0, 0 },
        { 112, 0 },
        { 0, 90 },
        { 112, 90 },
    };

    uint32_t i;

    s_shortcuts_panel = lv_obj_create(s_root);
    lv_obj_set_pos(s_shortcuts_panel, 12, 58);
    lv_obj_set_size(s_shortcuts_panel, 216, 174);
    lv_obj_set_style_bg_opa(s_shortcuts_panel, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_shortcuts_panel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_shortcuts_panel, 0, LV_PART_MAIN);
    lv_obj_clear_flag(s_shortcuts_panel, LV_OBJ_FLAG_SCROLLABLE);

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_obj_t *card;
        lv_obj_t *accent_bar;
        lv_obj_t *title_label;
        lv_obj_t *hint_label;

        card = lv_obj_create(s_shortcuts_panel);
        s_card_objs[i] = card;
        lv_obj_set_pos(card, s_card_pos[i][0], s_card_pos[i][1]);
        lv_obj_set_size(card, 104, 82);
        style_card_base(card, lv_color_hex(s_card_meta[i].accent_hex));
        lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_add_event_cb(card, on_card_clicked, LV_EVENT_CLICKED, (void *)(uintptr_t)s_card_meta[i].feature);

        accent_bar = lv_obj_create(card);
        lv_obj_set_pos(accent_bar, 0, 0);
        lv_obj_set_size(accent_bar, 28, 6);
        lv_obj_set_style_border_width(accent_bar, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(accent_bar, 3, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(accent_bar, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(accent_bar, lv_color_hex(s_card_meta[i].accent_hex), LV_PART_MAIN);

        title_label = lv_label_create(card);
        lv_label_set_text(title_label, s_card_meta[i].title);
        lv_obj_set_pos(title_label, 0, 14);
        lv_obj_set_style_text_font(title_label, &lv_font_montserrat_10, LV_PART_MAIN);
        lv_obj_set_style_text_color(title_label, lv_color_hex(0x8FA3B8), LV_PART_MAIN);

        s_card_value_labels[i] = lv_label_create(card);
        lv_label_set_text(s_card_value_labels[i], "--");
        lv_obj_set_pos(s_card_value_labels[i], 0, 32);
        lv_obj_set_style_text_font(s_card_value_labels[i], &lv_font_montserrat_20, LV_PART_MAIN);
        lv_obj_set_style_text_color(s_card_value_labels[i], lv_color_hex(0xF8FAFC), LV_PART_MAIN);

        hint_label = lv_label_create(card);
        lv_label_set_text(hint_label, s_card_meta[i].hint);
        lv_obj_set_pos(hint_label, 0, 60);
        lv_obj_set_style_text_font(hint_label, &lv_font_montserrat_10, LV_PART_MAIN);
        lv_obj_set_style_text_color(hint_label, lv_color_hex(s_card_meta[i].accent_hex), LV_PART_MAIN);
    }
}

static void create_detail_view(void)
{
    lv_obj_t *detail_box;
    lv_obj_t *accent_bar;
    lv_obj_t *back_label;

    s_detail_panel = lv_obj_create(s_root);
    lv_obj_set_pos(s_detail_panel, 12, 58);
    lv_obj_set_size(s_detail_panel, 216, 174);
    lv_obj_set_style_bg_opa(s_detail_panel, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_detail_panel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_detail_panel, 0, LV_PART_MAIN);
    lv_obj_clear_flag(s_detail_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);

    detail_box = lv_obj_create(s_detail_panel);
    lv_obj_set_pos(detail_box, 0, 0);
    lv_obj_set_size(detail_box, 216, 124);
    style_card_base(detail_box, lv_color_hex(0x4ECDC4));

    accent_bar = lv_obj_create(detail_box);
    lv_obj_set_pos(accent_bar, 0, 0);
    lv_obj_set_size(accent_bar, 38, 6);
    lv_obj_set_style_border_width(accent_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(accent_bar, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(accent_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(accent_bar, lv_color_hex(0x4ECDC4), LV_PART_MAIN);

    s_detail_title_label = lv_label_create(detail_box);
    lv_obj_set_pos(s_detail_title_label, 0, 18);
    lv_obj_set_style_text_font(s_detail_title_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_detail_title_label, lv_color_hex(0xE2E8F0), LV_PART_MAIN);

    s_detail_metric_label = lv_label_create(detail_box);
    lv_obj_set_pos(s_detail_metric_label, 0, 50);
    lv_obj_set_style_text_font(s_detail_metric_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_detail_metric_label, lv_color_hex(0xF8FAFC), LV_PART_MAIN);

    s_detail_hint_label = lv_label_create(detail_box);
    lv_obj_set_pos(s_detail_hint_label, 0, 84);
    lv_obj_set_style_text_font(s_detail_hint_label, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_detail_hint_label, lv_color_hex(0x94A3B8), LV_PART_MAIN);

    s_detail_back_button = lv_obj_create(s_detail_panel);
    lv_obj_set_pos(s_detail_back_button, 0, 136);
    lv_obj_set_size(s_detail_back_button, 216, 38);
    style_card_base(s_detail_back_button, lv_color_hex(0x38BDF8));
    lv_obj_add_flag(s_detail_back_button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(s_detail_back_button, on_back_clicked, LV_EVENT_CLICKED, 0);

    back_label = lv_label_create(s_detail_back_button);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
    lv_obj_set_style_text_font(back_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0xE2E8F0), LV_PART_MAIN);

    s_swipe_back_hint = lv_obj_create(s_root);
    lv_obj_set_pos(s_swipe_back_hint, 0, 120);
    lv_obj_set_size(s_swipe_back_hint, 6, 34);
    lv_obj_set_style_radius(s_swipe_back_hint, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_swipe_back_hint, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_swipe_back_hint, lv_color_hex(0x94A3B8), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_swipe_back_hint, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(s_swipe_back_hint, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(s_swipe_back_hint, LV_OBJ_FLAG_HIDDEN);

    s_swipe_back_hint_arrow = lv_label_create(s_swipe_back_hint);
    lv_label_set_text(s_swipe_back_hint_arrow, LV_SYMBOL_LEFT);
    lv_obj_center(s_swipe_back_hint_arrow);
    lv_obj_set_style_text_font(s_swipe_back_hint_arrow, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_swipe_back_hint_arrow, lv_color_hex(0xF8FAFC), LV_PART_MAIN);
    lv_obj_set_style_opa(s_swipe_back_hint_arrow, LV_OPA_TRANSP, LV_PART_MAIN);
}

void watch_lite_view_init(const watch_lite_view_callbacks_t *callbacks)
{
    if (s_root != 0) {
        return;
    }

    s_callbacks = callbacks;
    s_current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
    s_current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;

    s_root = lv_scr_act();
    lv_obj_set_style_bg_opa(s_root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_root, lv_color_hex(0x09131D), LV_PART_MAIN);

    s_header_title = lv_label_create(s_root);
    lv_obj_set_pos(s_header_title, 12, 10);
    lv_obj_set_style_text_font(s_header_title, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_header_title, lv_color_hex(0xF8FAFC), LV_PART_MAIN);

    s_header_subtitle = lv_label_create(s_root);
    lv_obj_set_pos(s_header_subtitle, 12, 36);
    lv_obj_set_style_text_font(s_header_subtitle, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(s_header_subtitle, lv_color_hex(0x94A3B8), LV_PART_MAIN);

    create_shortcuts_view();
    create_detail_view();

    s_focus_group = lv_group_create();
    watch_lvgl_port_set_group(s_focus_group);
}

void watch_lite_view_apply_snapshot(const WatchCoreUiModelSnapshot *snapshot)
{
    uint32_t i;
    uint32_t detail_index;

    if (snapshot == 0) {
        return;
    }

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_label_set_text(s_card_value_labels[i], snapshot->health_metric_text[i]);
    }

    detail_index = feature_index(s_current_page.feature);
    lv_label_set_text(s_detail_metric_label, snapshot->health_metric_text[detail_index]);
}

void watch_lite_view_apply_page_intent(const WatchCorePageIntent *intent)
{
    uint32_t detail_index;

    if (intent == 0) {
        return;
    }

    s_current_page = *intent;

    if (s_current_page.type == WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL) {
        detail_index = feature_index(s_current_page.feature);

        lv_obj_add_flag(s_shortcuts_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_header_title, s_card_meta[detail_index].title);
        lv_label_set_text(s_header_subtitle, "Coordinator-owned detail route");
        lv_label_set_text(s_detail_title_label, s_card_meta[detail_index].title);
        lv_label_set_text(s_detail_hint_label, "Placeholder detail page\nBack returns to shortcuts");
        lv_obj_set_style_text_color(s_detail_hint_label, lv_color_hex(s_card_meta[detail_index].accent_hex), LV_PART_MAIN);
    } else {
        lv_obj_clear_flag(s_shortcuts_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_detail_panel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(s_header_title, "F411 Lite UI");
        lv_label_set_text(s_header_subtitle, "Coordinator-driven 4-card shortcuts");
    }

    rebuild_focus_group();
}

void watch_lite_view_update_swipe_hint(uint16_t progress)
{
    lv_coord_t hint_width;
    lv_coord_t translate_x;
    lv_opa_t bg_opa;
    lv_opa_t arrow_opa;

    if ((s_swipe_back_hint == 0) || (s_swipe_back_hint_arrow == 0) ||
        (s_current_page.type != WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL) ||
        (progress == 0U)) {
        if (s_swipe_back_hint != 0) {
            lv_obj_add_flag(s_swipe_back_hint, LV_OBJ_FLAG_HIDDEN);
        }
        set_detail_translate_x(0);
        return;
    }

    hint_width = (lv_coord_t)(6U + (progress / 3U));
    if (hint_width > 34) {
        hint_width = 34;
    }
    translate_x = (lv_coord_t)(progress / 4U);
    if (translate_x > 18) {
        translate_x = 18;
    }
    bg_opa = (lv_opa_t)(((progress > 84U ? 84U : progress) * LV_OPA_70) / 84U);
    arrow_opa = (lv_opa_t)(((progress > 84U ? 84U : progress) * LV_OPA_80) / 84U);

    lv_obj_clear_flag(s_swipe_back_hint, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(s_swipe_back_hint, hint_width);
    lv_obj_set_style_bg_opa(s_swipe_back_hint, bg_opa, LV_PART_MAIN);
    lv_obj_set_style_opa(s_swipe_back_hint_arrow, arrow_opa, LV_PART_MAIN);
    set_detail_translate_x(translate_x);
}
