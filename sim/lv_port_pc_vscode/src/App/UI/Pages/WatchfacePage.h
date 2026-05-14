#pragma once

#include "App/UI/PageBase.h"

namespace twsim::app {

class WatchfacePage : public PageBase {
 public:
  explicit WatchfacePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void open_quick_status_event_cb(lv_event_t* event);

  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);

  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* date_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
};

}  // namespace twsim::app
