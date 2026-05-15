#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <variant>

namespace twsim::hal {

struct DisplayConfig {
  std::int32_t width {240};
  std::int32_t height {240};
  std::string title {"T-Watch S3 Plus Simulator"};
};

struct TimeSample {
  bool valid {false};
  std::uint16_t year {0};
  std::uint8_t month {0};
  std::uint8_t day {0};
  std::uint8_t hour {0};
  std::uint8_t minute {0};
  std::uint8_t second {0};
};

struct BatterySample {
  bool present {true};
  bool charging {false};
  bool external_power {false};
  std::int16_t percent {0};
  std::uint16_t millivolts {0};
};

struct ButtonSample {
  enum class Button {
    Main,
  };

  enum class Action {
    ShortPress,
    LongPress,
  };

  Button button {Button::Main};
  Action action {Action::ShortPress};
};

struct CrownSample {
  enum class Action {
    Press,
    RotateCW,
    RotateCCW,
  };

  Action action {Action::Press};
  std::int16_t detents {1};
};

struct TouchSample {
  enum class Action {
    TouchActivity,
    ScrollDrag,
    ScrollFlick,
    ScrollRelease,
    TopEdgeProgress,
    TopEdgeCancel,
    BottomEdgeProgress,
    BottomEdgeCancel,
    EdgeBackProgress,
    EdgeBackCancel,
    EdgeBackCommit,
    HorizontalSwipeRightCommit,
    HorizontalSwipeLeftCommit,
    BottomEdgeCommit,
    TopEdgeCommit,
    RightEdgeCommit,
  };

  Action action {Action::EdgeBackProgress};
  std::int16_t value {0};
  std::int16_t x {0};
  std::int16_t y {0};
};

struct MotionSample {
  bool valid {false};
  std::int16_t x_mg {0};
  std::int16_t y_mg {0};
  std::int16_t z_mg {0};
};

struct DebugSample {
  enum class Action {
    InjectMessageNotification,
    InjectBatteryLowNotification,
  };

  Action action {Action::InjectMessageNotification};
};

enum class EventKind {
  TimeUpdated,
  BatteryChanged,
  ButtonChanged,
  CrownUpdated,
  TouchUpdated,
  MotionUpdated,
  DebugAction,
};

using EventPayload =
    std::variant<TimeSample, BatterySample, ButtonSample, CrownSample, TouchSample, MotionSample, DebugSample>;

struct Event {
  EventKind kind;
  EventPayload payload;
};

using EventCallback = std::function<void(const Event&)>;

class Device {
 public:
  virtual ~Device() = default;

  virtual bool initialize(const DisplayConfig& config) = 0;
  virtual void set_event_callback(EventCallback callback) = 0;
  virtual void tick(std::uint32_t elapsed_ms) = 0;
};

std::unique_ptr<Device> create_simulator_device();

}  // namespace twsim::hal
