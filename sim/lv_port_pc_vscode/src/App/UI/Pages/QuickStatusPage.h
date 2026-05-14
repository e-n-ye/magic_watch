#pragma once

#include "App/UI/PageBase.h"

namespace twsim::app {

class QuickStatusPage : public PageBase {
 public:
  explicit QuickStatusPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);

  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);

  lv_obj_t* detail_label_ {nullptr};
};

}  // namespace twsim::app
