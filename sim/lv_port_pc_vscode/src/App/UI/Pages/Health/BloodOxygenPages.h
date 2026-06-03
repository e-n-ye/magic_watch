#pragma once

#include <array>
#include <cstdint>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"

namespace twsim::app {

class BloodOxygenAppPage : public PageBase {
 public:
  explicit BloodOxygenAppPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void entry_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void refresh_header_time();
  void schedule_crown_release();
  void stop_crown_release_timer();

  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  LvglTimerGuard crown_release_timer_;
};

class BloodOxygenSettingsPage : public PageBase {
 public:
  explicit BloodOxygenSettingsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class RowKind {
    AllDayMonitoring,
    LowOxygenReminder,
  };

  struct RowState {
    RowKind kind {RowKind::AllDayMonitoring};
    const char* title {nullptr};
    lv_obj_t* row {nullptr};
    lv_obj_t* status_label {nullptr};
    lv_obj_t* switch_track {nullptr};
  };

  static void back_event_cb(lv_event_t* event);
  static void row_event_cb(lv_event_t* event);
  static void switch_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_settings(const HealthMonitoringSettingsModel& model);
  void refresh_header_time();
  void refresh_rows();
  const char* reminder_status_text() const;
  void schedule_crown_release();
  void stop_crown_release_timer();

  HealthMonitoringSettingsModel current_settings_ {};
  std::array<RowState, 2> rows_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  LvglTimerGuard crown_release_timer_;
};

class BloodOxygenLowOxygenReminderPage : public PageBase {
 public:
  explicit BloodOxygenLowOxygenReminderPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    LowBloodOxygenReminderMode mode {LowBloodOxygenReminderMode::Off};
    const char* label {nullptr};
    lv_obj_t* row {nullptr};
    lv_obj_t* check_dot {nullptr};
  };

  static void back_event_cb(lv_event_t* event);
  static void option_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_settings(const HealthMonitoringSettingsModel& model);
  void refresh_header_time();
  void refresh_options();
  void schedule_crown_release();
  void stop_crown_release_timer();

  HealthMonitoringSettingsModel current_settings_ {};
  std::array<OptionState, 4> options_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class BloodOxygenInfoPage : public PageBase {
 public:
  explicit BloodOxygenInfoPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void refresh_header_time();
  void schedule_crown_release();
  void stop_crown_release_timer();

  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  LvglTimerGuard crown_release_timer_;
};

}  // namespace twsim::app
