#include "App/UI/Pages/SettingsPages.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/Settings/Display/SettingsDisplayHelpers.h"
#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextRaiseToWake = "\xE6\x8A\xAC\xE8\x85\x95\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextScheduledOn =
    "\xE5\xAE\x9A\xE6\x97\xB6\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextAllDayOn =
    "\xE5\x85\xA8\xE5\xA4\xA9\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextStart = "\xE5\xBC\x80\xE5\xA7\x8B";
constexpr const char* kTextEnd = "\xE7\xBB\x93\xE6\x9D\x9F";
constexpr const char* kTextConfirm = LV_SYMBOL_OK;
constexpr const char* kTextRaiseConflictBody =
    "\xE8\xAF\xB7\xE9\x80\x89\xE6\x8B\xA9\xE6\x8A\xAC\xE8\x85\x95\xE5\x90\x8E\xE6\x98\xBE\xE7\xA4\xBA\xE7\x9A\x84"
    "\xE8\xA1\xA8\xE7\x9B\x98\xEF\xBC\x8C\xE9\x80\x89\xE6\x8B\xA9\xE6\x81\xAF\xE5\xB1\x8F\xE8\xA1\xA8\xE7\x9B\x98"
    "\xE5\xB0\x86\xE5\x85\xB3\xE9\x97\xAD\xE6\x8A\xAC\xE8\x85\x95\xE4\xBA\xAE\xE5\xB1\x8F\xE3\x80\x82";
constexpr const char* kTextScreenOffWatchface =
    "\xE6\x81\xAF\xE5\xB1\x8F\xE8\xA1\xA8\xE7\x9B\x98";
constexpr const char* kTextMainWatchface =
    "\xE4\xB8\xBB\xE8\xA1\xA8\xE7\x9B\x98";

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kTileHeight = 58;
constexpr lv_coord_t kTileHorizontalPadding = 16;

}  // namespace

DisplayRaiseToWakePage::DisplayRaiseToWakePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayRaiseToWake, kTextRaiseToWake, true),
      options_ {{
          {RaiseToWakeMode::Off, kTextOff, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {RaiseToWakeMode::AllDay, kTextAllDayOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {RaiseToWakeMode::Scheduled, kTextScheduledOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void DisplayRaiseToWakePage::on_will_appear() {
  hide_conflict_overlay();
  refresh_selection();
}

void DisplayRaiseToWakePage::on_will_disappear() {
  hide_conflict_overlay();
}

lv_obj_t* DisplayRaiseToWakePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.button = create_card_shell(scroll, true);
    if (option.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_user_data(option.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(option.button, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);

    option.title_label = lv_label_create(option.button);
    if (option.title_label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(option.title_label);
    lv_obj_set_style_text_font(option.title_label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(option.title_label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(option.title_label, option.label);
    lv_obj_align(option.title_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(option.button);
    if (option.check_label == nullptr) {
      return nullptr;
    }

    if (option.mode == RaiseToWakeMode::Scheduled) {
      option.start_row = lv_button_create(option.button);
      option.end_row = lv_button_create(option.button);
      if (option.start_row == nullptr || option.end_row == nullptr) {
        return nullptr;
      }
      for (lv_obj_t* row : {option.start_row, option.end_row}) {
        ui_prepare_box(row);
        lv_obj_set_size(row, 172, 54);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_radius(row, 12, 0);
        lv_obj_add_flag(row, LV_OBJ_FLAG_EVENT_BUBBLE);
        attach_click_guard(row);
      }
      lv_obj_align(option.start_row, LV_ALIGN_TOP_LEFT, 8, 44);
      lv_obj_align(option.end_row, LV_ALIGN_TOP_LEFT, 8, 112);
      lv_obj_set_user_data(option.start_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(100)));
      lv_obj_set_user_data(option.end_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(101)));
      lv_obj_add_event_cb(option.start_row, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_add_event_cb(option.end_row, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);

      option.start_title_label = lv_label_create(option.start_row);
      option.start_value_label = lv_label_create(option.start_row);
      option.end_title_label = lv_label_create(option.end_row);
      option.end_value_label = lv_label_create(option.end_row);
      if (option.start_title_label == nullptr || option.start_value_label == nullptr ||
          option.end_title_label == nullptr || option.end_value_label == nullptr) {
        return nullptr;
      }

      for (lv_obj_t* label : {option.start_title_label, option.end_title_label}) {
        ui_prepare_label(label);
        lv_obj_set_style_text_font(label, cjk_font_20(), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
      }
      for (lv_obj_t* label : {option.start_value_label, option.end_value_label}) {
        ui_prepare_label(label);
        lv_obj_set_style_text_font(label, cjk_font_18(), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xE5F3FF), 0);
      }

      lv_label_set_text(option.start_title_label, kTextStart);
      lv_label_set_text(option.end_title_label, kTextEnd);
      lv_obj_align(option.start_title_label, LV_ALIGN_TOP_LEFT, 8, 0);
      lv_obj_align(option.start_value_label, LV_ALIGN_BOTTOM_LEFT, 8, 0);
      lv_obj_align(option.end_title_label, LV_ALIGN_TOP_LEFT, 8, 0);
      lv_obj_align(option.end_value_label, LV_ALIGN_BOTTOM_LEFT, 8, 0);
    }
  }

  conflict_overlay_ = lv_obj_create(root);
  if (conflict_overlay_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(conflict_overlay_);
  lv_obj_set_size(conflict_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(conflict_overlay_);
  lv_obj_set_style_bg_color(conflict_overlay_, lv_color_hex(0x02060C), 0);
  lv_obj_set_style_bg_opa(conflict_overlay_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(conflict_overlay_, 0, 0);
  lv_obj_set_style_radius(conflict_overlay_, 0, 0);
  lv_obj_add_flag(conflict_overlay_, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* body = lv_label_create(conflict_overlay_);
  if (body == nullptr) {
    return nullptr;
  }
  ui_prepare_label(body);
  lv_obj_set_width(body, 184);
  lv_obj_set_style_text_font(body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(body, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_text_align(body, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(body, kTextRaiseConflictBody);
  lv_obj_align(body, LV_ALIGN_TOP_MID, 0, 22);

  for (const auto [action, text, y] : {std::tuple {0U, kTextScreenOffWatchface, 156},
                                       std::tuple {1U, kTextMainWatchface, 214}}) {
    lv_obj_t* button = lv_button_create(conflict_overlay_);
    if (button == nullptr) {
      return nullptr;
    }
    attach_click_guard(button);
    ui_prepare_box(button);
    lv_obj_set_size(button, 150, 48);
    lv_obj_align(button, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x28476C), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 18, 0);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(action)));
    lv_obj_add_event_cb(button, &DisplayRaiseToWakePage::conflict_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }

  refresh_selection();
  return root;
}

void DisplayRaiseToWakePage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index == 100U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWakeStartTime});
    return;
  }
  if (index == 101U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWakeEndTime});
    return;
  }
  if (index < self->options_.size()) {
    const auto mode = self->options_[index].mode;
    if (DisplayPolicyRules::HasRaiseToWakeAndScreenOffDisplayConflict(
            mode,
            current_display_policy(self->data_center_).screen_off_display_mode)) {
      self->show_conflict_overlay(mode);
      return;
    }
    self->data_center_.set_raise_to_wake_mode(mode);
    self->refresh_selection();
  }
}

void DisplayRaiseToWakePage::conflict_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->apply_pending_mode(action == 0U);
}

void DisplayRaiseToWakePage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  const auto& window = policy.raise_to_wake_window;
  const std::string start_text = format_hhmm(window.start_hour, window.start_minute);
  const std::string end_text = format_hhmm(window.end_hour, window.end_minute);

  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }

    const bool selected = option.mode == policy.raise_to_wake_mode;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);

    if (option.mode != RaiseToWakeMode::Scheduled) {
      continue;
    }

    const bool expanded = selected;
    lv_obj_set_height(option.button, expanded ? 182 : kTileHeight);
    if (expanded) {
      lv_obj_align(option.title_label, LV_ALIGN_TOP_LEFT, kTileHorizontalPadding, 12);
      lv_obj_align(option.check_label, LV_ALIGN_TOP_RIGHT, -16, 16);
      lv_obj_clear_flag(option.start_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.start_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.start_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(option.start_value_label, start_text.c_str());
      lv_label_set_text(option.end_value_label, end_text.c_str());
    } else {
      lv_obj_align(option.title_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
      lv_obj_align(option.check_label, LV_ALIGN_RIGHT_MID, -16, 0);
      lv_obj_add_flag(option.start_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.start_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.start_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_value_label, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void DisplayRaiseToWakePage::show_conflict_overlay(RaiseToWakeMode mode) {
  if (conflict_overlay_ == nullptr) {
    return;
  }
  pending_mode_ = mode;
  lv_obj_clear_flag(conflict_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(conflict_overlay_);
}

void DisplayRaiseToWakePage::hide_conflict_overlay() {
  pending_mode_ = RaiseToWakeMode::Off;
  if (conflict_overlay_ != nullptr) {
    lv_obj_add_flag(conflict_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void DisplayRaiseToWakePage::apply_pending_mode(bool use_screen_off_watchface) {
  const auto mode = pending_mode_;
  hide_conflict_overlay();
  if (use_screen_off_watchface) {
    data_center_.set_raise_to_wake_mode(RaiseToWakeMode::Off);
    refresh_selection();
    return;
  }

  data_center_.set_raise_to_wake_mode(mode);
  refresh_selection();
}

DisplayRaiseToWakeTimePage::DisplayRaiseToWakeTimePage(DataCenter& data_center,
                                                       PageId page_id,
                                                       const char* title,
                                                       bool edit_start)
    : SettingsPageBase(data_center, page_id, title, true), edit_start_(edit_start) {}

void DisplayRaiseToWakeTimePage::on_will_appear() {
  reload_from_policy();
}

lv_obj_t* DisplayRaiseToWakeTimePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  static const std::string kHourOptions = two_digit_options(23);
  static const std::string kMinuteOptions = two_digit_options(59);

  hour_roller_ = lv_roller_create(root);
  minute_roller_ = lv_roller_create(root);
  if (hour_roller_ == nullptr || minute_roller_ == nullptr) {
    return nullptr;
  }

  for (lv_obj_t* roller : {hour_roller_, minute_roller_}) {
    lv_obj_set_size(roller, 74, 162);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_style_bg_opa(roller, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, lv_color_hex(0x7FCBFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(roller, cjk_font_20(), LV_PART_MAIN);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roller, LV_OPA_20, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(roller, lv_color_hex(0x0C1725), LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller, lv_color_hex(0xBDF7FF), LV_PART_SELECTED);
    lv_obj_set_style_radius(roller, 18, LV_PART_SELECTED);
    lv_obj_add_event_cb(roller, &DisplayRaiseToWakeTimePage::roller_event_cb, LV_EVENT_VALUE_CHANGED, this);
  }

  lv_roller_set_options(hour_roller_, kHourOptions.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_roller_set_options(minute_roller_, kMinuteOptions.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_obj_align(hour_roller_, LV_ALIGN_CENTER, -48, -6);
  lv_obj_align(minute_roller_, LV_ALIGN_CENTER, 48, -6);

  lv_obj_t* colon = lv_label_create(root);
  if (colon == nullptr) {
    return nullptr;
  }
  ui_prepare_label(colon);
  lv_obj_set_style_text_font(colon, cjk_font_20(), 0);
  lv_obj_set_style_text_color(colon, lv_color_hex(0xBDF7FF), 0);
  lv_label_set_text(colon, ":");
  lv_obj_align(colon, LV_ALIGN_CENTER, 0, -6);

  confirm_button_ = lv_button_create(root);
  if (confirm_button_ == nullptr) {
    return nullptr;
  }
  attach_click_guard(confirm_button_);
  ui_prepare_box(confirm_button_);
  lv_obj_set_size(confirm_button_, 98, 44);
  lv_obj_align(confirm_button_, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_set_style_bg_color(confirm_button_, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(confirm_button_, 0, 0);
  lv_obj_set_style_radius(confirm_button_, 16, 0);
  lv_obj_add_event_cb(confirm_button_, &DisplayRaiseToWakeTimePage::confirm_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* confirm_label = lv_label_create(confirm_button_);
  if (confirm_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(confirm_label);
  lv_obj_set_style_text_font(confirm_label, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(confirm_label, lv_color_hex(0xD8FFF7), 0);
  lv_label_set_text(confirm_label, kTextConfirm);
  lv_obj_center(confirm_label);

  reload_from_policy();
  return root;
}

void DisplayRaiseToWakeTimePage::roller_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakeTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->hour_roller_ == nullptr || self->minute_roller_ == nullptr) {
    return;
  }
  self->pending_hour_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->hour_roller_));
  self->pending_minute_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->minute_roller_));
  self->update_preview();
}

void DisplayRaiseToWakeTimePage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakeTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->save_and_exit();
}

void DisplayRaiseToWakeTimePage::reload_from_policy() {
  const auto policy = current_display_policy(data_center_);
  pending_hour_ = edit_start_ ? policy.raise_to_wake_window.start_hour : policy.raise_to_wake_window.end_hour;
  pending_minute_ = edit_start_ ? policy.raise_to_wake_window.start_minute : policy.raise_to_wake_window.end_minute;
  if (hour_roller_ != nullptr) {
    lv_roller_set_selected(hour_roller_, pending_hour_, LV_ANIM_OFF);
  }
  if (minute_roller_ != nullptr) {
    lv_roller_set_selected(minute_roller_, pending_minute_, LV_ANIM_OFF);
  }
  update_preview();
}

void DisplayRaiseToWakeTimePage::update_preview() {}

void DisplayRaiseToWakeTimePage::save_and_exit() {
  auto policy = current_display_policy(data_center_);
  auto window = policy.raise_to_wake_window;
  if (edit_start_) {
    window.start_hour = pending_hour_;
    window.start_minute = pending_minute_;
  } else {
    window.end_hour = pending_hour_;
    window.end_minute = pending_minute_;
  }
  data_center_.set_raise_to_wake_window(window);
  request_navigation({NavigationAction::Pop, PageId::Watchface});
}

}  // namespace twsim::app
