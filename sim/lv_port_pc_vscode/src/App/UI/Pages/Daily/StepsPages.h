#pragma once

#include <cstdint>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"

namespace twsim::app {

class StepsAppPage : public PageBase {
 public:
  explicit StepsAppPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void data_info_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_steps(const StepsModel& model);
  void refresh_steps_view();
  void schedule_crown_release();
  void stop_crown_release_timer();

  StepsModel steps_model_ {true, 0};
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* steps_arc_ {nullptr};
  lv_obj_t* steps_metric_value_label_ {nullptr};
  lv_obj_t* steps_metric_target_label_ {nullptr};
  lv_obj_t* steps_card_value_label_ {nullptr};
  lv_obj_t* steps_card_unit_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class StepsDataInfoPage : public PageBase {
 public:
  explicit StepsDataInfoPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void schedule_crown_release();
  void stop_crown_release_timer();

  lv_obj_t* scroll_root_ {nullptr};
  LvglTimerGuard crown_release_timer_;
};

}  // namespace twsim::app
