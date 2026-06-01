#pragma once

#include <cstdint>

#include "lvgl.h"

namespace twsim::app {

lv_obj_t* create_steps_scroll_root(lv_obj_t* root, lv_coord_t screen_w, lv_coord_t screen_h);
lv_obj_t* create_steps_icon(lv_obj_t* parent, lv_coord_t size, std::uint32_t bg, std::uint32_t fg, bool use_foot_asset);

}  // namespace twsim::app
