#include "App/UI/PageBase.h"

namespace twsim::app {

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
  return std::chrono::steady_clock::now() < suppress_click_until_;
}

}  // namespace twsim::app
