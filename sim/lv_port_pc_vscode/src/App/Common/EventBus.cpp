#include "App/Common/EventBus.h"

#include <algorithm>

namespace twsim::app {

EventBus::Subscription::Subscription(EventBus* bus,
                                     EventId event_id,
                                     SubscriptionId subscription_id)
    : bus_(bus), event_id_(event_id), subscription_id_(subscription_id) {}

EventBus::Subscription::Subscription(Subscription&& other) noexcept
    : bus_(other.bus_), event_id_(other.event_id_), subscription_id_(other.subscription_id_) {
  other.bus_ = nullptr;
  other.subscription_id_ = 0;
}

EventBus::Subscription& EventBus::Subscription::operator=(Subscription&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  reset();
  bus_ = other.bus_;
  event_id_ = other.event_id_;
  subscription_id_ = other.subscription_id_;
  other.bus_ = nullptr;
  other.subscription_id_ = 0;
  return *this;
}

EventBus::Subscription::~Subscription() {
  reset();
}

void EventBus::Subscription::reset() {
  if (bus_ == nullptr) {
    return;
  }

  bus_->unsubscribe(event_id_, subscription_id_);
  bus_ = nullptr;
  subscription_id_ = 0;
}

EventBus::Subscription EventBus::subscribe(EventId event_id, Handler handler) {
  const SubscriptionId subscription_id = next_subscription_id_++;
  slots_[event_id].push_back({subscription_id, std::move(handler)});
  return Subscription(this, event_id, subscription_id);
}

void EventBus::publish(const Event& event) const {
  const auto iter = slots_.find(event.id);
  if (iter == slots_.end()) {
    return;
  }

  const auto handlers = iter->second;
  for (const auto& slot : handlers) {
    if (slot.handler) {
      slot.handler(event);
    }
  }
}

void EventBus::unsubscribe(EventId event_id, SubscriptionId subscription_id) {
  const auto iter = slots_.find(event_id);
  if (iter == slots_.end()) {
    return;
  }

  auto& handlers = iter->second;
  handlers.erase(std::remove_if(handlers.begin(),
                                handlers.end(),
                                [subscription_id](const Slot& slot) {
                                  return slot.id == subscription_id;
                                }),
                 handlers.end());
}

}  // namespace twsim::app
