#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app::shell_home_health {

struct HealthCardsView {
  lv_obj_t* heart_card {nullptr};
  lv_obj_t* blood_oxygen_card {nullptr};
  lv_obj_t* breathing_card {nullptr};
  lv_obj_t* stress_card {nullptr};
};

bool create_health_cards(lv_obj_t* parent, HealthCardsView& out);

}  // namespace twsim::app::shell_home_health
