#include <algorithm>
#include <chrono>
#include <cstdint>
#include <thread>

#include "HAL/HAL.h"
#include "lvgl/lvgl.h"
#include "magic_watch_ui.h"
#include "screens/screen_health_shortcuts_gen.h"

#ifndef MAGIC_WATCH_XML_UI_ASSET_PATH
#define MAGIC_WATCH_XML_UI_ASSET_PATH "A:../../ui/lvgl_pro/"
#endif

#if defined(_WIN32)
extern "C" int SDL_main(int argc, char ** argv)
#else
int main(int argc, char ** argv)
#endif
{
  LV_UNUSED(argc);
  LV_UNUSED(argv);

  lv_init();

  auto device = twsim::hal::create_simulator_device();
  if (!device) {
    return 1;
  }

  if (!device->initialize({240, 280, "Magic Watch XML UI"})) {
    return 1;
  }

  magic_watch_ui_init(MAGIC_WATCH_XML_UI_ASSET_PATH);

  lv_obj_t * screen = screen_health_shortcuts_create();
  if (screen == nullptr) {
    return 1;
  }
  lv_screen_load(screen);

  auto previous_tick = std::chrono::steady_clock::now();
  while (true) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_tick).count();
    previous_tick = now;

    device->tick(static_cast<std::uint32_t>(elapsed));

    std::uint32_t sleep_time_ms = lv_timer_handler();
    if (sleep_time_ms == LV_NO_TIMER_READY) {
      sleep_time_ms = LV_DEF_REFR_PERIOD;
    }
    sleep_time_ms = std::min<std::uint32_t>(sleep_time_ms, 16U);

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
  }
}
