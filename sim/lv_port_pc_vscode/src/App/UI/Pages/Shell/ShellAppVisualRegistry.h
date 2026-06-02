#pragma once

#include <cstdint>

#include "App/UI/PageId.h"

namespace twsim::app {

struct AppVisualSpec {
  PageId target;
  const char* label;
  const char* icon_text;
  const char* icon_asset_path;
  std::uint32_t icon_bg;
  std::uint32_t icon_fg;
};

const AppVisualSpec* find_app_visual_spec(PageId target);

}  // namespace twsim::app
