#pragma once

#include <cstdint>

#include "App/UI/PageBase.h"

namespace twsim::app {

class PowerMenuPage : public PageBase {
 public:
  explicit PowerMenuPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void action_event_cb(lv_event_t* event);
};

class ScreenOffPage : public PageBase {
 public:
  explicit ScreenOffPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  void apply_policy(const DisplayPolicyModel& policy);
  void refresh_view();
  void update_analog_preview();
  void update_info_preview();

  TimeModel time_model_ {};
  BatteryModel battery_model_ {};
  DisplayPolicyModel display_policy_ {};
  lv_obj_t* analog_root_ {nullptr};
  lv_obj_t* analog_hour_hand_ {nullptr};
  lv_obj_t* analog_minute_hand_ {nullptr};
  lv_obj_t* analog_battery_label_ {nullptr};
  lv_obj_t* info_root_ {nullptr};
  lv_obj_t* info_hour_label_ {nullptr};
  lv_obj_t* info_minute_label_ {nullptr};
  lv_obj_t* info_date_label_ {nullptr};
  lv_obj_t* info_battery_label_ {nullptr};
  lv_point_precise_t analog_hour_points_[2] {};
  lv_point_precise_t analog_minute_points_[2] {};
};

class LongBatteryWatchfacePage : public PageBase {
 public:
  explicit LongBatteryWatchfacePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void watchface_click_event_cb(lv_event_t* event);
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  void apply_steps(const StepsModel& model);
  void refresh_view();

  TimeModel time_model_ {};
  BatteryModel battery_model_ {true, false, false, 52, 0};
  StepsModel steps_model_ {true, 0};
  lv_obj_t* date_label_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* steps_label_ {nullptr};
};

class LongBatteryExitPage : public PageBase {
 public:
  explicit LongBatteryExitPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_crown_delta(std::int16_t detents);
  void refresh_progress();

  std::int16_t exit_progress_ {0};
  lv_obj_t* progress_arc_ {nullptr};
};

class PassiveShellPage : public PageBase {
 public:
  PassiveShellPage(DataCenter& data_center, PageId page_id, const char* title, const char* detail);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  PageId page_id_;
  const char* title_;
  const char* detail_;
};

}  // namespace twsim::app
