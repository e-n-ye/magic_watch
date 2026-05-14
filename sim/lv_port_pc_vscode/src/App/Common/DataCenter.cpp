#include "App/Common/DataCenter.h"

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

const std::optional<TimeModel>& DataCenter::time() const {
  return last_time_;
}

const std::optional<BatteryModel>& DataCenter::battery() const {
  return last_battery_;
}

const std::optional<MotionModel>& DataCenter::motion() const {
  return last_motion_;
}

}  // namespace twsim::app
