#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app {

lv_obj_t* create_sleep_switch_track(lv_obj_t* parent);
void apply_sleep_switch_style(lv_obj_t* track, bool enabled);

}  // namespace twsim::app
