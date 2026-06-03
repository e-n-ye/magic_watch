#pragma once

#include "App/UI/Pages/Shell/Home/ShellHomeLayoutPrimitives.h"
#include "lvgl/lvgl.h"

namespace twsim::app::shell_home_daily {

struct DailyCardsView {
  lv_obj_t* stage {nullptr};
  lv_obj_t* weather_card {nullptr};
  lv_obj_t* weather_temp_label {nullptr};
  lv_obj_t* weather_range_label {nullptr};
  lv_obj_t* sleep_card {nullptr};
  lv_obj_t* sleep_value_label {nullptr};
  lv_obj_t* steps_card {nullptr};
  lv_obj_t* steps_value_label {nullptr};
};

bool create_daily_cards(lv_obj_t* parent, const shell_home::SurfaceLayout& layout, DailyCardsView& out);

}  // namespace twsim::app::shell_home_daily
