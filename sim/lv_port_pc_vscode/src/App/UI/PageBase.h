#pragma once

#include <vector>

#include "App/Common/DataCenter.h"
#include "lvgl/lvgl.h"

namespace twsim::app {

class PageBase {
 public:
  explicit PageBase(DataCenter& data_center);
  virtual ~PageBase() = default;

  virtual PageId id() const = 0;
  virtual const char* name() const = 0;

  bool ensure_created();
  lv_obj_t* root() const;

  virtual void on_will_appear();
  virtual void on_will_disappear();

 protected:
  virtual lv_obj_t* build() = 0;
  void track(EventBus::Subscription subscription);
  void request_navigation(const NavigationCommand& command);

  DataCenter& data_center_;
  lv_obj_t* root_ {nullptr};

 private:
  bool created_ {false};
  std::vector<EventBus::Subscription> subscriptions_;
};

}  // namespace twsim::app
