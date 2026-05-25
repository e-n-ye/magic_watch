#pragma once

#include <cstdint>

#include "App/Common/DataCenter.h"
#include "HAL/HAL.h"

namespace twsim::app {

class NotificationService {
 public:
  explicit NotificationService(DataCenter& data_center);

  void handle_sample(const hal::NotificationSample& sample);

 private:
  NotificationItem to_item(const hal::NotificationSample& sample);

  DataCenter& data_center_;
  std::uint32_t next_notification_id_ {1};
};

}  // namespace twsim::app
