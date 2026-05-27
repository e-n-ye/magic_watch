#include "App/UI/Pages/SettingsPages.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <utility>

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/Settings/SettingsDisplayHelpers.h"
#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextScreenOffDisplay = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\x98\xBE\xE7\xA4\xBA";
constexpr const char* kTextScreenOffStyle = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F";
constexpr const char* kTextSmartOn = "\xE6\x99\xBA\xE8\x83\xBD\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextScheduledOn =
    "\xE5\xAE\x9A\xE6\x97\xB6\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextDefault =
    "\xE9\xBB\x98\xE8\xAE\xA4";
constexpr const char* kTextAnalogHands =
    "\xE6\x97\xB6\xE5\x88\x86\xE6\x8C\x87\xE9\x92\x88";
constexpr const char* kTextInfoDigits =
    "\xE6\x97\xA5\xE6\x9C\x9F\xE6\x95\xB0\xE5\xAD\x97";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextStart = "\xE5\xBC\x80\xE5\xA7\x8B";
constexpr const char* kTextEnd = "\xE7\xBB\x93\xE6\x9D\x9F";
constexpr const char* kTextConfirm = LV_SYMBOL_OK;
constexpr const char* kTextCancel = LV_SYMBOL_CLOSE;
constexpr const char* kTextScreenOffDisplayConfirmBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE5\x90\x8E\xEF\xBC\x8C\xE8\xAE\xBE\xE5\xA4\x87\xE7\xBB\xAD\xE8\x88\xAA\xE6\x97\xB6"
    "\xE9\x97\xB4\xE5\xB0\x86\xE4\xBC\x9A\xE4\xB8\xA5\xE9\x87\x8D\xE5\x87\x8F\xE5\xB0\x91\xEF\xBC\x8C"
    "\xE7\xA1\xAE\xE8\xAE\xA4\xE5\xBC\x80\xE5\x90\xAF\xEF\xBC\x9F";
constexpr const char* kTextScreenOffStyleBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE6\x81\xAF\xE5\xB1\x8F\xE6\x98\xBE\xE7\xA4\xBA\xE5\x90\x8E\xEF\xBC\x8C"
    "\xE5\xB0\x86\xE4\xBC\x98\xE5\x85\x88\xE4\xBD\xBF\xE7\x94\xA8\xE8\xA1\xA8\xE7\x9B\x98\xE8\x87\xAA\xE5\xB8\xA6"
    "\xE7\x9A\x84\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F\xEF\xBC\x8C\xE8\x8B\xA5\xE6\x97\xA0\xE8\x87\xAA\xE5\xB8\xA6"
    "\xE6\xA0\xB7\xE5\xBC\x8F\xEF\xBC\x8C\xE5\x88\x99\xE4\xBD\xBF\xE7\x94\xA8\xE8\xAF\xA5\xE9\xBB\x98\xE8\xAE\xA4"
    "\xE6\x81\xAF\xE5\xB1\x8F\xE6\xA0\xB7\xE5\xBC\x8F\xE3\x80\x82";
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

DisplayScreenOffDisplayPage::DisplayScreenOffDisplayPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayScreenOffDisplay, kTextScreenOffDisplay, true),
      options_ {{
          {ScreenOffDisplayMode::Off, kTextOff, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {ScreenOffDisplayMode::Smart, kTextSmartOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {ScreenOffDisplayMode::Scheduled, kTextScheduledOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void DisplayScreenOffDisplayPage::on_will_appear() {
  hide_confirm_overlay();
  hide_conflict_overlay();
  refresh_selection();
}

void DisplayScreenOffDisplayPage::on_will_disappear() {
  hide_confirm_overlay();
  hide_conflict_overlay();
}

lv_obj_t* DisplayScreenOffDisplayPage::build() {
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
    lv_obj_add_event_cb(option.button, &DisplayScreenOffDisplayPage::option_event_cb, LV_EVENT_CLICKED, this);

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

    if (option.mode == ScreenOffDisplayMode::Scheduled) {
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
      lv_obj_set_user_data(option.start_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(200)));
      lv_obj_set_user_data(option.end_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(201)));
      lv_obj_add_event_cb(option.start_row, &DisplayScreenOffDisplayPage::option_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_add_event_cb(option.end_row, &DisplayScreenOffDisplayPage::option_event_cb, LV_EVENT_CLICKED, this);

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

  overlay_ = lv_obj_create(root);
  if (overlay_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(overlay_);
  lv_obj_set_size(overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(overlay_);
  lv_obj_set_style_bg_color(overlay_, lv_color_hex(0x02060C), 0);
  lv_obj_set_style_bg_opa(overlay_, LV_OPA_90, 0);
  lv_obj_set_style_border_width(overlay_, 0, 0);
  lv_obj_set_style_radius(overlay_, 0, 0);
  lv_obj_add_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(overlay_);

  lv_obj_t* body = lv_label_create(overlay_);
  if (body == nullptr) {
    return nullptr;
  }
  ui_prepare_label(body);
  lv_obj_set_width(body, 186);
  lv_obj_set_style_text_font(body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(body, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_text_align(body, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_obj_align(body, LV_ALIGN_CENTER, 0, -28);
  lv_obj_set_user_data(overlay_, body);

  lv_obj_t* cancel_button = lv_button_create(overlay_);
  lv_obj_t* confirm_button = lv_button_create(overlay_);
  if (cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }
  for (lv_obj_t* button : {cancel_button, confirm_button}) {
    ui_prepare_box(button);
    lv_obj_set_size(button, 84, 50);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 18, 0);
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &DisplayScreenOffDisplayPage::confirm_event_cb, LV_EVENT_CLICKED, this);
  }
  lv_obj_align(cancel_button, LV_ALIGN_BOTTOM_LEFT, 24, -24);
  lv_obj_align(confirm_button, LV_ALIGN_BOTTOM_RIGHT, -24, -24);
  lv_obj_set_style_bg_color(cancel_button, lv_color_hex(0x6B88A7), 0);
  lv_obj_set_style_bg_opa(cancel_button, LV_OPA_80, 0);
  lv_obj_set_style_bg_color(confirm_button, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button, LV_OPA_COVER, 0);
  lv_obj_set_user_data(cancel_button, reinterpret_cast<void*>(0U));
  lv_obj_set_user_data(confirm_button, reinterpret_cast<void*>(1U));

  for (const auto [button, text] : {std::pair {cancel_button, kTextCancel}, std::pair {confirm_button, kTextConfirm}}) {
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE2FCFF), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
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

  lv_obj_t* conflict_body = lv_label_create(conflict_overlay_);
  if (conflict_body == nullptr) {
    return nullptr;
  }
  ui_prepare_label(conflict_body);
  lv_obj_set_width(conflict_body, 184);
  lv_obj_set_style_text_font(conflict_body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(conflict_body, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_text_align(conflict_body, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_long_mode(conflict_body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(conflict_body, kTextRaiseConflictBody);
  lv_obj_align(conflict_body, LV_ALIGN_TOP_MID, 0, 22);

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
    lv_obj_add_event_cb(button, &DisplayScreenOffDisplayPage::confirm_event_cb, LV_EVENT_CLICKED, this);

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

void DisplayScreenOffDisplayPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffDisplayPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index == 200U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayStartTime});
    return;
  }
  if (index == 201U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayEndTime});
    return;
  }
  if (index >= self->options_.size()) {
    return;
  }

  const auto mode = self->options_[index].mode;
  if (mode == ScreenOffDisplayMode::Off) {
    self->hide_confirm_overlay();
    self->hide_conflict_overlay();
    self->data_center_.set_screen_off_display_mode(ScreenOffDisplayMode::Off);
    self->refresh_selection();
    return;
  }

  self->show_confirm_overlay(mode);
}

void DisplayScreenOffDisplayPage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffDisplayPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  if (lv_obj_get_parent(target) == self->conflict_overlay_) {
    self->finalize_pending_mode(action == 0U);
    return;
  }
  if (action == 1U) {
    self->apply_pending_mode();
    return;
  }
  self->hide_confirm_overlay();
}

void DisplayScreenOffDisplayPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  const auto& window = policy.screen_off_display_window;
  const std::string start_text = format_hhmm(window.start_hour, window.start_minute);
  const std::string end_text = format_hhmm(window.end_hour, window.end_minute);
  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }
    const bool selected = option.mode == policy.screen_off_display_mode;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);

    if (option.mode != ScreenOffDisplayMode::Scheduled) {
      continue;
    }

    const bool expanded = selected;
    lv_obj_set_height(option.button, expanded ? 182 : kTileHeight);
    if (expanded) {
      lv_obj_align(option.title_label, LV_ALIGN_TOP_LEFT, kTileHorizontalPadding, 12);
      lv_obj_align(option.check_label, LV_ALIGN_TOP_RIGHT, -16, 16);
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

void DisplayScreenOffDisplayPage::show_confirm_overlay(ScreenOffDisplayMode mode) {
  if (overlay_ == nullptr) {
    return;
  }
  pending_mode_ = mode;
  if (auto* body = static_cast<lv_obj_t*>(lv_obj_get_user_data(overlay_)); body != nullptr) {
    lv_label_set_text(body, kTextScreenOffDisplayConfirmBody);
  }
  lv_obj_clear_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(overlay_);
}

void DisplayScreenOffDisplayPage::hide_confirm_overlay() {
  if (overlay_ != nullptr) {
    lv_obj_add_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void DisplayScreenOffDisplayPage::apply_pending_mode() {
  const auto mode = pending_mode_;
  hide_confirm_overlay();
  if (mode == ScreenOffDisplayMode::Off) {
    refresh_selection();
    return;
  }
  if (DisplayPolicyRules::HasRaiseToWakeAndScreenOffDisplayConflict(
          current_display_policy(data_center_).raise_to_wake_mode,
          mode)) {
    show_conflict_overlay();
    return;
  }
  finalize_pending_mode(false);
}

void DisplayScreenOffDisplayPage::show_conflict_overlay() {
  if (conflict_overlay_ == nullptr) {
    return;
  }
  lv_obj_clear_flag(conflict_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(conflict_overlay_);
}

void DisplayScreenOffDisplayPage::hide_conflict_overlay() {
  if (conflict_overlay_ != nullptr) {
    lv_obj_add_flag(conflict_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void DisplayScreenOffDisplayPage::finalize_pending_mode(bool use_screen_off_watchface) {
  const auto mode = pending_mode_;
  hide_conflict_overlay();
  if (mode == ScreenOffDisplayMode::Off) {
    refresh_selection();
    return;
  }
  if (use_screen_off_watchface) {
    data_center_.set_raise_to_wake_mode(RaiseToWakeMode::Off);
  }
  data_center_.set_screen_off_display_mode(mode);
  pending_mode_ = ScreenOffDisplayMode::Off;
  refresh_selection();
}

DisplayScreenOffDisplaySchedulePage::DisplayScreenOffDisplaySchedulePage(DataCenter& data_center)
    : SettingsPageBase(data_center,
                       PageId::SettingDisplayScreenOffDisplaySchedule,
                       kTextScreenOffDisplay,
                       true) {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_from_policy(); }));
}

void DisplayScreenOffDisplaySchedulePage::on_will_appear() {
  refresh_from_policy();
}

lv_obj_t* DisplayScreenOffDisplaySchedulePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* card = lv_obj_create(scroll);
  if (card == nullptr) {
    return nullptr;
  }
  style_panel(card, lv_color_hex(0x16314C), 20);
  lv_obj_set_width(card, LV_PCT(100));
  lv_obj_set_height(card, LV_SIZE_CONTENT);
  ui_set_flex_column(card, 0, 0);
  lv_obj_set_style_pad_top(card, 12, 0);
  lv_obj_set_style_pad_bottom(card, 12, 0);

  for (std::size_t index = 0; index < rows_.size(); ++index) {
    auto& row = rows_[index];
    row.button = create_card_shell(card, true);
    if (row.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(row.button, LV_PCT(100));
    lv_obj_set_style_bg_opa(row.button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(row.button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row.button, 0, 0);
    lv_obj_set_style_radius(row.button, 0, 0);
    lv_obj_set_style_pad_top(row.button, 12, 0);
    lv_obj_set_style_pad_bottom(row.button, 10, 0);
    lv_obj_set_user_data(row.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(row.button, &DisplayScreenOffDisplaySchedulePage::row_event_cb, LV_EVENT_CLICKED, this);

    const auto parts = create_two_line_content(
        row.button,
        index == 0 ? kTextStart : kTextEnd,
        index == 0 ? "08:00" : "22:00");
    row.title_label = parts.title;
    row.value_label = parts.detail;
    if (row.title_label == nullptr || row.value_label == nullptr) {
      return nullptr;
    }
  }

  refresh_from_policy();
  return root;
}

void DisplayScreenOffDisplaySchedulePage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffDisplaySchedulePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index == 0U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayStartTime});
  } else if (index == 1U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayEndTime});
  }
}

void DisplayScreenOffDisplaySchedulePage::refresh_from_policy() {
  const auto window = current_display_policy(data_center_).screen_off_display_window;
  if (rows_[0].value_label != nullptr) {
    const std::string text = format_hhmm(window.start_hour, window.start_minute);
    lv_label_set_text(rows_[0].value_label, text.c_str());
  }
  if (rows_[1].value_label != nullptr) {
    const std::string text = format_hhmm(window.end_hour, window.end_minute);
    lv_label_set_text(rows_[1].value_label, text.c_str());
  }
}

DisplayScreenOffStylePage::DisplayScreenOffStylePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayScreenOffStyle, kTextScreenOffStyle, true) {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged, [this](const Event&) {
    sync_pending_from_policy();
    refresh_selection();
  }));
}

void DisplayScreenOffStylePage::on_will_appear() {
  sync_pending_from_policy();
  refresh_selection();
  scroll_to_pending(false);
}

#if 0
lv_obj_t* DisplayScreenOffStylePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* card = lv_obj_create(root);
  if (card == nullptr) {
    return nullptr;
  }
  style_panel(card, lv_color_hex(0x0A1522), 24);
  lv_obj_set_size(card, 198, 184);
  lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 56);

  preview_card_ = card;
  style_name_label_ = lv_label_create(root);
  if (style_name_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(style_name_label_);
  lv_obj_set_style_text_font(style_name_label_, cjk_font_18(), 0);
  lv_obj_set_style_text_color(style_name_label_, lv_color_hex(0xF6FAFF), 0);
  lv_obj_align(style_name_label_, LV_ALIGN_TOP_MID, 0, 28);

  lv_obj_t* body = lv_label_create(root);
  if (body == nullptr) {
    return nullptr;
  }
  ui_prepare_label(body);
  lv_obj_set_width(body, 198);
  lv_obj_set_style_text_font(body, cjk_font_14(), 0);
  lv_obj_set_style_text_color(body, lv_color_hex(0xA9C7E2), 0);
  lv_obj_set_style_text_align(body, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(body, kTextScreenOffStyleBody);
  lv_obj_align(body, LV_ALIGN_BOTTOM_MID, 0, -18);

  refresh_selection();
  return root;
}

void DisplayScreenOffStylePage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffStylePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto style = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)) == 0U
                         ? ScreenOffStyleId::AnalogHands
                         : ScreenOffStyleId::InfoDigits;
  self->data_center_.set_screen_off_style_id(style);
}

void DisplayScreenOffStylePage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  const bool selected = policy.screen_off_style_id == ScreenOffStyleId::InfoDigits;
  lv_obj_set_style_border_color(preview_card_,
                                selected ? lv_color_hex(0x11B8FF) : lv_color_hex(0x35506A),
                                0);
  lv_obj_set_style_bg_color(preview_card_,
                            selected ? lv_color_hex(0x0D1B2B) : lv_color_hex(0x09131E),
                            0);
  if (style_name_label_ != nullptr) {
    lv_label_set_text(style_name_label_, screen_off_style_text(policy));
  }
}
#endif

lv_obj_t* DisplayScreenOffStylePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  const auto mark_passthrough = [](lv_obj_t* object) {
    if (object == nullptr) {
      return;
    }
    lv_obj_remove_flag(object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(object, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(object, LV_OBJ_FLAG_GESTURE_BUBBLE);
  };

  carousel_ = lv_obj_create(root);
  if (carousel_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(carousel_);
  lv_obj_set_size(carousel_, 240, 232);
  lv_obj_align(carousel_, LV_ALIGN_TOP_MID, 0, 42);
  lv_obj_set_style_bg_opa(carousel_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(carousel_, 0, 0);
  lv_obj_set_style_pad_left(carousel_, 28, 0);
  lv_obj_set_style_pad_right(carousel_, 28, 0);
  lv_obj_set_style_pad_top(carousel_, 0, 0);
  lv_obj_set_style_pad_bottom(carousel_, 0, 0);
  lv_obj_set_style_pad_column(carousel_, 16, 0);
  lv_obj_add_flag(carousel_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scroll_dir(carousel_, LV_DIR_HOR);
  lv_obj_set_scroll_snap_x(carousel_, LV_SCROLL_SNAP_CENTER);
  lv_obj_set_scrollbar_mode(carousel_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(carousel_, LV_OBJ_FLAG_SCROLL_ONE);
  lv_obj_set_layout(carousel_, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(carousel_, LV_FLEX_FLOW_ROW);
  lv_obj_add_event_cb(carousel_, &DisplayScreenOffStylePage::scroll_event_cb, LV_EVENT_SCROLL_END, this);

  options_[0].title = kTextAnalogHands;
  options_[1].title = kTextInfoDigits;

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.card = lv_button_create(carousel_);
    if (option.card == nullptr) {
      return nullptr;
    }
    attach_click_guard(option.card);
    style_panel(option.card, lv_color_hex(0x09131E), 28);
    lv_obj_set_size(option.card, 184, 218);
    lv_obj_set_style_border_width(option.card, 2, 0);
    lv_obj_set_style_border_color(option.card, lv_color_hex(0x35506A), 0);
    lv_obj_set_style_shadow_width(option.card, 0, 0);
    lv_obj_add_flag(option.card, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(option.card, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_user_data(option.card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));
    lv_obj_add_event_cb(option.card, &DisplayScreenOffStylePage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* style_tag = lv_label_create(option.card);
    if (style_tag == nullptr) {
      return nullptr;
    }
    ui_prepare_label(style_tag);
    lv_obj_set_style_text_font(style_tag, cjk_font_12(), 0);
    lv_obj_set_style_text_color(style_tag, lv_color_hex(0x7FB9FF), 0);
    lv_label_set_text(style_tag, kTextDefault);
    lv_obj_align(style_tag, LV_ALIGN_TOP_MID, 0, 10);
    mark_passthrough(style_tag);

    lv_obj_t* frame = lv_obj_create(option.card);
    if (frame == nullptr) {
      return nullptr;
    }
    ui_prepare_box(frame);
    lv_obj_set_size(frame, 128, 138);
    lv_obj_align(frame, LV_ALIGN_TOP_MID, 0, 32);
    lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(frame, 2, 0);
    lv_obj_set_style_border_color(frame, lv_color_hex(0x7FCBFF), 0);
    lv_obj_set_style_radius(frame, 24, 0);
    mark_passthrough(frame);

    if (option.style_id == ScreenOffStyleId::AnalogHands) {
      static const lv_point_precise_t kHourPoints[] = {{54, 76}, {82, 63}};
      static const lv_point_precise_t kMinutePoints[] = {{54, 76}, {96, 48}};
      lv_obj_t* hour_hand = lv_line_create(frame);
      lv_obj_t* minute_hand = lv_line_create(frame);
      lv_obj_t* center_dot = lv_obj_create(frame);
      lv_obj_t* battery = lv_label_create(option.card);
      if (hour_hand == nullptr || minute_hand == nullptr || center_dot == nullptr || battery == nullptr) {
        return nullptr;
      }
      lv_line_set_points(hour_hand, kHourPoints, 2);
      lv_obj_set_size(hour_hand, LV_PCT(100), LV_PCT(100));
      lv_obj_set_style_line_width(hour_hand, 7, 0);
      lv_obj_set_style_line_color(hour_hand, lv_color_hex(0xE2F6FF), 0);
      lv_obj_set_style_line_rounded(hour_hand, true, 0);
      mark_passthrough(hour_hand);

      lv_line_set_points(minute_hand, kMinutePoints, 2);
      lv_obj_set_size(minute_hand, LV_PCT(100), LV_PCT(100));
      lv_obj_set_style_line_width(minute_hand, 7, 0);
      lv_obj_set_style_line_color(minute_hand, lv_color_hex(0xE2F6FF), 0);
      lv_obj_set_style_line_rounded(minute_hand, true, 0);
      mark_passthrough(minute_hand);

      ui_prepare_box(center_dot);
      lv_obj_set_size(center_dot, 8, 8);
      lv_obj_set_style_radius(center_dot, LV_RADIUS_CIRCLE, 0);
      lv_obj_set_style_bg_color(center_dot, lv_color_hex(0xE2F6FF), 0);
      lv_obj_set_style_bg_opa(center_dot, LV_OPA_COVER, 0);
      lv_obj_set_style_border_width(center_dot, 0, 0);
      lv_obj_align(center_dot, LV_ALIGN_CENTER, 0, 0);
      mark_passthrough(center_dot);

      ui_prepare_label(battery);
      lv_obj_set_style_text_font(battery, &lv_font_montserrat_16, 0);
      lv_obj_set_style_text_color(battery, lv_color_hex(0xD6F3FF), 0);
      lv_label_set_text(battery, "80%");
      lv_obj_align_to(battery, frame, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
      mark_passthrough(battery);
    } else {
      lv_obj_t* hour_label = lv_label_create(frame);
      lv_obj_t* minute_label = lv_label_create(frame);
      lv_obj_t* date_label = lv_label_create(frame);
      lv_obj_t* battery_label = lv_label_create(frame);
      if (hour_label == nullptr || minute_label == nullptr || date_label == nullptr || battery_label == nullptr) {
        return nullptr;
      }
      ui_prepare_label(hour_label);
      lv_obj_set_style_text_font(hour_label, &lv_font_montserrat_42, 0);
      lv_obj_set_style_text_color(hour_label, lv_color_hex(0xD6F3FF), 0);
      lv_label_set_text(hour_label, "09");
      lv_obj_align(hour_label, LV_ALIGN_TOP_MID, 0, 12);
      mark_passthrough(hour_label);

      ui_prepare_label(minute_label);
      lv_obj_set_style_text_font(minute_label, &lv_font_montserrat_42, 0);
      lv_obj_set_style_text_color(minute_label, lv_color_hex(0xD6F3FF), 0);
      lv_label_set_text(minute_label, "28");
      lv_obj_align_to(minute_label, hour_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);
      mark_passthrough(minute_label);

      ui_prepare_label(date_label);
      lv_obj_set_style_text_font(date_label, cjk_font_12(), 0);
      lv_obj_set_style_text_color(date_label, lv_color_hex(0xD6F3FF), 0);
      lv_label_set_text(date_label,
                        "8\xE6\x9C\x88"
                        "16"
                        "\xE6\x97\xA5 "
                        "\xE5\x91\xA8\xE4\xBA\x8C");
      lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, -24);
      mark_passthrough(date_label);

      ui_prepare_label(battery_label);
      lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_14, 0);
      lv_obj_set_style_text_color(battery_label, lv_color_hex(0xD6F3FF), 0);
      lv_label_set_text(battery_label, "80%");
      lv_obj_align(battery_label, LV_ALIGN_BOTTOM_MID, 0, -8);
      mark_passthrough(battery_label);
    }
  }

  style_name_label_ = lv_label_create(root);
  if (style_name_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(style_name_label_);
  lv_obj_set_style_text_font(style_name_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(style_name_label_, lv_color_hex(0xBFDFFF), 0);
  lv_label_set_text(style_name_label_, kTextAnalogHands);
  lv_obj_align(style_name_label_, LV_ALIGN_TOP_MID, 0, 254);

  confirm_button_ = lv_button_create(root);
  if (confirm_button_ == nullptr) {
    return nullptr;
  }
  attach_click_guard(confirm_button_);
  ui_prepare_box(confirm_button_);
  lv_obj_set_size(confirm_button_, 132, 44);
  lv_obj_align(confirm_button_, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_set_style_bg_color(confirm_button_, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(confirm_button_, 0, 0);
  lv_obj_set_style_radius(confirm_button_, 16, 0);
  lv_obj_add_event_cb(confirm_button_, &DisplayScreenOffStylePage::confirm_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* confirm_label = lv_label_create(confirm_button_);
  if (confirm_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(confirm_label);
  lv_obj_set_style_text_font(confirm_label, cjk_font_18(), 0);
  lv_obj_set_style_text_color(confirm_label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(confirm_label, "\xE7\xA1\xAE\xE5\xAE\x9A");
  lv_obj_center(confirm_label);

  sync_pending_from_policy();
  refresh_selection();
  scroll_to_pending(false);
  return root;
}

void DisplayScreenOffStylePage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffStylePage*>(lv_event_get_user_data(event));
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

  self->pending_style_id_ = self->options_[index].style_id;
  self->refresh_selection();
  self->scroll_to_pending(true);
}

void DisplayScreenOffStylePage::scroll_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffStylePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->update_pending_from_scroll();
  self->refresh_selection();
}

void DisplayScreenOffStylePage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffStylePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->data_center_.set_screen_off_style_id(self->pending_style_id_);
  self->request_navigation({NavigationAction::Pop, self->id()});
}

void DisplayScreenOffStylePage::sync_pending_from_policy() {
  pending_style_id_ = current_display_policy(data_center_).screen_off_style_id;
}

void DisplayScreenOffStylePage::refresh_selection() {
  if (style_name_label_ == nullptr) {
    return;
  }

  for (const auto& option : options_) {
    if (option.card == nullptr) {
      continue;
    }
    const bool selected = option.style_id == pending_style_id_;
    lv_obj_set_style_border_color(option.card,
                                  selected ? lv_color_hex(0x11B8FF) : lv_color_hex(0x35506A),
                                  0);
    lv_obj_set_style_bg_color(option.card,
                              selected ? lv_color_hex(0x0D1B2B) : lv_color_hex(0x09131E),
                              0);
  }
  lv_label_set_text(style_name_label_,
                    pending_style_id_ == ScreenOffStyleId::InfoDigits ? kTextInfoDigits : kTextAnalogHands);
}

void DisplayScreenOffStylePage::update_pending_from_scroll() {
  if (carousel_ == nullptr) {
    return;
  }

  lv_area_t carousel_area {};
  lv_obj_get_coords(carousel_, &carousel_area);
  const lv_coord_t viewport_center = static_cast<lv_coord_t>((carousel_area.x1 + carousel_area.x2) / 2);
  lv_coord_t best_delta = LV_COORD_MAX;
  ScreenOffStyleId best_style = pending_style_id_;

  for (const auto& option : options_) {
    if (option.card == nullptr) {
      continue;
    }
    lv_area_t card_area {};
    lv_obj_get_coords(option.card, &card_area);
    const lv_coord_t card_center = static_cast<lv_coord_t>((card_area.x1 + card_area.x2) / 2);
    const lv_coord_t delta = static_cast<lv_coord_t>(LV_ABS(card_center - viewport_center));
    if (delta < best_delta) {
      best_delta = delta;
      best_style = option.style_id;
    }
  }

  pending_style_id_ = best_style;
}

void DisplayScreenOffStylePage::scroll_to_pending(bool animated) {
  for (const auto& option : options_) {
    if (option.card == nullptr || option.style_id != pending_style_id_) {
      continue;
    }
    lv_obj_scroll_to_view(option.card, animated ? LV_ANIM_ON : LV_ANIM_OFF);
    return;
  }
}

DisplayScreenOffDisplayTimePage::DisplayScreenOffDisplayTimePage(DataCenter& data_center,
                                                                 PageId page_id,
                                                                 const char* title,
                                                                 bool edit_start)
    : SettingsPageBase(data_center, page_id, title, true), edit_start_(edit_start) {}

void DisplayScreenOffDisplayTimePage::on_will_appear() {
  reload_from_policy();
}

lv_obj_t* DisplayScreenOffDisplayTimePage::build() {
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
    lv_obj_add_event_cb(roller, &DisplayScreenOffDisplayTimePage::roller_event_cb, LV_EVENT_VALUE_CHANGED, this);
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
  lv_obj_add_event_cb(confirm_button_, &DisplayScreenOffDisplayTimePage::confirm_event_cb, LV_EVENT_CLICKED, this);

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

void DisplayScreenOffDisplayTimePage::roller_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffDisplayTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->hour_roller_ == nullptr || self->minute_roller_ == nullptr) {
    return;
  }
  self->pending_hour_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->hour_roller_));
  self->pending_minute_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->minute_roller_));
  self->update_preview();
}

void DisplayScreenOffDisplayTimePage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayScreenOffDisplayTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->save_and_exit();
}

void DisplayScreenOffDisplayTimePage::reload_from_policy() {
  const auto policy = current_display_policy(data_center_);
  pending_hour_ = edit_start_ ? policy.screen_off_display_window.start_hour : policy.screen_off_display_window.end_hour;
  pending_minute_ =
      edit_start_ ? policy.screen_off_display_window.start_minute : policy.screen_off_display_window.end_minute;
  if (hour_roller_ != nullptr) {
    lv_roller_set_selected(hour_roller_, pending_hour_, LV_ANIM_OFF);
  }
  if (minute_roller_ != nullptr) {
    lv_roller_set_selected(minute_roller_, pending_minute_, LV_ANIM_OFF);
  }
  update_preview();
}

void DisplayScreenOffDisplayTimePage::update_preview() {}

void DisplayScreenOffDisplayTimePage::save_and_exit() {
  auto policy = current_display_policy(data_center_);
  auto window = policy.screen_off_display_window;
  if (edit_start_) {
    window.start_hour = pending_hour_;
    window.start_minute = pending_minute_;
  } else {
    window.end_hour = pending_hour_;
    window.end_minute = pending_minute_;
  }
  data_center_.set_screen_off_display_window(window);
  request_navigation({NavigationAction::Pop, PageId::Watchface});
}

}  // namespace twsim::app
