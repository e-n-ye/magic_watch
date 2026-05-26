#include "App/UI/Pages/SettingsPages.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextBatterySettings = "电池";
constexpr const char* kTextBatteryLifeMode = "长续航模式";
constexpr const char* kTextBatteryOptimization = "续航优化";
constexpr const char* kTextBatteryInfo = "说明";
constexpr const char* kTextBatteryAvailable = "可用时长：";
constexpr const char* kTextBatteryWaiting = "等待电池数据";
constexpr const char* kTextBatteryMissing = "未检测到电池";
constexpr const char* kTextBatteryCharging = "充电中";
constexpr const char* kTextBatteryExternal = "外部供电";
constexpr const char* kTextBatteryDischarging = "电池供电";
constexpr const char* kTextBatteryLifeEstimate = "续航预估";
constexpr const char* kTextBatteryLifeEstimateBody =
    "可用时长根据实验室数据预估，仅供参考，实际时间取决于使用习惯等共同影响。";
constexpr const char* kTextBatteryOptimizationBody =
    "仅统计严重影响续航功能，根据当前状态显示，后续将接入可操作的优化建议。";
constexpr const char* kTextBatteryLifeModeDuration = "可用时长：约15天14小时";
constexpr const char* kTextBatteryLifeModeBody =
    "开启后仅保留时间、计步、NFC功能。旋转表冠或充电可退出长续航模式。";
constexpr const char* kTextBatteryOptimizationStatus = "续航状态中等";
constexpr const char* kTextBatteryOptimizationIntro = "以下功能开启后会严重影响设备续航";
constexpr const char* kTextBatteryOptimizationInterfaceNote =
    "本轮先保留可交互接口，后续接入心率、血氧、睡眠等真实设置页。";
constexpr const char* kTextSleepBreathingQuality = "睡眠呼吸质量检测";
constexpr const char* kTextHeartHealthMonitoring = "心脏健康监测";
constexpr const char* kTextAllDayStressMonitoring = "全天压力监测";
constexpr const char* kTextHighPrecisionSleep = "睡眠高精度监测";
constexpr const char* kTextAllDayBloodOxygen = "全天血氧监测";
constexpr const char* kTextExtend57Hours = "关闭后，预计可延长57小时续航。";
constexpr const char* kTextExtend98Hours = "关闭后，预计可延长98小时续航。";
constexpr const char* kTextExtend14Hours = "关闭后，预计可延长14小时续航。";
constexpr const char* kTextExtend78Hours = "关闭后，预计可延长78小时续航。";
constexpr const char* kTextPlaceholderTag = "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x8D\xA0\xE4\xBD\x8D";

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kTileHorizontalPadding = 16;

std::string battery_duration_text(const BatteryModel& model) {
  if (!model.present) {
    return std::string(kTextBatteryAvailable) + kTextBatteryMissing;
  }

  const auto percent = std::clamp<int>(model.percent, 0, 100);
  const int hours = std::max(1, percent * 2 + 15);
  return std::string(kTextBatteryAvailable) + "约" + std::to_string(hours / 24) + "天" +
         std::to_string(hours % 24) + "小时";
}

std::string battery_status_text(const BatteryModel& model) {
  if (!model.present) {
    return kTextBatteryMissing;
  }

  std::string text;
  if (model.charging) {
    text = kTextBatteryCharging;
  } else if (model.external_power) {
    text = kTextBatteryExternal;
  } else {
    text = kTextBatteryDischarging;
  }
  if (model.millivolts != 0) {
    text += " · ";
    text += std::to_string(static_cast<int>(model.millivolts));
    text += "mV";
  }
  return text;
}

}  // namespace

BatteryStatusPage::BatteryStatusPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingBattery, kTextBatterySettings, true) {
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 const auto* model = std::get_if<BatteryModel>(&event.payload);
                                 if (model != nullptr) {
                                   apply_battery(*model);
                                 }
                               }));
}

void BatteryStatusPage::on_will_appear() {
  refresh_from_model();
}

lv_obj_t* BatteryStatusPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* summary = lv_obj_create(scroll);
  if (summary == nullptr) {
    return nullptr;
  }
  ui_prepare_box(summary);
  lv_obj_set_width(summary, LV_PCT(100));
  lv_obj_set_height(summary, 116);
  lv_obj_set_style_bg_opa(summary, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(summary, 0, 0);

  lv_obj_t* icon = lv_obj_create(summary);
  if (icon == nullptr) {
    return nullptr;
  }
  ui_prepare_box(icon);
  lv_obj_set_size(icon, 44, 44);
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 42, 6);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(0x12C7A0), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon, 0, 0);

  lv_obj_t* icon_label = lv_label_create(icon);
  if (icon_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(icon_label);
  lv_obj_set_style_text_font(icon_label, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(icon_label, lv_color_hex(0xE7FFF7), 0);
  lv_label_set_text(icon_label, LV_SYMBOL_CHARGE);
  lv_obj_center(icon_label);

  percent_label_ = lv_label_create(summary);
  if (percent_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(percent_label_);
  lv_obj_set_style_text_font(percent_label_, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(percent_label_, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(percent_label_, "--%");
  lv_obj_align(percent_label_, LV_ALIGN_TOP_LEFT, 96, -1);

  duration_label_ = lv_label_create(summary);
  status_label_ = lv_label_create(summary);
  if (duration_label_ == nullptr || status_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(duration_label_);
  lv_obj_set_style_text_font(duration_label_, cjk_font_18(), 0);
  lv_obj_set_style_text_color(duration_label_, lv_color_hex(0xDDF6FF), 0);
  lv_label_set_text(duration_label_, kTextBatteryWaiting);
  lv_obj_align(duration_label_, LV_ALIGN_TOP_LEFT, 18, 61);

  ui_prepare_label(status_label_);
  lv_obj_set_style_text_font(status_label_, cjk_font_12(), 0);
  lv_obj_set_style_text_color(status_label_, lv_color_hex(0x84B6D8), 0);
  lv_label_set_text(status_label_, "--");
  lv_obj_align(status_label_, LV_ALIGN_TOP_LEFT, 18, 88);

  lv_obj_t* life_mode_card = create_single_line_card(scroll, kTextBatteryLifeMode, true);
  if (life_mode_card == nullptr) {
    return nullptr;
  }
  lv_obj_set_user_data(life_mode_card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(RowAction::OpenLifeMode)));
  lv_obj_add_event_cb(life_mode_card, &BatteryStatusPage::row_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* optimization_card = create_single_line_card(scroll, kTextBatteryOptimization, true);
  if (optimization_card == nullptr) {
    return nullptr;
  }
  lv_obj_set_user_data(optimization_card,
                       reinterpret_cast<void*>(static_cast<std::uintptr_t>(RowAction::OpenOptimization)));
  lv_obj_add_event_cb(optimization_card, &BatteryStatusPage::row_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* info_button = lv_button_create(scroll);
  if (info_button == nullptr) {
    return nullptr;
  }
  attach_click_guard(info_button);
  ui_prepare_box(info_button);
  lv_obj_set_size(info_button, 52, 52);
  lv_obj_set_style_radius(info_button, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(info_button, lv_color_hex(0x17314C), 0);
  lv_obj_set_style_bg_opa(info_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(info_button, 0, 0);
  lv_obj_add_event_cb(info_button, &BatteryStatusPage::info_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* info_label = lv_label_create(info_button);
  if (info_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(info_label);
  lv_obj_set_style_text_font(info_label, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(info_label, lv_color_hex(0xDDF6FF), 0);
  lv_label_set_text(info_label, "i");
  lv_obj_center(info_label);

  refresh_from_model();
  return root;
}

void BatteryStatusPage::info_event_cb(lv_event_t* event) {
  auto* self = static_cast<BatteryStatusPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::SettingBatteryInfo});
}

void BatteryStatusPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<BatteryStatusPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto action = static_cast<RowAction>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  switch (action) {
    case RowAction::OpenLifeMode:
      self->request_navigation({NavigationAction::Push, PageId::SettingBatteryLifeMode});
      break;
    case RowAction::OpenOptimization:
      self->request_navigation({NavigationAction::Push, PageId::SettingBatteryOptimization});
      break;
  }
}

void BatteryStatusPage::refresh_from_model() {
  if (const auto& model = data_center_.battery(); model) {
    apply_battery(*model);
    return;
  }

  if (percent_label_ != nullptr) {
    lv_label_set_text(percent_label_, "--%");
  }
  if (duration_label_ != nullptr) {
    lv_label_set_text(duration_label_, kTextBatteryWaiting);
  }
  if (status_label_ != nullptr) {
    lv_label_set_text(status_label_, "--");
  }
}

void BatteryStatusPage::apply_battery(const BatteryModel& model) {
  if (percent_label_ != nullptr) {
    const auto percent = std::clamp<int>(model.percent, 0, 100);
    lv_label_set_text(percent_label_, (std::to_string(percent) + "%").c_str());
  }
  if (duration_label_ != nullptr) {
    lv_label_set_text(duration_label_, battery_duration_text(model).c_str());
  }
  if (status_label_ != nullptr) {
    lv_label_set_text(status_label_, battery_status_text(model).c_str());
  }
}

BatteryLifeModePage::BatteryLifeModePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingBatteryLifeMode, kTextBatteryLifeMode, true) {
  track(data_center_.subscribe(EventId::PowerModeChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<PowerModeModel>(&event.payload)) {
                                   apply_power_mode(*model);
                                 }
                               }));
}

void BatteryLifeModePage::on_will_appear() {
  if (const auto& model = data_center_.power_mode(); model) {
    apply_power_mode(*model);
  }
  refresh_switch();
}

lv_obj_t* BatteryLifeModePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* row = create_single_line_card(scroll, kTextBatteryLifeMode, true);
  if (row == nullptr) {
    return nullptr;
  }
  lv_obj_add_event_cb(row, &BatteryLifeModePage::switch_event_cb, LV_EVENT_CLICKED, this);
  switch_track_ = create_switch_track(row);
  if (switch_track_ == nullptr) {
    return nullptr;
  }

  if (create_plain_text(scroll, kTextBatteryLifeModeDuration, cjk_font_18(), 0xDDF6FF) == nullptr) {
    return nullptr;
  }
  if (create_plain_text(scroll, kTextBatteryLifeModeBody, cjk_font_18(), 0xDDF6FF) == nullptr) {
    return nullptr;
  }

  refresh_switch();
  return root;
}

void BatteryLifeModePage::switch_event_cb(lv_event_t* event) {
  auto* self = static_cast<BatteryLifeModePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  self->data_center_.set_long_battery_mode_enabled(!self->enabled_);
}

void BatteryLifeModePage::refresh_switch() {
  apply_switch_visual(switch_track_, enabled_);
}

void BatteryLifeModePage::apply_power_mode(const PowerModeModel& model) {
  enabled_ = model.long_battery_mode_enabled;
  refresh_switch();
}

BatteryOptimizationPage::BatteryOptimizationPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingBatteryOptimization, kTextBatteryOptimization, true),
      options_ {{
          {OptionKey::SleepBreathingQuality, kTextSleepBreathingQuality, kTextExtend57Hours, nullptr, nullptr},
          {OptionKey::HeartHealthMonitoring, kTextHeartHealthMonitoring, kTextExtend98Hours, nullptr, nullptr},
          {OptionKey::AllDayStressMonitoring, kTextAllDayStressMonitoring, kTextExtend14Hours, nullptr, nullptr},
          {OptionKey::HighPrecisionSleep, kTextHighPrecisionSleep, kTextExtend57Hours, nullptr, nullptr},
          {OptionKey::AllDayBloodOxygen, kTextAllDayBloodOxygen, kTextExtend78Hours, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_health_monitoring_settings(*model);
                                 }
                               }));
}

void BatteryOptimizationPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model) {
    apply_health_monitoring_settings(*model);
  }
  refresh_options();
}

lv_obj_t* BatteryOptimizationPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* summary = lv_obj_create(scroll);
  if (summary == nullptr) {
    return nullptr;
  }
  ui_prepare_box(summary);
  lv_obj_set_width(summary, LV_PCT(100));
  lv_obj_set_height(summary, 120);
  lv_obj_set_style_bg_opa(summary, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(summary, 0, 0);

  lv_obj_t* icon = lv_obj_create(summary);
  if (icon == nullptr) {
    return nullptr;
  }
  ui_prepare_box(icon);
  lv_obj_set_size(icon, 52, 52);
  lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 6);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(0x17314C), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon, 0, 0);

  lv_obj_t* icon_label = lv_label_create(icon);
  if (icon_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(icon_label);
  lv_obj_set_style_text_font(icon_label, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(icon_label, lv_color_hex(0xDDF6FF), 0);
  lv_label_set_text(icon_label, "!");
  lv_obj_center(icon_label);

  lv_obj_t* status = lv_label_create(summary);
  if (status == nullptr) {
    return nullptr;
  }
  ui_prepare_label(status);
  lv_obj_set_style_text_font(status, cjk_font_20(), 0);
  lv_obj_set_style_text_color(status, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(status, kTextBatteryOptimizationStatus);
  lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 68);

  lv_obj_t* intro = create_plain_text(scroll, kTextBatteryOptimizationIntro, cjk_font_16(), 0xDDF6FF);
  if (intro == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.button = create_card_shell(scroll, true);
    if (option.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_height(option.button, 68);
    lv_obj_set_user_data(option.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(option.button, &BatteryOptimizationPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* title = lv_label_create(option.button);
    if (title == nullptr) {
      return nullptr;
    }
    ui_prepare_label(title);
    lv_obj_set_width(title, 124);
    lv_obj_set_style_text_font(title, cjk_font_20(), 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
    lv_label_set_text(title, option.title);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.switch_track = create_switch_track(option.button);
    if (option.switch_track == nullptr) {
      return nullptr;
    }

    lv_obj_t* detail = create_plain_text(scroll, option.detail, cjk_font_16(), 0x84B6D8);
    if (detail == nullptr) {
      return nullptr;
    }
  }

  if (create_note_card(scroll, kTextPlaceholderTag, kTextBatteryOptimizationInterfaceNote) == nullptr) {
    return nullptr;
  }

  refresh_options();
  return root;
}

void BatteryOptimizationPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<BatteryOptimizationPage*>(lv_event_get_user_data(event));
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

  const auto key = self->options_[index].key;
  self->set_option_enabled(key, !self->option_enabled(key));
}

void BatteryOptimizationPage::apply_health_monitoring_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
  refresh_options();
}

bool BatteryOptimizationPage::option_enabled(OptionKey key) const {
  switch (key) {
    case OptionKey::SleepBreathingQuality:
      return current_settings_.sleep_breathing_quality_enabled;
    case OptionKey::HeartHealthMonitoring:
      return current_settings_.heart_health_monitoring_enabled;
    case OptionKey::AllDayStressMonitoring:
      return current_settings_.all_day_stress_monitoring_enabled;
    case OptionKey::HighPrecisionSleep:
      return current_settings_.high_precision_sleep_enabled;
    case OptionKey::AllDayBloodOxygen:
      return current_settings_.all_day_blood_oxygen_enabled;
  }
  return true;
}

void BatteryOptimizationPage::set_option_enabled(OptionKey key, bool enabled) {
  switch (key) {
    case OptionKey::SleepBreathingQuality:
      data_center_.set_sleep_breathing_quality_enabled(enabled);
      return;
    case OptionKey::HeartHealthMonitoring:
      data_center_.set_heart_health_monitoring_enabled(enabled);
      return;
    case OptionKey::AllDayStressMonitoring:
      data_center_.set_all_day_stress_monitoring_enabled(enabled);
      return;
    case OptionKey::HighPrecisionSleep:
      data_center_.set_high_precision_sleep_enabled(enabled);
      return;
    case OptionKey::AllDayBloodOxygen:
      data_center_.set_all_day_blood_oxygen_enabled(enabled);
      return;
  }
}

void BatteryOptimizationPage::refresh_options() {
  for (const auto& option : options_) {
    apply_switch_visual(option.switch_track, option_enabled(option.key));
  }
}

BatteryInfoPage::BatteryInfoPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingBatteryInfo, kTextBatteryInfo, true) {}

lv_obj_t* BatteryInfoPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  if (create_note_card(scroll, kTextBatteryLifeEstimate, kTextBatteryLifeEstimateBody) == nullptr) {
    return nullptr;
  }
  if (create_note_card(scroll, kTextBatteryOptimization, kTextBatteryOptimizationBody) == nullptr) {
    return nullptr;
  }

  return root;
}

}  // namespace twsim::app
