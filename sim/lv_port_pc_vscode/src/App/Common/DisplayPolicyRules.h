#pragma once

#include <optional>

#include "App/Common/AppEvents.h"

namespace twsim::app {

class DisplayPolicyRules {
 public:
  static bool IsTimeInWindow(const TimeModel& time, const DailyTimeWindow& window);
  static bool IsRaiseToWakeAllowed(const DisplayPolicyModel& policy,
                                   const std::optional<TimeModel>& time);
  static bool IsRaiseToWakeAllowed(const DisplayPolicyModel& policy, const TimeModel& time);
  static bool IsScreenOffDisplayActive(const DisplayPolicyModel& policy,
                                       const std::optional<TimeModel>& time);
  static bool IsScreenOffDisplayActive(const DisplayPolicyModel& policy, const TimeModel& time);
  static bool ShouldSuppressAutoScreenOff(const DisplayPolicyModel& policy);
  static bool HasRaiseToWakeAndScreenOffDisplayConflict(RaiseToWakeMode raise_mode,
                                                        ScreenOffDisplayMode screen_off_mode);
};

}  // namespace twsim::app
