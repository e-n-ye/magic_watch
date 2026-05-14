#include "Platform/Simulator/SimulatorDevice.h"

#include <algorithm>
#include <ctime>

#include "SDL.h"
#if defined(_WIN32)
#include <windows.h>
#endif
#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"

extern "C" {
LV_IMAGE_DECLARE(mouse_cursor_icon);
}

namespace twsim::platform::simulator {

namespace {

constexpr std::uint32_t kMainButtonLongPressMs = 900U;
constexpr std::uint32_t kCrownRepeatMs = 120U;
constexpr std::int16_t kEdgeActivationPx = 28;
constexpr std::int16_t kBackCommitDx = 42;
constexpr std::int16_t kBackCancelDx = -18;
constexpr std::int16_t kBackMaxDy = 120;
constexpr std::int16_t kVerticalCommitDy = 42;
constexpr std::int16_t kHorizontalCommitDx = 42;
constexpr std::int16_t kScrollDragThresholdDy = 8;
constexpr std::int16_t kScrollFlickThresholdDy = 20;

bool key_pressed_fallback(int sdl_scancode, int virtual_key) {
  const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
  const bool sdl_pressed = keyboard_state != nullptr && keyboard_state[sdl_scancode] != 0;
#if defined(_WIN32)
  const bool os_pressed = (GetAsyncKeyState(virtual_key) & 0x8000) != 0;
  return sdl_pressed || os_pressed;
#else
  LV_UNUSED(virtual_key);
  return sdl_pressed;
#endif
}

class SimulatorDevice final : public hal::Device {
 public:
  bool initialize(const hal::DisplayConfig& config) override {
    lv_group_set_default(lv_group_create());
    width_ = config.width;
    height_ = config.height;

    display_ = lv_sdl_window_create(config.width, config.height);
    if (display_ == nullptr) {
      return false;
    }

    lv_display_set_default(display_);
    lv_sdl_window_set_title(display_, config.title.c_str());

    lv_indev_t* mouse = lv_sdl_mouse_create();
    lv_indev_t* wheel = lv_sdl_mousewheel_create();
    lv_indev_t* keyboard = lv_sdl_keyboard_create();
    if (mouse == nullptr || wheel == nullptr || keyboard == nullptr) {
      return false;
    }

    lv_indev_set_display(mouse, display_);
    lv_indev_set_display(wheel, display_);
    lv_indev_set_display(keyboard, display_);

    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_group(wheel, lv_group_get_default());
    lv_indev_set_group(keyboard, lv_group_get_default());

    lv_obj_t* cursor = lv_image_create(lv_screen_active());
    if (cursor != nullptr) {
      lv_image_set_src(cursor, &mouse_cursor_icon);
      lv_indev_set_cursor(mouse, cursor);
    }

    return true;
  }

  void set_event_callback(hal::EventCallback callback) override {
    callback_ = std::move(callback);
  }

  void tick(std::uint32_t elapsed_ms) override {
    SDL_PumpEvents();
    process_button(elapsed_ms);
    process_crown(elapsed_ms);
    process_pointer();

    if (!initial_publish_done_) {
      emit_time();
      emit_battery();
      initial_publish_done_ = true;
      return;
    }

    time_accumulator_ms_ += elapsed_ms;
    battery_accumulator_ms_ += elapsed_ms;

    while (time_accumulator_ms_ >= 1000U) {
      time_accumulator_ms_ -= 1000U;
      emit_time();
    }

    while (battery_accumulator_ms_ >= 5000U) {
      battery_accumulator_ms_ -= 5000U;
      advance_battery();
      emit_battery();
    }
  }

 private:
  enum class PointerGesture {
    None,
    EdgeBack,
    BottomOpen,
    TopOpen,
    RightReserved,
    Scroll,
  };

  static hal::TimeSample local_time_sample() {
    std::time_t now = std::time(nullptr);
    std::tm local_time {};
#if defined(_WIN32)
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif

    return {
        true,
        static_cast<std::uint16_t>(1900 + local_time.tm_year),
        static_cast<std::uint8_t>(1 + local_time.tm_mon),
        static_cast<std::uint8_t>(local_time.tm_mday),
        static_cast<std::uint8_t>(local_time.tm_hour),
        static_cast<std::uint8_t>(local_time.tm_min),
        static_cast<std::uint8_t>(local_time.tm_sec),
    };
  }

  void emit_time() const {
    if (!callback_) {
      return;
    }
    callback_({hal::EventKind::TimeUpdated, local_time_sample()});
  }

  void emit_battery() const {
    if (!callback_) {
      return;
    }
    callback_({hal::EventKind::BatteryChanged, battery_});
  }

  void emit_button(hal::ButtonSample::Action action) const {
    if (!callback_) {
      return;
    }
    callback_({hal::EventKind::ButtonChanged, hal::ButtonSample {hal::ButtonSample::Button::Main, action}});
  }

  void emit_touch(hal::TouchSample::Action action,
                  std::int16_t value,
                  std::int16_t x,
                  std::int16_t y) const {
    if (!callback_) {
      return;
    }
    callback_({hal::EventKind::TouchUpdated, hal::TouchSample {action, value, x, y}});
  }

  void emit_crown(hal::CrownSample::Action action, std::int16_t detents = 1) const {
    if (!callback_) {
      return;
    }
    callback_({hal::EventKind::CrownUpdated, hal::CrownSample {action, detents}});
  }

  void process_button(std::uint32_t elapsed_ms) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
    const bool pressed =
        keyboard_state != nullptr &&
        (keyboard_state[SDL_SCANCODE_5] != 0 || keyboard_state[SDL_SCANCODE_KP_5] != 0);

    if (pressed) {
      if (!button_pressed_) {
        button_pressed_ = true;
        long_press_emitted_ = false;
        button_pressed_ms_ = 0;
      } else {
        button_pressed_ms_ += elapsed_ms;
        if (!long_press_emitted_ && button_pressed_ms_ >= kMainButtonLongPressMs) {
          long_press_emitted_ = true;
          emit_button(hal::ButtonSample::Action::LongPress);
        }
      }
      return;
    }

    if (!button_pressed_) {
      return;
    }

    if (!long_press_emitted_) {
      emit_button(hal::ButtonSample::Action::ShortPress);
    }
    button_pressed_ = false;
    long_press_emitted_ = false;
    button_pressed_ms_ = 0;
  }

  void process_crown(std::uint32_t elapsed_ms) {
    const bool crown_press =
        key_pressed_fallback(SDL_SCANCODE_RETURN, VK_RETURN) || key_pressed_fallback(SDL_SCANCODE_SPACE, VK_SPACE);
    if (crown_press && !crown_press_latched_) {
      emit_crown(hal::CrownSample::Action::Press);
    }
    crown_press_latched_ = crown_press;

    process_crown_rotate_key(
        key_pressed_fallback(SDL_SCANCODE_E, 'E') || key_pressed_fallback(SDL_SCANCODE_DOWN, VK_DOWN),
        elapsed_ms,
        crown_cw_latched_,
        crown_cw_repeat_ms_,
        hal::CrownSample::Action::RotateCW);
    process_crown_rotate_key(
        key_pressed_fallback(SDL_SCANCODE_Q, 'Q') || key_pressed_fallback(SDL_SCANCODE_UP, VK_UP),
        elapsed_ms,
        crown_ccw_latched_,
        crown_ccw_repeat_ms_,
        hal::CrownSample::Action::RotateCCW);
  }

  void process_crown_rotate_key(bool pressed,
                                std::uint32_t elapsed_ms,
                                bool& latched,
                                std::uint32_t& repeat_ms,
                                hal::CrownSample::Action action) {
    if (pressed) {
      if (!latched) {
        emit_crown(action);
        latched = true;
        repeat_ms = 0;
        return;
      }

      repeat_ms += elapsed_ms;
      if (repeat_ms >= kCrownRepeatMs) {
        repeat_ms = 0;
        emit_crown(action);
      }
      return;
    }

    latched = false;
    repeat_ms = 0;
  }

  void process_pointer() {
    int mouse_x = 0;
    int mouse_y = 0;
    const Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    const bool pressed = (mouse_state & SDL_BUTTON_LMASK) != 0;

    if (pressed && !pointer_pressed_) {
      pointer_pressed_ = true;
      pointer_start_x_ = static_cast<std::int16_t>(mouse_x);
      pointer_start_y_ = static_cast<std::int16_t>(mouse_y);
      pointer_last_x_ = pointer_start_x_;
      pointer_last_y_ = pointer_start_y_;
      emit_touch(hal::TouchSample::Action::TouchActivity, 0, pointer_start_x_, pointer_start_y_);
      pointer_gesture_ = classify_pointer_gesture(pointer_start_x_, pointer_start_y_);
      return;
    }

    if (pressed && pointer_pressed_) {
      if (pointer_gesture_ == PointerGesture::EdgeBack) {
        const auto dx = static_cast<std::int16_t>(mouse_x - pointer_start_x_);
        if (dx <= kBackCancelDx) {
          emit_touch(hal::TouchSample::Action::EdgeBackCancel,
                     0,
                     static_cast<std::int16_t>(mouse_x),
                     static_cast<std::int16_t>(mouse_y));
          pointer_gesture_ = PointerGesture::None;
          return;
        }

        emit_touch(hal::TouchSample::Action::EdgeBackProgress,
                   std::max<std::int16_t>(0, dx),
                   static_cast<std::int16_t>(mouse_x),
                   static_cast<std::int16_t>(mouse_y));
      } else if (pointer_gesture_ == PointerGesture::Scroll) {
        const auto dx = static_cast<std::int16_t>(mouse_x - pointer_start_x_);
        const auto dy = static_cast<std::int16_t>(mouse_y - pointer_start_y_);
        const auto abs_dx = static_cast<std::int16_t>(dx >= 0 ? dx : -dx);
        const auto abs_dy = static_cast<std::int16_t>(dy >= 0 ? dy : -dy);
        if (abs_dy >= kScrollDragThresholdDy && abs_dy >= abs_dx) {
          emit_touch(hal::TouchSample::Action::ScrollDrag,
                     dy,
                     static_cast<std::int16_t>(mouse_x),
                     static_cast<std::int16_t>(mouse_y));
        }
      }
      pointer_last_x_ = static_cast<std::int16_t>(mouse_x);
      pointer_last_y_ = static_cast<std::int16_t>(mouse_y);
      return;
    }

    if (!pressed && pointer_pressed_) {
      finish_pointer_gesture(static_cast<std::int16_t>(mouse_x), static_cast<std::int16_t>(mouse_y));
      pointer_pressed_ = false;
      pointer_gesture_ = PointerGesture::None;
    }
  }

  PointerGesture classify_pointer_gesture(std::int16_t x, std::int16_t y) const {
    if (x <= kEdgeActivationPx) {
      return PointerGesture::EdgeBack;
    }
    if (y >= static_cast<std::int16_t>(height_ - kEdgeActivationPx)) {
      return PointerGesture::BottomOpen;
    }
    if (y <= kEdgeActivationPx) {
      return PointerGesture::TopOpen;
    }
    if (x >= static_cast<std::int16_t>(width_ - kEdgeActivationPx)) {
      return PointerGesture::RightReserved;
    }
    return PointerGesture::Scroll;
  }

  void finish_pointer_gesture(std::int16_t x, std::int16_t y) {
    const auto dx = static_cast<std::int16_t>(x - pointer_start_x_);
    const auto dy = static_cast<std::int16_t>(y - pointer_start_y_);
    const auto abs_dx = static_cast<std::int16_t>(dx >= 0 ? dx : -dx);
    const auto abs_dy = static_cast<std::int16_t>(dy >= 0 ? dy : -dy);

    switch (pointer_gesture_) {
      case PointerGesture::EdgeBack:
        if (dx >= kBackCommitDx && abs_dy <= kBackMaxDy && dx * 2 >= abs_dy) {
          emit_touch(hal::TouchSample::Action::EdgeBackCommit, std::max<std::int16_t>(0, dx), x, y);
        } else {
          emit_touch(hal::TouchSample::Action::EdgeBackCancel, 0, x, y);
        }
        break;
      case PointerGesture::BottomOpen:
        if (-dy >= kVerticalCommitDy && (-dy * 2) >= abs_dx) {
          emit_touch(hal::TouchSample::Action::BottomEdgeCommit, -dy, x, y);
        }
        break;
      case PointerGesture::TopOpen:
        if (dy >= kVerticalCommitDy && (dy * 2) >= abs_dx) {
          emit_touch(hal::TouchSample::Action::TopEdgeCommit, dy, x, y);
        }
        break;
      case PointerGesture::RightReserved:
        if (-dx >= kHorizontalCommitDx && (-dx * 2) >= abs_dy) {
          emit_touch(hal::TouchSample::Action::RightEdgeCommit, -dx, x, y);
        }
        break;
      case PointerGesture::Scroll:
        if (dx >= kHorizontalCommitDx && dx > abs_dy) {
          emit_touch(hal::TouchSample::Action::HorizontalSwipeRightCommit, dx, x, y);
        } else if (-dx >= kHorizontalCommitDx && (-dx) > abs_dy) {
          emit_touch(hal::TouchSample::Action::HorizontalSwipeLeftCommit, -dx, x, y);
        } else if (abs_dy >= kScrollFlickThresholdDy && abs_dy > abs_dx) {
          emit_touch(hal::TouchSample::Action::ScrollFlick, dy, x, y);
        }
        break;
      case PointerGesture::None:
      default:
        break;
    }
  }

  void advance_battery() {
    if (battery_.charging) {
      if (battery_.percent < 100) {
        ++battery_.percent;
      }
      if (battery_.percent >= 100) {
        battery_.charging = false;
        battery_.external_power = false;
      }
    } else {
      if (battery_.percent > 15) {
        --battery_.percent;
      }
      if (battery_.percent <= 25) {
        battery_.charging = true;
        battery_.external_power = true;
      }
    }

    battery_.millivolts = static_cast<std::uint16_t>(3500 + battery_.percent * 7);
  }

  lv_display_t* display_ {nullptr};
  hal::EventCallback callback_;
  mutable hal::BatterySample battery_ {true, false, false, 82, 4060};
  std::int32_t width_ {240};
  std::int32_t height_ {240};
  std::uint32_t time_accumulator_ms_ {0};
  std::uint32_t battery_accumulator_ms_ {0};
  std::uint32_t button_pressed_ms_ {0};
  bool initial_publish_done_ {false};
  bool button_pressed_ {false};
  bool long_press_emitted_ {false};
  bool crown_press_latched_ {false};
  bool crown_cw_latched_ {false};
  bool crown_ccw_latched_ {false};
  bool pointer_pressed_ {false};
  PointerGesture pointer_gesture_ {PointerGesture::None};
  std::uint32_t crown_cw_repeat_ms_ {0};
  std::uint32_t crown_ccw_repeat_ms_ {0};
  std::int16_t pointer_start_x_ {0};
  std::int16_t pointer_start_y_ {0};
  std::int16_t pointer_last_x_ {0};
  std::int16_t pointer_last_y_ {0};
};

}  // namespace

std::unique_ptr<hal::Device> create_device() {
  return std::make_unique<SimulatorDevice>();
}

}  // namespace twsim::platform::simulator
