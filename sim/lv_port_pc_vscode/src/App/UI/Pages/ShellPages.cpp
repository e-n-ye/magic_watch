#include "App/UI/Pages/ShellPages.h"

#include "App/UI/UiStyles.h"
#include "lvgl/src/misc/lv_fs.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>
#include <filesystem>
#include <string>

namespace twsim::app {

namespace {

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

lv_coord_t scale_by_ratio(lv_coord_t total, int numerator, int denominator) {
  return static_cast<lv_coord_t>((static_cast<long long>(total) * numerator) / denominator);
}

lv_coord_t clamp_coord(lv_coord_t value, lv_coord_t minimum, lv_coord_t maximum) {
  return std::max(minimum, std::min(maximum, value));
}

bool has_text(const char* text) {
  return text != nullptr && text[0] != '\0';
}

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
}

std::string resolve_lvgl_asset_path(const char* relative_asset_path) {
  if (!has_text(relative_asset_path)) {
    return {};
  }

  namespace fs = std::filesystem;
  const fs::path relative_path(relative_asset_path);
  const fs::path start = fs::current_path();
  std::array<fs::path, 7> candidates {
      start,
      start / "..",
      start / ".." / "..",
      start / ".." / ".." / "..",
      start / ".." / ".." / ".." / "..",
      start / ".." / ".." / ".." / ".." / "..",
      fs::path("D:/MY_Desk/watch/magic_watch"),
  };

  for (const auto& base : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(base / relative_path, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  std::printf("[magic_watch] asset not found: %s (cwd=%s)\n",
              relative_asset_path,
              start.generic_string().c_str());
  return {};
}

const char* weather_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/weather_cloud_sun.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* sleep_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/sleep_bed.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* steps_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/steps_foot.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_alipay_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_alipay.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_wechat_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_wechat.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_wechat_green_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_wechat_green.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_heart_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_heart.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_spo2_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_spo2.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_breathe_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_breathe.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_stress_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_stress.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* nfc_school_card_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/nfc_school_card.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* nfc_school_card_inner_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/nfc_school_card_inner.png");
  return path.empty() ? nullptr : path.c_str();
}

bool file_exists(const char* path) {
  if (!has_text(path)) {
    return false;
  }
  lv_fs_file_t file;
  if (lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
    return false;
  }
  lv_fs_close(&file);
  return true;
}

lv_obj_t* create_pager_dot(lv_obj_t* parent, lv_color_t color, lv_opa_t opa, bool active) {
  lv_obj_t* dot = lv_obj_create(parent);
  if (dot == nullptr) {
    return nullptr;
  }

  ui_prepare_box(dot);
  lv_obj_set_size(dot, active ? 9 : 7, active ? 9 : 7);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, color, 0);
  lv_obj_set_style_bg_opa(dot, active ? LV_OPA_70 : opa, 0);
  return dot;
}

struct HomeSurfaceLayout {
  lv_coord_t screen_w;
  lv_coord_t screen_h;
  lv_coord_t safe_margin_x;
  lv_coord_t chip_top;
  lv_coord_t chip_left;
  lv_coord_t stage_top;
  lv_coord_t stage_w;
  lv_coord_t stage_h;
  lv_coord_t stage_radius;
  lv_coord_t stage_pad;
  lv_coord_t stage_gap;
  lv_coord_t title_h;
  lv_coord_t hero_h;
  lv_coord_t card_gap;
  lv_coord_t card_w;
  lv_coord_t card_h;
  lv_coord_t pager_bottom;
};

HomeSurfaceLayout make_home_surface_layout() {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t safe_margin_x = clamp_coord(scale_by_ratio(screen_w, 4, 100), 8, 10);
  const lv_coord_t chip_top = clamp_coord(scale_by_ratio(screen_h, 4, 100), 8, 12);
  const lv_coord_t stage_top = clamp_coord(scale_by_ratio(screen_h, 7, 100), 18, 22);
  const lv_coord_t stage_w = screen_w - safe_margin_x * 2;
  const lv_coord_t stage_h = clamp_coord(scale_by_ratio(screen_h, 82, 100), 220, 244);
  const lv_coord_t stage_radius = clamp_coord(scale_by_ratio(stage_w, 10, 100), 22, 26);
  const lv_coord_t stage_pad = clamp_coord(scale_by_ratio(stage_w, 3, 100), 6, 10);
  const lv_coord_t stage_gap = clamp_coord(scale_by_ratio(stage_h, 2, 100), 4, 8);
  const lv_coord_t title_h = clamp_coord(scale_by_ratio(stage_h, 8, 100), 16, 20);
  const lv_coord_t hero_h = clamp_coord(scale_by_ratio(stage_h, 32, 100), 54, 64);
  const lv_coord_t card_gap = clamp_coord(scale_by_ratio(stage_w, 3, 100), 6, 10);
  const lv_coord_t available_h = stage_h - stage_pad * 2 - title_h - hero_h - stage_gap * 2;
  const lv_coord_t card_w = (stage_w - stage_pad * 2 - card_gap) / 2;
  const lv_coord_t card_h = (available_h - card_gap) / 2;
  const lv_coord_t pager_bottom = clamp_coord(scale_by_ratio(screen_h, 2, 100), 2, 8);
  return {screen_w,
          screen_h,
          safe_margin_x,
          chip_top,
          safe_margin_x,
          stage_top,
          stage_w,
          stage_h,
          stage_radius,
          stage_pad,
          stage_gap,
          title_h,
          hero_h,
          card_gap,
          card_w,
          card_h,
          pager_bottom};
}

void set_single_line_label(lv_obj_t* label, lv_coord_t width) {
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
}

void style_home_surface_stage(lv_obj_t* stage, lv_coord_t width, lv_coord_t height, lv_coord_t radius, lv_color_t bg_color) {
  ui_prepare_box(stage);
  ui_apply_surface(stage, SurfaceStyle::Panel);
  lv_obj_set_size(stage, width, height);
  lv_obj_set_style_bg_color(stage, bg_color, 0);
  lv_obj_set_style_border_width(stage, 0, 0);
  lv_obj_set_style_radius(stage, radius, 0);
  lv_obj_set_style_pad_all(stage, 0, 0);
}

lv_obj_t* create_home_stage_root(lv_obj_t* root, const HomeSurfaceLayout& layout, lv_color_t stage_bg) {
  lv_obj_t* stage = lv_obj_create(root);
  if (stage == nullptr) {
    return nullptr;
  }
  style_home_surface_stage(stage, layout.stage_w, layout.stage_h, layout.stage_radius, stage_bg);
  lv_obj_align(stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);
  lv_obj_set_style_shadow_width(stage, 0, 0);
  lv_obj_set_style_shadow_opa(stage, LV_OPA_TRANSP, 0);
  return stage;
}

lv_obj_t* create_home_pager(lv_obj_t* root, const HomeSurfaceLayout& layout, std::size_t active_index) {
  lv_obj_t* pager = lv_obj_create(root);
  if (pager == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pager);
  ui_set_flex_row(pager, 0, 8, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(pager, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(pager, LV_OPA_TRANSP, 0);
  lv_obj_align(pager, LV_ALIGN_BOTTOM_MID, 0, -layout.pager_bottom);

  for (std::size_t index = 0; index < 4; ++index) {
    const bool active = index == active_index;
    lv_obj_t* dot = create_pager_dot(pager, lv_color_hex(0xFFFFFF), LV_OPA_10, active);
    if (dot == nullptr) {
      return nullptr;
    }
    if (!active) {
      lv_obj_set_style_bg_color(dot, lv_color_hex(0xB9C5D5), 0);
    }
  }
  return pager;
}

lv_obj_t* create_contain_image(lv_obj_t* parent,
                               const char* path,
                               lv_coord_t width,
                               lv_coord_t height,
                               lv_align_t align,
                               lv_coord_t x,
                               lv_coord_t y) {
  lv_obj_t* image = lv_image_create(parent);
  if (image == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(image, width, height);
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
  if (file_exists(path)) {
    lv_image_set_src(image, path);
  } else {
    lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_align(image, align, x, y);
  return image;
}

lv_obj_t* create_cover_image(lv_obj_t* parent,
                             const char* path,
                             lv_coord_t width,
                             lv_coord_t height,
                             lv_align_t align,
                             lv_coord_t x,
                             lv_coord_t y) {
  lv_obj_t* image = lv_image_create(parent);
  if (image == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(image, width, height);
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_COVER);
  if (file_exists(path)) {
    lv_image_set_src(image, path);
  } else {
    lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_align(image, align, x, y);
  return image;
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
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
}

void WatchfacePage::on_will_disappear() {}

lv_obj_t* WatchfacePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);

  lv_obj_t* overlay = lv_obj_create(root);
  lv_obj_t* battery_chip = lv_obj_create(overlay);
  battery_label_ = lv_label_create(battery_chip);
  style_stage_ = lv_obj_create(overlay);
  minute_label_ = lv_label_create(overlay);
  status_label_ = lv_label_create(overlay);

  if (overlay == nullptr || battery_chip == nullptr || battery_label_ == nullptr || style_stage_ == nullptr ||
      minute_label_ == nullptr || status_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(overlay);
  lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_center(overlay);

  style_overlay_card(battery_chip, lv_color_hex(0x10192B), LV_OPA_50, 18);
  lv_obj_set_style_pad_hor(battery_chip, 16, 0);
  lv_obj_set_style_pad_ver(battery_chip, 6, 0);
  lv_obj_set_size(battery_chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(battery_chip, LV_ALIGN_TOP_MID, 0, 12);
  ui_prepare_label(battery_label_);
  ui_apply_text(battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xD9E4F5), 0);
  lv_obj_set_style_text_align(battery_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(battery_label_, "--%");

  ui_prepare_box(style_stage_);
  lv_obj_set_size(style_stage_, 220, 168);
  lv_obj_align(style_stage_, LV_ALIGN_CENTER, 0, -2);
  lv_obj_set_style_bg_opa(style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(style_stage_, 0, 0);
  lv_obj_set_style_pad_all(style_stage_, 0, 0);

  renderer_ = create_watchface_style_renderer(config_);
  if (!renderer_ || renderer_->build(style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(minute_label_);
  ui_apply_text(minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(minute_label_, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(minute_label_, 180);
  lv_obj_set_style_text_align(minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(minute_label_, LV_ALIGN_BOTTOM_MID, 0, -28);
  lv_label_set_text(minute_label_, "--");

  ui_prepare_label(status_label_);
  ui_apply_text(status_label_, TextStyle::Tiny);
  lv_obj_set_style_text_color(status_label_, lv_color_hex(0x7C8CA3), 0);
  lv_obj_set_width(status_label_, 220);
  lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(status_label_, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_label_set_text(status_label_, "");

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
                                     renderer_->apply(render_state_);
                                     break;
                                   case InputAction::CrownRotateCW:
                                     if (renderer_ != nullptr &&
                                         renderer_->on_crown_delta(std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (renderer_ != nullptr &&
                                         renderer_->on_crown_delta(-std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));
  return root;
}

void WatchfacePage::apply_time(const TimeModel& model) {
  if (minute_label_ == nullptr || renderer_ == nullptr || status_label_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(minute_label_, "--");
    lv_label_set_text(status_label_, "Waiting for RTC");
    render_state_.hour_digit = -1;
    render_state_.minute_text = "--";
    render_state_.spread_index = config_.spread_index;
    renderer_->apply(render_state_);
    return;
  }

  char minute_buffer[4] = {};
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));
  lv_label_set_text(minute_label_, minute_buffer);
  lv_label_set_text(status_label_,
                    config_.style_id == WatchfaceStyleId::Diffusion ? "" : "Reserved style placeholder");
  render_state_.hour_digit = static_cast<int>(model.hour % 10);
  render_state_.minute_text = minute_buffer;
  render_state_.spread_index = config_.spread_index;
  renderer_->apply(render_state_);
}

void WatchfacePage::apply_battery(const BatteryModel& model) {
  if (battery_label_ == nullptr || renderer_ == nullptr) {
    return;
  }

  char buffer[32] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%%s", static_cast<int>(model.percent), model.charging ? " +" : "");
  lv_label_set_text(battery_label_, buffer);
  render_state_.battery_percent = model.percent;
  render_state_.spread_index = config_.spread_index;
  renderer_->apply(render_state_);
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
  const auto layout = make_home_surface_layout();
  lv_obj_set_size(root, layout.screen_w, layout.screen_h);

  const lv_color_t accent = accent_for_home_shortcut(config_.page_id);
  const lv_color_t panel_bg = color_for_home_shortcut(config_.page_id);
  const bool show_orbit_label = has_text(config_.orbit_label);
  const bool show_subtitle = has_text(config_.subtitle);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* stage = lv_obj_create(root);
  lv_obj_t* hero_card = lv_obj_create(stage);
  lv_obj_t* hero_tag = lv_obj_create(hero_card);
  lv_obj_t* hero_tag_label = lv_label_create(hero_tag);
  lv_obj_t* hero_value = lv_label_create(hero_card);
  lv_obj_t* hero_meta = lv_label_create(hero_card);
  lv_obj_t* metric_grid = lv_obj_create(stage);
  if (title == nullptr || stage == nullptr || hero_card == nullptr || hero_tag == nullptr || hero_tag_label == nullptr ||
      hero_value == nullptr || hero_meta == nullptr || metric_grid == nullptr) {
    return nullptr;
  }

  ui_prepare_label(title);
  ui_apply_text(title, TextStyle::Title);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, layout.safe_margin_x + 6, layout.stage_top + 8);
  lv_label_set_text(title, config_.title);

  if (show_orbit_label) {
    lv_obj_t* orbit_chip = lv_obj_create(root);
    lv_obj_t* orbit_label = orbit_chip == nullptr ? nullptr : lv_label_create(orbit_chip);
    if (orbit_chip == nullptr || orbit_label == nullptr) {
      return nullptr;
    }
    ui_prepare_box(orbit_chip);
    ui_apply_surface(orbit_chip, SurfaceStyle::Chip);
    lv_obj_set_size(orbit_chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_color(orbit_chip, accent, 0);
    lv_obj_set_style_bg_color(orbit_chip, lv_color_mix(panel_bg, lv_color_hex(0x05080F), LV_OPA_60), 0);
    lv_obj_align(orbit_chip, LV_ALIGN_TOP_LEFT, layout.chip_left, layout.chip_top);

    ui_prepare_label(orbit_label);
    ui_apply_text(orbit_label, TextStyle::Accent);
    lv_obj_set_style_text_color(orbit_label, accent, 0);
    lv_label_set_text(orbit_label, config_.orbit_label);
    lv_obj_center(orbit_label);
  }

  style_home_surface_stage(stage,
                           layout.stage_w,
                           layout.stage_h,
                           layout.stage_radius,
                           lv_color_mix(panel_bg, lv_color_hex(0x03060B), LV_OPA_30));
  lv_obj_align(stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);

  ui_prepare_box(hero_card);
  ui_apply_surface(hero_card, SurfaceStyle::Panel);
  lv_obj_set_size(hero_card, layout.stage_w - layout.stage_pad * 2, layout.hero_h);
  lv_obj_align(hero_card, LV_ALIGN_TOP_MID, 0, layout.stage_pad + layout.title_h + layout.stage_gap);
  lv_obj_set_style_bg_color(hero_card, panel_bg, 0);
  lv_obj_set_style_border_color(hero_card, lv_color_mix(accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
  lv_obj_set_style_radius(hero_card, clamp_coord(scale_by_ratio(layout.stage_w, 9, 100), 20, 24), 0);
  lv_obj_set_style_pad_all(hero_card, 12, 0);

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
  lv_obj_set_style_text_font(hero_value, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_color(hero_value, accent, 0);
  set_single_line_label(hero_value, layout.stage_w - layout.stage_pad * 2 - 24);
  lv_obj_align(hero_value, LV_ALIGN_BOTTOM_LEFT, 0, show_subtitle ? -24 : -10);
  lv_label_set_text(hero_value, config_.focus_value);

  ui_prepare_label(hero_meta);
  ui_apply_text(hero_meta, TextStyle::Muted);
  lv_obj_set_style_text_color(hero_meta, lv_color_hex(0xB8C4D6), 0);
  set_single_line_label(hero_meta, layout.stage_w - layout.stage_pad * 2 - 24);
  lv_obj_align(hero_meta, LV_ALIGN_BOTTOM_LEFT, 0, -12);
  lv_label_set_text(hero_meta, config_.subtitle);
  if (!show_subtitle) {
    lv_obj_add_flag(hero_meta, LV_OBJ_FLAG_HIDDEN);
  }

  ui_prepare_box(metric_grid);
  lv_obj_set_size(metric_grid,
                  layout.stage_w - layout.stage_pad * 2,
                  layout.card_h * 2 + layout.card_gap);
  lv_obj_align(metric_grid, LV_ALIGN_BOTTOM_MID, 0, -layout.stage_pad);
  lv_obj_set_layout(metric_grid, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(metric_grid, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(metric_grid, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_all(metric_grid, 0, 0);
  lv_obj_set_style_pad_row(metric_grid, layout.card_gap, 0);
  lv_obj_set_style_pad_column(metric_grid, layout.card_gap, 0);
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
    lv_obj_set_size(card, layout.card_w, layout.card_h);
    lv_obj_set_style_bg_color(card, lv_color_mix(panel_bg, lv_color_hex(0x05080F), LV_OPA_50), 0);
    lv_obj_set_style_border_color(card, lv_color_mix(accent, lv_color_hex(0x203040), LV_OPA_30), 0);
    lv_obj_set_style_pad_all(card, 10, 0);
    lv_obj_set_style_radius(card, clamp_coord(scale_by_ratio(layout.card_w, 17, 100), 16, 20), 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Tiny);
    lv_obj_set_style_text_color(label, lv_color_hex(0xB2C1D4), 0);
    set_single_line_label(label, layout.card_w - 20);
    lv_label_set_text(label, metric.label);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    ui_prepare_label(value);
    ui_apply_text(value, TextStyle::Title);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(value, accent, 0);
    set_single_line_label(value, layout.card_w - 20);
    lv_label_set_text(value, metric.value);
    lv_obj_align(value, LV_ALIGN_BOTTOM_LEFT, 0, -6);

    ui_prepare_label(detail);
    ui_apply_text(detail, TextStyle::Muted);
    lv_obj_set_style_text_color(detail, lv_color_hex(0x7E90A7), 0);
    set_single_line_label(detail, layout.card_w - 20);
    lv_label_set_text(detail, metric.detail);
    if (has_text(metric.detail)) {
      lv_obj_align(detail, LV_ALIGN_TOP_LEFT, 0, 18);
    } else {
      lv_obj_add_flag(detail, LV_OBJ_FLAG_HIDDEN);
    }
  }

  return root;
}

WeatherShortcutPage::WeatherShortcutPage(DataCenter& data_center) : PageBase(data_center) {}

PageId WeatherShortcutPage::id() const {
  return PageId::HomeShortcutWeather;
}

const char* WeatherShortcutPage::name() const {
  return page_name(PageId::HomeShortcutWeather);
}

lv_obj_t* WeatherShortcutPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  const auto layout = make_home_surface_layout();
  lv_obj_set_size(root, layout.screen_w, layout.screen_h);

  const lv_coord_t stage_w = layout.stage_w;
  const lv_coord_t stage_h = layout.stage_h;
  const lv_coord_t hero_x = 0;
  const lv_coord_t hero_y = 15;
  const lv_coord_t hero_w = stage_w;
  const lv_coord_t hero_h = 110;
  const lv_coord_t temp_x = 15;
  const lv_coord_t temp_y = 23;
  const lv_coord_t temp_max_w = 90;
  const lv_coord_t range_x = 28;
  const lv_coord_t range_y = 68;
  const lv_coord_t range_max_w = 76;
  const lv_coord_t hero_icon_x = 140;
  const lv_coord_t hero_icon_y = 18;
  const lv_coord_t hero_icon_size = 73;
  const lv_coord_t sleep_x = 1;
  const lv_coord_t steps_x = 114;
  const lv_coord_t cards_y = 136;
  const lv_coord_t bottom_card_w = 106;
  const lv_coord_t bottom_card_h = 106;
  const lv_coord_t card_radius = 24;
  const lv_coord_t metric_icon_size = 58;
  const lv_coord_t metric_icon_y = 2;
  const lv_coord_t sleep_value_x = 6;
  const lv_coord_t sleep_value_y = 70;
  const lv_coord_t sleep_value_max_w = 72;
  const lv_coord_t steps_value_x = 2;
  const lv_coord_t steps_value_y = 70;
  const lv_coord_t steps_value_max_w = 72;

  const lv_color_t stage_bg = lv_color_hex(0x040812);
  const lv_color_t weather_bg = lv_color_hex(0x0E2B56);
  const lv_color_t weather_accent = lv_color_hex(0x7EC8FF);
  const lv_color_t sleep_bg = lv_color_hex(0x3A1B69);
  const lv_color_t sleep_accent = lv_color_hex(0xC06CFF);
  const lv_color_t steps_bg = lv_color_hex(0x054A42);
  const lv_color_t steps_accent = lv_color_hex(0x19F57A);

  lv_obj_t* stage = lv_obj_create(root);
  lv_obj_t* hero_card = lv_obj_create(stage);
  lv_obj_t* sleep_card = lv_obj_create(stage);
  lv_obj_t* steps_card = lv_obj_create(stage);
  lv_obj_t* pager = lv_obj_create(root);
  if (stage == nullptr || hero_card == nullptr || sleep_card == nullptr || steps_card == nullptr || pager == nullptr) {
    return nullptr;
  }

  style_home_surface_stage(stage, layout.stage_w, layout.stage_h, layout.stage_radius, stage_bg);
  lv_obj_align(stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);
  lv_obj_set_style_shadow_width(stage, 0, 0);
  lv_obj_set_style_shadow_opa(stage, LV_OPA_TRANSP, 0);

  ui_prepare_box(hero_card);
  ui_apply_surface(hero_card, SurfaceStyle::Panel);
  lv_obj_set_size(hero_card, hero_w, hero_h);
  lv_obj_align(hero_card, LV_ALIGN_TOP_LEFT, hero_x, hero_y);
  lv_obj_set_style_bg_color(hero_card, weather_bg, 0);
  lv_obj_set_style_border_color(hero_card, lv_color_mix(weather_accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
  lv_obj_set_style_radius(hero_card, 28, 0);
  lv_obj_set_style_pad_all(hero_card, 0, 0);

  lv_obj_t* temp_label = lv_label_create(hero_card);
  lv_obj_t* range_label = lv_label_create(hero_card);
  lv_obj_t* weather_icon_image = lv_image_create(hero_card);
  lv_obj_t* icon_sun = lv_obj_create(hero_card);
  lv_obj_t* icon_cloud_back = lv_obj_create(hero_card);
  lv_obj_t* icon_cloud_front = lv_obj_create(hero_card);
  if (temp_label == nullptr || range_label == nullptr || weather_icon_image == nullptr || icon_sun == nullptr ||
      icon_cloud_back == nullptr || icon_cloud_front == nullptr) {
    return nullptr;
  }

  ui_prepare_label(temp_label);
  ui_apply_text(temp_label, TextStyle::Hero);
  lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_34, 0);
  lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
  set_single_line_label(temp_label, temp_max_w);
  lv_label_set_text(temp_label, "23C");
  lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, temp_x, temp_y);

  ui_prepare_label(range_label);
  ui_apply_text(range_label, TextStyle::Title);
  lv_obj_set_style_text_font(range_label, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(range_label, lv_color_hex(0xC8D7EE), 0);
  set_single_line_label(range_label, range_max_w);
  lv_label_set_text(range_label, "30C / 18C");
  lv_obj_align(range_label, LV_ALIGN_TOP_LEFT, range_x, range_y);

  const char* weather_icon_path = weather_icon_asset_path();
  const bool has_weather_icon_asset = file_exists(weather_icon_path);
  if (has_weather_icon_asset) {
    lv_image_set_src(weather_icon_image, weather_icon_path);
    lv_obj_set_size(weather_icon_image, hero_icon_size, hero_icon_size);
    lv_image_set_inner_align(weather_icon_image, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_align(weather_icon_image, LV_ALIGN_TOP_LEFT, hero_icon_x, hero_icon_y);
    for (lv_obj_t* part : {icon_sun, icon_cloud_back, icon_cloud_front}) {
      lv_obj_add_flag(part, LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    lv_obj_add_flag(weather_icon_image, LV_OBJ_FLAG_HIDDEN);
  }

  ui_prepare_box(icon_sun);
  lv_obj_set_size(icon_sun, hero_icon_size, hero_icon_size);
  lv_obj_set_style_bg_color(icon_sun, lv_color_hex(0xFFD86B), 0);
  lv_obj_set_style_bg_opa(icon_sun, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(icon_sun, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(icon_sun, LV_ALIGN_TOP_LEFT, hero_icon_x, hero_icon_y);

  ui_prepare_box(icon_cloud_back);
  lv_obj_set_size(icon_cloud_back, hero_icon_size + 10, hero_icon_size - 2);
  lv_obj_set_style_bg_color(icon_cloud_back, lv_color_hex(0xEAF2FF), 0);
  lv_obj_set_style_bg_opa(icon_cloud_back, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(icon_cloud_back, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(icon_cloud_back, LV_ALIGN_TOP_LEFT, hero_icon_x - 9, hero_icon_y + 22);

  ui_prepare_box(icon_cloud_front);
  lv_obj_set_size(icon_cloud_front, hero_icon_size + 16, hero_icon_size);
  lv_obj_set_style_bg_color(icon_cloud_front, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_bg_opa(icon_cloud_front, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(icon_cloud_front, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(icon_cloud_front, LV_ALIGN_TOP_LEFT, hero_icon_x + 9, hero_icon_y + 26);

  for (lv_obj_t* card : {sleep_card, steps_card}) {
    ui_prepare_box(card);
    ui_apply_surface(card, SurfaceStyle::PanelSubtle);
    lv_obj_set_size(card, bottom_card_w, bottom_card_h);
    lv_obj_set_style_radius(card, card_radius, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
  }
  lv_obj_align(sleep_card, LV_ALIGN_TOP_LEFT, sleep_x, cards_y);
  lv_obj_align(steps_card, LV_ALIGN_TOP_LEFT, steps_x, cards_y);
  lv_obj_set_style_bg_color(sleep_card, sleep_bg, 0);
  lv_obj_set_style_border_color(sleep_card, lv_color_mix(sleep_accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
  lv_obj_set_style_bg_color(steps_card, steps_bg, 0);
  lv_obj_set_style_border_color(steps_card, lv_color_mix(steps_accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);

  auto build_metric_card = [&](lv_obj_t* card, const char* value, bool sleep_icon, bool emphasize)
      -> bool {
    lv_obj_t* icon_root = lv_obj_create(card);
    lv_obj_t* icon_image = lv_image_create(icon_root);
    lv_obj_t* value_label = lv_label_create(card);
    if (icon_root == nullptr || icon_image == nullptr || value_label == nullptr) {
      return false;
    }

    ui_prepare_box(icon_root);
    lv_obj_set_size(icon_root, metric_icon_size, metric_icon_size);
    lv_obj_set_style_bg_opa(icon_root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(icon_root, 0, 0);
    lv_obj_align(icon_root, LV_ALIGN_TOP_LEFT, 0, metric_icon_y);

    const char* icon_path = sleep_icon ? sleep_icon_asset_path() : steps_icon_asset_path();
    lv_obj_set_size(icon_image, metric_icon_size, metric_icon_size);
    lv_image_set_inner_align(icon_image, LV_IMAGE_ALIGN_CONTAIN);
    lv_image_set_src(icon_image, icon_path);
    lv_obj_center(icon_image);

    if (sleep_icon) {
      lv_obj_t* bed_frame = lv_obj_create(icon_root);
      lv_obj_t* bed_head = lv_obj_create(icon_root);
      lv_obj_t* bed_pillow = lv_obj_create(icon_root);
      if (bed_frame == nullptr || bed_head == nullptr || bed_pillow == nullptr) {
        return false;
      }
      const bool has_icon_asset = file_exists(icon_path);
      if (has_icon_asset) {
        lv_obj_clear_flag(icon_image, LV_OBJ_FLAG_HIDDEN);
        lv_obj_center(icon_image);
      } else {
        lv_obj_add_flag(icon_image, LV_OBJ_FLAG_HIDDEN);
      }
      for (lv_obj_t* part : {bed_frame, bed_head, bed_pillow}) {
        ui_prepare_box(part);
        lv_obj_set_style_bg_color(part, lv_color_hex(0xF8FBFF), 0);
        lv_obj_set_style_bg_opa(part, LV_OPA_90, 0);
        lv_obj_set_style_border_width(part, 0, 0);
        if (has_icon_asset) {
          lv_obj_add_flag(part, LV_OBJ_FLAG_HIDDEN);
        }
      }
      lv_obj_set_size(bed_frame, 21, 6);
      lv_obj_set_style_radius(bed_frame, 3, 0);
      lv_obj_align(bed_frame, LV_ALIGN_BOTTOM_MID, 0, -8);

      lv_obj_set_size(bed_head, 5, 14);
      lv_obj_set_style_radius(bed_head, 3, 0);
      lv_obj_align(bed_head, LV_ALIGN_TOP_LEFT, 8, 16);

      lv_obj_set_size(bed_pillow, 14, 7);
      lv_obj_set_style_radius(bed_pillow, 4, 0);
      lv_obj_align(bed_pillow, LV_ALIGN_TOP_LEFT, 18, 18);
    } else {
      lv_obj_t* foot = lv_obj_create(icon_root);
      lv_obj_t* toe1 = lv_obj_create(icon_root);
      lv_obj_t* toe2 = lv_obj_create(icon_root);
      lv_obj_t* toe3 = lv_obj_create(icon_root);
      if (foot == nullptr || toe1 == nullptr || toe2 == nullptr || toe3 == nullptr) {
        return false;
      }
      const bool has_icon_asset = file_exists(icon_path);
      if (has_icon_asset) {
        lv_obj_clear_flag(icon_image, LV_OBJ_FLAG_HIDDEN);
        lv_obj_center(icon_image);
      } else {
        lv_obj_add_flag(icon_image, LV_OBJ_FLAG_HIDDEN);
      }
      for (lv_obj_t* part : {foot, toe1, toe2, toe3}) {
        ui_prepare_box(part);
        lv_obj_set_style_bg_color(part, lv_color_hex(0xF8FBFF), 0);
        lv_obj_set_style_bg_opa(part, LV_OPA_90, 0);
        lv_obj_set_style_border_width(part, 0, 0);
        lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
        if (has_icon_asset) {
          lv_obj_add_flag(part, LV_OBJ_FLAG_HIDDEN);
        }
      }
      lv_obj_set_size(foot, 10, 18);
      lv_obj_align(foot, LV_ALIGN_TOP_LEFT, 20, 13);
      lv_obj_set_style_transform_rotation(foot, -220, 0);

      lv_obj_set_size(toe1, 4, 4);
      lv_obj_align(toe1, LV_ALIGN_TOP_LEFT, 33, 11);
      lv_obj_set_size(toe2, 4, 4);
      lv_obj_align(toe2, LV_ALIGN_TOP_LEFT, 38, 15);
      lv_obj_set_size(toe3, 4, 4);
      lv_obj_align(toe3, LV_ALIGN_TOP_LEFT, 42, 21);
    }

    ui_prepare_label(value_label);
    ui_apply_text(value_label, TextStyle::HeroSoft);
    lv_obj_set_style_text_font(value_label, emphasize ? &lv_font_montserrat_16 : &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(value_label, lv_color_hex(0xFFFFFF), 0);
    set_single_line_label(value_label, emphasize ? steps_value_max_w : sleep_value_max_w);
    lv_label_set_text(value_label, value);
    lv_obj_align(value_label,
                 LV_ALIGN_TOP_LEFT,
                 emphasize ? steps_value_x : sleep_value_x,
                 emphasize ? steps_value_y : sleep_value_y);
    return true;
  };

  if (!build_metric_card(sleep_card, "7h 36m", true, false) ||
      !build_metric_card(steps_card, "7645", false, true)) {
    return nullptr;
  }

  ui_prepare_box(pager);
  ui_set_flex_row(pager, 0, 8, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(pager, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(pager, LV_OPA_TRANSP, 0);
  lv_obj_align(pager, LV_ALIGN_BOTTOM_MID, 0, -layout.pager_bottom);

  const std::array<bool, 4> active_dots {{false, false, false, true}};
  for (bool active : active_dots) {
    lv_obj_t* dot = create_pager_dot(pager, lv_color_hex(0xFFFFFF), LV_OPA_10, active);
    if (dot == nullptr) {
      return nullptr;
    }
    if (!active) {
      lv_obj_set_style_bg_color(dot, lv_color_hex(0xB9C5D5), 0);
    }
  }

  return root;
}

PaymentsShortcutPage::PaymentsShortcutPage(DataCenter& data_center) : PageBase(data_center) {}

PageId PaymentsShortcutPage::id() const {
  return PageId::HomeShortcutPayments;
}

const char* PaymentsShortcutPage::name() const {
  return page_name(PageId::HomeShortcutPayments);
}

lv_obj_t* PaymentsShortcutPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  const auto layout = make_home_surface_layout();
  lv_obj_set_size(root, layout.screen_w, layout.screen_h);

  const lv_color_t stage_bg = lv_color_hex(0x040812);
  const lv_color_t tile_bg = lv_color_hex(0x0C1424);
  const lv_color_t tile_border = lv_color_hex(0x1E375A);
  const lv_color_t music_bg = lv_color_hex(0x4BE7E8);
  const lv_color_t music_fg = lv_color_hex(0xF7FFFE);

  lv_obj_t* stage = create_home_stage_root(root, layout, stage_bg);
  lv_obj_t* pager = create_home_pager(root, layout, 0);
  if (stage == nullptr || pager == nullptr) {
    return nullptr;
  }

  const lv_coord_t small_w = 106;
  const lv_coord_t small_h = 106;
  const lv_coord_t top_y = 15;
  const lv_coord_t music_y = 136;
  const lv_coord_t card_gap = 8;

  lv_obj_t* alipay_card = lv_obj_create(stage);
  lv_obj_t* wechat_card = lv_obj_create(stage);
  lv_obj_t* music_card = lv_obj_create(stage);
  if (alipay_card == nullptr || wechat_card == nullptr || music_card == nullptr) {
    return nullptr;
  }

  for (lv_obj_t* card : {alipay_card, wechat_card}) {
    ui_prepare_box(card);
    ui_apply_surface(card, SurfaceStyle::PanelSubtle);
    lv_obj_set_size(card, small_w, small_h);
    lv_obj_set_style_radius(card, 24, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_set_style_bg_color(card, tile_bg, 0);
    lv_obj_set_style_border_color(card, tile_border, 0);
  }
  lv_obj_align(alipay_card, LV_ALIGN_TOP_LEFT, 0, top_y);
  lv_obj_align(wechat_card, LV_ALIGN_TOP_LEFT, small_w + card_gap, top_y);

  auto build_payment_tile = [&](lv_obj_t* card,
                                const char* icon_path,
                                lv_coord_t icon_x,
                                lv_coord_t icon_y,
                                lv_coord_t icon_size,
                                const char* label_text,
                                lv_coord_t label_x,
                                lv_coord_t label_y,
                                lv_coord_t label_w) -> bool {
    lv_obj_t* icon = create_contain_image(card, icon_path, icon_size, icon_size, LV_ALIGN_TOP_LEFT, icon_x, icon_y);
    lv_obj_t* label = lv_label_create(card);
    if (icon == nullptr || label == nullptr) {
      return false;
    }
    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_width(label, label_w);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, label_text);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, label_x, label_y);
    return true;
  };

  const char* wechat_icon_path = payment_wechat_green_asset_path();
  if (!file_exists(wechat_icon_path)) {
    wechat_icon_path = payment_wechat_asset_path();
  }

  if (!build_payment_tile(alipay_card, payment_alipay_asset_path(), 2, 2, 49, "Alipay", 16, 72, 72) ||
      !build_payment_tile(wechat_card, wechat_icon_path, 7, 5, 40, "WeChat\nPay", 16, 58, 74)) {
    return nullptr;
  }

  ui_prepare_box(music_card);
  ui_apply_surface(music_card, SurfaceStyle::Panel);
  lv_obj_set_size(music_card, 220, 106);
  lv_obj_align(music_card, LV_ALIGN_TOP_LEFT, 0, music_y);
  lv_obj_set_style_radius(music_card, 24, 0);
  lv_obj_set_style_pad_all(music_card, 0, 0);
  lv_obj_set_style_bg_color(music_card, music_bg, 0);
  lv_obj_set_style_border_width(music_card, 0, 0);

  lv_obj_t* music_status = lv_label_create(music_card);
  lv_obj_t* music_prev = lv_label_create(music_card);
  lv_obj_t* music_play = lv_label_create(music_card);
  lv_obj_t* music_next = lv_label_create(music_card);
  if (music_status == nullptr || music_prev == nullptr || music_play == nullptr || music_next == nullptr) {
    return nullptr;
  }

  ui_prepare_label(music_status);
  ui_apply_text(music_status, TextStyle::Title);
  lv_obj_set_style_text_font(music_status, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(music_status, music_fg, 0);
  lv_obj_set_width(music_status, 180);
  lv_label_set_long_mode(music_status, LV_LABEL_LONG_DOT);
  lv_label_set_text(music_status, "Phone not playing");
  lv_obj_align(music_status, LV_ALIGN_TOP_LEFT, 16, 16);

  for (lv_obj_t* control : {music_prev, music_play, music_next}) {
    ui_prepare_label(control);
    ui_apply_text(control, TextStyle::Hero);
    lv_obj_set_style_text_color(control, music_fg, 0);
  }
  lv_obj_set_style_text_font(music_prev, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_font(music_play, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_font(music_next, &lv_font_montserrat_18, 0);
  lv_label_set_text(music_prev, LV_SYMBOL_PREV);
  lv_label_set_text(music_play, LV_SYMBOL_PLAY);
  lv_label_set_text(music_next, LV_SYMBOL_NEXT);
  lv_obj_align(music_prev, LV_ALIGN_BOTTOM_LEFT, 24, -16);
  lv_obj_align(music_play, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(music_next, LV_ALIGN_BOTTOM_RIGHT, -24, -16);

  return root;
}

NfcShortcutPage::NfcShortcutPage(DataCenter& data_center) : PageBase(data_center) {}

PageId NfcShortcutPage::id() const {
  return PageId::HomeShortcutNfc;
}

const char* NfcShortcutPage::name() const {
  return page_name(PageId::HomeShortcutNfc);
}

lv_obj_t* NfcShortcutPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  const auto layout = make_home_surface_layout();
  lv_obj_set_size(root, layout.screen_w, layout.screen_h);

  const lv_color_t stage_bg = lv_color_hex(0x040812);
  const lv_color_t card_bg = lv_color_hex(0x0A101A);

  lv_obj_t* stage = create_home_stage_root(root, layout, stage_bg);
  lv_obj_t* pager = create_home_pager(root, layout, 1);
  if (stage == nullptr || pager == nullptr) {
    return nullptr;
  }

  lv_obj_t* title = lv_label_create(stage);
  lv_obj_t* subtitle = lv_label_create(stage);
  lv_obj_t* card = lv_obj_create(stage);
  if (title == nullptr || subtitle == nullptr || card == nullptr) {
    return nullptr;
  }

  ui_prepare_label(title);
  ui_apply_text(title, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_width(title, 93);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(title, "School");
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 57, 18);

  ui_prepare_label(subtitle);
  ui_apply_text(subtitle, TextStyle::Title);
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x3B82F6), 0);
  lv_obj_set_width(subtitle, 151);
  lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(subtitle, "Tap card reader");
  lv_obj_align(subtitle, LV_ALIGN_TOP_LEFT, 30, 47);

  ui_prepare_box(card);
  ui_apply_surface(card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(card, 208, 120);
  lv_obj_align(card, LV_ALIGN_TOP_LEFT, 6, 75);
  lv_obj_set_style_radius(card, 28, 0);
  lv_obj_set_style_pad_all(card, 0, 0);
  lv_obj_set_style_bg_color(card, card_bg, 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_clip_corner(card, true, 0);

  const char* nfc_asset_path = nfc_school_card_inner_asset_path();
  if (!file_exists(nfc_asset_path)) {
    nfc_asset_path = nfc_school_card_asset_path();
  }

  if (create_cover_image(card, nfc_asset_path, 209, 124, LV_ALIGN_TOP_LEFT, -2, -4) == nullptr) {
    return nullptr;
  }

  return root;
}

HealthShortcutPage::HealthShortcutPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HealthShortcutPage::id() const {
  return PageId::HomeShortcutHealth;
}

const char* HealthShortcutPage::name() const {
  return page_name(PageId::HomeShortcutHealth);
}

lv_obj_t* HealthShortcutPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  const auto layout = make_home_surface_layout();
  lv_obj_set_size(root, layout.screen_w, layout.screen_h);

  const lv_color_t stage_bg = lv_color_hex(0x040812);
  lv_obj_t* stage = create_home_stage_root(root, layout, stage_bg);
  lv_obj_t* pager = create_home_pager(root, layout, 2);
  if (stage == nullptr || pager == nullptr) {
    return nullptr;
  }

  struct HealthTile {
    const char* icon_path;
    const char* value;
    lv_color_t bg;
    bool emphasize;
    lv_coord_t icon_x;
    lv_coord_t icon_y;
    lv_coord_t icon_size;
  };

  const std::array<HealthTile, 4> tiles {{
      {health_heart_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 36},
      {health_spo2_asset_path(), "--", lv_color_hex(0xFF4F72), false, 18, 12, 40},
      {health_breathe_asset_path(), "Breathe", lv_color_hex(0x4DBDFF), true, 14, 13, 39},
      {health_stress_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 44},
  }};

  const lv_coord_t tile_w = 106;
  const lv_coord_t tile_h = 106;
  const lv_coord_t tile_gap = 8;
  const lv_coord_t start_y = 15;

  for (std::size_t index = 0; index < tiles.size(); ++index) {
    const auto& tile = tiles[index];
    const lv_coord_t x = static_cast<lv_coord_t>((index % 2) * (tile_w + tile_gap));
    const lv_coord_t y = static_cast<lv_coord_t>(start_y + (index / 2) * (tile_h + tile_gap));
    lv_obj_t* card = lv_obj_create(stage);
    if (card == nullptr) {
      return nullptr;
    }
    ui_prepare_box(card);
    ui_apply_surface(card, SurfaceStyle::PanelSubtle);
    lv_obj_set_size(card, tile_w, tile_h);
    lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_radius(card, 24, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_set_style_bg_color(card, tile.bg, 0);
    lv_obj_set_style_border_width(card, 0, 0);

    if (create_contain_image(card, tile.icon_path, tile.icon_size, tile.icon_size, LV_ALIGN_TOP_LEFT, tile.icon_x, tile.icon_y) ==
        nullptr) {
      return nullptr;
    }

    lv_obj_t* value = lv_label_create(card);
    if (value == nullptr) {
      return nullptr;
    }
    ui_prepare_label(value);
    ui_apply_text(value, TextStyle::Title);
    lv_obj_set_style_text_font(value, tile.emphasize ? &lv_font_montserrat_16 : &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(value, tile.emphasize ? lv_color_hex(0xD7FBFF) : lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_width(value, 78);
    lv_label_set_long_mode(value, LV_LABEL_LONG_DOT);
    lv_label_set_text(value, tile.value);
    lv_obj_align(value, LV_ALIGN_BOTTOM_LEFT, 12, -14);
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
