#pragma once

#include <cstdint>

namespace mwbridge {

struct BatterySample {
    bool present = true;
    bool charging = false;
    bool external_power = false;
    std::int16_t percent = 0;
    std::uint16_t millivolts = 0;
};

struct BatteryModel {
    bool present = true;
    bool charging = false;
    bool external_power = false;
    std::int16_t percent = 0;
    std::uint16_t millivolts = 0;
};

}  // namespace mwbridge

namespace mwbridge::hal {

using BatterySample = ::mwbridge::BatterySample;

}  // namespace mwbridge::hal

namespace mwbridge::app {

using BatteryModel = ::mwbridge::BatteryModel;

}  // namespace mwbridge::app
