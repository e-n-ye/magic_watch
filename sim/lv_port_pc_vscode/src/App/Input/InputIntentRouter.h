#pragma once

#include <optional>

#include "App/Common/AppEvents.h"
#include "App/UI/PageManager.h"
#include "HAL/HAL.h"

namespace twsim::app {

class InputIntentRouter {
 public:
  explicit InputIntentRouter(const PageManager& page_manager);

 std::optional<InputCommand> translate(const hal::Event& event) const;

 private:
  bool is_home_surface_context() const;
  bool is_watchface_context() const;
  bool is_watchface_host_context() const;
  bool is_notifications_preview_context() const;
  bool is_quick_settings_preview_context() const;

  const PageManager& page_manager_;
};

}  // namespace twsim::app
