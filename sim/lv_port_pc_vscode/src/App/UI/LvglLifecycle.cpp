#include "App/UI/LvglLifecycle.h"

namespace twsim::app {

LvglTimerGuard::LvglTimerGuard(lv_timer_t* timer) noexcept : timer_(timer) {}

LvglTimerGuard::~LvglTimerGuard() {
  reset();
}

LvglTimerGuard::LvglTimerGuard(LvglTimerGuard&& other) noexcept : timer_(other.release()) {}

LvglTimerGuard& LvglTimerGuard::operator=(LvglTimerGuard&& other) noexcept {
  if (this != &other) {
    reset(other.release());
  }
  return *this;
}

void LvglTimerGuard::reset(lv_timer_t* timer) noexcept {
  if (timer_ == timer) {
    return;
  }

  if (timer_ != nullptr) {
    lv_timer_del(timer_);
  }
  timer_ = timer;
}

lv_timer_t* LvglTimerGuard::release() noexcept {
  lv_timer_t* timer = timer_;
  timer_ = nullptr;
  return timer;
}

lv_timer_t* LvglTimerGuard::get() const noexcept {
  return timer_;
}

LvglTimerGuard::operator bool() const noexcept {
  return timer_ != nullptr;
}

}  // namespace twsim::app
