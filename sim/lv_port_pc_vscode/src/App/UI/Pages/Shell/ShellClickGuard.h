#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app::shell_click_guard {

void attach_click_guard(lv_obj_t* object);
bool click_guard_allows(lv_obj_t* object);

}  // namespace twsim::app::shell_click_guard
