#pragma once

#include "mwbridge/BatteryTypes.h"
#include "mwbridge/EventBus.h"

namespace mwbridge {

class DataCenter {
public:
    explicit DataCenter(EventBus &event_bus);

    EventBus::Subscription subscribe_battery_changed(EventBus::BatteryChangedHandler handler, void *context);
    void publish_battery(const BatteryModel &model);

    bool has_battery() const;
    const BatteryModel *battery() const;

private:
    EventBus &event_bus_;
    BatteryModel last_battery_ {};
    bool has_battery_ = false;
};

}  // namespace mwbridge

namespace mwbridge::app {

using DataCenter = ::mwbridge::DataCenter;

}  // namespace mwbridge::app
