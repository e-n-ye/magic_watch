#pragma once

#include "App/UI/PageBase.h"

namespace twsim::app {

class WeatherAppPage : public PageBase {
 public:
  explicit WeatherAppPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  void bind_input();

  lv_obj_t* scroll_root_ {nullptr};
  lv_coord_t page_pitch_ {0};
};

}  // namespace twsim::app
