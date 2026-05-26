#pragma once

#include <cstdint>
#include <optional>

#include "App/Common/AppEvents.h"

namespace twsim::app {

enum class PowerActionType : std::uint8_t {
  None,
  WakeScreen,
  TurnScreenOff,
  EnterLowPowerMode,
  ExitLowPowerMode,
  ShowPowerMenu,
};

enum class PowerActionReason : std::uint8_t {
  UserInput,
  Timeout,
  BatteryLow,
  UserRequest,
  NotificationWake,
};

struct PowerAction {
  PowerActionType type {PowerActionType::None};
  PowerActionReason reason {PowerActionReason::UserInput};
  std::int8_t target_brightness_percent {-1};
  std::int32_t next_wakeup_ms {-1};
  bool pause_background_polling {false};
  bool restore_previous_page {false};
  bool reset_auto_screen_off_timer {false};
  bool cancel_auto_screen_off_timer {false};
  bool sync_keep_screen_on_policy {false};
  bool cancel_keep_screen_on_timer {false};
  bool clear_keep_screen_on_policy {false};
  bool start_raise_to_wake_session {false};

  bool valid() const;
};

class PowerController {
 public:
  enum class PowerState : std::uint8_t {
    Booting,
    Running,
    ScreenOff,
    PoweredOff,
  };

  PowerState power_state() const;
  bool is_running() const;
  bool is_screen_off() const;
  bool is_powered_off() const;

  PowerAction mark_running(PowerActionReason reason = PowerActionReason::UserRequest);
  PowerAction mark_screen_off(PowerActionReason reason = PowerActionReason::UserRequest);
  PowerAction mark_powered_off(PowerActionReason reason = PowerActionReason::UserRequest);

  PowerAction on_input_activity() const;
  PowerAction on_display_policy_changed() const;
  PowerAction on_auto_screen_off_timeout() const;
  PowerAction on_keep_screen_on_timeout() const;
  PowerAction on_debug_toggle_screen_off() const;
  PowerAction on_raise_to_wake(bool allowed);
  PowerAction on_raise_dismiss(bool keep_screen_on_active);
  PowerAction on_cover_sleep(bool allowed) const;
  PowerAction on_crown_press_wake(bool allowed) const;
  PowerAction on_touch_wake(bool allowed) const;
  PowerAction on_power_mode_changed(const PowerModeModel& model) const;
  PowerAction on_battery_changed(const BatteryModel& model, bool long_battery_mode_enabled) const;

  bool should_schedule_auto_screen_off(const std::optional<DisplayPolicyModel>& policy) const;
  std::uint32_t auto_screen_off_timeout_ms(const std::optional<DisplayPolicyModel>& policy) const;
  PowerAction sync_keep_screen_on_policy(const std::optional<DisplayPolicyModel>& policy,
                                         bool keep_screen_on_timer_exists) const;
  bool keep_screen_on_active(const std::optional<DisplayPolicyModel>& policy) const;
  std::uint32_t keep_screen_on_timer_duration_ms() const;
  void mark_keep_screen_on_timer_scheduled(std::uint32_t duration_ms);
  void clear_keep_screen_on_timer_cache();
  void clear_raise_to_wake_session();
  void mark_raise_to_wake_session();

 private:
  PowerAction state_action(PowerActionType type, PowerActionReason reason) const;

  PowerState power_state_ {PowerState::Booting};
  bool raise_to_wake_session_active_ {false};
  std::uint32_t keep_screen_on_timer_duration_ms_ {0};
};

}  // namespace twsim::app
