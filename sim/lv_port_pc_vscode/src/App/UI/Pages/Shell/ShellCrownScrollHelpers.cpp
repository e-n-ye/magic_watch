#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"

#include <algorithm>

namespace twsim::app::shell_crown {

namespace {

constexpr std::int32_t kLauncherCrownDragStep = 28;

}  // namespace

void apply_stream_crown_drag(lv_obj_t* scroll_root, bool forward, std::int16_t detents) {
  if (scroll_root == nullptr) {
    return;
  }
  const std::int32_t step = kLauncherCrownDragStep * std::max<std::int16_t>(1, detents);
  const std::int32_t elastic_limit = std::max<std::int32_t>(24, lv_obj_get_height(scroll_root) / 5);
  const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root);
  const std::int32_t scroll_top = lv_obj_get_scroll_top(scroll_root);
  const std::int32_t scroll_bottom = lv_obj_get_scroll_bottom(scroll_root);
  const std::int32_t scroll_max = std::max<std::int32_t>(0, scroll_top + scroll_bottom);
  std::int32_t target_y = current_y + (forward ? step : -step);
  target_y = std::clamp(target_y, -elastic_limit, scroll_max + elastic_limit);
  lv_obj_scroll_to_y(scroll_root, target_y, LV_ANIM_OFF);
}

void release_stream_crown_drag(lv_obj_t* scroll_root) {
  if (scroll_root == nullptr) {
    return;
  }
  const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root);
  const std::int32_t scroll_max =
      std::max<std::int32_t>(0, lv_obj_get_scroll_top(scroll_root) + lv_obj_get_scroll_bottom(scroll_root));
  lv_obj_scroll_to_y(scroll_root, std::clamp(current_y, 0, scroll_max), LV_ANIM_ON);
}

}  // namespace twsim::app::shell_crown
