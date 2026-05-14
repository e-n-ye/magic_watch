#include <chrono>
#include <algorithm>
#include <thread>

#include "App/Application.h"
#include "HAL/HAL.h"
#include "lvgl/lvgl.h"

#if defined(_WIN32)
extern "C" int SDL_main(int argc, char** argv) {
#else
int main(int argc, char** argv) {
#endif
  LV_UNUSED(argc);
  LV_UNUSED(argv);

  lv_init();

  auto device = twsim::hal::create_simulator_device();
  if (!device) {
    return 1;
  }

  if (!device->initialize({240, 296, "T-Watch S3 Plus / MDA Simulator"})) {
    return 1;
  }

  twsim::app::Application application(std::move(device));
  if (!application.start()) {
    return 1;
  }

  auto previous_tick = std::chrono::steady_clock::now();
  while (true) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_tick).count();
    previous_tick = now;

    application.tick(static_cast<std::uint32_t>(elapsed));

    std::uint32_t sleep_time_ms = lv_timer_handler();
    if (sleep_time_ms == LV_NO_TIMER_READY) {
      sleep_time_ms = LV_DEF_REFR_PERIOD;
    }
    sleep_time_ms = std::min<std::uint32_t>(sleep_time_ms, 16U);

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
  }
}
