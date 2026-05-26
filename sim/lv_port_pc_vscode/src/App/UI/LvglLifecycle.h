#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app {

class LvglTimerGuard {
 public:
  LvglTimerGuard() = default;
  explicit LvglTimerGuard(lv_timer_t* timer) noexcept;
  ~LvglTimerGuard();

  LvglTimerGuard(const LvglTimerGuard&) = delete;
  LvglTimerGuard& operator=(const LvglTimerGuard&) = delete;

  LvglTimerGuard(LvglTimerGuard&& other) noexcept;
  LvglTimerGuard& operator=(LvglTimerGuard&& other) noexcept;

  void reset(lv_timer_t* timer = nullptr) noexcept;
  lv_timer_t* release() noexcept;
  lv_timer_t* get() const noexcept;

  explicit operator bool() const noexcept;

 private:
  lv_timer_t* timer_ {nullptr};
};

}  // namespace twsim::app
