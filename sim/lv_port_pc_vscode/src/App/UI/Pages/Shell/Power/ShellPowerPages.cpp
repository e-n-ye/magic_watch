#include "App/UI/Pages/ShellPages.h"

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <string>
#include <utility>

namespace twsim::app {

namespace {

using shell_font::cjk_font_16;
using shell_font::cjk_font_72;

constexpr float kPi = 3.14159265358979323846f;

std::string screen_off_date_text(const TimeModel& time) {
  if (!time.valid) {
    return "--";
  }

  std::tm calendar {};
  calendar.tm_year = static_cast<int>(time.year) - 1900;
  calendar.tm_mon = static_cast<int>(time.month) - 1;
  calendar.tm_mday = static_cast<int>(time.day);
  calendar.tm_hour = 12;
  const int weekday = std::mktime(&calendar) == -1 ? -1 : calendar.tm_wday;

  const char* weekday_text = "--";
  switch (weekday) {
    case 0:
      weekday_text = "\xE5\x91\xA8\xE6\x97\xA5";
      break;
    case 1:
      weekday_text = "\xE5\x91\xA8\xE4\xB8\x80";
      break;
    case 2:
      weekday_text = "\xE5\x91\xA8\xE4\xBA\x8C";
      break;
    case 3:
      weekday_text = "\xE5\x91\xA8\xE4\xB8\x89";
      break;
    case 4:
      weekday_text = "\xE5\x91\xA8\xE5\x9B\x9B";
      break;
    case 5:
      weekday_text = "\xE5\x91\xA8\xE4\xBA\x94";
      break;
    case 6:
      weekday_text = "\xE5\x91\xA8\xE5\x85\xAD";
      break;
    default:
      break;
  }

  char buffer[48] = {};
  std::snprintf(buffer,
                sizeof(buffer),
                "%u\xE6\x9C\x88%u\xE6\x97\xA5 %s",
                static_cast<unsigned>(time.month),
                static_cast<unsigned>(time.day),
                weekday_text);
  return buffer;
}

void set_hand_points(lv_point_precise_t points[2],
                     float angle_degrees,
                     float center_x,
                     float center_y,
                     float back_length,
                     float front_length) {
  const float radians = (angle_degrees - 90.0f) * (kPi / 180.0f);
  const float direction_x = std::cos(radians);
  const float direction_y = std::sin(radians);
  points[0].x = center_x - direction_x * back_length;
  points[0].y = center_y - direction_y * back_length;
  points[1].x = center_x + direction_x * front_length;
  points[1].y = center_y + direction_y * front_length;
}

}  // namespace

PowerMenuPage::PowerMenuPage(DataCenter& data_center) : PageBase(data_center) {}

PageId PowerMenuPage::id() const {
  return PageId::PowerMenu;
}

const char* PowerMenuPage::name() const {
  return "PowerMenu";
}

lv_obj_t* PowerMenuPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x05080F);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* list = lv_obj_create(root);
  if (title == nullptr || subtitle == nullptr || list == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Power");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  lv_label_set_text(subtitle, "Long press 5 opens this page");
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

  lv_obj_set_size(list, 190, 130);
  lv_obj_center(list);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(list, 8, 0);
  lv_obj_set_style_pad_gap(list, 8, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(list, 1, 0);
  lv_obj_set_style_border_color(list, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(list, 20, 0);
  lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLLABLE);

  const std::array<std::pair<const char*, NavigationCommand>, 3> buttons {{
      {"Power Off", {NavigationAction::PowerOff, PageId::PoweredOff}},
      {"Restart", {NavigationAction::Restart, PageId::Watchface}},
      {"Cancel", {NavigationAction::ReturnHome, PageId::Watchface}},
  }};

  for (std::size_t index = 0; index < buttons.size(); ++index) {
    lv_obj_t* button = lv_button_create(list);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_set_height(button, 34);
    lv_obj_set_style_bg_color(button,
                              index == 0 ? lv_color_hex(0x6F1D1B)
                                         : (index == 1 ? lv_color_hex(0x5A401A) : lv_color_hex(0x1F3C5A)),
                              0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 14, 0);
    lv_obj_add_event_cb(button, &PowerMenuPage::action_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    lv_label_set_text(label, buttons[index].first);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
  }

  return root;
}

void PowerMenuPage::action_event_cb(lv_event_t* event) {
  auto* self = static_cast<PowerMenuPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  switch (index) {
    case 0:
      self->request_navigation({NavigationAction::PowerOff, PageId::PoweredOff});
      break;
    case 1:
      self->request_navigation({NavigationAction::Restart, PageId::Watchface});
      break;
    case 2:
    default:
      self->request_navigation({NavigationAction::ReturnHome, PageId::Watchface});
      break;
  }
}

ScreenOffPage::ScreenOffPage(DataCenter& data_center) : PageBase(data_center) {}

PageId ScreenOffPage::id() const {
  return PageId::ScreenOff;
}

const char* ScreenOffPage::name() const {
  return page_name(PageId::ScreenOff);
}

void ScreenOffPage::on_will_appear() {
  if (const auto time = data_center_.time()) {
    time_model_ = *time;
  }
  if (const auto battery = data_center_.battery()) {
    battery_model_ = *battery;
  }
  if (const auto policy = data_center_.display_policy()) {
    display_policy_ = *policy;
  }
  refresh_view();
}

lv_obj_t* ScreenOffPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);

  analog_root_ = lv_obj_create(root);
  info_root_ = lv_obj_create(root);
  if (analog_root_ == nullptr || info_root_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(analog_root_);
  lv_obj_set_size(analog_root_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(analog_root_);
  lv_obj_set_style_bg_opa(analog_root_, LV_OPA_TRANSP, 0);

  analog_hour_hand_ = lv_line_create(analog_root_);
  analog_minute_hand_ = lv_line_create(analog_root_);
  lv_obj_t* analog_dot = lv_obj_create(analog_root_);
  analog_battery_label_ = lv_label_create(analog_root_);
  if (analog_hour_hand_ == nullptr || analog_minute_hand_ == nullptr || analog_dot == nullptr ||
      analog_battery_label_ == nullptr) {
    return nullptr;
  }

  lv_line_set_points_mutable(analog_hour_hand_, analog_hour_points_, 2);
  lv_obj_set_size(analog_hour_hand_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_line_width(analog_hour_hand_, 10, 0);
  lv_obj_set_style_line_color(analog_hour_hand_, lv_color_hex(0xE2F6FF), 0);
  lv_obj_set_style_line_rounded(analog_hour_hand_, true, 0);

  lv_line_set_points_mutable(analog_minute_hand_, analog_minute_points_, 2);
  lv_obj_set_size(analog_minute_hand_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_line_width(analog_minute_hand_, 10, 0);
  lv_obj_set_style_line_color(analog_minute_hand_, lv_color_hex(0xE2F6FF), 0);
  lv_obj_set_style_line_rounded(analog_minute_hand_, true, 0);

  ui_prepare_box(analog_dot);
  lv_obj_set_size(analog_dot, 8, 8);
  lv_obj_set_style_radius(analog_dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(analog_dot, lv_color_hex(0xE2F6FF), 0);
  lv_obj_set_style_bg_opa(analog_dot, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(analog_dot, 0, 0);
  lv_obj_align(analog_dot, LV_ALIGN_CENTER, 0, -8);

  ui_prepare_label(analog_battery_label_);
  lv_obj_set_style_text_font(analog_battery_label_, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(analog_battery_label_, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(analog_battery_label_, "80%");
  lv_obj_align(analog_battery_label_, LV_ALIGN_BOTTOM_MID, 0, -26);

  ui_prepare_box(info_root_);
  lv_obj_set_size(info_root_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(info_root_);
  lv_obj_set_style_bg_opa(info_root_, LV_OPA_TRANSP, 0);

  info_hour_label_ = lv_label_create(info_root_);
  info_minute_label_ = lv_label_create(info_root_);
  info_date_label_ = lv_label_create(info_root_);
  info_battery_label_ = lv_label_create(info_root_);
  if (info_hour_label_ == nullptr || info_minute_label_ == nullptr || info_date_label_ == nullptr ||
      info_battery_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_label(info_hour_label_);
  lv_obj_set_style_text_font(info_hour_label_, cjk_font_72(), 0);
  lv_obj_set_style_text_color(info_hour_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(info_hour_label_, "09");
  lv_obj_align(info_hour_label_, LV_ALIGN_CENTER, 0, -52);

  ui_prepare_label(info_minute_label_);
  lv_obj_set_style_text_font(info_minute_label_, cjk_font_72(), 0);
  lv_obj_set_style_text_color(info_minute_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(info_minute_label_, "28");
  lv_obj_align_to(info_minute_label_, info_hour_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);

  ui_prepare_label(info_date_label_);
  lv_obj_set_style_text_font(info_date_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(info_date_label_, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(info_date_label_, "--");
  lv_obj_align(info_date_label_, LV_ALIGN_BOTTOM_MID, 0, -52);

  ui_prepare_label(info_battery_label_);
  lv_obj_set_style_text_font(info_battery_label_, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(info_battery_label_, lv_color_hex(0xD6F3FF), 0);
  lv_label_set_text(info_battery_label_, "80%");
  lv_obj_align(info_battery_label_, LV_ALIGN_BOTTOM_MID, 0, -24);

  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<DisplayPolicyModel>(&event.payload)) {
                                   apply_policy(*model);
                                 }
                               }));

  if (const auto time = data_center_.time()) {
    time_model_ = *time;
  }
  if (const auto battery = data_center_.battery()) {
    battery_model_ = *battery;
  }
  if (const auto policy = data_center_.display_policy()) {
    display_policy_ = *policy;
  }
  refresh_view();
  return root;
}

void ScreenOffPage::apply_time(const TimeModel& model) {
  time_model_ = model;
  refresh_view();
}

void ScreenOffPage::apply_battery(const BatteryModel& model) {
  battery_model_ = model;
  refresh_view();
}

void ScreenOffPage::apply_policy(const DisplayPolicyModel& policy) {
  display_policy_ = policy;
  refresh_view();
}

void ScreenOffPage::refresh_view() {
  if (analog_root_ == nullptr || info_root_ == nullptr) {
    return;
  }

  const bool active = DisplayPolicyRules::IsScreenOffDisplayActive(display_policy_, time_model_);
  if (!active) {
    lv_obj_add_flag(analog_root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(info_root_, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  switch (display_policy_.screen_off_style_id) {
    case ScreenOffStyleId::InfoDigits:
      lv_obj_add_flag(analog_root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(info_root_, LV_OBJ_FLAG_HIDDEN);
      update_info_preview();
      break;
    case ScreenOffStyleId::AnalogHands:
    default:
      lv_obj_add_flag(info_root_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(analog_root_, LV_OBJ_FLAG_HIDDEN);
      update_analog_preview();
      break;
  }
}

void ScreenOffPage::update_analog_preview() {
  if (analog_hour_hand_ == nullptr || analog_minute_hand_ == nullptr || analog_battery_label_ == nullptr) {
    return;
  }

  const float center_x = 120.0f;
  const float center_y = 128.0f;
  const unsigned hour = time_model_.valid ? static_cast<unsigned>(time_model_.hour % 12U) : 0U;
  const unsigned minute = time_model_.valid ? static_cast<unsigned>(time_model_.minute) : 0U;
  const float hour_angle = (static_cast<float>(hour) + static_cast<float>(minute) / 60.0f) * 30.0f;
  const float minute_angle = static_cast<float>(minute) * 6.0f;
  set_hand_points(analog_hour_points_, hour_angle, center_x, center_y, 12.0f, 50.0f);
  set_hand_points(analog_minute_points_, minute_angle, center_x, center_y, 12.0f, 76.0f);
  lv_obj_invalidate(analog_hour_hand_);
  lv_obj_invalidate(analog_minute_hand_);

  char battery_text[24] = {};
  std::snprintf(battery_text,
                sizeof(battery_text),
                "%d%%%s",
                static_cast<int>(battery_model_.percent),
                battery_model_.charging ? "+" : "");
  lv_label_set_text(analog_battery_label_, battery_text);
}

void ScreenOffPage::update_info_preview() {
  if (info_hour_label_ == nullptr || info_minute_label_ == nullptr || info_date_label_ == nullptr ||
      info_battery_label_ == nullptr) {
    return;
  }

  if (!time_model_.valid) {
    lv_label_set_text(info_hour_label_, "--");
    lv_label_set_text(info_minute_label_, "--");
    lv_label_set_text(info_date_label_, "--");
  } else {
    char hour_text[8] = {};
    char minute_text[8] = {};
    std::snprintf(hour_text, sizeof(hour_text), "%02u", static_cast<unsigned>(time_model_.hour));
    std::snprintf(minute_text, sizeof(minute_text), "%02u", static_cast<unsigned>(time_model_.minute));
    lv_label_set_text(info_hour_label_, hour_text);
    lv_label_set_text(info_minute_label_, minute_text);
    const std::string date_text = screen_off_date_text(time_model_);
    lv_label_set_text(info_date_label_, date_text.c_str());
  }

  char battery_text[32] = {};
  std::snprintf(battery_text,
                sizeof(battery_text),
                "%d%%%s",
                static_cast<int>(battery_model_.percent),
                battery_model_.charging ? "+" : "");
  lv_label_set_text(info_battery_label_, battery_text);
}

PassiveShellPage::PassiveShellPage(DataCenter& data_center, PageId page_id, const char* title, const char* detail)
    : PageBase(data_center), page_id_(page_id), title_(title), detail_(detail) {}

PageId PassiveShellPage::id() const {
  return page_id_;
}

const char* PassiveShellPage::name() const {
  return page_name(page_id_);
}

lv_obj_t* PassiveShellPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x000000);

  if (page_id_ == PageId::ScreenOff) {
    lv_obj_t* debug_hint = lv_label_create(root);
    if (debug_hint == nullptr) {
      return root;
    }
    lv_label_set_text(debug_hint, "screen off");
    lv_obj_set_style_text_font(debug_hint, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(debug_hint, lv_color_hex(0x101010), 0);
    lv_obj_align(debug_hint, LV_ALIGN_BOTTOM_MID, 0, -8);
    return root;
  }

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* detail = lv_label_create(root);
  if (title == nullptr || detail == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, title_);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -18);

  lv_label_set_text(detail, detail_);
  lv_obj_set_width(detail, 180);
  lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(detail, lv_color_hex(0x94A3B8), 0);
  lv_obj_align(detail, LV_ALIGN_CENTER, 0, 24);
  return root;
}

}  // namespace twsim::app
