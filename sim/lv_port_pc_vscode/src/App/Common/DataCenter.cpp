#include "App/Common/DataCenter.h"

#include <algorithm>

namespace twsim::app {

EventBus::Subscription DataCenter::subscribe(EventId event_id, EventBus::Handler handler) {
  return event_bus_.subscribe(event_id, std::move(handler));
}

void DataCenter::publish_time(const TimeModel& model) {
  last_time_ = model;
  event_bus_.publish({EventId::TimeUpdated, model});
}

void DataCenter::publish_battery(const BatteryModel& model) {
  last_battery_ = model;
  event_bus_.publish({EventId::BatteryChanged, model});
}

void DataCenter::publish_motion(const MotionModel& model) {
  last_motion_ = model;
  event_bus_.publish({EventId::MotionUpdated, model});
}

void DataCenter::publish_power_mode(const PowerModeModel& model) {
  power_mode_ = model;
  event_bus_.publish({EventId::PowerModeChanged, model});
}

void DataCenter::publish_navigation(const NavigationCommand& command) {
  event_bus_.publish({EventId::NavigationRequested, command});
}

void DataCenter::publish_input(const InputCommand& command) {
  event_bus_.publish({EventId::InputRequested, command});
}

void DataCenter::publish_shell_preview(const ShellPreviewModel& model) {
  event_bus_.publish({EventId::ShellPreviewRequested, model});
}

void DataCenter::publish_home_ring_preview(const HomeRingPreviewModel& model) {
  home_ring_preview_ = model;
  event_bus_.publish({EventId::HomeRingPreviewChanged, model});
}

void DataCenter::publish_notifications(const NotificationCenterModel& model) {
  notification_center_ = model;
  event_bus_.publish({EventId::NotificationsChanged, model});
}

void DataCenter::publish_display_policy(const DisplayPolicyModel& model) {
  display_policy_ = model;
  event_bus_.publish({EventId::DisplayPolicyChanged, model});
}

void DataCenter::push_notification(const NotificationItem& item) {
  if (!notification_center_) {
    notification_center_ = NotificationCenterModel {};
  }
  notification_center_->items.push_back(item);
  notify_notifications_changed();
  event_bus_.publish({EventId::NotificationWakeRequested, item});
}

bool DataCenter::dismiss_notification(std::string_view id) {
  if (!notification_center_) {
    return false;
  }

  const auto before = notification_center_->items.size();
  auto& items = notification_center_->items;
  items.erase(std::remove_if(items.begin(),
                             items.end(),
                             [id](const NotificationItem& item) { return item.id == id; }),
              items.end());
  if (notification_center_->active_toast_notification_id &&
      *notification_center_->active_toast_notification_id == id) {
    notification_center_->active_toast_notification_id.reset();
  }
  const bool changed = before != items.size();
  if (changed) {
    notify_notifications_changed();
  }
  return changed;
}

void DataCenter::clear_notifications() {
  if (!notification_center_) {
    return;
  }
  notification_center_->items.clear();
  notification_center_->active_toast_notification_id.reset();
  notify_notifications_changed();
}

void DataCenter::set_notification_wake_enabled(bool enabled) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->notification_wake_enabled == enabled) {
    return;
  }
  display_policy_->notification_wake_enabled = enabled;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_raise_to_wake_enabled(bool enabled) {
  set_raise_to_wake_mode(enabled ? RaiseToWakeMode::AllDay : RaiseToWakeMode::Off);
}

void DataCenter::set_raise_to_wake_mode(RaiseToWakeMode mode) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->raise_to_wake_mode == mode) {
    return;
  }
  display_policy_->raise_to_wake_mode = mode;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_raise_to_wake_window(const DailyTimeWindow& window) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->raise_to_wake_window.start_hour == window.start_hour &&
      display_policy_->raise_to_wake_window.start_minute == window.start_minute &&
      display_policy_->raise_to_wake_window.end_hour == window.end_hour &&
      display_policy_->raise_to_wake_window.end_minute == window.end_minute) {
    return;
  }
  display_policy_->raise_to_wake_window = window;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_tap_to_wake_enabled(bool enabled) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->tap_to_wake_enabled == enabled) {
    return;
  }
  display_policy_->tap_to_wake_enabled = enabled;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_cover_to_sleep_enabled(bool enabled) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->cover_to_sleep_enabled == enabled) {
    return;
  }
  display_policy_->cover_to_sleep_enabled = enabled;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_screen_off_display_mode(ScreenOffDisplayMode mode) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->screen_off_display_mode == mode) {
    return;
  }
  display_policy_->screen_off_display_mode = mode;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_screen_off_display_window(const DailyTimeWindow& window) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->screen_off_display_window.start_hour == window.start_hour &&
      display_policy_->screen_off_display_window.start_minute == window.start_minute &&
      display_policy_->screen_off_display_window.end_hour == window.end_hour &&
      display_policy_->screen_off_display_window.end_minute == window.end_minute) {
    return;
  }
  display_policy_->screen_off_display_window = window;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_screen_off_style_id(ScreenOffStyleId style_id) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->screen_off_style_id == style_id) {
    return;
  }
  display_policy_->screen_off_style_id = style_id;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_always_on_display_enabled(bool enabled) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->always_on_display_enabled == enabled) {
    return;
  }
  display_policy_->always_on_display_enabled = enabled;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_screen_off_timeout_ms(std::uint32_t timeout_ms) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->screen_off_timeout_ms == timeout_ms) {
    return;
  }
  display_policy_->screen_off_timeout_ms = timeout_ms;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_keep_screen_on_duration_ms(std::uint32_t duration_ms) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->keep_screen_on_duration_ms == duration_ms) {
    return;
  }
  display_policy_->keep_screen_on_duration_ms = duration_ms;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_brightness_mode(BrightnessMode mode) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->brightness_mode == mode) {
    return;
  }
  display_policy_->brightness_mode = mode;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_manual_brightness_level(std::uint8_t level) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  const auto clamped = static_cast<std::uint8_t>(std::clamp<int>(level, 0, 100));
  if (display_policy_->manual_brightness_level == clamped) {
    return;
  }
  display_policy_->manual_brightness_level = clamped;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_launcher_layout_mode(LauncherLayoutMode mode) {
  if (!display_policy_) {
    display_policy_ = DisplayPolicyModel {};
  }
  if (display_policy_->launcher_layout_mode == mode) {
    return;
  }
  display_policy_->launcher_layout_mode = mode;
  event_bus_.publish({EventId::DisplayPolicyChanged, *display_policy_});
}

void DataCenter::set_long_battery_mode_enabled(bool enabled) {
  if (!power_mode_) {
    power_mode_ = PowerModeModel {};
  }
  if (power_mode_->long_battery_mode_enabled == enabled) {
    return;
  }
  power_mode_->long_battery_mode_enabled = enabled;
  event_bus_.publish({EventId::PowerModeChanged, *power_mode_});
}

void DataCenter::show_toast_for(std::string_view id) {
  const NotificationItem* item = find_notification(id);
  if (item == nullptr) {
    return;
  }
  if (!notification_center_) {
    notification_center_ = NotificationCenterModel {};
  }
  notification_center_->active_toast_notification_id = item->id;
  notify_notifications_changed();
  event_bus_.publish({EventId::NotificationToastRequested, *item});
}

void DataCenter::clear_toast() {
  if (!notification_center_ || !notification_center_->active_toast_notification_id) {
    return;
  }
  notification_center_->active_toast_notification_id.reset();
  notify_notifications_changed();
}

const std::optional<TimeModel>& DataCenter::time() const {
  return last_time_;
}

const std::optional<BatteryModel>& DataCenter::battery() const {
  return last_battery_;
}

const std::optional<MotionModel>& DataCenter::motion() const {
  return last_motion_;
}

const std::optional<PowerModeModel>& DataCenter::power_mode() const {
  return power_mode_;
}

const std::optional<NotificationCenterModel>& DataCenter::notifications() const {
  return notification_center_;
}

const std::optional<DisplayPolicyModel>& DataCenter::display_policy() const {
  return display_policy_;
}

const std::optional<HomeRingPreviewModel>& DataCenter::home_ring_preview() const {
  return home_ring_preview_;
}

const NotificationItem* DataCenter::latest_notification() const {
  if (!notification_center_ || notification_center_->items.empty()) {
    return nullptr;
  }
  return &notification_center_->items.back();
}

const NotificationItem* DataCenter::find_notification(std::string_view id) const {
  if (!notification_center_) {
    return nullptr;
  }

  const auto& items = notification_center_->items;
  const auto it = std::find_if(items.begin(),
                               items.end(),
                               [id](const NotificationItem& item) { return item.id == id; });
  return it == items.end() ? nullptr : &(*it);
}

void DataCenter::notify_notifications_changed() {
  if (!notification_center_) {
    notification_center_ = NotificationCenterModel {};
  }
  event_bus_.publish({EventId::NotificationsChanged, *notification_center_});
}

}  // namespace twsim::app
