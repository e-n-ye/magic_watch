#include <algorithm>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <variant>

#include "HAL/HAL.h"
#include "XmlUi/watch_core_ui_adapter.h"
#include "lvgl/lvgl.h"
#include "magic_watch_ui.h"
#include "watch_core/watch_core.h"

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

  std::string asset_path = MAGIC_WATCH_XML_UI_ASSET_PATH;
  if (!asset_path.empty() && asset_path.back() != '/' && asset_path.back() != '\\') {
    asset_path.push_back('/');
  }
  magic_watch_ui_init(asset_path.c_str());

  WatchCore watch_core;
  watch_core_init(&watch_core);

  WatchCoreUiAdapter ui_adapter;
  if (!watch_core_ui_adapter_init(&ui_adapter, &watch_core)) {
    return 1;
  }

  bool battery_dirty = false;
  device->set_event_callback([&watch_core, &battery_dirty](const twsim::hal::Event& event) {
    if (event.kind != twsim::hal::EventKind::BatteryChanged) {
      return;
    }

    const auto* battery_sample = std::get_if<twsim::hal::BatterySample>(&event.payload);
    if (battery_sample == nullptr || battery_sample->percent < 0 ||
        battery_sample->percent > 255) {
      return;
    }

    const WatchCoreBatteryState battery_state {
        battery_sample->present,
        battery_sample->charging,
        static_cast<std::uint8_t>(battery_sample->percent),
    };

    if (watch_core_set_battery_state(&watch_core, battery_state)) {
      battery_dirty = true;
    }
  });

  auto previous_tick = std::chrono::steady_clock::now();
  while (true) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_tick).count();
    previous_tick = now;

    device->tick(static_cast<std::uint32_t>(elapsed));
    if (battery_dirty) {
      (void)watch_core_ui_adapter_sync_snapshot(&ui_adapter);
      battery_dirty = false;
    }

    std::uint32_t sleep_time_ms = lv_timer_handler();
    if (sleep_time_ms == LV_NO_TIMER_READY) {
      sleep_time_ms = LV_DEF_REFR_PERIOD;
    }
    sleep_time_ms = std::min<std::uint32_t>(sleep_time_ms, 16U);

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
  }
}
