#pragma once

#include <array>

#include "App/Common/DataCenter.h"
#include "App/UI/PageManager.h"

namespace twsim::app {

class AppStateMachine {
 public:
  AppStateMachine(DataCenter& data_center, PageManager& page_manager);

  bool start();

 private:
  enum class PowerState {
    Booting,
    Running,
    ScreenOff,
    PoweredOff,
  };

  enum class ShellSurface {
    None,
    Notifications,
    QuickSettings,
    RightReserved,
    PowerMenu,
  };

  void handle_event(const Event& event);
  void handle_navigation(const NavigationCommand& command);
  void handle_input(const InputCommand& command);
  void boot_to_home(PageTransition transition = PageTransition::Fade);
  void return_home(PageTransition transition = PageTransition::Fade);
  void enter_screen_off();
  void enter_powered_off();
  void open_shell_surface(ShellSurface surface);
  void close_shell_surface();
  void launch_app(PageId target);
  bool is_home_surface_page(PageId page_id) const;
  bool is_current_home_surface() const;
  void navigate_home_surface(int delta);
  void show_home_surface(std::size_t index, PageTransition transition);
  bool can_navigate_back() const;
  void sync_shell_surface();

  DataCenter& data_center_;
  PageManager& page_manager_;
  EventBus::Subscription navigation_subscription_;
  EventBus::Subscription input_subscription_;
  PowerState power_state_ {PowerState::Booting};
  ShellSurface shell_surface_ {ShellSurface::None};
  std::size_t home_surface_index_ {0};
  const std::array<PageId, 5> home_surfaces_ {{
      PageId::Watchface,
      PageId::HomeShortcutPayments,
      PageId::HomeShortcutNfc,
      PageId::HomeShortcutHealth,
      PageId::HomeShortcutWeather,
  }};
};

}  // namespace twsim::app
