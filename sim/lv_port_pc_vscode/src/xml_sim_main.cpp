#include <algorithm>
#include <chrono>
#include <cstdio>
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

namespace {

const char* power_request_name(WatchCorePowerRequest request)
{
  switch (request) {
    case WATCH_CORE_POWER_REQUEST_SCREEN_OFF:
      return "SCREEN_OFF";
    case WATCH_CORE_POWER_REQUEST_WAKE:
      return "WAKE";
    case WATCH_CORE_POWER_REQUEST_NONE:
    default:
      return "NONE";
  }
}

const char* power_action_name(WatchCorePowerActionType action_type)
{
  switch (action_type) {
    case WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF:
      return "TURN_SCREEN_OFF";
    case WATCH_CORE_POWER_ACTION_WAKE_SCREEN:
      return "WAKE_SCREEN";
    case WATCH_CORE_POWER_ACTION_NONE:
    default:
      return "NONE";
  }
}

const char* power_state_name(WatchCorePowerState state)
{
  switch (state) {
    case WATCH_CORE_POWER_STATE_SCREEN_OFF:
      return "SCREEN_OFF";
    case WATCH_CORE_POWER_STATE_SCREEN_ON:
    default:
      return "SCREEN_ON";
  }
}

const char* debug_action_name(twsim::hal::DebugSample::Action action)
{
  switch (action) {
    case twsim::hal::DebugSample::Action::SimCoverSleep:
      return "SimCoverSleep";
    case twsim::hal::DebugSample::Action::SimRaiseToWake:
      return "SimRaiseToWake";
    case twsim::hal::DebugSample::Action::SimRaiseDismiss:
      return "SimRaiseDismiss";
    case twsim::hal::DebugSample::Action::InjectBatteryLowNotification:
    default:
      return "InjectBatteryLowNotification";
  }
}

WatchCorePowerRequest map_debug_action_to_power_request(twsim::hal::DebugSample::Action action)
{
  switch (action) {
    case twsim::hal::DebugSample::Action::SimCoverSleep:
      return WATCH_CORE_POWER_REQUEST_SCREEN_OFF;
    case twsim::hal::DebugSample::Action::SimRaiseToWake:
      return WATCH_CORE_POWER_REQUEST_WAKE;
    case twsim::hal::DebugSample::Action::SimRaiseDismiss:
    case twsim::hal::DebugSample::Action::InjectBatteryLowNotification:
    default:
      return WATCH_CORE_POWER_REQUEST_NONE;
  }
}

lv_obj_t* create_screen_off_overlay()
{
  lv_display_t* display = lv_display_get_default();
  if (display == nullptr) {
    return nullptr;
  }

  lv_obj_t* overlay = lv_obj_create(lv_layer_top());
  if (overlay == nullptr) {
    return nullptr;
  }

  lv_obj_remove_style_all(overlay);
  lv_obj_set_pos(overlay, 0, 0);
  lv_obj_set_size(
      overlay,
      lv_display_get_horizontal_resolution(display),
      lv_display_get_vertical_resolution(display));
  lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(overlay, LV_OPA_COVER, 0);
  lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_remove_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
  return overlay;
}

bool apply_power_overlay(lv_obj_t* overlay, WatchCorePowerAction action)
{
  if (overlay == nullptr) {
    return false;
  }

  if (action.type == WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF) {
    lv_obj_remove_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(overlay);
    return true;
  }

  if (action.type == WATCH_CORE_POWER_ACTION_WAKE_SCREEN) {
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    return true;
  }

  return false;
}

void log_power_request(WatchCorePowerRequest request, WatchCorePowerAction action)
{
  std::printf(
      "[power] request=%s action=%s source=%s target=%s\n",
      power_request_name(request),
      power_action_name(action.type),
      power_state_name(action.source_state),
      power_state_name(action.target_state));
  std::fflush(stdout);
}

}  // namespace

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

  lv_obj_t* screen_off_overlay = create_screen_off_overlay();
  if (screen_off_overlay == nullptr) {
    return 1;
  }

  bool battery_dirty = false;
  bool pc_power_request_pending = false;
  WatchCorePowerRequest pending_power_request = WATCH_CORE_POWER_REQUEST_NONE;
  device->set_event_callback(
      [&watch_core, &battery_dirty, &pc_power_request_pending, &pending_power_request](
          const twsim::hal::Event& event) {
        if (event.kind == twsim::hal::EventKind::BatteryChanged) {
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
          return;
        }

        if (event.kind != twsim::hal::EventKind::DebugAction) {
          return;
        }

        const auto* debug_sample = std::get_if<twsim::hal::DebugSample>(&event.payload);
        if (debug_sample == nullptr) {
          return;
        }

        pending_power_request = map_debug_action_to_power_request(debug_sample->action);
        std::printf(
            "[power] debug=%s mapped_request=%s\n",
            debug_action_name(debug_sample->action),
            power_request_name(pending_power_request));
        std::fflush(stdout);

        if (pending_power_request != WATCH_CORE_POWER_REQUEST_NONE) {
          pc_power_request_pending = true;
        }
      });

  auto consume_pending_power_request = [&watch_core, screen_off_overlay](
                                           WatchCorePowerRequest request) {
    const WatchCorePowerAction action = watch_core_request_power_action(&watch_core, request);
    log_power_request(request, action);

    if (action.type == WATCH_CORE_POWER_ACTION_NONE) {
      std::printf("[power] no-op request ignored\n");
      std::fflush(stdout);
      return;
    }

    const bool overlay_applied = apply_power_overlay(screen_off_overlay, action);
    std::printf("[power] overlay_applied=%s\n", overlay_applied ? "true" : "false");
    std::fflush(stdout);
    if (!overlay_applied) {
      return;
    }

    const bool committed = watch_core_commit_power_action(&watch_core, action, true);
    std::printf("[power] commit=%s\n", committed ? "true" : "false");
    std::fflush(stdout);
  };

  auto previous_tick = std::chrono::steady_clock::now();
  while (true) {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_tick).count();
    previous_tick = now;

    device->tick(static_cast<std::uint32_t>(elapsed));
    if (pc_power_request_pending) {
      const WatchCorePowerRequest request = pending_power_request;
      pending_power_request = WATCH_CORE_POWER_REQUEST_NONE;
      pc_power_request_pending = false;
      consume_pending_power_request(request);
    }

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
