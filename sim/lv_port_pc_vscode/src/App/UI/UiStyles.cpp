#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

struct StyleRegistry {
  bool initialized {false};
  lv_style_t screen;
  lv_style_t panel;
  lv_style_t panel_subtle;
  lv_style_t chip;
  lv_style_t button_primary;
  lv_style_t button_secondary;
  lv_style_t button_pressed;
  lv_style_t text_eyebrow;
  lv_style_t text_hero;
  lv_style_t text_hero_soft;
  lv_style_t text_title;
  lv_style_t text_body;
  lv_style_t text_muted;
  lv_style_t text_accent;
  lv_style_t text_button;
  lv_style_t text_tiny;
};

StyleRegistry& styles() {
  static StyleRegistry registry;
  if (registry.initialized) {
    return registry;
  }

  registry.initialized = true;

  lv_style_init(&registry.screen);
  lv_style_set_bg_color(&registry.screen, lv_color_hex(0x05080F));
  lv_style_set_bg_opa(&registry.screen, LV_OPA_COVER);
  lv_style_set_border_width(&registry.screen, 0);
  lv_style_set_pad_all(&registry.screen, 0);
  lv_style_set_radius(&registry.screen, 0);

  lv_style_init(&registry.panel);
  lv_style_set_bg_color(&registry.panel, lv_color_hex(0x0D1520));
  lv_style_set_bg_opa(&registry.panel, LV_OPA_COVER);
  lv_style_set_border_color(&registry.panel, lv_color_hex(0x1B2939));
  lv_style_set_border_width(&registry.panel, 1);
  lv_style_set_radius(&registry.panel, 24);
  lv_style_set_pad_all(&registry.panel, 0);
  lv_style_set_shadow_opa(&registry.panel, LV_OPA_20);
  lv_style_set_shadow_width(&registry.panel, 16);
  lv_style_set_shadow_color(&registry.panel, lv_color_hex(0x000000));

  lv_style_init(&registry.panel_subtle);
  lv_style_set_bg_color(&registry.panel_subtle, lv_color_hex(0x0A121B));
  lv_style_set_bg_opa(&registry.panel_subtle, LV_OPA_COVER);
  lv_style_set_border_color(&registry.panel_subtle, lv_color_hex(0x182534));
  lv_style_set_border_width(&registry.panel_subtle, 1);
  lv_style_set_radius(&registry.panel_subtle, 18);
  lv_style_set_pad_all(&registry.panel_subtle, 0);

  lv_style_init(&registry.chip);
  lv_style_set_bg_color(&registry.chip, lv_color_hex(0x101B28));
  lv_style_set_bg_opa(&registry.chip, LV_OPA_COVER);
  lv_style_set_border_color(&registry.chip, lv_color_hex(0x22354B));
  lv_style_set_border_width(&registry.chip, 1);
  lv_style_set_radius(&registry.chip, 16);
  lv_style_set_pad_hor(&registry.chip, 10);
  lv_style_set_pad_ver(&registry.chip, 6);

  lv_style_init(&registry.button_primary);
  lv_style_set_bg_color(&registry.button_primary, lv_color_hex(0x2A6DF4));
  lv_style_set_bg_opa(&registry.button_primary, LV_OPA_COVER);
  lv_style_set_border_width(&registry.button_primary, 0);
  lv_style_set_radius(&registry.button_primary, 18);
  lv_style_set_pad_hor(&registry.button_primary, 14);
  lv_style_set_pad_ver(&registry.button_primary, 12);

  lv_style_init(&registry.button_secondary);
  lv_style_set_bg_color(&registry.button_secondary, lv_color_hex(0x152231));
  lv_style_set_bg_opa(&registry.button_secondary, LV_OPA_COVER);
  lv_style_set_border_color(&registry.button_secondary, lv_color_hex(0x23364A));
  lv_style_set_border_width(&registry.button_secondary, 1);
  lv_style_set_radius(&registry.button_secondary, 18);
  lv_style_set_pad_hor(&registry.button_secondary, 14);
  lv_style_set_pad_ver(&registry.button_secondary, 12);

  lv_style_init(&registry.button_pressed);
  lv_style_set_bg_opa(&registry.button_pressed, LV_OPA_80);
  lv_style_set_translate_y(&registry.button_pressed, 1);

  lv_style_init(&registry.text_eyebrow);
  lv_style_set_text_font(&registry.text_eyebrow, &lv_font_montserrat_12);
  lv_style_set_text_color(&registry.text_eyebrow, lv_color_hex(0x7FB0FF));
  lv_style_set_text_letter_space(&registry.text_eyebrow, 1);

  lv_style_init(&registry.text_hero);
  lv_style_set_text_font(&registry.text_hero, &lv_font_montserrat_42);
  lv_style_set_text_color(&registry.text_hero, lv_color_hex(0xF8FAFC));
  lv_style_set_text_align(&registry.text_hero, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&registry.text_hero_soft);
  lv_style_set_text_font(&registry.text_hero_soft, &lv_font_montserrat_42);
  lv_style_set_text_color(&registry.text_hero_soft, lv_color_hex(0x96A3B5));
  lv_style_set_text_align(&registry.text_hero_soft, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&registry.text_title);
  lv_style_set_text_font(&registry.text_title, &lv_font_montserrat_16);
  lv_style_set_text_color(&registry.text_title, lv_color_hex(0xF8FAFC));

  lv_style_init(&registry.text_body);
  lv_style_set_text_font(&registry.text_body, &lv_font_montserrat_14);
  lv_style_set_text_color(&registry.text_body, lv_color_hex(0xD5DFEA));

  lv_style_init(&registry.text_muted);
  lv_style_set_text_font(&registry.text_muted, &lv_font_montserrat_12);
  lv_style_set_text_color(&registry.text_muted, lv_color_hex(0x8A9AAF));

  lv_style_init(&registry.text_accent);
  lv_style_set_text_font(&registry.text_accent, &lv_font_montserrat_12);
  lv_style_set_text_color(&registry.text_accent, lv_color_hex(0x67E8F9));

  lv_style_init(&registry.text_button);
  lv_style_set_text_font(&registry.text_button, &lv_font_montserrat_14);
  lv_style_set_text_color(&registry.text_button, lv_color_hex(0xFFFFFF));
  lv_style_set_text_align(&registry.text_button, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&registry.text_tiny);
  lv_style_set_text_font(&registry.text_tiny, &lv_font_montserrat_12);
  lv_style_set_text_color(&registry.text_tiny, lv_color_hex(0x718197));

  return registry;
}

void add_main_style(lv_obj_t* obj, lv_style_t* style) {
  lv_obj_add_style(obj, style, LV_PART_MAIN);
}

}  // namespace

void ui_prepare_box(lv_obj_t* obj) {
  styles();
  lv_obj_remove_style_all(obj);
  lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(obj, 0, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
}

void ui_prepare_label(lv_obj_t* obj) {
  styles();
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
}

void ui_apply_surface(lv_obj_t* obj, SurfaceStyle style) {
  auto& registry = styles();
  switch (style) {
    case SurfaceStyle::Screen:
      add_main_style(obj, &registry.screen);
      break;
    case SurfaceStyle::Panel:
      add_main_style(obj, &registry.panel);
      break;
    case SurfaceStyle::PanelSubtle:
      add_main_style(obj, &registry.panel_subtle);
      break;
    case SurfaceStyle::Chip:
      add_main_style(obj, &registry.chip);
      break;
    case SurfaceStyle::ButtonPrimary:
      add_main_style(obj, &registry.button_primary);
      lv_obj_add_style(obj, &registry.button_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
      break;
    case SurfaceStyle::ButtonSecondary:
      add_main_style(obj, &registry.button_secondary);
      lv_obj_add_style(obj, &registry.button_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
      break;
  }
}

void ui_apply_text(lv_obj_t* obj, TextStyle style) {
  auto& registry = styles();
  switch (style) {
    case TextStyle::Eyebrow:
      add_main_style(obj, &registry.text_eyebrow);
      break;
    case TextStyle::Hero:
      add_main_style(obj, &registry.text_hero);
      break;
    case TextStyle::HeroSoft:
      add_main_style(obj, &registry.text_hero_soft);
      break;
    case TextStyle::Title:
      add_main_style(obj, &registry.text_title);
      break;
    case TextStyle::Body:
      add_main_style(obj, &registry.text_body);
      break;
    case TextStyle::Muted:
      add_main_style(obj, &registry.text_muted);
      break;
    case TextStyle::Accent:
      add_main_style(obj, &registry.text_accent);
      break;
    case TextStyle::Button:
      add_main_style(obj, &registry.text_button);
      break;
    case TextStyle::Tiny:
      add_main_style(obj, &registry.text_tiny);
      break;
  }
}

void ui_set_touch_target(lv_obj_t* obj, lv_coord_t extra) {
  lv_obj_set_ext_click_area(obj, extra);
}

void ui_set_flex_column(lv_obj_t* obj, lv_coord_t pad, lv_coord_t gap) {
  lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_all(obj, pad, 0);
  lv_obj_set_style_pad_gap(obj, gap, 0);
}

void ui_set_flex_row(lv_obj_t* obj,
                     lv_coord_t pad,
                     lv_coord_t gap,
                     lv_flex_align_t main_align,
                     lv_flex_align_t cross_align) {
  lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(obj, main_align, cross_align, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(obj, pad, 0);
  lv_obj_set_style_pad_gap(obj, gap, 0);
}

}  // namespace twsim::app
