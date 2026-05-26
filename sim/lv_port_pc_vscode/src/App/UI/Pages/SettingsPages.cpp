#include "App/UI/Pages/SettingsPages.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <string>
#include <tuple>

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextSettings = "\xE8\xAE\xBE\xE7\xBD\xAE";
constexpr const char* kTextDisplay = "\xE6\x98\xBE\xE7\xA4\xBA\xE4\xB8\x8E\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextSound = "\xE5\xA3\xB0\xE9\x9F\xB3\xE4\xB8\x8E\xE6\x8C\xAF\xE5\x8A\xA8";
constexpr const char* kTextDnd = "\xE5\x8B\xBF\xE6\x89\xB0\xE6\xA8\xA1\xE5\xBC\x8F";
constexpr const char* kTextNotifications = "\xE6\xB6\x88\xE6\x81\xAF\xE9\x80\x9A\xE7\x9F\xA5";
constexpr const char* kTextAppLayout = "\xE5\xBA\x94\xE7\x94\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextSystemActions = "\xE7\xB3\xBB\xE7\xBB\x9F\xE6\x93\x8D\xE4\xBD\x9C";
constexpr const char* kTextAbout = "\xE5\x85\xB3\xE4\xBA\x8E";
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
constexpr const char* kTextComingSoon =
    "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE4\xBF\x9D\xE7\x95\x99\xE9\xA1\xB5\xE9\x9D\xA2\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x8C"
    "\xE5\x90\x8E\xE7\xBB\xAD\xE6\x8C\x89\xE5\xB0\x8F\xE9\x97\xAD\xE7\x8E\xAF\xE7\xBB\xA7\xE7\xBB\xAD\xE8\xA1\xA5\xE9\xBD\x90\xE3\x80\x82";
constexpr const char* kTextPlaceholderTag = "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x8D\xA0\xE4\xBD\x8D";

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
constexpr const char* kTextSmartOn = "\xE6\x99\xBA\xE8\x83\xBD\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextScheduledOn =
    "\xE5\xAE\x9A\xE6\x97\xB6\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextDefault =
    "\xE9\xBB\x98\xE8\xAE\xA4";
constexpr const char* kTextAnalogHands =
    "\xE6\x97\xB6\xE5\x88\x86\xE6\x8C\x87\xE9\x92\x88";
constexpr const char* kTextInfoDigits =
    "\xE6\x97\xA5\xE6\x9C\x9F\xE6\x95\xB0\xE5\xAD\x97";
constexpr const char* kTextStart = "\xE5\xBC\x80\xE5\xA7\x8B";
constexpr const char* kTextEnd = "\xE7\xBB\x93\xE6\x9D\x9F";
constexpr const char* kTextStartTime =
    "\xE5\xBC\x80\xE5\xA7\x8B\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextEndTime =
    "\xE7\xBB\x93\xE6\x9D\x9F\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextConfirm = LV_SYMBOL_OK;
constexpr const char* kTextCancel = LV_SYMBOL_CLOSE;
constexpr const char* kTextPlaceholderBody =
    "\xE6\x98\xBE\xE7\xA4\xBA\xE8\xAF\xA6\xE7\xBB\x86\xE7\xAD\x96\xE7\x95\xA5\xE4\xBC\x9A\xE5\x9C\xA8"
    "\xE5\x90\x8E\xE7\xBB\xAD\xE5\xB0\x8F\xE8\xBD\xAE\xE9\x80\x90\xE6\xAD\xA5\xE8\xA1\xA5\xE9\xBD\x90\xE3\x80\x82";
constexpr const char* kTextAuto = "\xE8\x87\xAA\xE5\x8A\xA8";
constexpr const char* kTextOn = "\xE5\xBC\x80";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextAllDayOn =
    "\xE5\x85\xA8\xE5\xA4\xA9\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextBrightnessSaveHint =
    "\xE7\xA6\xBB\xE5\xBC\x80\xE9\xA1\xB5\xE9\x9D\xA2\xE6\x97\xB6\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBF\x9D\xE5\xAD\x98";
constexpr const char* kTextKeepScreenOnConfirmBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE5\x90\x8E\xEF\xBC\x8C\xE4\xBB\x85\xE6\x9C\xAC\xE6\xAC\xA1\xE4\xBA\xAE\xE5\xB1\x8F"
    "\xE6\x97\xB6\xE9\x97\xB4\xE6\x8C\x81\xE7\xBB\xAD%s\xEF\xBC\x8C\xE4\xBA\xAE\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4"
    "\xE8\xBF\x87\xE9\x95\xBF\xEF\xBC\x8C\xE4\xBC\x9A\xE5\x87\x8F\xE5\xB0\x91\xE7\xBB\xAD\xE8\x88\xAA\xE6\x97\xB6"
    "\xE9\x97\xB4\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE4\xBD\xBF\xE7\x94\xA8\xEF\xBC\x9F";
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
constexpr const char* kTextMultiColumnLayout =
    "\xE5\xA4\x9A\xE5\x88\x97\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextListLayout =
    "\xE5\x88\x97\xE8\xA1\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextCategorizedLayout =
    "\xE5\x88\x86\xE7\xB1\xBB\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextAppLayoutNoteTitle =
    "\xE5\xBA\x94\xE7\x94\xA8\xE5\x85\xA5\xE5\x8F\xA3";
constexpr const char* kTextAppLayoutNoteBody =
    "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE6\x94\xB6\xE5\x8F\xA3\xE5\xBA\x94\xE7\x94\xA8"
    "\xE5\xB8\x83\xE5\xB1\x80\xE8\xAE\xBE\xE7\xBD\xAE\xE4\xB8\x8E\xE4\xBF\x9D\xE5\xAD\x98"
    "\xE5\x9B\x9E\xE6\x98\xBE\xEF\xBC\x8CLauncher \xE5\xAE\x9E\xE9\x99\x85\xE6\xB8\xB2\xE6\x9F\x93"
    "\xE5\x88\x87\xE6\x8D\xA2\xE7\x95\x99\xE5\x88\xB0\xE5\x90\x8E\xE7\xBB\xAD\xE5\xB0\x8F\xE8\xBD\xAE\xE3\x80\x82";

constexpr lv_coord_t kHeaderTop = 13;
constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kListHorizontalInset = 14;
constexpr lv_coord_t kListBottomInset = 8;
constexpr lv_coord_t kTileHeight = 58;
constexpr lv_coord_t kTileRadius = 18;
constexpr lv_coord_t kTileGap = 8;
constexpr lv_coord_t kTileHorizontalPadding = 16;
constexpr lv_coord_t kHeaderBackSize = 40;

DisplayPolicyModel current_display_policy(DataCenter& data_center) {
  return data_center.display_policy().value_or(DisplayPolicyModel {});
}

const char* timeout_text(std::uint32_t timeout_ms) {
  switch (timeout_ms) {
    case 5000U:
      return "5\xE7\xA7\x92";
    case 10000U:
      return "10\xE7\xA7\x92";
    case 20000U:
      return "20\xE7\xA7\x92";
    case 30000U:
      return "30\xE7\xA7\x92";
    case 60000U:
      return "60\xE7\xA7\x92";
    default:
      return "5\xE7\xA7\x92";
  }
}

std::string brightness_detail_text(const DisplayPolicyModel& policy) {
  if (policy.brightness_mode == BrightnessMode::Auto) {
    return kTextAuto;
  }
  return std::to_string(static_cast<int>(policy.manual_brightness_level)) + "%";
}

const char* keep_screen_on_text(std::uint32_t duration_ms) {
  switch (duration_ms) {
    case 0U:
      return kTextOff;
    case 300000U:
      return "5\xE5\x88\x86\xE9\x92\x9F";
    case 600000U:
      return "10\xE5\x88\x86\xE9\x92\x9F";
    case 900000U:
      return "15\xE5\x88\x86\xE9\x92\x9F";
    case 1200000U:
      return "20\xE5\x88\x86\xE9\x92\x9F";
    default:
      return kTextOff;
  }
}

const char* raise_to_wake_text(const DisplayPolicyModel& policy) {
  switch (policy.raise_to_wake_mode) {
    case RaiseToWakeMode::Off:
      return kTextOff;
    case RaiseToWakeMode::Scheduled:
      return kTextScheduledOn;
    case RaiseToWakeMode::AllDay:
    default:
      return kTextAllDayOn;
  }
}

std::string format_hhmm(std::uint8_t hour, std::uint8_t minute) {
  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%02u:%02u", static_cast<unsigned>(hour), static_cast<unsigned>(minute));
  return buffer;
}

std::string two_digit_options(int max_value) {
  std::string options;
  options.reserve(static_cast<std::size_t>(max_value + 1) * 3);
  char buffer[4] = {};
  for (int value = 0; value <= max_value; ++value) {
    std::snprintf(buffer, sizeof(buffer), "%02d", value);
    if (!options.empty()) {
      options.push_back('\n');
    }
    options.append(buffer);
  }
  return options;
}

TileVisual visual_for_page(PageId page_id) {
  switch (page_id) {
    case PageId::SettingDisplay:
      return {LV_SYMBOL_TINT, 0xF0B323};
    case PageId::SettingSound:
      return {LV_SYMBOL_VOLUME_MAX, 0x22D67C};
    case PageId::SettingDoNotDisturb:
      return {LV_SYMBOL_MUTE, 0x3B82F6};
    case PageId::SettingNotifications:
      return {LV_SYMBOL_BELL, 0x1DA1FF};
    case PageId::SettingAppLayout:
      return {LV_SYMBOL_LIST, 0x3B82F6};
    case PageId::SettingSystemActions:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
    case PageId::SettingAbout:
      return {"i", 0x50A8FF};
    case PageId::SettingBattery:
      return {LV_SYMBOL_BATTERY_FULL, 0x14C38E};
    default:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
  }
}

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

SettingsPageBase::SettingsPageBase(DataCenter& data_center, PageId page_id, const char* title, bool show_back_button)
    : PageBase(data_center), page_id_(page_id), title_(title), show_back_button_(show_back_button) {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 const auto* model = std::get_if<TimeModel>(&event.payload);
                                 if (model != nullptr) {
                                   apply_time(*model);
                                 }
                               }));
}

PageId SettingsPageBase::id() const {
  return page_id_;
}

const char* SettingsPageBase::name() const {
  return page_name(page_id_);
}

lv_obj_t* SettingsPageBase::create_page_root() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root != nullptr) {
    style_page_root(root);
    lv_obj_set_size(root, display_width(), display_height());
  }
  return root;
}

void SettingsPageBase::build_header(lv_obj_t* root) {
  if (root == nullptr) {
    return;
  }

  if (show_back_button_ && create_back_button(root, &SettingsPageBase::back_event_cb, this) == nullptr) {
    return;
  }

  lv_obj_t* title = lv_label_create(root);
  time_label_ = lv_label_create(root);
  if (title == nullptr || time_label_ == nullptr) {
    return;
  }

  ui_prepare_label(title);
  lv_obj_set_style_text_font(title, cjk_font_20(), 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(title, title_);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, show_back_button_ ? 40 : 18, kHeaderTop);

  ui_prepare_label(time_label_);
  lv_obj_set_style_text_font(time_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xF7FBFF), 0);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, kHeaderTop);

  if (const auto& time = data_center_.time(); time) {
    apply_time(*time);
  } else {
    lv_label_set_text(time_label_, "--:--");
  }
}

void SettingsPageBase::apply_time(const TimeModel& model) {
  set_time_label_text(time_label_, model);
}

const char* SettingsPageBase::title_text() const {
  return title_;
}

void SettingsPageBase::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsPageBase*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

SettingsHomePage::SettingsHomePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingsHome, kTextSettings, false),
      entries_ {{
          {LV_SYMBOL_TINT, 0xF0B323, kTextDisplay, PageId::SettingDisplay},
          {LV_SYMBOL_VOLUME_MAX, 0x22D67C, kTextSound, PageId::SettingSound},
          {LV_SYMBOL_BATTERY_FULL, 0x14C38E, kTextBatterySettings, PageId::SettingBattery},
          {LV_SYMBOL_MUTE, 0x3B82F6, kTextDnd, PageId::SettingDoNotDisturb},
          {LV_SYMBOL_BELL, 0x1DA1FF, kTextNotifications, PageId::SettingNotifications},
          {LV_SYMBOL_LIST, 0x3B82F6, kTextAppLayout, PageId::SettingAppLayout},
          {LV_SYMBOL_SETTINGS, 0x2AA8FF, kTextSystemActions, PageId::SettingSystemActions},
          {"i", 0x50A8FF, kTextAbout, PageId::SettingAbout},
      }} {}

lv_obj_t* SettingsHomePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < entries_.size(); ++index) {
    const auto& entry = entries_[index];
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_add_event_cb(button, &SettingsHomePage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    if (create_home_tile_icon(button, entry.icon_text, entry.icon_bg) == nullptr) {
      return nullptr;
    }

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, entry.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);
  }

  return root;
}

void SettingsHomePage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsHomePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* button = lv_event_get_current_target_obj(event);
  if (button == nullptr || !click_guard_allows(button)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(button)));
  if (index >= self->entries_.size()) {
    return;
  }

  self->request_navigation({NavigationAction::Push, self->entries_[index].target});
}

SettingsPlaceholderPage::SettingsPlaceholderPage(DataCenter& data_center,
                                                 PageId page_id,
                                                 const char* title,
                                                 const char* detail)
    : SettingsPageBase(data_center, page_id, title, true), detail_(detail) {}

lv_obj_t* SettingsPlaceholderPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  const TileVisual visual = visual_for_page(id());
  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* card = create_card_shell(scroll, false);
  if (card == nullptr) {
    return nullptr;
  }
  if (create_home_tile_icon(card, visual.icon_text, visual.icon_bg) == nullptr) {
    return nullptr;
  }

  lv_obj_t* label = lv_label_create(card);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(label, title_text());
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);

  if (create_note_card(scroll, kTextPlaceholderTag, detail_) == nullptr) {
    return nullptr;
  }
  if (create_note_card(scroll, kTextSettings, kTextComingSoon) == nullptr) {
    return nullptr;
  }

  return root;
}

AppLayoutSettingsPage::AppLayoutSettingsPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingAppLayout, kTextAppLayout, true),
      options_ {{
          {LauncherLayoutMode::MultiColumn, kTextMultiColumnLayout, nullptr, nullptr},
          {LauncherLayoutMode::List, kTextListLayout, nullptr, nullptr},
          {LauncherLayoutMode::Categorized, kTextCategorizedLayout, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void AppLayoutSettingsPage::on_will_appear() {
  refresh_selection();
}

lv_obj_t* AppLayoutSettingsPage::build() {
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
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return nullptr;
    }
    option.button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &AppLayoutSettingsPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, option.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(button);
    if (option.check_label == nullptr) {
      return nullptr;
    }
  }

  if (create_note_card(scroll, kTextAppLayoutNoteTitle, kTextAppLayoutNoteBody) == nullptr) {
    return nullptr;
  }

  refresh_selection();
  return root;
}

void AppLayoutSettingsPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<AppLayoutSettingsPage*>(lv_event_get_user_data(event));
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

  self->data_center_.set_launcher_layout_mode(self->options_[index].mode);
}

void AppLayoutSettingsPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    apply_selection_dot(option.check_label, policy.launcher_layout_mode == option.mode);
  }
}

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

const char* launcher_layout_text(LauncherLayoutMode mode) {
  switch (mode) {
    case LauncherLayoutMode::MultiColumn:
      return kTextMultiColumnLayout;
    case LauncherLayoutMode::List:
      return kTextListLayout;
    case LauncherLayoutMode::Categorized:
    default:
      return kTextCategorizedLayout;
  }
}

const char* screen_off_display_text(const DisplayPolicyModel& policy) {
  switch (policy.screen_off_display_mode) {
    case ScreenOffDisplayMode::Smart:
      return kTextSmartOn;
    case ScreenOffDisplayMode::Scheduled:
      return kTextScheduledOn;
    case ScreenOffDisplayMode::Off:
    default:
      return kTextOff;
  }
}

const char* screen_off_style_text(const DisplayPolicyModel& policy) {
  switch (policy.screen_off_style_id) {
    case ScreenOffStyleId::InfoDigits:
      return kTextInfoDigits;
    case ScreenOffStyleId::AnalogHands:
    default:
      return kTextAnalogHands;
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
  switch (index) {
    case 0:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayStartTime});
      break;
    case 1:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayScreenOffDisplayEndTime});
      break;
    default:
      break;
  }
}

void DisplayScreenOffDisplaySchedulePage::refresh_from_policy() {
  const auto policy = current_display_policy(data_center_);
  const auto& window = policy.screen_off_display_window;
  const std::string start_text = format_hhmm(window.start_hour, window.start_minute);
  const std::string end_text = format_hhmm(window.end_hour, window.end_minute);
  if (rows_[0].value_label != nullptr) {
    lv_label_set_text(rows_[0].value_label, start_text.c_str());
  }
  if (rows_[1].value_label != nullptr) {
    lv_label_set_text(rows_[1].value_label, end_text.c_str());
  }
}

DisplayScreenOffStylePage::DisplayScreenOffStylePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayScreenOffStyle, kTextScreenOffStyle, true) {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) {
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

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  preview_card_ = nullptr;

  lv_obj_t* style_tag = lv_label_create(preview_card_);
  if (style_tag == nullptr) {
    return nullptr;
  }
  ui_prepare_label(style_tag);
  lv_obj_set_style_text_font(style_tag, cjk_font_12(), 0);
  lv_obj_set_style_text_color(style_tag, lv_color_hex(0x7FB9FF), 0);
  lv_label_set_text(style_tag, kTextDefault);
  lv_obj_align(style_tag, LV_ALIGN_TOP_MID, 0, 10);

  lv_obj_t* frame = lv_obj_create(preview_card_);
  if (frame == nullptr) {
    return nullptr;
  }
  ui_prepare_box(frame);
  lv_obj_set_size(frame, 146, 156);
  lv_obj_align(frame, LV_ALIGN_TOP_MID, 0, 30);
  lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(frame, 2, 0);
  lv_obj_set_style_border_color(frame, lv_color_hex(0x7FCBFF), 0);
  lv_obj_set_style_radius(frame, 24, 0);

  lv_obj_t* day_label = lv_label_create(frame);
  if (day_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(day_label);
  lv_obj_set_style_text_font(day_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(day_label, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(day_label, "09\n28");
  lv_obj_set_style_text_line_space(day_label, -6, 0);
  lv_obj_set_style_text_align(day_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(day_label, LV_ALIGN_TOP_MID, 0, 18);

  lv_obj_t* date_label = lv_label_create(frame);
  if (date_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(date_label);
  lv_obj_set_style_text_font(date_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(date_label, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(date_label, "8月16日 周二");
  lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, -28);

  lv_obj_t* battery_label = lv_label_create(frame);
  if (battery_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(battery_label);
  lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(battery_label, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(battery_label, "80%");
  lv_obj_align(battery_label, LV_ALIGN_BOTTOM_MID, 0, -8);

  style_name_label_ = lv_label_create(preview_card_);
  if (style_name_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(style_name_label_);
  lv_obj_set_style_text_font(style_name_label_, cjk_font_18(), 0);
  lv_obj_set_style_text_color(style_name_label_, lv_color_hex(0xF4FAFF), 0);
  lv_label_set_text(style_name_label_, kTextDefault);
  lv_obj_align(style_name_label_, LV_ALIGN_BOTTOM_MID, 0, -16);

  if (create_note_card(scroll, kTextScreenOffStyle, kTextScreenOffStyleBody) == nullptr) {
    return nullptr;
  }

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

  self->data_center_.set_screen_off_style_id(ScreenOffStyleId::Default);
  self->refresh_selection();
}

void DisplayScreenOffStylePage::refresh_selection() {
  if (preview_card_ == nullptr) {
    return;
  }

  const auto policy = current_display_policy(data_center_);
  const bool selected = policy.screen_off_style_id == ScreenOffStyleId::Default;
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
