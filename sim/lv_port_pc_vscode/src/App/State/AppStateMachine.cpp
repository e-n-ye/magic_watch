#include "App/State/AppStateMachine.h"

#include <algorithm>
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
      return PageTransition::MoveDown;
    case PageId::QuickSettings:
      return PageTransition::MoveUp;
    case PageId::PowerMenu:
    default:
      return PageTransition::Fade;
  }
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
}

bool AppStateMachine::start() {
  if (!page_manager_.set_root(PageId::Watchface, PageTransition::None)) {
    return false;
  }

  power_state_ = PowerState::Running;
  shell_surface_ = ShellSurface::None;
  home_surface_index_ = 0;
  return true;
}

void AppStateMachine::handle_event(const Event& event) {
  if (const auto* command = std::get_if<NavigationCommand>(&event.payload)) {
    handle_navigation(*command);
    return;
  }

  if (const auto* input = std::get_if<InputCommand>(&event.payload)) {
    handle_input(*input);
  }
}

void AppStateMachine::handle_navigation(const NavigationCommand& command) {
  bool navigation_ok = false;

  switch (command.action) {
    case NavigationAction::SetRoot:
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

  switch (command.action) {
    case InputAction::MainButtonShortPress:
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
    case InputAction::MainButtonLongPress:
      overlay.hide();
      if (power_state_ == PowerState::PoweredOff) {
        boot_to_home();
        return;
      }
      open_shell_surface(ShellSurface::PowerMenu);
      return;
    case InputAction::EdgeBackProgress:
      if (can_navigate_back()) {
        overlay.show_progress(command.value);
      }
      return;
    case InputAction::EdgeBackCancel:
      overlay.hide();
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
        open_shell_surface(ShellSurface::Notifications);
      }
      return;
    case InputAction::OpenQuickSettings:
      if (power_state_ == PowerState::Running) {
        open_shell_surface(ShellSurface::QuickSettings);
      }
      return;
    case InputAction::CrownPress:
      overlay.hide();
      if (power_state_ == PowerState::PoweredOff) {
        return;
      }
      if (power_state_ == PowerState::ScreenOff) {
        return_home();
        return;
      }
      if (page_manager_.temporary_page_id() || page_manager_.stack_depth() > 1) {
        return_home();
        return;
      }
      if (is_current_home_surface()) {
        launch_app(PageId::Launcher);
      }
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
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        navigate_home_surface(1);
      }
      return;
    case InputAction::CrownRotateCCW:
      if (power_state_ == PowerState::Running && is_current_home_surface()) {
        navigate_home_surface(-1);
      }
      return;
    case InputAction::ScrollDrag:
    case InputAction::ScrollFlick:
    default:
      return;
  }
}

void AppStateMachine::boot_to_home(PageTransition transition) {
  if (page_manager_.set_root(PageId::Watchface, transition)) {
    power_state_ = PowerState::Running;
    shell_surface_ = ShellSurface::None;
    home_surface_index_ = 0;
  }
}

void AppStateMachine::return_home(PageTransition transition) {
  boot_to_home(transition);
}

void AppStateMachine::enter_screen_off() {
  if (page_manager_.set_root(PageId::ScreenOff, PageTransition::Fade)) {
    power_state_ = PowerState::ScreenOff;
    shell_surface_ = ShellSurface::None;
  }
}

void AppStateMachine::enter_powered_off() {
  if (page_manager_.set_root(PageId::PoweredOff, PageTransition::Fade)) {
    power_state_ = PowerState::PoweredOff;
    shell_surface_ = ShellSurface::None;
  }
}

void AppStateMachine::open_shell_surface(ShellSurface surface) {
  if (power_state_ == PowerState::PoweredOff) {
    return;
  }
  if (power_state_ == PowerState::ScreenOff && surface != ShellSurface::PowerMenu) {
    return;
  }

  PageId target = PageId::Watchface;
  switch (surface) {
    case ShellSurface::Notifications:
      target = PageId::Notifications;
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

  if (page_manager_.dismiss_temporary(close_transition_for(*temporary))) {
    shell_surface_ = ShellSurface::None;
  }
}

void AppStateMachine::launch_app(PageId target) {
  if (power_state_ == PowerState::PoweredOff) {
    return;
  }

  if (!page_manager_.set_root(PageId::Watchface, PageTransition::None)) {
    return;
  }

  shell_surface_ = ShellSurface::None;
  power_state_ = PowerState::Running;
  home_surface_index_ = 0;

  if (target == PageId::Watchface) {
    return;
  }

  page_manager_.push(target, PageTransition::MoveLeft);
}

bool AppStateMachine::is_home_surface_page(PageId page_id) const {
  return std::find(home_surfaces_.begin(), home_surfaces_.end(), page_id) != home_surfaces_.end();
}

bool AppStateMachine::is_current_home_surface() const {
  const auto stack_top = page_manager_.stack_top_page_id();
  return !page_manager_.temporary_page_id() && page_manager_.stack_depth() == 1 && stack_top &&
         is_home_surface_page(*stack_top);
}

void AppStateMachine::navigate_home_surface(int delta) {
  if (home_surfaces_.empty()) {
    return;
  }

  const auto size = static_cast<int>(home_surfaces_.size());
  const auto current = static_cast<int>(home_surface_index_);
  int next = (current + delta) % size;
  if (next < 0) {
    next += size;
  }

  show_home_surface(static_cast<std::size_t>(next), delta >= 0 ? PageTransition::MoveLeft : PageTransition::MoveRight);
}

void AppStateMachine::show_home_surface(std::size_t index, PageTransition transition) {
  if (index >= home_surfaces_.size()) {
    return;
  }

  if (page_manager_.set_root(home_surfaces_[index], transition)) {
    home_surface_index_ = index;
    shell_surface_ = ShellSurface::None;
    power_state_ = PowerState::Running;
  }
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

}  // namespace twsim::app
