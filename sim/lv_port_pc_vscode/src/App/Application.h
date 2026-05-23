#pragma once

#include <cstdint>
#include <memory>

#include "App/Common/DataCenter.h"
#include "App/Input/InputIntentRouter.h"
#include "App/Services/BatteryPowerService.h"
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

  std::unique_ptr<hal::Device> device_;
  DataCenter data_center_;
  BatteryPowerService battery_power_service_;
  PageManager page_manager_;
  InputIntentRouter input_router_;
  AppStateMachine state_machine_;
  std::uint32_t next_notification_id_ {1};
};

}  // namespace twsim::app
