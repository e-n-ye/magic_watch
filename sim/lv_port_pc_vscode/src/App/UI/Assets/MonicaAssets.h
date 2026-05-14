#pragma once

#include <cstddef>

#include "lvgl/lvgl.h"

namespace twsim::app {

const lv_image_dsc_t* const* monica_spring_wreath_frames();
std::size_t monica_spring_wreath_frame_count();
bool monica_spring_wreath_available();
const char* monica_spring_wreath_status();

}  // namespace twsim::app
