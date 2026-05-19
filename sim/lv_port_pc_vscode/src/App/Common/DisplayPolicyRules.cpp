#include "App/Common/DisplayPolicyRules.h"

namespace twsim::app {

bool DisplayPolicyRules::IsTimeInWindow(const TimeModel& time, const DailyTimeWindow& window) {
  if (!time.valid) {
    return false;
  }

  const int current_minutes = static_cast<int>(time.hour) * 60 + static_cast<int>(time.minute);
  const int start_minutes = static_cast<int>(window.start_hour) * 60 + static_cast<int>(window.start_minute);
  const int end_minutes = static_cast<int>(window.end_hour) * 60 + static_cast<int>(window.end_minute);

  if (start_minutes == end_minutes) {
    return true;
  }
  if (start_minutes < end_minutes) {
    return current_minutes >= start_minutes && current_minutes < end_minutes;
  }
  return current_minutes >= start_minutes || current_minutes < end_minutes;
}

bool DisplayPolicyRules::IsRaiseToWakeAllowed(const DisplayPolicyModel& policy,
                                              const std::optional<TimeModel>& time) {
  switch (policy.raise_to_wake_mode) {
    case RaiseToWakeMode::Off:
      return false;
    case RaiseToWakeMode::Scheduled:
      return time && IsTimeInWindow(*time, policy.raise_to_wake_window);
    case RaiseToWakeMode::AllDay:
    default:
      return true;
  }
}

bool DisplayPolicyRules::IsRaiseToWakeAllowed(const DisplayPolicyModel& policy, const TimeModel& time) {
  return IsRaiseToWakeAllowed(policy, std::optional<TimeModel> {time});
}

bool DisplayPolicyRules::IsScreenOffDisplayActive(const DisplayPolicyModel& policy,
                                                  const std::optional<TimeModel>& time) {
  switch (policy.screen_off_display_mode) {
    case ScreenOffDisplayMode::Smart:
      return true;
    case ScreenOffDisplayMode::Scheduled:
      return time && IsTimeInWindow(*time, policy.screen_off_display_window);
    case ScreenOffDisplayMode::Off:
    default:
      return false;
  }
}

bool DisplayPolicyRules::IsScreenOffDisplayActive(const DisplayPolicyModel& policy, const TimeModel& time) {
  return IsScreenOffDisplayActive(policy, std::optional<TimeModel> {time});
}

bool DisplayPolicyRules::ShouldSuppressAutoScreenOff(const DisplayPolicyModel& policy) {
  return !policy.auto_screen_off_enabled || policy.always_on_display_enabled ||
         policy.keep_screen_on_duration_ms > 0U;
}

bool DisplayPolicyRules::HasRaiseToWakeAndScreenOffDisplayConflict(
    RaiseToWakeMode raise_mode,
    ScreenOffDisplayMode screen_off_mode) {
  return raise_mode != RaiseToWakeMode::Off && screen_off_mode != ScreenOffDisplayMode::Off;
}

}  // namespace twsim::app
