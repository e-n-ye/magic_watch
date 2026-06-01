#include "App/UI/Pages/Daily/StepsMainPrimitives.h"

#include <algorithm>

#include "App/UI/Pages/Daily/StepsPagePrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

using shell_font::cjk_font_16;

std::size_t decimal_digits(std::uint32_t value) {
  std::size_t digits = 1;
  while (value >= 10U) {
    value /= 10U;
    ++digits;
  }
  return digits;
}

}  // namespace

const lv_font_t* steps_overview_value_font(std::uint32_t value) {
  return decimal_digits(value) >= 4 ? &lv_font_montserrat_24 : &lv_font_montserrat_28;
}

const lv_font_t* steps_card_value_font(std::uint32_t value) {
  return decimal_digits(value) >= 4 ? &lv_font_montserrat_28 : &lv_font_montserrat_42;
}

lv_obj_t* create_steps_arc(lv_obj_t* parent,
                           lv_coord_t size,
                           lv_coord_t width,
                           std::uint32_t color,
                           std::int32_t value,
                           std::int32_t max_value) {
  lv_obj_t* arc = lv_arc_create(parent);
  if (arc == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(arc, size, size);
  lv_arc_set_bg_angles(arc, 180, 360);
  lv_arc_set_angles(arc, 180, 180 + std::clamp(value * 180 / std::max(1, max_value), 6, 180));
  lv_obj_set_style_arc_width(arc, width, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x142030), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, lv_color_hex(color), LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(arc, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
  lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  return arc;
}

bool create_steps_metric_line(lv_obj_t* parent, lv_coord_t y, std::uint32_t accent, const char* value, const char* target) {
  lv_obj_t* icon = create_steps_icon(parent, 34, 0x1A2430, accent, accent == 0xF6D34D);
  lv_obj_t* value_label = create_steps_label(parent, value, &lv_font_montserrat_28, 0xF8FAFC, 74);
  lv_obj_t* target_label = create_steps_label(parent, target, &lv_font_montserrat_16, 0xB9C7D9, 80);
  if (icon == nullptr || value_label == nullptr || target_label == nullptr) {
    return false;
  }
  lv_obj_set_style_bg_color(icon, lv_color_mix(lv_color_hex(accent), lv_color_hex(0x0A1626), LV_OPA_70), 0);
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 18, y);
  lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, 64, y - 1);
  lv_obj_align_to(target_label, value_label, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  return true;
}

lv_obj_t* create_steps_distribution_card(lv_obj_t* parent,
                                         lv_coord_t width,
                                         lv_coord_t height,
                                         const char* title,
                                         const char* value,
                                         const char* unit,
                                         const char* target,
                                         std::uint32_t accent,
                                         bool use_foot_asset) {
  lv_obj_t* card = create_steps_panel(parent, width, height);
  if (card == nullptr) {
    return nullptr;
  }

  const lv_coord_t divider_y = clamp_coord(scale_by_ratio(height, 28, 100), 58, 66);
  const lv_coord_t value_y = clamp_coord(scale_by_ratio(height, 35, 100), 76, 86);
  const lv_coord_t target_y = clamp_coord(scale_by_ratio(height, 57, 100), 118, 132);
  const lv_coord_t tick_y = clamp_coord(scale_by_ratio(height, 68, 100), 140, 154);
  const lv_coord_t tick_label_y = clamp_coord(scale_by_ratio(height, 88, 100), 184, 198);
  const lv_coord_t dot_x = width - 30;
  const lv_coord_t dot_y = clamp_coord(scale_by_ratio(height, 67, 100), 136, 174);

  lv_obj_t* icon = create_steps_icon(card, 36, 0x1A2430, accent, use_foot_asset);
  lv_obj_t* title_label = create_steps_label(card, title, cjk_font_16(), 0xF8FAFC, width - 76);
  lv_obj_t* divider = lv_obj_create(card);
  lv_obj_t* value_label = create_steps_label(card, value, &lv_font_montserrat_42, 0xF8FAFC, 76);
  lv_obj_t* unit_label = create_steps_label(card, unit, cjk_font_16(), 0xF8FAFC, 88);
  lv_obj_t* target_label = create_steps_label(card, target, cjk_font_16(), 0xAFC4DA, width - 36);
  if (icon == nullptr || title_label == nullptr || divider == nullptr || value_label == nullptr ||
      unit_label == nullptr || target_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 20, 16);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 66, 22);

  ui_prepare_box(divider);
  lv_obj_set_size(divider, width - 36, 1);
  lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
  lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
  lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, divider_y);

  lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, 18, value_y);
  lv_obj_align_to(unit_label, value_label, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
  lv_obj_align(target_label, LV_ALIGN_TOP_LEFT, 22, target_y);

  lv_coord_t xs[] = {24, static_cast<lv_coord_t>(width / 2), static_cast<lv_coord_t>(width - 24)};
  const char* labels[] = {"0", "12", "24"};
  for (std::size_t i = 0; i < 3; ++i) {
    lv_obj_t* tick = lv_obj_create(card);
    lv_obj_t* tick_label = create_steps_label(card, labels[i], &lv_font_montserrat_16, 0x8DB9E3, 32);
    if (tick == nullptr || tick_label == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, 46);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5AA4DD), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_70, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, xs[i], tick_y);
    lv_obj_align(tick_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(xs[i] - 10), tick_label_y);
  }

  lv_obj_t* dot = lv_obj_create(card);
  if (dot == nullptr) {
    return nullptr;
  }
  ui_prepare_box(dot);
  lv_obj_set_size(dot, 7, 7);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, lv_color_hex(accent), 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_align(dot, LV_ALIGN_TOP_LEFT, dot_x, dot_y);

  return card;
}

}  // namespace twsim::app
