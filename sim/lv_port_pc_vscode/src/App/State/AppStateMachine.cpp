#include "App/State/AppStateMachine.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>

#include "lvgl/lvgl.h"

namespace twsim::app {

namespace {

class EdgeBackOverlay {
 public:
  void show_progress(std::int16_t progress) {
    ensure_created();
    if (hint_root_ == nullptr || active_screen_ == nullptr) {
      return;
    }

    const auto clamped = std::clamp<std::int16_t>(progress, 0, 84);
    lv_obj_set_width(hint_root_, std::max<lv_coord_t>(6, clamped / 3));
    lv_obj_set_style_bg_opa(
        hint_root_, static_cast<lv_opa_t>((clamped * LV_OPA_70) / 84), 0);
    if (arrow_ != nullptr) {
      lv_obj_set_style_opa(arrow_, static_cast<lv_opa_t>((clamped * LV_OPA_80) / 84), 0);
    }
    lv_obj_set_style_translate_x(active_screen_, clamped / 4, 0);
  }

  void hide() {
    if (active_screen_ != nullptr) {
      lv_obj_set_style_translate_x(active_screen_, 0, 0);
      active_screen_ = nullptr;
    }
    if (hint_root_ != nullptr) {
      lv_obj_set_width(hint_root_, 0);
      lv_obj_set_style_bg_opa(hint_root_, LV_OPA_TRANSP, 0);
    }
    if (arrow_ != nullptr) {
      lv_obj_set_style_opa(arrow_, LV_OPA_TRANSP, 0);
    }
  }

 private:
  void ensure_created() {
    lv_obj_t* current_screen = lv_screen_active();
    if (active_screen_ != nullptr && active_screen_ != current_screen) {
      lv_obj_set_style_translate_x(active_screen_, 0, 0);
    }
    active_screen_ = current_screen;
    if (hint_root_ != nullptr) {
      if (lv_obj_get_parent(hint_root_) != lv_layer_top()) {
        lv_obj_set_parent(hint_root_, lv_layer_top());
      }
      return;
    }

    hint_root_ = lv_obj_create(lv_layer_top());
    if (hint_root_ == nullptr) {
      return;
    }

    lv_obj_set_size(hint_root_, 0, 132);
    lv_obj_align(hint_root_, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_remove_flag(hint_root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_root_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(hint_root_, lv_color_hex(0xD8E6FF), 0);
    lv_obj_set_style_bg_opa(hint_root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hint_root_, 0, 0);
    lv_obj_set_style_radius(hint_root_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(hint_root_, 0, 0);

    arrow_ = lv_label_create(hint_root_);
    if (arrow_ == nullptr) {
      return;
    }
    lv_label_set_text(arrow_, ">");
    lv_obj_set_style_text_font(arrow_, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(arrow_, lv_color_hex(0x0F172A), 0);
    lv_obj_set_style_opa(arrow_, LV_OPA_TRANSP, 0);
    lv_obj_center(arrow_);
  }

  lv_obj_t* hint_root_ {nullptr};
  lv_obj_t* arrow_ {nullptr};
  lv_obj_t* active_screen_ {nullptr};
};

PageTransition open_transition_for(PageId page_id) {
  switch (page_id) {
    case PageId::Notifications:
      return PageTransition::MoveUp;
    case PageId::NotificationWakePreview:
      return PageTransition::Fade;
    case PageId::QuickSettings:
      return PageTransition::MoveDown;
    case PageId::PowerMenu:
    default:
      return PageTransition::Fade;
  }
}

PageTransition close_transition_for(PageId page_id) {
  switch (page_id) {
    case PageId::Notifications:
      return PageTransition::None;
    case PageId::NotificationWakePreview:
      return PageTransition::Fade;
    case PageId::QuickSettings:
      return PageTransition::None;
    case PageId::PowerMenu:
    default:
      return PageTransition::Fade;
  }
}

bool is_time_in_window(const TimeModel& time, const DailyTimeWindow& window) {
  if (!time.valid) {
    return false;
  }

  const int current_minutes = static_cast<int>(time.hour) * 60 + static_cast<int>(time.minute);
  const int start_minutes = static_cast<int>(window.start_hour) * 60 + static_cast<int>(window.start_minute);
  const int end_minutes = static_cast<int>(window.end_hour) * 60 + static_cast<int>(window.end_minute);

  if (start_minutes == end_minutes) {
    return true;
  }
  if (start_minutes < end_minutes) {
    return current_minutes >= start_minutes && current_minutes < end_minutes;
  }
  return current_minutes >= start_minutes || current_minutes < end_minutes;
}

bool raise_to_wake_allowed(DataCenter& data_center) {
  const auto policy = data_center.display_policy();
  if (!policy) {
    return true;
  }

  switch (policy->raise_to_wake_mode) {
    case RaiseToWakeMode::Off:
      return false;
    case RaiseToWakeMode::Scheduled: {
      const auto& time = data_center.time();
      return time && is_time_in_window(*time, policy->raise_to_wake_window);
    }
    case RaiseToWakeMode::AllDay:
    default:
      return true;
  }
}

void clear_raise_session(bool& session_flag) {
  session_flag = false;
}

}  // namespace

AppStateMachine::AppStateMachine(DataCenter& data_center, PageManager& page_manager)
    : data_center_(data_center), page_manager_(page_manager) {
  navigation_subscription_ =
      data_center_.subscribe(EventId::NavigationRequested,
                             [this](const Event& event) { handle_event(event); });
  input_subscription_ =
      data_center_.subscribe(EventId::InputRequested,
                             [this](const Event& event) { handle_event(event); });
  notification_wake_subscription_ =
      data_center_.subscribe(EventId::NotificationWakeRequested,
                             [this](const Event& event) { handle_event(event); });
  display_policy_subscription_ =
      data_center_.subscribe(EventId::DisplayPolicyChanged,
                             [this](const Event&) {
                               if (suppress_display_policy_sync_) {
                                 return;
                               }
                               if (power_state_ == PowerState::Running) {
                                 sync_keep_screen_on_policy();
                                 schedule_auto_screen_off();
                               }
                             });
}

bool AppStateMachine::start() {
  if (!page_manager_.set_root(PageId::HomeRingHost, PageTransition::None)) {
    return false;
  }

  power_state_ = PowerState::Running;
  shell_surface_ = ShellSurface::None;
  home_surface_index_ = 0;
  publish_home_ring_preview(0, 0, 0, false, false);
  sync_keep_screen_on_policy();
  schedule_auto_screen_off();
  return true;
}

void AppStateMachine::handle_event(const Event& event) {
  if (const auto* command = std::get_if<NavigationCommand>(&event.payload)) {
    handle_navigation(*command);
    return;
  }

  if (const auto* input = std::get_if<InputCommand>(&event.payload)) {
    handle_input(*input);
    return;
  }

  if (event.id == EventId::NotificationWakeRequested) {
    if (const auto* item = std::get_if<NotificationItem>(&event.payload)) {
      handle_notification_wake_request(*item);
    }
  }
}

void AppStateMachine::handle_navigation(const NavigationCommand& command) {
  bool navigation_ok = false;

  switch (command.action) {
    case NavigationAction::SetRoot:
      if (command.target == PageId::Watchface || command.target == PageId::HomeRingHost) {
        if (power_state_ != PowerState::PoweredOff) {
          boot_to_home(PageTransition::Fade);
        }
        return;
      }
      if (command.target == PageId::ScreenOff) {
        enter_screen_off();
        return;
      }
      if (command.target == PageId::PoweredOff) {
        enter_powered_off();
        return;
      }
      navigation_ok =
          power_state_ != PowerState::PoweredOff && page_manager_.set_root(command.target, PageTransition::Fade);
      break;
    case NavigationAction::Push:
      navigation_ok =
          power_state_ == PowerState::Running && page_manager_.push(command.target, PageTransition::MoveLeft);
      break;
    case NavigationAction::Pop:
      if (page_manager_.temporary_page_id()) {
        close_shell_surface();
        return;
      }
      navigation_ok =
          power_state_ == PowerState::Running && page_manager_.pop(PageTransition::MoveRight);
      break;
    case NavigationAction::ReturnHome:
      if (power_state_ != PowerState::PoweredOff) {
        return_home();
      }
      return;
    case NavigationAction::LaunchApp:
      launch_app(command.target);
      return;
    case NavigationAction::CloseShellSurface:
      close_shell_surface();
      return;
    case NavigationAction::OpenLauncher:
      launch_app(PageId::Launcher);
      return;
    case NavigationAction::OpenNotifications:
      open_shell_surface(ShellSurface::Notifications);
      return;
    case NavigationAction::OpenQuickSettings:
      open_shell_surface(ShellSurface::QuickSettings);
      return;
    case NavigationAction::OpenNotificationWakePreview:
      open_shell_surface(ShellSurface::NotificationWakePreview);
      return;
    case NavigationAction::OpenPowerMenu:
      open_shell_surface(ShellSurface::PowerMenu);
      return;
    case NavigationAction::PowerOff:
      enter_powered_off();
      return;
    case NavigationAction::Restart:
      boot_to_home();
      return;
    default:
      return;
  }

  if (navigation_ok) {
    sync_shell_surface();
  }
}

void AppStateMachine::handle_input(const InputCommand& command) {
  static EdgeBackOverlay overlay;

  if (power_state_ == PowerState::Running && notification_screen_off_timer_ != nullptr) {
    cancel_notification_screen_off(true);
  }
  if (power_state_ == PowerState::Running) {
    reset_auto_screen_off_timer();
  }

  switch (command.action) {
    case InputAction::DebugToggleScreenOff:
      overlay.hide();
      if (power_state_ == PowerState::PoweredOff) {
        return;
      }
      if (power_state_ == PowerState::ScreenOff) {
        return_home();
        return;
      }
      enter_screen_off();
      return;
    case InputAction::DebugOpenPowerMenu:
      overlay.hide();
      if (power_state_ == PowerState::PoweredOff) {
        boot_to_home();
        return;
      }
      open_shell_surface(ShellSurface::PowerMenu);
      return;
    case InputAction::SimRaiseToWake:
      overlay.hide();
      if (power_state_ != PowerState::ScreenOff) {
        return;
      }
      if (!raise_to_wake_allowed(data_center_)) {
        return;
      }
      return_home();
      raise_to_wake_session_active_ = true;
      return;
    case InputAction::SimRaiseDismiss:
      overlay.hide();
      if (power_state_ != PowerState::Running || !raise_to_wake_session_active_) {
        return;
      }
      if (keep_screen_on_active()) {
        clear_raise_session(raise_to_wake_session_active_);
        return;
      }
      enter_screen_off();
      return;
    case InputAction::EdgeBackProgress:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_), -1, command.value, true, false);
      } else if (can_navigate_back()) {
        overlay.show_progress(command.value);
      }
      return;
    case InputAction::EdgeBackCancel:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_), 0, 0, false, false);
      } else {
        overlay.hide();
      }
      return;
    case InputAction::HomeSwipeProgress:
      if (power_state_ == PowerState::Running && is_current_home_surface() && command.value != 0) {
        const std::int8_t direction = command.value < 0 ? 1 : -1;
        publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_),
                                  direction,
                                  static_cast<std::int16_t>(std::abs(command.value)),
                                  true,
                                  false);
      }
      return;
    case InputAction::HomeSwipeCancel:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_), 0, 0, false, false);
      }
      return;
    case InputAction::NavigateBack:
      overlay.hide();
      if (page_manager_.temporary_page_id()) {
        close_shell_surface();
      } else if (power_state_ == PowerState::Running) {
        page_manager_.pop(PageTransition::MoveRight);
      }
      return;
    case InputAction::OpenNotifications:
      if (power_state_ == PowerState::Running) {
        if (!notifications_pull_preview_active_ && !is_watchface_shell_preview_context()) {
          return;
        }
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::Notifications &&
            !notifications_pull_preview_active_) {
          return;
        }
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::Notifications &&
            notifications_pull_preview_active_) {
          data_center_.publish_shell_preview({PageId::Notifications, 220, true, true});
          notifications_pull_preview_active_ = false;
          return;
        }
        notifications_pull_preview_active_ = false;
        open_shell_surface(ShellSurface::Notifications);
      }
      return;
    case InputAction::TopEdgeProgress:
      if (power_state_ == PowerState::Running) {
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::Notifications &&
            !notifications_pull_preview_active_) {
          return;
        }
        preview_notifications_pull(command.value);
      }
      return;
    case InputAction::TopEdgeCancel:
      if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::Notifications &&
          !notifications_pull_preview_active_) {
        return;
      }
      cancel_notifications_pull_preview();
      return;
    case InputAction::BottomEdgeProgress:
      if (power_state_ == PowerState::Running) {
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::QuickSettings &&
            !quick_settings_pull_preview_active_) {
          return;
        }
        preview_quick_settings_pull(command.value);
      }
      return;
    case InputAction::BottomEdgeCancel:
      if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::QuickSettings &&
          !quick_settings_pull_preview_active_) {
        return;
      }
      cancel_quick_settings_pull_preview();
      return;
    case InputAction::OpenQuickSettings:
      if (power_state_ == PowerState::Running) {
        if (!quick_settings_pull_preview_active_ && !is_watchface_shell_preview_context()) {
          return;
        }
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::QuickSettings &&
            !quick_settings_pull_preview_active_) {
          return;
        }
        if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::QuickSettings &&
            quick_settings_pull_preview_active_) {
          data_center_.publish_shell_preview({PageId::QuickSettings, 220, true, true});
          quick_settings_pull_preview_active_ = false;
          return;
        }
        open_shell_surface(ShellSurface::QuickSettings);
      }
      return;
    case InputAction::CrownPress:
      overlay.hide();
      if (power_state_ == PowerState::PoweredOff) {
        return;
      }
      if (power_state_ == PowerState::ScreenOff) {
        const auto policy = data_center_.display_policy();
        if (policy && !policy->crown_press_wake_enabled) {
          return;
        }
        return_home();
        return;
      }
      if (is_current_watchface_surface()) {
        launch_app(PageId::Launcher);
        return;
      }
      return_home();
      return;
    case InputAction::HomeEdgeBackRight:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        navigate_home_surface(-1);
      }
      return;
    case InputAction::HomeSwipeLeft:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        navigate_home_surface(1);
      }
      return;
    case InputAction::CrownRotateCW:
      if (power_state_ == PowerState::Running && is_current_home_surface() && !is_current_watchface_surface()) {
        navigate_home_surface(1);
      }
      return;
    case InputAction::CrownRotateCCW:
      if (power_state_ == PowerState::Running && is_current_home_surface() && !is_current_watchface_surface()) {
        navigate_home_surface(-1);
      }
      return;
    case InputAction::TouchActivity:
      if (power_state_ != PowerState::ScreenOff) {
        return;
      }
      if (const auto policy = data_center_.display_policy(); policy && !policy->tap_to_wake_enabled) {
        return;
      }
      return_home();
      return;
    case InputAction::ScrollDrag:
    case InputAction::ScrollFlick:
    default:
      return;
  }
}

void AppStateMachine::boot_to_home(PageTransition transition) {
  cancel_notification_screen_off(true);
  clear_raise_session(raise_to_wake_session_active_);
  if (page_manager_.set_root(PageId::HomeRingHost, transition)) {
    power_state_ = PowerState::Running;
    shell_surface_ = ShellSurface::None;
    home_surface_index_ = 0;
    publish_home_ring_preview(0, 0, 0, false, false);
    sync_keep_screen_on_policy();
    schedule_auto_screen_off();
  }
}

void AppStateMachine::return_home(PageTransition transition) {
  boot_to_home(transition);
}

void AppStateMachine::enter_screen_off() {
  cancel_notification_screen_off(true);
  cancel_auto_screen_off();
  suppress_display_policy_sync_ = true;
  cancel_keep_screen_on_timer(true);
  suppress_display_policy_sync_ = false;
  clear_raise_session(raise_to_wake_session_active_);
  if (page_manager_.set_root(PageId::ScreenOff, PageTransition::Fade)) {
    power_state_ = PowerState::ScreenOff;
    shell_surface_ = ShellSurface::None;
  }
}

void AppStateMachine::enter_powered_off() {
  cancel_notification_screen_off(true);
  cancel_auto_screen_off();
  suppress_display_policy_sync_ = true;
  cancel_keep_screen_on_timer(true);
  suppress_display_policy_sync_ = false;
  clear_raise_session(raise_to_wake_session_active_);
  if (page_manager_.set_root(PageId::PoweredOff, PageTransition::Fade)) {
    power_state_ = PowerState::PoweredOff;
    shell_surface_ = ShellSurface::None;
  }
}

void AppStateMachine::open_shell_surface(ShellSurface surface) {
  if (power_state_ == PowerState::PoweredOff) {
    return;
  }
  if (power_state_ == PowerState::ScreenOff &&
      surface != ShellSurface::PowerMenu &&
      surface != ShellSurface::NotificationWakePreview) {
    return;
  }

  PageId target = PageId::Watchface;
  switch (surface) {
    case ShellSurface::Notifications:
      target = PageId::Notifications;
      break;
    case ShellSurface::NotificationWakePreview:
      target = PageId::NotificationWakePreview;
      break;
    case ShellSurface::QuickSettings:
      target = PageId::QuickSettings;
      break;
    case ShellSurface::PowerMenu:
      target = PageId::PowerMenu;
      break;
    case ShellSurface::RightReserved:
    case ShellSurface::None:
    default:
      return;
  }

  if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == target) {
    shell_surface_ = surface;
    return;
  }

  if (page_manager_.show_temporary(target, open_transition_for(target))) {
    shell_surface_ = surface;
  }
}

void AppStateMachine::close_shell_surface() {
  const auto temporary = page_manager_.temporary_page_id();
  if (!temporary) {
    shell_surface_ = ShellSurface::None;
    return;
  }

  const bool was_notification_wake_preview = *temporary == PageId::NotificationWakePreview;
  if (page_manager_.dismiss_temporary(close_transition_for(*temporary))) {
    shell_surface_ = ShellSurface::None;
    notifications_pull_preview_active_ = false;
    quick_settings_pull_preview_active_ = false;
    if (was_notification_wake_preview && notification_wake_session_active_) {
      schedule_notification_screen_off();
    }
  }
}

void AppStateMachine::preview_notifications_pull(std::int16_t progress) {
  const auto stack_top = page_manager_.stack_top_page_id();
  if (power_state_ != PowerState::Running || !stack_top || *stack_top != PageId::HomeRingHost ||
      !is_watchface_shell_preview_context()) {
    return;
  }

  const auto temporary = page_manager_.temporary_page_id();
  if (!temporary) {
    if (page_manager_.show_temporary(PageId::Notifications, PageTransition::None)) {
      shell_surface_ = ShellSurface::Notifications;
      notifications_pull_preview_active_ = true;
    }
  } else if (*temporary != PageId::Notifications) {
    return;
  } else {
    notifications_pull_preview_active_ = true;
  }

  data_center_.publish_shell_preview({PageId::Notifications, progress, true, false});
}

void AppStateMachine::cancel_notifications_pull_preview() {
  if (!notifications_pull_preview_active_) {
    return;
  }
  notifications_pull_preview_active_ = false;

  if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::Notifications) {
    data_center_.publish_shell_preview({PageId::Notifications, 0, false, false});
  }
}

void AppStateMachine::preview_quick_settings_pull(std::int16_t progress) {
  const auto stack_top = page_manager_.stack_top_page_id();
  if (power_state_ != PowerState::Running || !stack_top || *stack_top != PageId::HomeRingHost ||
      !is_watchface_shell_preview_context()) {
    return;
  }

  const auto temporary = page_manager_.temporary_page_id();
  if (!temporary) {
    if (page_manager_.show_temporary(PageId::QuickSettings, PageTransition::None)) {
      shell_surface_ = ShellSurface::QuickSettings;
      quick_settings_pull_preview_active_ = true;
    }
  } else if (*temporary != PageId::QuickSettings) {
    return;
  } else {
    quick_settings_pull_preview_active_ = true;
  }

  data_center_.publish_shell_preview({PageId::QuickSettings, progress, true, false});
}

void AppStateMachine::cancel_quick_settings_pull_preview() {
  if (!quick_settings_pull_preview_active_) {
    return;
  }
  quick_settings_pull_preview_active_ = false;

  if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::QuickSettings) {
    data_center_.publish_shell_preview({PageId::QuickSettings, 0, false, false});
  }
}

void AppStateMachine::launch_app(PageId target) {
  if (power_state_ == PowerState::PoweredOff) {
    return;
  }

  if (!page_manager_.temporary_page_id()) {
    const auto stack_top = page_manager_.stack_top_page_id();
    if (target != PageId::Launcher && stack_top && *stack_top == PageId::Launcher) {
      page_manager_.push(target, PageTransition::MoveLeft);
      return;
    }
  }

  if (!page_manager_.set_root(PageId::HomeRingHost, PageTransition::None)) {
    return;
  }

  shell_surface_ = ShellSurface::None;
  power_state_ = PowerState::Running;
  home_surface_index_ = 0;
  publish_home_ring_preview(0, 0, 0, false, false);

  if (target == PageId::Watchface || target == PageId::HomeRingHost) {
    return;
  }

  page_manager_.push(target, PageTransition::MoveLeft);
}

bool AppStateMachine::is_home_surface_page(PageId page_id) const {
  return page_id == PageId::HomeRingHost;
}

bool AppStateMachine::is_current_home_surface() const {
  const auto stack_top = page_manager_.stack_top_page_id();
  return !page_manager_.temporary_page_id() && page_manager_.stack_depth() == 1 && stack_top &&
         is_home_surface_page(*stack_top);
}

bool AppStateMachine::is_current_watchface_surface() const {
  return is_current_home_surface() && home_surface_index_ == 0;
}

bool AppStateMachine::is_watchface_shell_preview_context() const {
  const auto stack_top = page_manager_.stack_top_page_id();
  return power_state_ == PowerState::Running && stack_top && *stack_top == PageId::HomeRingHost &&
         page_manager_.stack_depth() == 1 && home_surface_index_ == 0;
}

void AppStateMachine::navigate_home_surface(int delta) {
  const auto size = static_cast<int>(kHomeSurfaceCount);
  const auto current = static_cast<int>(home_surface_index_);
  int next = (current + delta) % size;
  if (next < 0) {
    next += size;
  }

  publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_),
                            delta >= 0 ? 1 : -1,
                            240,
                            false,
                            true);
  show_home_surface(static_cast<std::size_t>(next));
}

void AppStateMachine::show_home_surface(std::size_t index) {
  if (index >= kHomeSurfaceCount) {
    return;
  }

  home_surface_index_ = index;
  shell_surface_ = ShellSurface::None;
  power_state_ = PowerState::Running;
  publish_home_ring_preview(static_cast<std::uint8_t>(home_surface_index_), 0, 0, false, false);
  sync_keep_screen_on_policy();
  schedule_auto_screen_off();
}

bool AppStateMachine::can_navigate_back() const {
  if (power_state_ == PowerState::PoweredOff) {
    return false;
  }
  if (page_manager_.temporary_page_id()) {
    return true;
  }
  return power_state_ == PowerState::Running && page_manager_.stack_depth() > 1;
}

void AppStateMachine::sync_shell_surface() {
  const auto temporary = page_manager_.temporary_page_id();
  if (!temporary) {
    shell_surface_ = ShellSurface::None;
    return;
  }

  switch (*temporary) {
    case PageId::Notifications:
      shell_surface_ = ShellSurface::Notifications;
      break;
    case PageId::NotificationWakePreview:
      shell_surface_ = ShellSurface::NotificationWakePreview;
      break;
    case PageId::QuickSettings:
      shell_surface_ = ShellSurface::QuickSettings;
      break;
    case PageId::PowerMenu:
      shell_surface_ = ShellSurface::PowerMenu;
      break;
    default:
      shell_surface_ = ShellSurface::None;
      break;
  }
}

void AppStateMachine::handle_notification_wake_request(const NotificationItem& item) {
  if (power_state_ == PowerState::PoweredOff) {
    return;
  }

  const auto display_policy = data_center_.display_policy();
  const bool wake_on_notification = !display_policy || display_policy->notification_wake_enabled;

  if (power_state_ == PowerState::ScreenOff) {
    if (!wake_on_notification) {
      return;
    }

    cancel_notification_screen_off(false);
    if (!page_manager_.set_root(PageId::HomeRingHost, PageTransition::None)) {
      return;
    }

    power_state_ = PowerState::Running;
    shell_surface_ = ShellSurface::None;
    home_surface_index_ = 0;
    publish_home_ring_preview(0, 0, 0, false, false);
    notification_wake_session_active_ = true;
    open_shell_surface(ShellSurface::NotificationWakePreview);
    return;
  }

  if (page_manager_.temporary_page_id() && *page_manager_.temporary_page_id() == PageId::NotificationWakePreview) {
    return;
  }

  if (power_state_ == PowerState::Running) {
    data_center_.show_toast_for(item.id);
  }
}

void AppStateMachine::publish_home_ring_preview(std::uint8_t base_index,
                                                std::int8_t direction,
                                                std::int16_t progress,
                                                bool active,
                                                bool commit) {
  data_center_.publish_home_ring_preview(
      {base_index, direction, static_cast<std::int16_t>(std::clamp<std::int16_t>(progress, 0, 240)), active, commit});
}

void AppStateMachine::schedule_notification_screen_off() {
  cancel_notification_screen_off(false);
  notification_screen_off_timer_ = lv_timer_create(&AppStateMachine::notification_screen_off_timer_cb, 5000U, this);
  if (notification_screen_off_timer_ != nullptr) {
    lv_timer_set_repeat_count(notification_screen_off_timer_, 1);
  }
}

void AppStateMachine::cancel_notification_screen_off(bool clear_session) {
  if (notification_screen_off_timer_ != nullptr) {
    lv_timer_del(notification_screen_off_timer_);
    notification_screen_off_timer_ = nullptr;
  }
  if (clear_session) {
    notification_wake_session_active_ = false;
  }
}

void AppStateMachine::notification_screen_off_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<AppStateMachine*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }

  self->notification_screen_off_timer_ = nullptr;
  self->enter_screen_off();
}

void AppStateMachine::schedule_auto_screen_off() {
  cancel_auto_screen_off();
  if (power_state_ != PowerState::Running) {
    return;
  }

  const auto policy = data_center_.display_policy();
  if (policy &&
      (!policy->auto_screen_off_enabled || policy->always_on_display_enabled || policy->keep_screen_on_duration_ms > 0U)) {
    return;
  }

  const std::uint32_t timeout_ms = policy ? policy->screen_off_timeout_ms : 5000U;
  if (timeout_ms == 0U) {
    return;
  }

  auto_screen_off_timer_ = lv_timer_create(&AppStateMachine::auto_screen_off_timer_cb, timeout_ms, this);
  if (auto_screen_off_timer_ != nullptr) {
    lv_timer_set_repeat_count(auto_screen_off_timer_, 1);
  }
}

void AppStateMachine::cancel_auto_screen_off() {
  if (auto_screen_off_timer_ != nullptr) {
    lv_timer_del(auto_screen_off_timer_);
    auto_screen_off_timer_ = nullptr;
  }
}

void AppStateMachine::sync_keep_screen_on_policy() {
  if (power_state_ != PowerState::Running) {
    cancel_keep_screen_on_timer(false);
    return;
  }

  const auto policy = data_center_.display_policy();
  const std::uint32_t duration_ms = policy ? policy->keep_screen_on_duration_ms : 0U;
  if (duration_ms == 0U) {
    cancel_keep_screen_on_timer(false);
    return;
  }
  if (keep_screen_on_timer_ != nullptr && keep_screen_on_timer_duration_ms_ == duration_ms) {
    return;
  }

  cancel_keep_screen_on_timer(false);
  keep_screen_on_timer_ = lv_timer_create(&AppStateMachine::keep_screen_on_timer_cb, duration_ms, this);
  if (keep_screen_on_timer_ != nullptr) {
    lv_timer_set_repeat_count(keep_screen_on_timer_, 1);
    keep_screen_on_timer_duration_ms_ = duration_ms;
  }
}

void AppStateMachine::cancel_keep_screen_on_timer(bool clear_policy) {
  if (keep_screen_on_timer_ != nullptr) {
    lv_timer_del(keep_screen_on_timer_);
    keep_screen_on_timer_ = nullptr;
  }
  keep_screen_on_timer_duration_ms_ = 0U;
  if (clear_policy) {
    const auto policy = data_center_.display_policy();
    if (policy && policy->keep_screen_on_duration_ms != 0U) {
      data_center_.set_keep_screen_on_duration_ms(0U);
    }
  }
}

void AppStateMachine::keep_screen_on_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<AppStateMachine*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }

  self->keep_screen_on_timer_ = nullptr;
  self->keep_screen_on_timer_duration_ms_ = 0U;
  self->data_center_.set_keep_screen_on_duration_ms(0U);
  self->enter_screen_off();
}

bool AppStateMachine::keep_screen_on_active() const {
  const auto policy = data_center_.display_policy();
  return power_state_ == PowerState::Running && policy && policy->keep_screen_on_duration_ms != 0U;
}

void AppStateMachine::reset_auto_screen_off_timer() {
  schedule_auto_screen_off();
}

void AppStateMachine::auto_screen_off_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<AppStateMachine*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }

  self->auto_screen_off_timer_ = nullptr;
  if (self->power_state_ == PowerState::Running) {
    self->enter_screen_off();
  }
}

}  // namespace twsim::app
