#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include "App/UI/PageBase.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

class WatchfacePage : public PageBase {
 public:
  explicit WatchfacePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  lv_obj_t* battery_icon_label_ {nullptr};
  lv_obj_t* minute_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* style_stage_ {nullptr};
  WatchfaceConfig config_ {default_watchface_config()};
  WatchfaceRenderState render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> renderer_;
};

class HomeRingHostPage : public PageBase {
 public:
  explicit HomeRingHostPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  void apply_time(const TimeModel& model);
  void apply_battery(const BatteryModel& model);
  void apply_steps(const StepsModel& model);
  void apply_home_ring_preview(const HomeRingPreviewModel& model);
  void layout_surfaces_for_preview(std::uint8_t base_index, std::int8_t direction);
  void set_track_x(lv_coord_t x);
  std::size_t wrap_surface_index(int index) const;

  lv_obj_t* track_ {nullptr};
  std::array<lv_obj_t*, 5> surfaces_ {{nullptr, nullptr, nullptr, nullptr, nullptr}};
  lv_obj_t* battery_icon_label_ {nullptr};
  lv_obj_t* minute_label_ {nullptr};
  lv_obj_t* battery_label_ {nullptr};
  lv_obj_t* style_stage_ {nullptr};
  lv_obj_t* weather_steps_value_label_ {nullptr};
  lv_obj_t* pager_root_ {nullptr};
  lv_obj_t* pager_active_dot_ {nullptr};
  std::array<lv_obj_t*, 4> pager_dots_ {{nullptr, nullptr, nullptr, nullptr}};
  WatchfaceConfig config_ {default_watchface_config()};
  WatchfaceRenderState render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> renderer_;
  StepsModel steps_model_ {true, 0};
  std::size_t settled_surface_index_ {0};
};

}  // namespace twsim::app
