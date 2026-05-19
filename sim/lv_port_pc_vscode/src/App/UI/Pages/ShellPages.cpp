#include "App/UI/Pages/ShellPages.h"

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/UiStyles.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"
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

constexpr const char* kTextClear = "\xE6\xB8\x85\xE7\xA9\xBA";
constexpr const char* kTextNoMessages = "\xE6\x9A\x82\xE6\x97\xA0\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextDismiss = "\xE5\xBF\xBD\xE7\x95\xA5";
constexpr lv_coord_t kNotificationsCloseDragThreshold = 96;
constexpr lv_coord_t kNotificationsCloseFlickThreshold = 20;
constexpr lv_coord_t kNotificationsMaxDragOffset = 220;
constexpr lv_coord_t kNotificationsOpenCommitThreshold = 148;
constexpr lv_coord_t kNotificationsSheetY = 14;
constexpr lv_coord_t kNotificationsSheetWidth = 228;
constexpr lv_coord_t kNotificationsSheetHeight = 268;
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

std::string resolve_windows_cjk_font_path() {
  namespace fs = std::filesystem;
  const std::array<fs::path, 5> candidates {
      fs::path("C:/Windows/Fonts/Deng.ttf"),
      fs::path("C:/Windows/Fonts/Dengl.ttf"),
      fs::path("C:/Windows/Fonts/simhei.ttf"),
      fs::path("C:/Windows/Fonts/msyh.ttc"),
      fs::path("C:/Windows/Fonts/simsun.ttc"),
  };

  for (const auto& candidate : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(candidate, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  return {};
}

const lv_font_t* cjk_font_14() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 14);
  return font != nullptr ? font : &lv_font_montserrat_14;
}

const lv_font_t* cjk_font_16() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 16);
  return font != nullptr ? font : &lv_font_montserrat_16;
}

const lv_font_t* cjk_font_72() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 72);
  return font != nullptr ? font : &lv_font_montserrat_48;
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

    auto build_metric_card = [&](lv_obj_t* card, const char* value, bool sleep_icon, bool emphasize) -> bool {
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

      const char* icon_path = sleep_icon ? sleep_icon_asset_path() : steps_icon_asset_path();
      lv_obj_set_size(icon_image, 58, 58);
      lv_image_set_inner_align(icon_image, LV_IMAGE_ALIGN_CONTAIN);
      lv_image_set_src(icon_image, icon_path);
      lv_obj_center(icon_image);

      ui_prepare_label(value_label);
      ui_apply_text(value_label, TextStyle::HeroSoft);
      lv_obj_set_style_text_font(value_label, emphasize ? &lv_font_montserrat_16 : &lv_font_montserrat_14, 0);
      lv_obj_set_style_text_color(value_label, lv_color_hex(0xFFFFFF), 0);
      set_single_line_label(value_label, 72);
      lv_label_set_text(value_label, value);
      lv_obj_align(value_label, LV_ALIGN_TOP_LEFT, sleep_icon ? 6 : 2, 70);
      return true;
    };

    if (!build_metric_card(sleep_card, "7h 36m", true, false) ||
        !build_metric_card(steps_card, "7645", false, true)) {
      return nullptr;
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
  const auto app_tile_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<WeatherShortcutPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_target_obj(event);
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

  lv_obj_add_event_cb(hero_card,
                      app_tile_event_cb,
                      LV_EVENT_CLICKED,
                      this);
  attach_click_guard(hero_card);
  lv_obj_set_user_data(hero_card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(PageId::AppWeather)));

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

  for (auto [card, target] : {std::pair<lv_obj_t*, PageId> {sleep_card, PageId::AppSleep},
                              std::pair<lv_obj_t*, PageId> {steps_card, PageId::Pedometer}}) {
    attach_click_guard(card);
    lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
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
  const auto app_tile_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<PaymentsShortcutPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_target_obj(event);
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
  const auto app_card_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<NfcShortcutPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_target_obj(event);
    if (target == nullptr || !click_guard_allows(target)) {
      return;
    }
    self->request_navigation({NavigationAction::Push, PageId::AppNfc});
  };

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
  attach_click_guard(card);
  lv_obj_add_event_cb(card, app_card_event_cb, LV_EVENT_CLICKED, this);

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
  const auto app_tile_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<HealthShortcutPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_target_obj(event);
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
                                     lv_obj_scroll_by(list_root_, 0, 82 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     lv_obj_scroll_by(list_root_, 0, -82 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
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
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kNotificationsSheetY);
  }
  refresh_backdrop();
  refresh_content();
}

void NotificationsPage::on_will_disappear() {
  stop_preview_close_timer();
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
  drag_handle_ = lv_obj_create(sheet_container_);
  if (clear_button_ == nullptr || list_root_ == nullptr || empty_state_ == nullptr || drag_handle_ == nullptr) {
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
  self->data_center_.clear_notifications();
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
  if (list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr) {
    return;
  }

  lv_obj_clean(list_root_);

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
    lv_obj_set_style_bg_color(card, notification_card_color(item.category), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 20, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* icon = create_notification_icon(card, item, true);
    lv_obj_t* source = lv_label_create(card);
    lv_obj_t* title = lv_label_create(card);
    lv_obj_t* body = lv_label_create(card);
    lv_obj_t* time = lv_label_create(card);
    if (icon == nullptr || source == nullptr || title == nullptr || body == nullptr || time == nullptr) {
      return;
    }

    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_label_set_text(source, item.source_label.c_str());
    lv_obj_set_style_text_font(source, cjk_font_14(), 0);
    lv_obj_set_style_text_color(source, notification_accent_color(item.category), 0);
    lv_obj_align(source, LV_ALIGN_TOP_LEFT, 52, 2);

    lv_label_set_text(title, item.title.c_str());
    lv_obj_set_style_text_font(title, cjk_font_16(), 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 48);

    lv_label_set_text(body, item.body.c_str());
    lv_obj_set_width(body, 182);
    lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(body, cjk_font_14(), 0);
    lv_obj_set_style_text_color(body, lv_color_hex(0xE2E8F0), 0);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 0, 82);

    lv_label_set_text(time, item.time_text.c_str());
    lv_obj_set_style_text_font(time, cjk_font_14(), 0);
    lv_obj_set_style_text_color(time, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align(time, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  }
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
  lv_obj_t* card = lv_obj_create(root);
  dismiss_button_ = lv_button_create(root);
  if (icon_container_ == nullptr || card == nullptr || dismiss_button_ == nullptr) {
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

  lv_obj_set_size(card, 210, 126);
  lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 82);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x14263A), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_radius(card, 22, 0);
  lv_obj_set_style_pad_all(card, 14, 0);

  source_label_ = lv_label_create(card);
  title_label_ = lv_label_create(card);
  body_label_ = lv_label_create(card);
  time_label_ = lv_label_create(card);
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
  if (backdrop_root_ == nullptr || sheet_container_ == nullptr || toast_container_ == nullptr) {
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
  self->suppress_next_click_ = true;
  self->suppress_click_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(1200);
  self->long_press_source_button_ = target;
  self->suppress_global_clicks_for(std::chrono::milliseconds(520));
  append_quick_settings_log(
      "LONG_PRESS_NAV", index, self->toggles_[index].label, self->suppress_next_click_, true, target, event);
  self->request_navigation({NavigationAction::LaunchApp, self->toggles_[index].detail_page});
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
  bool active = toggle.mode != 0;
  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = data_center_.display_policy();
    active = !policy || policy->notification_wake_enabled;
  }
  if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = data_center_.display_policy();
    active = !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
  }
  if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = data_center_.display_policy();
    active = policy && policy->keep_screen_on_duration_ms > 0U;
  }
  if (toggle.kind == ToggleKind::OpenSettings) {
    active = false;
  }

  lv_obj_set_style_bg_color(toggle.button, active ? lv_color_hex(0x1493FF) : lv_color_hex(0x15294A), 0);
  lv_obj_set_style_bg_opa(toggle.button, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(toggle.icon_label, lv_color_hex(0xF5FAFF), 0);
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

namespace {

constexpr float kPi = 3.14159265358979323846f;

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

std::string screen_off_date_text(const TimeModel& time) {
  if (!time.valid) {
    return "--";
  }
  char buffer[48] = {};
  std::snprintf(buffer,
                sizeof(buffer),
                "%u\xE6\x9C\x88%u\xE6\x97\xA5 %s",
                static_cast<unsigned>(time.month),
                static_cast<unsigned>(time.day),
                weekday_text(weekday_index(time)));
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
