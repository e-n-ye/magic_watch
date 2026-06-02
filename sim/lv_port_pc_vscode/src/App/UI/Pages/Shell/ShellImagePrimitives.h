#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app {

lv_obj_t* create_contain_image(lv_obj_t* parent,
                               const char* path,
                               lv_coord_t width,
                               lv_coord_t height,
                               lv_align_t align,
                               lv_coord_t x,
                               lv_coord_t y);

}  // namespace twsim::app
