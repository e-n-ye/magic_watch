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
  void publish_motion(const MotionModel& model);
  void publish_navigation(const NavigationCommand& command);
  void publish_input(const InputCommand& command);
  void publish_shell_preview(const ShellPreviewModel& model);
  void publish_notifications(const NotificationCenterModel& model);

  void push_notification(const NotificationItem& item);
  bool dismiss_notification(std::string_view id);
  void clear_notifications();
  void set_notification_wake_enabled(bool enabled);
  void show_toast_for(std::string_view id);
  void clear_toast();

  const std::optional<TimeModel>& time() const;
  const std::optional<BatteryModel>& battery() const;
  const std::optional<MotionModel>& motion() const;
  const std::optional<NotificationCenterModel>& notifications() const;
  const NotificationItem* latest_notification() const;
  const NotificationItem* find_notification(std::string_view id) const;

 private:
  void notify_notifications_changed();

  EventBus event_bus_;
  std::optional<TimeModel> last_time_;
  std::optional<BatteryModel> last_battery_;
  std::optional<MotionModel> last_motion_;
  std::optional<NotificationCenterModel> notification_center_ {NotificationCenterModel {}};
};

}  // namespace twsim::app
