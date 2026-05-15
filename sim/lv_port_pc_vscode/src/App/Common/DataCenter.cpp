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

void DataCenter::publish_navigation(const NavigationCommand& command) {
  event_bus_.publish({EventId::NavigationRequested, command});
}

void DataCenter::publish_input(const InputCommand& command) {
  event_bus_.publish({EventId::InputRequested, command});
}

void DataCenter::publish_shell_preview(const ShellPreviewModel& model) {
  event_bus_.publish({EventId::ShellPreviewRequested, model});
}

void DataCenter::publish_notifications(const NotificationCenterModel& model) {
  notification_center_ = model;
  event_bus_.publish({EventId::NotificationsChanged, model});
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
  if (!notification_center_) {
    notification_center_ = NotificationCenterModel {};
  }
  if (notification_center_->wake_on_notification == enabled) {
    return;
  }
  notification_center_->wake_on_notification = enabled;
  notify_notifications_changed();
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

const std::optional<NotificationCenterModel>& DataCenter::notifications() const {
  return notification_center_;
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
