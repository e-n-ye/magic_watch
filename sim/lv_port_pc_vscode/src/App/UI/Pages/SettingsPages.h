#pragma once

#include <array>
#include <cstdint>

#include "App/UI/PageBase.h"

namespace twsim::app {

class SettingsPageBase : public PageBase {
 public:
  SettingsPageBase(DataCenter& data_center, PageId page_id, const char* title, bool show_back_button = false);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* create_page_root();
  void build_header(lv_obj_t* root);
  void apply_time(const TimeModel& model);
  const char* title_text() const;

 private:
  static void back_event_cb(lv_event_t* event);

  PageId page_id_;
  const char* title_;
  bool show_back_button_ {true};
  lv_obj_t* time_label_ {nullptr};
};

class SettingsHomePage : public SettingsPageBase {
 public:
  explicit SettingsHomePage(DataCenter& data_center);

 protected:
  lv_obj_t* build() override;

 private:
  struct Entry {
    const char* icon_text;
    std::uint32_t icon_bg;
    const char* label;
    PageId target;
  };

  static void item_event_cb(lv_event_t* event);

  std::array<Entry, 7> entries_;
};

class SettingsPlaceholderPage : public SettingsPageBase {
 public:
  SettingsPlaceholderPage(DataCenter& data_center, PageId page_id, const char* title, const char* detail);

 protected:
  lv_obj_t* build() override;

 private:
  const char* detail_;
};

class DisplaySettingsPage : public SettingsPageBase {
 public:
  explicit DisplaySettingsPage(DataCenter& data_center);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  enum class RowAction {
    OpenBrightness,
    OpenScreenTimeout,
    OpenRaiseToWake,
    OpenKeepScreenOn,
    ToggleTapToWake,
    OpenScreenOffDisplay,
    OpenScreenOffStyle,
    ToggleCoverToSleep,
  };

  struct DisplayRowState {
    RowAction action;
    const char* label;
    lv_obj_t* button {nullptr};
    lv_obj_t* title_label {nullptr};
    lv_obj_t* detail_label {nullptr};
    lv_obj_t* switch_track {nullptr};
  };

  static void row_event_cb(lv_event_t* event);
  void refresh_from_policy();

  std::array<DisplayRowState, 8> rows_;
  lv_obj_t* note_body_ {nullptr};
};

class DisplayBrightnessPage : public SettingsPageBase {
 public:
  explicit DisplayBrightnessPage(DataCenter& data_center);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void row_event_cb(lv_event_t* event);

  void refresh_from_policy();

  lv_obj_t* auto_switch_track_ {nullptr};
  lv_obj_t* manual_row_ {nullptr};
  lv_obj_t* manual_detail_label_ {nullptr};
};

class DisplayManualBrightnessPage : public SettingsPageBase {
 public:
  explicit DisplayManualBrightnessPage(DataCenter& data_center);
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void slider_event_cb(lv_event_t* event);

  void reload_from_policy();
  void apply_preview_value(std::uint8_t level);

  lv_obj_t* slider_ {nullptr};
  lv_obj_t* value_label_ {nullptr};
  std::uint8_t pending_level_ {60};
  bool dirty_ {false};
  bool suppress_slider_event_ {false};
};

class DisplayTimeoutPage : public SettingsPageBase {
 public:
  explicit DisplayTimeoutPage(DataCenter& data_center);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct TimeoutOptionState {
    std::uint32_t timeout_ms;
    const char* label;
    lv_obj_t* button {nullptr};
    lv_obj_t* check_label {nullptr};
  };

  static void option_event_cb(lv_event_t* event);

  void refresh_selection();

  std::array<TimeoutOptionState, 5> options_;
};

class DisplayRaiseToWakePage : public SettingsPageBase {
 public:
  explicit DisplayRaiseToWakePage(DataCenter& data_center);
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct RaiseOptionState {
    RaiseToWakeMode mode {RaiseToWakeMode::Off};
    const char* label {nullptr};
    lv_obj_t* button {nullptr};
    lv_obj_t* title_label {nullptr};
    lv_obj_t* check_label {nullptr};
    lv_obj_t* start_row {nullptr};
    lv_obj_t* start_title_label {nullptr};
    lv_obj_t* start_value_label {nullptr};
    lv_obj_t* end_row {nullptr};
    lv_obj_t* end_title_label {nullptr};
    lv_obj_t* end_value_label {nullptr};
  };

  static void option_event_cb(lv_event_t* event);
  static void conflict_event_cb(lv_event_t* event);

  void refresh_selection();
  void show_conflict_overlay(RaiseToWakeMode mode);
  void hide_conflict_overlay();
  void apply_pending_mode(bool use_screen_off_watchface);

  std::array<RaiseOptionState, 3> options_;
  lv_obj_t* conflict_overlay_ {nullptr};
  RaiseToWakeMode pending_mode_ {RaiseToWakeMode::Off};
};

class DisplayKeepScreenOnPage : public SettingsPageBase {
 public:
  explicit DisplayKeepScreenOnPage(DataCenter& data_center);
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct KeepScreenOnOptionState {
    std::uint32_t duration_ms {0};
    const char* label {nullptr};
    lv_obj_t* button {nullptr};
    lv_obj_t* check_label {nullptr};
  };

  static void option_event_cb(lv_event_t* event);
  static void confirm_event_cb(lv_event_t* event);

  void refresh_selection();
  void show_confirm_overlay(std::uint32_t duration_ms);
  void hide_confirm_overlay();
  void apply_pending_duration();

  std::array<KeepScreenOnOptionState, 5> options_;
  lv_obj_t* overlay_ {nullptr};
  std::uint32_t pending_duration_ms_ {0};
};

class DisplayScreenOffDisplayPage : public SettingsPageBase {
 public:
  explicit DisplayScreenOffDisplayPage(DataCenter& data_center);
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    ScreenOffDisplayMode mode {ScreenOffDisplayMode::Off};
    const char* label {nullptr};
    lv_obj_t* button {nullptr};
    lv_obj_t* title_label {nullptr};
    lv_obj_t* check_label {nullptr};
    lv_obj_t* start_row {nullptr};
    lv_obj_t* start_title_label {nullptr};
    lv_obj_t* start_value_label {nullptr};
    lv_obj_t* end_row {nullptr};
    lv_obj_t* end_title_label {nullptr};
    lv_obj_t* end_value_label {nullptr};
  };

  static void option_event_cb(lv_event_t* event);
  static void confirm_event_cb(lv_event_t* event);

  void refresh_selection();
  void show_confirm_overlay(ScreenOffDisplayMode mode);
  void hide_confirm_overlay();
  void show_conflict_overlay();
  void hide_conflict_overlay();
  void apply_pending_mode();
  void finalize_pending_mode(bool use_screen_off_watchface);

  std::array<OptionState, 3> options_;
  lv_obj_t* overlay_ {nullptr};
  lv_obj_t* conflict_overlay_ {nullptr};
  ScreenOffDisplayMode pending_mode_ {ScreenOffDisplayMode::Off};
};

class DisplayScreenOffDisplaySchedulePage : public SettingsPageBase {
 public:
  explicit DisplayScreenOffDisplaySchedulePage(DataCenter& data_center);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct RowState {
    lv_obj_t* button {nullptr};
    lv_obj_t* title_label {nullptr};
    lv_obj_t* value_label {nullptr};
  };

  static void row_event_cb(lv_event_t* event);
  void refresh_from_policy();

  std::array<RowState, 2> rows_;
};

class DisplayScreenOffStylePage : public SettingsPageBase {
 public:
  explicit DisplayScreenOffStylePage(DataCenter& data_center);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct OptionState {
    ScreenOffStyleId style_id {ScreenOffStyleId::AnalogHands};
    const char* title {nullptr};
    lv_obj_t* card {nullptr};
  };

  static void option_event_cb(lv_event_t* event);
  static void scroll_event_cb(lv_event_t* event);
  static void confirm_event_cb(lv_event_t* event);

  void sync_pending_from_policy();
  void refresh_selection();
  void update_pending_from_scroll();
  void scroll_to_pending(bool animated);

  std::array<OptionState, 2> options_ {{
      {ScreenOffStyleId::AnalogHands, nullptr, nullptr},
      {ScreenOffStyleId::InfoDigits, nullptr, nullptr},
  }};
  lv_obj_t* carousel_ {nullptr};
  lv_obj_t* style_name_label_ {nullptr};
  lv_obj_t* confirm_button_ {nullptr};
  ScreenOffStyleId pending_style_id_ {ScreenOffStyleId::AnalogHands};
};

class DisplayRaiseToWakeTimePage : public SettingsPageBase {
 public:
  DisplayRaiseToWakeTimePage(DataCenter& data_center, PageId page_id, const char* title, bool edit_start);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void roller_event_cb(lv_event_t* event);
  static void confirm_event_cb(lv_event_t* event);

  void reload_from_policy();
  void update_preview();
  void save_and_exit();

  bool edit_start_ {true};
  lv_obj_t* hour_roller_ {nullptr};
  lv_obj_t* minute_roller_ {nullptr};
  lv_obj_t* confirm_button_ {nullptr};
  std::uint8_t pending_hour_ {0};
  std::uint8_t pending_minute_ {0};
};

class DisplayScreenOffDisplayTimePage : public SettingsPageBase {
 public:
  DisplayScreenOffDisplayTimePage(DataCenter& data_center, PageId page_id, const char* title, bool edit_start);
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void roller_event_cb(lv_event_t* event);
  static void confirm_event_cb(lv_event_t* event);

  void reload_from_policy();
  void update_preview();
  void save_and_exit();

  bool edit_start_ {true};
  lv_obj_t* hour_roller_ {nullptr};
  lv_obj_t* minute_roller_ {nullptr};
  lv_obj_t* confirm_button_ {nullptr};
  std::uint8_t pending_hour_ {0};
  std::uint8_t pending_minute_ {0};
};

}  // namespace twsim::app
