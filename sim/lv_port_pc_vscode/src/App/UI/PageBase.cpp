#include "App/UI/PageBase.h"

namespace twsim::app {

PageBase::PageBase(DataCenter& data_center) : data_center_(data_center) {}

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

}  // namespace twsim::app
