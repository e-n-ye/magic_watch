#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "App/UI/PageId.h"

namespace twsim::app {

enum class EventId {
  TimeUpdated,
  BatteryChanged,
  StepsChanged,
  MotionUpdated,
  PowerModeChanged,
  DisplayPolicyChanged,
  HealthMonitoringSettingsChanged,
  NotificationsChanged,
  NotificationToastRequested,
  NotificationWakeRequested,
  ShellPreviewRequested,
  HomeRingPreviewChanged,
  NavigationRequested,
  InputRequested,
};

struct TimeModel {
  bool valid {false};
  std::uint16_t year {0};
  std::uint8_t month {0};
  std::uint8_t day {0};
  std::uint8_t hour {0};
  std::uint8_t minute {0};
  std::uint8_t second {0};
};

struct BatteryModel {
  bool present {true};
  bool charging {false};
  bool external_power {false};
  std::int16_t percent {0};
  std::uint16_t millivolts {0};
};

struct StepsModel {
  bool valid {false};
  std::uint32_t daily_steps {0};
};

struct MotionModel {
  bool valid {false};
  std::int16_t x_mg {0};
  std::int16_t y_mg {0};
  std::int16_t z_mg {0};
};

struct PowerModeModel {
  bool long_battery_mode_enabled {false};
};

enum class LowBloodOxygenReminderMode {
  Off,
  Threshold90,
  Threshold85,
  Threshold80,
};

struct HealthMonitoringSettingsModel {
  bool sleep_breathing_quality_enabled {true};
  bool heart_health_monitoring_enabled {true};
  bool all_day_stress_monitoring_enabled {true};
  bool high_precision_sleep_enabled {true};
  bool all_day_blood_oxygen_enabled {true};
  LowBloodOxygenReminderMode low_blood_oxygen_reminder_mode {LowBloodOxygenReminderMode::Off};
};

enum class NotificationCategory {
  Message,
  BatteryLow,
};

struct NotificationItem {
  std::string id;
  NotificationCategory category {NotificationCategory::Message};
  bool read {false};
  std::string source_label;
  std::string title;
  std::string body;
  std::string time_text;
  std::string badge_text;
};

struct NotificationCenterModel {
  std::vector<NotificationItem> items;
  std::optional<std::string> active_toast_notification_id;
};

enum class BrightnessMode {
  Auto,
  Manual,
};

enum class RaiseToWakeMode {
  Off,
  AllDay,
  Scheduled,
};

enum class ScreenOffDisplayMode {
  Off,
  Smart,
  Scheduled,
};

enum class ScreenOffStyleId {
  AnalogHands,
  InfoDigits,
};

enum class LauncherLayoutMode {
  MultiColumn,
  List,
  Categorized,
};

struct DailyTimeWindow {
  std::uint8_t start_hour {8};
  std::uint8_t start_minute {0};
  std::uint8_t end_hour {22};
  std::uint8_t end_minute {0};
};

struct DisplayPolicyModel {
  bool crown_press_wake_enabled {true};
  bool notification_wake_enabled {true};
  RaiseToWakeMode raise_to_wake_mode {RaiseToWakeMode::AllDay};
  DailyTimeWindow raise_to_wake_window {};
  bool tap_to_wake_enabled {false};
  bool cover_to_sleep_enabled {true};
  ScreenOffDisplayMode screen_off_display_mode {ScreenOffDisplayMode::Off};
  DailyTimeWindow screen_off_display_window {};
  ScreenOffStyleId screen_off_style_id {ScreenOffStyleId::AnalogHands};
  bool always_on_display_enabled {false};
  bool auto_screen_off_enabled {true};
  std::uint32_t screen_off_timeout_ms {5000};
  std::uint32_t keep_screen_on_duration_ms {0};
  BrightnessMode brightness_mode {BrightnessMode::Auto};
  std::uint8_t manual_brightness_level {60};
  LauncherLayoutMode launcher_layout_mode {LauncherLayoutMode::Categorized};
};

struct ShellPreviewModel {
  PageId page_id {PageId::Watchface};
  std::int16_t progress {0};
  bool active {false};
  bool commit {false};
};

struct HomeRingPreviewModel {
  std::uint8_t base_index {0};
  std::int8_t direction {0};
  std::int16_t progress {0};
  bool active {false};
  bool commit {false};
};

enum class NavigationAction {
  SetRoot,
  Push,
  Pop,
  ReturnHome,
  LaunchApp,
  CloseShellSurface,
  OpenLauncher,
  OpenNotifications,
  OpenQuickSettings,
  OpenNotificationWakePreview,
  OpenPowerMenu,
  PowerOff,
  Restart,
};

struct NavigationCommand {
  NavigationAction action {NavigationAction::SetRoot};
  PageId target {PageId::Watchface};
};

enum class InputAction {
  DebugToggleScreenOff,
  DebugOpenPowerMenu,
  SimRaiseToWake,
  SimRaiseDismiss,
  SimCoverSleep,
  CrownPress,
  CrownRotateCW,
  CrownRotateCCW,
  TouchActivity,
  ScrollDrag,
  ScrollFlick,
  ScrollRelease,
  TopEdgeProgress,
  TopEdgeCancel,
  BottomEdgeProgress,
  BottomEdgeCancel,
  EdgeBackProgress,
  EdgeBackCancel,
  HomeSwipeProgress,
  HomeSwipeCancel,
  NavigateBack,
  HomeEdgeBackRight,
  HomeSwipeLeft,
  OpenNotifications,
  OpenQuickSettings,
};

struct InputCommand {
  InputAction action {InputAction::DebugToggleScreenOff};
  std::int16_t value {0};
  std::int16_t x {0};
  std::int16_t y {0};
};

using EventPayload =
    std::variant<std::monostate,
                 TimeModel,
                 BatteryModel,
                 StepsModel,
                 MotionModel,
                 PowerModeModel,
                 DisplayPolicyModel,
                 HealthMonitoringSettingsModel,
                 NotificationItem,
                 NotificationCenterModel,
                 ShellPreviewModel,
                 HomeRingPreviewModel,
                 NavigationCommand,
                 InputCommand>;

struct Event {
  EventId id;
  EventPayload payload;
};

}  // namespace twsim::app
