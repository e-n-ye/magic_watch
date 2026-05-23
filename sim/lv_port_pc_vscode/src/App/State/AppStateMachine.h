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
  void handle_power_mode(const PowerModeModel& model);
  void handle_battery_changed(const BatteryModel& model);
  void boot_to_home(PageTransition transition = PageTransition::Fade);
  void return_home(PageTransition transition = PageTransition::Fade);
  void enter_long_battery_mode(PageTransition transition = PageTransition::Fade);
  void exit_long_battery_mode(PageTransition transition = PageTransition::Fade);
  bool wake_from_screen_off(PageTransition transition = PageTransition::Fade);
  void enter_screen_off();
  void enter_powered_off();
  void open_shell_surface(ShellSurface surface);
  void close_shell_surface();
  void preview_notifications_pull(std::int16_t progress);
  void cancel_notifications_pull_preview();
  void preview_quick_settings_pull(std::int16_t progress);
  void cancel_quick_settings_pull_preview();
  void launch_app(PageId target);
  void publish_home_ring_preview(std::uint8_t base_index,
                                 std::int8_t direction,
                                 std::int16_t progress,
                                 bool active,
                                 bool commit);
  void preview_home_surface_crown_rotate(std::int8_t direction);
  void finalize_home_surface_crown_preview();
  void clear_home_surface_crown_preview();
  static void home_surface_crown_preview_timer_cb(lv_timer_t* timer);
  void schedule_notification_screen_off();
  void cancel_notification_screen_off(bool clear_session);
  static void notification_screen_off_timer_cb(lv_timer_t* timer);
  void schedule_auto_screen_off();
  void cancel_auto_screen_off();
  void reset_auto_screen_off_timer();
  static void auto_screen_off_timer_cb(lv_timer_t* timer);
  void sync_keep_screen_on_policy();
  void cancel_keep_screen_on_timer(bool clear_policy);
  static void keep_screen_on_timer_cb(lv_timer_t* timer);
  bool keep_screen_on_active() const;
  bool long_battery_mode_enabled() const;
  bool is_home_surface_page(PageId page_id) const;
  bool is_long_battery_page(PageId page_id) const;
  bool is_current_home_surface() const;
  bool is_current_watchface_surface() const;
  bool is_watchface_shell_preview_context() const;
  void navigate_home_surface(int delta);
  void show_home_surface(std::size_t index);
  bool can_navigate_back() const;
  void sync_shell_surface();

  DataCenter& data_center_;
  PageManager& page_manager_;
  EventBus::Subscription navigation_subscription_;
  EventBus::Subscription input_subscription_;
  EventBus::Subscription notification_wake_subscription_;
  EventBus::Subscription power_mode_subscription_;
  EventBus::Subscription battery_subscription_;
  EventBus::Subscription display_policy_subscription_;
  PowerState power_state_ {PowerState::Booting};
  ShellSurface shell_surface_ {ShellSurface::None};
  bool notification_wake_session_active_ {false};
  bool raise_to_wake_session_active_ {false};
  bool suppress_display_policy_sync_ {false};
  lv_timer_t* notification_screen_off_timer_ {nullptr};
  lv_timer_t* auto_screen_off_timer_ {nullptr};
  lv_timer_t* keep_screen_on_timer_ {nullptr};
  std::uint32_t keep_screen_on_timer_duration_ms_ {0};
  std::size_t home_surface_index_ {0};
  std::optional<PageManager::State> screen_off_page_state_;
  std::size_t screen_off_home_surface_index_ {0};
  bool notifications_pull_preview_active_ {false};
  bool quick_settings_pull_preview_active_ {false};
  lv_timer_t* home_surface_crown_preview_timer_ {nullptr};
  bool home_surface_crown_preview_active_ {false};
  std::int8_t home_surface_crown_preview_direction_ {0};
  std::int16_t home_surface_crown_preview_progress_ {0};
  static constexpr std::size_t kHomeSurfaceCount = 5;
  static constexpr std::int16_t kHomeSurfacePreviewStepPx = 18;
  static constexpr std::int16_t kHomeSurfacePreviewCommitPx = 60;
  static constexpr std::uint32_t kHomeSurfacePreviewSettleMs = 180U;
};

}  // namespace twsim::app
