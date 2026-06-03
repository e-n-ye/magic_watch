#pragma once

#include <array>
#include <cstdint>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"

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

}  // namespace twsim::app
