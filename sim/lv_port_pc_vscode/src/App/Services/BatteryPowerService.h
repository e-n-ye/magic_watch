#pragma once

#include <cstdint>

#include "App/Common/DataCenter.h"
#include "HAL/HAL.h"

namespace twsim::app {

class BatteryPowerService {
 public:
  explicit BatteryPowerService(DataCenter& data_center);

  void handle_sample(const hal::BatterySample& sample);
  void inject_low_battery_notification(std::int16_t percent);

 private:
  static BatteryModel to_model(const hal::BatterySample& sample);
  NotificationItem make_low_battery_notification(std::int16_t percent);

  DataCenter& data_center_;
  std::uint32_t next_notification_id_ {1};
  bool low_battery_notification_latched_ {false};
};

}  // namespace twsim::app
