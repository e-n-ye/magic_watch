#pragma once

#include <cstdint>

#include "App/Common/AppEvents.h"
#include "lvgl/lvgl.h"

namespace twsim::app {

struct TileVisual {
  const char* icon_text;
  std::uint32_t icon_bg;
};

struct TwoLineTileParts {
  lv_obj_t* title {nullptr};
  lv_obj_t* detail {nullptr};
  lv_obj_t* accessory {nullptr};
};

void attach_click_guard(lv_obj_t* object);
bool click_guard_allows(lv_obj_t* object);

const lv_font_t* cjk_font_12();
const lv_font_t* cjk_font_16();
const lv_font_t* cjk_font_18();
const lv_font_t* cjk_font_20();

void style_page_root(lv_obj_t* root);
lv_coord_t display_width();
lv_coord_t display_height();
lv_coord_t list_width();
lv_coord_t list_height();
void style_panel(lv_obj_t* panel, lv_color_t bg_color, lv_coord_t radius = 18);
lv_obj_t* create_back_button(lv_obj_t* root, lv_event_cb_t callback, void* user_data);
void set_time_label_text(lv_obj_t* label, const TimeModel& model);
lv_obj_t* create_scroll_column(lv_obj_t* root, lv_coord_t top, lv_coord_t height);
lv_obj_t* create_card_shell(lv_obj_t* parent, bool interactive = false);
lv_obj_t* create_switch_track(lv_obj_t* parent);
void apply_switch_visual(lv_obj_t* track, bool enabled);
lv_obj_t* create_selection_dot(lv_obj_t* parent);
void apply_selection_dot(lv_obj_t* dot, bool selected);
TwoLineTileParts create_two_line_content(lv_obj_t* card, const char* title_text, const char* detail_text);
lv_obj_t* create_home_tile_icon(lv_obj_t* parent, const char* icon_text, std::uint32_t icon_bg);
lv_obj_t* create_note_card(lv_obj_t* parent, const char* title_text, const char* body_text);
lv_obj_t* create_plain_text(lv_obj_t* parent, const char* text, const lv_font_t* font, std::uint32_t color);
lv_obj_t* create_single_line_card(lv_obj_t* parent, const char* title, bool interactive);

}  // namespace twsim::app
