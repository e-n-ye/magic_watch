#pragma once

#include <cstdint>

#include "lvgl.h"

namespace twsim::app {

inline constexpr lv_coord_t kStepsScrollInset = 8;
inline constexpr lv_coord_t kStepsScrollTop = 8;
inline constexpr lv_coord_t kStepsScrollBottom = 0;

const lv_font_t* steps_overview_value_font(std::uint32_t value);
const lv_font_t* steps_card_value_font(std::uint32_t value);
lv_obj_t* create_steps_arc(lv_obj_t* parent,
                           lv_coord_t size,
                           lv_coord_t width,
                           std::uint32_t color,
                           std::int32_t value,
                           std::int32_t max_value);
bool create_steps_metric_line(lv_obj_t* parent, lv_coord_t y, std::uint32_t accent, const char* value, const char* target);
lv_obj_t* create_steps_distribution_card(lv_obj_t* parent,
                                         lv_coord_t width,
                                         lv_coord_t height,
                                         const char* title,
                                         const char* value,
                                         const char* unit,
                                         const char* target,
                                         std::uint32_t accent,
                                         bool use_foot_asset);

}  // namespace twsim::app
