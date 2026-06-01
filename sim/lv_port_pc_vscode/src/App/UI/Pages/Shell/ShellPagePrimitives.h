#pragma once

#include <cstdint>

#include "lvgl/lvgl.h"

namespace twsim::app {

void style_root(lv_obj_t* root, std::uint32_t color);
void set_content_width_label(lv_obj_t* label, const char* text, const lv_font_t* font, std::uint32_t color);
lv_coord_t scale_by_ratio(lv_coord_t total, int numerator, int denominator);
lv_coord_t clamp_coord(lv_coord_t value, lv_coord_t minimum, lv_coord_t maximum);

}  // namespace twsim::app
