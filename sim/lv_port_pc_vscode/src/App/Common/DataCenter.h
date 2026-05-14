#pragma once

#include <optional>

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

  const std::optional<TimeModel>& time() const;
  const std::optional<BatteryModel>& battery() const;
  const std::optional<MotionModel>& motion() const;

 private:
  EventBus event_bus_;
  std::optional<TimeModel> last_time_;
  std::optional<BatteryModel> last_battery_;
  std::optional<MotionModel> last_motion_;
};

}  // namespace twsim::app
