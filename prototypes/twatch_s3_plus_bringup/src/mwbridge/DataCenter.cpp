#include "mwbridge/DataCenter.h"

namespace mwbridge {

DataCenter::DataCenter(EventBus &event_bus) : event_bus_(event_bus)
{
}

EventBus::Subscription DataCenter::subscribe_battery_changed(EventBus::BatteryChangedHandler handler, void *context)
{
    return event_bus_.subscribe_battery_changed(handler, context);
}

void DataCenter::publish_battery(const BatteryModel &model)
{
    last_battery_ = model;
    has_battery_ = true;
    event_bus_.publish_battery_changed(model);
}

bool DataCenter::has_battery() const
{
    return has_battery_;
}

const BatteryModel *DataCenter::battery() const
{
    return has_battery_ ? &last_battery_ : nullptr;
}

}  // namespace mwbridge
