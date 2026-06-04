#pragma once

#include <cstdint>

#include "mwbridge/BatteryTypes.h"

namespace mwbridge {

class EventBus {
public:
    using BatteryChangedHandler = void (*)(void *context, const BatteryModel &model);
    using SubscriptionId = std::uint8_t;

    class Subscription {
    public:
        Subscription() = default;
        Subscription(EventBus *bus, SubscriptionId id);
        Subscription(const Subscription &) = delete;
        Subscription &operator=(const Subscription &) = delete;
        Subscription(Subscription &&other) noexcept;
        Subscription &operator=(Subscription &&other) noexcept;
        ~Subscription();

        void reset();

    private:
        EventBus *bus_ = nullptr;
        SubscriptionId id_ = kInvalidSubscriptionId;
    };

    Subscription subscribe_battery_changed(BatteryChangedHandler handler, void *context);
    void publish_battery_changed(const BatteryModel &model) const;

private:
    static constexpr SubscriptionId kInvalidSubscriptionId = 0xff;
    static constexpr SubscriptionId kMaxBatterySubscribers = 4;

    struct Slot {
        bool active = false;
        BatteryChangedHandler handler = nullptr;
        void *context = nullptr;
    };

    void unsubscribe_battery_changed(SubscriptionId id);

    Slot battery_slots_[kMaxBatterySubscribers] = {};

    friend class Subscription;
};

}  // namespace mwbridge

namespace mwbridge::app {

using EventBus = ::mwbridge::EventBus;

}  // namespace mwbridge::app
