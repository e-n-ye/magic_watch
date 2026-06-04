#pragma once

#include "mwbridge/BatteryTypes.h"
#include "mwbridge/DataCenter.h"

namespace mwbridge {

class BatteryPowerService {
public:
    explicit BatteryPowerService(DataCenter &data_center);

    void handle_sample(const BatterySample &sample);

private:
    static BatteryModel to_model(const BatterySample &sample);

    DataCenter &data_center_;
};

}  // namespace mwbridge

namespace mwbridge::app {

using BatteryPowerService = ::mwbridge::BatteryPowerService;

}  // namespace mwbridge::app
