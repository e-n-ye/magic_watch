#pragma once

#include <memory>
#include <string>

#include "lvgl/lvgl.h"

namespace twsim::app {

enum class WatchfaceStyleId {
  Diffusion,
  CoolDigitsReserved,
};

struct WatchfaceConfig {
  WatchfaceStyleId style_id {WatchfaceStyleId::Diffusion};
  int spread_index {4};
};

struct WatchfaceRenderState {
  std::string hour_text {"--"};
  std::string minute_text {"--"};
  int battery_percent {-1};
  int spread_index {4};
};

class IWatchfaceStyleRenderer {
 public:
  virtual ~IWatchfaceStyleRenderer() = default;

  virtual lv_obj_t* build(lv_obj_t* parent) = 0;
  virtual void apply(const WatchfaceRenderState& state) = 0;
  virtual bool on_crown_delta(int delta, WatchfaceConfig& config) = 0;
  virtual bool supports_spread_control() const = 0;
};

std::unique_ptr<IWatchfaceStyleRenderer> create_watchface_style_renderer(const WatchfaceConfig& config);
WatchfaceConfig default_watchface_config();
const char* watchface_style_name(WatchfaceStyleId style_id);

}  // namespace twsim::app
