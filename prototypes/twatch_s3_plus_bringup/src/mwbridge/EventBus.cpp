#include "mwbridge/EventBus.h"

namespace mwbridge {

EventBus::Subscription::Subscription(EventBus *bus, SubscriptionId id) : bus_(bus), id_(id)
{
}

EventBus::Subscription::Subscription(Subscription &&other) noexcept : bus_(other.bus_), id_(other.id_)
{
    other.bus_ = nullptr;
    other.id_ = kInvalidSubscriptionId;
}

EventBus::Subscription &EventBus::Subscription::operator=(Subscription &&other) noexcept
{
    if (this == &other) {
        return *this;
    }

    reset();
    bus_ = other.bus_;
    id_ = other.id_;
    other.bus_ = nullptr;
    other.id_ = kInvalidSubscriptionId;
    return *this;
}

EventBus::Subscription::~Subscription()
{
    reset();
}

void EventBus::Subscription::reset()
{
    if (bus_ == nullptr) {
        return;
    }

    bus_->unsubscribe_battery_changed(id_);
    bus_ = nullptr;
    id_ = kInvalidSubscriptionId;
}

EventBus::Subscription EventBus::subscribe_battery_changed(BatteryChangedHandler handler, void *context)
{
    if (handler == nullptr) {
        return {};
    }

    for (SubscriptionId index = 0; index < kMaxBatterySubscribers; ++index) {
        if (battery_slots_[index].active) {
            continue;
        }

        battery_slots_[index].active = true;
        battery_slots_[index].handler = handler;
        battery_slots_[index].context = context;
        return Subscription(this, index);
    }

    return {};
}

void EventBus::publish_battery_changed(const BatteryModel &model) const
{
    for (SubscriptionId index = 0; index < kMaxBatterySubscribers; ++index) {
        const Slot &slot = battery_slots_[index];
        if (!slot.active || slot.handler == nullptr) {
            continue;
        }
        slot.handler(slot.context, model);
    }
}

void EventBus::unsubscribe_battery_changed(SubscriptionId id)
{
    if (id >= kMaxBatterySubscribers) {
        return;
    }

    battery_slots_[id] = {};
}

}  // namespace mwbridge
