#pragma once

#include <cstdint>

#include "lvgl/lvgl.h"

namespace twsim::app::shell_crown {

inline constexpr std::uint32_t kLauncherCrownReleaseDelayMs = 95U;

void apply_stream_crown_drag(lv_obj_t* scroll_root, bool forward, std::int16_t detents);
void release_stream_crown_drag(lv_obj_t* scroll_root);

}  // namespace twsim::app::shell_crown
