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

}  // namespace twsim::app
