#include "App/UI/Pages/Settings/SettingsDisplayHelpers.h"

#include <cstddef>
#include <cstdio>

#include "App/Common/DataCenter.h"

namespace twsim::app {

namespace {

constexpr const char* kTextAuto = "\xE8\x87\xAA\xE5\x8A\xA8";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextSmartOn = "\xE6\x99\xBA\xE8\x83\xBD\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextScheduledOn =
    "\xE5\xAE\x9A\xE6\x97\xB6\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextAllDayOn =
    "\xE5\x85\xA8\xE5\xA4\xA9\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextAnalogHands =
    "\xE6\x97\xB6\xE5\x88\x86\xE6\x8C\x87\xE9\x92\x88";
constexpr const char* kTextInfoDigits =
    "\xE6\x97\xA5\xE6\x9C\x9F\xE6\x95\xB0\xE5\xAD\x97";

}  // namespace

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
