#include "App/Input/InputIntentRouter.h"

#include <optional>

namespace twsim::app {

namespace {

bool is_home_surface_page(PageId page_id) {
  switch (page_id) {
    case PageId::HomeRingHost:
      return true;
    default:
      return false;
  }
}

bool is_watchface_page(PageId page_id) {
  return page_id == PageId::HomeRingHost || page_id == PageId::Watchface;
}

std::optional<InputCommand> from_button(const hal::ButtonSample& sample) {
  return InputCommand {
      sample.action == hal::ButtonSample::Action::LongPress ? InputAction::DebugOpenPowerMenu
                                                            : InputAction::DebugToggleScreenOff,
      0,
      0,
      0,
  };
}

std::optional<InputCommand> from_crown(const hal::CrownSample& sample) {
  switch (sample.action) {
    case hal::CrownSample::Action::Press:
      return InputCommand {InputAction::CrownPress, sample.detents, 0, 0};
    case hal::CrownSample::Action::RotateCW:
      return InputCommand {InputAction::CrownRotateCW, sample.detents, 0, 0};
    case hal::CrownSample::Action::RotateCCW:
      return InputCommand {InputAction::CrownRotateCCW, sample.detents, 0, 0};
    default:
      return std::nullopt;
  }
}

std::optional<InputCommand> from_debug(const hal::DebugSample& sample) {
  switch (sample.action) {
    case hal::DebugSample::Action::SimRaiseToWake:
      return InputCommand {InputAction::SimRaiseToWake, 0, 0, 0};
    case hal::DebugSample::Action::SimRaiseDismiss:
      return InputCommand {InputAction::SimRaiseDismiss, 0, 0, 0};
    default:
      return std::nullopt;
  }
}

}  // namespace

InputIntentRouter::InputIntentRouter(const PageManager& page_manager) : page_manager_(page_manager) {}

std::optional<InputCommand> InputIntentRouter::translate(const hal::Event& event) const {
  switch (event.kind) {
    case hal::EventKind::ButtonChanged:
      if (const auto* sample = std::get_if<hal::ButtonSample>(&event.payload)) {
        return from_button(*sample);
      }
      return std::nullopt;
    case hal::EventKind::CrownUpdated:
      if (const auto* sample = std::get_if<hal::CrownSample>(&event.payload)) {
        return from_crown(*sample);
      }
      return std::nullopt;
    case hal::EventKind::TouchUpdated:
      if (const auto* sample = std::get_if<hal::TouchSample>(&event.payload)) {
        switch (sample->action) {
          case hal::TouchSample::Action::TouchActivity:
            return InputCommand {InputAction::TouchActivity, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::ScrollDrag:
            return InputCommand {InputAction::ScrollDrag, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::ScrollFlick:
            return InputCommand {InputAction::ScrollFlick, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::ScrollRelease:
            return InputCommand {InputAction::ScrollRelease, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::TopEdgeProgress:
            if (is_notifications_preview_context()) {
              return InputCommand {InputAction::TopEdgeProgress, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::TopEdgeCancel:
            if (is_notifications_preview_context()) {
              return InputCommand {InputAction::TopEdgeCancel, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::BottomEdgeProgress:
            if (is_quick_settings_preview_context()) {
              return InputCommand {InputAction::BottomEdgeProgress, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::BottomEdgeCancel:
            if (is_quick_settings_preview_context()) {
              return InputCommand {InputAction::BottomEdgeCancel, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::EdgeBackProgress:
            return InputCommand {InputAction::EdgeBackProgress, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::EdgeBackCancel:
            return InputCommand {InputAction::EdgeBackCancel, sample->value, sample->x, sample->y};
          case hal::TouchSample::Action::HorizontalSwipeProgress:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeProgress, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::HorizontalSwipeCancel:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeCancel, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::EdgeBackCommit:
            return InputCommand {is_home_surface_context() ? InputAction::HomeEdgeBackRight
                                                           : InputAction::NavigateBack,
                                 sample->value,
                                 sample->x,
                                 sample->y};
          case hal::TouchSample::Action::HorizontalSwipeRightCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeEdgeBackRight, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::HorizontalSwipeLeftCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeLeft, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::BottomEdgeCommit:
            if (is_quick_settings_preview_context()) {
              return InputCommand {InputAction::OpenQuickSettings, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::TopEdgeCommit:
            if (is_notifications_preview_context()) {
              return InputCommand {InputAction::OpenNotifications, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          case hal::TouchSample::Action::RightEdgeCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeLeft, sample->value, sample->x, sample->y};
            }
            return std::nullopt;
          default:
            return std::nullopt;
        }
      }
      return std::nullopt;
    case hal::EventKind::DebugAction:
      if (const auto* sample = std::get_if<hal::DebugSample>(&event.payload)) {
        return from_debug(*sample);
      }
      return std::nullopt;
    default:
      return std::nullopt;
  }
}

bool InputIntentRouter::is_home_surface_context() const {
  const auto current_page = page_manager_.current_page_id();
  return !page_manager_.temporary_page_id() && current_page && is_home_surface_page(*current_page);
}

bool InputIntentRouter::is_watchface_context() const {
  const auto current_page = page_manager_.current_page_id();
  return !page_manager_.temporary_page_id() && current_page && is_watchface_page(*current_page);
}

bool InputIntentRouter::is_notifications_preview_context() const {
  const auto stack_top = page_manager_.stack_top_page_id();
  if (!stack_top || *stack_top != PageId::HomeRingHost) {
    return false;
  }

  const auto temporary = page_manager_.temporary_page_id();
  return !temporary || *temporary == PageId::Notifications;
}

bool InputIntentRouter::is_quick_settings_preview_context() const {
  const auto stack_top = page_manager_.stack_top_page_id();
  if (!stack_top || *stack_top != PageId::HomeRingHost) {
    return false;
  }

  const auto temporary = page_manager_.temporary_page_id();
  return !temporary || *temporary == PageId::QuickSettings;
}

}  // namespace twsim::app
