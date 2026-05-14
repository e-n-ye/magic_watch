#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "App/UI/PageBase.h"

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
  static void wreath_timer_cb(lv_timer_t* timer);
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  void advance_wreath_frame();
  void trigger_wreath_animation();

  lv_obj_t* wreath_image_ {nullptr};
  lv_obj_t* hour_label_ {nullptr};
  lv_obj_t* minute_label_ {nullptr};
  lv_obj_t* date_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* status_label_ {nullptr};
  lv_timer_t* wreath_timer_ {nullptr};
  std::size_t wreath_frame_index_ {0};
  std::size_t wreath_frames_remaining_ {0};
};

class LauncherPage : public PageBase {
 public:
  explicit LauncherPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  struct Item {
    const char* label;
    NavigationCommand command;
    const char* detail;
  };

  static void back_event_cb(lv_event_t* event);
  static void item_event_cb(lv_event_t* event);
  void bind_input();
  lv_obj_t* list_root_ {nullptr};
  std::vector<Item> items_;
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

class NotificationsPage : public PageBase {
 public:
  explicit NotificationsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  struct Item {
    const char* source;
    const char* detail;
  };

  static void close_event_cb(lv_event_t* event);
  void bind_input();

  std::array<Item, 6> items_ {{
      {"Phone", "Alice: The board prototype is cleaner now."},
      {"Calendar", "Tomorrow 09:00 project review."},
      {"Weather", "Cloudy, 24C, carry an umbrella."},
      {"Music", "Resume playback on the phone."},
      {"Health", "Stand reminder: 42 minutes seated."},
      {"System", "Simulator message list mock for v0 shell."},
  }};
  lv_obj_t* list_root_ {nullptr};
};

class QuickSettingsPage : public PageBase {
 public:
  explicit QuickSettingsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  struct ToggleState {
    const char* label;
    PageId detail_page;
    std::int16_t mode {0};
    lv_obj_t* value_label {nullptr};
  };

  static void close_event_cb(lv_event_t* event);
  static void toggle_event_cb(lv_event_t* event);
  static void toggle_long_press_event_cb(lv_event_t* event);

  void update_toggle_label(std::size_t index);

  std::array<ToggleState, 4> toggles_ {{
      {"Brightness", PageId::SettingDisplay, 1, nullptr},
      {"Sound", PageId::SettingSound, 1, nullptr},
      {"Vibration", PageId::SettingSound, 1, nullptr},
      {"Wi-Fi", PageId::SettingWifi, 0, nullptr},
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
