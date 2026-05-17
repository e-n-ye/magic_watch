#include "App/UI/Pages/SettingsPages.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "App/UI/UiStyles.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"

namespace twsim::app {

namespace {

constexpr const char* kTextSettings = "\xE8\xAE\xBE\xE7\xBD\xAE";
constexpr const char* kTextDisplay = "\xE6\x98\xBE\xE7\xA4\xBA\xE4\xB8\x8E\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextSound = "\xE5\xA3\xB0\xE9\x9F\xB3\xE4\xB8\x8E\xE6\x8C\xAF\xE5\x8A\xA8";
constexpr const char* kTextDnd = "\xE5\x8B\xBF\xE6\x89\xB0\xE6\xA8\xA1\xE5\xBC\x8F";
constexpr const char* kTextNotifications = "\xE6\xB6\x88\xE6\x81\xAF\xE9\x80\x9A\xE7\x9F\xA5";
constexpr const char* kTextAppLayout = "\xE5\xBA\x94\xE7\x94\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextSystemActions = "\xE7\xB3\xBB\xE7\xBB\x9F\xE6\x93\x8D\xE4\xBD\x9C";
constexpr const char* kTextAbout = "\xE5\x85\xB3\xE4\xBA\x8E";
constexpr const char* kTextComingSoon =
    "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE4\xBF\x9D\xE7\x95\x99\xE9\xA1\xB5\xE9\x9D\xA2\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x8C"
    "\xE5\x90\x8E\xE7\xBB\xAD\xE6\x8C\x89\xE5\xB0\x8F\xE9\x97\xAD\xE7\x8E\xAF\xE7\xBB\xA7\xE7\xBB\xAD\xE8\xA1\xA5\xE9\xBD\x90\xE3\x80\x82";
constexpr const char* kTextPlaceholderTag = "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x8D\xA0\xE4\xBD\x8D";

constexpr const char* kTextBrightness = "\xE5\xB1\x8F\xE5\xB9\x95\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextAutoBrightness = "\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextCustomBrightness = "\xE8\x87\xAA\xE5\xAE\x9A\xE4\xB9\x89\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextScreenTimeout = "\xE6\x81\xAF\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextRaiseToWake = "\xE6\x8A\xAC\xE8\x85\x95\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextKeepScreenOn = "\xE6\x8C\x81\xE7\xBB\xAD\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextTapToWake = "\xE5\x8D\x95\xE5\x87\xBB\xE4\xBA\xAE\xE5\xB1\x8F";
constexpr const char* kTextScheduledOn =
    "\xE5\xAE\x9A\xE6\x97\xB6\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextStart = "\xE5\xBC\x80\xE5\xA7\x8B";
constexpr const char* kTextEnd = "\xE7\xBB\x93\xE6\x9D\x9F";
constexpr const char* kTextStartTime =
    "\xE5\xBC\x80\xE5\xA7\x8B\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextEndTime =
    "\xE7\xBB\x93\xE6\x9D\x9F\xE6\x97\xB6\xE9\x97\xB4";
constexpr const char* kTextConfirm = LV_SYMBOL_OK;
constexpr const char* kTextCancel = LV_SYMBOL_CLOSE;
constexpr const char* kTextPlaceholderBody =
    "\xE6\x98\xBE\xE7\xA4\xBA\xE8\xAF\xA6\xE7\xBB\x86\xE7\xAD\x96\xE7\x95\xA5\xE4\xBC\x9A\xE5\x9C\xA8"
    "\xE5\x90\x8E\xE7\xBB\xAD\xE5\xB0\x8F\xE8\xBD\xAE\xE9\x80\x90\xE6\xAD\xA5\xE8\xA1\xA5\xE9\xBD\x90\xE3\x80\x82";
constexpr const char* kTextAuto = "\xE8\x87\xAA\xE5\x8A\xA8";
constexpr const char* kTextOn = "\xE5\xBC\x80";
constexpr const char* kTextOff = "\xE5\x85\xB3";
constexpr const char* kTextAllDayOn =
    "\xE5\x85\xA8\xE5\xA4\xA9\xE5\xBC\x80\xE5\x90\xAF";
constexpr const char* kTextBrightnessSaveHint =
    "\xE7\xA6\xBB\xE5\xBC\x80\xE9\xA1\xB5\xE9\x9D\xA2\xE6\x97\xB6\xE8\x87\xAA\xE5\x8A\xA8\xE4\xBF\x9D\xE5\xAD\x98";
constexpr const char* kTextKeepScreenOnConfirmBody =
    "\xE5\xBC\x80\xE5\x90\xAF\xE5\x90\x8E\xEF\xBC\x8C\xE4\xBB\x85\xE6\x9C\xAC\xE6\xAC\xA1\xE4\xBA\xAE\xE5\xB1\x8F"
    "\xE6\x97\xB6\xE9\x97\xB4\xE6\x8C\x81\xE7\xBB\xAD%s\xEF\xBC\x8C\xE4\xBA\xAE\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4"
    "\xE8\xBF\x87\xE9\x95\xBF\xEF\xBC\x8C\xE4\xBC\x9A\xE5\x87\x8F\xE5\xB0\x91\xE7\xBB\xAD\xE8\x88\xAA\xE6\x97\xB6"
    "\xE9\x97\xB4\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE4\xBD\xBF\xE7\x94\xA8\xEF\xBC\x9F";

constexpr lv_coord_t kHeaderTop = 13;
constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kListHorizontalInset = 14;
constexpr lv_coord_t kListBottomInset = 8;
constexpr lv_coord_t kTileHeight = 58;
constexpr lv_coord_t kTileRadius = 18;
constexpr lv_coord_t kTileGap = 8;
constexpr lv_coord_t kTileHorizontalPadding = 16;
constexpr lv_coord_t kHeaderBackSize = 40;

constexpr lv_coord_t kClickDragThreshold = 12;

struct TileVisual {
  const char* icon_text;
  std::uint32_t icon_bg;
};

struct TwoLineTileParts {
  lv_obj_t* title {nullptr};
  lv_obj_t* detail {nullptr};
  lv_obj_t* accessory {nullptr};
};

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
    return false;
  }

  const bool allows = !it->second.moved;
  states.erase(it);
  return allows;
}

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
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

const lv_font_t* cjk_font_12() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 12);
  return font != nullptr ? font : &lv_font_montserrat_12;
}

const lv_font_t* cjk_font_16() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 16);
  return font != nullptr ? font : &lv_font_montserrat_16;
}

const lv_font_t* cjk_font_18() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 18);
  return font != nullptr ? font : &lv_font_montserrat_18;
}

const lv_font_t* cjk_font_20() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 20);
  return font != nullptr ? font : &lv_font_montserrat_20;
}

void style_page_root(lv_obj_t* root) {
  ui_prepare_box(root);
  ui_apply_surface(root, SurfaceStyle::Screen);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x02060C), 0);
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
}

lv_coord_t display_width() {
  return static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
}

lv_coord_t display_height() {
  return static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
}

lv_coord_t list_width() {
  return static_cast<lv_coord_t>(std::max<int>(display_width() - kListHorizontalInset * 2, 0));
}

lv_coord_t list_height() {
  return static_cast<lv_coord_t>(std::max<int>(display_height() - kListTop - kListBottomInset, 0));
}

void style_panel(lv_obj_t* panel, lv_color_t bg_color, lv_coord_t radius = kTileRadius) {
  ui_prepare_box(panel);
  lv_obj_set_style_bg_color(panel, bg_color, 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 0, 0);
  lv_obj_set_style_radius(panel, radius, 0);
}

lv_obj_t* create_back_button(lv_obj_t* root, lv_event_cb_t callback, void* user_data) {
  lv_obj_t* button = lv_button_create(root);
  if (button == nullptr) {
    return nullptr;
  }

  ui_prepare_box(button);
  lv_obj_set_size(button, 40, 40);
  lv_obj_align(button, LV_ALIGN_TOP_LEFT, 6, 4);
  lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
  attach_click_guard(button);
  lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, user_data);
  ui_set_touch_target(button, 18);

  lv_obj_t* label = lv_label_create(button);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xDDF6FF), 0);
  lv_label_set_text(label, "<");
  lv_obj_add_flag(label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_move_foreground(button);
  return button;
}

DisplayPolicyModel current_display_policy(DataCenter& data_center) {
  return data_center.display_policy().value_or(DisplayPolicyModel {});
}

const char* timeout_text(std::uint32_t timeout_ms) {
  switch (timeout_ms) {
    case 5000U:
      return "5\xE7\xA7\x92";
    case 10000U:
      return "10\xE7\xA7\x92";
    case 20000U:
      return "20\xE7\xA7\x92";
    case 30000U:
      return "30\xE7\xA7\x92";
    case 60000U:
      return "60\xE7\xA7\x92";
    default:
      return "5\xE7\xA7\x92";
  }
}

std::string brightness_detail_text(const DisplayPolicyModel& policy) {
  if (policy.brightness_mode == BrightnessMode::Auto) {
    return kTextAuto;
  }
  return std::to_string(static_cast<int>(policy.manual_brightness_level)) + "%";
}

const char* keep_screen_on_text(std::uint32_t duration_ms) {
  switch (duration_ms) {
    case 0U:
      return kTextOff;
    case 300000U:
      return "5\xE5\x88\x86\xE9\x92\x9F";
    case 600000U:
      return "10\xE5\x88\x86\xE9\x92\x9F";
    case 900000U:
      return "15\xE5\x88\x86\xE9\x92\x9F";
    case 1200000U:
      return "20\xE5\x88\x86\xE9\x92\x9F";
    default:
      return kTextOff;
  }
}

const char* raise_to_wake_text(const DisplayPolicyModel& policy) {
  switch (policy.raise_to_wake_mode) {
    case RaiseToWakeMode::Off:
      return kTextOff;
    case RaiseToWakeMode::Scheduled:
      return kTextScheduledOn;
    case RaiseToWakeMode::AllDay:
    default:
      return kTextAllDayOn;
  }
}

std::string format_hhmm(std::uint8_t hour, std::uint8_t minute) {
  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%02u:%02u", static_cast<unsigned>(hour), static_cast<unsigned>(minute));
  return buffer;
}

std::string two_digit_options(int max_value) {
  std::string options;
  options.reserve(static_cast<std::size_t>(max_value + 1) * 3);
  char buffer[4] = {};
  for (int value = 0; value <= max_value; ++value) {
    std::snprintf(buffer, sizeof(buffer), "%02d", value);
    if (!options.empty()) {
      options.push_back('\n');
    }
    options.append(buffer);
  }
  return options;
}

void set_time_label_text(lv_obj_t* label, const TimeModel& model) {
  if (label == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(label, "--:--");
    return;
  }

  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%02u:%02u", static_cast<unsigned>(model.hour), static_cast<unsigned>(model.minute));
  lv_label_set_text(label, buffer);
}

TileVisual visual_for_page(PageId page_id) {
  switch (page_id) {
    case PageId::SettingDisplay:
      return {LV_SYMBOL_TINT, 0xF0B323};
    case PageId::SettingSound:
      return {LV_SYMBOL_VOLUME_MAX, 0x22D67C};
    case PageId::SettingDoNotDisturb:
      return {LV_SYMBOL_MUTE, 0x3B82F6};
    case PageId::SettingNotifications:
      return {LV_SYMBOL_BELL, 0x1DA1FF};
    case PageId::SettingAppLayout:
      return {LV_SYMBOL_LIST, 0x3B82F6};
    case PageId::SettingSystemActions:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
    case PageId::SettingAbout:
      return {"i", 0x50A8FF};
    default:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
  }
}

lv_obj_t* create_scroll_column(lv_obj_t* root, lv_coord_t top, lv_coord_t height) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }

  ui_prepare_box(scroll);
  lv_obj_set_size(scroll, list_width(), height);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, top);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_layout(scroll, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(scroll, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(scroll, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_all(scroll, 0, 0);
  lv_obj_set_style_pad_gap(scroll, kTileGap, 0);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_clip_corner(scroll, false, 0);
  return scroll;
}

lv_obj_t* create_card_shell(lv_obj_t* parent, bool interactive = false) {
  lv_obj_t* card = interactive ? lv_button_create(parent) : lv_obj_create(parent);
  if (card == nullptr) {
    return nullptr;
  }

  style_panel(card, lv_color_hex(0x16314C));
  lv_obj_set_width(card, LV_PCT(100));
  lv_obj_set_height(card, kTileHeight);
  if (interactive) {
    attach_click_guard(card);
    ui_set_touch_target(card, 8);
  }
  return card;
}

lv_obj_t* create_switch_track(lv_obj_t* parent) {
  lv_obj_t* track = lv_obj_create(parent);
  if (track == nullptr) {
    return nullptr;
  }

  ui_prepare_box(track);
  lv_obj_set_size(track, 48, 26);
  lv_obj_align(track, LV_ALIGN_RIGHT_MID, -14, 0);
  lv_obj_set_style_radius(track, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(track, LV_OPA_COVER, 0);
  lv_obj_add_flag(track, LV_OBJ_FLAG_EVENT_BUBBLE);

  lv_obj_t* knob = lv_obj_create(track);
  if (knob == nullptr) {
    return nullptr;
  }
  ui_prepare_box(knob);
  lv_obj_set_size(knob, 18, 18);
  lv_obj_set_style_radius(knob, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(knob, lv_color_hex(0xD8F9FF), 0);
  lv_obj_set_style_bg_opa(knob, LV_OPA_COVER, 0);
  lv_obj_add_flag(knob, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_align(knob, LV_ALIGN_LEFT_MID, 4, 0);
  return track;
}

void apply_switch_visual(lv_obj_t* track, bool enabled) {
  if (track == nullptr) {
    return;
  }
  lv_obj_set_style_bg_color(track, enabled ? lv_color_hex(0x11B8FF) : lv_color_hex(0x5F8CB7), 0);
  lv_obj_set_style_bg_opa(track, enabled ? LV_OPA_COVER : LV_OPA_60, 0);
  lv_obj_t* knob = lv_obj_get_child(track, 0);
  if (knob != nullptr) {
    lv_obj_align(knob, enabled ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, enabled ? -4 : 4, 0);
  }
}

lv_obj_t* create_selection_dot(lv_obj_t* parent) {
  lv_obj_t* dot = lv_obj_create(parent);
  if (dot == nullptr) {
    return nullptr;
  }

  ui_prepare_box(dot);
  lv_obj_set_size(dot, 24, 24);
  lv_obj_align(dot, LV_ALIGN_RIGHT_MID, -16, 0);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_add_flag(dot, LV_OBJ_FLAG_EVENT_BUBBLE);

  lv_obj_t* inner = lv_obj_create(dot);
  if (inner == nullptr) {
    return nullptr;
  }
  ui_prepare_box(inner);
  lv_obj_set_size(inner, 12, 12);
  lv_obj_set_style_radius(inner, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(inner, lv_color_hex(0xD8F9FF), 0);
  lv_obj_set_style_bg_opa(inner, LV_OPA_COVER, 0);
  lv_obj_add_flag(inner, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_center(inner);
  return dot;
}

void apply_selection_dot(lv_obj_t* dot, bool selected) {
  if (dot == nullptr) {
    return;
  }
  lv_obj_set_style_bg_color(dot, selected ? lv_color_hex(0x11B8FF) : lv_color_hex(0x77A5CE), 0);
  lv_obj_set_style_bg_opa(dot, selected ? LV_OPA_COVER : LV_OPA_30, 0);
  lv_obj_t* inner = lv_obj_get_child(dot, 0);
  if (inner != nullptr) {
    lv_obj_set_style_bg_opa(inner, selected ? LV_OPA_COVER : LV_OPA_20, 0);
  }
}

TwoLineTileParts create_two_line_content(lv_obj_t* card, const char* title_text, const char* detail_text) {
  TwoLineTileParts parts;
  if (card == nullptr) {
    return parts;
  }

  parts.title = lv_label_create(card);
  parts.detail = lv_label_create(card);
  if (parts.title == nullptr || parts.detail == nullptr) {
    return {};
  }

  ui_prepare_label(parts.title);
  lv_obj_set_style_text_font(parts.title, cjk_font_20(), 0);
  lv_obj_set_style_text_color(parts.title, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(parts.title, title_text);
  lv_obj_align(parts.title, LV_ALIGN_TOP_LEFT, kTileHorizontalPadding, 8);

  ui_prepare_label(parts.detail);
  lv_obj_set_style_text_font(parts.detail, cjk_font_18(), 0);
  lv_obj_set_style_text_color(parts.detail, lv_color_hex(0xE5F3FF), 0);
  lv_label_set_text(parts.detail, detail_text);
  lv_obj_align(parts.detail, LV_ALIGN_BOTTOM_LEFT, kTileHorizontalPadding, -7);
  return parts;
}

lv_obj_t* create_home_tile_icon(lv_obj_t* parent, const char* icon_text, std::uint32_t icon_bg) {
  lv_obj_t* icon = lv_obj_create(parent);
  if (icon == nullptr) {
    return nullptr;
  }

  ui_prepare_box(icon);
  lv_obj_set_size(icon, 34, 34);
  lv_obj_align(icon, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(icon_bg), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);

  lv_obj_t* icon_label = lv_label_create(icon);
  if (icon_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(icon_label);
  lv_obj_set_style_text_font(icon_label,
                             icon_text != nullptr && icon_text[0] == 'i' && icon_text[1] == '\0'
                                 ? &lv_font_montserrat_20
                                 : &lv_font_montserrat_18,
                             0);
  lv_obj_set_style_text_color(icon_label, lv_color_hex(0xF8FBFF), 0);
  lv_label_set_text(icon_label, icon_text);
  lv_obj_center(icon_label);
  return icon;
}

lv_obj_t* create_note_card(lv_obj_t* parent, const char* title_text, const char* body_text) {
  lv_obj_t* card = lv_obj_create(parent);
  if (card == nullptr) {
    return nullptr;
  }

  style_panel(card, lv_color_hex(0x09131E), 18);
  lv_obj_set_width(card, LV_PCT(100));
  lv_obj_set_height(card, LV_SIZE_CONTENT);
  ui_set_flex_column(card, 12, 6);

  lv_obj_t* title = lv_label_create(card);
  lv_obj_t* body = lv_label_create(card);
  if (title == nullptr || body == nullptr) {
    return nullptr;
  }

  ui_prepare_label(title);
  lv_obj_set_style_text_font(title, cjk_font_12(), 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0x78AFFF), 0);
  lv_label_set_text(title, title_text);

  ui_prepare_label(body);
  lv_obj_set_width(body, 184);
  lv_obj_set_style_text_font(body, cjk_font_16(), 0);
  lv_obj_set_style_text_color(body, lv_color_hex(0xE1F0FF), 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(body, body_text);
  return card;
}

}  // namespace

SettingsPageBase::SettingsPageBase(DataCenter& data_center, PageId page_id, const char* title, bool show_back_button)
    : PageBase(data_center), page_id_(page_id), title_(title), show_back_button_(show_back_button) {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 const auto* model = std::get_if<TimeModel>(&event.payload);
                                 if (model != nullptr) {
                                   apply_time(*model);
                                 }
                               }));
}

PageId SettingsPageBase::id() const {
  return page_id_;
}

const char* SettingsPageBase::name() const {
  return page_name(page_id_);
}

lv_obj_t* SettingsPageBase::create_page_root() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root != nullptr) {
    style_page_root(root);
    lv_obj_set_size(root, display_width(), display_height());
  }
  return root;
}

void SettingsPageBase::build_header(lv_obj_t* root) {
  if (root == nullptr) {
    return;
  }

  if (show_back_button_ && create_back_button(root, &SettingsPageBase::back_event_cb, this) == nullptr) {
    return;
  }

  lv_obj_t* title = lv_label_create(root);
  time_label_ = lv_label_create(root);
  if (title == nullptr || time_label_ == nullptr) {
    return;
  }

  ui_prepare_label(title);
  lv_obj_set_style_text_font(title, cjk_font_20(), 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(title, title_);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, show_back_button_ ? 40 : 18, kHeaderTop);

  ui_prepare_label(time_label_);
  lv_obj_set_style_text_font(time_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xF7FBFF), 0);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, kHeaderTop);

  if (const auto& time = data_center_.time(); time) {
    apply_time(*time);
  } else {
    lv_label_set_text(time_label_, "--:--");
  }
}

void SettingsPageBase::apply_time(const TimeModel& model) {
  set_time_label_text(time_label_, model);
}

const char* SettingsPageBase::title_text() const {
  return title_;
}

void SettingsPageBase::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsPageBase*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

SettingsHomePage::SettingsHomePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingsHome, kTextSettings, false),
      entries_ {{
          {LV_SYMBOL_TINT, 0xF0B323, kTextDisplay, PageId::SettingDisplay},
          {LV_SYMBOL_VOLUME_MAX, 0x22D67C, kTextSound, PageId::SettingSound},
          {LV_SYMBOL_MUTE, 0x3B82F6, kTextDnd, PageId::SettingDoNotDisturb},
          {LV_SYMBOL_BELL, 0x1DA1FF, kTextNotifications, PageId::SettingNotifications},
          {LV_SYMBOL_LIST, 0x3B82F6, kTextAppLayout, PageId::SettingAppLayout},
          {LV_SYMBOL_SETTINGS, 0x2AA8FF, kTextSystemActions, PageId::SettingSystemActions},
          {"i", 0x50A8FF, kTextAbout, PageId::SettingAbout},
      }} {}

lv_obj_t* SettingsHomePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < entries_.size(); ++index) {
    const auto& entry = entries_[index];
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_add_event_cb(button, &SettingsHomePage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    if (create_home_tile_icon(button, entry.icon_text, entry.icon_bg) == nullptr) {
      return nullptr;
    }

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, entry.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);
  }

  return root;
}

void SettingsHomePage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsHomePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* button = lv_event_get_current_target_obj(event);
  if (button == nullptr || !click_guard_allows(button)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(button)));
  if (index >= self->entries_.size()) {
    return;
  }

  self->request_navigation({NavigationAction::Push, self->entries_[index].target});
}

SettingsPlaceholderPage::SettingsPlaceholderPage(DataCenter& data_center,
                                                 PageId page_id,
                                                 const char* title,
                                                 const char* detail)
    : SettingsPageBase(data_center, page_id, title, true), detail_(detail) {}

lv_obj_t* SettingsPlaceholderPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  const TileVisual visual = visual_for_page(id());
  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* card = create_card_shell(scroll, false);
  if (card == nullptr) {
    return nullptr;
  }
  if (create_home_tile_icon(card, visual.icon_text, visual.icon_bg) == nullptr) {
    return nullptr;
  }

  lv_obj_t* label = lv_label_create(card);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(label, title_text());
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);

  if (create_note_card(scroll, kTextPlaceholderTag, detail_) == nullptr) {
    return nullptr;
  }
  if (create_note_card(scroll, kTextSettings, kTextComingSoon) == nullptr) {
    return nullptr;
  }

  return root;
}

DisplaySettingsPage::DisplaySettingsPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplay, kTextDisplay, true),
      rows_ {{
          {RowAction::OpenBrightness, kTextBrightness, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenScreenTimeout, kTextScreenTimeout, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenRaiseToWake, kTextRaiseToWake, nullptr, nullptr, nullptr, nullptr},
          {RowAction::OpenKeepScreenOn, kTextKeepScreenOn, nullptr, nullptr, nullptr, nullptr},
          {RowAction::ToggleTapToWake, kTextTapToWake, nullptr, nullptr, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_from_policy(); }));
}

void DisplaySettingsPage::on_will_appear() {
  refresh_from_policy();
}

lv_obj_t* DisplaySettingsPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  auto build_detail_row = [&](std::size_t index, const char* title, const char* detail) -> bool {
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return false;
    }
    rows_[index].button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &DisplaySettingsPage::row_event_cb, LV_EVENT_CLICKED, this);
    const auto parts = create_two_line_content(button, title, detail);
    rows_[index].title_label = parts.title;
    rows_[index].detail_label = parts.detail;
    return parts.title != nullptr && parts.detail != nullptr;
  };

  auto build_switch_row = [&](std::size_t index, const char* title) -> bool {
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return false;
    }
    rows_[index].button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &DisplaySettingsPage::row_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return false;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
    rows_[index].title_label = label;

    rows_[index].switch_track = create_switch_track(button);
    return rows_[index].switch_track != nullptr;
  };

  if (!build_detail_row(0, kTextBrightness, kTextAuto)) {
    return nullptr;
  }
  if (!build_detail_row(1, kTextScreenTimeout, "5\xE7\xA7\x92")) {
    return nullptr;
  }
  if (!build_detail_row(2, kTextRaiseToWake, kTextOff)) {
    return nullptr;
  }
  if (!build_detail_row(3, kTextKeepScreenOn, kTextOff)) {
    return nullptr;
  }
  if (!build_switch_row(4, kTextTapToWake)) {
    return nullptr;
  }

  if ((note_body_ = create_note_card(scroll, kTextSettings, kTextPlaceholderBody)) == nullptr) {
    return nullptr;
  }

  refresh_from_policy();
  return root;
}

void DisplaySettingsPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplaySettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->rows_.size()) {
    return;
  }

  const auto policy = current_display_policy(self->data_center_);
  switch (self->rows_[index].action) {
    case RowAction::OpenBrightness:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayBrightness});
      break;
    case RowAction::OpenScreenTimeout:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayTimeout});
      break;
    case RowAction::OpenRaiseToWake:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWake});
      break;
    case RowAction::OpenKeepScreenOn:
      self->request_navigation({NavigationAction::Push, PageId::SettingDisplayKeepScreenOn});
      break;
    case RowAction::ToggleTapToWake:
      self->data_center_.set_tap_to_wake_enabled(!policy.tap_to_wake_enabled);
      break;
  }
}

void DisplaySettingsPage::refresh_from_policy() {
  if (rows_[0].detail_label == nullptr) {
    return;
  }

  const auto policy = current_display_policy(data_center_);
  lv_label_set_text(rows_[0].detail_label, brightness_detail_text(policy).c_str());
  lv_label_set_text(rows_[1].detail_label, timeout_text(policy.screen_off_timeout_ms));
  lv_label_set_text(rows_[2].detail_label, raise_to_wake_text(policy));
  lv_label_set_text(rows_[3].detail_label, keep_screen_on_text(policy.keep_screen_on_duration_ms));
  apply_switch_visual(rows_[4].switch_track, policy.tap_to_wake_enabled);
}

DisplayBrightnessPage::DisplayBrightnessPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayBrightness, kTextBrightness, true) {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_from_policy(); }));
}

void DisplayBrightnessPage::on_will_appear() {
  refresh_from_policy();
}

lv_obj_t* DisplayBrightnessPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* auto_row = create_card_shell(scroll, true);
  if (auto_row == nullptr) {
    return nullptr;
  }
  lv_obj_add_event_cb(auto_row, &DisplayBrightnessPage::row_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_set_user_data(auto_row, reinterpret_cast<void*>(0));
  lv_obj_t* auto_label = lv_label_create(auto_row);
  if (auto_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(auto_label);
  lv_obj_set_style_text_font(auto_label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(auto_label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(auto_label, kTextAutoBrightness);
  lv_obj_align(auto_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
  auto_switch_track_ = create_switch_track(auto_row);
  if (auto_switch_track_ == nullptr) {
    return nullptr;
  }

  manual_row_ = create_card_shell(scroll, true);
  if (manual_row_ == nullptr) {
    return nullptr;
  }
  lv_obj_add_event_cb(manual_row_, &DisplayBrightnessPage::row_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_set_user_data(manual_row_, reinterpret_cast<void*>(1));
  const auto manual_parts = create_two_line_content(manual_row_, kTextCustomBrightness, "60%");
  manual_detail_label_ = manual_parts.detail;
  if (manual_parts.title == nullptr || manual_detail_label_ == nullptr) {
    return nullptr;
  }

  refresh_from_policy();
  return root;
}

void DisplayBrightnessPage::row_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayBrightnessPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto row = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  const auto policy = current_display_policy(self->data_center_);
  if (row == 0) {
    self->data_center_.set_brightness_mode(policy.brightness_mode == BrightnessMode::Auto ? BrightnessMode::Manual
                                                                                           : BrightnessMode::Auto);
    return;
  }
  if (row == 1 && policy.brightness_mode == BrightnessMode::Manual) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayManualBrightness});
  }
}

void DisplayBrightnessPage::refresh_from_policy() {
  const auto policy = current_display_policy(data_center_);
  apply_switch_visual(auto_switch_track_, policy.brightness_mode == BrightnessMode::Auto);
  if (manual_row_ != nullptr) {
    const bool hidden = policy.brightness_mode == BrightnessMode::Auto;
    if (hidden) {
      lv_obj_add_flag(manual_row_, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(manual_row_, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (manual_detail_label_ != nullptr) {
    lv_label_set_text(manual_detail_label_, brightness_detail_text(policy).c_str());
  }
}

DisplayManualBrightnessPage::DisplayManualBrightnessPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayManualBrightness, kTextCustomBrightness, true) {}

void DisplayManualBrightnessPage::on_will_appear() {
  reload_from_policy();
}

void DisplayManualBrightnessPage::on_will_disappear() {
  if (!dirty_) {
    return;
  }
  data_center_.set_manual_brightness_level(pending_level_);
  dirty_ = false;
}

lv_obj_t* DisplayManualBrightnessPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  slider_ = lv_slider_create(root);
  if (slider_ == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(slider_, 48, 176);
  lv_obj_center(slider_);
  lv_slider_set_range(slider_, 0, 100);
  lv_obj_add_event_cb(slider_, &DisplayManualBrightnessPage::slider_event_cb, LV_EVENT_VALUE_CHANGED, this);
  lv_obj_set_style_radius(slider_, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x6F8FB4), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_radius(slider_, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x16E3FF), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider_, lv_color_hex(0x16E3FF), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(slider_, LV_OPA_TRANSP, LV_PART_KNOB);
  lv_obj_set_style_pad_all(slider_, 0, LV_PART_KNOB);

  value_label_ = lv_label_create(root);
  if (value_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_label(value_label_);
  lv_obj_set_style_text_font(value_label_, cjk_font_18(), 0);
  lv_obj_set_style_text_color(value_label_, lv_color_hex(0xDFFBFF), 0);
  lv_obj_align_to(value_label_, slider_, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  lv_obj_t* hint = lv_label_create(root);
  if (hint == nullptr) {
    return nullptr;
  }
  ui_prepare_label(hint);
  lv_obj_set_style_text_font(hint, cjk_font_16(), 0);
  lv_obj_set_style_text_color(hint, lv_color_hex(0xD8ECFF), 0);
  lv_label_set_text(hint, kTextBrightnessSaveHint);
  lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -18);

  reload_from_policy();
  return root;
}

void DisplayManualBrightnessPage::slider_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayManualBrightnessPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->slider_ == nullptr) {
    return;
  }

  self->pending_level_ = static_cast<std::uint8_t>(lv_slider_get_value(self->slider_));
  self->dirty_ = true;
  self->apply_preview_value(self->pending_level_);
}

void DisplayManualBrightnessPage::reload_from_policy() {
  const auto policy = current_display_policy(data_center_);
  pending_level_ = policy.manual_brightness_level;
  if (slider_ != nullptr) {
    lv_slider_set_value(slider_, pending_level_, LV_ANIM_OFF);
  }
  apply_preview_value(pending_level_);
  dirty_ = false;
}

void DisplayManualBrightnessPage::apply_preview_value(std::uint8_t level) {
  if (value_label_ == nullptr) {
    return;
  }
  char buffer[16] = {};
  std::snprintf(buffer, sizeof(buffer), "%u%%", static_cast<unsigned>(level));
  lv_label_set_text(value_label_, buffer);
}

DisplayTimeoutPage::DisplayTimeoutPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayTimeout, kTextScreenTimeout, true),
      options_ {{
          {5000U, "5\xE7\xA7\x92", nullptr, nullptr},
          {10000U, "10\xE7\xA7\x92", nullptr, nullptr},
          {20000U, "20\xE7\xA7\x92", nullptr, nullptr},
          {30000U, "30\xE7\xA7\x92", nullptr, nullptr},
          {60000U, "60\xE7\xA7\x92", nullptr, nullptr},
      }} {}

void DisplayTimeoutPage::on_will_appear() {
  refresh_selection();
}

lv_obj_t* DisplayTimeoutPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.button = create_card_shell(scroll, true);
    if (option.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_user_data(option.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(option.button, &DisplayTimeoutPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(option.button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, option.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(option.button);
    if (option.check_label == nullptr) {
      return nullptr;
    }
  }

  refresh_selection();
  return root;
}

void DisplayTimeoutPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayTimeoutPage*>(lv_event_get_user_data(event));
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

  self->data_center_.set_screen_off_timeout_ms(self->options_[index].timeout_ms);
  self->refresh_selection();
}

void DisplayTimeoutPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }
    const bool selected = option.timeout_ms == policy.screen_off_timeout_ms;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);
  }
}

DisplayRaiseToWakePage::DisplayRaiseToWakePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayRaiseToWake, kTextRaiseToWake, true),
      options_ {{
          {RaiseToWakeMode::Off, kTextOff, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {RaiseToWakeMode::AllDay, kTextAllDayOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
          {RaiseToWakeMode::Scheduled, kTextScheduledOn, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void DisplayRaiseToWakePage::on_will_appear() {
  refresh_selection();
}

lv_obj_t* DisplayRaiseToWakePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.button = create_card_shell(scroll, true);
    if (option.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_user_data(option.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(option.button, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);

    option.title_label = lv_label_create(option.button);
    if (option.title_label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(option.title_label);
    lv_obj_set_style_text_font(option.title_label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(option.title_label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(option.title_label, option.label);
    lv_obj_align(option.title_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(option.button);
    if (option.check_label == nullptr) {
      return nullptr;
    }

    if (option.mode == RaiseToWakeMode::Scheduled) {
      option.start_row = lv_button_create(option.button);
      option.end_row = lv_button_create(option.button);
      if (option.start_row == nullptr || option.end_row == nullptr) {
        return nullptr;
      }
      for (lv_obj_t* row : {option.start_row, option.end_row}) {
        ui_prepare_box(row);
        lv_obj_set_size(row, 172, 54);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_radius(row, 12, 0);
        lv_obj_add_flag(row, LV_OBJ_FLAG_EVENT_BUBBLE);
        attach_click_guard(row);
      }
      lv_obj_align(option.start_row, LV_ALIGN_TOP_LEFT, 8, 44);
      lv_obj_align(option.end_row, LV_ALIGN_TOP_LEFT, 8, 112);
      lv_obj_set_user_data(option.start_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(100)));
      lv_obj_set_user_data(option.end_row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(101)));
      lv_obj_add_event_cb(option.start_row, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_add_event_cb(option.end_row, &DisplayRaiseToWakePage::option_event_cb, LV_EVENT_CLICKED, this);

      option.start_title_label = lv_label_create(option.start_row);
      option.start_value_label = lv_label_create(option.start_row);
      option.end_title_label = lv_label_create(option.end_row);
      option.end_value_label = lv_label_create(option.end_row);
      if (option.start_title_label == nullptr || option.start_value_label == nullptr ||
          option.end_title_label == nullptr || option.end_value_label == nullptr) {
        return nullptr;
      }

      for (lv_obj_t* label : {option.start_title_label, option.end_title_label}) {
        ui_prepare_label(label);
        lv_obj_set_style_text_font(label, cjk_font_20(), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
      }
      for (lv_obj_t* label : {option.start_value_label, option.end_value_label}) {
        ui_prepare_label(label);
        lv_obj_set_style_text_font(label, cjk_font_18(), 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xE5F3FF), 0);
      }

      lv_label_set_text(option.start_title_label, kTextStart);
      lv_label_set_text(option.end_title_label, kTextEnd);
      lv_obj_align(option.start_title_label, LV_ALIGN_TOP_LEFT, 8, 0);
      lv_obj_align(option.start_value_label, LV_ALIGN_BOTTOM_LEFT, 8, 0);
      lv_obj_align(option.end_title_label, LV_ALIGN_TOP_LEFT, 8, 0);
      lv_obj_align(option.end_value_label, LV_ALIGN_BOTTOM_LEFT, 8, 0);
    }
  }

  refresh_selection();
  return root;
}

void DisplayRaiseToWakePage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index == 100U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWakeStartTime});
    return;
  }
  if (index == 101U) {
    self->request_navigation({NavigationAction::Push, PageId::SettingDisplayRaiseToWakeEndTime});
    return;
  }
  if (index < self->options_.size()) {
    self->data_center_.set_raise_to_wake_mode(self->options_[index].mode);
    self->refresh_selection();
  }
}

void DisplayRaiseToWakePage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  const auto& window = policy.raise_to_wake_window;
  const std::string start_text = format_hhmm(window.start_hour, window.start_minute);
  const std::string end_text = format_hhmm(window.end_hour, window.end_minute);

  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }

    const bool selected = option.mode == policy.raise_to_wake_mode;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);

    if (option.mode != RaiseToWakeMode::Scheduled) {
      continue;
    }

    const bool expanded = selected;
    lv_obj_set_height(option.button, expanded ? 182 : kTileHeight);
    if (expanded) {
      lv_obj_align(option.title_label, LV_ALIGN_TOP_LEFT, kTileHorizontalPadding, 12);
      lv_obj_clear_flag(option.start_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.start_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.start_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_clear_flag(option.end_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_label_set_text(option.start_value_label, start_text.c_str());
      lv_label_set_text(option.end_value_label, end_text.c_str());
    } else {
      lv_obj_align(option.title_label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
      lv_obj_add_flag(option.start_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.start_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.start_value_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_row, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_title_label, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(option.end_value_label, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

DisplayKeepScreenOnPage::DisplayKeepScreenOnPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingDisplayKeepScreenOn, kTextKeepScreenOn, true),
      options_ {{
          {0U, kTextOff, nullptr, nullptr},
          {300000U, "5\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {600000U, "10\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {900000U, "15\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
          {1200000U, "20\xE5\x88\x86\xE9\x92\x9F", nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void DisplayKeepScreenOnPage::on_will_appear() {
  hide_confirm_overlay();
  refresh_selection();
}

void DisplayKeepScreenOnPage::on_will_disappear() {
  hide_confirm_overlay();
}

lv_obj_t* DisplayKeepScreenOnPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    option.button = create_card_shell(scroll, true);
    if (option.button == nullptr) {
      return nullptr;
    }
    lv_obj_set_user_data(option.button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(option.button, &DisplayKeepScreenOnPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(option.button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, option.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(option.button);
    if (option.check_label == nullptr) {
      return nullptr;
    }
  }

  overlay_ = lv_obj_create(root);
  if (overlay_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(overlay_);
  lv_obj_set_size(overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_center(overlay_);
  lv_obj_set_style_bg_color(overlay_, lv_color_hex(0x02060C), 0);
  lv_obj_set_style_bg_opa(overlay_, LV_OPA_90, 0);
  lv_obj_set_style_border_width(overlay_, 0, 0);
  lv_obj_set_style_radius(overlay_, 0, 0);
  lv_obj_add_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(overlay_);

  lv_obj_t* body = lv_label_create(overlay_);
  if (body == nullptr) {
    return nullptr;
  }
  ui_prepare_label(body);
  lv_obj_set_width(body, 186);
  lv_obj_set_style_text_font(body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(body, lv_color_hex(0xF6FAFF), 0);
  lv_obj_set_style_text_align(body, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_obj_align(body, LV_ALIGN_CENTER, 0, -28);
  lv_obj_set_user_data(overlay_, body);

  lv_obj_t* cancel_button = lv_button_create(overlay_);
  lv_obj_t* confirm_button = lv_button_create(overlay_);
  if (cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }
  for (lv_obj_t* button : {cancel_button, confirm_button}) {
    ui_prepare_box(button);
    lv_obj_set_size(button, 84, 50);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 18, 0);
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &DisplayKeepScreenOnPage::confirm_event_cb, LV_EVENT_CLICKED, this);
  }
  lv_obj_align(cancel_button, LV_ALIGN_BOTTOM_LEFT, 24, -24);
  lv_obj_align(confirm_button, LV_ALIGN_BOTTOM_RIGHT, -24, -24);
  lv_obj_set_style_bg_color(cancel_button, lv_color_hex(0x6B88A7), 0);
  lv_obj_set_style_bg_opa(cancel_button, LV_OPA_80, 0);
  lv_obj_set_style_bg_color(confirm_button, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button, LV_OPA_COVER, 0);
  lv_obj_set_user_data(cancel_button, reinterpret_cast<void*>(0U));
  lv_obj_set_user_data(confirm_button, reinterpret_cast<void*>(1U));

  for (const auto [button, text] : {std::pair {cancel_button, kTextCancel}, std::pair {confirm_button, kTextConfirm}}) {
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE2FCFF), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }

  refresh_selection();
  return root;
}

void DisplayKeepScreenOnPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayKeepScreenOnPage*>(lv_event_get_user_data(event));
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

  const auto duration_ms = self->options_[index].duration_ms;
  if (duration_ms == 0U) {
    self->hide_confirm_overlay();
    self->data_center_.set_keep_screen_on_duration_ms(0U);
    self->refresh_selection();
    return;
  }

  self->show_confirm_overlay(duration_ms);
}

void DisplayKeepScreenOnPage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayKeepScreenOnPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  if (action == 1U) {
    self->apply_pending_duration();
    return;
  }
  self->hide_confirm_overlay();
}

void DisplayKeepScreenOnPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    if (option.button == nullptr) {
      continue;
    }
    const bool selected = option.duration_ms == policy.keep_screen_on_duration_ms;
    lv_obj_set_style_bg_color(option.button, selected ? lv_color_hex(0x1B4D74) : lv_color_hex(0x16314C), 0);
    apply_selection_dot(option.check_label, selected);
  }
}

void DisplayKeepScreenOnPage::show_confirm_overlay(std::uint32_t duration_ms) {
  if (overlay_ == nullptr) {
    return;
  }
  pending_duration_ms_ = duration_ms;
  auto* body = static_cast<lv_obj_t*>(lv_obj_get_user_data(overlay_));
  if (body != nullptr) {
    char buffer[256] = {};
    std::snprintf(buffer, sizeof(buffer), kTextKeepScreenOnConfirmBody, keep_screen_on_text(duration_ms));
    lv_label_set_text(body, buffer);
  }
  lv_obj_clear_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(overlay_);
}

void DisplayKeepScreenOnPage::hide_confirm_overlay() {
  pending_duration_ms_ = 0U;
  if (overlay_ != nullptr) {
    lv_obj_add_flag(overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void DisplayKeepScreenOnPage::apply_pending_duration() {
  if (pending_duration_ms_ == 0U) {
    hide_confirm_overlay();
    return;
  }
  data_center_.set_keep_screen_on_duration_ms(pending_duration_ms_);
  hide_confirm_overlay();
  refresh_selection();
}

DisplayRaiseToWakeTimePage::DisplayRaiseToWakeTimePage(DataCenter& data_center,
                                                       PageId page_id,
                                                       const char* title,
                                                       bool edit_start)
    : SettingsPageBase(data_center, page_id, title, true), edit_start_(edit_start) {}

void DisplayRaiseToWakeTimePage::on_will_appear() {
  reload_from_policy();
}

lv_obj_t* DisplayRaiseToWakeTimePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  static const std::string kHourOptions = two_digit_options(23);
  static const std::string kMinuteOptions = two_digit_options(59);

  hour_roller_ = lv_roller_create(root);
  minute_roller_ = lv_roller_create(root);
  if (hour_roller_ == nullptr || minute_roller_ == nullptr) {
    return nullptr;
  }

  for (lv_obj_t* roller : {hour_roller_, minute_roller_}) {
    lv_obj_set_size(roller, 74, 162);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_style_bg_opa(roller, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller, 0, LV_PART_MAIN);
    lv_obj_set_style_text_color(roller, lv_color_hex(0x7FCBFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(roller, cjk_font_20(), LV_PART_MAIN);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roller, LV_OPA_20, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(roller, lv_color_hex(0x0C1725), LV_PART_SELECTED);
    lv_obj_set_style_text_color(roller, lv_color_hex(0xBDF7FF), LV_PART_SELECTED);
    lv_obj_set_style_radius(roller, 18, LV_PART_SELECTED);
    lv_obj_add_event_cb(roller, &DisplayRaiseToWakeTimePage::roller_event_cb, LV_EVENT_VALUE_CHANGED, this);
  }

  lv_roller_set_options(hour_roller_, kHourOptions.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_roller_set_options(minute_roller_, kMinuteOptions.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_obj_align(hour_roller_, LV_ALIGN_CENTER, -48, -6);
  lv_obj_align(minute_roller_, LV_ALIGN_CENTER, 48, -6);

  lv_obj_t* colon = lv_label_create(root);
  if (colon == nullptr) {
    return nullptr;
  }
  ui_prepare_label(colon);
  lv_obj_set_style_text_font(colon, cjk_font_20(), 0);
  lv_obj_set_style_text_color(colon, lv_color_hex(0xBDF7FF), 0);
  lv_label_set_text(colon, ":");
  lv_obj_align(colon, LV_ALIGN_CENTER, 0, -6);

  confirm_button_ = lv_button_create(root);
  if (confirm_button_ == nullptr) {
    return nullptr;
  }
  attach_click_guard(confirm_button_);
  ui_prepare_box(confirm_button_);
  lv_obj_set_size(confirm_button_, 98, 44);
  lv_obj_align(confirm_button_, LV_ALIGN_BOTTOM_MID, 0, -22);
  lv_obj_set_style_bg_color(confirm_button_, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(confirm_button_, 0, 0);
  lv_obj_set_style_radius(confirm_button_, 16, 0);
  lv_obj_add_event_cb(confirm_button_, &DisplayRaiseToWakeTimePage::confirm_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* confirm_label = lv_label_create(confirm_button_);
  if (confirm_label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(confirm_label);
  lv_obj_set_style_text_font(confirm_label, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(confirm_label, lv_color_hex(0xD8FFF7), 0);
  lv_label_set_text(confirm_label, kTextConfirm);
  lv_obj_center(confirm_label);

  reload_from_policy();
  return root;
}

void DisplayRaiseToWakeTimePage::roller_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakeTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->hour_roller_ == nullptr || self->minute_roller_ == nullptr) {
    return;
  }
  self->pending_hour_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->hour_roller_));
  self->pending_minute_ = static_cast<std::uint8_t>(lv_roller_get_selected(self->minute_roller_));
  self->update_preview();
}

void DisplayRaiseToWakeTimePage::confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<DisplayRaiseToWakeTimePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->save_and_exit();
}

void DisplayRaiseToWakeTimePage::reload_from_policy() {
  const auto policy = current_display_policy(data_center_);
  pending_hour_ = edit_start_ ? policy.raise_to_wake_window.start_hour : policy.raise_to_wake_window.end_hour;
  pending_minute_ = edit_start_ ? policy.raise_to_wake_window.start_minute : policy.raise_to_wake_window.end_minute;
  if (hour_roller_ != nullptr) {
    lv_roller_set_selected(hour_roller_, pending_hour_, LV_ANIM_OFF);
  }
  if (minute_roller_ != nullptr) {
    lv_roller_set_selected(minute_roller_, pending_minute_, LV_ANIM_OFF);
  }
  update_preview();
}

void DisplayRaiseToWakeTimePage::update_preview() {}

void DisplayRaiseToWakeTimePage::save_and_exit() {
  auto policy = current_display_policy(data_center_);
  auto window = policy.raise_to_wake_window;
  if (edit_start_) {
    window.start_hour = pending_hour_;
    window.start_minute = pending_minute_;
  } else {
    window.end_hour = pending_hour_;
    window.end_minute = pending_minute_;
  }
  data_center_.set_raise_to_wake_window(window);
  request_navigation({NavigationAction::Pop, PageId::Watchface});
}

}  // namespace twsim::app
