#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Assets/MonicaAssets.h"
#include "App/UI/UiStyles.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace twsim::app {

namespace {

constexpr lv_coord_t kWatchfaceCanvasSize = 236;
constexpr std::size_t kInitialWreathFrameIndex = 8;
constexpr std::uint32_t kWreathFrameIntervalMs = 220;

void style_root(lv_obj_t* root, std::uint32_t color) {
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(color), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root, 0, 0);
  lv_obj_set_style_pad_all(root, 0, 0);
}

lv_obj_t* create_close_chip(lv_obj_t* root, const char* label_text, lv_event_cb_t callback, void* user_data) {
  lv_obj_t* button = lv_button_create(root);
  if (button == nullptr) {
    return nullptr;
  }

  ui_prepare_box(button);
  ui_apply_surface(button, SurfaceStyle::ButtonSecondary);
  ui_set_touch_target(button);
  lv_obj_set_size(button, 72, 28);
  lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, user_data);

  lv_obj_t* label = lv_label_create(button);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  ui_apply_text(label, TextStyle::Button);
  lv_label_set_text(label, label_text);
  lv_obj_center(label);
  return button;
}

void style_overlay_card(lv_obj_t* obj, lv_color_t color, lv_opa_t opa, lv_coord_t radius) {
  ui_prepare_box(obj);
  lv_obj_set_style_bg_color(obj, color, 0);
  lv_obj_set_style_bg_opa(obj, opa, 0);
  lv_obj_set_style_border_color(obj, lv_color_hex(0xD6C089), 0);
  lv_obj_set_style_border_opa(obj, LV_OPA_30, 0);
  lv_obj_set_style_border_width(obj, 1, 0);
  lv_obj_set_style_radius(obj, radius, 0);
}

lv_color_t color_for_home_shortcut(PageId page_id) {
  switch (page_id) {
    case PageId::HomeShortcutPayments:
      return lv_color_hex(0x1C2E46);
    case PageId::HomeShortcutNfc:
      return lv_color_hex(0x17343A);
    case PageId::HomeShortcutHealth:
      return lv_color_hex(0x321E3A);
    case PageId::HomeShortcutWeather:
      return lv_color_hex(0x253422);
    default:
      return lv_color_hex(0x182230);
  }
}

lv_color_t accent_for_home_shortcut(PageId page_id) {
  switch (page_id) {
    case PageId::HomeShortcutPayments:
      return lv_color_hex(0x7CC4FF);
    case PageId::HomeShortcutNfc:
      return lv_color_hex(0x67E8F9);
    case PageId::HomeShortcutHealth:
      return lv_color_hex(0xF9A8D4);
    case PageId::HomeShortcutWeather:
      return lv_color_hex(0xB7F07A);
    default:
      return lv_color_hex(0x93C5FD);
  }
}

}  // namespace

WatchfacePage::WatchfacePage(DataCenter& data_center) : PageBase(data_center) {}

PageId WatchfacePage::id() const {
  return PageId::Watchface;
}

const char* WatchfacePage::name() const {
  return "Watchface";
}

void WatchfacePage::on_will_appear() {
  if (wreath_timer_ != nullptr && monica_spring_wreath_available() && wreath_frames_remaining_ > 0) {
    lv_timer_resume(wreath_timer_);
  }
  if (wreath_image_ != nullptr && monica_spring_wreath_available()) {
    if (wreath_frames_remaining_ == 0) {
      wreath_frame_index_ = kInitialWreathFrameIndex % monica_spring_wreath_frame_count();
    }
    lv_image_set_src(wreath_image_, monica_spring_wreath_frames()[wreath_frame_index_]);
  }
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
}

void WatchfacePage::on_will_disappear() {
  if (wreath_timer_ != nullptr) {
    lv_timer_pause(wreath_timer_);
  }
}

lv_obj_t* WatchfacePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  wreath_image_ = lv_image_create(root);
  lv_obj_t* overlay = lv_obj_create(root);
  lv_obj_t* battery_chip = lv_obj_create(overlay);
  battery_label_ = lv_label_create(battery_chip);
  lv_obj_t* time_row = lv_obj_create(overlay);
  hour_label_ = lv_label_create(time_row);
  lv_obj_t* colon_label = lv_label_create(time_row);
  minute_label_ = lv_label_create(time_row);
  date_label_ = lv_label_create(overlay);
  status_label_ = lv_label_create(overlay);

  if (wreath_image_ == nullptr || overlay == nullptr || battery_chip == nullptr || battery_label_ == nullptr ||
      time_row == nullptr || hour_label_ == nullptr || colon_label == nullptr || minute_label_ == nullptr ||
      date_label_ == nullptr || status_label_ == nullptr) {
    return nullptr;
  }

  if (monica_spring_wreath_available()) {
    wreath_frame_index_ = kInitialWreathFrameIndex % monica_spring_wreath_frame_count();
    lv_image_set_src(wreath_image_, monica_spring_wreath_frames()[wreath_frame_index_]);
    const auto* first_frame = monica_spring_wreath_frames()[0];
    if (first_frame != nullptr && first_frame->header.w > 0 && first_frame->header.h > 0) {
      const auto scale_x =
          static_cast<std::uint32_t>((kWatchfaceCanvasSize * LV_SCALE_NONE) / first_frame->header.w);
      const auto scale_y =
          static_cast<std::uint32_t>((kWatchfaceCanvasSize * LV_SCALE_NONE) / first_frame->header.h);
      lv_image_set_scale(wreath_image_, std::min(scale_x, scale_y));
    }
  }
  lv_obj_align(wreath_image_, LV_ALIGN_CENTER, 0, 0);
  wreath_timer_ = lv_timer_create(&WatchfacePage::wreath_timer_cb, kWreathFrameIntervalMs, this);
  if (wreath_timer_ != nullptr) {
    lv_timer_set_repeat_count(wreath_timer_, -1);
    lv_timer_set_user_data(wreath_timer_, this);
    lv_timer_pause(wreath_timer_);
  }

  ui_prepare_box(overlay);
  lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_center(overlay);
  lv_obj_move_foreground(overlay);

  style_overlay_card(battery_chip, lv_color_hex(0x0D100C), LV_OPA_50, 16);
  lv_obj_set_style_pad_hor(battery_chip, 10, 0);
  lv_obj_set_style_pad_ver(battery_chip, 6, 0);
  lv_obj_set_size(battery_chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(battery_chip, LV_ALIGN_TOP_RIGHT, -12, 12);
  ui_prepare_label(battery_label_);
  ui_apply_text(battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xF7E6A7), 0);
  lv_label_set_text(battery_label_, "--%");

  ui_prepare_box(time_row);
  ui_set_flex_row(time_row, 0, 2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(time_row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(time_row, LV_ALIGN_CENTER, 0, -34);

  for (lv_obj_t* part : {hour_label_, colon_label, minute_label_}) {
    ui_prepare_label(part);
    ui_apply_text(part, TextStyle::Hero);
    lv_obj_set_style_text_color(part, lv_color_hex(0xF8E8A9), 0);
    lv_obj_set_style_text_font(part, &lv_font_montserrat_48, 0);
  }
  lv_label_set_text(hour_label_, "--");
  lv_label_set_text(colon_label, ":");
  lv_label_set_text(minute_label_, "--");

  ui_prepare_label(date_label_);
  ui_apply_text(date_label_, TextStyle::Title);
  lv_obj_set_style_text_color(date_label_, lv_color_hex(0xFFF5CB), 0);
  lv_obj_set_width(date_label_, 220);
  lv_obj_set_style_text_align(date_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(date_label_, LV_ALIGN_CENTER, 0, 26);
  lv_label_set_text(date_label_, "Waiting for RTC");

  ui_prepare_label(status_label_);
  ui_apply_text(status_label_, TextStyle::Tiny);
  lv_obj_set_style_text_color(status_label_, lv_color_hex(0xCABC86), 0);
  lv_obj_set_width(status_label_, 220);
  lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(status_label_, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_label_set_text(status_label_, monica_spring_wreath_available() ? "" : monica_spring_wreath_status());

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
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::TouchActivity:
                                     trigger_wreath_animation();
                                     break;
                                   default:
                                     break;
                                 }
                               }));
  return root;
}

void WatchfacePage::wreath_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<WatchfacePage*>(timer != nullptr ? lv_timer_get_user_data(timer) : nullptr);
  if (self == nullptr) {
    return;
  }
  self->advance_wreath_frame();
}

void WatchfacePage::advance_wreath_frame() {
  if (wreath_image_ == nullptr || !monica_spring_wreath_available()) {
    return;
  }

  const auto frame_count = monica_spring_wreath_frame_count();
  if (frame_count == 0) {
    return;
  }

  wreath_frame_index_ = (wreath_frame_index_ + 1) % frame_count;
  lv_image_set_src(wreath_image_, monica_spring_wreath_frames()[wreath_frame_index_]);
  if (wreath_frames_remaining_ > 0) {
    --wreath_frames_remaining_;
    if (wreath_frames_remaining_ == 0 && wreath_timer_ != nullptr) {
      lv_timer_pause(wreath_timer_);
    }
  }
}

void WatchfacePage::trigger_wreath_animation() {
  const auto frame_count = monica_spring_wreath_frame_count();
  if (wreath_timer_ == nullptr || frame_count == 0 || !monica_spring_wreath_available()) {
    return;
  }

  wreath_frames_remaining_ = frame_count;
  lv_timer_resume(wreath_timer_);
}

void WatchfacePage::apply_time(const TimeModel& model) {
  if (hour_label_ == nullptr || minute_label_ == nullptr || date_label_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(hour_label_, "--");
    lv_label_set_text(minute_label_, "--");
    lv_label_set_text(date_label_, "Waiting for RTC");
    return;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  char date_buffer[24] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%02u", static_cast<unsigned>(model.hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));
  std::snprintf(date_buffer,
                sizeof(date_buffer),
                "%02u/%02u/%04u",
                static_cast<unsigned>(model.month),
                static_cast<unsigned>(model.day),
                static_cast<unsigned>(model.year));
  lv_label_set_text(hour_label_, hour_buffer);
  lv_label_set_text(minute_label_, minute_buffer);
  lv_label_set_text(date_label_, date_buffer);
}

void WatchfacePage::apply_battery(const BatteryModel& model) {
  if (battery_label_ == nullptr) {
    return;
  }

  char buffer[32] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%%s", static_cast<int>(model.percent), model.charging ? " +" : "");
  lv_label_set_text(battery_label_, buffer);
}

LauncherPage::LauncherPage(DataCenter& data_center) : PageBase(data_center) {
  items_ = {
      {"Settings", {NavigationAction::LaunchApp, PageId::SettingsHome}, "Scroll prototype and system settings"},
      {"Display", {NavigationAction::LaunchApp, PageId::SettingDisplay}, "Brightness and wake policy shell"},
      {"Sound", {NavigationAction::LaunchApp, PageId::SettingSound}, "Sound and vibration shell"},
      {"Battery", {NavigationAction::LaunchApp, PageId::SettingBattery}, "Power and charging shell"},
      {"Bluetooth", {NavigationAction::LaunchApp, PageId::SettingBluetooth}, "Nearby devices shell"},
      {"Wi-Fi", {NavigationAction::LaunchApp, PageId::SettingWifi}, "Wireless connectivity shell"},
      {"Version", {NavigationAction::LaunchApp, PageId::SettingVersion}, "Build information"},
      {"Developer", {NavigationAction::LaunchApp, PageId::SettingDeveloper}, "Diagnostics and logs shell"},
  };
}

PageId LauncherPage::id() const {
  return PageId::Launcher;
}

const char* LauncherPage::name() const {
  return "Launcher";
}

lv_obj_t* LauncherPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x041119);

  lv_obj_t* title = lv_label_create(root);
  list_root_ = lv_obj_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* close = create_close_chip(root, "Home", &LauncherPage::back_event_cb, this);
  if (title == nullptr || subtitle == nullptr || list_root_ == nullptr || close == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Launcher");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 16);

  lv_obj_align(close, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_label_set_text(subtitle, "Flow list prototype, drag or rotate crown");
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

  lv_obj_set_size(list_root_, 220, 164);
  lv_obj_align(list_root_, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_obj_set_flex_flow(list_root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(list_root_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(list_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(list_root_, 8, 0);
  lv_obj_set_style_pad_row(list_root_, 8, 0);
  lv_obj_set_style_bg_color(list_root_, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(list_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(list_root_, 1, 0);
  lv_obj_set_style_border_color(list_root_, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(list_root_, 18, 0);

  for (std::size_t index = 0; index < items_.size(); ++index) {
    lv_obj_t* button = lv_button_create(list_root_);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_set_height(button, 52);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x132033), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 16, 0);
    lv_obj_add_event_cb(button, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* label = lv_label_create(button);
    lv_obj_t* detail = lv_label_create(button);
    if (label == nullptr || detail == nullptr) {
      return nullptr;
    }

    lv_label_set_text(label, items_[index].label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 12, -8);

    lv_label_set_text(detail, items_[index].detail);
    lv_obj_set_style_text_font(detail, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(detail, lv_color_hex(0x93C5FD), 0);
    lv_obj_align(detail, LV_ALIGN_LEFT_MID, 12, 10);
  }

  bind_input();
  return root;
}

HomeShortcutPage::HomeShortcutPage(DataCenter& data_center, Config config)
    : PageBase(data_center), config_(config) {}

PageId HomeShortcutPage::id() const {
  return config_.page_id;
}

const char* HomeShortcutPage::name() const {
  return page_name(config_.page_id);
}

lv_obj_t* HomeShortcutPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);

  const lv_color_t accent = accent_for_home_shortcut(config_.page_id);
  const lv_color_t panel_bg = color_for_home_shortcut(config_.page_id);

  lv_obj_t* orbit_chip = lv_obj_create(root);
  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* hero_card = lv_obj_create(root);
  lv_obj_t* hero_tag = lv_obj_create(hero_card);
  lv_obj_t* hero_tag_label = lv_label_create(hero_tag);
  lv_obj_t* hero_value = lv_label_create(hero_card);
  lv_obj_t* hero_title = lv_label_create(hero_card);
  lv_obj_t* hero_detail = lv_label_create(hero_card);
  lv_obj_t* metric_grid = lv_obj_create(root);
  if (orbit_chip == nullptr || title == nullptr || subtitle == nullptr || hero_card == nullptr || hero_tag == nullptr ||
      hero_tag_label == nullptr || hero_value == nullptr || hero_title == nullptr || hero_detail == nullptr ||
      metric_grid == nullptr) {
    return nullptr;
  }

  ui_prepare_box(orbit_chip);
  ui_apply_surface(orbit_chip, SurfaceStyle::Chip);
  lv_obj_set_size(orbit_chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_border_color(orbit_chip, accent, 0);
  lv_obj_set_style_bg_color(orbit_chip, lv_color_mix(panel_bg, lv_color_hex(0x05080F), LV_OPA_60), 0);
  lv_obj_align(orbit_chip, LV_ALIGN_TOP_LEFT, 12, 12);

  ui_prepare_label(title);
  ui_apply_text(title, TextStyle::Title);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 52);
  lv_label_set_text(title, config_.title);

  ui_prepare_label(subtitle);
  ui_apply_text(subtitle, TextStyle::Muted);
  lv_obj_set_width(subtitle, 208);
  lv_label_set_long_mode(subtitle, LV_LABEL_LONG_WRAP);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 6);
  lv_label_set_text(subtitle, config_.subtitle);

  lv_obj_t* orbit_label = lv_label_create(orbit_chip);
  if (orbit_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(orbit_label);
  ui_apply_text(orbit_label, TextStyle::Accent);
  lv_obj_set_style_text_color(orbit_label, accent, 0);
  lv_label_set_text(orbit_label, config_.orbit_label);
  lv_obj_center(orbit_label);

  ui_prepare_box(hero_card);
  ui_apply_surface(hero_card, SurfaceStyle::Panel);
  lv_obj_set_size(hero_card, 220, 88);
  lv_obj_align(hero_card, LV_ALIGN_TOP_MID, 0, 96);
  lv_obj_set_style_bg_color(hero_card, panel_bg, 0);
  lv_obj_set_style_border_color(hero_card, lv_color_mix(accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
  lv_obj_set_style_pad_all(hero_card, 14, 0);

  ui_prepare_box(hero_tag);
  ui_apply_surface(hero_tag, SurfaceStyle::Chip);
  lv_obj_set_size(hero_tag, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_color(hero_tag, lv_color_mix(accent, lv_color_hex(0x05080F), LV_OPA_70), 0);
  lv_obj_set_style_border_width(hero_tag, 0, 0);
  lv_obj_align(hero_tag, LV_ALIGN_TOP_LEFT, 0, 0);

  ui_prepare_label(hero_tag_label);
  ui_apply_text(hero_tag_label, TextStyle::Tiny);
  lv_obj_set_style_text_color(hero_tag_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(hero_tag_label, config_.focus_label);
  lv_obj_center(hero_tag_label);

  ui_prepare_label(hero_value);
  ui_apply_text(hero_value, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(hero_value, &lv_font_montserrat_26, 0);
  lv_obj_set_style_text_color(hero_value, accent, 0);
  lv_obj_align(hero_value, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_label_set_text(hero_value, config_.focus_value);

  ui_prepare_label(hero_title);
  ui_apply_text(hero_title, TextStyle::Title);
  lv_obj_set_style_text_color(hero_title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(hero_title, LV_ALIGN_TOP_RIGHT, 0, 30);
  lv_label_set_text(hero_title, config_.title);

  ui_prepare_label(hero_detail);
  ui_apply_text(hero_detail, TextStyle::Tiny);
  lv_obj_set_width(hero_detail, 208);
  lv_label_set_long_mode(hero_detail, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_align(hero_detail, LV_TEXT_ALIGN_LEFT, 0);
  lv_obj_align(hero_detail, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_label_set_text(hero_detail, config_.focus_detail);

  ui_prepare_box(metric_grid);
  lv_obj_set_size(metric_grid, 220, 98);
  lv_obj_align(metric_grid, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_set_layout(metric_grid, LV_LAYOUT_GRID);
  static lv_coord_t columns[] = {106, 106, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t rows[] = {45, 45, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(metric_grid, columns, rows);
  lv_obj_set_style_pad_all(metric_grid, 0, 0);
  lv_obj_set_style_pad_row(metric_grid, 8, 0);
  lv_obj_set_style_pad_column(metric_grid, 8, 0);
  lv_obj_set_style_bg_opa(metric_grid, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(metric_grid, 0, 0);

  for (std::size_t index = 0; index < config_.metrics.size(); ++index) {
    const auto& metric = config_.metrics[index];
    const auto row = static_cast<lv_coord_t>(index / 2);
    const auto col = static_cast<lv_coord_t>(index % 2);

    lv_obj_t* card = lv_obj_create(metric_grid);
    lv_obj_t* label = lv_label_create(card);
    lv_obj_t* value = lv_label_create(card);
    lv_obj_t* detail = lv_label_create(card);
    if (card == nullptr || label == nullptr || value == nullptr || detail == nullptr) {
      return nullptr;
    }

    ui_prepare_box(card);
    ui_apply_surface(card, SurfaceStyle::PanelSubtle);
    lv_obj_set_grid_cell(card, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
    lv_obj_set_style_bg_color(card, lv_color_mix(panel_bg, lv_color_hex(0x05080F), LV_OPA_50), 0);
    lv_obj_set_style_border_color(card, lv_color_mix(accent, lv_color_hex(0x203040), LV_OPA_30), 0);
    lv_obj_set_style_pad_all(card, 10, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Tiny);
    lv_label_set_text(label, metric.label);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    ui_prepare_label(value);
    ui_apply_text(value, TextStyle::Body);
    lv_obj_set_style_text_color(value, accent, 0);
    lv_label_set_text(value, metric.value);
    lv_obj_align(value, LV_ALIGN_LEFT_MID, 0, -2);

    ui_prepare_label(detail);
    ui_apply_text(detail, TextStyle::Tiny);
    lv_obj_set_width(detail, 82);
    lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
    lv_label_set_text(detail, metric.detail);
    lv_obj_align(detail, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  }

  return root;
}

void LauncherPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->request_navigation({NavigationAction::ReturnHome, PageId::Watchface});
}

void LauncherPage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }

  const auto item_index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (item_index >= self->items_.size()) {
    return;
  }

  self->request_navigation(self->items_[item_index].command);
}

void LauncherPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || list_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     lv_obj_scroll_by(list_root_, 0, 44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     lv_obj_scroll_by(list_root_, 0, -44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

NotificationsPage::NotificationsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId NotificationsPage::id() const {
  return PageId::Notifications;
}

const char* NotificationsPage::name() const {
  return "Notifications";
}

lv_obj_t* NotificationsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x07131F);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* close = create_close_chip(root, "Home", &NotificationsPage::close_event_cb, this);
  list_root_ = lv_obj_create(root);
  if (title == nullptr || subtitle == nullptr || close == nullptr || list_root_ == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Messages");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 16);

  lv_label_set_text(subtitle, "Mock phone notifications");
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
  lv_obj_align(close, LV_ALIGN_TOP_RIGHT, -10, 10);

  lv_obj_set_size(list_root_, 220, 164);
  lv_obj_align(list_root_, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_obj_set_flex_flow(list_root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(list_root_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(list_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(list_root_, 8, 0);
  lv_obj_set_style_pad_row(list_root_, 8, 0);
  lv_obj_set_style_bg_color(list_root_, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(list_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(list_root_, 1, 0);
  lv_obj_set_style_border_color(list_root_, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(list_root_, 18, 0);

  for (const auto& item : items_) {
    lv_obj_t* card = lv_obj_create(list_root_);
    if (card == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(card, 12, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x132033), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* source = lv_label_create(card);
    lv_obj_t* detail = lv_label_create(card);
    if (source == nullptr || detail == nullptr) {
      return nullptr;
    }

    lv_label_set_text(source, item.source);
    lv_obj_set_style_text_font(source, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(source, lv_color_hex(0x67E8F9), 0);
    lv_obj_align(source, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_label_set_text(detail, item.detail);
    lv_obj_set_width(detail, 188);
    lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(detail, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(detail, lv_color_hex(0xE2E8F0), 0);
    lv_obj_align_to(detail, source, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 6);
  }

  bind_input();
  return root;
}

void NotificationsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || list_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     lv_obj_scroll_by(list_root_, 0, 44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     lv_obj_scroll_by(list_root_, 0, -44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

QuickSettingsPage::QuickSettingsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId QuickSettingsPage::id() const {
  return PageId::QuickSettings;
}

const char* QuickSettingsPage::name() const {
  return "QuickSettings";
}

lv_obj_t* QuickSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x07131F);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* close = create_close_chip(root, "Close", &QuickSettingsPage::close_event_cb, this);
  lv_obj_t* grid = lv_obj_create(root);
  if (title == nullptr || subtitle == nullptr || close == nullptr || grid == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Quick Settings");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, 16);

  lv_label_set_text(subtitle, "Tap to toggle, long press for detail");
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
  lv_obj_align(close, LV_ALIGN_TOP_RIGHT, -10, 10);

  lv_obj_set_size(grid, 220, 154);
  lv_obj_align(grid, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_layout(grid, LV_LAYOUT_GRID);
  static lv_coord_t columns[] = {106, 106, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t rows[] = {72, 72, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(grid, columns, rows);
  lv_obj_set_style_pad_all(grid, 6, 0);
  lv_obj_set_style_pad_row(grid, 8, 0);
  lv_obj_set_style_pad_column(grid, 8, 0);
  lv_obj_set_style_bg_color(grid, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(grid, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(grid, 1, 0);
  lv_obj_set_style_border_color(grid, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(grid, 18, 0);
  lv_obj_remove_flag(grid, LV_OBJ_FLAG_SCROLLABLE);

  for (std::size_t index = 0; index < toggles_.size(); ++index) {
    const auto row = static_cast<lv_coord_t>(index / 2);
    const auto col = static_cast<lv_coord_t>(index % 2);
    lv_obj_t* button = lv_button_create(grid);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_grid_cell(button, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x16263A), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 18, 0);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_long_press_event_cb, LV_EVENT_LONG_PRESSED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* label = lv_label_create(button);
    toggles_[index].value_label = lv_label_create(button);
    if (label == nullptr || toggles_[index].value_label == nullptr) {
      return nullptr;
    }

    lv_label_set_text(label, toggles_[index].label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE2E8F0), 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 12, 10);

    lv_obj_set_style_text_font(toggles_[index].value_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(toggles_[index].value_label, lv_color_hex(0x67E8F9), 0);
    lv_obj_align(toggles_[index].value_label, LV_ALIGN_BOTTOM_LEFT, 12, -10);
    update_toggle_label(index);
  }

  return root;
}

void QuickSettingsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::toggle_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->toggles_.size()) {
    return;
  }

  self->toggles_[index].mode = static_cast<std::int16_t>((self->toggles_[index].mode + 1) % 3);
  self->update_toggle_label(index);
}

void QuickSettingsPage::toggle_long_press_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->toggles_.size()) {
    return;
  }

  self->request_navigation({NavigationAction::LaunchApp, self->toggles_[index].detail_page});
}

void QuickSettingsPage::update_toggle_label(std::size_t index) {
  if (index >= toggles_.size() || toggles_[index].value_label == nullptr) {
    return;
  }

  const char* text = "Off";
  if (std::strcmp(toggles_[index].label, "Brightness") == 0) {
    switch (toggles_[index].mode) {
      case 0:
        text = "Low";
        break;
      case 1:
        text = "Medium";
        break;
      default:
        text = "High";
        break;
    }
  } else {
    text = toggles_[index].mode == 0 ? "Off" : (toggles_[index].mode == 1 ? "On" : "Boost");
  }
  lv_label_set_text(toggles_[index].value_label, text);
}

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
