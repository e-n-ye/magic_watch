#pragma once

#include <cstddef>

namespace twsim::app {

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T value) const noexcept {
    return static_cast<std::size_t>(value);
  }
};

}  // namespace twsim::app
