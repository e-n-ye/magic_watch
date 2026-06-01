#pragma once

#include <cstdint>
#include <optional>

#include "App/Common/AppEvents.h"
#include "lvgl/lvgl.h"

namespace twsim::app {

void apply_compact_time_label(lv_obj_t* label, const std::optional<TimeModel>& model);
lv_obj_t* create_steps_label(lv_obj_t* parent,
                             const char* text,
                             const lv_font_t* font,
                             std::uint32_t color,
                             lv_coord_t width,
                             lv_label_long_mode_t long_mode = LV_LABEL_LONG_DOT);
lv_obj_t* create_steps_panel(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, std::uint32_t bg = 0x0A1626);
lv_obj_t* create_sleep_scroll_root(lv_obj_t* root,
                                   lv_coord_t screen_w,
                                   lv_coord_t screen_h,
                                   lv_coord_t top,
                                   lv_coord_t bottom,
                                   lv_coord_t gap = 10);

}  // namespace twsim::app
