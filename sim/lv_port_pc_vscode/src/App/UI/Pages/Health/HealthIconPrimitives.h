#pragma once

#include <cstdint>

#include "lvgl/lvgl.h"

namespace twsim::app {

lv_obj_t* create_sleep_round_icon(lv_obj_t* parent,
                                  lv_coord_t size,
                                  std::uint32_t bg,
                                  std::uint32_t fg,
                                  const char* asset_path,
                                  const char* fallback_text,
                                  const lv_font_t* fallback_font = &lv_font_montserrat_20);

}  // namespace twsim::app
