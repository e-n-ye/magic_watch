#include "app/lvgl_demo/watch_lvgl_debug_screen.h"

#include "config/user_config.h"
#include "lvgl.h"

static lv_obj_t *s_root;

static void style_card_base(lv_obj_t *card)
{
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x13202B), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x2D3E50), 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_pad_top(card, 10, 0);
    lv_obj_set_style_pad_bottom(card, 10, 0);
    lv_obj_set_style_pad_left(card, 10, 0);
    lv_obj_set_style_pad_right(card, 10, 0);
}

static void create_card(
    lv_coord_t x,
    lv_coord_t y,
    lv_color_t accent,
    const char *title,
    const char *value,
    const char *hint)
{
    lv_obj_t *card;
    lv_obj_t *accent_bar;
    lv_obj_t *title_label;
    lv_obj_t *value_label;
    lv_obj_t *hint_label;

    card = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, 104, 82);
    style_card_base(card);

    accent_bar = lv_obj_create(card);
    lv_obj_set_pos(accent_bar, 0, 0);
    lv_obj_set_size(accent_bar, 28, 6);
    lv_obj_set_style_border_width(accent_bar, 0, 0);
    lv_obj_set_style_radius(accent_bar, 3, 0);
    lv_obj_set_style_bg_opa(accent_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(accent_bar, accent, 0);

    title_label = lv_label_create(card);
    lv_label_set_text(title_label, title);
    lv_obj_set_pos(title_label, 0, 14);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x8FA3B8), 0);

    value_label = lv_label_create(card);
    lv_label_set_text(value_label, value);
    lv_obj_set_pos(value_label, 0, 32);
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(value_label, lv_color_hex(0xF8FAFC), 0);

    hint_label = lv_label_create(card);
    lv_label_set_text(hint_label, hint);
    lv_obj_set_pos(hint_label, 0, 60);
    lv_obj_set_style_text_font(hint_label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(hint_label, accent, 0);
}

void watch_lvgl_debug_screen_init(void)
{
    lv_obj_t *title_label;
    lv_obj_t *subtitle_label;

    if (s_root != 0) {
        return;
    }

    s_root = lv_scr_act();
    lv_obj_set_style_bg_opa(s_root, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(s_root, lv_color_hex(0x09131D), 0);

    title_label = lv_label_create(s_root);
    lv_label_set_text(title_label, "F411 Lite UI");
    lv_obj_set_pos(title_label, 12, 10);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xF8FAFC), 0);

    subtitle_label = lv_label_create(s_root);
    lv_label_set_text(subtitle_label, "Static 4-card health shortcuts");
    lv_obj_set_pos(subtitle_label, 12, 36);
    lv_obj_set_style_text_font(subtitle_label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(subtitle_label, lv_color_hex(0x94A3B8), 0);

    create_card(12, 58, lv_color_hex(0xFF6B6B), "Heart Rate", "78", "bpm");
    create_card(124, 58, lv_color_hex(0x4ECDC4), "SpO2", "98", "%");
    create_card(12, 148, lv_color_hex(0xFFD166), "Breathing", "18", "rpm");
    create_card(124, 148, lv_color_hex(0xA78BFA), "Stress", "34", "calm");
}

void watch_lvgl_debug_screen_on_input_intent(watch_input_intent_t intent)
{
    (void)intent;
}

void watch_lvgl_debug_screen_task(void)
{
}
