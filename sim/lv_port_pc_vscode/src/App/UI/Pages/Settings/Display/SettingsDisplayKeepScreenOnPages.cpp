#include "App/UI/Pages/SettingsPages.h"

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <utility>

#include "App/UI/Pages/Settings/Display/SettingsDisplayHelpers.h"
#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextKeepScreenOn = "\xE6\x8C\x81\xE7\xBB\xAD\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextConfirm = LV_SYMBOL_OK;
constexpr const char* kTextCancel = LV_SYMBOL_CLOSE;
constexpr const char* kTextKeepScreenOnConfirmBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE5\x90\x8E\xEF\xBC\x8C\xE4\xBB\x85\xE6\x9C\xAC\xE6\xAC\xA1\xE4\xBA\xAE\xE5\xB1\x8F"
    "\xE6\x97\xB6\xE9\x97\xB4\xE6\x8C\x81\xE7\xBB\xAD%s\xEF\xBC\x8C\xE4\xBA\xAE\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4"
    "\xE8\xBF\x87\xE9\x95\xBF\xEF\xBC\x8C\xE4\xBC\x9A\xE5\x87\x8F\xE5\xB0\x91\xE7\xBB\xAD\xE8\x88\xAA\xE6\x97\xB6"
    "\xE9\x97\xB4\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE4\xBD\xBF\xE7\x94\xA8\xEF\xBC\x9F";

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kTileHorizontalPadding = 16;

}  // namespace

DisplayKeepScreenOnPage::DisplayKeepScreenOnPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayKeepScreenOn, kTextKeepScreenOn, true),
      options_ {{
          {0U, kTextOff, nullptr, nullptr},
          {300000U, "5\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {600000U, "10\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {900000U, "15\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {1200000U, "20\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void DisplayKeepScreenOnPage::on_will_appear() {
  hide_confirm_overlay();
  refresh_selection();
}

void DisplayKeepScreenOnPage::on_will_disappear() {
  hide_confirm_overlay();
}

lv_obj_t* DisplayKeepScreenOnPage::build() {
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
    lv_obj_add_event_cb(option.button, &DisplayKeepScreenOnPage::option_event_cb, LV_EVENT_CLICKED, this);

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
    lv_obj_add_event_cb(button, &DisplayKeepScreenOnPage::confirm_event_cb, LV_EVENT_CLICKED, this);
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

  refresh_selection();
  return root;
}

void DisplayKeepScreenOnPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayKeepScreenOnPage*>(lv_event_get_user_data(event));
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

  const auto duration_ms = self->options_[index].duration_ms;
  if (duration_ms == 0U) {
    self->hide_confirm_overlay();
    self->data_center_.set_keep_screen_on_duration_ms(0U);
    self->refresh_selection();
    return;
  }

  self->show_confirm_overlay(duration_ms);
}

void DisplayKeepScreenOnPage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayKeepScreenOnPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  if (action == 1U) {
    self->apply_pending_duration();
    return;
  }
  self->hide_confirm_overlay();
}

void DisplayKeepScreenOnPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }
    const bool selected = option.duration_ms == policy.keep_screen_on_duration_ms;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);
  }
}

void DisplayKeepScreenOnPage::show_confirm_overlay(std::uint32_t duration_ms) {
  if (overlay_ == nullptr) {
    return;
  }
  pending_duration_ms_ = duration_ms;
  auto* body = static_cast<lv_obj_t*>(lv_obj_get_user_data(overlay_));
  if (body != nullptr) {
    char buffer[256] = {};
    std::snprintf(buffer, sizeof(buffer), kTextKeepScreenOnConfirmBody, keep_screen_on_text(duration_ms));
    lv_label_set_text(body, buffer);
  }
  lv_obj_clear_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(overlay_);
}

void DisplayKeepScreenOnPage::hide_confirm_overlay() {
  pending_duration_ms_ = 0U;
  if (overlay_ != nullptr) {
    lv_obj_add_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void DisplayKeepScreenOnPage::apply_pending_duration() {
  if (pending_duration_ms_ == 0U) {
    hide_confirm_overlay();
    return;
  }
  data_center_.set_keep_screen_on_duration_ms(pending_duration_ms_);
  hide_confirm_overlay();
  refresh_selection();
}

}  // namespace twsim::app
