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
  MotionUpdated,
  DisplayPolicyChanged,
  NotificationsChanged,
  NotificationToastRequested,
  NotificationWakeRequested,
  ShellPreviewRequested,
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

struct MotionModel {
  bool valid {false};
  std::int16_t x_mg {0};
  std::int16_t y_mg {0};
  std::int16_t z_mg {0};
};

enum class NotificationCategory {
  Message,
  BatteryLow,
};

struct NotificationItem {
  std::string id;
  NotificationCategory category {NotificationCategory::Message};
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

struct DisplayPolicyModel {
  bool crown_press_wake_enabled {true};
  bool notification_wake_enabled {true};
  bool raise_to_wake_enabled {true};
  bool tap_to_wake_enabled {false};
  bool always_on_display_enabled {false};
  bool auto_screen_off_enabled {true};
  std::uint32_t screen_off_timeout_ms {5000};
};

struct ShellPreviewModel {
  PageId page_id {PageId::Watchface};
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
                 MotionModel,
                 DisplayPolicyModel,
                 NotificationItem,
                 NotificationCenterModel,
                 ShellPreviewModel,
                 NavigationCommand,
                 InputCommand>;

struct Event {
  EventId id;
  EventPayload payload;
};

}  // namespace twsim::app
