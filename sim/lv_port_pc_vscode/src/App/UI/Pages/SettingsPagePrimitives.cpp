#include "App/UI/Pages/SettingsPagePrimitives.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "App/UI/UiStyles.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"

namespace twsim::app {

namespace {

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kListHorizontalInset = 14;
constexpr lv_coord_t kListBottomInset = 8;
constexpr lv_coord_t kTileHeight = 58;
constexpr lv_coord_t kTileRadius = 18;
constexpr lv_coord_t kTileGap = 8;
constexpr lv_coord_t kTileHorizontalPadding = 16;
constexpr lv_coord_t kClickDragThreshold = 12;

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

}  // namespace

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

void style_panel(lv_obj_t* panel, lv_color_t bg_color, lv_coord_t radius) {
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

lv_obj_t* create_card_shell(lv_obj_t* parent, bool interactive) {
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

lv_obj_t* create_plain_text(lv_obj_t* parent, const char* text, const lv_font_t* font, std::uint32_t color) {
  lv_obj_t* label = lv_label_create(parent);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_width(label, LV_PCT(100));
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_label_set_text(label, text);
  return label;
}

lv_obj_t* create_single_line_card(lv_obj_t* parent, const char* title, bool interactive) {
  lv_obj_t* card = create_card_shell(parent, interactive);
  if (card == nullptr) {
    return nullptr;
  }

  lv_obj_t* label = lv_label_create(card);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(label, title);
  lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);
  return card;
}

}  // namespace twsim::app
