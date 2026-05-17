#include "App/UI/PageBase.h"

namespace twsim::app {

namespace {

std::chrono::steady_clock::time_point& global_suppress_click_until() {
  static std::chrono::steady_clock::time_point value {};
  return value;
}

}  // namespace

PageBase::PageBase(DataCenter& data_center) : data_center_(data_center) {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::EdgeBackProgress:
                                   case InputAction::EdgeBackCancel:
                                   case InputAction::NavigateBack:
                                   case InputAction::HomeSwipeProgress:
                                   case InputAction::HomeSwipeCancel:
                                   case InputAction::HomeEdgeBackRight:
                                   case InputAction::HomeSwipeLeft:
                                     suppress_click_until_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(220);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

bool PageBase::ensure_created() {
  if (created_) {
    return true;
  }

  root_ = build();
  created_ = root_ != nullptr;
  return created_;
}

lv_obj_t* PageBase::root() const {
  return root_;
}

void PageBase::on_will_appear() {}

void PageBase::on_will_disappear() {}

void PageBase::track(EventBus::Subscription subscription) {
  subscriptions_.push_back(std::move(subscription));
}

void PageBase::request_navigation(const NavigationCommand& command) {
  data_center_.publish_navigation(command);
}

bool PageBase::should_ignore_click() const {
  const auto now = std::chrono::steady_clock::now();
  return now < suppress_click_until_ || now < global_suppress_click_until();
}

void PageBase::suppress_click_for(std::chrono::milliseconds duration) {
  suppress_click_until_ = std::chrono::steady_clock::now() + duration;
}

void PageBase::suppress_global_clicks_for(std::chrono::milliseconds duration) {
  global_suppress_click_until() = std::chrono::steady_clock::now() + duration;
}

}  // namespace twsim::app
