#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

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
    std::optional<PageId> detail_page;
    std::int16_t mode {0};
    lv_obj_t* button {nullptr};
    lv_obj_t* icon_label {nullptr};
  };

  static void close_event_cb(lv_event_t* event);
  static void toggle_event_cb(lv_event_t* event);
  static void toggle_long_press_event_cb(lv_event_t* event);
  static void toggle_release_event_cb(lv_event_t* event);
  static void toggle_press_lost_event_cb(lv_event_t* event);
  static void long_battery_confirm_event_cb(lv_event_t* event);
  static void toast_timeout_cb(lv_timer_t* timer);

  void bind_input();
  void bind_display_policy();
  void bind_backdrop();
  void refresh_backdrop();
  void apply_backdrop_time(const TimeModel& model);
  void apply_backdrop_battery(const BatteryModel& model);
  void apply_toggle_visual(std::size_t index);
  bool is_toggle_active(const ToggleState& toggle) const;
  void show_long_battery_confirm();
  void hide_long_battery_confirm();
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
  lv_obj_t* long_battery_confirm_overlay_ {nullptr};
  lv_coord_t shell_drag_offset_ {0};
  lv_coord_t open_preview_progress_ {0};
  bool shell_drag_active_ {false};
  bool suppress_next_click_ {false};
  std::chrono::steady_clock::time_point suppress_click_deadline_ {};
  lv_obj_t* long_press_source_button_ {nullptr};
  LvglTimerGuard preview_close_timer_;
  LvglTimerGuard toast_timer_;
  WatchfaceConfig backdrop_config_ {default_watchface_config()};
  WatchfaceRenderState backdrop_render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> backdrop_renderer_;
  std::array<ToggleState, 9> toggles_ {{
      {"勿扰模式", LV_SYMBOL_BELL, ToggleKind::DoNotDisturb, PageId::SettingDoNotDisturb, 0, nullptr, nullptr},
      {"静音模式", LV_SYMBOL_MUTE, ToggleKind::Silent, PageId::SettingSound, 1, nullptr, nullptr},
      {"翻腕亮屏", LV_SYMBOL_EYE_OPEN, ToggleKind::RaiseToWake, PageId::SettingDisplayRaiseToWake, 1, nullptr, nullptr},
      {"设置界面", LV_SYMBOL_SETTINGS, ToggleKind::OpenSettings, PageId::SettingsHome, 0, nullptr, nullptr},
      {"手电筒", LV_SYMBOL_CHARGE, ToggleKind::Flashlight, PageId::SettingDisplay, 0, nullptr, nullptr},
      {"寻找手机", LV_SYMBOL_CALL, ToggleKind::FindPhone, std::nullopt, 0, nullptr, nullptr},
      {"排水模式", LV_SYMBOL_REFRESH, ToggleKind::Drain, PageId::SettingDisplay, 1, nullptr, nullptr},
      {"长续航模式", LV_SYMBOL_BATTERY_FULL, ToggleKind::LongBattery, PageId::SettingBattery, 0, nullptr, nullptr},
      {"持续亮屏", LV_SYMBOL_EYE_CLOSE, ToggleKind::AodFiveMinutes, PageId::SettingDisplayKeepScreenOn, 0, nullptr, nullptr},
  }};
};

}  // namespace twsim::app
