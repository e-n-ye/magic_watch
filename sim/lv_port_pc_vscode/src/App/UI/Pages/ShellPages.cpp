#include "App/UI/Pages/ShellPages.h"

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"
#include "lvgl/src/misc/lv_fs.h"

#include <chrono>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace twsim::app {

namespace {

using shell_font::cjk_font_14;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;
using shell_font::cjk_font_72;

constexpr const char* kTextClear = "\xE6\xB8\x85\xE7\xA9\xBA";
constexpr const char* kTextConfirm = "\xE7\xA1\xAE\xE8\xAE\xA4";
constexpr const char* kTextCancel = "\xE5\x8F\x96\xE6\xB6\x88";
constexpr const char* kTextNoMessages = "\xE6\x9A\x82\xE6\x97\xA0\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextDismiss = "\xE5\xBF\xBD\xE7\x95\xA5";
constexpr const char* kTextNotificationClearConfirmBody = "\xE6\xB8\x85\xE7\xA9\xBA\xE5\x90\x8E\xE5\xB0\x86\xE7\xA7\xBB\xE9\x99\xA4\xE5\xBD\x93\xE5\x89\x8D\xE9\x80\x9A\xE7\x9F\xA5\xE5\x88\x97\xE8\xA1\xA8\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE6\xB8\x85\xE7\xA9\xBA\xEF\xBC\x9F";
constexpr const char* kTextLongBatteryConfirmBody =
    "长续航模式开启后仅保留时间、计步、NFC功能。旋转表冠或充电可退出此功能。确定开启？";
constexpr lv_coord_t kNotificationsCloseDragThreshold = 96;
constexpr lv_coord_t kNotificationsCloseFlickThreshold = 20;
constexpr lv_coord_t kNotificationsMaxDragOffset = 220;
constexpr lv_coord_t kNotificationsOpenCommitThreshold = 148;
constexpr lv_coord_t kNotificationsSheetY = 14;
constexpr lv_coord_t kNotificationsSheetWidth = 228;
constexpr lv_coord_t kNotificationsSheetHeight = 268;
constexpr lv_coord_t kNotificationCardSwipeStartThreshold = 14;
constexpr lv_coord_t kNotificationCardSwipeDismissThreshold = 112;
constexpr lv_coord_t kNotificationCardSwipeMaxOffset = 138;
constexpr lv_coord_t kQuickSettingsCloseDragThreshold = 96;
constexpr lv_coord_t kQuickSettingsCloseFlickThreshold = 20;
constexpr lv_coord_t kQuickSettingsMaxDragOffset = 220;
constexpr lv_coord_t kQuickSettingsOpenCommitThreshold = 148;
constexpr lv_coord_t kQuickSettingsSheetY = 46;
constexpr lv_coord_t kQuickSettingsSheetWidth = 228;
constexpr lv_coord_t kQuickSettingsSheetHeight = 228;
constexpr lv_coord_t kLauncherScrollTop = 10;
constexpr lv_coord_t kLauncherScrollBottom = 0;
constexpr lv_coord_t kLauncherScrollInset = 8;
constexpr lv_coord_t kLauncherScrollPadTop = 10;
constexpr lv_coord_t kLauncherScrollPadBottom = 12;
constexpr lv_coord_t kLauncherScrollGap = 10;
constexpr lv_coord_t kLauncherSectionPad = 10;
constexpr lv_coord_t kLauncherSectionGap = 8;
constexpr lv_coord_t kLauncherSectionHeaderBottom = 8;
constexpr lv_coord_t kLauncherTileWidth = 60;
constexpr lv_coord_t kLauncherTileHeight = 84;
constexpr lv_coord_t kLauncherMultiColumnTileHeight = 58;
constexpr lv_coord_t kLauncherIconSize = 52;
constexpr lv_coord_t kLauncherColumnGap = 8;
constexpr lv_coord_t kLauncherRowGap = 10;
constexpr lv_coord_t kLauncherMultiColumnElasticSpacerHeight = 40;
constexpr lv_coord_t kLauncherListRowHeight = 62;
constexpr lv_coord_t kLauncherListIconSize = 38;
constexpr std::int32_t kLauncherCrownDragStep = 28;
constexpr std::uint32_t kLauncherCrownReleaseDelayMs = 95U;
constexpr lv_coord_t kWeatherSectionGap = 10;
constexpr std::int32_t kWeatherSectionCount = 5;
constexpr lv_coord_t kStepsScrollInset = 8;
constexpr lv_coord_t kStepsScrollTop = 8;
constexpr lv_coord_t kStepsScrollBottom = 0;
constexpr lv_coord_t kClickDragThreshold = 12;
constexpr lv_coord_t kHomePagerStep = 15;

const char* quick_settings_log_path() {
  static const std::string path = (std::filesystem::current_path() / "quicksettings_debug.log").string();
  return path.c_str();
}

void append_quick_settings_log(const char* phase,
                               std::size_t index,
                               const char* kind,
                               bool suppress_next_click,
                               bool guard_allows,
                               lv_obj_t* target,
                               lv_event_t* event) {
  std::FILE* file = std::fopen(quick_settings_log_path(), "a");
  if (file == nullptr) {
    return;
  }

  lv_point_t point {0, 0};
  if (event != nullptr) {
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &point);
    }
  }

  const auto now = std::chrono::steady_clock::now().time_since_epoch();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  std::fprintf(file,
               "[%lld] %s idx=%zu kind=%s suppress=%d guard=%d target=%p x=%d y=%d\n",
               static_cast<long long>(ms),
               phase != nullptr ? phase : "Unknown",
               index,
               kind != nullptr ? kind : "Unknown",
               suppress_next_click ? 1 : 0,
               guard_allows ? 1 : 0,
               static_cast<void*>(target),
               static_cast<int>(point.x),
               static_cast<int>(point.y));
  std::fclose(file);
}

void reset_quick_settings_log() {
  std::FILE* file = std::fopen(quick_settings_log_path(), "w");
  if (file == nullptr) {
    return;
  }
  std::fclose(file);
}

struct ClickGestureState {
  lv_point_t press_point {0, 0};
  bool moved {false};
};

std::unordered_map<lv_obj_t*, ClickGestureState>& click_gesture_states() {
  static std::unordered_map<lv_obj_t*, ClickGestureState> states;
  return states;
}

void click_guard_event_cb(lv_event_t* event) {
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr) {
    return;
  }

  auto& states = click_gesture_states();
  switch (lv_event_get_code(event)) {
    case LV_EVENT_PRESSED: {
      lv_point_t point {0, 0};
      if (lv_indev_t* indev = lv_event_get_indev(event)) {
        lv_indev_get_point(indev, &point);
      }
      states[target] = {point, false};
      break;
    }
    case LV_EVENT_PRESSING: {
      auto it = states.find(target);
      if (it == states.end()) {
        break;
      }
      lv_point_t point {0, 0};
      if (lv_indev_t* indev = lv_event_get_indev(event)) {
        lv_indev_get_point(indev, &point);
      }
      const lv_coord_t dx = point.x - it->second.press_point.x;
      const lv_coord_t dy = point.y - it->second.press_point.y;
      if (LV_ABS(dx) >= kClickDragThreshold || LV_ABS(dy) >= kClickDragThreshold) {
        it->second.moved = true;
      }
      break;
    }
    case LV_EVENT_PRESS_LOST: {
      auto it = states.find(target);
      if (it != states.end()) {
        it->second.moved = true;
      }
      break;
    }
    case LV_EVENT_DELETE:
      states.erase(target);
      break;
    default:
      break;
  }
}

void attach_click_guard(lv_obj_t* object) {
  if (object == nullptr) {
    return;
  }
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESS_LOST, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_DELETE, nullptr);
}

bool click_guard_allows(lv_obj_t* object) {
  if (object == nullptr) {
    return false;
  }

  auto& states = click_gesture_states();
  const auto it = states.find(object);
  if (it == states.end()) {
    return true;
  }

  const bool allows = !it->second.moved;
  states.erase(it);
  return allows;
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

void apply_compact_time_label(lv_obj_t* label, const std::optional<TimeModel>& model) {
  if (label == nullptr) {
    return;
  }

  char buffer[8] = "--:--";
  if (model.has_value() && model->valid) {
    std::snprintf(buffer, sizeof(buffer), "%02u:%02u", model->hour, model->minute);
  }
  lv_label_set_text(label, buffer);
}

void set_translate_y_exec(void* obj, int32_t value) {
  if (obj == nullptr) {
    return;
  }
  lv_obj_set_style_translate_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value), 0);
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

const char* preferred_wechat_icon_asset_path() {
  return payment_wechat_green_asset_path() != nullptr ? payment_wechat_green_asset_path() : payment_wechat_asset_path();
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

struct AppVisualSpec {
  PageId target;
  const char* label;
  const char* icon_text;
  const char* icon_asset_path;
  std::uint32_t icon_bg;
  std::uint32_t icon_fg;
};

const AppVisualSpec* find_app_visual_spec(PageId target) {
  static const std::array<AppVisualSpec, 11> specs {{
      {PageId::SettingsHome, "Settings", LV_SYMBOL_SETTINGS, nullptr, 0x201C22, 0xC7CED8},
      {PageId::AppWeather, "Weather", nullptr, weather_icon_asset_path(), 0x1FA3E5, 0xF8FAFC},
      {PageId::Pedometer, "Steps", nullptr, steps_icon_asset_path(), 0x2A5A2E, 0xF8FAFC},
      {PageId::AppHeartRate, "Heart", nullptr, health_heart_asset_path(), 0xFAFAFA, 0x0F172A},
      {PageId::AppBloodOxygen, "SpO2", nullptr, health_spo2_asset_path(), 0x26D9D6, 0xF8FAFC},
      {PageId::AppSleep, "Sleep", nullptr, sleep_icon_asset_path(), 0x9162C0, 0xF8FAFC},
      {PageId::AppStress, "Stress", nullptr, health_stress_asset_path(), 0x23433A, 0xF8FAFC},
      {PageId::AppBreathing, "Breathe", nullptr, health_breathe_asset_path(), 0x8F8F8F, 0xF8FAFC},
      {PageId::AppNfc, "NFC", "CARD", nullptr, 0x8E9B50, 0xF8FAFC},
      {PageId::AppAlipay, "Alipay", nullptr, payment_alipay_asset_path(), 0x008CFF, 0xF8FAFC},
      {PageId::AppWeChatPay, "WeChat", nullptr, preferred_wechat_icon_asset_path(), 0xEBEBEB, 0x0F172A},
  }};

  const auto it = std::find_if(specs.begin(), specs.end(), [target](const AppVisualSpec& spec) { return spec.target == target; });
  return it != specs.end() ? &(*it) : nullptr;
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

std::size_t decimal_digits(std::uint32_t value) {
  std::size_t digits = 1;
  while (value >= 10U) {
    value /= 10U;
    ++digits;
  }
  return digits;
}

const lv_font_t* steps_overview_value_font(std::uint32_t value) {
  return decimal_digits(value) >= 4 ? &lv_font_montserrat_24 : &lv_font_montserrat_28;
}

const lv_font_t* steps_card_value_font(std::uint32_t value) {
  return decimal_digits(value) >= 4 ? &lv_font_montserrat_28 : &lv_font_montserrat_42;
}

const lv_font_t* home_shortcut_steps_font(std::uint32_t value) {
  return decimal_digits(value) >= 5 ? &lv_font_montserrat_12 : &lv_font_montserrat_14;
}

void set_content_width_label(lv_obj_t* label, const char* text, const lv_font_t* font, std::uint32_t color) {
  if (label == nullptr) {
    return;
  }
  lv_obj_set_width(label, LV_SIZE_CONTENT);
  lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_text(label, text);
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

lv_obj_t* create_app_round_icon(lv_obj_t* parent, const AppVisualSpec& spec, lv_coord_t size) {
  lv_obj_t* icon_root = lv_obj_create(parent);
  if (icon_root == nullptr) {
    return nullptr;
  }

  ui_prepare_box(icon_root);
  lv_obj_set_size(icon_root, size, size);
  lv_obj_set_style_radius(icon_root, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon_root, lv_color_hex(spec.icon_bg), 0);
  lv_obj_set_style_bg_opa(icon_root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon_root, 0, 0);
  lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_CLICKABLE);

  const char* asset_path = spec.icon_asset_path;
  if (file_exists(asset_path)) {
    lv_obj_t* image = lv_image_create(icon_root);
    if (image == nullptr) {
      return nullptr;
    }
    const lv_coord_t image_size = static_cast<lv_coord_t>(size - 12);
    lv_obj_set_size(image, image_size, image_size);
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
    lv_image_set_src(image, asset_path);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(image);
    return icon_root;
  }

  if (spec.target == PageId::AppNfc) {
    lv_obj_t* wallet_body = lv_obj_create(icon_root);
    lv_obj_t* wallet_flap = lv_obj_create(icon_root);
    lv_obj_t* wallet_slot = lv_obj_create(icon_root);
    if (wallet_body == nullptr || wallet_flap == nullptr || wallet_slot == nullptr) {
      return nullptr;
    }

    for (lv_obj_t* part : {wallet_body, wallet_flap, wallet_slot}) {
      ui_prepare_box(part);
      lv_obj_set_style_border_width(part, 0, 0);
      lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_set_size(wallet_body, 32, 22);
    lv_obj_set_style_bg_color(wallet_body, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(wallet_body, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_body, 6, 0);
    lv_obj_align(wallet_body, LV_ALIGN_CENTER, 0, 4);

    lv_obj_set_size(wallet_flap, 30, 8);
    lv_obj_set_style_bg_color(wallet_flap, lv_color_hex(0xFDE68A), 0);
    lv_obj_set_style_bg_opa(wallet_flap, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_flap, 4, 0);
    lv_obj_align(wallet_flap, LV_ALIGN_CENTER, 0, -6);

    lv_obj_set_size(wallet_slot, 8, 3);
    lv_obj_set_style_bg_color(wallet_slot, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_bg_opa(wallet_slot, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_slot, 2, 0);
    lv_obj_align(wallet_slot, LV_ALIGN_CENTER, 6, 4);
    return icon_root;
  }

  lv_obj_t* label = lv_label_create(icon_root);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  ui_apply_text(label, TextStyle::Title);
  lv_obj_set_style_text_font(label,
                             std::strlen(spec.icon_text == nullptr ? "" : spec.icon_text) > 1 ? &lv_font_montserrat_14
                                                                                               : &lv_font_montserrat_18,
                             0);
  lv_obj_set_style_text_color(label, lv_color_hex(spec.icon_fg), 0);
  lv_label_set_text(label, spec.icon_text == nullptr ? "?" : spec.icon_text);
  lv_obj_remove_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(label);
  return icon_root;
}

LauncherLayoutMode current_launcher_layout_mode(DataCenter& data_center) {
  return data_center.display_policy().value_or(DisplayPolicyModel {}).launcher_layout_mode;
}

lv_obj_t* create_launcher_scroll_root(lv_obj_t* root, lv_coord_t screen_w, lv_coord_t screen_h) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(scroll, screen_w - kLauncherScrollInset * 2, screen_h - kLauncherScrollTop - kLauncherScrollBottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, kLauncherScrollTop);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  return scroll;
}

lv_obj_t* create_weather_label(lv_obj_t* parent,
                               const char* text,
                               const lv_font_t* font,
                               std::uint32_t color,
                               lv_coord_t width,
                               lv_label_long_mode_t long_mode = LV_LABEL_LONG_DOT) {
  lv_obj_t* label = lv_label_create(parent);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, long_mode);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_text(label, text);
  return label;
}

lv_obj_t* create_weather_panel(lv_obj_t* parent,
                               lv_coord_t width,
                               lv_coord_t height,
                               std::uint32_t bg,
                               std::uint32_t border,
                               lv_coord_t radius) {
  lv_obj_t* panel = lv_obj_create(parent);
  if (panel == nullptr) {
    return nullptr;
  }
  ui_prepare_box(panel);
  ui_apply_surface(panel, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(panel, width, height);
  lv_obj_set_style_bg_color(panel, lv_color_hex(bg), 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(border), 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_radius(panel, radius, 0);
  lv_obj_set_style_shadow_width(panel, 0, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_TRANSP, 0);
  return panel;
}

lv_obj_t* create_weather_section(lv_obj_t* parent,
                                 const char* title,
                                 const char* time_text,
                                 lv_coord_t width,
                                 lv_coord_t height) {
  lv_obj_t* section = create_weather_panel(parent, width, height, 0x071E3A, 0x12385F, 30);
  if (section == nullptr) {
    return nullptr;
  }
  lv_obj_remove_flag(section, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* title_label = create_weather_label(section, title, cjk_font_16(), 0xF8FAFC, 112);
  lv_obj_t* time_label = create_weather_label(section, time_text, &lv_font_montserrat_16, 0xD8E9FF, 72);
  if (title_label == nullptr || time_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -18, 16);
  return section;
}

lv_obj_t* create_weather_horizontal_row(lv_obj_t* parent, lv_coord_t width, lv_coord_t height) {
  lv_obj_t* row = lv_obj_create(parent);
  if (row == nullptr) {
    return nullptr;
  }
  ui_prepare_box(row);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(row, width, height);
  lv_obj_set_scroll_dir(row, LV_DIR_HOR);
  lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  ui_set_flex_row(row, 0, 8, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  return row;
}

void create_weather_mini_icon(lv_obj_t* parent, lv_coord_t x, lv_coord_t y, lv_coord_t size) {
  const char* weather_icon_path = weather_icon_asset_path();
  if (file_exists(weather_icon_path)) {
    lv_obj_t* image = lv_image_create(parent);
    if (image == nullptr) {
      return;
    }
    lv_obj_set_size(image, size, size);
    lv_image_set_src(image, weather_icon_path);
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_align(image, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    return;
  }

  lv_obj_t* sun = lv_obj_create(parent);
  lv_obj_t* cloud = lv_obj_create(parent);
  if (sun == nullptr || cloud == nullptr) {
    return;
  }
  for (lv_obj_t* part : {sun, cloud}) {
    ui_prepare_box(part);
    lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_border_width(part, 0, 0);
  }
  lv_obj_set_size(sun, size / 2, size / 2);
  lv_obj_set_style_bg_color(sun, lv_color_hex(0xFFE082), 0);
  lv_obj_set_style_bg_opa(sun, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(sun, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(sun, LV_ALIGN_TOP_LEFT, x + size / 3, y + 2);

  lv_obj_set_size(cloud, size, size / 2);
  lv_obj_set_style_bg_color(cloud, lv_color_hex(0xEAF6FF), 0);
  lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(cloud, size / 4, 0);
  lv_obj_align(cloud, LV_ALIGN_TOP_LEFT, x, y + size / 3);
}

lv_obj_t* create_steps_label(lv_obj_t* parent,
                             const char* text,
                             const lv_font_t* font,
                             std::uint32_t color,
                             lv_coord_t width,
                             lv_label_long_mode_t long_mode = LV_LABEL_LONG_DOT) {
  lv_obj_t* label = lv_label_create(parent);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, long_mode);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_text(label, text);
  return label;
}

lv_obj_t* create_steps_panel(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, std::uint32_t bg = 0x0A1626) {
  lv_obj_t* panel = lv_obj_create(parent);
  if (panel == nullptr) {
    return nullptr;
  }
  ui_prepare_box(panel);
  ui_apply_surface(panel, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(panel, width, height);
  lv_obj_set_style_bg_color(panel, lv_color_hex(bg), 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_radius(panel, clamp_coord(scale_by_ratio(width, 12, 100), 22, 28), 0);
  lv_obj_set_style_shadow_width(panel, 0, 0);
  return panel;
}

lv_obj_t* create_sleep_scroll_root(lv_obj_t* root,
                                   lv_coord_t screen_w,
                                   lv_coord_t screen_h,
                                   lv_coord_t top,
                                   lv_coord_t bottom,
                                   lv_coord_t gap = 10) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(scroll, screen_w - 16, screen_h - top - bottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, top);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  ui_set_flex_column(scroll, 0, gap);
  lv_obj_set_style_pad_top(scroll, 0, 0);
  lv_obj_set_style_pad_bottom(scroll, 10, 0);
  lv_obj_set_style_pad_left(scroll, 0, 0);
  lv_obj_set_style_pad_right(scroll, 0, 0);
  return scroll;
}

lv_obj_t* create_sleep_round_icon(lv_obj_t* parent,
                                  lv_coord_t size,
                                  std::uint32_t bg,
                                  std::uint32_t fg,
                                  const char* asset_path,
                                  const char* fallback_text,
                                  const lv_font_t* fallback_font = &lv_font_montserrat_20) {
  lv_obj_t* icon = lv_obj_create(parent);
  if (icon == nullptr) {
    return nullptr;
  }
  ui_prepare_box(icon);
  lv_obj_set_size(icon, size, size);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(bg), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon, 0, 0);
  lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);

  if (file_exists(asset_path)) {
    lv_obj_t* image = lv_image_create(icon);
    if (image != nullptr) {
      lv_image_set_src(image, asset_path);
      lv_obj_set_size(image, static_cast<lv_coord_t>(size * 2 / 3), static_cast<lv_coord_t>(size * 2 / 3));
      lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
      lv_obj_center(image);
      lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
      return icon;
    }
  }

  lv_obj_t* label = lv_label_create(icon);
  if (label == nullptr) {
    return icon;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, fallback_font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(fg), 0);
  lv_label_set_text(label, fallback_text);
  lv_obj_center(label);
  return icon;
}

lv_obj_t* create_sleep_switch_track(lv_obj_t* parent) {
  lv_obj_t* track = lv_obj_create(parent);
  if (track == nullptr) {
    return nullptr;
  }
  ui_prepare_box(track);
  lv_obj_remove_flag(track, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(track, 64, 34);
  lv_obj_set_style_radius(track, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_border_width(track, 0, 0);

  lv_obj_t* thumb = lv_obj_create(track);
  if (thumb == nullptr) {
    return track;
  }
  ui_prepare_box(thumb);
  lv_obj_set_size(thumb, 28, 28);
  lv_obj_set_style_radius(thumb, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_border_width(thumb, 0, 0);
  lv_obj_set_style_bg_color(thumb, lv_color_hex(0xEAF6FF), 0);
  lv_obj_set_style_bg_opa(thumb, LV_OPA_COVER, 0);
  lv_obj_remove_flag(thumb, LV_OBJ_FLAG_CLICKABLE);
  return track;
}

void apply_sleep_switch_style(lv_obj_t* track, bool enabled) {
  if (track == nullptr) {
    return;
  }
  lv_obj_set_style_bg_color(track, lv_color_hex(enabled ? 0x14B8FF : 0x3A4F66), 0);
  lv_obj_set_style_bg_opa(track, LV_OPA_COVER, 0);

  lv_obj_t* thumb = lv_obj_get_child(track, 0);
  if (thumb == nullptr) {
    return;
  }
  lv_obj_align(thumb, enabled ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, enabled ? -3 : 3, 0);
}

const char* low_blood_oxygen_mode_text(LowBloodOxygenReminderMode mode) {
  switch (mode) {
    case LowBloodOxygenReminderMode::Threshold90:
      return "90%";
    case LowBloodOxygenReminderMode::Threshold85:
      return "85%";
    case LowBloodOxygenReminderMode::Threshold80:
      return "80%";
    case LowBloodOxygenReminderMode::Off:
    default:
      return "不提醒";
  }
}

const char* heart_rate_all_day_mode_text(HeartRateAllDayMonitoringMode mode) {
  switch (mode) {
    case HeartRateAllDayMonitoringMode::Off:
      return "关闭";
    case HeartRateAllDayMonitoringMode::Every1Min:
      return "1分钟一次";
    case HeartRateAllDayMonitoringMode::Every10Min:
      return "10分钟一次";
    case HeartRateAllDayMonitoringMode::Every30Min:
      return "30分钟一次";
    case HeartRateAllDayMonitoringMode::Smart:
    default:
      return "智能监测";
  }
}

const char* heart_rate_high_reminder_text(HeartRateHighReminderMode mode) {
  switch (mode) {
    case HeartRateHighReminderMode::Bpm100:
      return "100次/分";
    case HeartRateHighReminderMode::Bpm110:
      return "110次/分";
    case HeartRateHighReminderMode::Bpm120:
      return "120次/分";
    case HeartRateHighReminderMode::Bpm130:
      return "130次/分";
    case HeartRateHighReminderMode::Bpm140:
      return "140次/分";
    case HeartRateHighReminderMode::Bpm150:
      return "150次/分";
    case HeartRateHighReminderMode::Off:
    default:
      return "关闭";
  }
}

const char* heart_rate_low_reminder_text(HeartRateLowReminderMode mode) {
  switch (mode) {
    case HeartRateLowReminderMode::Bpm40:
      return "40次/分";
    case HeartRateLowReminderMode::Bpm45:
      return "45次/分";
    case HeartRateLowReminderMode::Bpm50:
      return "50次/分";
    case HeartRateLowReminderMode::Off:
    default:
      return "关闭";
  }
}

lv_obj_t* create_steps_scroll_root(lv_obj_t* root, lv_coord_t screen_w, lv_coord_t screen_h) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(scroll, screen_w - kStepsScrollInset * 2, screen_h - kStepsScrollTop - kStepsScrollBottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, kStepsScrollTop);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  ui_set_flex_column(scroll, 0, 10);
  lv_obj_set_style_pad_top(scroll, 0, 0);
  lv_obj_set_style_pad_bottom(scroll, 10, 0);
  lv_obj_set_style_pad_left(scroll, 0, 0);
  lv_obj_set_style_pad_right(scroll, 0, 0);
  return scroll;
}

lv_obj_t* create_steps_icon(lv_obj_t* parent, lv_coord_t size, std::uint32_t bg, std::uint32_t fg, bool use_foot_asset) {
  lv_obj_t* icon = lv_obj_create(parent);
  if (icon == nullptr) {
    return nullptr;
  }
  ui_prepare_box(icon);
  lv_obj_set_size(icon, size, size);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(bg), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon, 0, 0);
  lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);

  const char* asset_path = use_foot_asset ? steps_icon_asset_path() : nullptr;
  if (file_exists(asset_path)) {
    lv_obj_t* image = lv_image_create(icon);
    if (image != nullptr) {
      lv_image_set_src(image, asset_path);
      lv_obj_set_size(image, static_cast<lv_coord_t>(size * 2 / 3), static_cast<lv_coord_t>(size * 2 / 3));
      lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
      lv_obj_center(image);
      lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
      return icon;
    }
  }

  lv_obj_t* mark = lv_label_create(icon);
  if (mark == nullptr) {
    return icon;
  }
  ui_prepare_label(mark);
  lv_obj_set_style_text_font(mark, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(mark, lv_color_hex(fg), 0);
  lv_label_set_text(mark, use_foot_asset ? "S" : "i");
  lv_obj_center(mark);
  return icon;
}

lv_obj_t* create_steps_arc(lv_obj_t* parent,
                           lv_coord_t size,
                           lv_coord_t width,
                           std::uint32_t color,
                           std::int32_t value,
                           std::int32_t max_value) {
  lv_obj_t* arc = lv_arc_create(parent);
  if (arc == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(arc, size, size);
  lv_arc_set_bg_angles(arc, 180, 360);
  lv_arc_set_angles(arc, 180, 180 + std::clamp(value * 180 / std::max(1, max_value), 6, 180));
  lv_obj_set_style_arc_width(arc, width, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x142030), LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, lv_color_hex(color), LV_PART_INDICATOR);
  lv_obj_set_style_arc_opa(arc, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_arc_opa(arc, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
  lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  return arc;
}

bool create_steps_metric_line(lv_obj_t* parent,
                              lv_coord_t y,
                              std::uint32_t accent,
                              const char* value,
                              const char* target) {
  lv_obj_t* icon = create_steps_icon(parent, 34, 0x1A2430, accent, accent == 0xF6D34D);
  lv_obj_t* value_label = create_steps_label(parent, value, &lv_font_montserrat_28, 0xF8FAFC, 74);
  lv_obj_t* target_label = create_steps_label(parent, target, &lv_font_montserrat_16, 0xB9C7D9, 80);
  if (icon == nullptr || value_label == nullptr || target_label == nullptr) {
    return false;
  }
  lv_obj_set_style_bg_color(icon, lv_color_mix(lv_color_hex(accent), lv_color_hex(0x0A1626), LV_OPA_70), 0);
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 18, y);
  lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, 64, y - 1);
  lv_obj_align_to(target_label, value_label, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  return true;
}

lv_obj_t* create_steps_distribution_card(lv_obj_t* parent,
                                         lv_coord_t width,
                                         lv_coord_t height,
                                         const char* title,
                                         const char* value,
                                         const char* unit,
                                         const char* target,
                                         std::uint32_t accent,
                                         bool use_foot_asset) {
  lv_obj_t* card = create_steps_panel(parent, width, height);
  if (card == nullptr) {
    return nullptr;
  }

  const lv_coord_t divider_y = clamp_coord(scale_by_ratio(height, 28, 100), 58, 66);
  const lv_coord_t value_y = clamp_coord(scale_by_ratio(height, 35, 100), 76, 86);
  const lv_coord_t target_y = clamp_coord(scale_by_ratio(height, 57, 100), 118, 132);
  const lv_coord_t tick_y = clamp_coord(scale_by_ratio(height, 68, 100), 140, 154);
  const lv_coord_t tick_label_y = clamp_coord(scale_by_ratio(height, 88, 100), 184, 198);
  const lv_coord_t dot_x = width - 30;
  const lv_coord_t dot_y = clamp_coord(scale_by_ratio(height, 67, 100), 136, 174);

  lv_obj_t* icon = create_steps_icon(card, 36, 0x1A2430, accent, use_foot_asset);
  lv_obj_t* title_label = create_steps_label(card, title, cjk_font_16(), 0xF8FAFC, width - 76);
  lv_obj_t* divider = lv_obj_create(card);
  lv_obj_t* value_label = create_steps_label(card, value, &lv_font_montserrat_42, 0xF8FAFC, 76);
  lv_obj_t* unit_label = create_steps_label(card, unit, cjk_font_16(), 0xF8FAFC, 88);
  lv_obj_t* target_label = create_steps_label(card, target, cjk_font_16(), 0xAFC4DA, width - 36);
  if (icon == nullptr || title_label == nullptr || divider == nullptr || value_label == nullptr ||
      unit_label == nullptr || target_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 20, 16);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 66, 22);

  ui_prepare_box(divider);
  lv_obj_set_size(divider, width - 36, 1);
  lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
  lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
  lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, divider_y);

  lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, 18, value_y);
  lv_obj_align_to(unit_label, value_label, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
  lv_obj_align(target_label, LV_ALIGN_TOP_LEFT, 22, target_y);

  lv_coord_t xs[] = {24, static_cast<lv_coord_t>(width / 2), static_cast<lv_coord_t>(width - 24)};
  const char* labels[] = {"0", "12", "24"};
  for (std::size_t i = 0; i < 3; ++i) {
    lv_obj_t* tick = lv_obj_create(card);
    lv_obj_t* tick_label = create_steps_label(card, labels[i], &lv_font_montserrat_16, 0x8DB9E3, 32);
    if (tick == nullptr || tick_label == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, 46);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5AA4DD), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_70, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, xs[i], tick_y);
    lv_obj_align(tick_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(xs[i] - 10), tick_label_y);
  }

  lv_obj_t* dot = lv_obj_create(card);
  if (dot == nullptr) {
    return nullptr;
  }
  ui_prepare_box(dot);
  lv_obj_set_size(dot, 7, 7);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, lv_color_hex(accent), 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_align(dot, LV_ALIGN_TOP_LEFT, dot_x, dot_y);

  return card;
}

void apply_stream_crown_drag(lv_obj_t* scroll_root, bool forward, std::int16_t detents) {
  if (scroll_root == nullptr) {
    return;
  }
  const std::int32_t step = kLauncherCrownDragStep * std::max<std::int16_t>(1, detents);
  const std::int32_t elastic_limit = std::max<std::int32_t>(24, lv_obj_get_height(scroll_root) / 5);
  const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root);
  const std::int32_t scroll_top = lv_obj_get_scroll_top(scroll_root);
  const std::int32_t scroll_bottom = lv_obj_get_scroll_bottom(scroll_root);
  const std::int32_t scroll_max = std::max<std::int32_t>(0, scroll_top + scroll_bottom);
  std::int32_t target_y = current_y + (forward ? step : -step);
  target_y = std::clamp(target_y, -elastic_limit, scroll_max + elastic_limit);
  lv_obj_scroll_to_y(scroll_root, target_y, LV_ANIM_OFF);
}

void release_stream_crown_drag(lv_obj_t* scroll_root) {
  if (scroll_root == nullptr) {
    return;
  }
  const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root);
  const std::int32_t scroll_max =
      std::max<std::int32_t>(0, lv_obj_get_scroll_top(scroll_root) + lv_obj_get_scroll_bottom(scroll_root));
  lv_obj_scroll_to_y(scroll_root, std::clamp(current_y, 0, scroll_max), LV_ANIM_ON);
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
  lv_obj_t* battery_row = lv_obj_create(overlay);
  battery_icon_label_ = lv_label_create(battery_row);
  battery_label_ = lv_label_create(battery_row);
  style_stage_ = lv_obj_create(overlay);
  minute_label_ = lv_label_create(overlay);

  if (overlay == nullptr || battery_row == nullptr || battery_icon_label_ == nullptr || battery_label_ == nullptr ||
      style_stage_ == nullptr || minute_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(overlay);
  lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_center(overlay);

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(battery_icon_label_);
  ui_apply_text(battery_icon_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(battery_icon_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(battery_icon_label_, LV_SYMBOL_CHARGE);

  ui_prepare_label(battery_label_);
  ui_apply_text(battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(battery_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(battery_label_, "--%");

  ui_prepare_box(style_stage_);
  lv_obj_set_size(style_stage_, 240, 296);
  lv_obj_align(style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(style_stage_, 0, 0);
  lv_obj_set_style_pad_all(style_stage_, 0, 0);

  renderer_ = create_watchface_style_renderer(config_);
  if (!renderer_ || renderer_->build(style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(minute_label_);
  ui_apply_text(minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(minute_label_, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(minute_label_, 96);
  lv_obj_set_style_text_align(minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(minute_label_, 76, 234);
  lv_label_set_text(minute_label_, "--");

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
  if (minute_label_ == nullptr || renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(minute_label_, "--");
    render_state_.hour_text = "--";
    render_state_.minute_text = "--";
    render_state_.spread_index = config_.spread_index;
    renderer_->apply(render_state_);
    return;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  unsigned display_hour = static_cast<unsigned>(model.hour % 12);
  if (display_hour == 0U) {
    display_hour = 12U;
  }
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", display_hour);
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));
  lv_label_set_text(minute_label_, minute_buffer);
  render_state_.hour_text = hour_buffer;
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

HomeRingHostPage::HomeRingHostPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HomeRingHostPage::id() const {
  return PageId::HomeRingHost;
}

const char* HomeRingHostPage::name() const {
  return "HomeRingHost";
}

void HomeRingHostPage::on_will_appear() {
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
  if (data_center_.home_ring_preview()) {
    apply_home_ring_preview(*data_center_.home_ring_preview());
  } else {
    layout_surfaces_for_preview(static_cast<std::uint8_t>(settled_surface_index_), 0);
    set_track_x(static_cast<lv_coord_t>(-240 * static_cast<int>(settled_surface_index_)));
  }
  if (data_center_.steps()) {
    apply_steps(*data_center_.steps());
  }
}

void HomeRingHostPage::apply_time(const TimeModel& model) {
  if (minute_label_ == nullptr || renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(minute_label_, "--");
    render_state_.hour_text = "--";
    render_state_.minute_text = "--";
    render_state_.spread_index = config_.spread_index;
    renderer_->apply(render_state_);
    return;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  unsigned display_hour = static_cast<unsigned>(model.hour % 12);
  if (display_hour == 0U) {
    display_hour = 12U;
  }
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", display_hour);
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));
  lv_label_set_text(minute_label_, minute_buffer);
  render_state_.hour_text = hour_buffer;
  render_state_.minute_text = minute_buffer;
  render_state_.spread_index = config_.spread_index;
  renderer_->apply(render_state_);
}

void HomeRingHostPage::apply_battery(const BatteryModel& model) {
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

void HomeRingHostPage::apply_steps(const StepsModel& model) {
  steps_model_ = model;
  if (weather_steps_value_label_ == nullptr) {
    return;
  }

  if (!steps_model_.valid) {
    set_content_width_label(weather_steps_value_label_, "--", &lv_font_montserrat_14, 0xFFFFFF);
    lv_obj_align(weather_steps_value_label_, LV_ALIGN_TOP_LEFT, 2, 70);
    return;
  }

  char value_text[16] = {};
  std::snprintf(value_text, sizeof(value_text), "%lu", static_cast<unsigned long>(steps_model_.daily_steps));
  set_content_width_label(weather_steps_value_label_,
                          value_text,
                          home_shortcut_steps_font(steps_model_.daily_steps),
                          0xFFFFFF);
  lv_obj_align(weather_steps_value_label_, LV_ALIGN_TOP_LEFT, 2, 70);
}

std::size_t HomeRingHostPage::wrap_surface_index(int index) const {
  const int size = static_cast<int>(surfaces_.size());
  int wrapped = index % size;
  if (wrapped < 0) {
    wrapped += size;
  }
  return static_cast<std::size_t>(wrapped);
}

void HomeRingHostPage::set_track_x(lv_coord_t x) {
  if (track_ == nullptr) {
    return;
  }
  lv_obj_set_x(track_, x);
}

void HomeRingHostPage::layout_surfaces_for_preview(std::uint8_t base_index, std::int8_t direction) {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const bool wrap_to_weather = base_index == 0 && direction < 0;
  const bool wrap_to_watchface = base_index == surfaces_.size() - 1 && direction > 0;

  if (wrap_to_weather || wrap_to_watchface) {
    for (std::size_t index = 0; index < surfaces_.size(); ++index) {
      if (surfaces_[index] == nullptr) {
        continue;
      }
      lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>((index + 2) * screen_w), 0);
    }

    if (surfaces_[0] != nullptr) {
      lv_obj_set_pos(surfaces_[0], screen_w, 0);
    }
    if (surfaces_.back() != nullptr) {
      lv_obj_set_pos(surfaces_.back(), 0, 0);
    }
    return;
  }

  for (std::size_t index = 0; index < surfaces_.size(); ++index) {
    if (surfaces_[index] == nullptr) {
      continue;
    }
    lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>(index * screen_w), 0);
  }
}

void HomeRingHostPage::apply_home_ring_preview(const HomeRingPreviewModel& model) {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t base_x = static_cast<lv_coord_t>(-screen_w * static_cast<int>(model.base_index));
  const lv_coord_t clamped_progress = clamp_coord(model.progress, 0, screen_w);
  auto update_pager = [&](float shortcut_position, bool visible) {
    if (pager_root_ == nullptr || pager_active_dot_ == nullptr) {
      return;
    }
    if (!visible) {
      lv_obj_add_flag(pager_root_, LV_OBJ_FLAG_HIDDEN);
      return;
    }

    lv_obj_clear_flag(pager_root_, LV_OBJ_FLAG_HIDDEN);
    const float clamped = std::clamp(shortcut_position, 0.0f, 3.0f);
    const lv_coord_t offset = static_cast<lv_coord_t>(std::lround(clamped * static_cast<float>(kHomePagerStep)));
    lv_obj_set_x(pager_active_dot_, offset);
  };

  if (model.active && model.direction != 0) {
    layout_surfaces_for_preview(model.base_index, model.direction);
    const bool touches_watchface = (model.base_index == 0 && model.direction < 0) ||
                                   (model.base_index == static_cast<std::uint8_t>(surfaces_.size() - 1) &&
                                    model.direction > 0);
    if (touches_watchface) {
      const lv_coord_t offset = model.base_index == 0
                                    ? static_cast<lv_coord_t>(clamped_progress - screen_w)
                                    : static_cast<lv_coord_t>(-clamped_progress);
      set_track_x(offset);
    } else {
      const lv_coord_t offset = model.direction > 0 ? static_cast<lv_coord_t>(-clamped_progress)
                                                    : static_cast<lv_coord_t>(clamped_progress);
      set_track_x(static_cast<lv_coord_t>(base_x + offset));
    }

    if (touches_watchface) {
      update_pager(0.0f, false);
    } else if (model.base_index > 0) {
      const float ratio = static_cast<float>(clamped_progress) / static_cast<float>(screen_w);
      const float base_shortcut = static_cast<float>(model.base_index - 1);
      const float delta = model.direction > 0 ? ratio : -ratio;
      update_pager(base_shortcut + delta, true);
    } else {
      update_pager(0.0f, false);
    }
    return;
  }

  if (model.commit && model.direction != 0) {
    settled_surface_index_ = wrap_surface_index(static_cast<int>(model.base_index) + model.direction);
  } else {
    settled_surface_index_ = wrap_surface_index(model.base_index);
  }

  layout_surfaces_for_preview(static_cast<std::uint8_t>(settled_surface_index_), 0);
  set_track_x(static_cast<lv_coord_t>(-screen_w * static_cast<int>(settled_surface_index_)));
  if (settled_surface_index_ == 0) {
    update_pager(0.0f, false);
  } else {
    update_pager(static_cast<float>(settled_surface_index_ - 1), true);
  }
}

lv_obj_t* HomeRingHostPage::build() {
  const auto app_tile_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<HomeRingHostPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_current_target_obj(event);
    if (target == nullptr || !click_guard_allows(target)) {
      return;
    }
    const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
    self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
  };

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const auto layout = make_home_surface_layout();

  track_ = lv_obj_create(root);
  if (track_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(track_);
  lv_obj_set_size(track_, static_cast<lv_coord_t>(screen_w * static_cast<int>(surfaces_.size())), screen_h);
  lv_obj_align(track_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(track_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(track_, 0, 0);
  lv_obj_set_style_pad_all(track_, 0, 0);
  lv_obj_remove_flag(track_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(track_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

  for (std::size_t index = 0; index < surfaces_.size(); ++index) {
    surfaces_[index] = lv_obj_create(track_);
    if (surfaces_[index] == nullptr) {
      return nullptr;
    }
    ui_prepare_box(surfaces_[index]);
    ui_apply_surface(surfaces_[index], SurfaceStyle::Screen);
    lv_obj_set_size(surfaces_[index], screen_w, screen_h);
    lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>(index * screen_w), 0);
    lv_obj_remove_flag(surfaces_[index], LV_OBJ_FLAG_SCROLLABLE);
  }

  pager_root_ = lv_obj_create(root);
  if (pager_root_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pager_root_);
  lv_obj_set_size(pager_root_, 54, 14);
  lv_obj_set_style_bg_opa(pager_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(pager_root_, 0, 0);
  lv_obj_set_style_pad_all(pager_root_, 0, 0);
  lv_obj_align(pager_root_, LV_ALIGN_BOTTOM_MID, 0, -layout.pager_bottom);
  lv_obj_add_flag(pager_root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(pager_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(pager_root_, LV_OBJ_FLAG_CLICKABLE);

  for (std::size_t index = 0; index < pager_dots_.size(); ++index) {
    pager_dots_[index] = create_pager_dot(pager_root_, lv_color_hex(0xB9C5D5), LV_OPA_10, false);
    if (pager_dots_[index] == nullptr) {
      return nullptr;
    }
    lv_obj_add_flag(pager_dots_[index], LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_remove_flag(pager_dots_[index], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(pager_dots_[index], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(pager_dots_[index], static_cast<lv_coord_t>(index * kHomePagerStep + 1), 3);
  }

  pager_active_dot_ = lv_obj_create(pager_root_);
  if (pager_active_dot_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pager_active_dot_);
  lv_obj_set_size(pager_active_dot_, 9, 9);
  lv_obj_add_flag(pager_active_dot_, LV_OBJ_FLAG_IGNORE_LAYOUT);
  lv_obj_remove_flag(pager_active_dot_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(pager_active_dot_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_radius(pager_active_dot_, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(pager_active_dot_, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(pager_active_dot_, LV_OPA_70, 0);
  lv_obj_set_style_border_width(pager_active_dot_, 0, 0);
  lv_obj_set_style_pad_all(pager_active_dot_, 0, 0);
  lv_obj_set_pos(pager_active_dot_, 0, 2);

  {
    lv_obj_t* surface = surfaces_[0];
    lv_obj_t* overlay = lv_obj_create(surface);
    lv_obj_t* battery_row = lv_obj_create(overlay);
    battery_icon_label_ = lv_label_create(battery_row);
    battery_label_ = lv_label_create(battery_row);
    style_stage_ = lv_obj_create(overlay);
    minute_label_ = lv_label_create(overlay);

    if (overlay == nullptr || battery_row == nullptr || battery_icon_label_ == nullptr || battery_label_ == nullptr ||
        style_stage_ == nullptr || minute_label_ == nullptr) {
      return nullptr;
    }

    ui_prepare_box(overlay);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(overlay);

    ui_prepare_box(battery_row);
    ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(battery_row, 53, 18);
    lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_row, 0, 0);
    lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

    ui_prepare_label(battery_icon_label_);
    ui_apply_text(battery_icon_label_, TextStyle::Tiny);
    lv_obj_set_style_text_font(battery_icon_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
    lv_label_set_text(battery_icon_label_, LV_SYMBOL_CHARGE);

    ui_prepare_label(battery_label_);
    ui_apply_text(battery_label_, TextStyle::Tiny);
    lv_obj_set_style_text_font(battery_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xF5F7FB), 0);
    lv_label_set_text(battery_label_, "--%");

    ui_prepare_box(style_stage_);
    lv_obj_set_size(style_stage_, 240, 296);
    lv_obj_align(style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(style_stage_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(style_stage_, 0, 0);
    lv_obj_set_style_pad_all(style_stage_, 0, 0);

    renderer_ = create_watchface_style_renderer(config_);
    if (!renderer_ || renderer_->build(style_stage_) == nullptr) {
      return nullptr;
    }

    ui_prepare_label(minute_label_);
    ui_apply_text(minute_label_, TextStyle::HeroSoft);
    lv_obj_set_style_text_font(minute_label_, &lv_font_montserrat_42, 0);
    lv_obj_set_style_text_color(minute_label_, lv_color_hex(0xD7E3F4), 0);
    lv_obj_set_width(minute_label_, 96);
    lv_obj_set_style_text_align(minute_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(minute_label_, 76, 234);
    lv_label_set_text(minute_label_, "--");
  }

  {
    lv_obj_t* surface = surfaces_[1];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    const lv_color_t tile_bg = lv_color_hex(0x0C1424);
    const lv_color_t tile_border = lv_color_hex(0x1E375A);
    const lv_color_t music_bg = lv_color_hex(0x4BE7E8);
    const lv_color_t music_fg = lv_color_hex(0xF7FFFE);
    lv_obj_t* stage = create_home_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
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

    for (auto [card, target] : {std::pair<lv_obj_t*, PageId> {alipay_card, PageId::AppAlipay},
                                std::pair<lv_obj_t*, PageId> {wechat_card, PageId::AppWeChatPay}}) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
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
  }

  {
    lv_obj_t* surface = surfaces_[3];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    lv_obj_t* stage = create_home_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
      return nullptr;
    }

    struct HealthTile {
      PageId target;
      const char* icon_path;
      const char* value;
      lv_color_t bg;
      bool emphasize;
      lv_coord_t icon_x;
      lv_coord_t icon_y;
      lv_coord_t icon_size;
    };

    const std::array<HealthTile, 4> tiles {{
        {PageId::AppHeartRate, health_heart_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 36},
        {PageId::AppBloodOxygen, health_spo2_asset_path(), "--", lv_color_hex(0xFF4F72), false, 18, 12, 40},
        {PageId::AppBreathing, health_breathe_asset_path(), "Breathe", lv_color_hex(0x4DBDFF), true, 14, 13, 39},
        {PageId::AppStress, health_stress_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 44},
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
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(tile.target)));

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
      lv_obj_set_style_text_font(value, &lv_font_montserrat_16, 0);
      lv_obj_set_style_text_color(value, tile.emphasize ? lv_color_hex(0xD7FBFF) : lv_color_hex(0xF8FAFC), 0);
      lv_obj_set_width(value, 78);
      lv_label_set_long_mode(value, LV_LABEL_LONG_DOT);
      lv_label_set_text(value, tile.value);
      lv_obj_align(value, LV_ALIGN_BOTTOM_LEFT, 12, -14);
    }
  }

  {
    lv_obj_t* surface = surfaces_[2];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    const lv_color_t card_bg = lv_color_hex(0x0A101A);
    lv_obj_t* stage = create_home_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
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
    attach_click_guard(card);
    lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(PageId::AppNfc)));
  }

  {
    lv_obj_t* surface = surfaces_[4];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    const lv_color_t weather_bg = lv_color_hex(0x0E2B56);
    const lv_color_t weather_accent = lv_color_hex(0x7EC8FF);
    const lv_color_t sleep_bg = lv_color_hex(0x3A1B69);
    const lv_color_t sleep_accent = lv_color_hex(0xC06CFF);
    const lv_color_t steps_bg = lv_color_hex(0x054A42);
    const lv_color_t steps_accent = lv_color_hex(0x19F57A);

    lv_obj_t* stage = lv_obj_create(surface);
    lv_obj_t* hero_card = lv_obj_create(stage);
    lv_obj_t* sleep_card = lv_obj_create(stage);
    lv_obj_t* steps_card = lv_obj_create(stage);
    if (stage == nullptr || hero_card == nullptr || sleep_card == nullptr || steps_card == nullptr) {
      return nullptr;
    }

    style_home_surface_stage(stage, layout.stage_w, layout.stage_h, layout.stage_radius, stage_bg);
    lv_obj_align(stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);

    ui_prepare_box(hero_card);
    ui_apply_surface(hero_card, SurfaceStyle::Panel);
    lv_obj_set_size(hero_card, layout.stage_w, 110);
    lv_obj_align(hero_card, LV_ALIGN_TOP_LEFT, 0, 15);
    lv_obj_set_style_bg_color(hero_card, weather_bg, 0);
    lv_obj_set_style_border_color(hero_card, lv_color_mix(weather_accent, lv_color_hex(0xFFFFFF), LV_OPA_20), 0);
    lv_obj_set_style_radius(hero_card, 28, 0);
    lv_obj_set_style_pad_all(hero_card, 0, 0);

    lv_obj_t* temp_label = lv_label_create(hero_card);
    lv_obj_t* range_label = lv_label_create(hero_card);
    lv_obj_t* weather_icon_image = lv_image_create(hero_card);
    if (temp_label == nullptr || range_label == nullptr || weather_icon_image == nullptr) {
      return nullptr;
    }
    ui_prepare_label(temp_label);
    ui_apply_text(temp_label, TextStyle::Hero);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
    set_single_line_label(temp_label, 90);
    lv_label_set_text(temp_label, "23C");
    lv_obj_align(temp_label, LV_ALIGN_TOP_LEFT, 15, 23);

    ui_prepare_label(range_label);
    ui_apply_text(range_label, TextStyle::Title);
    lv_obj_set_style_text_font(range_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(range_label, lv_color_hex(0xC8D7EE), 0);
    set_single_line_label(range_label, 76);
    lv_label_set_text(range_label, "30C / 18C");
    lv_obj_align(range_label, LV_ALIGN_TOP_LEFT, 28, 68);

    const char* weather_icon_path = weather_icon_asset_path();
    if (file_exists(weather_icon_path)) {
      lv_image_set_src(weather_icon_image, weather_icon_path);
      lv_obj_set_size(weather_icon_image, 73, 73);
      lv_image_set_inner_align(weather_icon_image, LV_IMAGE_ALIGN_CONTAIN);
      lv_obj_align(weather_icon_image, LV_ALIGN_TOP_LEFT, 140, 18);
    } else {
      lv_obj_add_flag(weather_icon_image, LV_OBJ_FLAG_HIDDEN);
    }
    attach_click_guard(hero_card);
    lv_obj_add_event_cb(hero_card, app_tile_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(hero_card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(PageId::AppWeather)));

    for (auto [card, bg, border, x, target] :
         {std::tuple<lv_obj_t*, lv_color_t, lv_color_t, lv_coord_t, PageId> {sleep_card,
                                                                              sleep_bg,
                                                                              lv_color_mix(sleep_accent, lv_color_hex(0xFFFFFF), LV_OPA_20),
                                                                              1,
                                                                              PageId::AppSleep},
          std::tuple<lv_obj_t*, lv_color_t, lv_color_t, lv_coord_t, PageId> {steps_card,
                                                                              steps_bg,
                                                                              lv_color_mix(steps_accent, lv_color_hex(0xFFFFFF), LV_OPA_20),
                                                                              114,
                                                                              PageId::Pedometer}}) {
      ui_prepare_box(card);
      ui_apply_surface(card, SurfaceStyle::PanelSubtle);
      lv_obj_set_size(card, 106, 106);
      lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, 136);
      lv_obj_set_style_radius(card, 24, 0);
      lv_obj_set_style_pad_all(card, 0, 0);
      lv_obj_set_style_bg_color(card, bg, 0);
      lv_obj_set_style_border_color(card, border, 0);
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
    }

    auto build_metric_card = [&](lv_obj_t* card,
                                 const char* value,
                                 bool sleep_icon,
                                 bool emphasize,
                                 lv_obj_t** out_value_label = nullptr) -> bool {
      lv_obj_t* icon_root = lv_obj_create(card);
      lv_obj_t* icon_image = lv_image_create(icon_root);
      lv_obj_t* value_label = lv_label_create(card);
      if (icon_root == nullptr || icon_image == nullptr || value_label == nullptr) {
        return false;
      }
      ui_prepare_box(icon_root);
      lv_obj_set_size(icon_root, 58, 58);
      lv_obj_set_style_bg_opa(icon_root, LV_OPA_TRANSP, 0);
      lv_obj_set_style_border_width(icon_root, 0, 0);
      lv_obj_align(icon_root, LV_ALIGN_TOP_LEFT, 0, 2);
      lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_CLICKABLE);

      const char* icon_path = sleep_icon ? sleep_icon_asset_path() : steps_icon_asset_path();
      lv_obj_set_size(icon_image, 58, 58);
      lv_image_set_inner_align(icon_image, LV_IMAGE_ALIGN_CONTAIN);
      lv_image_set_src(icon_image, icon_path);
      lv_obj_center(icon_image);
      lv_obj_remove_flag(icon_image, LV_OBJ_FLAG_CLICKABLE);

      ui_prepare_label(value_label);
      ui_apply_text(value_label, TextStyle::HeroSoft);
      lv_obj_set_style_text_font(value_label, emphasize ? &lv_font_montserrat_16 : &lv_font_montserrat_14, 0);
      lv_obj_set_style_text_color(value_label, lv_color_hex(0xFFFFFF), 0);
      set_single_line_label(value_label, 72);
      lv_label_set_text(value_label, value);
      lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, sleep_icon ? 6 : 2, 70);
      if (out_value_label != nullptr) {
        *out_value_label = value_label;
      }
      return true;
    };

    if (!build_metric_card(sleep_card, "7h 36m", true, false) ||
        !build_metric_card(steps_card, "--", false, true, &weather_steps_value_label_)) {
      return nullptr;
    }

    if (const auto steps = data_center_.steps()) {
      apply_steps(*steps);
    }

  }

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
  track(data_center_.subscribe(EventId::StepsChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<StepsModel>(&event.payload)) {
                                   apply_steps(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::HomeRingPreviewChanged,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }
                                 if (const auto* model = std::get_if<HomeRingPreviewModel>(&event.payload)) {
                                   apply_home_ring_preview(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || renderer_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr || settled_surface_index_ != 0) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::TouchActivity:
                                     renderer_->apply(render_state_);
                                     break;
                                   case InputAction::CrownRotateCW:
                                     if (renderer_->on_crown_delta(std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (renderer_->on_crown_delta(-std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));

  layout_surfaces_for_preview(0, 0);
  set_track_x(0);
  return root;
}

LauncherPage::LauncherPage(DataCenter& data_center) : PageBase(data_center) {
  struct LauncherEntrySpec {
    const char* section_title;
    PageId target;
  };

  const std::array<LauncherEntrySpec, 11> launcher_targets {{
      {"System", PageId::SettingsHome},
      {"Daily", PageId::AppWeather},
      {"Daily", PageId::Pedometer},
      {"Daily", PageId::AppSleep},
      {"Health", PageId::AppHeartRate},
      {"Health", PageId::AppBloodOxygen},
      {"Health", PageId::AppStress},
      {"Health", PageId::AppBreathing},
      {"Wallet", PageId::AppNfc},
      {"Wallet", PageId::AppAlipay},
      {"Wallet", PageId::AppWeChatPay},
  }};

  items_.reserve(launcher_targets.size());
  for (const auto& entry : launcher_targets) {
    const AppVisualSpec* spec = find_app_visual_spec(entry.target);
    if (spec == nullptr) {
      continue;
    }
    items_.push_back({entry.section_title,
                      spec->label,
                      {NavigationAction::LaunchApp, entry.target},
                      spec->icon_text,
                      spec->icon_asset_path,
                      lv_color_hex(spec->icon_bg),
                      lv_color_hex(spec->icon_fg)});
  }
}

PageId LauncherPage::id() const {
  return PageId::Launcher;
}

const char* LauncherPage::name() const {
  return "Launcher";
}

void LauncherPage::on_will_appear() {
  rebuild_layout_if_needed();
  if (list_root_ != nullptr) {
    lv_obj_update_layout(list_root_);
    lv_obj_scroll_to_y(list_root_, 0, LV_ANIM_OFF);
  }
}

lv_obj_t* LauncherPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));

  rebuild_layout_if_needed(true);

  bind_input();
  return root;
}

void LauncherPage::rebuild_layout_if_needed(bool force) {
  const LauncherLayoutMode target_mode = current_launcher_layout_mode(data_center_);
  if (!force && list_root_ != nullptr && current_layout_mode_ == target_mode) {
    return;
  }

  current_layout_mode_ = target_mode;
  stop_crown_release_timer();

  if (list_root_ != nullptr) {
    lv_obj_delete(list_root_);
    list_root_ = nullptr;
  }
  if (root_ == nullptr) {
    return;
  }

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  list_root_ = create_launcher_scroll_root(root_, screen_w, screen_h);
  if (list_root_ == nullptr) {
    return;
  }

  switch (current_layout_mode_) {
    case LauncherLayoutMode::MultiColumn:
      build_multi_column_layout(list_root_);
      break;
    case LauncherLayoutMode::List:
      build_list_layout(list_root_);
      break;
    case LauncherLayoutMode::Categorized:
    default:
      build_categorized_layout(list_root_);
      break;
  }
}

void LauncherPage::build_multi_column_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 12, 0);
  lv_obj_set_style_pad_right(parent, 12, 0);
  lv_obj_set_style_pad_row(parent, kLauncherRowGap, 0);
  lv_obj_set_style_pad_column(parent, kLauncherColumnGap, 0);

  for (std::size_t index = 0; index < items_.size(); ++index) {
    const Item& item = items_[index];
    lv_obj_t* tile = lv_button_create(parent);
    if (tile == nullptr) {
      return;
    }
    ui_prepare_box(tile);
    ui_set_touch_target(tile, 8);
    lv_obj_set_size(tile, kLauncherTileWidth, kLauncherMultiColumnTileHeight);
    lv_obj_set_style_bg_opa(tile, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(tile, 0, 0);
    lv_obj_set_style_shadow_width(tile, 0, 0);
    lv_obj_set_style_pad_all(tile, 0, 0);
    lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
    attach_click_guard(tile);
    lv_obj_add_event_cb(tile, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(tile, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    const AppVisualSpec render_spec {
        item.command.target,
        item.label,
        item.icon_text,
        item.icon_asset,
        lv_color_to_u32(item.icon_bg),
        lv_color_to_u32(item.icon_fg),
    };
    lv_obj_t* icon = create_app_round_icon(tile, render_spec, kLauncherIconSize);
    lv_obj_t* label = lv_label_create(tile);
    if (icon == nullptr || label == nullptr) {
      return;
    }
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 0);
    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Tiny);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
  }

  lv_obj_t* elastic_spacer = lv_obj_create(parent);
  if (elastic_spacer == nullptr) {
    return;
  }
  lv_obj_set_width(elastic_spacer, LV_PCT(100));
  lv_obj_set_height(elastic_spacer, kLauncherMultiColumnElasticSpacerHeight);
  lv_obj_set_style_bg_opa(elastic_spacer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(elastic_spacer, 0, 0);
  lv_obj_set_style_pad_all(elastic_spacer, 0, 0);
  lv_obj_remove_flag(elastic_spacer, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_remove_flag(elastic_spacer, LV_OBJ_FLAG_SCROLLABLE);
}

void LauncherPage::build_list_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 0, 0);
  lv_obj_set_style_pad_right(parent, 0, 0);
  lv_obj_set_style_pad_row(parent, kLauncherScrollGap, 0);

  for (std::size_t index = 0; index < items_.size(); ++index) {
    const Item& item = items_[index];
    lv_obj_t* row = lv_button_create(parent);
    if (row == nullptr) {
      return;
    }
    ui_prepare_box(row);
    ui_apply_surface(row, SurfaceStyle::PanelSubtle);
    ui_set_touch_target(row, 8);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, kLauncherListRowHeight);
    lv_obj_set_style_bg_color(row, lv_color_hex(0x0C131D), 0);
    lv_obj_set_style_border_color(row, lv_color_hex(0x1A2635), 0);
    lv_obj_set_style_radius(row, 22, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    const AppVisualSpec render_spec {
        item.command.target,
        item.label,
        item.icon_text,
        item.icon_asset,
        lv_color_to_u32(item.icon_bg),
        lv_color_to_u32(item.icon_fg),
    };
    lv_obj_t* icon = create_app_round_icon(row, render_spec, kLauncherListIconSize);
    lv_obj_t* label = lv_label_create(row);
    if (icon == nullptr || label == nullptr) {
      return;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 14, 0);
    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF7FBFF), 0);
    lv_obj_set_width(label, 136);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_label_set_text(label, item.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 66, 0);
  }
}

void LauncherPage::build_categorized_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 0, 0);
  lv_obj_set_style_pad_right(parent, 0, 0);
  lv_obj_set_style_pad_row(parent, kLauncherScrollGap, 0);
  lv_obj_set_style_pad_column(parent, 0, 0);

  std::size_t index = 0;
  while (index < items_.size()) {
    const char* section_title = items_[index].section_title;
    lv_obj_t* section = lv_obj_create(parent);
    lv_obj_t* section_title_label = lv_label_create(section);
    lv_obj_t* section_grid = lv_obj_create(section);
    if (section == nullptr || section_title_label == nullptr || section_grid == nullptr) {
      return;
    }

    ui_prepare_box(section);
    ui_apply_surface(section, SurfaceStyle::PanelSubtle);
    lv_obj_set_width(section, LV_PCT(100));
    lv_obj_set_layout(section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(section, kLauncherSectionPad, 0);
    lv_obj_set_style_pad_gap(section, kLauncherSectionGap, 0);
    lv_obj_set_style_bg_color(section, lv_color_hex(0x0C131D), 0);
    lv_obj_set_style_border_color(section, lv_color_hex(0x1A2635), 0);
    lv_obj_set_style_radius(section, 22, 0);
    lv_obj_remove_flag(section, LV_OBJ_FLAG_SCROLLABLE);

    ui_prepare_label(section_title_label);
    ui_apply_text(section_title_label, TextStyle::Eyebrow);
    lv_label_set_text(section_title_label, section_title);
    lv_obj_set_style_pad_bottom(section_title_label, kLauncherSectionHeaderBottom, 0);

    ui_prepare_box(section_grid);
    lv_obj_set_width(section_grid, LV_PCT(100));
    lv_obj_set_layout(section_grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(section_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(section_grid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(section_grid, 0, 0);
    lv_obj_set_style_pad_row(section_grid, kLauncherRowGap, 0);
    lv_obj_set_style_pad_column(section_grid, kLauncherColumnGap, 0);
    lv_obj_set_style_bg_opa(section_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(section_grid, 0, 0);
    lv_obj_remove_flag(section_grid, LV_OBJ_FLAG_SCROLLABLE);

    std::size_t section_count = 0;
    while (index < items_.size() && std::strcmp(items_[index].section_title, section_title) == 0) {
      const Item& item = items_[index];
      lv_obj_t* tile = lv_button_create(section_grid);
      if (tile == nullptr) {
        return;
      }
      ui_prepare_box(tile);
      ui_set_touch_target(tile, 8);
      lv_obj_set_size(tile, kLauncherTileWidth, kLauncherTileHeight);
      lv_obj_set_style_bg_opa(tile, LV_OPA_TRANSP, 0);
      lv_obj_set_style_border_width(tile, 0, 0);
      lv_obj_set_style_shadow_width(tile, 0, 0);
      lv_obj_set_style_pad_all(tile, 0, 0);
      lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
      attach_click_guard(tile);
      lv_obj_add_event_cb(tile, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(tile, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

      const AppVisualSpec render_spec {
          item.command.target,
          item.label,
          item.icon_text,
          item.icon_asset,
          lv_color_to_u32(item.icon_bg),
          lv_color_to_u32(item.icon_fg),
      };
      lv_obj_t* icon = create_app_round_icon(tile, render_spec, kLauncherIconSize);
      lv_obj_t* label = lv_label_create(tile);
      if (icon == nullptr || label == nullptr) {
        return;
      }
      lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 0);
      ui_prepare_label(label);
      ui_apply_text(label, TextStyle::Tiny);
      lv_obj_set_width(label, kLauncherTileWidth);
      lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_set_style_text_color(label, lv_color_hex(0xDCE6F4), 0);
      lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
      lv_label_set_text(label, item.label);
      lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

      ++index;
      ++section_count;
    }

    const std::size_t remainder = section_count % 3U;
    if (remainder != 0U) {
      const std::size_t filler_count = 3U - remainder;
      for (std::size_t filler_index = 0; filler_index < filler_count; ++filler_index) {
        lv_obj_t* filler = lv_obj_create(section_grid);
        if (filler == nullptr) {
          return;
        }
        lv_obj_set_size(filler, kLauncherTileWidth, kLauncherTileHeight);
        lv_obj_set_style_bg_opa(filler, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(filler, 0, 0);
        lv_obj_set_style_pad_all(filler, 0, 0);
        lv_obj_remove_flag(filler, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(filler, LV_OBJ_FLAG_SCROLLABLE);
      }
    }
  }
}

WeatherAppPage::WeatherAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId WeatherAppPage::id() const {
  return PageId::AppWeather;
}

const char* WeatherAppPage::name() const {
  return page_name(PageId::AppWeather);
}

lv_obj_t* WeatherAppPage::build() {
  struct HourForecast {
    const char* time;
    const char* temp;
    const char* wind;
    const char* air;
  };

  struct DayForecast {
    const char* day;
    const char* temp;
    const char* weather;
  };

  struct WeatherIndex {
    const char* value;
    const char* label;
    std::uint32_t accent;
  };

  static const std::array<HourForecast, 8> kHourly {{
      {"\xE7\x8E\xB0\xE5\x9C\xA8", "28C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
      {"12:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"13:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"14:00", "27C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"15:00", "28C", "\xE8\xA5\xBF\xE9\xA3\x8E 2", "\xE8\x89\xAF"},
      {"16:00", "27C", "\xE8\xA5\xBF\xE9\xA3\x8E 2", "\xE8\x89\xAF"},
      {"17:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
      {"18:00", "25C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
  }};

  static const std::array<DayForecast, 7> kDaily {{
      {"\xE4\xBB\x8A\xE5\xA4\xA9", "28/22", "\xE5\xB0\x8F\xE9\x9B\xA8"},
      {"\xE6\x98\x8E\xE5\xA4\xA9", "26/22", "\xE5\xB0\x8F\xE9\x9B\xA8"},
      {"\xE5\x91\xA8\xE5\x85\xAD", "31/23", "\xE9\x98\xB5\xE9\x9B\xA8"},
      {"\xE5\x91\xA8\xE6\x97\xA5", "29/24", "\xE5\xA4\x9A\xE4\xBA\x91"},
      {"\xE5\x91\xA8\xE4\xB8\x80", "33/25", "\xE5\xA4\x9A\xE4\xBA\x91"},
      {"\xE5\x91\xA8\xE4\xBA\x8C", "32/24", "\xE9\x98\xB4"},
      {"\xE5\x91\xA8\xE4\xB8\x89", "30/23", "\xE5\xB0\x8F\xE9\x9B\xA8"},
  }};

  static const std::array<WeatherIndex, 4> kIndices {{
      {"46", "\xE7\xA9\xBA\xE6\xB0\x94\xE8\xB4\xA8\xE9\x87\x8F", 0xA5E9FF},
      {"79", "\xE7\x9B\xB8\xE5\xAF\xB9\xE6\xB9\xBF\xE5\xBA\xA6", 0xBFE7FF},
      {"1", "\xE8\xA5\xBF\xE9\xA3\x8E", 0xD8E9FF},
      {"1", "\xE7\xB4\xAB\xE5\xA4\x96\xE7\xBA\xBF\xE6\x8C\x87\xE6\x95\xB0", 0xFFE082},
  }};

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t section_w = std::min<lv_coord_t>(screen_w - 16, 224);
  const lv_coord_t section_h = screen_h - 16;
  page_pitch_ = section_h + kWeatherSectionGap;

  scroll_root_ = lv_obj_create(root);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll_root_);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_ONE);
  lv_obj_set_size(scroll_root_, screen_w, screen_h);
  lv_obj_set_scroll_dir(scroll_root_, LV_DIR_VER);
  lv_obj_set_scroll_snap_y(scroll_root_, LV_SCROLL_SNAP_START);
  lv_obj_set_scrollbar_mode(scroll_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll_root_, 0, 0);
  ui_set_flex_column(scroll_root_, 8, kWeatherSectionGap);
  lv_obj_set_style_pad_bottom(scroll_root_, 8, 0);

  lv_obj_t* current = create_weather_section(
      scroll_root_, "\xE9\xBE\x99\xE6\xB9\xBE", "11:11", section_w, section_h);
  lv_obj_t* hourly = create_weather_section(
      scroll_root_, "\xE5\xA4\xA9\xE6\xB0\x94\xE9\xA2\x84\xE6\xB5\x8B", "11:10", section_w, section_h);
  lv_obj_t* daily = create_weather_section(
      scroll_root_, "\xE6\x9C\xAA\xE6\x9D\xA5\xE5\xA4\xA9\xE6\xB0\x94", "11:11", section_w, section_h);
  lv_obj_t* indices = create_weather_section(
      scroll_root_, "\xE5\xA4\xA9\xE6\xB0\x94\xE6\x8C\x87\xE6\x95\xB0", "11:11", section_w, section_h);
  lv_obj_t* sunrise = create_weather_section(
      scroll_root_, "\xE6\x97\xA5\xE5\x8D\x87\xE6\x97\xA5\xE8\x90\xBD", "11:11", section_w, section_h);
  if (current == nullptr || hourly == nullptr || daily == nullptr || indices == nullptr || sunrise == nullptr) {
    return nullptr;
  }

  {
    lv_obj_t* temp = create_weather_label(current, "28", cjk_font_72(), 0xF8FAFC, 128);
    lv_obj_t* degree = create_weather_label(current, "C", &lv_font_montserrat_34, 0xE8F5FF, 26);
    lv_obj_t* updated =
        create_weather_label(current, "\xE5\x88\x9A\xE5\x88\x9A\xE6\x9B\xB4\xE6\x96\xB0", cjk_font_14(), 0xB9D7F2, 96);
    lv_obj_t* condition =
        create_weather_label(current, "\xE5\xB0\x8F\xE9\x9B\xA8", cjk_font_16(), 0xF8FAFC, 100);
    lv_obj_t* range = create_weather_label(current, "28C / 22C", &lv_font_montserrat_20, 0xD8E9FF, 130);
    lv_obj_t* chip = create_weather_panel(current, 128, 28, 0x143B63, 0x275F8C, 14);
    lv_obj_t* air = chip == nullptr
                        ? nullptr
                        : create_weather_label(chip,
                                               "\xE7\xA9\xBA\xE6\xB0\x94\xE8\xB4\xA8\xE9\x87\x8F  \xE4\xBC\x98",
                                               cjk_font_16(),
                                               0xD8E9FF,
                                               112);
    if (temp == nullptr || degree == nullptr || updated == nullptr || condition == nullptr || range == nullptr ||
        air == nullptr || chip == nullptr) {
      return nullptr;
    }
    lv_obj_align(updated, LV_ALIGN_TOP_LEFT, 18, 42);
    lv_obj_align(temp, LV_ALIGN_TOP_MID, -24, 84);
    lv_obj_align_to(degree, temp, LV_ALIGN_OUT_RIGHT_TOP, -6, 12);
    lv_obj_align(condition, LV_ALIGN_TOP_MID, 0, 154);
    lv_obj_align(range, LV_ALIGN_TOP_MID, 0, 184);
    lv_obj_align(chip, LV_ALIGN_BOTTOM_MID, 0, -22);
    lv_obj_align(air, LV_ALIGN_CENTER, 0, 0);
  }

  {
    lv_obj_t* row = create_weather_horizontal_row(hourly, section_w - 28, 156);
    if (row == nullptr) {
      return nullptr;
    }
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);

    for (const auto& item : kHourly) {
      lv_obj_t* card = create_weather_panel(row, 62, 142, 0x0D3158, 0x1A527F, 20);
      if (card == nullptr) {
        return nullptr;
      }
      lv_obj_t* time = create_weather_label(card, item.time, cjk_font_14(), 0xE8F5FF, 54);
      lv_obj_t* temp = create_weather_label(card, item.temp, &lv_font_montserrat_20, 0xF8FAFC, 54);
      lv_obj_t* wind = create_weather_label(card, item.wind, cjk_font_14(), 0xCDE6FF, 54);
      lv_obj_t* air = create_weather_label(card, item.air, cjk_font_14(), 0xD7FBE8, 54);
      if (time == nullptr || temp == nullptr || wind == nullptr || air == nullptr) {
        return nullptr;
      }
      lv_obj_align(time, LV_ALIGN_TOP_MID, 0, 10);
      lv_obj_align(temp, LV_ALIGN_TOP_MID, 0, 38);
      create_weather_mini_icon(card, 16, 66, 30);
      lv_obj_align(wind, LV_ALIGN_TOP_MID, 0, 102);
      lv_obj_align(air, LV_ALIGN_TOP_MID, 0, 122);
    }
  }

  {
    lv_obj_t* row = create_weather_horizontal_row(daily, section_w - 28, 158);
    if (row == nullptr) {
      return nullptr;
    }
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);

    for (const auto& item : kDaily) {
      lv_obj_t* card = create_weather_panel(row, 54, 144, 0x0D3158, 0x1A527F, 18);
      if (card == nullptr) {
        return nullptr;
      }
      lv_obj_t* day = create_weather_label(card, item.day, cjk_font_14(), 0xF8FAFC, 46);
      lv_obj_t* weather = create_weather_label(card, item.weather, cjk_font_14(), 0xCDE6FF, 46);
      lv_obj_t* temp = create_weather_label(card, item.temp, &lv_font_montserrat_16, 0xF8FAFC, 48);
      if (day == nullptr || weather == nullptr || temp == nullptr) {
        return nullptr;
      }
      lv_obj_align(day, LV_ALIGN_TOP_MID, 0, 12);
      create_weather_mini_icon(card, 13, 44, 30);
      lv_obj_align(weather, LV_ALIGN_TOP_MID, 0, 82);
      lv_obj_align(temp, LV_ALIGN_TOP_MID, 0, 112);
    }
  }

  {
    for (std::size_t index = 0; index < kIndices.size(); ++index) {
      const auto& item = kIndices[index];
      const lv_coord_t x = static_cast<lv_coord_t>(14 + (index % 2) * 100);
      const lv_coord_t y = static_cast<lv_coord_t>(60 + (index / 2) * 78);
      lv_obj_t* card = create_weather_panel(indices, 92, 68, 0x0D3158, 0x1A527F, 18);
      lv_obj_t* value = create_weather_label(card, item.value, &lv_font_montserrat_34, 0xF8FAFC, 62);
      lv_obj_t* marker = lv_obj_create(card);
      lv_obj_t* label = create_weather_label(card, item.label, cjk_font_14(), 0xCDE6FF, 84);
      if (card == nullptr || value == nullptr || marker == nullptr || label == nullptr) {
        return nullptr;
      }
      lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, y);
      lv_obj_align(value, LV_ALIGN_TOP_LEFT, 8, 7);
      ui_prepare_box(marker);
      lv_obj_set_size(marker, 8, 8);
      lv_obj_set_style_bg_color(marker, lv_color_hex(item.accent), 0);
      lv_obj_set_style_bg_opa(marker, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(marker, LV_RADIUS_CIRCLE, 0);
      lv_obj_remove_flag(marker, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_align(marker, LV_ALIGN_TOP_RIGHT, -18, 18);
      lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 8, -8);
    }
  }

  {
    static const lv_point_precise_t kSunPath[] = {{28, 112}, {58, 72}, {96, 52}, {134, 72}, {164, 112}};
    static const lv_point_precise_t kHorizon[] = {{22, 112}, {170, 112}};

    lv_obj_t* graph = create_weather_panel(sunrise, section_w - 38, 126, 0x0D3158, 0x1A527F, 22);
    if (graph == nullptr) {
      return nullptr;
    }
    lv_obj_align(graph, LV_ALIGN_TOP_MID, 0, 58);

    lv_obj_t* sun_path = lv_line_create(graph);
    lv_obj_t* horizon = lv_line_create(graph);
    lv_obj_t* sun = lv_obj_create(graph);
    lv_obj_t* rise = create_weather_label(sunrise, "\xE6\x97\xA5\xE5\x87\xBA  05:05", cjk_font_16(), 0xF8FAFC, 150);
    lv_obj_t* set = create_weather_label(sunrise, "\xE6\x97\xA5\xE8\x90\xBD  18:42", cjk_font_16(), 0xF8FAFC, 150);
    if (sun_path == nullptr || horizon == nullptr || sun == nullptr || rise == nullptr || set == nullptr) {
      return nullptr;
    }

    lv_line_set_points(sun_path, kSunPath, 5);
    lv_obj_set_size(sun_path, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_line_width(sun_path, 5, 0);
    lv_obj_set_style_line_color(sun_path, lv_color_hex(0x9BD7FF), 0);
    lv_obj_set_style_line_opa(sun_path, LV_OPA_70, 0);
    lv_obj_set_style_line_rounded(sun_path, true, 0);

    lv_line_set_points(horizon, kHorizon, 2);
    lv_obj_set_size(horizon, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_line_width(horizon, 2, 0);
    lv_obj_set_style_line_color(horizon, lv_color_hex(0xD8E9FF), 0);
    lv_obj_set_style_line_opa(horizon, LV_OPA_50, 0);

    ui_prepare_box(sun);
    lv_obj_set_size(sun, 22, 22);
    lv_obj_set_style_bg_color(sun, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(sun, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(sun, 0, 0);
    lv_obj_set_style_radius(sun, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(sun, LV_ALIGN_TOP_LEFT, 86, 48);

    lv_obj_align(rise, LV_ALIGN_BOTTOM_MID, 0, -38);
    lv_obj_align(set, LV_ALIGN_BOTTOM_MID, 0, -14);
  }

  bind_input();
  return root;
}

void WeatherAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root_);
                                 const std::int32_t page_pitch = page_pitch_ > 0 ? page_pitch_ : 1;
                                 const std::int32_t detents = std::max<std::int16_t>(1, command->value);
                                 auto scroll_to_page = [&](bool forward) {
                                   const std::int32_t current_index = forward
                                                                          ? current_y / page_pitch
                                                                          : (current_y + page_pitch - 1) / page_pitch;
                                   const std::int32_t target_index =
                                       std::clamp(current_index + (forward ? detents : -detents),
                                                  0,
                                                  kWeatherSectionCount - 1);
                                   lv_obj_scroll_to_y(scroll_root_, target_index * page_pitch, LV_ANIM_ON);
                                 };
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     scroll_to_page(true);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     scroll_to_page(false);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

StepsAppPage::StepsAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId StepsAppPage::id() const {
  return PageId::Pedometer;
}

const char* StepsAppPage::name() const {
  return page_name(PageId::Pedometer);
}

void StepsAppPage::on_will_appear() {
  if (const auto steps = data_center_.steps()) {
    steps_model_ = *steps;
  }
  refresh_steps_view();
}

void StepsAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* StepsAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t viewport_h = screen_h - kStepsScrollTop - kStepsScrollBottom;
  const lv_coord_t card_w = screen_w - kStepsScrollInset * 2;
  const lv_coord_t overview_h = viewport_h;
  const lv_coord_t metric_card_h = viewport_h;

  scroll_root_ = create_steps_scroll_root(root, screen_w, screen_h);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* overview = create_steps_panel(scroll_root_, card_w, overview_h, 0x050A12);
  if (overview == nullptr) {
    return nullptr;
  }

  lv_obj_t* arc_outer = create_steps_arc(overview, 174, 16, 0xF97316, 1, 400);
  steps_arc_ = create_steps_arc(overview, 138, 15, 0xF6D34D, static_cast<std::int32_t>(steps_model_.daily_steps), 6000);
  lv_obj_t* arc_inner = create_steps_arc(overview, 102, 14, 0x14B8FF, 0, 30);
  if (arc_outer == nullptr || steps_arc_ == nullptr || arc_inner == nullptr) {
    return nullptr;
  }
  lv_obj_align(arc_outer, LV_ALIGN_TOP_MID, 0, 14);
  lv_obj_align(steps_arc_, LV_ALIGN_TOP_MID, 0, 32);
  lv_obj_align(arc_inner, LV_ALIGN_TOP_MID, 0, 50);

  if (!create_steps_metric_line(overview, static_cast<lv_coord_t>(overview_h - 122), 0xF97316, "1", "/400") ||
      !create_steps_metric_line(overview, static_cast<lv_coord_t>(overview_h - 38), 0x14B8FF, "0", "/30")) {
    return nullptr;
  }

  {
    const lv_coord_t y = static_cast<lv_coord_t>(overview_h - 76);
    lv_obj_t* icon = create_steps_icon(overview, 34, 0x1A2430, 0xF6D34D, true);
    steps_metric_value_label_ = create_steps_label(overview, "0", &lv_font_montserrat_28, 0xF8FAFC, LV_SIZE_CONTENT);
    steps_metric_target_label_ = create_steps_label(overview, "/6000", &lv_font_montserrat_16, 0xB9C7D9, LV_SIZE_CONTENT);
    if (icon == nullptr || steps_metric_value_label_ == nullptr || steps_metric_target_label_ == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_bg_color(icon, lv_color_mix(lv_color_hex(0xF6D34D), lv_color_hex(0x0A1626), LV_OPA_70), 0);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 18, y);
    lv_obj_align(steps_metric_value_label_, LV_ALIGN_TOP_LEFT, 64, y - 1);
    lv_obj_align_to(steps_metric_target_label_, steps_metric_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  }

  lv_obj_t* kcal = create_steps_distribution_card(scroll_root_,
                                                  card_w,
                                                  metric_card_h,
                                                  "\xE5\x8D\xA1\xE8\xB7\xAF\xE9\x87\x8C",
                                                  "1",
                                                  "\xE5\x8D\x83\xE5\x8D\xA1",
                                                  "\xE7\x9B\xAE\xE6\xA0\x87 400\xE5\x8D\x83\xE5\x8D\xA1",
                                                  0xF97316,
                                                  false);
  lv_obj_t* steps = create_steps_panel(scroll_root_, card_w, metric_card_h);
  lv_obj_t* active = create_steps_distribution_card(scroll_root_,
                                                    card_w,
                                                    metric_card_h,
                                                    "\xE4\xB8\xAD\xE9\xAB\x98\xE5\xBC\xBA\xE5\xBA\xA6\xE6\xB4\xBB\xE5\x8A\xA8",
                                                    "0",
                                                    "\xE5\x88\x86\xE9\x92\x9F",
                                                    "\xE7\x9B\xAE\xE6\xA0\x87 30\xE5\x88\x86\xE9\x92\x9F",
                                                    0x14B8FF,
                                                    false);
  if (kcal == nullptr || steps == nullptr || active == nullptr) {
    return nullptr;
  }

  {
    lv_obj_t* icon = create_steps_icon(steps, 36, 0x1A2430, 0xF6D34D, true);
    lv_obj_t* title_label = create_steps_label(steps, "\xE6\xAD\xA5\xE6\x95\xB0", cjk_font_16(), 0xF8FAFC, card_w - 76);
    lv_obj_t* divider = lv_obj_create(steps);
    steps_card_value_label_ = create_steps_label(steps, "0", &lv_font_montserrat_42, 0xF8FAFC, LV_SIZE_CONTENT);
    steps_card_unit_label_ = create_steps_label(steps, "\xE6\xAD\xA5", cjk_font_16(), 0xF8FAFC, LV_SIZE_CONTENT);
    lv_obj_t* target_label =
        create_steps_label(steps, "\xE7\x9B\xAE\xE6\xA0\x87 6000\xE6\xAD\xA5", cjk_font_16(), 0xAFC4DA, card_w - 36);
    if (icon == nullptr || title_label == nullptr || divider == nullptr || steps_card_value_label_ == nullptr ||
        steps_card_unit_label_ == nullptr || target_label == nullptr) {
      return nullptr;
    }

    const lv_coord_t divider_y = clamp_coord(scale_by_ratio(metric_card_h, 28, 100), 58, 66);
    const lv_coord_t value_y = clamp_coord(scale_by_ratio(metric_card_h, 35, 100), 76, 86);
    const lv_coord_t target_y = clamp_coord(scale_by_ratio(metric_card_h, 57, 100), 118, 132);
    const lv_coord_t tick_y = clamp_coord(scale_by_ratio(metric_card_h, 68, 100), 140, 154);
    const lv_coord_t tick_label_y = clamp_coord(scale_by_ratio(metric_card_h, 88, 100), 184, 198);

    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 20, 16);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 66, 22);

    ui_prepare_box(divider);
    lv_obj_set_size(divider, card_w - 36, 1);
    lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
    lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
    lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, divider_y);

    lv_obj_align(steps_card_value_label_, LV_ALIGN_TOP_LEFT, 18, value_y);
    lv_obj_align_to(steps_card_unit_label_, steps_card_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
    lv_obj_align(target_label, LV_ALIGN_TOP_LEFT, 22, target_y);

    lv_coord_t xs[] = {24, static_cast<lv_coord_t>(card_w / 2), static_cast<lv_coord_t>(card_w - 24)};
    const char* labels[] = {"0", "12", "24"};
    for (std::size_t i = 0; i < 3; ++i) {
      lv_obj_t* tick = lv_obj_create(steps);
      lv_obj_t* tick_label = create_steps_label(steps, labels[i], &lv_font_montserrat_16, 0x8DB9E3, 32);
      if (tick == nullptr || tick_label == nullptr) {
        return nullptr;
      }
      ui_prepare_box(tick);
      lv_obj_set_size(tick, 1, 46);
      lv_obj_set_style_bg_color(tick, lv_color_hex(0x5AA4DD), 0);
      lv_obj_set_style_bg_opa(tick, LV_OPA_70, 0);
      lv_obj_align(tick, LV_ALIGN_TOP_LEFT, xs[i], tick_y);
      lv_obj_align(tick_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(xs[i] - 10), tick_label_y);
    }

    lv_obj_t* dot = lv_obj_create(steps);
    if (dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 7, 7);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0xF6D34D), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_align(dot, LV_ALIGN_TOP_LEFT, card_w - 30, clamp_coord(scale_by_ratio(metric_card_h, 67, 100), 136, 174));
  }

  lv_obj_t* info = create_steps_panel(scroll_root_, card_w, 78, 0x111D2E);
  lv_obj_t* info_icon = info == nullptr ? nullptr : create_steps_icon(info, 38, 0x075985, 0x7DD3FC, false);
  lv_obj_t* info_label =
      info == nullptr ? nullptr : create_steps_label(info, "\xE6\x95\xB0\xE6\x8D\xAE\xE8\xAF\xB4\xE6\x98\x8E", cjk_font_16(), 0xF8FAFC, 132);
  if (info == nullptr || info_icon == nullptr || info_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(info, 18, 0);
  lv_obj_align(info_icon, LV_ALIGN_LEFT_MID, 20, 0);
  lv_obj_align(info_label, LV_ALIGN_LEFT_MID, 72, 0);
  attach_click_guard(info);
  lv_obj_add_event_cb(info, data_info_event_cb, LV_EVENT_CLICKED, this);

  track(data_center_.subscribe(EventId::StepsChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<StepsModel>(&event.payload)) {
                                   apply_steps(*model);
                                 }
                               }));

  bind_input();
  on_will_appear();
  return root;
}

void StepsAppPage::data_info_event_cb(lv_event_t* event) {
  auto* self = static_cast<StepsAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::PedometerDataInfo});
}

void StepsAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void StepsAppPage::apply_steps(const StepsModel& model) {
  steps_model_ = model;
  refresh_steps_view();
}

void StepsAppPage::refresh_steps_view() {
  char value_text[24] = {};
  std::snprintf(value_text, sizeof(value_text), "%lu", static_cast<unsigned long>(steps_model_.daily_steps));

  if (steps_metric_value_label_ != nullptr) {
    set_content_width_label(steps_metric_value_label_, value_text, steps_overview_value_font(steps_model_.daily_steps), 0xF8FAFC);
  }
  if (steps_metric_target_label_ != nullptr && steps_metric_value_label_ != nullptr) {
    lv_obj_align_to(steps_metric_target_label_, steps_metric_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  }
  if (steps_card_value_label_ != nullptr) {
    set_content_width_label(steps_card_value_label_, value_text, steps_card_value_font(steps_model_.daily_steps), 0xF8FAFC);
  }
  if (steps_card_unit_label_ != nullptr && steps_card_value_label_ != nullptr) {
    lv_obj_align_to(steps_card_unit_label_, steps_card_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
  }
  if (steps_arc_ != nullptr) {
    const std::int32_t clamped_steps =
        std::clamp<std::int32_t>(static_cast<std::int32_t>(steps_model_.daily_steps), 0, 6000);
    lv_arc_set_angles(steps_arc_, 180, 180 + std::clamp(clamped_steps * 180 / 6000, 6, 180));
  }
}

void StepsAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&StepsAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void StepsAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

void StepsAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<StepsAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void StepsAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

StepsDataInfoPage::StepsDataInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId StepsDataInfoPage::id() const {
  return PageId::PedometerDataInfo;
}

const char* StepsDataInfoPage::name() const {
  return page_name(PageId::PedometerDataInfo);
}

void StepsDataInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* StepsDataInfoPage::build() {
  struct InfoBlock {
    const char* title;
    const char* body;
    std::uint32_t accent;
    bool foot;
  };

  static const std::array<InfoBlock, 4> kInfoBlocks {{
      {"\xE6\xAD\xA5\xE6\x95\xB0",
       "\xE6\xAD\xA5\xE6\x95\xB0\xE7\x94\xA8\xE4\xBA\x8E\xE4\xBC\xB0\xE8\xAE\xA1\xE6\x97\xA5\xE5\xB8\xB8\xE6\xB4\xBB\xE5\x8A\xA8\xE9\x87\x8F\xEF\xBC\x8C\xE4\xBD\x8E\xE4\xBA\x8E\x35\x30\x30\x30\xE6\xAD\xA5\xE9\x80\x9A\xE5\xB8\xB8\xE5\xB1\x9E\xE4\xBA\x8E\xE9\x9D\x99\xE5\x9D\x90\xE5\xB0\x91\xE5\x8A\xA8\xEF\xBC\x9B\x38\x30\x30\x30\x2D\x31\x30\x30\x30\x30\xE6\xAD\xA5\xE6\x9B\xB4\xE6\x8E\xA5\xE8\xBF\x91\xE6\xB4\xBB\xE8\xB7\x83\xE7\x94\x9F\xE6\xB4\xBB\xE6\x96\xB9\xE5\xBC\x8F\xE3\x80\x82",
       0xF6D34D,
       true},
      {"\xE5\x8D\xA1\xE8\xB7\xAF\xE9\x87\x8C",
       "\xE5\x8D\xA1\xE8\xB7\xAF\xE9\x87\x8C\xE5\x8F\x8D\xE6\x98\xA0\xE4\xBD\x93\xE5\x8A\x9B\xE6\xB4\xBB\xE5\x8A\xA8\xE6\xB6\x88\xE8\x80\x97\xE7\x9A\x84\xE8\x83\xBD\xE9\x87\x8F\xEF\xBC\x8C\xE6\x9C\xAC\xE9\xA1\xB5\xE4\xBB\x85\xE5\xB1\x95\xE7\xA4\xBA\xE6\xA8\xA1\xE6\x8B\x9F\xE6\x95\xB0\xE6\x8D\xAE\xEF\xBC\x8C\xE5\x90\x8E\xE7\xBB\xAD\xE5\x8F\xAF\xE7\x94\xB1\xE4\xBC\xA0\xE6\x84\x9F\xE5\x99\xA8\xE5\x92\x8C\xE7\xAE\x97\xE6\xB3\x95\xE6\x9B\xBF\xE6\x8D\xA2\xE3\x80\x82",
       0xF97316,
       false},
      {"\xE4\xB8\xAD\xE9\xAB\x98\xE5\xBC\xBA\xE5\xBA\xA6\xE6\xB4\xBB\xE5\x8A\xA8",
       "\xE4\xB8\xAD\xE9\xAB\x98\xE5\xBC\xBA\xE5\xBA\xA6\xE6\xB4\xBB\xE5\x8A\xA8\xE5\x9C\xA8\xE5\xBF\x83\xE7\x8E\x87\xE6\x88\x96\xE6\xAD\xA5\xE9\xA2\x91\xE8\xBE\xBE\xE5\x88\xB0\xE9\x98\x88\xE5\x80\xBC\xE6\x97\xB6\xE8\xAE\xB0\xE5\xBD\x95\xEF\xBC\x8C\xE7\x94\xA8\xE4\xBA\x8E\xE8\xA7\x82\xE5\xAF\x9F\xE6\xAF\x8F\xE5\xA4\xA9\xE6\x9C\x89\xE6\x95\x88\xE8\xBF\x90\xE5\x8A\xA8\xE6\x97\xB6\xE9\x95\xBF\xE3\x80\x82",
       0x14B8FF,
       false},
      {"\xE7\xAB\x99\xE7\xAB\x8B",
       "\xE4\xB9\x85\xE5\x9D\x90\xE5\xB0\x91\xE5\x8A\xA8\xE5\x8F\xAF\xE8\x83\xBD\xE5\xB8\xA6\xE6\x9D\xA5\xE5\x81\xA5\xE5\xBA\xB7\xE9\xA3\x8E\xE9\x99\xA9\xEF\xBC\x8C\xE6\x97\xA5\xE5\xB8\xB8\xE4\xBD\xA9\xE6\x88\xB4\xE6\x89\x8B\xE8\xA1\xA8\xE6\x97\xB6\xE5\xBB\xBA\xE8\xAE\xAE\xE6\xAF\x8F\xE5\xB0\x8F\xE6\x97\xB6\xE8\xB5\xB7\xE8\xBA\xAB\xE6\xB4\xBB\xE5\x8A\xA8\xE4\xB8\x80\xE6\xAC\xA1\xE3\x80\x82",
       0x7DD3FC,
       false},
  }};

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = std::min<lv_coord_t>(screen_w - 18, 224);
  scroll_root_ = create_steps_scroll_root(root, screen_w, screen_h);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* header = create_steps_panel(scroll_root_, card_w, 58, 0x07111D);
  lv_obj_t* back_button = header == nullptr ? nullptr : lv_button_create(header);
  lv_obj_t* back = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xCDEBFF, 18);
  lv_obj_t* title =
      header == nullptr ? nullptr : create_steps_label(header, "\xE6\x95\xB0\xE6\x8D\xAE\xE8\xAF\xB4\xE6\x98\x8E", cjk_font_16(), 0xF8FAFC, 126);
  lv_obj_t* time = header == nullptr ? nullptr : create_steps_label(header, "17:47", &lv_font_montserrat_18, 0xD8E9FF, 58);
  if (header == nullptr || back_button == nullptr || back == nullptr || title == nullptr || time == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(header, 20, 0);
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 40, 40);
  lv_obj_align(back_button, LV_ALIGN_LEFT_MID, 6, 0);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, &StepsDataInfoPage::back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title, LV_ALIGN_LEFT_MID, 34, 0);
  lv_obj_align(time, LV_ALIGN_RIGHT_MID, -14, 0);

  for (const InfoBlock& block : kInfoBlocks) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 170, 0x07111D);
    lv_obj_t* icon = card == nullptr ? nullptr : create_steps_icon(card, 30, 0x1A2430, block.accent, block.foot);
    lv_obj_t* label = card == nullptr ? nullptr : create_steps_label(card, block.title, cjk_font_16(), 0xF8FAFC, card_w - 64);
    lv_obj_t* body = card == nullptr ? nullptr : create_steps_label(card, block.body, cjk_font_16(), 0xD8E9FF, card_w - 32, LV_LABEL_LONG_WRAP);
    if (card == nullptr || icon == nullptr || label == nullptr || body == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_radius(card, 18, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 14, 14);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 52, 17);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 16, 58);
  }

  bind_input();
  return root;
}

void StepsDataInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<StepsDataInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void StepsDataInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void StepsDataInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&StepsDataInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void StepsDataInfoPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

void StepsDataInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<StepsDataInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void StepsDataInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

SleepAppPage::SleepAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId SleepAppPage::id() const {
  return PageId::AppSleep;
}

const char* SleepAppPage::name() const {
  return page_name(PageId::AppSleep);
}

void SleepAppPage::on_will_appear() {
  refresh_header_time();
}

void SleepAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "睡眠", cjk_font_20(), 0xF8FAFC, 80);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 16, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* last_night_card = create_steps_panel(scroll_root_, card_w, 168, 0x0A1626);
  if (last_night_card == nullptr) {
    return nullptr;
  }
  lv_obj_t* last_night_icon =
      create_sleep_round_icon(last_night_card, 52, 0x0A4F76, 0x7DD3FC, sleep_icon_asset_path(), "Z", cjk_font_20());
  lv_obj_t* last_night_label = create_steps_label(last_night_card,
                                                  "无睡眠记录，请佩戴入睡",
                                                  cjk_font_20(),
                                                  0xF8FAFC,
                                                  card_w - 40,
                                                  LV_LABEL_LONG_WRAP);
  if (last_night_icon == nullptr || last_night_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(last_night_icon, LV_ALIGN_TOP_MID, 0, 28);
  lv_obj_align(last_night_label, LV_ALIGN_TOP_MID, 0, 96);
  lv_obj_set_style_text_align(last_night_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* week_card = create_steps_panel(scroll_root_, card_w, 206, 0x0A1626);
  if (week_card == nullptr) {
    return nullptr;
  }
  lv_obj_t* week_icon =
      create_sleep_round_icon(week_card, 42, 0x0A4F76, 0x7DD3FC, sleep_icon_asset_path(), "Z", cjk_font_20());
  lv_obj_t* week_title = create_steps_label(week_card, "近7天睡眠", cjk_font_20(), 0xF8FAFC, card_w - 90);
  lv_obj_t* divider = lv_obj_create(week_card);
  lv_obj_t* week_value =
      create_steps_label(week_card, "--小时--分钟", cjk_font_20(), 0xF8FAFC, card_w - 36);
  lv_obj_t* week_subtitle = create_steps_label(week_card, "平均时长", cjk_font_16(), 0xAFC4DA, card_w - 36);
  if (week_icon == nullptr || week_title == nullptr || divider == nullptr || week_value == nullptr ||
      week_subtitle == nullptr) {
    return nullptr;
  }
  lv_obj_align(week_icon, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(week_title, LV_ALIGN_TOP_LEFT, 68, 22);
  ui_prepare_box(divider);
  lv_obj_set_size(divider, card_w - 36, 1);
  lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
  lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
  lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, 72);
  lv_obj_align(week_value, LV_ALIGN_TOP_LEFT, 18, 94);
  lv_obj_align(week_subtitle, LV_ALIGN_TOP_LEFT, 18, 130);

  const lv_coord_t tick_y = 152;
  const lv_coord_t tick_label_y = 184;
  const lv_coord_t chart_left = 28;
  const lv_coord_t chart_right = card_w - 28;
  const lv_coord_t chart_width = chart_right - chart_left;
  for (int i = 0; i < 7; ++i) {
    lv_obj_t* tick = lv_obj_create(week_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, 34);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x486784), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    const lv_coord_t x = static_cast<lv_coord_t>(chart_left + (chart_width * i) / 6);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, x, tick_y);
    if (i == 0 || i == 6) {
      lv_obj_t* date_label = create_steps_label(week_card, i == 0 ? "5/18" : "5/24", &lv_font_montserrat_14, 0x8DB9E3, 44);
      if (date_label == nullptr) {
        return nullptr;
      }
      lv_obj_align(date_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(x - 12), tick_label_y);
    }
  }

  for (const auto& entry :
       {std::pair<const char*, PageId> {"设置", PageId::AppSleepSettings},
        std::pair<const char*, PageId> {"说明", PageId::AppSleepInfo}}) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 86, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    const char* fallback = entry.second == PageId::AppSleepSettings ? LV_SYMBOL_SETTINGS : "i";
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             entry.second == PageId::AppSleepSettings ? 0x114C9D : 0x0A4F76,
                                             0x7DD3FC,
                                             nullptr,
                                             fallback,
                                             entry.second == PageId::AppSleepSettings ? &lv_font_montserrat_18
                                                                                      : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, entry.first, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(entry.second)));
  }

  bind_input();
  on_will_appear();
  return root;
}

void SleepAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
}

void SleepAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void SleepAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void SleepAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&SleepAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void SleepAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

SleepSettingsPage::SleepSettingsPage(DataCenter& data_center) : PageBase(data_center) {
  rows_[0].kind = RowKind::HighPrecisionSleep;
  rows_[0].title = "睡眠高精度监测";
  rows_[1].kind = RowKind::SleepBreathingQuality;
  rows_[1].title = "睡眠呼吸质量监测";
}

PageId SleepSettingsPage::id() const {
  return PageId::AppSleepSettings;
}

const char* SleepSettingsPage::name() const {
  return page_name(PageId::AppSleepSettings);
}

void SleepSettingsPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_rows();
}

void SleepSettingsPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "设置", cjk_font_20(), 0xF8FAFC, 80);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  for (std::size_t i = 0; i < rows_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
    lv_obj_t* title = row == nullptr ? nullptr
                                     : create_steps_label(row, rows_[i].title, cjk_font_20(), 0xF8FAFC, card_w - 32, LV_LABEL_LONG_WRAP);
    lv_obj_t* status = row == nullptr ? nullptr : create_steps_label(row, "关闭", cjk_font_16(), 0xAFC4DA, card_w - 32);
    if (row == nullptr || title == nullptr || status == nullptr) {
      return nullptr;
    }
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 18, 16);
    lv_obj_align(status, LV_ALIGN_TOP_LEFT, 18, 56);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, row_event_cb, LV_EVENT_CLICKED, this);
    const PageId target =
        rows_[i].kind == RowKind::HighPrecisionSleep ? PageId::AppSleepSettingHighPrecision
                                                     : PageId::AppSleepSettingBreathingQuality;
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
    rows_[i].row = row;
    rows_[i].status_label = status;
  }

  bind_input();
  on_will_appear();
  return root;
}

void SleepSettingsPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void SleepSettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
}

void SleepSettingsPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void SleepSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void SleepSettingsPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepSettingsPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void SleepSettingsPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepSettingsPage::refresh_rows() {
  for (const auto& row : rows_) {
    if (row.status_label == nullptr) {
      continue;
    }
    lv_label_set_text(row.status_label, row_enabled(row.kind) ? "开启" : "关闭");
  }
}

bool SleepSettingsPage::row_enabled(RowKind kind) const {
  switch (kind) {
    case RowKind::HighPrecisionSleep:
      return current_settings_.high_precision_sleep_enabled;
    case RowKind::SleepBreathingQuality:
      return current_settings_.sleep_breathing_quality_enabled;
    default:
      return false;
  }
}

void SleepSettingsPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&SleepSettingsPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void SleepSettingsPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

SleepMonitoringDetailPage::SleepMonitoringDetailPage(DataCenter& data_center,
                                                     PageId page_id,
                                                     const char* title,
                                                     const char* body,
                                                     SettingKind kind)
    : PageBase(data_center), page_id_(page_id), title_(title), body_(body), kind_(kind) {}

PageId SleepMonitoringDetailPage::id() const {
  return page_id_;
}

const char* SleepMonitoringDetailPage::name() const {
  return page_name(page_id_);
}

void SleepMonitoringDetailPage::on_will_appear() {
  apply_enabled(current_enabled());
  refresh_header_time();
}

void SleepMonitoringDetailPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepMonitoringDetailPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, title_, cjk_font_20(), 0xF8FAFC, 124);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* switch_card = create_steps_panel(scroll_root_, card_w, 96, 0x102033);
  lv_obj_t* switch_title = switch_card == nullptr
                               ? nullptr
                               : create_steps_label(switch_card, title_, cjk_font_20(), 0xF8FAFC, card_w - 112, LV_LABEL_LONG_WRAP);
  switch_track_ = switch_card == nullptr ? nullptr : create_sleep_switch_track(switch_card);
  if (switch_card == nullptr || switch_title == nullptr || switch_track_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(switch_title, LV_ALIGN_TOP_LEFT, 18, 14);
  lv_obj_align(switch_track_, LV_ALIGN_RIGHT_MID, -18, 0);
  attach_click_guard(switch_card);
  lv_obj_add_event_cb(switch_card, switch_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* body_card = create_steps_panel(scroll_root_, card_w, 140, 0x07111D);
  lv_obj_t* body_label = body_card == nullptr ? nullptr : create_steps_label(body_card, body_, cjk_font_16(), 0xD8E9FF, card_w - 32, LV_LABEL_LONG_WRAP);
  if (body_card == nullptr || body_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(body_label, LV_ALIGN_TOP_LEFT, 16, 18);

  bind_input();
  on_will_appear();
  return root;
}

void SleepMonitoringDetailPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepMonitoringDetailPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void SleepMonitoringDetailPage::switch_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepMonitoringDetailPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const bool next_enabled = !self->enabled_;
  self->apply_enabled(next_enabled);
  self->publish_enabled(next_enabled);
}

void SleepMonitoringDetailPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepMonitoringDetailPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void SleepMonitoringDetailPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void SleepMonitoringDetailPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepMonitoringDetailPage::apply_enabled(bool enabled) {
  enabled_ = enabled;
  apply_sleep_switch_style(switch_track_, enabled_);
}

void SleepMonitoringDetailPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepMonitoringDetailPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&SleepMonitoringDetailPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void SleepMonitoringDetailPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

bool SleepMonitoringDetailPage::current_enabled() const {
  const auto& model = data_center_.health_monitoring_settings();
  if (!model.has_value()) {
    return false;
  }
  switch (kind_) {
    case SettingKind::HighPrecisionSleep:
      return model->high_precision_sleep_enabled;
    case SettingKind::SleepBreathingQuality:
      return model->sleep_breathing_quality_enabled;
    default:
      return false;
  }
}

void SleepMonitoringDetailPage::publish_enabled(bool enabled) {
  switch (kind_) {
    case SettingKind::HighPrecisionSleep:
      data_center_.set_high_precision_sleep_enabled(enabled);
      break;
    case SettingKind::SleepBreathingQuality:
      data_center_.set_sleep_breathing_quality_enabled(enabled);
      break;
    default:
      break;
  }
}

SleepInfoPage::SleepInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId SleepInfoPage::id() const {
  return PageId::AppSleepInfo;
}

const char* SleepInfoPage::name() const {
  return page_name(PageId::AppSleepInfo);
}

void SleepInfoPage::on_will_appear() {
  refresh_header_time();
}

void SleepInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepInfoPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "睡眠说明", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* sleep_info_text =
      "佩戴手表入睡，手表将自动记录睡眠信息。手表端只展示最近3条睡眠片段，更多数据请至App端查看。\n\n"
      "推荐在设置中开启“睡眠高精度监测”的设置项，可以帮助睡眠算法更准确地评估你的睡眠状态，识别REM快速眼动期睡眠。\n\n"
      "大于等于3小时的长睡眠，设备会根据可获取的数据展示该片段内的平均心率，小于3小时的零星小睡，只展示入睡醒来时间。小于20分钟的睡眠不会被记录。\n\n"
      "如果睡眠质量很差，睡眠中手腕动作过多，可能会导致睡眠心率测量和血氧测量失败。";

  lv_obj_t* note_body =
      create_steps_label(scroll_root_, sleep_info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void SleepInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void SleepInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void SleepInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void SleepInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&SleepInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void SleepInfoPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

HeartRateAppPage::HeartRateAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateAppPage::id() const {
  return PageId::AppHeartRate;
}

const char* HeartRateAppPage::name() const {
  return page_name(PageId::AppHeartRate);
}

void HeartRateAppPage::on_will_appear() {
  refresh_header_time();
  if (!measurement_completed_) {
    start_entry_measurement();
  }
}

void HeartRateAppPage::on_will_disappear() {
  stop_measurement_timer();
  stop_crown_release_timer();
}

lv_obj_t* HeartRateAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "心率", cjk_font_20(), 0xF8FAFC, 80);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 16, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* stage = create_steps_panel(scroll_root_, card_w, 260, 0x02070D);
  if (stage == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_bg_opa(stage, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(stage, 0, 0);

  measuring_stage_ = create_steps_panel(stage, card_w, 248, 0x02070D);
  result_stage_ = create_steps_panel(stage, card_w, 248, 0x02070D);
  if (measuring_stage_ == nullptr || result_stage_ == nullptr) {
    return nullptr;
  }
  for (lv_obj_t* panel : {measuring_stage_, result_stage_}) {
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 0);
  }

  lv_obj_t* heart_icon = create_contain_image(measuring_stage_, health_heart_asset_path(), 92, 92, LV_ALIGN_TOP_MID, 0, 42);
  lv_obj_t* heart_fallback = nullptr;
  if (heart_icon != nullptr && lv_obj_has_flag(heart_icon, LV_OBJ_FLAG_HIDDEN)) {
    heart_fallback = create_steps_label(measuring_stage_, "HR", &lv_font_montserrat_48, 0xFF3B74, 96);
  }
  lv_obj_t* measuring_title = create_steps_label(measuring_stage_, "正在测量", cjk_font_20(), 0xF8FAFC, card_w);
  lv_obj_t* measuring_hint = create_steps_label(measuring_stage_, "请保持静止", cjk_font_20(), 0xD8E9FF, card_w);
  if (heart_icon == nullptr || measuring_title == nullptr || measuring_hint == nullptr) {
    return nullptr;
  }
  if (heart_fallback != nullptr) {
    lv_obj_set_style_text_align(heart_fallback, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(heart_fallback, LV_ALIGN_TOP_MID, 0, 56);
  }
  lv_obj_set_style_text_align(measuring_title, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(measuring_hint, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(measuring_title, LV_ALIGN_TOP_MID, 0, 146);
  lv_obj_align(measuring_hint, LV_ALIGN_TOP_MID, 0, 182);

  lv_obj_t* chart_card = create_steps_panel(result_stage_, card_w, 124, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }
  lv_obj_align(chart_card, LV_ALIGN_TOP_MID, 0, 0);

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 14;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 84;
  for (int i = 0; i < 5; ++i) {
    lv_obj_t* tick = lv_obj_create(chart_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, chart_h);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5A82B2), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(chart_left + (chart_w * i) / 4), chart_top);
  }

  lv_obj_t* pulse_bar = lv_obj_create(chart_card);
  if (pulse_bar == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pulse_bar);
  lv_obj_set_size(pulse_bar, 8, 72);
  lv_obj_set_style_bg_color(pulse_bar, lv_color_hex(0xFF4F83), 0);
  lv_obj_set_style_bg_opa(pulse_bar, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(pulse_bar, 4, 0);
  lv_obj_align(pulse_bar, LV_ALIGN_TOP_RIGHT, -55, 16);

  lv_obj_t* top_value = create_steps_label(chart_card, "76", &lv_font_montserrat_18, 0xD8E9FF, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "75", &lv_font_montserrat_18, 0xD8E9FF, 28);
  lv_obj_t* left_label = create_steps_label(chart_card, "00:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* mid_label = create_steps_label(chart_card, "12:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* right_label = create_steps_label(chart_card, "24:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  if (top_value == nullptr || bottom_value == nullptr || left_label == nullptr || mid_label == nullptr ||
      right_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(top_value, LV_ALIGN_TOP_RIGHT, -10, 6);
  lv_obj_align(bottom_value, LV_ALIGN_BOTTOM_RIGHT, -10, -12);
  lv_obj_align(left_label, LV_ALIGN_BOTTOM_LEFT, 14, -12);
  lv_obj_align(mid_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(right_label, LV_ALIGN_BOTTOM_RIGHT, -24, -12);

  lv_obj_t* wear_label = create_steps_label(result_stage_, "未佩戴", cjk_font_20(), 0xF8FAFC, card_w - 20);
  lv_obj_t* bpm_value = create_steps_label(result_stage_, "76", &lv_font_montserrat_48, 0xEAF7FF, 116);
  lv_obj_t* heart_small = create_contain_image(result_stage_, health_heart_asset_path(), 24, 24, LV_ALIGN_TOP_LEFT, 124, 190);
  lv_obj_t* unit_label = create_steps_label(result_stage_, "次/分", cjk_font_20(), 0xEAF7FF, 64);
  lv_obj_t* updated_label =
      create_steps_label(result_stage_, "76次/分(1分钟前)", cjk_font_20(), 0xD8E9FF, card_w - 20);
  if (wear_label == nullptr || bpm_value == nullptr || heart_small == nullptr || unit_label == nullptr ||
      updated_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(wear_label, LV_ALIGN_TOP_LEFT, 18, 140);
  lv_obj_align(bpm_value, LV_ALIGN_TOP_LEFT, 16, 166);
  lv_obj_align(unit_label, LV_ALIGN_TOP_LEFT, 150, 202);
  lv_obj_align(updated_label, LV_ALIGN_TOP_LEFT, 18, 230);

  for (const auto& entry : {std::tuple {EntryKind::Resting30Days, "近30天静息", "|||", true},
                            std::tuple {EntryKind::Settings, "心率设置", LV_SYMBOL_SETTINGS, true},
                            std::tuple {EntryKind::Info, "心率说明", "i", true}}) {
    const auto [kind, title, symbol, enabled] = entry;
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 82, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             0x5B1740,
                                             0xFF5AA0,
                                             nullptr,
                                             symbol,
                                             kind == EntryKind::Settings ? &lv_font_montserrat_18 : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, title, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    if (enabled) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(kind)));
    }
  }

  wear_prompt_overlay_ = lv_obj_create(root);
  if (wear_prompt_overlay_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(wear_prompt_overlay_);
  lv_obj_set_size(wear_prompt_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(wear_prompt_overlay_);
  lv_obj_set_style_bg_color(wear_prompt_overlay_, lv_color_hex(0x02070D), 0);
  lv_obj_set_style_bg_opa(wear_prompt_overlay_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(wear_prompt_overlay_, 0, 0);
  lv_obj_set_style_radius(wear_prompt_overlay_, 0, 0);
  lv_obj_add_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* wear_distance = create_steps_label(wear_prompt_overlay_, "1.5cm-2cm", &lv_font_montserrat_28, 0xEAF7FF, 190);
  lv_obj_t* wrist = lv_obj_create(wear_prompt_overlay_);
  lv_obj_t* watch = lv_obj_create(wear_prompt_overlay_);
  lv_obj_t* message = create_steps_label(wear_prompt_overlay_, "请正确佩戴手表后重试", cjk_font_20(), 0xF8FAFC, 210);
  lv_obj_t* info_icon = create_sleep_round_icon(wear_prompt_overlay_, 36, 0x0E4C78, 0xC9F0FF, nullptr, "i", cjk_font_20());
  lv_obj_t* ok_button = lv_button_create(wear_prompt_overlay_);
  lv_obj_t* ok_label = ok_button == nullptr ? nullptr : create_steps_label(ok_button, "知道了", cjk_font_20(), 0xF8FAFC, 130);
  if (wear_distance == nullptr || wrist == nullptr || watch == nullptr || message == nullptr || info_icon == nullptr ||
      ok_button == nullptr || ok_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(wear_distance, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(wear_distance, LV_ALIGN_TOP_MID, 0, 16);

  ui_prepare_box(wrist);
  lv_obj_set_size(wrist, 190, 4);
  lv_obj_set_style_bg_color(wrist, lv_color_hex(0xD8E9FF), 0);
  lv_obj_set_style_bg_opa(wrist, LV_OPA_80, 0);
  lv_obj_set_style_radius(wrist, 2, 0);
  lv_obj_align(wrist, LV_ALIGN_TOP_MID, 0, 82);

  ui_prepare_box(watch);
  lv_obj_set_size(watch, 38, 44);
  lv_obj_set_style_bg_color(watch, lv_color_hex(0x93CFF8), 0);
  lv_obj_set_style_bg_opa(watch, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(watch, 3, 0);
  lv_obj_set_style_border_color(watch, lv_color_hex(0xDFF7FF), 0);
  lv_obj_set_style_radius(watch, 10, 0);
  lv_obj_align(watch, LV_ALIGN_TOP_MID, -54, 60);

  lv_obj_set_style_text_align(message, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(message, LV_ALIGN_TOP_MID, 0, 140);
  lv_obj_align(info_icon, LV_ALIGN_TOP_MID, 0, 182);

  attach_click_guard(ok_button);
  ui_prepare_box(ok_button);
  lv_obj_set_size(ok_button, 164, 48);
  lv_obj_align(ok_button, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_set_style_bg_color(ok_button, lv_color_hex(0x17314D), 0);
  lv_obj_set_style_bg_opa(ok_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(ok_button, 0, 0);
  lv_obj_set_style_radius(ok_button, 18, 0);
  lv_obj_add_event_cb(ok_button, wear_prompt_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_center(ok_label);

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto kind = static_cast<EntryKind>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (kind == EntryKind::Resting30Days) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateResting30Days});
  } else if (kind == EntryKind::Settings) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateSettings});
  } else if (kind == EntryKind::Info) {
    self->request_navigation({NavigationAction::Push, PageId::AppHeartRateInfo});
  }
}

void HeartRateAppPage::wear_prompt_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->hide_wear_prompt();
}

void HeartRateAppPage::measurement_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->measurement_timer_ = nullptr;
  self->complete_entry_measurement();
}

void HeartRateAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateAppPage::start_entry_measurement() {
  show_measurement_stage();
  stop_measurement_timer();
  measurement_timer_ = lv_timer_create(&HeartRateAppPage::measurement_timer_cb, 1200U, this);
  if (measurement_timer_ != nullptr) {
    lv_timer_set_repeat_count(measurement_timer_, 1);
  }
}

void HeartRateAppPage::complete_entry_measurement() {
  measurement_completed_ = true;
  show_result_stage();
  if (!wear_prompt_shown_) {
    show_wear_prompt();
    wear_prompt_shown_ = true;
  }
}

void HeartRateAppPage::show_measurement_stage() {
  if (measuring_stage_ != nullptr) {
    lv_obj_clear_flag(measuring_stage_, LV_OBJ_FLAG_HIDDEN);
  }
  if (result_stage_ != nullptr) {
    lv_obj_add_flag(result_stage_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::show_result_stage() {
  if (measuring_stage_ != nullptr) {
    lv_obj_add_flag(measuring_stage_, LV_OBJ_FLAG_HIDDEN);
  }
  if (result_stage_ != nullptr) {
    lv_obj_clear_flag(result_stage_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::show_wear_prompt() {
  if (wear_prompt_overlay_ == nullptr) {
    return;
  }
  lv_obj_clear_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(wear_prompt_overlay_);
}

void HeartRateAppPage::hide_wear_prompt() {
  if (wear_prompt_overlay_ != nullptr) {
    lv_obj_add_flag(wear_prompt_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void HeartRateAppPage::stop_measurement_timer() {
  if (measurement_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(measurement_timer_);
  measurement_timer_ = nullptr;
}

void HeartRateAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateResting30DaysPage::HeartRateResting30DaysPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateResting30DaysPage::id() const {
  return PageId::AppHeartRateResting30Days;
}

const char* HeartRateResting30DaysPage::name() const {
  return page_name(PageId::AppHeartRateResting30Days);
}

void HeartRateResting30DaysPage::on_will_appear() {
  refresh_header_time();
}

void HeartRateResting30DaysPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateResting30DaysPage::build() {
  static const lv_point_precise_t kRestingLine[] = {
      {42, 48}, {62, 116}, {88, 92}, {104, 24}, {120, 104}, {150, 84}, {166, 88}, {184, 116},
  };

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "近30天静息", cjk_font_20(), 0xF8FAFC, 136);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(back_button, 0, 0);
  lv_obj_center(back_label);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 38, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* chart_card = create_steps_panel(scroll_root_, card_w, 166, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 14;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 116;
  for (int i = 0; i < 5; ++i) {
    lv_obj_t* tick = lv_obj_create(chart_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, chart_h);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5A82B2), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(chart_left + (chart_w * i) / 4), chart_top);
  }

  lv_obj_t* line = lv_line_create(chart_card);
  lv_obj_t* top_value = create_steps_label(chart_card, "74", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "54", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* left_label = create_steps_label(chart_card, "4/26", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* mid_label = create_steps_label(chart_card, "5/10", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* right_label = create_steps_label(chart_card, "5/25", &lv_font_montserrat_16, 0x8DB9E3, 48);
  if (line == nullptr || top_value == nullptr || bottom_value == nullptr || left_label == nullptr || mid_label == nullptr ||
      right_label == nullptr) {
    return nullptr;
  }
  lv_line_set_points(line, kRestingLine, 8);
  lv_obj_set_size(line, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_line_width(line, 5, 0);
  lv_obj_set_style_line_color(line, lv_color_hex(0xFF4F83), 0);
  lv_obj_set_style_line_rounded(line, true, 0);
  lv_obj_align(top_value, LV_ALIGN_TOP_RIGHT, -10, 6);
  lv_obj_align(bottom_value, LV_ALIGN_BOTTOM_RIGHT, -10, -28);
  lv_obj_align(left_label, LV_ALIGN_BOTTOM_LEFT, 16, -12);
  lv_obj_align(mid_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(right_label, LV_ALIGN_BOTTOM_RIGHT, -18, -12);

  lv_obj_t* empty_value = create_steps_label(scroll_root_, "--", &lv_font_montserrat_48, 0xEAF7FF, card_w);
  lv_obj_t* unit_label = create_steps_label(scroll_root_, "次/分", cjk_font_20(), 0xF8FAFC, card_w);
  lv_obj_t* today_label = create_steps_label(scroll_root_, "今日静息", cjk_font_20(), 0xD8E9FF, card_w);
  if (empty_value == nullptr || unit_label == nullptr || today_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(empty_value, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(unit_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(today_label, LV_TEXT_ALIGN_CENTER, 0);

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateResting30DaysPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateResting30DaysPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateResting30DaysPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateResting30DaysPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateResting30DaysPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateResting30DaysPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateResting30DaysPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateResting30DaysPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&HeartRateResting30DaysPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateResting30DaysPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateSettingsPage::HeartRateSettingsPage(DataCenter& data_center) : PageBase(data_center) {
  rows_[0].kind = RowKind::AllDayMonitoring;
  rows_[0].title = "全天心率监测";
  rows_[1].kind = RowKind::HeartHealthMonitoring;
  rows_[1].title = "心脏健康监测";
  rows_[2].kind = RowKind::HighReminder;
  rows_[2].title = "高心率提醒";
  rows_[3].kind = RowKind::LowReminder;
  rows_[3].title = "低心率提醒";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_rows();
                                 }
                               }));
}

PageId HeartRateSettingsPage::id() const {
  return PageId::AppHeartRateSettings;
}

const char* HeartRateSettingsPage::name() const {
  return page_name(PageId::AppHeartRateSettings);
}

void HeartRateSettingsPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_rows();
}

void HeartRateSettingsPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "心率设置", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  for (std::size_t i = 0; i < rows_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
    lv_obj_t* title = row == nullptr
                          ? nullptr
                          : create_steps_label(row, rows_[i].title, cjk_font_20(), 0xF8FAFC, card_w - 32, LV_LABEL_LONG_WRAP);
    lv_obj_t* status = row == nullptr ? nullptr : create_steps_label(row, "关闭", cjk_font_16(), 0xAFC4DA, card_w - 32);
    if (row == nullptr || title == nullptr || status == nullptr) {
      return nullptr;
    }
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 18, 14);
    lv_obj_align(status, LV_ALIGN_TOP_LEFT, 18, 54);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, row_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(rows_[i].kind)));
    rows_[i].row = row;
    rows_[i].status_label = status;
  }

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateSettingsPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateSettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto kind = static_cast<RowKind>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  switch (kind) {
    case RowKind::AllDayMonitoring:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateAllDayMonitoring});
      break;
    case RowKind::HeartHealthMonitoring:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateHeartHealthMonitoring});
      break;
    case RowKind::HighReminder:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateHighReminder});
      break;
    case RowKind::LowReminder:
      self->request_navigation({NavigationAction::Push, PageId::AppHeartRateLowReminder});
      break;
  }
}

void HeartRateSettingsPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateSettingsPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateSettingsPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateSettingsPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateSettingsPage::refresh_rows() {
  for (const auto& row : rows_) {
    if (row.status_label != nullptr) {
      lv_label_set_text(row.status_label, row_status_text(row.kind));
    }
  }
}

const char* HeartRateSettingsPage::row_status_text(RowKind kind) const {
  switch (kind) {
    case RowKind::AllDayMonitoring:
      return heart_rate_all_day_mode_text(current_settings_.all_day_heart_rate_monitoring_mode);
    case RowKind::HeartHealthMonitoring:
      return current_settings_.heart_health_monitoring_enabled ? "开启" : "关闭";
    case RowKind::HighReminder:
      return heart_rate_high_reminder_text(current_settings_.high_heart_rate_reminder_mode);
    case RowKind::LowReminder:
      return heart_rate_low_reminder_text(current_settings_.low_heart_rate_reminder_mode);
  }
  return "关闭";
}

void HeartRateSettingsPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateSettingsPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateSettingsPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateAllDayMonitoringPage::HeartRateAllDayMonitoringPage(DataCenter& data_center) : PageBase(data_center) {
  options_[0].mode = HeartRateAllDayMonitoringMode::Off;
  options_[0].label = "关闭";
  options_[1].mode = HeartRateAllDayMonitoringMode::Smart;
  options_[1].label = "智能监测";
  options_[2].mode = HeartRateAllDayMonitoringMode::Every1Min;
  options_[2].label = "1分钟一次";
  options_[3].mode = HeartRateAllDayMonitoringMode::Every10Min;
  options_[3].label = "10分钟一次";
  options_[4].mode = HeartRateAllDayMonitoringMode::Every30Min;
  options_[4].label = "30分钟一次";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_options();
                                 }
                               }));
}

PageId HeartRateAllDayMonitoringPage::id() const {
  return PageId::AppHeartRateAllDayMonitoring;
}

const char* HeartRateAllDayMonitoringPage::name() const {
  return page_name(PageId::AppHeartRateAllDayMonitoring);
}

void HeartRateAllDayMonitoringPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_options();
}

void HeartRateAllDayMonitoringPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateAllDayMonitoringPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "全天心率监测", cjk_font_20(), 0xF8FAFC, 142);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* intro = create_steps_label(scroll_root_, "按照设定频率自动测量，记录全天心率", cjk_font_16(), 0xEAF2FF, card_w - 8, LV_LABEL_LONG_WRAP);
  if (intro == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(intro, 8, 0);

  for (std::size_t i = 0; i < options_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 78, 0x102033);
    lv_obj_t* label = row == nullptr ? nullptr : create_steps_label(row, options_[i].label, cjk_font_20(), 0xF8FAFC, 130);
    lv_obj_t* dot = row == nullptr ? nullptr : lv_obj_create(row);
    if (row == nullptr || label == nullptr || dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 24, 24);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x5D728E), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x1A2D44), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -18, 0);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, option_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    options_[i].check_dot = dot;
  }

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateAllDayMonitoringPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAllDayMonitoringPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateAllDayMonitoringPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateAllDayMonitoringPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }
  self->data_center_.set_all_day_heart_rate_monitoring_mode(self->options_[index].mode);
}

void HeartRateAllDayMonitoringPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateAllDayMonitoringPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateAllDayMonitoringPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateAllDayMonitoringPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateAllDayMonitoringPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateAllDayMonitoringPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateAllDayMonitoringPage::refresh_options() {
  for (const auto& option : options_) {
    if (option.check_dot == nullptr) {
      continue;
    }
    const bool selected = option.mode == current_settings_.all_day_heart_rate_monitoring_mode;
    lv_obj_set_style_border_color(option.check_dot, lv_color_hex(selected ? 0x14B8FF : 0x5D728E), 0);
    lv_obj_set_style_bg_color(option.check_dot, lv_color_hex(selected ? 0xEAF6FF : 0x1A2D44), 0);
  }
}

void HeartRateAllDayMonitoringPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&HeartRateAllDayMonitoringPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateAllDayMonitoringPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateHeartHealthMonitoringPage::HeartRateHeartHealthMonitoringPage(DataCenter& data_center) : PageBase(data_center) {
  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_switch();
                                 }
                               }));
}

PageId HeartRateHeartHealthMonitoringPage::id() const {
  return PageId::AppHeartRateHeartHealthMonitoring;
}

const char* HeartRateHeartHealthMonitoringPage::name() const {
  return page_name(PageId::AppHeartRateHeartHealthMonitoring);
}

void HeartRateHeartHealthMonitoringPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_switch();
}

void HeartRateHeartHealthMonitoringPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateHeartHealthMonitoringPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "心脏健康监测", cjk_font_20(), 0xF8FAFC, 142);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* switch_card = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
  lv_obj_t* switch_title = switch_card == nullptr
                               ? nullptr
                               : create_steps_label(switch_card, "心脏健康监测", cjk_font_20(), 0xF8FAFC, card_w - 112, LV_LABEL_LONG_WRAP);
  switch_track_ = switch_card == nullptr ? nullptr : create_sleep_switch_track(switch_card);
  lv_obj_t* info_label = create_steps_label(scroll_root_,
                                            "设备在非活跃状态持续监测脉搏节律，评估心脏健康，若产生异常心搏记录，请留意健康状况，但若感觉不适请及时向医疗机构咨询。",
                                            cjk_font_16(),
                                            0xD8E9FF,
                                            card_w - 8,
                                            LV_LABEL_LONG_WRAP);
  if (switch_card == nullptr || switch_title == nullptr || switch_track_ == nullptr || info_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(switch_title, LV_ALIGN_LEFT_MID, 18, 0);
  lv_obj_align(switch_track_, LV_ALIGN_RIGHT_MID, -18, 0);
  attach_click_guard(switch_card);
  lv_obj_add_event_cb(switch_card, switch_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_set_style_text_line_space(info_label, 8, 0);

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateHeartHealthMonitoringPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateHeartHealthMonitoringPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateHeartHealthMonitoringPage::switch_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateHeartHealthMonitoringPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->data_center_.set_heart_health_monitoring_enabled(!self->current_settings_.heart_health_monitoring_enabled);
}

void HeartRateHeartHealthMonitoringPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateHeartHealthMonitoringPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateHeartHealthMonitoringPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateHeartHealthMonitoringPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateHeartHealthMonitoringPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateHeartHealthMonitoringPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateHeartHealthMonitoringPage::refresh_switch() {
  apply_sleep_switch_style(switch_track_, current_settings_.heart_health_monitoring_enabled);
}

void HeartRateHeartHealthMonitoringPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateHeartHealthMonitoringPage::crown_release_timer_cb,
                                         kLauncherCrownReleaseDelayMs,
                                         this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateHeartHealthMonitoringPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateHighReminderPage::HeartRateHighReminderPage(DataCenter& data_center) : PageBase(data_center) {
  options_[0].mode = HeartRateHighReminderMode::Off;
  options_[0].label = "关闭";
  options_[1].mode = HeartRateHighReminderMode::Bpm100;
  options_[1].label = "100次/分";
  options_[2].mode = HeartRateHighReminderMode::Bpm110;
  options_[2].label = "110次/分";
  options_[3].mode = HeartRateHighReminderMode::Bpm120;
  options_[3].label = "120次/分";
  options_[4].mode = HeartRateHighReminderMode::Bpm130;
  options_[4].label = "130次/分";
  options_[5].mode = HeartRateHighReminderMode::Bpm140;
  options_[5].label = "140次/分";
  options_[6].mode = HeartRateHighReminderMode::Bpm150;
  options_[6].label = "150次/分";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_options();
                                 }
                               }));
}

PageId HeartRateHighReminderPage::id() const {
  return PageId::AppHeartRateHighReminder;
}

const char* HeartRateHighReminderPage::name() const {
  return page_name(PageId::AppHeartRateHighReminder);
}

void HeartRateHighReminderPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_options();
}

void HeartRateHighReminderPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateHighReminderPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "高心率提醒", cjk_font_20(), 0xF8FAFC, 122);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* intro = create_steps_label(scroll_root_,
                                       "开启后，监测到安静状态下心率高于设置的心率值时进行提醒。",
                                       cjk_font_16(),
                                       0xEAF2FF,
                                       card_w - 8,
                                       LV_LABEL_LONG_WRAP);
  if (intro == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(intro, 8, 0);

  for (std::size_t i = 0; i < options_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 78, 0x102033);
    lv_obj_t* label = row == nullptr ? nullptr : create_steps_label(row, options_[i].label, cjk_font_20(), 0xF8FAFC, 120);
    lv_obj_t* dot = row == nullptr ? nullptr : lv_obj_create(row);
    if (row == nullptr || label == nullptr || dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 24, 24);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x5D728E), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x1A2D44), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -18, 0);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, option_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    options_[i].check_dot = dot;
  }

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateHighReminderPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateHighReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateHighReminderPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateHighReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }
  self->data_center_.set_high_heart_rate_reminder_mode(self->options_[index].mode);
}

void HeartRateHighReminderPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateHighReminderPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateHighReminderPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateHighReminderPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateHighReminderPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateHighReminderPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateHighReminderPage::refresh_options() {
  for (const auto& option : options_) {
    if (option.check_dot == nullptr) {
      continue;
    }
    const bool selected = option.mode == current_settings_.high_heart_rate_reminder_mode;
    lv_obj_set_style_border_color(option.check_dot, lv_color_hex(selected ? 0x14B8FF : 0x5D728E), 0);
    lv_obj_set_style_bg_color(option.check_dot, lv_color_hex(selected ? 0xEAF6FF : 0x1A2D44), 0);
  }
}

void HeartRateHighReminderPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateHighReminderPage::crown_release_timer_cb,
                                         kLauncherCrownReleaseDelayMs,
                                         this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateHighReminderPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateLowReminderPage::HeartRateLowReminderPage(DataCenter& data_center) : PageBase(data_center) {
  options_[0].mode = HeartRateLowReminderMode::Off;
  options_[0].label = "关闭";
  options_[1].mode = HeartRateLowReminderMode::Bpm40;
  options_[1].label = "40次/分";
  options_[2].mode = HeartRateLowReminderMode::Bpm45;
  options_[2].label = "45次/分";
  options_[3].mode = HeartRateLowReminderMode::Bpm50;
  options_[3].label = "50次/分";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_options();
                                 }
                               }));
}

PageId HeartRateLowReminderPage::id() const {
  return PageId::AppHeartRateLowReminder;
}

const char* HeartRateLowReminderPage::name() const {
  return page_name(PageId::AppHeartRateLowReminder);
}

void HeartRateLowReminderPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_options();
}

void HeartRateLowReminderPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateLowReminderPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "低心率提醒", cjk_font_20(), 0xF8FAFC, 122);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* intro = create_steps_label(scroll_root_,
                                       "开启后，监测到安静状态下心率低于设置的心率值时进行提醒。",
                                       cjk_font_16(),
                                       0xEAF2FF,
                                       card_w - 8,
                                       LV_LABEL_LONG_WRAP);
  if (intro == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(intro, 8, 0);

  for (std::size_t i = 0; i < options_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 78, 0x102033);
    lv_obj_t* label = row == nullptr ? nullptr : create_steps_label(row, options_[i].label, cjk_font_20(), 0xF8FAFC, 120);
    lv_obj_t* dot = row == nullptr ? nullptr : lv_obj_create(row);
    if (row == nullptr || label == nullptr || dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 24, 24);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x5D728E), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x1A2D44), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -18, 0);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, option_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    options_[i].check_dot = dot;
  }

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateLowReminderPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateLowReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateLowReminderPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateLowReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }
  self->data_center_.set_low_heart_rate_reminder_mode(self->options_[index].mode);
}

void HeartRateLowReminderPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateLowReminderPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateLowReminderPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateLowReminderPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateLowReminderPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void HeartRateLowReminderPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateLowReminderPage::refresh_options() {
  for (const auto& option : options_) {
    if (option.check_dot == nullptr) {
      continue;
    }
    const bool selected = option.mode == current_settings_.low_heart_rate_reminder_mode;
    lv_obj_set_style_border_color(option.check_dot, lv_color_hex(selected ? 0x14B8FF : 0x5D728E), 0);
    lv_obj_set_style_bg_color(option.check_dot, lv_color_hex(selected ? 0xEAF6FF : 0x1A2D44), 0);
  }
}

void HeartRateLowReminderPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateLowReminderPage::crown_release_timer_cb,
                                         kLauncherCrownReleaseDelayMs,
                                         this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateLowReminderPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

HeartRateInfoPage::HeartRateInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateInfoPage::id() const {
  return PageId::AppHeartRateInfo;
}

const char* HeartRateInfoPage::name() const {
  return page_name(PageId::AppHeartRateInfo);
}

void HeartRateInfoPage::on_will_appear() {
  refresh_header_time();
}

void HeartRateInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateInfoPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "心率说明", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* info_text =
      "心率\n\n"
      "心率是心脏每分钟跳动的次数（bpm），它是心血管健康的重要指标。"
      "成年人安静状态下的心率（静息心率）通常为55-80次/分，可能因年龄、性别、健身水平或其他生理因素产生个体差异。\n\n"
      "注意：本设备测量范围为30-240次/分。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "3. 受佩戴状态、皮肤特征、环境等因素影响，可能影响测量的准确性，甚至无法输出结果。\n\n"
      "本品非医疗器械，结果仅供参考，不作为诊断治疗依据。";
  lv_obj_t* note_body =
      create_steps_label(scroll_root_, info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void HeartRateInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void HeartRateInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateInfoPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

BloodOxygenAppPage::BloodOxygenAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenAppPage::id() const {
  return PageId::AppBloodOxygen;
}

const char* BloodOxygenAppPage::name() const {
  return page_name(PageId::AppBloodOxygen);
}

void BloodOxygenAppPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "血氧饱和度", cjk_font_20(), 0xF8FAFC, 120);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 16, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* chart_card = create_steps_panel(scroll_root_, card_w, 124, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 16;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 82;
  for (int i = 0; i < 5; ++i) {
    lv_obj_t* tick = lv_obj_create(chart_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, chart_h);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5A82B2), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(chart_left + (chart_w * i) / 4), chart_top);
  }

  lv_obj_t* top_value = create_steps_label(chart_card, "95", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "85", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* left_label = create_steps_label(chart_card, "00:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* mid_label = create_steps_label(chart_card, "12:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* right_label = create_steps_label(chart_card, "24:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  if (top_value == nullptr || bottom_value == nullptr || left_label == nullptr || mid_label == nullptr ||
      right_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(top_value, LV_ALIGN_TOP_RIGHT, -10, 6);
  lv_obj_align(bottom_value, LV_ALIGN_BOTTOM_RIGHT, -10, -12);
  lv_obj_align(left_label, LV_ALIGN_BOTTOM_LEFT, 14, -12);
  lv_obj_align(mid_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(right_label, LV_ALIGN_BOTTOM_RIGHT, -24, -12);

  lv_obj_t* dot = lv_obj_create(chart_card);
  if (dot == nullptr) {
    return nullptr;
  }
  ui_prepare_box(dot);
  lv_obj_set_size(dot, 8, 8);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, lv_color_hex(0xFF4F72), 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_align(dot, LV_ALIGN_TOP_RIGHT, -34, 18);

  lv_obj_t* value_label =
      create_steps_label(scroll_root_, "95%", &lv_font_montserrat_48, 0xF8FAFC, card_w, LV_LABEL_LONG_CLIP);
  lv_obj_t* update_label = create_steps_label(scroll_root_, "20:37更新", cjk_font_20(), 0xD8E9FF, card_w, LV_LABEL_LONG_CLIP);
  if (value_label == nullptr || update_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(update_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* measure_button = create_steps_panel(scroll_root_, card_w, 74, 0x111D2E);
  lv_obj_t* measure_label = measure_button == nullptr
                                ? nullptr
                                : create_steps_label(measure_button, "开始测量", cjk_font_20(), 0xFF4F72, card_w - 24);
  if (measure_button == nullptr || measure_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(measure_button, 26, 0);
  lv_obj_set_style_bg_color(measure_button, lv_color_hex(0x0D1624), 0);
  lv_obj_set_style_text_align(measure_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(measure_label);

  for (const auto& entry :
       {std::pair<const char*, PageId> {"设置", PageId::AppBloodOxygenSettings},
        std::pair<const char*, PageId> {"说明", PageId::AppBloodOxygenInfo}}) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 86, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    const bool is_settings = entry.second == PageId::AppBloodOxygenSettings;
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             0x4C1732,
                                             0xFF8FB3,
                                             nullptr,
                                             is_settings ? LV_SYMBOL_SETTINGS : "i",
                                             is_settings ? &lv_font_montserrat_18 : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, entry.first, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(entry.second)));
  }

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
}

void BloodOxygenAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&BloodOxygenAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

BloodOxygenSettingsPage::BloodOxygenSettingsPage(DataCenter& data_center) : PageBase(data_center) {
  rows_[0].kind = RowKind::AllDayMonitoring;
  rows_[0].title = "全天血氧监测";
  rows_[1].kind = RowKind::LowOxygenReminder;
  rows_[1].title = "低血氧提醒";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_rows();
                                 }
                               }));
}

PageId BloodOxygenSettingsPage::id() const {
  return PageId::AppBloodOxygenSettings;
}

const char* BloodOxygenSettingsPage::name() const {
  return page_name(PageId::AppBloodOxygenSettings);
}

void BloodOxygenSettingsPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_rows();
}

void BloodOxygenSettingsPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "血氧设置", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* monitoring_row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
  lv_obj_t* monitoring_title = monitoring_row == nullptr
                                   ? nullptr
                                   : create_steps_label(monitoring_row, rows_[0].title, cjk_font_20(), 0xF8FAFC, card_w - 112, LV_LABEL_LONG_WRAP);
  lv_obj_t* monitoring_switch = monitoring_row == nullptr ? nullptr : create_sleep_switch_track(monitoring_row);
  if (monitoring_row == nullptr || monitoring_title == nullptr || monitoring_switch == nullptr) {
    return nullptr;
  }
  lv_obj_align(monitoring_title, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(monitoring_switch, LV_ALIGN_RIGHT_MID, -18, 0);
  attach_click_guard(monitoring_row);
  lv_obj_add_event_cb(monitoring_row, switch_event_cb, LV_EVENT_CLICKED, this);
  rows_[0].row = monitoring_row;
  rows_[0].switch_track = monitoring_switch;

  lv_obj_t* reminder_row = create_steps_panel(scroll_root_, card_w, 92, 0x102033);
  lv_obj_t* reminder_title = reminder_row == nullptr
                                 ? nullptr
                                 : create_steps_label(reminder_row, rows_[1].title, cjk_font_20(), 0xF8FAFC, card_w - 32, LV_LABEL_LONG_WRAP);
  lv_obj_t* reminder_status = reminder_row == nullptr ? nullptr : create_steps_label(reminder_row, "不提醒", cjk_font_16(), 0xAFC4DA, card_w - 32);
  if (reminder_row == nullptr || reminder_title == nullptr || reminder_status == nullptr) {
    return nullptr;
  }
  lv_obj_align(reminder_title, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(reminder_status, LV_ALIGN_TOP_LEFT, 18, 56);
  attach_click_guard(reminder_row);
  lv_obj_add_event_cb(reminder_row, row_event_cb, LV_EVENT_CLICKED, this);
  rows_[1].row = reminder_row;
  rows_[1].status_label = reminder_status;

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenSettingsPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenSettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::AppBloodOxygenLowOxygenReminder});
}

void BloodOxygenSettingsPage::switch_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->data_center_.set_all_day_blood_oxygen_enabled(!self->current_settings_.all_day_blood_oxygen_enabled);
}

void BloodOxygenSettingsPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenSettingsPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenSettingsPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void BloodOxygenSettingsPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenSettingsPage::refresh_rows() {
  if (rows_[0].switch_track != nullptr) {
    apply_sleep_switch_style(rows_[0].switch_track, current_settings_.all_day_blood_oxygen_enabled);
  }
  if (rows_[1].row != nullptr) {
    if (current_settings_.all_day_blood_oxygen_enabled) {
      lv_obj_remove_flag(rows_[1].row, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_add_flag(rows_[1].row, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (rows_[1].status_label != nullptr) {
    lv_label_set_text(rows_[1].status_label, reminder_status_text());
  }
}

const char* BloodOxygenSettingsPage::reminder_status_text() const {
  return low_blood_oxygen_mode_text(current_settings_.low_blood_oxygen_reminder_mode);
}

void BloodOxygenSettingsPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&BloodOxygenSettingsPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenSettingsPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

BloodOxygenLowOxygenReminderPage::BloodOxygenLowOxygenReminderPage(DataCenter& data_center) : PageBase(data_center) {
  options_[0].mode = LowBloodOxygenReminderMode::Off;
  options_[0].label = "不提醒";
  options_[1].mode = LowBloodOxygenReminderMode::Threshold90;
  options_[1].label = "90%";
  options_[2].mode = LowBloodOxygenReminderMode::Threshold85;
  options_[2].label = "85%";
  options_[3].mode = LowBloodOxygenReminderMode::Threshold80;
  options_[3].label = "80%";

  track(data_center_.subscribe(EventId::HealthMonitoringSettingsChanged,
                               [this](const Event& event) {
                                 if (const auto* model =
                                         std::get_if<HealthMonitoringSettingsModel>(&event.payload)) {
                                   apply_settings(*model);
                                   refresh_options();
                                 }
                               }));
}

PageId BloodOxygenLowOxygenReminderPage::id() const {
  return PageId::AppBloodOxygenLowOxygenReminder;
}

const char* BloodOxygenLowOxygenReminderPage::name() const {
  return page_name(PageId::AppBloodOxygenLowOxygenReminder);
}

void BloodOxygenLowOxygenReminderPage::on_will_appear() {
  if (const auto& model = data_center_.health_monitoring_settings(); model.has_value()) {
    apply_settings(*model);
  } else {
    apply_settings(HealthMonitoringSettingsModel {});
  }
  refresh_header_time();
  refresh_options();
}

void BloodOxygenLowOxygenReminderPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenLowOxygenReminderPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "低血氧提醒", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* intro_text =
      "开启后，在非睡眠状态下，检测到血氧饱和度在一定时间内低于设定值时设备上出现提醒通知。";
  lv_obj_t* intro_label =
      create_steps_label(scroll_root_, intro_text, cjk_font_16(), 0xEAF2FF, card_w - 8, LV_LABEL_LONG_WRAP);
  if (intro_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(intro_label, 8, 0);
  lv_obj_align(intro_label, LV_ALIGN_TOP_LEFT, 4, 0);

  for (std::size_t i = 0; i < options_.size(); ++i) {
    lv_obj_t* row = create_steps_panel(scroll_root_, card_w, 78, 0x102033);
    lv_obj_t* label = row == nullptr ? nullptr : create_steps_label(row, options_[i].label, cjk_font_20(), 0xF8FAFC, 72);
    lv_obj_t* dot = row == nullptr ? nullptr : lv_obj_create(row);
    if (row == nullptr || label == nullptr || dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 24, 24);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_border_color(dot, lv_color_hex(0x5D728E), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x1A2D44), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -18, 0);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, option_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    options_[i].row = row;
    options_[i].check_dot = dot;
  }

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenLowOxygenReminderPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenLowOxygenReminderPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }
  self->data_center_.set_low_blood_oxygen_reminder_mode(self->options_[index].mode);
}

void BloodOxygenLowOxygenReminderPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenLowOxygenReminderPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenLowOxygenReminderPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenLowOxygenReminderPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenLowOxygenReminderPage::apply_settings(const HealthMonitoringSettingsModel& model) {
  current_settings_ = model;
}

void BloodOxygenLowOxygenReminderPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenLowOxygenReminderPage::refresh_options() {
  for (const auto& option : options_) {
    if (option.check_dot == nullptr) {
      continue;
    }
    const bool selected = option.mode == current_settings_.low_blood_oxygen_reminder_mode;
    lv_obj_set_style_border_color(option.check_dot, lv_color_hex(selected ? 0x14B8FF : 0x5D728E), 0);
    lv_obj_set_style_bg_color(option.check_dot, lv_color_hex(selected ? 0xEAF6FF : 0x1A2D44), 0);
  }
}

void BloodOxygenLowOxygenReminderPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&BloodOxygenLowOxygenReminderPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenLowOxygenReminderPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

BloodOxygenInfoPage::BloodOxygenInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenInfoPage::id() const {
  return PageId::AppBloodOxygenInfo;
}

const char* BloodOxygenInfoPage::name() const {
  return page_name(PageId::AppBloodOxygenInfo);
}

void BloodOxygenInfoPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenInfoPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "血氧说明", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* info_text =
      "说明\n\n"
      "1. 血氧饱和度是血液中氧合血红蛋白（HbO2）容量占全部可结合血红蛋白（Hb）的百分比，"
      "它是人体呼吸循环的重要生理参数。\n\n"
      "2. 设备血氧饱和度参考值如下：\n"
      "日常活动：一般 ≥95%\n"
      "夜间睡眠：一般 ≥90%\n"
      "需要关注：<90%\n"
      "注意：本设备测量范围为80%-100%。\n\n"
      "3. 佩戴状态、皮肤特征、环境等因素都可能影响测试准确性，甚至导致测量失败。"
      "睡眠过程中，设备可能因睡眠姿势不同而倾斜，导致伪低血氧值出现。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "本品非医疗器械，结果仅供参考，不作为诊断治疗依据。";
  lv_obj_t* note_body =
      create_steps_label(scroll_root_, info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&BloodOxygenInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenInfoPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

void LauncherPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  self->request_navigation({NavigationAction::ReturnHome, PageId::Watchface});
}

void LauncherPage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto item_index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (item_index >= self->items_.size()) {
    return;
  }

  self->request_navigation(self->items_[item_index].command);
}

void LauncherPage::apply_crown_drag(bool forward, std::int16_t detents) {
  if (list_root_ == nullptr) {
    return;
  }

  stop_crown_release_timer();
  const std::int32_t step = kLauncherCrownDragStep * std::max<std::int16_t>(1, detents);
  const std::int32_t elastic_limit = std::max<std::int32_t>(24, lv_obj_get_height(list_root_) / 5);
  const std::int32_t current_y = lv_obj_get_scroll_y(list_root_);
  const std::int32_t scroll_top = lv_obj_get_scroll_top(list_root_);
  const std::int32_t scroll_bottom = lv_obj_get_scroll_bottom(list_root_);
  const std::int32_t scroll_max = std::max<std::int32_t>(0, scroll_top + scroll_bottom);

  std::int32_t target_y = current_y + (forward ? step : -step);
  target_y = std::clamp(target_y, -elastic_limit, scroll_max + elastic_limit);

  lv_obj_scroll_to_y(list_root_, target_y, LV_ANIM_OFF);
  schedule_crown_release();
}

void LauncherPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&LauncherPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void LauncherPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

void LauncherPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<LauncherPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr || self->list_root_ == nullptr) {
    return;
  }

  self->crown_release_timer_ = nullptr;
  const std::int32_t current_y = lv_obj_get_scroll_y(self->list_root_);
  const std::int32_t scroll_max =
      std::max<std::int32_t>(0, lv_obj_get_scroll_top(self->list_root_) + lv_obj_get_scroll_bottom(self->list_root_));
  const std::int32_t clamped_y = std::clamp(current_y, 0, scroll_max);
  lv_obj_scroll_to_y(self->list_root_, clamped_y, LV_ANIM_ON);
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
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

lv_color_t notification_card_color(NotificationCategory category) {
  return category == NotificationCategory::BatteryLow ? lv_color_hex(0x172636) : lv_color_hex(0x132033);
}

lv_color_t notification_accent_color(NotificationCategory category) {
  return category == NotificationCategory::BatteryLow ? lv_color_hex(0xFACC15) : lv_color_hex(0x22D3EE);
}

lv_color_t notification_read_card_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0x0E1824) : notification_card_color(item.category);
}

lv_color_t notification_primary_text_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0xD3DCE8) : lv_color_hex(0xF8FAFC);
}

lv_color_t notification_secondary_text_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0x94A3B8) : lv_color_hex(0xE2E8F0);
}

lv_obj_t* create_notification_icon(lv_obj_t* parent, const NotificationItem& item, bool compact) {
  lv_obj_t* holder = lv_obj_create(parent);
  if (holder == nullptr) {
    return nullptr;
  }

  const lv_coord_t size = compact ? 38 : 48;
  lv_obj_remove_flag(holder, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(holder, size, size);
  lv_obj_set_style_border_width(holder, 0, 0);
  lv_obj_set_style_radius(holder, compact ? 14 : 18, 0);
  lv_obj_set_style_pad_all(holder, 0, 0);

  if (item.category == NotificationCategory::Message && file_exists(payment_wechat_green_asset_path())) {
    lv_obj_set_style_bg_color(holder, lv_color_hex(0x19C37D), 0);
    lv_obj_set_style_bg_opa(holder, LV_OPA_COVER, 0);
    lv_obj_t* image = lv_image_create(holder);
    if (image == nullptr) {
      return holder;
    }
    lv_image_set_src(image, payment_wechat_green_asset_path());
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_STRETCH);
    lv_obj_set_size(image, compact ? 24 : 30, compact ? 24 : 30);
    lv_obj_center(image);
    return holder;
  }

  lv_obj_set_style_bg_color(holder,
                            item.category == NotificationCategory::BatteryLow ? lv_color_hex(0x8A6A00)
                                                                              : lv_color_hex(0x1D4ED8),
                            0);
  lv_obj_set_style_bg_opa(holder, LV_OPA_COVER, 0);

  lv_obj_t* label = lv_label_create(holder);
  if (label == nullptr) {
    return holder;
  }
  lv_obj_set_style_text_font(label, compact ? &lv_font_montserrat_14 : &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(label,
                    item.category == NotificationCategory::BatteryLow
                        ? (item.badge_text.empty() ? LV_SYMBOL_CHARGE : item.badge_text.c_str())
                        : LV_SYMBOL_BELL);
  lv_obj_center(label);
  return holder;
}

NotificationsPage::NotificationsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId NotificationsPage::id() const {
  return PageId::Notifications;
}

const char* NotificationsPage::name() const {
  return "Notifications";
}

void NotificationsPage::on_will_appear() {
  stop_preview_close_timer();
  shell_drag_offset_ = 0;
  open_preview_progress_ = 0;
  shell_drag_active_ = false;
  reset_notification_card_swipe_state();
  hide_clear_confirm_overlay();
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kNotificationsSheetY);
  }
  refresh_backdrop();
  refresh_content();
}

void NotificationsPage::on_will_disappear() {
  stop_preview_close_timer();
  reset_notification_card_swipe_state();
  detail_active_ = false;
  detail_notification_id_.clear();
  hide_clear_confirm_overlay();
}

lv_obj_t* NotificationsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x040B14);
  lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

  backdrop_root_ = lv_obj_create(root);
  sheet_container_ = lv_obj_create(root);
  if (backdrop_root_ == nullptr || sheet_container_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(backdrop_root_);
  lv_obj_set_size(backdrop_root_, 240, 296);
  lv_obj_align(backdrop_root_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(backdrop_root_, lv_color_hex(0x05080F), 0);
  lv_obj_set_style_bg_opa(backdrop_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(backdrop_root_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_root_, 0, 0);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* battery_row = lv_obj_create(backdrop_root_);
  backdrop_battery_icon_label_ = lv_label_create(battery_row);
  backdrop_battery_label_ = lv_label_create(battery_row);
  backdrop_style_stage_ = lv_obj_create(backdrop_root_);
  backdrop_minute_label_ = lv_label_create(backdrop_root_);
  if (battery_row == nullptr || backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr ||
      backdrop_style_stage_ == nullptr || backdrop_minute_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(backdrop_battery_icon_label_);
  ui_apply_text(backdrop_battery_icon_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_icon_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);

  ui_prepare_label(backdrop_battery_label_);
  ui_apply_text(backdrop_battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_label_, "--%");

  ui_prepare_box(backdrop_style_stage_);
  lv_obj_set_size(backdrop_style_stage_, 240, 296);
  lv_obj_align(backdrop_style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(backdrop_style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(backdrop_style_stage_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_style_stage_, 0, 0);

  backdrop_renderer_ = create_watchface_style_renderer(backdrop_config_);
  if (!backdrop_renderer_ || backdrop_renderer_->build(backdrop_style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(backdrop_minute_label_);
  ui_apply_text(backdrop_minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(backdrop_minute_label_, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(backdrop_minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(backdrop_minute_label_, 96);
  lv_obj_set_style_text_align(backdrop_minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(backdrop_minute_label_, 76, 234);
  lv_label_set_text(backdrop_minute_label_, "--");

  lv_obj_remove_flag(sheet_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(sheet_container_, kNotificationsSheetWidth, kNotificationsSheetHeight);
  lv_obj_align(sheet_container_, LV_ALIGN_TOP_MID, 0, kNotificationsSheetY);
  lv_obj_set_style_bg_color(sheet_container_, lv_color_hex(0x040B14), 0);
  lv_obj_set_style_bg_opa(sheet_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sheet_container_, 0, 0);
  lv_obj_set_style_radius(sheet_container_, 28, 0);
  lv_obj_set_style_pad_all(sheet_container_, 0, 0);
  lv_obj_set_style_shadow_width(sheet_container_, 28, 0);
  lv_obj_set_style_shadow_color(sheet_container_, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(sheet_container_, LV_OPA_40, 0);
  lv_obj_set_style_clip_corner(sheet_container_, true, 0);

  clear_button_ = lv_button_create(sheet_container_);
  list_root_ = lv_obj_create(sheet_container_);
  empty_state_ = lv_obj_create(sheet_container_);
  detail_root_ = lv_obj_create(sheet_container_);
  drag_handle_ = lv_obj_create(sheet_container_);
  clear_confirm_overlay_ = lv_obj_create(root);
  if (clear_button_ == nullptr || list_root_ == nullptr || empty_state_ == nullptr || detail_root_ == nullptr ||
      clear_confirm_overlay_ == nullptr ||
      drag_handle_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(clear_button_, 208, 46);
  lv_obj_align(clear_button_, LV_ALIGN_TOP_MID, 0, 18);
  lv_obj_set_style_bg_color(clear_button_, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(clear_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(clear_button_, 0, 0);
  lv_obj_set_style_radius(clear_button_, 22, 0);
  lv_obj_add_event_cb(clear_button_, &NotificationsPage::clear_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* clear_label = lv_label_create(clear_button_);
  if (clear_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(clear_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(clear_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(clear_label, kTextClear);
  lv_obj_center(clear_label);

  lv_obj_set_size(list_root_, 208, 156);
  lv_obj_align(list_root_, LV_ALIGN_TOP_MID, 0, 76);
  lv_obj_set_flex_flow(list_root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(list_root_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(list_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(list_root_, 0, 0);
  lv_obj_set_style_pad_row(list_root_, 10, 0);
  lv_obj_set_style_bg_opa(list_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list_root_, 0, 0);
  lv_obj_set_style_radius(list_root_, 0, 0);

  lv_obj_remove_flag(empty_state_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(empty_state_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(empty_state_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(empty_state_, 0, 0);
  lv_obj_set_style_pad_all(empty_state_, 0, 0);

  lv_obj_t* bubble = lv_obj_create(empty_state_);
  if (bubble == nullptr) {
    return nullptr;
  }
  lv_obj_remove_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(bubble, 68, 68);
  lv_obj_align(bubble, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_bg_color(bubble, lv_color_hex(0x1D4ED8), 0);
  lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(bubble, 0, 0);
  lv_obj_set_style_radius(bubble, LV_RADIUS_CIRCLE, 0);

  lv_obj_t* bubble_label = lv_label_create(bubble);
  lv_obj_t* empty_text = lv_label_create(empty_state_);
  if (bubble_label == nullptr || empty_text == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(bubble_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(bubble_label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(bubble_label, LV_SYMBOL_BELL);
  lv_obj_center(bubble_label);

  lv_obj_set_style_text_font(empty_text, cjk_font_16(), 0);
  lv_obj_set_style_text_color(empty_text, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(empty_text, kTextNoMessages);
  lv_obj_align(empty_text, LV_ALIGN_CENTER, 0, 34);

  lv_obj_remove_flag(detail_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(detail_root_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(detail_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(detail_root_, 0, 0);
  lv_obj_set_style_pad_all(detail_root_, 0, 0);
  lv_obj_add_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  detail_back_button_ = lv_button_create(detail_root_);
  lv_obj_t* detail_card = lv_obj_create(detail_root_);
  if (detail_back_button_ == nullptr || detail_card == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(detail_back_button_, 48, 34);
  lv_obj_align(detail_back_button_, LV_ALIGN_TOP_LEFT, 14, 16);
  lv_obj_set_style_bg_color(detail_back_button_, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(detail_back_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(detail_back_button_, 0, 0);
  lv_obj_set_style_radius(detail_back_button_, 16, 0);
  lv_obj_add_event_cb(detail_back_button_, &NotificationsPage::detail_back_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* detail_back_label = lv_label_create(detail_back_button_);
  if (detail_back_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(detail_back_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(detail_back_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(detail_back_label, LV_SYMBOL_LEFT);
  lv_obj_center(detail_back_label);

  lv_obj_remove_flag(detail_card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(detail_card, 208, 206);
  lv_obj_align(detail_card, LV_ALIGN_TOP_MID, 0, 58);
  lv_obj_set_style_bg_color(detail_card, lv_color_hex(0x132033), 0);
  lv_obj_set_style_bg_opa(detail_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(detail_card, 0, 0);
  lv_obj_set_style_radius(detail_card, 22, 0);
  lv_obj_set_style_pad_all(detail_card, 14, 0);

  detail_source_label_ = lv_label_create(detail_card);
  detail_title_label_ = lv_label_create(detail_card);
  detail_body_label_ = lv_label_create(detail_card);
  detail_time_label_ = lv_label_create(detail_card);
  if (detail_source_label_ == nullptr || detail_title_label_ == nullptr || detail_body_label_ == nullptr ||
      detail_time_label_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_style_text_font(detail_source_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_source_label_, lv_color_hex(0x67E8F9), 0);
  lv_obj_align(detail_source_label_, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_set_width(detail_title_label_, 178);
  lv_label_set_long_mode(detail_title_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail_title_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(detail_title_label_, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(detail_title_label_, LV_ALIGN_TOP_LEFT, 0, 26);

  lv_obj_set_width(detail_body_label_, 178);
  lv_label_set_long_mode(detail_body_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail_body_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_body_label_, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(detail_body_label_, LV_ALIGN_TOP_LEFT, 0, 78);

  lv_obj_set_style_text_font(detail_time_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_time_label_, lv_color_hex(0xCBD5E1), 0);
  lv_obj_align(detail_time_label_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  ui_prepare_box(clear_confirm_overlay_);
  lv_obj_set_size(clear_confirm_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_align(clear_confirm_overlay_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(clear_confirm_overlay_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(clear_confirm_overlay_, LV_OPA_90, 0);
  lv_obj_set_style_border_width(clear_confirm_overlay_, 0, 0);
  lv_obj_set_style_radius(clear_confirm_overlay_, 0, 0);
  lv_obj_add_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(clear_confirm_overlay_);

  lv_obj_t* confirm_body = lv_label_create(clear_confirm_overlay_);
  lv_obj_t* cancel_button = lv_button_create(clear_confirm_overlay_);
  lv_obj_t* confirm_button = lv_button_create(clear_confirm_overlay_);
  if (confirm_body == nullptr || cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }

  lv_obj_set_width(confirm_body, 176);
  lv_label_set_long_mode(confirm_body, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(confirm_body, cjk_font_16(), 0);
  lv_obj_set_style_text_color(confirm_body, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(confirm_body, kTextNotificationClearConfirmBody);
  lv_obj_set_style_text_align(confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(confirm_body, LV_ALIGN_CENTER, 0, -28);

  for (const auto [button, text, accent] : {std::tuple {cancel_button, kTextCancel, lv_color_hex(0x334155)},
                                            std::tuple {confirm_button, kTextConfirm, lv_color_hex(0x2563EB)}}) {
    lv_obj_set_size(button, 82, 44);
    lv_obj_set_style_bg_color(button, accent, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 20, 0);
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_text_font(label, cjk_font_16(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF8FAFC), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }
  lv_obj_align(cancel_button, LV_ALIGN_CENTER, -48, 64);
  lv_obj_align(confirm_button, LV_ALIGN_CENTER, 48, 64);
  lv_obj_add_event_cb(cancel_button, &NotificationsPage::clear_cancel_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_add_event_cb(confirm_button, &NotificationsPage::clear_confirm_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(drag_handle_, 46, 7);
  lv_obj_align(drag_handle_, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_set_style_bg_color(drag_handle_, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_bg_opa(drag_handle_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(drag_handle_, 0, 0);
  lv_obj_set_style_radius(drag_handle_, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_shadow_width(drag_handle_, 16, 0);
  lv_obj_set_style_shadow_color(drag_handle_, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_shadow_opa(drag_handle_, static_cast<lv_opa_t>(140), 0);

  bind_input();
  bind_notifications();
  bind_backdrop();
  refresh_backdrop();
  refresh_content();
  return root;
}

void NotificationsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationsPage::clear_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->show_clear_confirm_overlay();
}

void NotificationsPage::clear_confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->hide_clear_confirm_overlay();
  self->data_center_.clear_notifications();
}

void NotificationsPage::clear_cancel_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->hide_clear_confirm_overlay();
}

void NotificationsPage::detail_back_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->close_detail();
}

void NotificationsPage::notification_card_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  auto* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || self->list_root_ == nullptr) {
    return;
  }
  if (self->suppressed_card_click_target_ == target) {
    self->suppressed_card_click_target_ = nullptr;
    return;
  }
  if (!click_guard_allows(target)) {
    return;
  }
  const std::uint32_t index = static_cast<std::uint32_t>(lv_obj_get_index(target));
  if (index >= self->rendered_notification_ids_.size()) {
    return;
  }
  self->open_detail_for(self->rendered_notification_ids_[index]);
}

void NotificationsPage::notification_card_swipe_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  auto* target = lv_event_get_current_target_obj(event);
  if (self == nullptr || target == nullptr || self->list_root_ == nullptr || self->detail_active_ ||
      self->clear_confirm_active_) {
    return;
  }

  const auto code = lv_event_get_code(event);
  if (code == LV_EVENT_PRESSED) {
    const auto index = static_cast<std::uint32_t>(lv_obj_get_index(target));
    if (index >= self->rendered_notification_ids_.size()) {
      self->reset_notification_card_swipe_state();
      return;
    }
    self->reset_notification_card_swipe_state();
    self->active_card_swipe_ = target;
    self->active_card_swipe_notification_id_ = self->rendered_notification_ids_[index];
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &self->active_card_swipe_press_point_);
    }
    return;
  }

  if (target != self->active_card_swipe_) {
    return;
  }

  if (code == LV_EVENT_PRESSING) {
    lv_point_t point = self->active_card_swipe_press_point_;
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &point);
    }
    const lv_coord_t dx = point.x - self->active_card_swipe_press_point_.x;
    const lv_coord_t dy = point.y - self->active_card_swipe_press_point_.y;

    if (!self->card_swipe_horizontal_capture_) {
      if (LV_ABS(dy) >= kNotificationCardSwipeStartThreshold &&
          LV_ABS(dy) > LV_ABS(dx) + 6) {
        self->card_swipe_vertical_lock_ = true;
        return;
      }
      if (self->card_swipe_vertical_lock_ || dx <= 0) {
        return;
      }
      if (dx < kNotificationCardSwipeStartThreshold || dx <= LV_ABS(dy) + 6) {
        return;
      }

      self->card_swipe_horizontal_capture_ = true;
      lv_obj_clear_flag(self->list_root_, LV_OBJ_FLAG_SCROLLABLE);
    }

    self->set_notification_card_swipe_offset(
        target, clamp_coord(dx, 0, kNotificationCardSwipeMaxOffset));
    return;
  }

  if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
    const bool should_dismiss =
        self->card_swipe_horizontal_capture_ &&
        self->active_card_swipe_offset_ >= kNotificationCardSwipeDismissThreshold &&
        !self->active_card_swipe_notification_id_.empty();
    const std::string dismiss_id = self->active_card_swipe_notification_id_;
    lv_obj_t* card = self->active_card_swipe_;
    const bool had_horizontal_capture = self->card_swipe_horizontal_capture_;
    const lv_coord_t release_offset = self->active_card_swipe_offset_;

    if (self->list_root_ != nullptr) {
      lv_obj_add_flag(self->list_root_, LV_OBJ_FLAG_SCROLLABLE);
    }
    self->active_card_swipe_ = nullptr;
    self->active_card_swipe_press_point_ = {};
    self->active_card_swipe_offset_ = 0;
    self->active_card_swipe_notification_id_.clear();
    self->card_swipe_horizontal_capture_ = false;
    self->card_swipe_vertical_lock_ = false;

    if (should_dismiss) {
      self->suppressed_card_click_target_ = card;
      self->data_center_.dismiss_notification(dismiss_id);
      return;
    }

    if (had_horizontal_capture && card != nullptr) {
      self->suppressed_card_click_target_ = card;
      lv_obj_set_style_translate_x(card, release_offset, 0);
      self->animate_notification_card_swipe_offset(card, 0);
    }
  }
}

void NotificationsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 if (clear_confirm_active_) {
                                   return;
                                 }

                                 const bool list_hidden =
                                     list_root_ == nullptr || lv_obj_has_flag(list_root_, LV_OBJ_FLAG_HIDDEN);

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     if (!list_hidden) {
                                       lv_obj_scroll_by(
                                           list_root_, 0, 44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (!list_hidden) {
                                       lv_obj_scroll_by(
                                           list_root_, 0, -44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     break;
                                    case InputAction::ScrollDrag:
                                      if (should_capture_shell_drag(*command)) {
                                        set_close_drag_offset(
                                            clamp_coord(static_cast<lv_coord_t>(-command->value),
                                                        0,
                                                        kNotificationsMaxDragOffset),
                                            false);
                                      }
                                      break;
                                   case InputAction::ScrollFlick:
                                      if (should_capture_shell_drag(*command)) {
                                        finish_drag_close(command->value, true);
                                      }
                                      break;
                                   case InputAction::ScrollRelease:
                                      if (shell_drag_active_ || shell_drag_offset_ > 0 || should_capture_shell_drag(*command)) {
                                        finish_drag_close(command->value, false);
                                      }
                                      break;
                                    default:
                                      break;
                                  }
                               }));
}

void NotificationsPage::set_open_preview_progress(lv_coord_t progress, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  open_preview_progress_ = clamp_coord(progress, 0, kNotificationsMaxDragOffset);
  const lv_coord_t hidden_y = static_cast<lv_coord_t>(-kNotificationsSheetHeight + 28);
  const lv_coord_t target_y =
      static_cast<lv_coord_t>(hidden_y + ((kNotificationsSheetY - hidden_y) * open_preview_progress_) / kNotificationsMaxDragOffset);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, sheet_container_);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&anim, 220);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::set_close_drag_offset(lv_coord_t offset, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  shell_drag_offset_ = clamp_coord(offset, 0, kNotificationsMaxDragOffset);
  const lv_coord_t target_y = static_cast<lv_coord_t>(kNotificationsSheetY - shell_drag_offset_);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, sheet_container_);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&anim, 220);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::stop_preview_close_timer() {
  if (preview_close_timer_ != nullptr) {
    lv_timer_del(preview_close_timer_);
    preview_close_timer_ = nullptr;
  }
}

void NotificationsPage::preview_close_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<NotificationsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->preview_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationsPage::finish_drag_close(std::int16_t release_delta, bool flick_close) {
  if (root_ == nullptr) {
    return;
  }

  shell_drag_active_ = false;
  const bool should_close =
      shell_drag_offset_ >= kNotificationsCloseDragThreshold ||
      (flick_close && release_delta <= -kNotificationsCloseFlickThreshold);
  if (should_close) {
    shell_drag_offset_ = 0;
    request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
    return;
  }

  shell_drag_offset_ = 0;
  set_close_drag_offset(0, true);
}

bool NotificationsPage::is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const {
  if (drag_handle_ == nullptr) {
    return false;
  }
  lv_area_t coords {};
  lv_obj_get_coords(drag_handle_, &coords);
  return x >= coords.x1 - 20 && x <= coords.x2 + 20 && y >= coords.y1 - 24 && y <= coords.y2 + 18;
}

bool NotificationsPage::should_capture_shell_drag(const InputCommand& command) const {
  if (clear_confirm_active_) {
    return false;
  }
  if (shell_drag_active_) {
    return command.value <= 0 || shell_drag_offset_ > 0;
  }
  if (command.value >= 0) {
    return false;
  }
  if (is_handle_drag_start_zone(command.x, command.y)) {
    const_cast<NotificationsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  if (list_root_ == nullptr || lv_obj_has_flag(list_root_, LV_OBJ_FLAG_HIDDEN)) {
    const_cast<NotificationsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  return false;
}

void NotificationsPage::bind_notifications() {
  track(data_center_.subscribe(EventId::NotificationsChanged,
                               [this](const Event&) {
                                 if (root_ == nullptr) {
                                   return;
                                 }
                                 refresh_content();
                               }));
}

void NotificationsPage::bind_backdrop() {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_backdrop_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_backdrop_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::ShellPreviewRequested,
                               [this](const Event& event) {
                                 const auto* preview = std::get_if<ShellPreviewModel>(&event.payload);
                                 if (preview == nullptr || preview->page_id != PageId::Notifications ||
                                     root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 if (!preview->active) {
                                   shell_drag_active_ = false;
                                   shell_drag_offset_ = 0;
                                   if (open_preview_progress_ >= kNotificationsOpenCommitThreshold) {
                                     set_open_preview_progress(kNotificationsMaxDragOffset, true);
                                   } else {
                                     open_preview_progress_ = 0;
                                     set_open_preview_progress(0, true);
                                     stop_preview_close_timer();
                                     preview_close_timer_ =
                                         lv_timer_create(&NotificationsPage::preview_close_timer_cb, 240U, this);
                                     if (preview_close_timer_ != nullptr) {
                                       lv_timer_set_repeat_count(preview_close_timer_, 1);
                                     }
                                   }
                                   return;
                                 }

                                 stop_preview_close_timer();
                                 if (preview->commit) {
                                   open_preview_progress_ = kNotificationsMaxDragOffset;
                                   set_open_preview_progress(kNotificationsMaxDragOffset, true);
                                   return;
                                 }

                                 set_open_preview_progress(clamp_coord(static_cast<lv_coord_t>(preview->progress),
                                                                       0,
                                                                       kNotificationsMaxDragOffset),
                                                           false);
                               }));
}

void NotificationsPage::refresh_backdrop() {
  if (const auto& time = data_center_.time(); time) {
    apply_backdrop_time(*time);
  }
  if (const auto& battery = data_center_.battery(); battery) {
    apply_backdrop_battery(*battery);
  }
}

void NotificationsPage::apply_backdrop_time(const TimeModel& model) {
  if (backdrop_minute_label_ == nullptr || backdrop_renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(backdrop_minute_label_, "--");
    backdrop_render_state_.hour_text = "--";
    backdrop_render_state_.minute_text = "--";
    backdrop_render_state_.spread_index = backdrop_config_.spread_index;
    backdrop_renderer_->apply(backdrop_render_state_);
    return;
  }

  std::uint8_t hour = model.hour % 12;
  if (hour == 0) {
    hour = 12;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", static_cast<unsigned>(hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));

  lv_label_set_text(backdrop_minute_label_, minute_buffer);
  backdrop_render_state_.hour_text = hour_buffer;
  backdrop_render_state_.minute_text = minute_buffer;
  backdrop_render_state_.spread_index = backdrop_config_.spread_index;
  backdrop_renderer_->apply(backdrop_render_state_);
}

void NotificationsPage::apply_backdrop_battery(const BatteryModel& model) {
  if (backdrop_battery_label_ == nullptr || backdrop_battery_icon_label_ == nullptr) {
    return;
  }

  if (!model.present) {
    lv_label_set_text(backdrop_battery_label_, "--%");
    backdrop_render_state_.battery_percent = -1;
    return;
  }

  char battery_buffer[8] = {};
  std::snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", static_cast<int>(model.percent));
  lv_label_set_text(backdrop_battery_label_, battery_buffer);
  lv_label_set_text(backdrop_battery_icon_label_, model.charging ? LV_SYMBOL_CHARGE : LV_SYMBOL_BATTERY_FULL);
  backdrop_render_state_.battery_percent = model.percent;
}

void NotificationsPage::refresh_content() {
  if (list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr || detail_root_ == nullptr) {
    return;
  }

  if (detail_active_) {
    if (data_center_.find_notification(detail_notification_id_) == nullptr) {
      close_detail();
      return;
    }
    refresh_detail_content();
    return;
  }

  refresh_list_content();
}

void NotificationsPage::refresh_list_content() {
  if (list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr || detail_root_ == nullptr) {
    return;
  }

  reset_notification_card_swipe_state();
  rendered_notification_ids_.clear();
  lv_obj_clean(list_root_);
  lv_obj_add_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  const auto notifications = data_center_.notifications();
  const bool has_items = notifications && !notifications->items.empty();
  if (!has_items) {
    lv_obj_add_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  lv_obj_clear_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);

  for (auto it = notifications->items.rbegin(); it != notifications->items.rend(); ++it) {
    const NotificationItem& item = *it;
    lv_obj_t* card = lv_obj_create(list_root_);
    if (card == nullptr) {
      return;
    }
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(card, 12, 0);
    lv_obj_set_style_bg_color(card, notification_read_card_color(item), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 20, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESS_LOST, this);

    lv_obj_t* icon = create_notification_icon(card, item, true);
    lv_obj_t* status_chip = lv_obj_create(card);
    lv_obj_t* status_label = status_chip == nullptr ? nullptr : lv_label_create(status_chip);
    lv_obj_t* source = lv_label_create(card);
    lv_obj_t* title = lv_label_create(card);
    lv_obj_t* body = lv_label_create(card);
    lv_obj_t* time = lv_label_create(card);
    if (icon == nullptr || status_chip == nullptr || status_label == nullptr || source == nullptr || title == nullptr ||
        body == nullptr || time == nullptr) {
      return;
    }

    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_remove_flag(status_chip, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(status_chip, 46, 22);
    lv_obj_align(status_chip, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(status_chip,
                              item.read ? lv_color_hex(0x223247) : lv_color_hex(0x0F3B5A),
                              0);
    lv_obj_set_style_bg_opa(status_chip, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(status_chip, 0, 0);
    lv_obj_set_style_radius(status_chip, 11, 0);
    lv_obj_set_style_pad_all(status_chip, 0, 0);

    lv_label_set_text(status_label, item.read ? "已读" : "未读");
    lv_obj_set_style_text_font(status_label, cjk_font_14(), 0);
    lv_obj_set_style_text_color(status_label,
                                item.read ? lv_color_hex(0xB7C4D4) : lv_color_hex(0x67E8F9),
                                0);
    lv_obj_center(status_label);

    lv_label_set_text(source, item.source_label.c_str());
    lv_obj_set_width(source, 104);
    lv_label_set_long_mode(source, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(source, cjk_font_14(), 0);
    lv_obj_set_style_text_color(source, notification_accent_color(item.category), 0);
    lv_obj_align(source, LV_ALIGN_TOP_LEFT, 52, 2);

    lv_label_set_text(title, item.title.c_str());
    lv_obj_set_style_text_font(title, cjk_font_16(), 0);
    lv_obj_set_style_text_color(title, notification_primary_text_color(item), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 48);

    lv_label_set_text(body, item.body.c_str());
    lv_obj_set_width(body, 182);
    lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(body, cjk_font_14(), 0);
    lv_obj_set_style_text_color(body, notification_secondary_text_color(item), 0);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 0, 82);
    lv_obj_update_layout(body);

    lv_label_set_text(time, item.time_text.c_str());
    lv_obj_set_style_text_font(time, cjk_font_14(), 0);
    lv_obj_set_style_text_color(time, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align_to(time, body, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    rendered_notification_ids_.push_back(item.id);
  }
}

void NotificationsPage::refresh_detail_content() {
  if (detail_root_ == nullptr || list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr ||
      detail_source_label_ == nullptr || detail_title_label_ == nullptr || detail_body_label_ == nullptr ||
      detail_time_label_ == nullptr) {
    return;
  }

  const NotificationItem* item = data_center_.find_notification(detail_notification_id_);
  if (item == nullptr) {
    close_detail();
    return;
  }

  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  lv_label_set_text(detail_source_label_, item->source_label.c_str());
  lv_obj_set_style_text_color(detail_source_label_, notification_accent_color(item->category), 0);
  lv_label_set_text(detail_title_label_, item->title.c_str());
  lv_label_set_text(detail_body_label_, item->body.c_str());
  lv_label_set_text(detail_time_label_, item->time_text.c_str());
}

void NotificationsPage::open_detail_for(std::string_view id) {
  detail_notification_id_ = std::string(id);
  detail_active_ = true;
  data_center_.mark_notification_read(id);
  refresh_content();
}

void NotificationsPage::close_detail() {
  detail_active_ = false;
  detail_notification_id_.clear();
  refresh_content();
}

void NotificationsPage::show_clear_confirm_overlay() {
  if (clear_confirm_overlay_ == nullptr) {
    return;
  }
  reset_notification_card_swipe_state();
  clear_confirm_active_ = true;
  lv_obj_clear_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(clear_confirm_overlay_);
}

void NotificationsPage::hide_clear_confirm_overlay() {
  clear_confirm_active_ = false;
  if (clear_confirm_overlay_ != nullptr) {
    lv_obj_add_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void NotificationsPage::set_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t offset) {
  if (card == nullptr) {
    return;
  }
  active_card_swipe_offset_ = clamp_coord(offset, 0, kNotificationCardSwipeMaxOffset);
  lv_obj_set_style_translate_x(card, active_card_swipe_offset_, 0);
}

void NotificationsPage::animate_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t target_offset) {
  if (card == nullptr) {
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, card);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_style_translate_x(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value), 0);
    }
  });
  lv_anim_set_values(&anim,
                     lv_obj_get_style_translate_x(card, LV_PART_MAIN),
                     clamp_coord(target_offset, 0, kNotificationCardSwipeMaxOffset));
  lv_anim_set_duration(&anim, 180);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::reset_notification_card_swipe_state() {
  if (active_card_swipe_ != nullptr) {
    lv_obj_set_style_translate_x(active_card_swipe_, 0, 0);
  }
  if (list_root_ != nullptr) {
    lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLLABLE);
  }
  active_card_swipe_ = nullptr;
  active_card_swipe_press_point_ = {};
  active_card_swipe_offset_ = 0;
  active_card_swipe_notification_id_.clear();
  card_swipe_horizontal_capture_ = false;
  card_swipe_vertical_lock_ = false;
}

NotificationWakePage::NotificationWakePage(DataCenter& data_center) : PageBase(data_center) {}

PageId NotificationWakePage::id() const {
  return PageId::NotificationWakePreview;
}

const char* NotificationWakePage::name() const {
  return "NotificationWakePreview";
}

void NotificationWakePage::on_will_appear() {
  refresh_content();
  start_auto_close_timer();
}

void NotificationWakePage::on_will_disappear() {
  stop_auto_close_timer();
}

lv_obj_t* NotificationWakePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02060D);

  icon_container_ = lv_obj_create(root);
  preview_card_ = lv_obj_create(root);
  dismiss_button_ = lv_button_create(root);
  if (icon_container_ == nullptr || preview_card_ == nullptr || dismiss_button_ == nullptr) {
    return nullptr;
  }

  lv_obj_remove_flag(icon_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(icon_container_, 52, 52);
  lv_obj_align(icon_container_, LV_ALIGN_TOP_MID, 0, 18);
  lv_obj_set_style_bg_color(icon_container_, lv_color_hex(0x12304A), 0);
  lv_obj_set_style_bg_opa(icon_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon_container_, 0, 0);
  lv_obj_set_style_radius(icon_container_, 18, 0);
  lv_obj_set_style_pad_all(icon_container_, 0, 0);

  icon_image_ = lv_image_create(icon_container_);
  icon_label_ = lv_label_create(icon_container_);
  if (icon_image_ == nullptr || icon_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_center(icon_image_);
  lv_image_set_inner_align(icon_image_, LV_IMAGE_ALIGN_STRETCH);
  lv_obj_center(icon_label_);
  lv_obj_set_style_text_font(icon_label_, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(icon_label_, lv_color_hex(0xFFFFFF), 0);

  lv_obj_set_size(preview_card_, 210, 126);
  lv_obj_align(preview_card_, LV_ALIGN_TOP_MID, 0, 82);
  lv_obj_set_style_bg_color(preview_card_, lv_color_hex(0x14263A), 0);
  lv_obj_set_style_bg_opa(preview_card_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(preview_card_, 0, 0);
  lv_obj_set_style_radius(preview_card_, 22, 0);
  lv_obj_set_style_pad_all(preview_card_, 14, 0);
  lv_obj_add_event_cb(preview_card_, &NotificationWakePage::open_notifications_event_cb, LV_EVENT_CLICKED, this);

  source_label_ = lv_label_create(preview_card_);
  title_label_ = lv_label_create(preview_card_);
  body_label_ = lv_label_create(preview_card_);
  time_label_ = lv_label_create(preview_card_);
  if (source_label_ == nullptr || title_label_ == nullptr || body_label_ == nullptr || time_label_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_style_text_font(source_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(source_label_, lv_color_hex(0x67E8F9), 0);
  lv_obj_align(source_label_, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_set_style_text_font(title_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(title_label_, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title_label_, LV_ALIGN_TOP_LEFT, 0, 24);

  lv_obj_set_width(body_label_, 182);
  lv_label_set_long_mode(body_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(body_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(body_label_, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(body_label_, LV_ALIGN_TOP_LEFT, 0, 56);

  lv_obj_set_style_text_font(time_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xCBD5E1), 0);
  lv_obj_align(time_label_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_set_size(dismiss_button_, 174, 44);
  lv_obj_align(dismiss_button_, LV_ALIGN_BOTTOM_MID, 0, -16);
  lv_obj_set_style_bg_color(dismiss_button_, lv_color_hex(0x1A3148), 0);
  lv_obj_set_style_bg_opa(dismiss_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(dismiss_button_, 0, 0);
  lv_obj_set_style_radius(dismiss_button_, 22, 0);
  lv_obj_add_event_cb(dismiss_button_, &NotificationWakePage::dismiss_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* dismiss_label = lv_label_create(dismiss_button_);
  if (dismiss_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(dismiss_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(dismiss_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(dismiss_label, kTextDismiss);
  lv_obj_center(dismiss_label);

  bind_notifications();
  refresh_content();
  return root;
}

void NotificationWakePage::dismiss_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  if (!self->current_notification_id_.empty()) {
    self->data_center_.dismiss_notification(self->current_notification_id_);
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationWakePage::open_notifications_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::OpenNotifications, PageId::Notifications});
}

void NotificationWakePage::timeout_cb(lv_timer_t* timer) {
  auto* self = static_cast<NotificationWakePage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->auto_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationWakePage::bind_notifications() {
  track(data_center_.subscribe(EventId::NotificationsChanged,
                               [this](const Event&) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }
                                 refresh_content();
                                 start_auto_close_timer();
                               }));
}

void NotificationWakePage::refresh_content() {
  const NotificationItem* item = data_center_.latest_notification();
  if (item == nullptr || icon_container_ == nullptr || source_label_ == nullptr || title_label_ == nullptr ||
      body_label_ == nullptr || time_label_ == nullptr || icon_image_ == nullptr || icon_label_ == nullptr) {
    return;
  }

  current_notification_id_ = item->id;
  lv_obj_set_style_bg_color(icon_container_,
                            item->category == NotificationCategory::BatteryLow ? lv_color_hex(0x8A6A00)
                                                                              : lv_color_hex(0x19C37D),
                            0);

  if (item->category == NotificationCategory::Message && file_exists(payment_wechat_green_asset_path())) {
    lv_image_set_src(icon_image_, payment_wechat_green_asset_path());
    lv_obj_set_size(icon_image_, 30, 30);
    lv_obj_clear_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_label_set_text(icon_label_,
                      item->category == NotificationCategory::BatteryLow
                          ? (item->badge_text.empty() ? LV_SYMBOL_CHARGE : item->badge_text.c_str())
                          : LV_SYMBOL_BELL);
    lv_obj_clear_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
  }

  lv_label_set_text(source_label_, item->source_label.c_str());
  lv_obj_set_style_text_color(source_label_, notification_accent_color(item->category), 0);
  lv_label_set_text(title_label_, item->title.c_str());
  lv_label_set_text(body_label_, item->body.c_str());
  lv_label_set_text(time_label_, item->time_text.c_str());
}

void NotificationWakePage::start_auto_close_timer() {
  stop_auto_close_timer();
  auto_close_timer_ = lv_timer_create(&NotificationWakePage::timeout_cb, 5000U, this);
  if (auto_close_timer_ != nullptr) {
    lv_timer_set_repeat_count(auto_close_timer_, 1);
  }
}

void NotificationWakePage::stop_auto_close_timer() {
  if (auto_close_timer_ != nullptr) {
    lv_timer_del(auto_close_timer_);
    auto_close_timer_ = nullptr;
  }
}

QuickSettingsPage::QuickSettingsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId QuickSettingsPage::id() const {
  return PageId::QuickSettings;
}

const char* QuickSettingsPage::name() const {
  return "QuickSettings";
}

void QuickSettingsPage::on_will_appear() {
  PageBase::on_will_appear();
  reset_quick_settings_log();
  hide_toggle_toast();
  stop_preview_close_timer();
  shell_drag_offset_ = 0;
  open_preview_progress_ = 0;
  shell_drag_active_ = false;
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kQuickSettingsSheetY);
  }
  refresh_backdrop();
  if (suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= suppress_click_deadline_) {
    suppress_next_click_ = false;
    suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
  }
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
}

void QuickSettingsPage::on_will_disappear() {
  stop_toast_timer();
  stop_preview_close_timer();
  hide_toggle_toast();
  hide_long_battery_confirm();
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
  PageBase::on_will_disappear();
}

lv_obj_t* QuickSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x050913);
  lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

  backdrop_root_ = lv_obj_create(root);
  sheet_container_ = lv_obj_create(root);
  toast_container_ = lv_obj_create(root);
  long_battery_confirm_overlay_ = lv_obj_create(root);
  if (backdrop_root_ == nullptr || sheet_container_ == nullptr || toast_container_ == nullptr ||
      long_battery_confirm_overlay_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(backdrop_root_, 240, 296);
  lv_obj_align(backdrop_root_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(backdrop_root_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(backdrop_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(backdrop_root_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_root_, 0, 0);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t* backdrop_overlay = lv_obj_create(backdrop_root_);
  lv_obj_t* battery_row = lv_obj_create(backdrop_root_);
  backdrop_battery_icon_label_ = lv_label_create(battery_row);
  backdrop_battery_label_ = lv_label_create(battery_row);
  backdrop_style_stage_ = lv_obj_create(backdrop_root_);
  backdrop_minute_label_ = lv_label_create(backdrop_root_);
  if (backdrop_overlay == nullptr || battery_row == nullptr || backdrop_battery_icon_label_ == nullptr ||
      backdrop_battery_label_ == nullptr || backdrop_style_stage_ == nullptr || backdrop_minute_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(backdrop_overlay);
  lv_obj_set_size(backdrop_overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(backdrop_overlay, lv_color_hex(0x01040A), 0);
  lv_obj_set_style_bg_opa(backdrop_overlay, LV_OPA_20, 0);
  lv_obj_set_style_border_width(backdrop_overlay, 0, 0);
  lv_obj_set_style_pad_all(backdrop_overlay, 0, 0);
  lv_obj_align(backdrop_overlay, LV_ALIGN_TOP_LEFT, 0, 0);

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(backdrop_battery_icon_label_);
  ui_apply_text(backdrop_battery_icon_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_icon_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);

  ui_prepare_label(backdrop_battery_label_);
  ui_apply_text(backdrop_battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_label_, "--%");

  ui_prepare_box(backdrop_style_stage_);
  lv_obj_set_size(backdrop_style_stage_, 240, 296);
  lv_obj_align(backdrop_style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(backdrop_style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(backdrop_style_stage_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_style_stage_, 0, 0);

  backdrop_renderer_ = create_watchface_style_renderer(backdrop_config_);
  if (!backdrop_renderer_ || backdrop_renderer_->build(backdrop_style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(backdrop_minute_label_);
  ui_apply_text(backdrop_minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(backdrop_minute_label_, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(backdrop_minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(backdrop_minute_label_, 96);
  lv_obj_set_style_text_align(backdrop_minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(backdrop_minute_label_, 76, 234);
  lv_label_set_text(backdrop_minute_label_, "--");

  lv_obj_set_size(sheet_container_, kQuickSettingsSheetWidth, kQuickSettingsSheetHeight);
  lv_obj_set_style_bg_color(sheet_container_, lv_color_hex(0x09131F), 0);
  lv_obj_set_style_bg_opa(sheet_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sheet_container_, 0, 0);
  lv_obj_set_style_radius(sheet_container_, 32, 0);
  lv_obj_set_style_pad_top(sheet_container_, 18, 0);
  lv_obj_set_style_pad_bottom(sheet_container_, 18, 0);
  lv_obj_set_style_pad_left(sheet_container_, 16, 0);
  lv_obj_set_style_pad_right(sheet_container_, 16, 0);
  lv_obj_set_style_shadow_width(sheet_container_, 34, 0);
  lv_obj_set_style_shadow_color(sheet_container_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_shadow_opa(sheet_container_, LV_OPA_50, 0);
  lv_obj_set_style_border_color(sheet_container_, lv_color_hex(0x15263A), 0);
  lv_obj_set_style_border_opa(sheet_container_, LV_OPA_50, 0);
  lv_obj_set_style_border_width(sheet_container_, 1, 0);
  lv_obj_remove_flag(sheet_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_align(sheet_container_, LV_ALIGN_TOP_MID, 0, 0);

  toast_label_ = lv_label_create(toast_container_);
  if (toast_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(toast_container_);
  lv_obj_set_style_bg_color(toast_container_, lv_color_hex(0x6A7D97), 0);
  lv_obj_set_style_bg_opa(toast_container_, LV_OPA_80, 0);
  lv_obj_set_style_border_width(toast_container_, 0, 0);
  lv_obj_set_style_radius(toast_container_, 22, 0);
  lv_obj_set_style_pad_left(toast_container_, 18, 0);
  lv_obj_set_style_pad_right(toast_container_, 18, 0);
  lv_obj_set_style_pad_top(toast_container_, 8, 0);
  lv_obj_set_style_pad_bottom(toast_container_, 8, 0);
  lv_obj_remove_flag(toast_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(toast_container_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_align(toast_container_, LV_ALIGN_TOP_MID, 0, 28);
  lv_obj_set_size(toast_container_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_move_foreground(toast_container_);

  ui_prepare_label(toast_label_);
  ui_apply_text(toast_label_, TextStyle::Body);
  lv_obj_set_style_text_font(toast_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(toast_label_, lv_color_hex(0xF8FBFF), 0);
  lv_label_set_long_mode(toast_label_, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_set_size(toast_label_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_text_align(toast_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(toast_label_, "");
  lv_obj_center(toast_label_);

  ui_prepare_box(long_battery_confirm_overlay_);
  lv_obj_set_size(long_battery_confirm_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_align(long_battery_confirm_overlay_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(long_battery_confirm_overlay_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(long_battery_confirm_overlay_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(long_battery_confirm_overlay_, 0, 0);
  lv_obj_set_style_radius(long_battery_confirm_overlay_, 0, 0);
  lv_obj_add_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* confirm_body = lv_label_create(long_battery_confirm_overlay_);
  lv_obj_t* cancel_button = lv_button_create(long_battery_confirm_overlay_);
  lv_obj_t* confirm_button = lv_button_create(long_battery_confirm_overlay_);
  if (confirm_body == nullptr || cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }

  ui_prepare_label(confirm_body);
  lv_obj_set_width(confirm_body, 196);
  lv_obj_set_style_text_font(confirm_body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(confirm_body, lv_color_hex(0xF7FBFF), 0);
  lv_obj_set_style_text_align(confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(confirm_body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(confirm_body, kTextLongBatteryConfirmBody);
  lv_obj_align(confirm_body, LV_ALIGN_TOP_MID, 0, 34);

  for (lv_obj_t* button : {cancel_button, confirm_button}) {
    attach_click_guard(button);
    ui_prepare_box(button);
    lv_obj_set_size(button, 92, 50);
    lv_obj_set_style_radius(button, 18, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_add_event_cb(button, &QuickSettingsPage::long_battery_confirm_event_cb, LV_EVENT_CLICKED, this);
  }
  lv_obj_align(cancel_button, LV_ALIGN_BOTTOM_LEFT, 24, -34);
  lv_obj_align(confirm_button, LV_ALIGN_BOTTOM_RIGHT, -24, -34);
  lv_obj_set_style_bg_color(cancel_button, lv_color_hex(0x17314C), 0);
  lv_obj_set_style_bg_opa(cancel_button, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(confirm_button, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button, LV_OPA_COVER, 0);
  lv_obj_set_user_data(cancel_button, reinterpret_cast<void*>(0U));
  lv_obj_set_user_data(confirm_button, reinterpret_cast<void*>(1U));

  for (const auto [button, text] : {std::pair {cancel_button, LV_SYMBOL_CLOSE}, std::pair {confirm_button, LV_SYMBOL_OK}}) {
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE8FBFF), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }
  lv_obj_move_foreground(long_battery_confirm_overlay_);

  drag_handle_ = lv_obj_create(sheet_container_);
  if (drag_handle_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* grid = lv_obj_create(sheet_container_);
  if (grid == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(grid, 196, 196);
  lv_obj_align(grid, LV_ALIGN_CENTER, 0, 6);
  lv_obj_set_layout(grid, LV_LAYOUT_GRID);
  static lv_coord_t columns[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t rows[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(grid, columns, rows);
  lv_obj_set_style_pad_all(grid, 0, 0);
  lv_obj_set_style_pad_row(grid, 8, 0);
  lv_obj_set_style_pad_column(grid, 8, 0);
  lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(grid, 0, 0);
  lv_obj_set_style_radius(grid, 0, 0);
  lv_obj_remove_flag(grid, LV_OBJ_FLAG_SCROLLABLE);

  for (std::size_t index = 0; index < toggles_.size(); ++index) {
    const auto row = static_cast<lv_coord_t>(index / 3);
    const auto col = static_cast<lv_coord_t>(index % 3);
    lv_obj_t* button = lv_button_create(grid);
    if (button == nullptr) {
      return nullptr;
    }
    toggles_[index].button = button;
    lv_obj_set_grid_cell(button, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_size(button, 58, 58);
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x15294A), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_long_press_event_cb, LV_EVENT_LONG_PRESSED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_release_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_press_lost_event_cb, LV_EVENT_PRESS_LOST, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* icon = lv_label_create(button);
    if (icon == nullptr) {
      return nullptr;
    }
    toggles_[index].icon_label = icon;
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);
    lv_label_set_text(icon, toggles_[index].icon_text);
    lv_obj_center(icon);
    apply_toggle_visual(index);
  }

  lv_obj_set_size(drag_handle_, 52, 7);
  lv_obj_align(drag_handle_, LV_ALIGN_TOP_MID, 0, 4);
  lv_obj_set_style_bg_color(drag_handle_, lv_color_hex(0x4B5E7A), 0);
  lv_obj_set_style_bg_opa(drag_handle_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(drag_handle_, 0, 0);
  lv_obj_set_style_radius(drag_handle_, LV_RADIUS_CIRCLE, 0);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_foreground(drag_handle_);

  bind_input();
  bind_display_policy();
  bind_backdrop();
  refresh_backdrop();
  set_open_preview_progress(0, false);
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
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("CLICK_ENTER", index, label, self->suppress_next_click_, true, target, event);
  const bool guard_allows = target != nullptr && click_guard_allows(target);
  append_quick_settings_log("CLICK_GUARD", index, label, self->suppress_next_click_, guard_allows, target, event);
  if (target == nullptr || !guard_allows) {
    return;
  }
  if (self->suppress_next_click_ &&
      self->suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= self->suppress_click_deadline_) {
    append_quick_settings_log("CLICK_SUPPRESS_EXPIRED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
  }
  if (self->suppress_next_click_) {
    append_quick_settings_log("CLICK_SWALLOWED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
    return;
  }

  if (index >= self->toggles_.size()) {
    append_quick_settings_log("CLICK_INDEX_OOB", index, label, self->suppress_next_click_, true, target, event);
    return;
  }

  auto& toggle = self->toggles_[index];
  append_quick_settings_log("CLICK_HANDLE", index, toggle.label, self->suppress_next_click_, true, target, event);
  if (toggle.kind == ToggleKind::OpenSettings) {
    append_quick_settings_log("CLICK_NAV_SETTINGS", index, toggle.label, self->suppress_next_click_, true, target, event);
    self->request_navigation({NavigationAction::LaunchApp, PageId::SettingsHome});
    return;
  }

  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->notification_wake_enabled;
    self->data_center_.set_notification_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
  } else if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
    self->data_center_.set_raise_to_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(!current ? "抬腕亮屏已开启" : "抬腕亮屏已关闭");
  } else if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = self->data_center_.display_policy();
    const bool current = policy && policy->keep_screen_on_duration_ms > 0U;
    self->data_center_.set_keep_screen_on_duration_ms(current ? 0U : 300000U);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(current ? "持续亮屏已关闭" : "持续亮屏5分钟");
  } else if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = self->data_center_.power_mode();
    const bool current = mode && mode->long_battery_mode_enabled;
    if (current) {
      self->data_center_.set_long_battery_mode_enabled(false);
      toggle.mode = 0;
    } else {
      self->show_long_battery_confirm();
      return;
    }
  } else {
    toggle.mode = toggle.mode == 0 ? 1 : 0;
  }
  self->apply_toggle_visual(index);
  append_quick_settings_log("CLICK_DONE", index, toggle.label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toggle_long_press_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  const char* label = index < self->toggles_.size() ? self->toggles_[index].label : "oob";
  append_quick_settings_log("LONG_PRESS_ENTER", index, label, self->suppress_next_click_, true, target, event);
  if (index >= self->toggles_.size()) {
    return;
  }

  self->stop_toast_timer();
  self->hide_toggle_toast();
  if (!self->toggles_[index].detail_page.has_value()) {
    return;
  }
  self->suppress_next_click_ = true;
  self->suppress_click_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(1200);
  self->long_press_source_button_ = target;
  self->suppress_global_clicks_for(std::chrono::milliseconds(520));
  append_quick_settings_log(
      "LONG_PRESS_NAV", index, self->toggles_[index].label, self->suppress_next_click_, true, target, event);
  self->request_navigation({NavigationAction::LaunchApp, *self->toggles_[index].detail_page});
}

void QuickSettingsPage::toggle_release_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("RELEASE", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
}

void QuickSettingsPage::toggle_press_lost_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("PRESS_LOST", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
  self->long_press_source_button_ = nullptr;
  append_quick_settings_log("PRESS_LOST_SOURCE_CLEARED", index, label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toast_timeout_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->toast_timer_ = nullptr;
  self->hide_toggle_toast();
}

void QuickSettingsPage::long_battery_confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto confirmed = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)) == 1U;
  self->hide_long_battery_confirm();
  if (!confirmed) {
    return;
  }

  self->data_center_.set_long_battery_mode_enabled(true);
}

void QuickSettingsPage::bind_input() {
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
                                   case InputAction::ScrollDrag:
                                     if (should_capture_shell_drag(*command)) {
                                       set_close_drag_offset(
                                           clamp_coord(static_cast<lv_coord_t>(command->value),
                                                       0,
                                                       kQuickSettingsMaxDragOffset),
                                           false);
                                     }
                                     break;
                                   case InputAction::ScrollFlick:
                                     if (should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, true);
                                     }
                                     break;
                                   case InputAction::ScrollRelease:
                                     if (shell_drag_active_ || shell_drag_offset_ > 0 || should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, false);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void QuickSettingsPage::bind_display_policy() {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event& event) {
                                 const auto* policy = std::get_if<DisplayPolicyModel>(&event.payload);
                                 if (policy == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   switch (toggles_[index].kind) {
                                     case ToggleKind::NotifyWake:
                                       toggles_[index].mode = policy->notification_wake_enabled ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::RaiseToWake:
                                       toggles_[index].mode = policy->raise_to_wake_mode != RaiseToWakeMode::Off ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::AodFiveMinutes:
                                       toggles_[index].mode = policy->keep_screen_on_duration_ms > 0U ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     default:
                                       break;
                                   }
                                 }
                               }));
  track(data_center_.subscribe(EventId::PowerModeChanged,
                               [this](const Event& event) {
                                 const auto* mode = std::get_if<PowerModeModel>(&event.payload);
                                 if (mode == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   if (toggles_[index].kind == ToggleKind::LongBattery) {
                                     toggles_[index].mode = mode->long_battery_mode_enabled ? 1 : 0;
                                     apply_toggle_visual(index);
                                   }
                                 }
                               }));
}

void QuickSettingsPage::bind_backdrop() {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_backdrop_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_backdrop_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::ShellPreviewRequested,
                               [this](const Event& event) {
                                 const auto* preview = std::get_if<ShellPreviewModel>(&event.payload);
                                 if (preview == nullptr || preview->page_id != PageId::QuickSettings ||
                                     root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 if (!preview->active) {
                                   shell_drag_active_ = false;
                                   shell_drag_offset_ = 0;
                                   if (open_preview_progress_ >= kQuickSettingsOpenCommitThreshold) {
                                     set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   } else {
                                     open_preview_progress_ = 0;
                                     set_open_preview_progress(0, true);
                                     stop_preview_close_timer();
                                     preview_close_timer_ =
                                         lv_timer_create(&QuickSettingsPage::preview_close_timer_cb, 240U, this);
                                     if (preview_close_timer_ != nullptr) {
                                       lv_timer_set_repeat_count(preview_close_timer_, 1);
                                     }
                                   }
                                   return;
                                 }

                                 stop_preview_close_timer();
                                 if (preview->commit) {
                                   open_preview_progress_ = kQuickSettingsMaxDragOffset;
                                   set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   return;
                                 }

                                 set_open_preview_progress(clamp_coord(static_cast<lv_coord_t>(preview->progress),
                                                                       0,
                                                                       kQuickSettingsMaxDragOffset),
                                                           false);
                               }));
}

void QuickSettingsPage::refresh_backdrop() {
  if (const auto& time = data_center_.time(); time) {
    apply_backdrop_time(*time);
  }
  if (const auto& battery = data_center_.battery(); battery) {
    apply_backdrop_battery(*battery);
  }
}

void QuickSettingsPage::apply_backdrop_time(const TimeModel& model) {
  if (backdrop_minute_label_ == nullptr || backdrop_renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(backdrop_minute_label_, "--");
    backdrop_render_state_.hour_text = "--";
    backdrop_render_state_.minute_text = "--";
    backdrop_render_state_.spread_index = backdrop_config_.spread_index;
    backdrop_renderer_->apply(backdrop_render_state_);
    return;
  }

  std::uint8_t hour = model.hour % 12;
  if (hour == 0) {
    hour = 12;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", static_cast<unsigned>(hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));

  lv_label_set_text(backdrop_minute_label_, minute_buffer);
  backdrop_render_state_.hour_text = hour_buffer;
  backdrop_render_state_.minute_text = minute_buffer;
  backdrop_render_state_.spread_index = backdrop_config_.spread_index;
  backdrop_renderer_->apply(backdrop_render_state_);
}

void QuickSettingsPage::apply_backdrop_battery(const BatteryModel& model) {
  if (backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr) {
    return;
  }

  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);
  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%", static_cast<int>(model.percent));
  lv_label_set_text(backdrop_battery_label_, buffer);
}

void QuickSettingsPage::apply_toggle_visual(std::size_t index) {
  if (index >= toggles_.size() || toggles_[index].button == nullptr || toggles_[index].icon_label == nullptr) {
    return;
  }

  auto& toggle = toggles_[index];
  const bool active = is_toggle_active(toggle);

  lv_obj_set_style_bg_color(toggle.button, active ? lv_color_hex(0x1493FF) : lv_color_hex(0x15294A), 0);
  lv_obj_set_style_bg_opa(toggle.button, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(toggle.icon_label, lv_color_hex(0xF5FAFF), 0);
}

bool QuickSettingsPage::is_toggle_active(const ToggleState& toggle) const {
  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->notification_wake_enabled;
  }
  if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
  }
  if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = data_center_.display_policy();
    return policy && policy->keep_screen_on_duration_ms > 0U;
  }
  if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = data_center_.power_mode();
    return mode && mode->long_battery_mode_enabled;
  }
  if (toggle.kind == ToggleKind::OpenSettings) {
    return false;
  }
  return toggle.mode != 0;
}

void QuickSettingsPage::show_long_battery_confirm() {
  if (long_battery_confirm_overlay_ == nullptr) {
    return;
  }
  hide_toggle_toast();
  lv_obj_clear_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(long_battery_confirm_overlay_);
}

void QuickSettingsPage::hide_long_battery_confirm() {
  if (long_battery_confirm_overlay_ != nullptr) {
    lv_obj_add_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void QuickSettingsPage::show_toggle_toast(const char* text) {
  if (toast_container_ == nullptr || toast_label_ == nullptr || text == nullptr) {
    return;
  }
  lv_label_set_text(toast_label_, text);
  lv_obj_update_layout(toast_label_);
  const lv_coord_t label_width = static_cast<lv_coord_t>(lv_obj_get_width(toast_label_) + 1);
  const lv_coord_t label_height = static_cast<lv_coord_t>(lv_obj_get_height(toast_label_) + 1);
  lv_obj_set_size(toast_container_,
                  static_cast<lv_coord_t>(label_width + 32),
                  static_cast<lv_coord_t>(label_height + 16));
  lv_obj_update_layout(toast_container_);
  lv_obj_clear_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(toast_container_);
  stop_toast_timer();
  toast_timer_ = lv_timer_create(&QuickSettingsPage::toast_timeout_cb, 3000U, this);
  if (toast_timer_ != nullptr) {
    lv_timer_set_repeat_count(toast_timer_, 1);
  }
}

void QuickSettingsPage::hide_toggle_toast() {
  if (toast_container_ == nullptr) {
    return;
  }
  lv_obj_add_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
}

void QuickSettingsPage::stop_toast_timer() {
  if (toast_timer_ != nullptr) {
    lv_timer_del(toast_timer_);
    toast_timer_ = nullptr;
  }
}

void QuickSettingsPage::set_open_preview_progress(lv_coord_t progress, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  open_preview_progress_ = clamp_coord(progress, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t hidden_y = 296;
  const lv_coord_t target_y = static_cast<lv_coord_t>(
      hidden_y - ((hidden_y - kQuickSettingsSheetY) * open_preview_progress_) / kQuickSettingsMaxDragOffset);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);

}

void QuickSettingsPage::set_close_drag_offset(lv_coord_t offset, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  shell_drag_offset_ = clamp_coord(offset, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t target_y = static_cast<lv_coord_t>(kQuickSettingsSheetY + shell_drag_offset_);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);

}

void QuickSettingsPage::stop_preview_close_timer() {
  if (preview_close_timer_ != nullptr) {
    lv_timer_del(preview_close_timer_);
    preview_close_timer_ = nullptr;
  }
}

void QuickSettingsPage::preview_close_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->preview_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::finish_drag_close(std::int16_t release_delta, bool flick_close) {
  if (root_ == nullptr) {
    return;
  }

  shell_drag_active_ = false;
  const bool should_close =
      shell_drag_offset_ >= kQuickSettingsCloseDragThreshold ||
      (flick_close && release_delta >= kQuickSettingsCloseFlickThreshold);
  if (should_close) {
    shell_drag_offset_ = 0;
    request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
    return;
  }

  shell_drag_offset_ = 0;
  set_close_drag_offset(0, true);
}

bool QuickSettingsPage::is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const {
  if (drag_handle_ == nullptr) {
    return false;
  }
  lv_area_t coords {};
  lv_obj_get_coords(drag_handle_, &coords);
  return x >= coords.x1 - 20 && x <= coords.x2 + 20 && y >= coords.y1 - 18 && y <= coords.y2 + 24;
}

bool QuickSettingsPage::should_capture_shell_drag(const InputCommand& command) const {
  if (shell_drag_active_) {
    return command.value >= 0 || shell_drag_offset_ > 0;
  }
  if (command.value <= 0) {
    return false;
  }
  if (is_handle_drag_start_zone(command.x, command.y)) {
    const_cast<QuickSettingsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  return false;
}

namespace {

int weekday_index(const TimeModel& time) {
  if (!time.valid) {
    return -1;
  }
  std::tm calendar {};
  calendar.tm_year = static_cast<int>(time.year) - 1900;
  calendar.tm_mon = static_cast<int>(time.month) - 1;
  calendar.tm_mday = static_cast<int>(time.day);
  calendar.tm_hour = 12;
  if (std::mktime(&calendar) == -1) {
    return -1;
  }
  return calendar.tm_wday;
}

const char* weekday_text(int index) {
  switch (index) {
    case 0:
      return "\xE5\x91\xA8\xE6\x97\xA5";
    case 1:
      return "\xE5\x91\xA8\xE4\xB8\x80";
    case 2:
      return "\xE5\x91\xA8\xE4\xBA\x8C";
    case 3:
      return "\xE5\x91\xA8\xE4\xB8\x89";
    case 4:
      return "\xE5\x91\xA8\xE5\x9B\x9B";
    case 5:
      return "\xE5\x91\xA8\xE4\xBA\x94";
    case 6:
      return "\xE5\x91\xA8\xE5\x85\xAD";
    default:
      return "--";
  }
}

std::string long_battery_date_text(const TimeModel& time) {
  if (!time.valid) {
    return "--/-- --";
  }
  char buffer[32] = {};
  std::snprintf(buffer,
                sizeof(buffer),
                "%02u/%02u %s",
                static_cast<unsigned>(time.month),
                static_cast<unsigned>(time.day),
                weekday_text(weekday_index(time)));
  return buffer;
}

}  // namespace

LongBatteryWatchfacePage::LongBatteryWatchfacePage(DataCenter& data_center) : PageBase(data_center) {}

PageId LongBatteryWatchfacePage::id() const {
  return PageId::LongBatteryWatchface;
}

const char* LongBatteryWatchfacePage::name() const {
  return page_name(PageId::LongBatteryWatchface);
}

void LongBatteryWatchfacePage::on_will_appear() {
  if (const auto time = data_center_.time()) {
    time_model_ = *time;
  }
  if (const auto battery = data_center_.battery()) {
    battery_model_ = *battery;
  }
  if (const auto steps = data_center_.steps()) {
    steps_model_ = *steps;
  }
  refresh_view();
}

lv_obj_t* LongBatteryWatchfacePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  ui_prepare_box(root);
  lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_add_flag(root, LV_OBJ_FLAG_CLICKABLE);
  attach_click_guard(root);
  lv_obj_add_event_cb(root, &LongBatteryWatchfacePage::watchface_click_event_cb, LV_EVENT_CLICKED, this);

  date_label_ = lv_label_create(root);
  time_label_ = lv_label_create(root);
  battery_label_ = lv_label_create(root);
  steps_label_ = lv_label_create(root);
  if (date_label_ == nullptr || time_label_ == nullptr || battery_label_ == nullptr || steps_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_label(date_label_);
  lv_obj_set_style_text_font(date_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(date_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(date_label_, "--/-- --");
  lv_obj_align(date_label_, LV_ALIGN_CENTER, 0, -48);

  ui_prepare_label(time_label_);
  lv_obj_set_style_text_font(time_label_, cjk_font_72(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(time_label_, "--:--");
  lv_obj_align(time_label_, LV_ALIGN_CENTER, 0, 4);

  ui_prepare_label(battery_label_);
  lv_obj_set_style_text_font(battery_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(battery_label_, "电量 0%");
  lv_obj_align(battery_label_, LV_ALIGN_CENTER, 0, 58);

  ui_prepare_label(steps_label_);
  lv_obj_set_style_text_font(steps_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(steps_label_, lv_color_hex(0xE2F6FF), 0);
  lv_label_set_text(steps_label_, "步数 0");
  lv_obj_align(steps_label_, LV_ALIGN_BOTTOM_MID, 0, -28);

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
  track(data_center_.subscribe(EventId::StepsChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<StepsModel>(&event.payload)) {
                                   apply_steps(*model);
                                 }
                               }));

  on_will_appear();
  return root;
}

void LongBatteryWatchfacePage::watchface_click_event_cb(lv_event_t* event) {
  auto* self = static_cast<LongBatteryWatchfacePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::LongBatteryExit});
}

void LongBatteryWatchfacePage::apply_time(const TimeModel& model) {
  time_model_ = model;
  refresh_view();
}

void LongBatteryWatchfacePage::apply_battery(const BatteryModel& model) {
  battery_model_ = model;
  refresh_view();
}

void LongBatteryWatchfacePage::apply_steps(const StepsModel& model) {
  steps_model_ = model;
  refresh_view();
}

void LongBatteryWatchfacePage::refresh_view() {
  if (date_label_ == nullptr || time_label_ == nullptr || battery_label_ == nullptr || steps_label_ == nullptr) {
    return;
  }

  const std::string date_text = long_battery_date_text(time_model_);
  lv_label_set_text(date_label_, date_text.c_str());

  char time_text[8] = {};
  if (time_model_.valid) {
    std::snprintf(time_text,
                  sizeof(time_text),
                  "%02u:%02u",
                  static_cast<unsigned>(time_model_.hour),
                  static_cast<unsigned>(time_model_.minute));
  } else {
    std::snprintf(time_text, sizeof(time_text), "--:--");
  }
  lv_label_set_text(time_label_, time_text);

  char battery_text[24] = {};
  std::snprintf(battery_text,
                sizeof(battery_text),
                "\xE7\x94\xB5\xE9\x87\x8F %d%%",
                std::clamp<int>(battery_model_.percent, 0, 100));
  lv_label_set_text(battery_label_, battery_text);

  char steps_text[32] = {};
  std::snprintf(steps_text,
                sizeof(steps_text),
                "\xE6\xAD\xA5\xE6\x95\xB0 %lu",
                static_cast<unsigned long>(steps_model_.daily_steps));
  lv_label_set_text(steps_label_, steps_text);
}

}  // namespace twsim::app
