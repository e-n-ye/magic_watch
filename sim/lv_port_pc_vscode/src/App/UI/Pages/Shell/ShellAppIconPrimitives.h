#pragma once

#include "App/UI/Pages/Shell/ShellAppVisualRegistry.h"
#include "lvgl/lvgl.h"

namespace twsim::app {

lv_obj_t* create_app_round_icon(lv_obj_t* parent, const AppVisualSpec& spec, lv_coord_t size);

}  // namespace twsim::app
