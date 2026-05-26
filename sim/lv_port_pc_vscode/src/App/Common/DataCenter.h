#pragma once

#include <optional>
#include <string_view>

#include "App/Common/AppEvents.h"
#include "App/Common/EventBus.h"

namespace twsim::app {

class DataCenter {
 public:
  EventBus::Subscription subscribe(EventId event_id, EventBus::Handler handler);

  void publish_time(const TimeModel& model);
  void publish_battery(const BatteryModel& model);
  void publish_steps(const StepsModel& model);
  void publish_motion(const MotionModel& model);
  void publish_power_mode(const PowerModeModel& model);
  void publish_health_monitoring_settings(const HealthMonitoringSettingsModel& model);
  void publish_navigation(const NavigationCommand& command);
  void publish_input(const InputCommand& command);
  void publish_shell_preview(const ShellPreviewModel& model);
  void publish_home_ring_preview(const HomeRingPreviewModel& model);
  void publish_notifications(const NotificationCenterModel& model);
  void publish_display_policy(const DisplayPolicyModel& model);

  void push_notification(const NotificationItem& item);
  bool mark_notification_read(std::string_view id);
  bool dismiss_notification(std::string_view id);
  void clear_notifications();
  void set_notification_wake_enabled(bool enabled);
  void set_raise_to_wake_enabled(bool enabled);
  void set_raise_to_wake_mode(RaiseToWakeMode mode);
  void set_raise_to_wake_window(const DailyTimeWindow& window);
  void set_tap_to_wake_enabled(bool enabled);
  void set_cover_to_sleep_enabled(bool enabled);
  void set_screen_off_display_mode(ScreenOffDisplayMode mode);
  void set_screen_off_display_window(const DailyTimeWindow& window);
  void set_screen_off_style_id(ScreenOffStyleId style_id);
  void set_always_on_display_enabled(bool enabled);
  void set_screen_off_timeout_ms(std::uint32_t timeout_ms);
  void set_keep_screen_on_duration_ms(std::uint32_t duration_ms);
  void set_brightness_mode(BrightnessMode mode);
  void set_manual_brightness_level(std::uint8_t level);
  void set_launcher_layout_mode(LauncherLayoutMode mode);
  void set_long_battery_mode_enabled(bool enabled);
  void set_sleep_breathing_quality_enabled(bool enabled);
  void set_heart_health_monitoring_enabled(bool enabled);
  void set_all_day_stress_monitoring_enabled(bool enabled);
  void set_high_precision_sleep_enabled(bool enabled);
  void set_all_day_blood_oxygen_enabled(bool enabled);
  void set_low_blood_oxygen_reminder_mode(LowBloodOxygenReminderMode mode);
  void set_all_day_heart_rate_monitoring_mode(HeartRateAllDayMonitoringMode mode);
  void set_high_heart_rate_reminder_mode(HeartRateHighReminderMode mode);
  void set_low_heart_rate_reminder_mode(HeartRateLowReminderMode mode);
  void show_toast_for(std::string_view id);
  void clear_toast();

  const std::optional<TimeModel>& time() const;
  const std::optional<BatteryModel>& battery() const;
  const std::optional<StepsModel>& steps() const;
  const std::optional<MotionModel>& motion() const;
  const std::optional<PowerModeModel>& power_mode() const;
  const std::optional<HealthMonitoringSettingsModel>& health_monitoring_settings() const;
  const std::optional<NotificationCenterModel>& notifications() const;
  const std::optional<DisplayPolicyModel>& display_policy() const;
  const std::optional<HomeRingPreviewModel>& home_ring_preview() const;
  const NotificationItem* latest_notification() const;
  const NotificationItem* find_notification(std::string_view id) const;

 private:
  void notify_notifications_changed();

  EventBus event_bus_;
  std::optional<TimeModel> last_time_;
  std::optional<BatteryModel> last_battery_;
  std::optional<StepsModel> last_steps_;
  std::optional<MotionModel> last_motion_;
  std::optional<PowerModeModel> power_mode_ {PowerModeModel {}};
  std::optional<HealthMonitoringSettingsModel> health_monitoring_settings_ {HealthMonitoringSettingsModel {}};
  std::optional<NotificationCenterModel> notification_center_ {NotificationCenterModel {}};
  std::optional<DisplayPolicyModel> display_policy_ {DisplayPolicyModel {}};
  std::optional<HomeRingPreviewModel> home_ring_preview_ {HomeRingPreviewModel {}};
};

}  // namespace twsim::app
