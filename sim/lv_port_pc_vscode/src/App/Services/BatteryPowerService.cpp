#include "App/Services/BatteryPowerService.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace twsim::app {

namespace {

constexpr std::int16_t kLowBatteryThresholdPercent = 20;
constexpr const char* kTextBattery = "\xE7\x94\xB5\xE9\x87\x8F";
constexpr const char* kTextBatteryLow = "\xE7\x94\xB5\xE9\x87\x8F\xE4\xBD\x8E";
constexpr const char* kTextBatteryBodyPrefix =
    "\xE6\x89\x8B\xE8\xA1\xA8\xE7\x94\xB5\xE9\x87\x8F\xE8\xBF\x87\xE4\xBD\x8E\xEF\xBC\x8C"
    "\xE8\xAF\xB7\xE5\x8F\x8A\xE6\x97\xB6\xE5\x85\x85\xE7\x94\xB5\xE3\x80\x82";
constexpr const char* kTextJustNow = "\xE5\x88\x9A\xE5\x88\x9A";

std::int16_t clamp_percent(std::int16_t percent) {
  return static_cast<std::int16_t>(std::clamp<int>(percent, 0, 100));
}

}  // namespace

BatteryPowerService::BatteryPowerService(DataCenter& data_center) : data_center_(data_center) {}

void BatteryPowerService::handle_sample(const hal::BatterySample& sample) {
  const BatteryModel model = to_model(sample);
  data_center_.publish_battery(model);

  if (!model.present || model.percent > kLowBatteryThresholdPercent) {
    low_battery_notification_latched_ = false;
    return;
  }
  if (low_battery_notification_latched_) {
    return;
  }

  low_battery_notification_latched_ = true;
  data_center_.push_notification(make_low_battery_notification(model.percent));
}

void BatteryPowerService::inject_low_battery_notification(std::int16_t percent) {
  data_center_.push_notification(make_low_battery_notification(clamp_percent(percent)));
}

BatteryModel BatteryPowerService::to_model(const hal::BatterySample& sample) {
  return {
      sample.present,
      sample.charging,
      sample.external_power,
      clamp_percent(sample.percent),
      sample.millivolts,
  };
}

NotificationItem BatteryPowerService::make_low_battery_notification(std::int16_t percent) {
  NotificationItem item;
  item.id = "battery-" + std::to_string(clamp_percent(percent)) + "-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::BatteryLow;
  item.source_label = kTextBattery;
  item.title = kTextBatteryLow;
  char body_buffer[96] = {};
  std::snprintf(body_buffer,
                sizeof(body_buffer),
                "%s %d%%",
                kTextBatteryBodyPrefix,
                static_cast<int>(clamp_percent(percent)));
  item.body = body_buffer;
  item.time_text = kTextJustNow;
  item.badge_text = std::to_string(static_cast<int>(clamp_percent(percent))) + "%";
  return item;
}

}  // namespace twsim::app
