#pragma once

#include <cstdint>
#include <memory>

#include "App/Common/DataCenter.h"
#include "App/Input/InputIntentRouter.h"
#include "App/State/AppStateMachine.h"
#include "App/UI/PageManager.h"
#include "HAL/HAL.h"

namespace twsim::app {

class Application {
 public:
  explicit Application(std::unique_ptr<hal::Device> device);

  bool start();
  void tick(std::uint32_t elapsed_ms);

 private:
  void register_pages();
  void handle_hal_event(const hal::Event& event);
  NotificationItem make_mock_message_notification();
  NotificationItem make_mock_battery_low_notification(std::int16_t percent);
  void maybe_emit_battery_low_notification(const BatteryModel& model);

  std::unique_ptr<hal::Device> device_;
  DataCenter data_center_;
  PageManager page_manager_;
  InputIntentRouter input_router_;
  AppStateMachine state_machine_;
  std::uint32_t next_notification_id_ {1};
  bool battery_low_notification_latched_ {false};
};

}  // namespace twsim::app
