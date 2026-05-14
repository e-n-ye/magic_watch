#pragma once

#include <memory>

#include "HAL/HAL.h"

namespace twsim::platform::simulator {

std::unique_ptr<hal::Device> create_device();

}  // namespace twsim::platform::simulator
