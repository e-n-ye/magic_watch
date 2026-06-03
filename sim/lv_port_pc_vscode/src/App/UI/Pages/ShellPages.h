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
#include "App/UI/Pages/Shell/Home/ShellHomePages.h"
#include "App/UI/Pages/Shell/Notifications/ShellNotificationPages.h"
#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPages.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

class LauncherPage : public PageBase {
 public:
  explicit LauncherPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct Item {
    const char* section_title;
    const char* label;
    NavigationCommand command;
    const char* icon_text;
    const char* icon_asset;
    lv_color_t icon_bg;
    lv_color_t icon_fg;
  };

  static void back_event_cb(lv_event_t* event);
  static void item_event_cb(lv_event_t* event);
  void rebuild_layout_if_needed(bool force = false);
  void build_multi_column_layout(lv_obj_t* parent);
  void build_list_layout(lv_obj_t* parent);
  void build_categorized_layout(lv_obj_t* parent);
  void apply_crown_drag(bool forward, std::int16_t detents);
  void schedule_crown_release();
  void stop_crown_release_timer();
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  lv_obj_t* list_root_ {nullptr};
  std::vector<Item> items_;
  LauncherLayoutMode current_layout_mode_ {LauncherLayoutMode::Categorized};
  lv_timer_t* crown_release_timer_ {nullptr};
};

class WeatherAppPage : public PageBase {
 public:
  explicit WeatherAppPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  void bind_input();

  lv_obj_t* scroll_root_ {nullptr};
  lv_coord_t page_pitch_ {0};
};

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

