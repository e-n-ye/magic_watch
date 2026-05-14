#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app {

enum class SurfaceStyle {
  Screen,
  Panel,
  PanelSubtle,
  Chip,
  ButtonPrimary,
  ButtonSecondary,
};

enum class TextStyle {
  Eyebrow,
  Hero,
  HeroSoft,
  Title,
  Body,
  Muted,
  Accent,
  Button,
  Tiny,
};

void ui_prepare_box(lv_obj_t* obj);
void ui_prepare_label(lv_obj_t* obj);
void ui_apply_surface(lv_obj_t* obj, SurfaceStyle style);
void ui_apply_text(lv_obj_t* obj, TextStyle style);
void ui_set_touch_target(lv_obj_t* obj, lv_coord_t extra = 12);
void ui_set_flex_column(lv_obj_t* obj, lv_coord_t pad, lv_coord_t gap);
void ui_set_flex_row(lv_obj_t* obj,
                     lv_coord_t pad,
                     lv_coord_t gap,
                     lv_flex_align_t main_align = LV_FLEX_ALIGN_START,
                     lv_flex_align_t cross_align = LV_FLEX_ALIGN_CENTER);

}  // namespace twsim::app
