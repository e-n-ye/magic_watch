#pragma once

#include "App/Common/DataCenter.h"
#include "HAL/HAL.h"

namespace twsim::app {

class StepsActivityService {
 public:
  explicit StepsActivityService(DataCenter& data_center);

  void handle_sample(const hal::ActivitySample& sample);

 private:
  static StepsModel to_model(const hal::ActivitySample& sample);

  DataCenter& data_center_;
};

}  // namespace twsim::app
