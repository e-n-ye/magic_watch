#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Health/HealthIconPrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellImagePrimitives.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace {

using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_asset::health_heart_asset_path;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

const char* heart_rate_all_day_mode_text(HeartRateAllDayMonitoringMode mode) {
  switch (mode) {
    case HeartRateAllDayMonitoringMode::Off:
      return "关闭";
    case HeartRateAllDayMonitoringMode::Every1Min:
      return "1分钟一次";
    case HeartRateAllDayMonitoringMode::Every10Min:
      return "10分钟一次";
    case HeartRateAllDayMonitoringMode::Every30Min:
      return "30分钟一次";
    case HeartRateAllDayMonitoringMode::Smart:
    default:
      return "智能监测";
  }
}

const char* heart_rate_high_reminder_text(HeartRateHighReminderMode mode) {
  switch (mode) {
    case HeartRateHighReminderMode::Bpm100:
      return "100次/分";
    case HeartRateHighReminderMode::Bpm110:
      return "110次/分";
    case HeartRateHighReminderMode::Bpm120:
      return "120次/分";
    case HeartRateHighReminderMode::Bpm130:
      return "130次/分";
    case HeartRateHighReminderMode::Bpm140:
      return "140次/分";
    case HeartRateHighReminderMode::Bpm150:
      return "150次/分";
    case HeartRateHighReminderMode::Off:
    default:
      return "关闭";
  }
}

const char* heart_rate_low_reminder_text(HeartRateLowReminderMode mode) {
  switch (mode) {
    case HeartRateLowReminderMode::Bpm40:
      return "40次/分";
    case HeartRateLowReminderMode::Bpm45:
      return "45次/分";
    case HeartRateLowReminderMode::Bpm50:
      return "50次/分";
    case HeartRateLowReminderMode::Off:
    default:
      return "关闭";
  }
}

}  // namespace

HeartRateAppPage::HeartRateAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateAppPage::id() const {
  return PageId::AppHeartRate;
}

const char* HeartRateAppPage::name() const {
  return page_name(PageId::AppHeartRate);
}

void HeartRateAppPage::on_will_appear() {
  refresh_header_time();
  if (!measurement_completed_) {
    start_entry_measurement();
  }
}

void HeartRateAppPage::on_will_disappear() {
  stop_measurement_timer();
  stop_crown_release_timer();
}

lv_obj_t* HeartRateAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "心率", cjk_font_20(), 0xF8FAFC, 80);
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

  lv_obj_t* stage = create_steps_panel(scroll_root_, card_w, 260, 0x02070D);
  if (stage == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_bg_opa(stage, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(stage, 0, 0);

  measuring_stage_ = create_steps_panel(stage, card_w, 248, 0x02070D);
  result_stage_ = create_steps_panel(stage, card_w, 248, 0x02070D);
  if (measuring_stage_ == nullptr || result_stage_ == nullptr) {
    return nullptr;
  }
  for (lv_obj_t* panel : {measuring_stage_, result_stage_}) {
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 0);
  }

  lv_obj_t* heart_icon = create_contain_image(measuring_stage_, health_heart_asset_path(), 92, 92, LV_ALIGN_TOP_MID, 0, 42);
  lv_obj_t* heart_fallback = nullptr;
  if (heart_icon != nullptr && lv_obj_has_flag(heart_icon, LV_OBJ_FLAG_HIDDEN)) {
    heart_fallback = create_steps_label(measuring_stage_, "HR", &lv_font_montserrat_48, 0xFF3B74, 96);
  }
  lv_obj_t* measuring_title = create_steps_label(measuring_stage_, "正在测量", cjk_font_20(), 0xF8FAFC, card_w);
  lv_obj_t* measuring_hint = create_steps_label(measuring_stage_, "请保持静止", cjk_font_20(), 0xD8E9FF, card_w);
  if (heart_icon == nullptr || measuring_title == nullptr || measuring_hint == nullptr) {
    return nullptr;
  }
  if (heart_fallback != nullptr) {
    lv_obj_set_style_text_align(heart_fallback, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(heart_fallback, LV_ALIGN_TOP_MID, 0, 56);
  }
  lv_obj_set_style_text_align(measuring_title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(measuring_hint, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(measuring_title, LV_ALIGN_TOP_MID, 0, 146);
  lv_obj_align(measuring_hint, LV_ALIGN_TOP_MID, 0, 182);

  lv_obj_t* chart_card = create_steps_panel(result_stage_, card_w, 124, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }
  lv_obj_align(chart_card, LV_ALIGN_TOP_MID, 0, 0);

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 14;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 84;
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

  lv_obj_t* pulse_bar = lv_obj_create(chart_card);
  if (pulse_bar == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pulse_bar);
  lv_obj_set_size(pulse_bar, 8, 72);
  lv_obj_set_style_bg_color(pulse_bar, lv_color_hex(0xFF4F83), 0);
  lv_obj_set_style_bg_opa(pulse_bar, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(pulse_bar, 4, 0);
  lv_obj_align(pulse_bar, LV_ALIGN_TOP_RIGHT, -55, 16);

  lv_obj_t* top_value = create_steps_label(chart_card, "76", &lv_font_montserrat_18, 0xD8E9FF, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "75", &lv_font_montserrat_18, 0xD8E9FF, 28);
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

  lv_obj_t* wear_label = create_steps_label(result_stage_, "未佩戴", cjk_font_20(), 0xF8FAFC, card_w - 20);
  lv_obj_t* bpm_value = create_steps_label(result_stage_, "76", &lv_font_montserrat_48, 0xEAF7FF, 116);
  lv_obj_t* heart_small = create_contain_image(result_stage_, health_heart_asset_path(), 24, 24, LV_ALIGN_TOP_LEFT, 124, 190);
  lv_obj_t* unit_label = create_steps_label(result_stage_, "次/分", cjk_font_20(), 0xEAF7FF, 64);
  lv_obj_t* updated_label =
      create_steps_label(result_stage_, "76次/分(1分钟前)", cjk_font_20(), 0xD8E9FF, card_w - 20);
  if (wear_label == nullptr || bpm_value == nullptr || heart_small == nullptr || unit_label == nullptr ||
      updated_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(wear_label, LV_ALIGN_TOP_LEFT, 18, 140);
  lv_obj_align(bpm_value, LV_ALIGN_TOP_LEFT, 16, 166);
  lv_obj_align(unit_label, LV_ALIGN_TOP_LEFT, 150, 202);
  lv_obj_align(updated_label, LV_ALIGN_TOP_LEFT, 18, 230);

  for (const auto& entry : {std::tuple {EntryKind::Resting30Days, "近30天静息", "|||", true},
                            std::tuple {EntryKind::Settings, "心率设置", LV_SYMBOL_SETTINGS, true},
                            std::tuple {EntryKind::Info, "心率说明", "i", true}}) {
    const auto [kind, title, symbol, enabled] = entry;
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 82, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             0x5B1740,
                                             0xFF5AA0,
                                             nullptr,
                                             symbol,
                                             kind == EntryKind::Settings ? &lv_font_montserrat_18 : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, title, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    if (enabled) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(kind)));
    }
  }

  wear_prompt_overlay_ = lv_obj_create(root);
  if (wear_prompt_overlay_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(wear_prompt_overlay_);
  lv_obj_set_size(wear_prompt_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(wear_prompt_overlay_);
  lv_obj_set_style_bg_color(wear_prompt_overlay_, lv_color_hex(0x02070D), 0);
  lv_obj_set_style_bg_opa(wear_prompt_overlay_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(wear_prompt_overlay_, 0, 0);
  lv_obj_set_style_radius(wear_prompt_overlay_, 0, 0);
  lv_obj_add_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* wear_distance = create_steps_label(wear_prompt_overlay_, "1.5cm-2cm", &lv_font_montserrat_28, 0xEAF7FF, 190);
  lv_obj_t* wrist = lv_obj_create(wear_prompt_overlay_);
  lv_obj_t* watch = lv_obj_create(wear_prompt_overlay_);
  lv_obj_t* message = create_steps_label(wear_prompt_overlay_, "请正确佩戴手表后重试", cjk_font_20(), 0xF8FAFC, 210);
  lv_obj_t* info_icon = create_sleep_round_icon(wear_prompt_overlay_, 36, 0x0E4C78, 0xC9F0FF, nullptr, "i", cjk_font_20());
  lv_obj_t* ok_button = lv_button_create(wear_prompt_overlay_);
  lv_obj_t* ok_label = ok_button == nullptr ? nullptr : create_steps_label(ok_button, "知道了", cjk_font_20(), 0xF8FAFC, 130);
  if (wear_distance == nullptr || wrist == nullptr || watch == nullptr || message == nullptr || info_icon == nullptr ||
      ok_button == nullptr || ok_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(wear_distance, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(wear_distance, LV_ALIGN_TOP_MID, 0, 16);

  ui_prepare_box(wrist);
  lv_obj_set_size(wrist, 190, 4);
  lv_obj_set_style_bg_color(wrist, lv_color_hex(0xD8E9FF), 0);
  lv_obj_set_style_bg_opa(wrist, LV_OPA_80, 0);
  lv_obj_set_style_radius(wrist, 2, 0);
  lv_obj_align(wrist, LV_ALIGN_TOP_MID, 0, 82);

  ui_prepare_box(watch);
  lv_obj_set_size(watch, 38, 44);
  lv_obj_set_style_bg_color(watch, lv_color_hex(0x93CFF8), 0);
  lv_obj_set_style_bg_opa(watch, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(watch, 3, 0);
  lv_obj_set_style_border_color(watch, lv_color_hex(0xDFF7FF), 0);
  lv_obj_set_style_radius(watch, 10, 0);
  lv_obj_align(watch, LV_ALIGN_TOP_MID, -54, 60);

  lv_obj_set_style_text_align(message, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(message, LV_ALIGN_TOP_MID, 0, 140);
  lv_obj_align(info_icon, LV_ALIGN_TOP_MID, 0, 182);

  attach_click_guard(ok_button);
  ui_prepare_box(ok_button);
  lv_obj_set_size(ok_button, 164, 48);
  lv_obj_align(ok_button, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_set_style_bg_color(ok_button, lv_color_hex(0x17314D), 0);
  lv_obj_set_style_bg_opa(ok_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(ok_button, 0, 0);
  lv_obj_set_style_radius(ok_button, 18, 0);
  lv_obj_add_event_cb(ok_button, wear_prompt_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_center(ok_label);

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto kind = static_cast<EntryKind>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (kind == EntryKind::Resting30Days) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateResting30Days});
  } else if (kind == EntryKind::Settings) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateSettings});
  } else if (kind == EntryKind::Info) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateInfo});
  }
}

void HeartRateAppPage::wear_prompt_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->hide_wear_prompt();
}

void HeartRateAppPage::measurement_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->measurement_timer_ = nullptr;
  self->complete_entry_measurement();
}

void HeartRateAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateAppPage::bind_input() {
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

void HeartRateAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateAppPage::start_entry_measurement() {
  show_measurement_stage();
  stop_measurement_timer();
  measurement_timer_ = lv_timer_create(&HeartRateAppPage::measurement_timer_cb, 1200U, this);
  if (measurement_timer_ != nullptr) {
    lv_timer_set_repeat_count(measurement_timer_, 1);
  }
}

void HeartRateAppPage::complete_entry_measurement() {
  measurement_completed_ = true;
  show_result_stage();
  if (!wear_prompt_shown_) {
    show_wear_prompt();
    wear_prompt_shown_ = true;
  }
}

void HeartRateAppPage::show_measurement_stage() {
  if (measuring_stage_ != nullptr) {
    lv_obj_clear_flag(measuring_stage_, LV_OBJ_FLAG_HIDDEN);
  }
  if (result_stage_ != nullptr) {
    lv_obj_add_flag(result_stage_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::show_result_stage() {
  if (measuring_stage_ != nullptr) {
    lv_obj_add_flag(measuring_stage_, LV_OBJ_FLAG_HIDDEN);
  }
  if (result_stage_ != nullptr) {
    lv_obj_clear_flag(result_stage_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::show_wear_prompt() {
  if (wear_prompt_overlay_ == nullptr) {
    return;
  }
  lv_obj_clear_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(wear_prompt_overlay_);
}

void HeartRateAppPage::hide_wear_prompt() {
  if (wear_prompt_overlay_ != nullptr) {
    lv_obj_add_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::stop_measurement_timer() {
  if (measurement_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(measurement_timer_);
  measurement_timer_ = nullptr;
}

void HeartRateAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateInfoPage::HeartRateInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateInfoPage::id() const {
  return PageId::AppHeartRateInfo;
}

const char* HeartRateInfoPage::name() const {
  return page_name(PageId::AppHeartRateInfo);
}

void HeartRateInfoPage::on_will_appear() {
  refresh_header_time();
}

void HeartRateInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateInfoPage::build() {
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
  lv_obj_t* title_label = create_steps_label(root, "心率说明", cjk_font_20(), 0xF8FAFC, 116);
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
      "心率\n\n"
      "心率是心脏每分钟跳动的次数（bpm），它是心血管健康的重要指标。"
      "成年人安静状态下的心率（静息心率）通常为55-80次/分，可能因年龄、性别、健身水平或其他生理因素产生个体差异。\n\n"
      "注意：本设备测量范围为30-240次/分。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "3. 受佩戴状态、皮肤特征、环境等因素影响，可能影响测量的准确性，甚至无法输出结果。\n\n"
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

void HeartRateInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateInfoPage::bind_input() {
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

void HeartRateInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateInfoPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateSettingsPage::HeartRateSettingsPage(DataCenter& data_center) : PageBase(data_center) {
  rows_[0].kind = RowKind::AllDayMonitoring;
  rows_[0].title = "全天心率监测";
  rows_[1].kind = RowKind::HeartHealthMonitoring;
  rows_[1].title = "心脏健康监测";
  rows_[2].kind = RowKind::HighReminder;
  rows_[2].title = "高心率提醒";
  rows_[3].kind = RowKind::LowReminder;
  rows_[3].title = "低心率提醒";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_rows();
                                 }
                               }));
}

PageId HeartRateSettingsPage::id() const {
  return PageId::AppHeartRateSettings;
}

const char* HeartRateSettingsPage::name() const {
  return page_name(PageId::AppHeartRateSettings);
}

void HeartRateSettingsPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_rows();
}

void HeartRateSettingsPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateSettingsPage::build() {
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
  lv_obj_t* title_label = create_steps_label(root, "心率设置", cjk_font_20(), 0xF8FAFC, 116);
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

  for (std::size_t i = 0; i < rows_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
    lv_obj_t* title = row == nullptr
                          ? nullptr
                          : create_steps_label(row, rows_[i].title, cjk_font_20(), 0xF8FAFC, card_w - 32, LV_LABEL_LONG_WRAP);
    lv_obj_t* status = row == nullptr ? nullptr : create_steps_label(row, "关闭", cjk_font_16(), 0xAFC4DA, card_w - 32);
    if (row == nullptr || title == nullptr || status == nullptr) {
      return nullptr;
    }
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 18, 14);
    lv_obj_align(status, LV_ALIGN_TOP_LEFT, 18, 54);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, row_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(rows_[i].kind)));
    rows_[i].row = row;
    rows_[i].status_label = status;
  }

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateSettingsPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateSettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto kind = static_cast<RowKind>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  switch (kind) {
    case RowKind::AllDayMonitoring:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateAllDayMonitoring});
      break;
    case RowKind::HeartHealthMonitoring:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateHeartHealthMonitoring});
      break;
    case RowKind::HighReminder:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateHighReminder});
      break;
    case RowKind::LowReminder:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateLowReminder});
      break;
  }
}

void HeartRateSettingsPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateSettingsPage::bind_input() {
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

void HeartRateSettingsPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateSettingsPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateSettingsPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateSettingsPage::refresh_rows() {
  for (const auto& row : rows_) {
    if (row.status_label != nullptr) {
      lv_label_set_text(row.status_label, row_status_text(row.kind));
    }
  }
}

const char* HeartRateSettingsPage::row_status_text(RowKind kind) const {
  switch (kind) {
    case RowKind::AllDayMonitoring:
      return heart_rate_all_day_mode_text(current_settings_.all_day_heart_rate_monitoring_mode);
    case RowKind::HeartHealthMonitoring:
      return current_settings_.heart_health_monitoring_enabled ? "开启" : "关闭";
    case RowKind::HighReminder:
      return heart_rate_high_reminder_text(current_settings_.high_heart_rate_reminder_mode);
    case RowKind::LowReminder:
      return heart_rate_low_reminder_text(current_settings_.low_heart_rate_reminder_mode);
  }
  return "关闭";
}

void HeartRateSettingsPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateSettingsPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateSettingsPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

}  // namespace twsim::app
