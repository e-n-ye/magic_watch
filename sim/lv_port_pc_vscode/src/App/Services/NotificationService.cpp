#include "App/Services/NotificationService.h"

#include <utility>

namespace twsim::app {

namespace {

constexpr const char* kTextMessage = "\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextJustNow = "\xE5\x88\x9A\xE5\x88\x9A";

}  // namespace

NotificationService::NotificationService(DataCenter& data_center) : data_center_(data_center) {}

void NotificationService::handle_sample(const hal::NotificationSample& sample) {
  if (!sample.valid) {
    return;
  }

  data_center_.push_notification(to_item(sample));
}

NotificationItem NotificationService::to_item(const hal::NotificationSample& sample) {
  NotificationItem item;
  item.id = "msg-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::Message;
  item.source_label = sample.source_label.empty() ? kTextMessage : sample.source_label;
  item.title = sample.title.empty() ? item.source_label : sample.title;
  item.body = sample.body;
  item.time_text = sample.time_text.empty() ? kTextJustNow : sample.time_text;
  item.badge_text = sample.badge_text;
  return item;
}

}  // namespace twsim::app
