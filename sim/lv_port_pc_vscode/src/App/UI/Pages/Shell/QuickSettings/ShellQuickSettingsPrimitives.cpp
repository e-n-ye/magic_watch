#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPrimitives.h"

#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/UiStyles.h"

#include <tuple>

namespace twsim::app::shell_quick_settings {

namespace {

using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

constexpr const char* kTextLongBatteryConfirmBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE5\x90\x8E\xE5\xB0\x86\xE5\x88\x87\xE6\x8D\xA2\xE8\x87\xB3\xE9\x95\xBF\xE7\xBB\xAD"
    "\xE8\x88\xAA\xE6\xA8\xA1\xE5\xBC\x8F\xE3\x80\x82";

bool build_backdrop_shell(lv_obj_t* root, QuickSettingsPrimitivesView& out) {
  out.backdrop_root = lv_obj_create(root);
  if (out.backdrop_root == nullptr) {
    return false;
  }

  lv_obj_set_size(out.backdrop_root, 240, 296);
  lv_obj_align(out.backdrop_root, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(out.backdrop_root, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(out.backdrop_root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out.backdrop_root, 0, 0);
  lv_obj_set_style_pad_all(out.backdrop_root, 0, 0);
  lv_obj_remove_flag(out.backdrop_root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(out.backdrop_root, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t* backdrop_overlay = lv_obj_create(out.backdrop_root);
  lv_obj_t* battery_row = lv_obj_create(out.backdrop_root);
  out.backdrop_battery_icon_label = lv_label_create(battery_row);
  out.backdrop_battery_label = lv_label_create(battery_row);
  out.backdrop_style_stage = lv_obj_create(out.backdrop_root);
  out.backdrop_minute_label = lv_label_create(out.backdrop_root);
  if (backdrop_overlay == nullptr || battery_row == nullptr || out.backdrop_battery_icon_label == nullptr ||
      out.backdrop_battery_label == nullptr || out.backdrop_style_stage == nullptr ||
      out.backdrop_minute_label == nullptr) {
    return false;
  }

  ui_prepare_box(backdrop_overlay);
  lv_obj_set_size(backdrop_overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(backdrop_overlay, lv_color_hex(0x01040A), 0);
  lv_obj_set_style_bg_opa(backdrop_overlay, LV_OPA_20, 0);
  lv_obj_set_style_border_width(backdrop_overlay, 0, 0);
  lv_obj_set_style_pad_all(backdrop_overlay, 0, 0);
  lv_obj_align(backdrop_overlay, LV_ALIGN_TOP_LEFT, 0, 0);

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(out.backdrop_battery_icon_label);
  ui_apply_text(out.backdrop_battery_icon_label, TextStyle::Tiny);
  lv_obj_set_style_text_font(out.backdrop_battery_icon_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(out.backdrop_battery_icon_label, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(out.backdrop_battery_icon_label, LV_SYMBOL_CHARGE);

  ui_prepare_label(out.backdrop_battery_label);
  ui_apply_text(out.backdrop_battery_label, TextStyle::Tiny);
  lv_obj_set_style_text_font(out.backdrop_battery_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(out.backdrop_battery_label, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(out.backdrop_battery_label, "--%");

  ui_prepare_box(out.backdrop_style_stage);
  lv_obj_set_size(out.backdrop_style_stage, 240, 296);
  lv_obj_align(out.backdrop_style_stage, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(out.backdrop_style_stage, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(out.backdrop_style_stage, 0, 0);
  lv_obj_set_style_pad_all(out.backdrop_style_stage, 0, 0);

  ui_prepare_label(out.backdrop_minute_label);
  ui_apply_text(out.backdrop_minute_label, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(out.backdrop_minute_label, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(out.backdrop_minute_label, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(out.backdrop_minute_label, 96);
  lv_obj_set_style_text_align(out.backdrop_minute_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(out.backdrop_minute_label, 76, 234);
  lv_label_set_text(out.backdrop_minute_label, "--");
  return true;
}

bool build_sheet_shell(lv_obj_t* root, QuickSettingsPrimitivesView& out) {
  out.sheet_container = lv_obj_create(root);
  if (out.sheet_container == nullptr) {
    return false;
  }

  lv_obj_set_size(out.sheet_container, 228, 228);
  lv_obj_set_style_bg_color(out.sheet_container, lv_color_hex(0x09131F), 0);
  lv_obj_set_style_bg_opa(out.sheet_container, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out.sheet_container, 0, 0);
  lv_obj_set_style_radius(out.sheet_container, 32, 0);
  lv_obj_set_style_pad_top(out.sheet_container, 18, 0);
  lv_obj_set_style_pad_bottom(out.sheet_container, 18, 0);
  lv_obj_set_style_pad_left(out.sheet_container, 16, 0);
  lv_obj_set_style_pad_right(out.sheet_container, 16, 0);
  lv_obj_set_style_shadow_width(out.sheet_container, 34, 0);
  lv_obj_set_style_shadow_color(out.sheet_container, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_shadow_opa(out.sheet_container, LV_OPA_50, 0);
  lv_obj_set_style_border_color(out.sheet_container, lv_color_hex(0x15263A), 0);
  lv_obj_set_style_border_opa(out.sheet_container, LV_OPA_50, 0);
  lv_obj_set_style_border_width(out.sheet_container, 1, 0);
  lv_obj_remove_flag(out.sheet_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_align(out.sheet_container, LV_ALIGN_TOP_MID, 0, 0);
  return true;
}

bool build_toast(lv_obj_t* root, QuickSettingsPrimitivesView& out) {
  out.toast_container = lv_obj_create(root);
  out.toast_label = out.toast_container == nullptr ? nullptr : lv_label_create(out.toast_container);
  if (out.toast_container == nullptr || out.toast_label == nullptr) {
    return false;
  }

  ui_prepare_box(out.toast_container);
  lv_obj_set_style_bg_color(out.toast_container, lv_color_hex(0x6A7D97), 0);
  lv_obj_set_style_bg_opa(out.toast_container, LV_OPA_80, 0);
  lv_obj_set_style_border_width(out.toast_container, 0, 0);
  lv_obj_set_style_radius(out.toast_container, 22, 0);
  lv_obj_set_style_pad_left(out.toast_container, 18, 0);
  lv_obj_set_style_pad_right(out.toast_container, 18, 0);
  lv_obj_set_style_pad_top(out.toast_container, 8, 0);
  lv_obj_set_style_pad_bottom(out.toast_container, 8, 0);
  lv_obj_remove_flag(out.toast_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(out.toast_container, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(out.toast_container, LV_OBJ_FLAG_HIDDEN);
  lv_obj_align(out.toast_container, LV_ALIGN_TOP_MID, 0, 28);
  lv_obj_set_size(out.toast_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_move_foreground(out.toast_container);

  ui_prepare_label(out.toast_label);
  ui_apply_text(out.toast_label, TextStyle::Body);
  lv_obj_set_style_text_font(out.toast_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(out.toast_label, lv_color_hex(0xF8FBFF), 0);
  lv_label_set_long_mode(out.toast_label, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_set_size(out.toast_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_text_align(out.toast_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(out.toast_label, "");
  lv_obj_center(out.toast_label);
  return true;
}

bool build_long_battery_confirm(lv_obj_t* root, QuickSettingsPrimitivesView& out) {
  out.long_battery_confirm_overlay = lv_obj_create(root);
  if (out.long_battery_confirm_overlay == nullptr) {
    return false;
  }

  ui_prepare_box(out.long_battery_confirm_overlay);
  lv_obj_set_size(out.long_battery_confirm_overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_align(out.long_battery_confirm_overlay, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(out.long_battery_confirm_overlay, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(out.long_battery_confirm_overlay, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out.long_battery_confirm_overlay, 0, 0);
  lv_obj_set_style_radius(out.long_battery_confirm_overlay, 0, 0);
  lv_obj_add_flag(out.long_battery_confirm_overlay, LV_OBJ_FLAG_HIDDEN);

  out.long_battery_confirm_body = lv_label_create(out.long_battery_confirm_overlay);
  out.long_battery_confirm_cancel_button = lv_button_create(out.long_battery_confirm_overlay);
  out.long_battery_confirm_confirm_button = lv_button_create(out.long_battery_confirm_overlay);
  if (out.long_battery_confirm_body == nullptr || out.long_battery_confirm_cancel_button == nullptr ||
      out.long_battery_confirm_confirm_button == nullptr) {
    return false;
  }

  ui_prepare_label(out.long_battery_confirm_body);
  lv_obj_set_width(out.long_battery_confirm_body, 196);
  lv_obj_set_style_text_font(out.long_battery_confirm_body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(out.long_battery_confirm_body, lv_color_hex(0xF7FBFF), 0);
  lv_obj_set_style_text_align(out.long_battery_confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(out.long_battery_confirm_body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(out.long_battery_confirm_body, kTextLongBatteryConfirmBody);
  lv_obj_align(out.long_battery_confirm_body, LV_ALIGN_TOP_MID, 0, 34);

  for (const auto [button, color] : {
           std::tuple {out.long_battery_confirm_cancel_button, lv_color_hex(0x17314C)},
           std::tuple {out.long_battery_confirm_confirm_button, lv_color_hex(0x11B8FF)}}) {
    ui_prepare_box(button);
    lv_obj_set_size(button, 92, 50);
    lv_obj_set_style_radius(button, 18, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, color, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return false;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE8FBFF), 0);
    lv_label_set_text(label, button == out.long_battery_confirm_cancel_button ? LV_SYMBOL_CLOSE : LV_SYMBOL_OK);
    lv_obj_center(label);
  }

  lv_obj_align(out.long_battery_confirm_cancel_button, LV_ALIGN_BOTTOM_LEFT, 24, -34);
  lv_obj_align(out.long_battery_confirm_confirm_button, LV_ALIGN_BOTTOM_RIGHT, -24, -34);
  lv_obj_move_foreground(out.long_battery_confirm_overlay);
  return true;
}

bool build_toggle_grid(QuickSettingsPrimitivesView& out) {
  out.drag_handle = lv_obj_create(out.sheet_container);
  out.toggle_grid = lv_obj_create(out.sheet_container);
  if (out.drag_handle == nullptr || out.toggle_grid == nullptr) {
    return false;
  }

  lv_obj_set_size(out.drag_handle, 52, 7);
  lv_obj_align(out.drag_handle, LV_ALIGN_TOP_MID, 0, 4);
  lv_obj_set_style_bg_color(out.drag_handle, lv_color_hex(0x4B5E7A), 0);
  lv_obj_set_style_bg_opa(out.drag_handle, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out.drag_handle, 0, 0);
  lv_obj_set_style_radius(out.drag_handle, LV_RADIUS_CIRCLE, 0);
  lv_obj_remove_flag(out.drag_handle, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(out.drag_handle, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_foreground(out.drag_handle);

  lv_obj_set_size(out.toggle_grid, 196, 196);
  lv_obj_align(out.toggle_grid, LV_ALIGN_CENTER, 0, 6);
  lv_obj_set_layout(out.toggle_grid, LV_LAYOUT_GRID);
  static lv_coord_t columns[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t rows[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(out.toggle_grid, columns, rows);
  lv_obj_set_style_pad_all(out.toggle_grid, 0, 0);
  lv_obj_set_style_pad_row(out.toggle_grid, 8, 0);
  lv_obj_set_style_pad_column(out.toggle_grid, 8, 0);
  lv_obj_set_style_bg_opa(out.toggle_grid, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(out.toggle_grid, 0, 0);
  lv_obj_set_style_radius(out.toggle_grid, 0, 0);
  lv_obj_remove_flag(out.toggle_grid, LV_OBJ_FLAG_SCROLLABLE);

  for (std::size_t index = 0; index < out.toggle_buttons.size(); ++index) {
    const auto row = static_cast<lv_coord_t>(index / 3);
    const auto col = static_cast<lv_coord_t>(index % 3);
    out.toggle_buttons[index] = lv_button_create(out.toggle_grid);
    if (out.toggle_buttons[index] == nullptr) {
      return false;
    }
    lv_obj_set_grid_cell(out.toggle_buttons[index], LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_size(out.toggle_buttons[index], 58, 58);
    lv_obj_set_style_radius(out.toggle_buttons[index], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(out.toggle_buttons[index], lv_color_hex(0x15294A), 0);
    lv_obj_set_style_bg_opa(out.toggle_buttons[index], LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(out.toggle_buttons[index], 0, 0);
    lv_obj_set_style_shadow_width(out.toggle_buttons[index], 0, 0);

    out.toggle_icon_labels[index] = lv_label_create(out.toggle_buttons[index]);
    if (out.toggle_icon_labels[index] == nullptr) {
      return false;
    }
    lv_obj_set_style_text_font(out.toggle_icon_labels[index], &lv_font_montserrat_24, 0);
    lv_obj_center(out.toggle_icon_labels[index]);
  }

  return true;
}

}  // namespace

bool create_quick_settings_primitives(lv_obj_t* root, QuickSettingsPrimitivesView& out) {
  if (root == nullptr) {
    return false;
  }

  return build_backdrop_shell(root, out) && build_sheet_shell(root, out) && build_toast(root, out) &&
         build_long_battery_confirm(root, out) && build_toggle_grid(out);
}

}  // namespace twsim::app::shell_quick_settings
