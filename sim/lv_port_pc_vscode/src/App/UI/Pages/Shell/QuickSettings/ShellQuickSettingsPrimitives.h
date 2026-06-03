#pragma once

#include <array>

#include "lvgl/lvgl.h"

namespace twsim::app::shell_quick_settings {

constexpr std::size_t kToggleCount = 9;

struct QuickSettingsPrimitivesView {
  lv_obj_t* backdrop_root {nullptr};
  lv_obj_t* backdrop_battery_icon_label {nullptr};
  lv_obj_t* backdrop_battery_label {nullptr};
  lv_obj_t* backdrop_style_stage {nullptr};
  lv_obj_t* backdrop_minute_label {nullptr};
  lv_obj_t* sheet_container {nullptr};
  lv_obj_t* toast_container {nullptr};
  lv_obj_t* toast_label {nullptr};
  lv_obj_t* long_battery_confirm_overlay {nullptr};
  lv_obj_t* long_battery_confirm_body {nullptr};
  lv_obj_t* long_battery_confirm_cancel_button {nullptr};
  lv_obj_t* long_battery_confirm_confirm_button {nullptr};
  lv_obj_t* drag_handle {nullptr};
  lv_obj_t* toggle_grid {nullptr};
  std::array<lv_obj_t*, kToggleCount> toggle_buttons {};
  std::array<lv_obj_t*, kToggleCount> toggle_icon_labels {};
};

bool create_quick_settings_primitives(lv_obj_t* root, QuickSettingsPrimitivesView& out);

}  // namespace twsim::app::shell_quick_settings
