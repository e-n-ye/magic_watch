#pragma once

#include <cstddef>

#include "lvgl/lvgl.h"

namespace twsim::app::shell_home {

struct SurfaceLayout {
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

SurfaceLayout make_surface_layout();
void style_surface_stage(lv_obj_t* stage,
                         lv_coord_t width,
                         lv_coord_t height,
                         lv_coord_t radius,
                         lv_color_t bg_color);
lv_obj_t* create_stage_root(lv_obj_t* root, const SurfaceLayout& layout, lv_color_t stage_bg);
void style_pager_root(lv_obj_t* pager_root, const SurfaceLayout& layout);
lv_obj_t* create_pager_dot(lv_obj_t* parent, lv_color_t color, lv_opa_t opa, bool active);
void set_single_line_label(lv_obj_t* label, lv_coord_t width);

}  // namespace twsim::app::shell_home
