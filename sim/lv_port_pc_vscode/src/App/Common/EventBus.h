#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "App/Common/AppEvents.h"
#include "App/Common/EnumClassHash.h"

namespace twsim::app {

class EventBus {
 public:
  using Handler = std::function<void(const Event&)>;
  using SubscriptionId = std::uint32_t;

  class Subscription {
   public:
    Subscription() = default;
    Subscription(EventBus* bus, EventId event_id, SubscriptionId subscription_id);
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(Subscription&& other) noexcept;
    ~Subscription();

    void reset();

   private:
    EventBus* bus_ {nullptr};
    EventId event_id_ {EventId::TimeUpdated};
    SubscriptionId subscription_id_ {0};
  };

  Subscription subscribe(EventId event_id, Handler handler);
  void publish(const Event& event) const;

 private:
  struct Slot {
    SubscriptionId id;
    Handler handler;
  };

  void unsubscribe(EventId event_id, SubscriptionId subscription_id);

  mutable std::unordered_map<EventId, std::vector<Slot>, EnumClassHash> slots_;
  SubscriptionId next_subscription_id_ {1};

  friend class Subscription;
};

}  // namespace twsim::app
