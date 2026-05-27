#include "App/UI/Pages/SettingsPages.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "App/UI/Pages/Settings/SettingsDisplayHelpers.h"
#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextDisplay = "\xE6\x98\xBE\xE7\xA4\xBA\xE4\xB8\x8E\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextBrightness = "\xE5\xB1\x8F\xE5\xB9\x95\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextAutoBrightness = "\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextCustomBrightness = "\xE8\x87\xAA\xE5\xAE\x9A\xE4\xB9\x89\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextScreenTimeout = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextRaiseToWake = "\xE6\x8A\xAC\xE8\x85\x95\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextKeepScreenOn = "\xE6\x8C\x81\xE7\xBB\xAD\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextTapToWake = "\xE5\x8D\x95\xE5\x87\xBB\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextCoverToSleep = "\xE9\x81\xAE\xE7\x9B\x96\xE6\x81\xAF\xE5\xB1\x8F";
constexpr const char* kTextScreenOffDisplay = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\x98\xBE\xE7\xA4\xBA";
constexpr const char* kTextScreenOffStyle = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F";
constexpr const char* kTextDefaultScreenOffStyle =
    "\xE9\xBB\x98\xE8\xAE\xA4\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F";
constexpr const char* kTextDefault =
    "\xE9\xBB\x98\xE8\xAE\xA4";
constexpr const char* kTextAuto = "\xE8\x87\xAA\xE5\x8A\xA8";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextBrightnessSaveHint =
    "\xE7\xA6\xBB\xE5\xBC\x80\xE9\xA1\xB5\xE9\x9D\xA2\xE6\x97\xB6\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBF\x9D\xE5\xAD\x98";
constexpr const char* kTextScreenOffStyleBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE6\x81\xAF\xE5\xB1\x8F\xE6\x98\xBE\xE7\xA4\xBA\xE5\x90\x8E\xEF\xBC\x8C"
    "\xE5\xB0\x86\xE4\xBC\x98\xE5\x85\x88\xE4\xBD\xBF\xE7\x94\xA8\xE8\xA1\xA8\xE7\x9B\x98\xE8\x87\xAA\xE5\xB8\xA6"
    "\xE7\x9A\x84\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F\xEF\xBC\x8C\xE8\x8B\xA5\xE6\x97\xA0\xE8\x87\xAA\xE5\xB8\xA6"
    "\xE6\xA0\xB7\xE5\xBC\x8F\xEF\xBC\x8C\xE5\x88\x99\xE4\xBD\xBF\xE7\x94\xA8\xE8\xAF\xA5\xE9\xBB\x98\xE8\xAE\xA4"
    "\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F\xE3\x80\x82";

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kTileHorizontalPadding = 16;

}  // namespace

DisplaySettingsPage::DisplaySettingsPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplay, kTextDisplay, true),
      rows_ {{
          {RowAction::OpenBrightness, kTextBrightness, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenScreenTimeout, kTextScreenTimeout, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenRaiseToWake, kTextRaiseToWake, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenKeepScreenOn, kTextKeepScreenOn, nullptr, nullptr, nullptr, nullptr},
          {RowAction::ToggleTapToWake, kTextTapToWake, nullptr, nullptr, nullptr, nullptr},
          {RowAction::ToggleCoverToSleep, kTextCoverToSleep, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenScreenOffDisplay, kTextScreenOffDisplay, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenScreenOffStyle, kTextDefaultScreenOffStyle, nullptr, nullptr, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_from_policy(); }));
}

void DisplaySettingsPage::on_will_appear() {
  refresh_from_policy();
}

lv_obj_t* DisplaySettingsPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  auto build_detail_row = [&](std::size_t index, const char* title, const char* detail) -> bool {
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return false;
    }
    rows_[index].button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &DisplaySettingsPage::row_event_cb, LV_EVENT_CLICKED, this);
    const auto parts = create_two_line_content(button, title, detail);
    rows_[index].title_label = parts.title;
    rows_[index].detail_label = parts.detail;
    return parts.title != nullptr && parts.detail != nullptr;
  };

  auto build_switch_row = [&](std::size_t index, const char* title) -> bool {
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return false;
    }
    rows_[index].button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &DisplaySettingsPage::row_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return false;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
    rows_[index].title_label = label;

    rows_[index].switch_track = create_switch_track(button);
    return rows_[index].switch_track != nullptr;
  };

  if (!build_detail_row(0, kTextBrightness, kTextAuto)) {
    return nullptr;
  }
  if (!build_detail_row(1, kTextScreenTimeout, "5\xE7\xA7\x92")) {
    return nullptr;
  }
  if (!build_detail_row(2, kTextRaiseToWake, kTextOff)) {
    return nullptr;
  }
  if (!build_detail_row(3, kTextKeepScreenOn, kTextOff)) {
    return nullptr;
  }
  if (!build_switch_row(4, kTextTapToWake)) {
    return nullptr;
  }
  if (!build_switch_row(5, kTextCoverToSleep)) {
    return nullptr;
  }
  if (!build_detail_row(6, kTextScreenOffDisplay, kTextOff)) {
    return nullptr;
  }
  if (!build_detail_row(7, kTextDefaultScreenOffStyle, kTextDefault)) {
    return nullptr;
  }

  if ((note_body_ = create_note_card(scroll, kTextScreenOffStyle, kTextScreenOffStyleBody)) == nullptr) {
    return nullptr;
  }

  refresh_from_policy();
  return root;
}

void DisplaySettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplaySettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->rows_.size()) {
    return;
  }

  const auto policy = current_display_policy(self->data_center_);
  switch (self->rows_[index].action) {
    case RowAction::OpenBrightness:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayBrightness});
      break;
    case RowAction::OpenScreenTimeout:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayTimeout});
      break;
    case RowAction::OpenRaiseToWake:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWake});
      break;
    case RowAction::OpenKeepScreenOn:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayKeepScreenOn});
      break;
    case RowAction::ToggleTapToWake:
      self->data_center_.set_tap_to_wake_enabled(!policy.tap_to_wake_enabled);
      break;
    case RowAction::ToggleCoverToSleep:
      self->data_center_.set_cover_to_sleep_enabled(!policy.cover_to_sleep_enabled);
      break;
    case RowAction::OpenScreenOffDisplay:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplay});
      break;
    case RowAction::OpenScreenOffStyle:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffStyle});
      break;
  }
}

void DisplaySettingsPage::refresh_from_policy() {
  if (rows_[0].detail_label == nullptr) {
    return;
  }

  const auto policy = current_display_policy(data_center_);
  lv_label_set_text(rows_[0].detail_label, brightness_detail_text(policy).c_str());
  lv_label_set_text(rows_[1].detail_label, timeout_text(policy.screen_off_timeout_ms));
  lv_label_set_text(rows_[2].detail_label, raise_to_wake_text(policy));
  lv_label_set_text(rows_[3].detail_label, keep_screen_on_text(policy.keep_screen_on_duration_ms));
  apply_switch_visual(rows_[4].switch_track, policy.tap_to_wake_enabled);
  apply_switch_visual(rows_[5].switch_track, policy.cover_to_sleep_enabled);
  lv_label_set_text(rows_[6].detail_label, screen_off_display_text(policy));
  lv_label_set_text(rows_[7].detail_label, screen_off_style_text(policy));
}

DisplayBrightnessPage::DisplayBrightnessPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayBrightness, kTextBrightness, true) {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_from_policy(); }));
}

void DisplayBrightnessPage::on_will_appear() {
  refresh_from_policy();
}

lv_obj_t* DisplayBrightnessPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* auto_row = create_card_shell(scroll, true);
  if (auto_row == nullptr) {
    return nullptr;
  }
  lv_obj_add_event_cb(auto_row, &DisplayBrightnessPage::row_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_set_user_data(auto_row, reinterpret_cast<void*>(0));
  lv_obj_t* auto_label = lv_label_create(auto_row);
  if (auto_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(auto_label);
  lv_obj_set_style_text_font(auto_label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(auto_label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(auto_label, kTextAutoBrightness);
  lv_obj_align(auto_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
  auto_switch_track_ = create_switch_track(auto_row);
  if (auto_switch_track_ == nullptr) {
    return nullptr;
  }

  manual_row_ = create_card_shell(scroll, true);
  if (manual_row_ == nullptr) {
    return nullptr;
  }
  lv_obj_add_event_cb(manual_row_, &DisplayBrightnessPage::row_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_set_user_data(manual_row_, reinterpret_cast<void*>(1));
  const auto manual_parts = create_two_line_content(manual_row_, kTextCustomBrightness, "60%");
  manual_detail_label_ = manual_parts.detail;
  if (manual_parts.title == nullptr || manual_detail_label_ == nullptr) {
    return nullptr;
  }

  refresh_from_policy();
  return root;
}

void DisplayBrightnessPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayBrightnessPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto row = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  const auto policy = current_display_policy(self->data_center_);
  if (row == 0) {
    self->data_center_.set_brightness_mode(policy.brightness_mode == BrightnessMode::Auto ? BrightnessMode::Manual
                                                                                           : BrightnessMode::Auto);
    return;
  }
  if (row == 1 && policy.brightness_mode == BrightnessMode::Manual) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayManualBrightness});
  }
}

void DisplayBrightnessPage::refresh_from_policy() {
  const auto policy = current_display_policy(data_center_);
  apply_switch_visual(auto_switch_track_, policy.brightness_mode == BrightnessMode::Auto);
  if (manual_row_ != nullptr) {
    const bool hidden = policy.brightness_mode == BrightnessMode::Auto;
    if (hidden) {
      lv_obj_add_flag(manual_row_, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(manual_row_, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (manual_detail_label_ != nullptr) {
    lv_label_set_text(manual_detail_label_, brightness_detail_text(policy).c_str());
  }
}

DisplayManualBrightnessPage::DisplayManualBrightnessPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayManualBrightness, kTextCustomBrightness, true) {}

void DisplayManualBrightnessPage::on_will_appear() {
  reload_from_policy();
}

void DisplayManualBrightnessPage::on_will_disappear() {
  if (!dirty_) {
    return;
  }
  data_center_.set_manual_brightness_level(pending_level_);
  dirty_ = false;
}

lv_obj_t* DisplayManualBrightnessPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  slider_ = lv_slider_create(root);
  if (slider_ == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(slider_, 48, 176);
  lv_obj_center(slider_);
  lv_slider_set_range(slider_, 0, 100);
  lv_obj_add_event_cb(slider_, &DisplayManualBrightnessPage::slider_event_cb, LV_EVENT_VALUE_CHANGED, this);
  lv_obj_set_style_radius(slider_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x6F8FB4), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_radius(slider_, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x16E3FF), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x16E3FF), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_TRANSP, LV_PART_KNOB);
  lv_obj_set_style_pad_all(slider_, 0, LV_PART_KNOB);

  value_label_ = lv_label_create(root);
  if (value_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(value_label_);
  lv_obj_set_style_text_font(value_label_, cjk_font_18(), 0);
  lv_obj_set_style_text_color(value_label_, lv_color_hex(0xDFFBFF), 0);
  lv_obj_align_to(value_label_, slider_, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  lv_obj_t* hint = lv_label_create(root);
  if (hint == nullptr) {
    return nullptr;
  }
  ui_prepare_label(hint);
  lv_obj_set_style_text_font(hint, cjk_font_16(), 0);
  lv_obj_set_style_text_color(hint, lv_color_hex(0xD8ECFF), 0);
  lv_label_set_text(hint, kTextBrightnessSaveHint);
  lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -18);

  reload_from_policy();
  return root;
}

void DisplayManualBrightnessPage::slider_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayManualBrightnessPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->slider_ == nullptr) {
    return;
  }

  if (self->suppress_slider_event_) {
    return;
  }

  if (self->should_ignore_click()) {
    self->suppress_slider_event_ = true;
    lv_slider_set_value(self->slider_, self->pending_level_, LV_ANIM_OFF);
    self->suppress_slider_event_ = false;
    self->apply_preview_value(self->pending_level_);
    return;
  }

  self->pending_level_ = static_cast<std::uint8_t>(lv_slider_get_value(self->slider_));
  self->dirty_ = true;
  self->apply_preview_value(self->pending_level_);
}

void DisplayManualBrightnessPage::reload_from_policy() {
  const auto policy = current_display_policy(data_center_);
  pending_level_ = policy.manual_brightness_level;
  if (slider_ != nullptr) {
    lv_slider_set_value(slider_, pending_level_, LV_ANIM_OFF);
  }
  apply_preview_value(pending_level_);
  dirty_ = false;
}

void DisplayManualBrightnessPage::apply_preview_value(std::uint8_t level) {
  if (value_label_ == nullptr) {
    return;
  }
  char buffer[16] = {};
  std::snprintf(buffer, sizeof(buffer), "%u%%", static_cast<unsigned>(level));
  lv_label_set_text(value_label_, buffer);
}

DisplayTimeoutPage::DisplayTimeoutPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayTimeout, kTextScreenTimeout, true),
      options_ {{
          {5000U, "5\xE7\xA7\x92", nullptr, nullptr},
          {10000U, "10\xE7\xA7\x92", nullptr, nullptr},
          {20000U, "20\xE7\xA7\x92", nullptr, nullptr},
          {30000U, "30\xE7\xA7\x92", nullptr, nullptr},
          {60000U, "60\xE7\xA7\x92", nullptr, nullptr},
      }} {}

void DisplayTimeoutPage::on_will_appear() {
  refresh_selection();
}

lv_obj_t* DisplayTimeoutPage::build() {
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
    lv_obj_add_event_cb(option.button, &DisplayTimeoutPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(option.button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, option.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(option.button);
    if (option.check_label == nullptr) {
      return nullptr;
    }
  }

  refresh_selection();
  return root;
}

void DisplayTimeoutPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayTimeoutPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }

  self->data_center_.set_screen_off_timeout_ms(self->options_[index].timeout_ms);
  self->refresh_selection();
}

void DisplayTimeoutPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }
    const bool selected = option.timeout_ms == policy.screen_off_timeout_ms;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);
  }
}

}  // namespace twsim::app
