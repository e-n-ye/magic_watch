#include "App/UI/Pages/Shell/ShellPagePrimitives.h"

#include <algorithm>

namespace twsim::app {

void style_root(lv_obj_t* root, std::uint32_t color) {
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(color), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root, 0, 0);
  lv_obj_set_style_pad_all(root, 0, 0);
}

lv_coord_t scale_by_ratio(lv_coord_t total, int numerator, int denominator) {
  return static_cast<lv_coord_t>((static_cast<long long>(total) * numerator) / denominator);
}

lv_coord_t clamp_coord(lv_coord_t value, lv_coord_t minimum, lv_coord_t maximum) {
  return std::max(minimum, std::min(maximum, value));
}

}  // namespace twsim::app
