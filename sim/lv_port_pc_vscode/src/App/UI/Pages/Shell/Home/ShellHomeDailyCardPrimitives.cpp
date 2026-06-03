#include "App/UI/Pages/Shell/Home/ShellHomeDailyCardPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/UiStyles.h"

namespace twsim::app::shell_home_daily {

namespace {

bool create_metric_icon(lv_obj_t* card, const char* icon_path) {
  lv_obj_t* icon_root = lv_obj_create(card);
  lv_obj_t* icon_image = icon_root == nullptr ? nullptr : lv_image_create(icon_root);
  if (icon_root == nullptr || icon_image == nullptr) {
    return false;
  }

  ui_prepare_box(icon_root);
  lv_obj_set_size(icon_root, 58, 58);
  lv_obj_set_style_bg_opa(icon_root, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(icon_root, 0, 0);
  lv_obj_align(icon_root, LV_ALIGN_TOP_LEFT, 0, 2);
  lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_set_size(icon_image, 58, 58);
  lv_image_set_inner_align(icon_image, LV_IMAGE_ALIGN_CONTAIN);
  lv_image_set_src(icon_image, icon_path);
  lv_obj_center(icon_image);
  lv_obj_remove_flag(icon_image, LV_OBJ_FLAG_CLICKABLE);
  return true;
}

bool create_metric_card(lv_obj_t* card,
                        lv_color_t bg,
                        lv_color_t border,
                        lv_coord_t x,
                        const char* icon_path,
                        bool emphasize,
                        lv_obj_t*& out_value_label) {
  ui_prepare_box(card);
  ui_apply_surface(card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(card, 106, 106);
  lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, 136);
  lv_obj_set_style_radius(card, 24, 0);
  lv_obj_set_style_pad_all(card, 0, 0);
  lv_obj_set_style_bg_color(card, bg, 0);
  lv_obj_set_style_border_color(card, border, 0);

  if (!create_metric_icon(card, icon_path)) {
    return false;
  }

  out_value_label = lv_label_create(card);
  if (out_value_label == nullptr) {
    return false;
  }

  ui_prepare_label(out_value_label);
  ui_apply_text(out_value_label, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(out_value_label, emphasize ? &lv_font_montserrat_16 : &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(out_value_label, lv_color_hex(0xFFFFFF), 0);
  shell_home::set_single_line_label(out_value_label, 72);
  lv_obj_align(out_value_label, LV_ALIGN_TOP_LEFT, emphasize ? 2 : 6, 70);
  return true;
}

bool create_weather_card(lv_obj_t* parent, const shell_home::SurfaceLayout& layout, DailyCardsView& out) {
  const lv_color_t weather_bg = lv_color_hex(0x0E2B56);
  const lv_color_t weather_accent = lv_color_hex(0x7EC8FF);

  out.weather_card = lv_obj_create(out.stage);
  if (out.weather_card == nullptr) {
    return false;
  }

  ui_prepare_box(out.weather_card);
  ui_apply_surface(out.weather_card, SurfaceStyle::Panel);
  lv_obj_set_size(out.weather_card, layout.stage_w, 110);
  lv_obj_align(out.weather_card, LV_ALIGN_TOP_LEFT, 0, 15);
  lv_obj_set_style_bg_color(out.weather_card, weather_bg, 0);
  lv_obj_set_style_border_color(out.weather_card, lv_color_mix(weather_accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
  lv_obj_set_style_radius(out.weather_card, 28, 0);
  lv_obj_set_style_pad_all(out.weather_card, 0, 0);

  out.weather_temp_label = lv_label_create(out.weather_card);
  out.weather_range_label = lv_label_create(out.weather_card);
  lv_obj_t* weather_icon_image = lv_image_create(out.weather_card);
  if (out.weather_temp_label == nullptr || out.weather_range_label == nullptr || weather_icon_image == nullptr) {
    return false;
  }

  ui_prepare_label(out.weather_temp_label);
  ui_apply_text(out.weather_temp_label, TextStyle::Hero);
  lv_obj_set_style_text_font(out.weather_temp_label, &lv_font_montserrat_34, 0);
  lv_obj_set_style_text_color(out.weather_temp_label, lv_color_hex(0xFFFFFF), 0);
  shell_home::set_single_line_label(out.weather_temp_label, 90);
  lv_obj_align(out.weather_temp_label, LV_ALIGN_TOP_LEFT, 15, 23);

  ui_prepare_label(out.weather_range_label);
  ui_apply_text(out.weather_range_label, TextStyle::Title);
  lv_obj_set_style_text_font(out.weather_range_label, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(out.weather_range_label, lv_color_hex(0xC8D7EE), 0);
  shell_home::set_single_line_label(out.weather_range_label, 76);
  lv_obj_align(out.weather_range_label, LV_ALIGN_TOP_LEFT, 28, 68);

  const char* weather_icon_path = shell_asset::weather_icon_asset_path();
  if (shell_asset::file_exists(weather_icon_path)) {
    lv_image_set_src(weather_icon_image, weather_icon_path);
    lv_obj_set_size(weather_icon_image, 73, 73);
    lv_image_set_inner_align(weather_icon_image, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_align(weather_icon_image, LV_ALIGN_TOP_LEFT, 140, 18);
  } else {
    lv_obj_add_flag(weather_icon_image, LV_OBJ_FLAG_HIDDEN);
  }

  return true;
}

}  // namespace

bool create_daily_cards(lv_obj_t* parent, const shell_home::SurfaceLayout& layout, DailyCardsView& out) {
  const lv_color_t stage_bg = lv_color_hex(0x040812);
  const lv_color_t sleep_bg = lv_color_hex(0x3A1B69);
  const lv_color_t sleep_accent = lv_color_hex(0xC06CFF);
  const lv_color_t steps_bg = lv_color_hex(0x054A42);
  const lv_color_t steps_accent = lv_color_hex(0x19F57A);

  out.stage = lv_obj_create(parent);
  out.sleep_card = out.stage == nullptr ? nullptr : lv_obj_create(out.stage);
  out.steps_card = out.stage == nullptr ? nullptr : lv_obj_create(out.stage);
  if (out.stage == nullptr || out.sleep_card == nullptr || out.steps_card == nullptr) {
    return false;
  }

  shell_home::style_surface_stage(out.stage, layout.stage_w, layout.stage_h, layout.stage_radius, stage_bg);
  lv_obj_align(out.stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);

  if (!create_weather_card(parent, layout, out)) {
    return false;
  }

  return create_metric_card(out.sleep_card,
                            sleep_bg,
                            lv_color_mix(sleep_accent, lv_color_hex(0xFFFFFF), LV_OPA_20),
                            1,
                            shell_asset::sleep_icon_asset_path(),
                            false,
                            out.sleep_value_label) &&
         create_metric_card(out.steps_card,
                            steps_bg,
                            lv_color_mix(steps_accent, lv_color_hex(0xFFFFFF), LV_OPA_20),
                            114,
                            shell_asset::steps_icon_asset_path(),
                            true,
                            out.steps_value_label);
}

}  // namespace twsim::app::shell_home_daily
