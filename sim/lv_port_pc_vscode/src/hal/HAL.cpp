#include "HAL/HAL.h"

#include "Platform/Simulator/SimulatorDevice.h"

namespace twsim::hal {

std::unique_ptr<Device> create_simulator_device() {
  return platform::simulator::create_device();
}

}  // namespace twsim::hal
