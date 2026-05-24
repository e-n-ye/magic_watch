#include "App/Services/StepsActivityService.h"

#include <algorithm>

namespace twsim::app {

namespace {

constexpr std::uint32_t kMaxReasonableDailySteps = 200000U;

}  // namespace

StepsActivityService::StepsActivityService(DataCenter& data_center) : data_center_(data_center) {}

void StepsActivityService::handle_sample(const hal::ActivitySample& sample) {
  data_center_.publish_steps(to_model(sample));
}

StepsModel StepsActivityService::to_model(const hal::ActivitySample& sample) {
  StepsModel model;
  model.valid = sample.valid;
  model.daily_steps = std::min(sample.daily_steps, kMaxReasonableDailySteps);
  return model;
}

}  // namespace twsim::app
