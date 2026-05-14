#include "App/Input/InputIntentRouter.h"

#include <optional>

namespace twsim::app {

namespace {

bool is_home_surface_page(PageId page_id) {
  switch (page_id) {
    case PageId::Watchface:
    case PageId::HomeShortcutPayments:
    case PageId::HomeShortcutNfc:
    case PageId::HomeShortcutHealth:
    case PageId::HomeShortcutWeather:
      return true;
    default:
      return false;
  }
}

std::optional<InputCommand> from_button(const hal::ButtonSample& sample) {
  return InputCommand {
      sample.action == hal::ButtonSample::Action::LongPress ? InputAction::MainButtonLongPress
                                                            : InputAction::MainButtonShortPress,
      0,
  };
}

std::optional<InputCommand> from_crown(const hal::CrownSample& sample) {
  switch (sample.action) {
    case hal::CrownSample::Action::Press:
      return InputCommand {InputAction::CrownPress, sample.detents};
    case hal::CrownSample::Action::RotateCW:
      return InputCommand {InputAction::CrownRotateCW, sample.detents};
    case hal::CrownSample::Action::RotateCCW:
      return InputCommand {InputAction::CrownRotateCCW, sample.detents};
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
            return InputCommand {InputAction::TouchActivity, sample->value};
          case hal::TouchSample::Action::ScrollDrag:
            return InputCommand {InputAction::ScrollDrag, sample->value};
          case hal::TouchSample::Action::ScrollFlick:
            return InputCommand {InputAction::ScrollFlick, sample->value};
          case hal::TouchSample::Action::EdgeBackProgress:
            return InputCommand {InputAction::EdgeBackProgress, sample->value};
          case hal::TouchSample::Action::EdgeBackCancel:
            return InputCommand {InputAction::EdgeBackCancel, sample->value};
          case hal::TouchSample::Action::EdgeBackCommit:
            return InputCommand {is_home_surface_context() ? InputAction::HomeEdgeBackRight
                                                           : InputAction::NavigateBack,
                                 sample->value};
          case hal::TouchSample::Action::HorizontalSwipeRightCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeEdgeBackRight, sample->value};
            }
            return std::nullopt;
          case hal::TouchSample::Action::HorizontalSwipeLeftCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeLeft, sample->value};
            }
            return std::nullopt;
          case hal::TouchSample::Action::BottomEdgeCommit:
            return InputCommand {InputAction::OpenQuickSettings, sample->value};
          case hal::TouchSample::Action::TopEdgeCommit:
            return InputCommand {InputAction::OpenNotifications, sample->value};
          case hal::TouchSample::Action::RightEdgeCommit:
            if (is_home_surface_context()) {
              return InputCommand {InputAction::HomeSwipeLeft, sample->value};
            }
            return std::nullopt;
          default:
            return std::nullopt;
        }
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

}  // namespace twsim::app
