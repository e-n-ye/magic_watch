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
    NotificationWakePreview,
    QuickSettings,
    RightReserved,
    PowerMenu,
  };

  void handle_event(const Event& event);
  void handle_navigation(const NavigationCommand& command);
  void handle_input(const InputCommand& command);
  void handle_notification_wake_request(const NotificationItem& item);
  void boot_to_home(PageTransition transition = PageTransition::Fade);
  void return_home(PageTransition transition = PageTransition::Fade);
  void enter_screen_off();
  void enter_powered_off();
  void open_shell_surface(ShellSurface surface);
  void close_shell_surface();
  void preview_notifications_pull(std::int16_t progress);
  void cancel_notifications_pull_preview();
  void preview_quick_settings_pull(std::int16_t progress);
  void cancel_quick_settings_pull_preview();
  void launch_app(PageId target);
  void schedule_notification_screen_off();
  void cancel_notification_screen_off(bool clear_session);
  static void notification_screen_off_timer_cb(lv_timer_t* timer);
  void schedule_auto_screen_off();
  void cancel_auto_screen_off();
  void reset_auto_screen_off_timer();
  static void auto_screen_off_timer_cb(lv_timer_t* timer);
  bool is_home_surface_page(PageId page_id) const;
  bool is_current_home_surface() const;
  bool is_current_watchface_surface() const;
  void navigate_home_surface(int delta);
  void show_home_surface(std::size_t index, PageTransition transition);
  bool can_navigate_back() const;
  void sync_shell_surface();

  DataCenter& data_center_;
  PageManager& page_manager_;
  EventBus::Subscription navigation_subscription_;
  EventBus::Subscription input_subscription_;
  EventBus::Subscription notification_wake_subscription_;
  EventBus::Subscription display_policy_subscription_;
  PowerState power_state_ {PowerState::Booting};
  ShellSurface shell_surface_ {ShellSurface::None};
  bool notification_wake_session_active_ {false};
  lv_timer_t* notification_screen_off_timer_ {nullptr};
  lv_timer_t* auto_screen_off_timer_ {nullptr};
  std::size_t home_surface_index_ {0};
  bool notifications_pull_preview_active_ {false};
  bool quick_settings_pull_preview_active_ {false};
  const std::array<PageId, 5> home_surfaces_ {{
      PageId::Watchface,
      PageId::HomeShortcutPayments,
      PageId::HomeShortcutNfc,
      PageId::HomeShortcutHealth,
      PageId::HomeShortcutWeather,
  }};
};

}  // namespace twsim::app
