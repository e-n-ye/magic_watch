#include "App/UI/Pages/Health/BloodOxygenPages.h"

#include "App/UI/Pages/Health/BloodOxygenPageHelpers.h"
#include "App/UI/Pages/Health/HealthIconPrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Health/HealthSwitchPrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace {

using shell_asset::sleep_icon_asset_path;
using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

}  // namespace

BloodOxygenAppPage::BloodOxygenAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenAppPage::id() const {
  return PageId::AppBloodOxygen;
}

const char* BloodOxygenAppPage::name() const {
  return page_name(PageId::AppBloodOxygen);
}

void BloodOxygenAppPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "血氧饱和度", cjk_font_20(), 0xF8FAFC, 120);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 16, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* chart_card = create_steps_panel(scroll_root_, card_w, 124, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 16;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 82;
  for (int i = 0; i < 5; ++i) {
    lv_obj_t* tick = lv_obj_create(chart_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, chart_h);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5A82B2), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(chart_left + (chart_w * i) / 4), chart_top);
  }

  lv_obj_t* top_value = create_steps_label(chart_card, "95", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "85", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* left_label = create_steps_label(chart_card, "00:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* mid_label = create_steps_label(chart_card, "12:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* right_label = create_steps_label(chart_card, "24:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  if (top_value == nullptr || bottom_value == nullptr || left_label == nullptr || mid_label == nullptr ||
      right_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(top_value, LV_ALIGN_TOP_RIGHT, -10, 6);
  lv_obj_align(bottom_value, LV_ALIGN_BOTTOM_RIGHT, -10, -12);
  lv_obj_align(left_label, LV_ALIGN_BOTTOM_LEFT, 14, -12);
  lv_obj_align(mid_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(right_label, LV_ALIGN_BOTTOM_RIGHT, -24, -12);

  lv_obj_t* dot = lv_obj_create(chart_card);
  if (dot == nullptr) {
    return nullptr;
  }
  ui_prepare_box(dot);
  lv_obj_set_size(dot, 8, 8);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, lv_color_hex(0xFF4F72), 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_align(dot, LV_ALIGN_TOP_RIGHT, -34, 18);

  lv_obj_t* value_label =
      create_steps_label(scroll_root_, "95%", &lv_font_montserrat_48, 0xF8FAFC, card_w, LV_LABEL_LONG_CLIP);
  lv_obj_t* update_label = create_steps_label(scroll_root_, "20:37更新", cjk_font_20(), 0xD8E9FF, card_w, LV_LABEL_LONG_CLIP);
  if (value_label == nullptr || update_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(update_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* measure_button = create_steps_panel(scroll_root_, card_w, 74, 0x111D2E);
  lv_obj_t* measure_label = measure_button == nullptr
                                ? nullptr
                                : create_steps_label(measure_button, "开始测量", cjk_font_20(), 0xFF4F72, card_w - 24);
  if (measure_button == nullptr || measure_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(measure_button, 26, 0);
  lv_obj_set_style_bg_color(measure_button, lv_color_hex(0x0D1624), 0);
  lv_obj_set_style_text_align(measure_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(measure_label);

  for (const auto& entry :
       {std::pair<const char*, PageId> {"设置", PageId::AppBloodOxygenSettings},
        std::pair<const char*, PageId> {"说明", PageId::AppBloodOxygenInfo}}) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 86, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    const bool is_settings = entry.second == PageId::AppBloodOxygenSettings;
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             0x4C1732,
                                             0xFF8FB3,
                                             nullptr,
                                             is_settings ? LV_SYMBOL_SETTINGS : "i",
                                             is_settings ? &lv_font_montserrat_18 : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, entry.first, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(entry.second)));
  }

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
}

void BloodOxygenAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&BloodOxygenAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void BloodOxygenAppPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

BloodOxygenSettingsPage::BloodOxygenSettingsPage(DataCenter& data_center) : PageBase(data_center) {
  rows_[0].kind = RowKind::AllDayMonitoring;
  rows_[0].title = "全天血氧监测";
  rows_[1].kind = RowKind::LowOxygenReminder;
  rows_[1].title = "低血氧提醒";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_rows();
                                 }
                               }));
}

PageId BloodOxygenSettingsPage::id() const {
  return PageId::AppBloodOxygenSettings;
}

const char* BloodOxygenSettingsPage::name() const {
  return page_name(PageId::AppBloodOxygenSettings);
}

void BloodOxygenSettingsPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_rows();
}

void BloodOxygenSettingsPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "血氧设置", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* monitoring_row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
  lv_obj_t* monitoring_title = monitoring_row == nullptr
                                   ? nullptr
                                   : create_steps_label(monitoring_row, rows_[0].title, cjk_font_20(), 0xF8FAFC, card_w - 112, LV_LABEL_LONG_WRAP);
  lv_obj_t* monitoring_switch = monitoring_row == nullptr ? nullptr : create_sleep_switch_track(monitoring_row);
  if (monitoring_row == nullptr || monitoring_title == nullptr || monitoring_switch == nullptr) {
    return nullptr;
  }
  lv_obj_align(monitoring_title, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(monitoring_switch, LV_ALIGN_RIGHT_MID, -18, 0);
  attach_click_guard(monitoring_row);
  lv_obj_add_event_cb(monitoring_row, switch_event_cb, LV_EVENT_CLICKED, this);
  rows_[0].row = monitoring_row;
  rows_[0].switch_track = monitoring_switch;

  lv_obj_t* reminder_row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
  lv_obj_t* reminder_title = reminder_row == nullptr
                                 ? nullptr
                                 : create_steps_label(reminder_row, rows_[1].title, cjk_font_20(), 0xF8FAFC, card_w - 32, LV_LABEL_LONG_WRAP);
  lv_obj_t* reminder_status = reminder_row == nullptr ? nullptr : create_steps_label(reminder_row, "不提醒", cjk_font_16(), 0xAFC4DA, card_w - 32);
  if (reminder_row == nullptr || reminder_title == nullptr || reminder_status == nullptr) {
    return nullptr;
  }
  lv_obj_align(reminder_title, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(reminder_status, LV_ALIGN_TOP_LEFT, 18, 56);
  attach_click_guard(reminder_row);
  lv_obj_add_event_cb(reminder_row, row_event_cb, LV_EVENT_CLICKED, this);
  rows_[1].row = reminder_row;
  rows_[1].status_label = reminder_status;

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenSettingsPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenSettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::AppBloodOxygenLowOxygenReminder});
}

void BloodOxygenSettingsPage::switch_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->data_center_.set_all_day_blood_oxygen_enabled(!self->current_settings_.all_day_blood_oxygen_enabled);
}

void BloodOxygenSettingsPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenSettingsPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenSettingsPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void BloodOxygenSettingsPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenSettingsPage::refresh_rows() {
  if (rows_[0].switch_track != nullptr) {
    apply_sleep_switch_style(rows_[0].switch_track, current_settings_.all_day_blood_oxygen_enabled);
  }
  if (rows_[1].row != nullptr) {
    if (current_settings_.all_day_blood_oxygen_enabled) {
      lv_obj_remove_flag(rows_[1].row, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_add_flag(rows_[1].row, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (rows_[1].status_label != nullptr) {
    lv_label_set_text(rows_[1].status_label, reminder_status_text());
  }
}

const char* BloodOxygenSettingsPage::reminder_status_text() const {
  return low_blood_oxygen_mode_text(current_settings_.low_blood_oxygen_reminder_mode);
}

void BloodOxygenSettingsPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(
      lv_timer_create(&BloodOxygenSettingsPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void BloodOxygenSettingsPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

BloodOxygenLowOxygenReminderPage::BloodOxygenLowOxygenReminderPage(DataCenter& data_center) : PageBase(data_center) {
  options_[0].mode = LowBloodOxygenReminderMode::Off;
  options_[0].label = "不提醒";
  options_[1].mode = LowBloodOxygenReminderMode::Threshold90;
  options_[1].label = "90%";
  options_[2].mode = LowBloodOxygenReminderMode::Threshold85;
  options_[2].label = "85%";
  options_[3].mode = LowBloodOxygenReminderMode::Threshold80;
  options_[3].label = "80%";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_options();
                                 }
                               }));
}

PageId BloodOxygenLowOxygenReminderPage::id() const {
  return PageId::AppBloodOxygenLowOxygenReminder;
}

const char* BloodOxygenLowOxygenReminderPage::name() const {
  return page_name(PageId::AppBloodOxygenLowOxygenReminder);
}

void BloodOxygenLowOxygenReminderPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_options();
}

void BloodOxygenLowOxygenReminderPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenLowOxygenReminderPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label =
      back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "低血氧提醒", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* intro_text =
      "开启后，在非睡眠状态下，检测到血氧饱和度在一定时间内低于设定值时设备上出现提醒通知。";
  lv_obj_t* intro_label =
      create_steps_label(scroll_root_, intro_text, cjk_font_16(), 0xEAF2FF, card_w - 8, LV_LABEL_LONG_WRAP);
  if (intro_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(intro_label, 8, 0);
  lv_obj_align(intro_label, LV_ALIGN_TOP_LEFT, 4, 0);

  for (std::size_t i = 0; i < options_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 78, 0x102033);
    lv_obj_t* label = row == nullptr ? nullptr : create_steps_label(row, options_[i].label, cjk_font_20(), 0xF8FAFC, 72);
    lv_obj_t* dot = row == nullptr ? nullptr : lv_obj_create(row);
    if (row == nullptr || label == nullptr || dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 24, 24);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x5D728E), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x1A2D44), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -18, 0);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, option_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    options_[i].row = row;
    options_[i].check_dot = dot;
  }

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenLowOxygenReminderPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenLowOxygenReminderPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_event_get_user_data(event));
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
  self->data_center_.set_low_blood_oxygen_reminder_mode(self->options_[index].mode);
}

void BloodOxygenLowOxygenReminderPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenLowOxygenReminderPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenLowOxygenReminderPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenLowOxygenReminderPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void BloodOxygenLowOxygenReminderPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenLowOxygenReminderPage::refresh_options() {
  for (const auto& option : options_) {
    if (option.check_dot == nullptr) {
      continue;
    }
    const bool selected = option.mode == current_settings_.low_blood_oxygen_reminder_mode;
    lv_obj_set_style_border_color(option.check_dot, lv_color_hex(selected ? 0x14B8FF : 0x5D728E), 0);
    lv_obj_set_style_bg_color(option.check_dot, lv_color_hex(selected ? 0xEAF6FF : 0x1A2D44), 0);
  }
}

void BloodOxygenLowOxygenReminderPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(
      lv_timer_create(&BloodOxygenLowOxygenReminderPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void BloodOxygenLowOxygenReminderPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

BloodOxygenInfoPage::BloodOxygenInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenInfoPage::id() const {
  return PageId::AppBloodOxygenInfo;
}

const char* BloodOxygenInfoPage::name() const {
  return page_name(PageId::AppBloodOxygenInfo);
}

void BloodOxygenInfoPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenInfoPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "血氧说明", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* info_text =
      "说明\n\n"
      "1. 血氧饱和度是血液中氧合血红蛋白（HbO2）容量占全部可结合血红蛋白（Hb）的百分比，"
      "它是人体呼吸循环的重要生理参数。\n\n"
      "2. 设备血氧饱和度参考值如下：\n"
      "日常活动：一般 ≥95%\n"
      "夜间睡眠：一般 ≥90%\n"
      "需要关注：<90%\n"
      "注意：本设备测量范围为80%-100%。\n\n"
      "3. 佩戴状态、皮肤特征、环境等因素都可能影响测试准确性，甚至导致测量失败。"
      "睡眠过程中，设备可能因睡眠姿势不同而倾斜，导致伪低血氧值出现。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "本品非医疗器械，结果仅供参考，不作为诊断治疗依据。";
  lv_obj_t* note_body =
      create_steps_label(scroll_root_, info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(
      lv_timer_create(&BloodOxygenInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void BloodOxygenInfoPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

}  // namespace twsim::app
