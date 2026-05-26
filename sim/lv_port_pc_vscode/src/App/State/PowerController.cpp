#include "App/State/PowerController.h"

#include "App/Common/DisplayPolicyRules.h"

namespace twsim::app {

bool PowerAction::valid() const {
  if (reset_auto_screen_off_timer && cancel_auto_screen_off_timer) {
    return false;
  }
  if (sync_keep_screen_on_policy && (cancel_keep_screen_on_timer || clear_keep_screen_on_policy)) {
    return false;
  }
  return true;
}

PowerController::PowerState PowerController::power_state() const {
  return power_state_;
}

bool PowerController::is_running() const {
  return power_state_ == PowerState::Running;
}

bool PowerController::is_screen_off() const {
  return power_state_ == PowerState::ScreenOff;
}

bool PowerController::is_powered_off() const {
  return power_state_ == PowerState::PoweredOff;
}

PowerAction PowerController::mark_running(PowerActionReason reason) {
  power_state_ = PowerState::Running;
  PowerAction action;
  action.reason = reason;
  action.reset_auto_screen_off_timer = true;
  action.sync_keep_screen_on_policy = true;
  return action;
}

PowerAction PowerController::mark_screen_off(PowerActionReason reason) {
  power_state_ = PowerState::ScreenOff;
  raise_to_wake_session_active_ = false;
  PowerAction action;
  action.reason = reason;
  action.cancel_auto_screen_off_timer = true;
  action.clear_keep_screen_on_policy = true;
  return action;
}

PowerAction PowerController::mark_powered_off(PowerActionReason reason) {
  power_state_ = PowerState::PoweredOff;
  raise_to_wake_session_active_ = false;
  PowerAction action;
  action.reason = reason;
  action.cancel_auto_screen_off_timer = true;
  action.clear_keep_screen_on_policy = true;
  return action;
}

PowerAction PowerController::on_input_activity() const {
  if (!is_running()) {
    return {};
  }
  PowerAction action;
  action.reset_auto_screen_off_timer = true;
  return action;
}

PowerAction PowerController::on_display_policy_changed() const {
  if (!is_running()) {
    return {};
  }
  PowerAction action;
  action.reset_auto_screen_off_timer = true;
  action.sync_keep_screen_on_policy = true;
  return action;
}

PowerAction PowerController::on_auto_screen_off_timeout() const {
  if (!is_running()) {
    return {};
  }
  return state_action(PowerActionType::TurnScreenOff, PowerActionReason::Timeout);
}

PowerAction PowerController::on_keep_screen_on_timeout() const {
  if (!is_running()) {
    PowerAction action;
    action.clear_keep_screen_on_policy = true;
    return action;
  }
  PowerAction action = state_action(PowerActionType::TurnScreenOff, PowerActionReason::Timeout);
  action.clear_keep_screen_on_policy = true;
  return action;
}

PowerAction PowerController::on_debug_toggle_screen_off() const {
  if (is_powered_off()) {
    return {};
  }
  return state_action(is_screen_off() ? PowerActionType::WakeScreen : PowerActionType::TurnScreenOff,
                      PowerActionReason::UserInput);
}

PowerAction PowerController::on_raise_to_wake(bool allowed) {
  if (!is_screen_off() || !allowed) {
    return {};
  }
  PowerAction action = state_action(PowerActionType::WakeScreen, PowerActionReason::UserInput);
  action.start_raise_to_wake_session = true;
  return action;
}

PowerAction PowerController::on_raise_dismiss(bool keep_screen_on_active) {
  if (!is_running() || !raise_to_wake_session_active_) {
    return {};
  }
  if (keep_screen_on_active) {
    raise_to_wake_session_active_ = false;
    return {};
  }
  return state_action(PowerActionType::TurnScreenOff, PowerActionReason::UserInput);
}

PowerAction PowerController::on_cover_sleep(bool allowed) const {
  if (!is_running() || !allowed) {
    return {};
  }
  return state_action(PowerActionType::TurnScreenOff, PowerActionReason::UserInput);
}

PowerAction PowerController::on_crown_press_wake(bool allowed) const {
  if (!is_screen_off() || !allowed) {
    return {};
  }
  return state_action(PowerActionType::WakeScreen, PowerActionReason::UserInput);
}

PowerAction PowerController::on_touch_wake(bool allowed) const {
  if (!is_screen_off() || !allowed) {
    return {};
  }
  return state_action(PowerActionType::WakeScreen, PowerActionReason::UserInput);
}

PowerAction PowerController::on_power_mode_changed(const PowerModeModel& model) const {
  return state_action(model.long_battery_mode_enabled ? PowerActionType::EnterLowPowerMode
                                                      : PowerActionType::ExitLowPowerMode,
                      PowerActionReason::UserRequest);
}

PowerAction PowerController::on_battery_changed(const BatteryModel& model,
                                                bool long_battery_mode_enabled) const {
  if (!long_battery_mode_enabled || (!model.charging && !model.external_power)) {
    return {};
  }
  return state_action(PowerActionType::ExitLowPowerMode, PowerActionReason::UserRequest);
}

bool PowerController::should_schedule_auto_screen_off(
    const std::optional<DisplayPolicyModel>& policy) const {
  if (!is_running()) {
    return false;
  }
  if (policy && DisplayPolicyRules::ShouldSuppressAutoScreenOff(*policy)) {
    return false;
  }
  return auto_screen_off_timeout_ms(policy) != 0U;
}

std::uint32_t PowerController::auto_screen_off_timeout_ms(
    const std::optional<DisplayPolicyModel>& policy) const {
  return policy ? policy->screen_off_timeout_ms : 5000U;
}

PowerAction PowerController::sync_keep_screen_on_policy(
    const std::optional<DisplayPolicyModel>& policy,
    bool keep_screen_on_timer_exists) const {
  PowerAction action;
  if (!is_running()) {
    action.cancel_keep_screen_on_timer = true;
    return action;
  }

  const std::uint32_t duration_ms = policy ? policy->keep_screen_on_duration_ms : 0U;
  if (duration_ms == 0U) {
    action.cancel_keep_screen_on_timer = true;
    return action;
  }
  if (keep_screen_on_timer_exists && keep_screen_on_timer_duration_ms_ == duration_ms) {
    return action;
  }

  action.sync_keep_screen_on_policy = true;
  action.next_wakeup_ms = static_cast<std::int32_t>(duration_ms);
  return action;
}

bool PowerController::keep_screen_on_active(
    const std::optional<DisplayPolicyModel>& policy) const {
  return is_running() && policy && policy->keep_screen_on_duration_ms != 0U;
}

std::uint32_t PowerController::keep_screen_on_timer_duration_ms() const {
  return keep_screen_on_timer_duration_ms_;
}

void PowerController::mark_keep_screen_on_timer_scheduled(std::uint32_t duration_ms) {
  keep_screen_on_timer_duration_ms_ = duration_ms;
}

void PowerController::clear_keep_screen_on_timer_cache() {
  keep_screen_on_timer_duration_ms_ = 0U;
}

void PowerController::clear_raise_to_wake_session() {
  raise_to_wake_session_active_ = false;
}

void PowerController::mark_raise_to_wake_session() {
  raise_to_wake_session_active_ = true;
}

PowerAction PowerController::state_action(PowerActionType type, PowerActionReason reason) const {
  PowerAction action;
  action.type = type;
  action.reason = reason;
  return action;
}

}  // namespace twsim::app
