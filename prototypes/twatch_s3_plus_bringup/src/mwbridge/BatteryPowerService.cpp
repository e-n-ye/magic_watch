#include "mwbridge/BatteryPowerService.h"

namespace mwbridge {

namespace {

std::int16_t clampPercent(std::int16_t percent)
{
    if (percent < 0) {
        return 0;
    }
    if (percent > 100) {
        return 100;
    }
    return percent;
}

}  // namespace

BatteryPowerService::BatteryPowerService(DataCenter &data_center) : data_center_(data_center)
{
}

void BatteryPowerService::handle_sample(const BatterySample &sample)
{
    data_center_.publish_battery(to_model(sample));
}

BatteryModel BatteryPowerService::to_model(const BatterySample &sample)
{
    BatteryModel model;
    model.present = sample.present;
    model.charging = sample.charging;
    model.external_power = sample.external_power;
    model.percent = clampPercent(sample.percent);
    model.millivolts = sample.millivolts;
    return model;
}

}  // namespace mwbridge
