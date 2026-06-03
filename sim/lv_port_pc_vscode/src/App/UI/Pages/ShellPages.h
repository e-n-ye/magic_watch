#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"
#include "App/UI/Pages/Daily/StepsPages.h"
#include "App/UI/Pages/Daily/WeatherPages.h"
#include "App/UI/Pages/Shell/Home/ShellHomePages.h"
#include "App/UI/Pages/Shell/Launcher/ShellLauncherPages.h"
#include "App/UI/Pages/Shell/Notifications/ShellNotificationPages.h"
#include "App/UI/Pages/Shell/Power/ShellPowerPages.h"
#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPages.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

class SleepAppPage : public PageBase {
 public:
  explicit SleepAppPage(DataCenter& data_center);

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
  lv_timer_t* crown_release_timer_ {nullptr};
};

class SleepSettingsPage : public PageBase {
 public:
  explicit SleepSettingsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class RowKind {
    HighPrecisionSleep,
    SleepBreathingQuality,
  };

  struct RowState {
    RowKind kind {RowKind::HighPrecisionSleep};
    const char* title {nullptr};
    lv_obj_t* row {nullptr};
    lv_obj_t* status_label {nullptr};
  };

  static void back_event_cb(lv_event_t* event);
  static void row_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_settings(const HealthMonitoringSettingsModel& model);
  void refresh_header_time();
  void refresh_rows();
  bool row_enabled(RowKind kind) const;
  void schedule_crown_release();
  void stop_crown_release_timer();

  HealthMonitoringSettingsModel current_settings_ {};
  std::array<RowState, 2> rows_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class SleepMonitoringDetailPage : public PageBase {
 public:
  enum class SettingKind {
    HighPrecisionSleep,
    SleepBreathingQuality,
  };

  SleepMonitoringDetailPage(DataCenter& data_center,
                            PageId page_id,
                            const char* title,
                            const char* body,
                            SettingKind kind);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void switch_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_enabled(bool enabled);
  void refresh_header_time();
  void schedule_crown_release();
  void stop_crown_release_timer();
  bool current_enabled() const;
  void publish_enabled(bool enabled);

  PageId page_id_;
  const char* title_;
  const char* body_;
  SettingKind kind_ {SettingKind::HighPrecisionSleep};
  bool enabled_ {false};
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* switch_track_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class SleepInfoPage : public PageBase {
 public:
  explicit SleepInfoPage(DataCenter& data_center);

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

class HeartRateAppPage : public PageBase {
 public:
  explicit HeartRateAppPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class EntryKind {
    Resting30Days,
    Settings,
    Info,
  };

  static void entry_event_cb(lv_event_t* event);
  static void wear_prompt_event_cb(lv_event_t* event);
  static void measurement_timer_cb(lv_timer_t* timer);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void refresh_header_time();
  void start_entry_measurement();
  void complete_entry_measurement();
  void show_measurement_stage();
  void show_result_stage();
  void show_wear_prompt();
  void hide_wear_prompt();
  void stop_measurement_timer();
  void schedule_crown_release();
  void stop_crown_release_timer();

  bool measurement_completed_ {false};
  bool wear_prompt_shown_ {false};
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* measuring_stage_ {nullptr};
  lv_obj_t* result_stage_ {nullptr};
  lv_obj_t* wear_prompt_overlay_ {nullptr};
  lv_timer_t* measurement_timer_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateResting30DaysPage : public PageBase {
 public:
  explicit HeartRateResting30DaysPage(DataCenter& data_center);

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
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateSettingsPage : public PageBase {
 public:
  explicit HeartRateSettingsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class RowKind {
    AllDayMonitoring,
    HeartHealthMonitoring,
    HighReminder,
    LowReminder,
  };

  struct RowState {
    RowKind kind {RowKind::AllDayMonitoring};
    const char* title {nullptr};
    lv_obj_t* row {nullptr};
    lv_obj_t* status_label {nullptr};
  };

  static void back_event_cb(lv_event_t* event);
  static void row_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_settings(const HealthMonitoringSettingsModel& model);
  void refresh_header_time();
  void refresh_rows();
  const char* row_status_text(RowKind kind) const;
  void schedule_crown_release();
  void stop_crown_release_timer();

  HealthMonitoringSettingsModel current_settings_ {};
  std::array<RowState, 4> rows_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateAllDayMonitoringPage : public PageBase {
 public:
  explicit HeartRateAllDayMonitoringPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    HeartRateAllDayMonitoringMode mode {HeartRateAllDayMonitoringMode::Off};
    const char* label {nullptr};
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
  std::array<OptionState, 5> options_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateHeartHealthMonitoringPage : public PageBase {
 public:
  explicit HeartRateHeartHealthMonitoringPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void switch_event_cb(lv_event_t* event);
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  void apply_crown_drag(bool forward, std::int16_t detents);
  void apply_settings(const HealthMonitoringSettingsModel& model);
  void refresh_header_time();
  void refresh_switch();
  void schedule_crown_release();
  void stop_crown_release_timer();

  HealthMonitoringSettingsModel current_settings_ {};
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* switch_track_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateHighReminderPage : public PageBase {
 public:
  explicit HeartRateHighReminderPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    HeartRateHighReminderMode mode {HeartRateHighReminderMode::Off};
    const char* label {nullptr};
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
  std::array<OptionState, 7> options_;
  lv_obj_t* scroll_root_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class HeartRateLowReminderPage : public PageBase {
 public:
  explicit HeartRateLowReminderPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    HeartRateLowReminderMode mode {HeartRateLowReminderMode::Off};
    const char* label {nullptr};
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

class HeartRateInfoPage : public PageBase {
 public:
  explicit HeartRateInfoPage(DataCenter& data_center);

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
  lv_timer_t* crown_release_timer_ {nullptr};
};

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
  lv_timer_t* crown_release_timer_ {nullptr};
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
  lv_timer_t* crown_release_timer_ {nullptr};
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
  lv_timer_t* crown_release_timer_ {nullptr};
};

}  // namespace twsim::app

