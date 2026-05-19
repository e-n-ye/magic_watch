#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "App/UI/PageBase.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

class WatchfacePage : public PageBase {
 public:
  explicit WatchfacePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  lv_obj_t* battery_icon_label_ {nullptr};
  lv_obj_t* minute_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* style_stage_ {nullptr};
  WatchfaceConfig config_ {default_watchface_config()};
  WatchfaceRenderState render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> renderer_;
};

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
    const char* label;
    NavigationCommand command;
    const char* icon_text;
    const char* icon_asset;
    lv_color_t icon_bg;
    lv_color_t icon_fg;
  };

  static void back_event_cb(lv_event_t* event);
  static void item_event_cb(lv_event_t* event);
  void bind_input();
  lv_obj_t* list_root_ {nullptr};
  std::vector<Item> items_;
};

class HomeRingHostPage : public PageBase {
 public:
  explicit HomeRingHostPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  void apply_home_ring_preview(const HomeRingPreviewModel& model);
  void layout_surfaces_for_preview(std::uint8_t base_index, std::int8_t direction);
  void set_track_x(lv_coord_t x);
  std::size_t wrap_surface_index(int index) const;

  lv_obj_t* track_ {nullptr};
  std::array<lv_obj_t*, 5> surfaces_ {{nullptr, nullptr, nullptr, nullptr, nullptr}};
  lv_obj_t* battery_icon_label_ {nullptr};
  lv_obj_t* minute_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* style_stage_ {nullptr};
  lv_obj_t* pager_root_ {nullptr};
  lv_obj_t* pager_active_dot_ {nullptr};
  std::array<lv_obj_t*, 4> pager_dots_ {{nullptr, nullptr, nullptr, nullptr}};
  WatchfaceConfig config_ {default_watchface_config()};
  WatchfaceRenderState render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> renderer_;
  std::size_t settled_surface_index_ {0};
};

class HomeShortcutPage : public PageBase {
 public:
  struct Metric {
    const char* label;
    const char* value;
    const char* detail;
  };

  struct Config {
    PageId page_id;
    const char* orbit_label;
    const char* title;
    const char* subtitle;
    const char* focus_label;
    const char* focus_value;
    const char* focus_detail;
    std::array<Metric, 4> metrics;
  };

  HomeShortcutPage(DataCenter& data_center, Config config);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  Config config_;
};

class WeatherShortcutPage : public PageBase {
 public:
  explicit WeatherShortcutPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;
};

class PaymentsShortcutPage : public PageBase {
 public:
  explicit PaymentsShortcutPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;
};

class NfcShortcutPage : public PageBase {
 public:
  explicit NfcShortcutPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;
};

class HealthShortcutPage : public PageBase {
 public:
  explicit HealthShortcutPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;
};

class NotificationsPage : public PageBase {
 public:
  explicit NotificationsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void close_event_cb(lv_event_t* event);
  static void clear_event_cb(lv_event_t* event);
  void bind_input();
  void bind_notifications();
  void bind_backdrop();
  void refresh_content();
  void refresh_backdrop();
  void apply_backdrop_time(const TimeModel& model);
  void apply_backdrop_battery(const BatteryModel& model);
  void set_open_preview_progress(lv_coord_t progress, bool animated);
  void set_close_drag_offset(lv_coord_t offset, bool animated);
  void stop_preview_close_timer();
  static void preview_close_timer_cb(lv_timer_t* timer);
  void finish_drag_close(std::int16_t release_delta, bool flick_close);
  bool is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const;
  bool should_capture_shell_drag(const InputCommand& command) const;

  lv_obj_t* backdrop_root_ {nullptr};
  lv_obj_t* backdrop_battery_icon_label_ {nullptr};
  lv_obj_t* backdrop_battery_label_ {nullptr};
  lv_obj_t* backdrop_minute_label_ {nullptr};
  lv_obj_t* backdrop_style_stage_ {nullptr};
  lv_obj_t* list_root_ {nullptr};
  lv_obj_t* empty_state_ {nullptr};
  lv_obj_t* clear_button_ {nullptr};
  lv_obj_t* drag_handle_ {nullptr};
  lv_obj_t* sheet_container_ {nullptr};
  lv_coord_t shell_drag_offset_ {0};
  lv_coord_t open_preview_progress_ {0};
  bool shell_drag_active_ {false};
  lv_timer_t* preview_close_timer_ {nullptr};
  WatchfaceConfig backdrop_config_ {default_watchface_config()};
  WatchfaceRenderState backdrop_render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> backdrop_renderer_;
};

class NotificationWakePage : public PageBase {
 public:
  explicit NotificationWakePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void dismiss_event_cb(lv_event_t* event);
  static void timeout_cb(lv_timer_t* timer);
  void bind_notifications();
  void refresh_content();
  void start_auto_close_timer();
  void stop_auto_close_timer();

  lv_obj_t* icon_container_ {nullptr};
  lv_obj_t* icon_image_ {nullptr};
  lv_obj_t* icon_label_ {nullptr};
  lv_obj_t* source_label_ {nullptr};
  lv_obj_t* title_label_ {nullptr};
  lv_obj_t* body_label_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* dismiss_button_ {nullptr};
  std::string current_notification_id_;
  lv_timer_t* auto_close_timer_ {nullptr};
};

class QuickSettingsPage : public PageBase {
 public:
  explicit QuickSettingsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class ToggleKind {
    DoNotDisturb,
    Silent,
    RaiseToWake,
    OpenSettings,
    Flashlight,
    FindPhone,
    Drain,
    LongBattery,
    AodFiveMinutes,
    NotifyWake,
  };

  struct ToggleState {
    const char* label;
    const char* icon_text;
    ToggleKind kind;
    PageId detail_page;
    std::int16_t mode {0};
    lv_obj_t* button {nullptr};
    lv_obj_t* icon_label {nullptr};
  };

  static void close_event_cb(lv_event_t* event);
  static void toggle_event_cb(lv_event_t* event);
  static void toggle_long_press_event_cb(lv_event_t* event);
  static void toggle_release_event_cb(lv_event_t* event);
  static void toggle_press_lost_event_cb(lv_event_t* event);
  static void toast_timeout_cb(lv_timer_t* timer);

  void bind_input();
  void bind_display_policy();
  void bind_backdrop();
  void refresh_backdrop();
  void apply_backdrop_time(const TimeModel& model);
  void apply_backdrop_battery(const BatteryModel& model);
  void apply_toggle_visual(std::size_t index);
  void show_toggle_toast(const char* text);
  void hide_toggle_toast();
  void stop_toast_timer();
  void set_open_preview_progress(lv_coord_t progress, bool animated);
  void set_close_drag_offset(lv_coord_t offset, bool animated);
  void stop_preview_close_timer();
  static void preview_close_timer_cb(lv_timer_t* timer);
  void finish_drag_close(std::int16_t release_delta, bool flick_close);
  bool is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const;
  bool should_capture_shell_drag(const InputCommand& command) const;

  lv_obj_t* backdrop_root_ {nullptr};
  lv_obj_t* backdrop_battery_icon_label_ {nullptr};
  lv_obj_t* backdrop_battery_label_ {nullptr};
  lv_obj_t* backdrop_minute_label_ {nullptr};
  lv_obj_t* backdrop_style_stage_ {nullptr};
  lv_obj_t* sheet_container_ {nullptr};
  lv_obj_t* drag_handle_ {nullptr};
  lv_obj_t* toast_container_ {nullptr};
  lv_obj_t* toast_label_ {nullptr};
  lv_coord_t shell_drag_offset_ {0};
  lv_coord_t open_preview_progress_ {0};
  bool shell_drag_active_ {false};
  bool suppress_next_click_ {false};
  std::chrono::steady_clock::time_point suppress_click_deadline_ {};
  lv_obj_t* long_press_source_button_ {nullptr};
  lv_timer_t* preview_close_timer_ {nullptr};
  lv_timer_t* toast_timer_ {nullptr};
  WatchfaceConfig backdrop_config_ {default_watchface_config()};
  WatchfaceRenderState backdrop_render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> backdrop_renderer_;
  std::array<ToggleState, 9> toggles_ {{
      {"勿扰模式", LV_SYMBOL_BELL, ToggleKind::DoNotDisturb, PageId::SettingDoNotDisturb, 0, nullptr, nullptr},
      {"静音模式", LV_SYMBOL_MUTE, ToggleKind::Silent, PageId::SettingSound, 1, nullptr, nullptr},
      {"翻腕亮屏", LV_SYMBOL_EYE_OPEN, ToggleKind::RaiseToWake, PageId::SettingDisplayRaiseToWake, 1, nullptr, nullptr},
      {"设置界面", LV_SYMBOL_SETTINGS, ToggleKind::OpenSettings, PageId::SettingsHome, 0, nullptr, nullptr},
      {"手电筒", LV_SYMBOL_CHARGE, ToggleKind::Flashlight, PageId::SettingDisplay, 0, nullptr, nullptr},
      {"寻找手机", LV_SYMBOL_CALL, ToggleKind::FindPhone, PageId::SettingBluetooth, 0, nullptr, nullptr},
      {"排水模式", LV_SYMBOL_REFRESH, ToggleKind::Drain, PageId::SettingDisplay, 1, nullptr, nullptr},
      {"长续航模式", LV_SYMBOL_BATTERY_FULL, ToggleKind::LongBattery, PageId::SettingBattery, 0, nullptr, nullptr},
      {"持续亮屏", LV_SYMBOL_EYE_CLOSE, ToggleKind::AodFiveMinutes, PageId::SettingDisplayKeepScreenOn, 0, nullptr, nullptr},
  }};
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
