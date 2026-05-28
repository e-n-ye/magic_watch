#include "App/UI/Pages/Shell/ShellFontHelpers.h"

#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"

#include <array>
#include <filesystem>
#include <string>

namespace twsim::app::shell_font {

namespace {

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
}

std::string resolve_windows_cjk_font_path() {
  namespace fs = std::filesystem;
  const std::array<fs::path, 5> candidates {
      fs::path("C:/Windows/Fonts/Deng.ttf"),
      fs::path("C:/Windows/Fonts/Dengl.ttf"),
      fs::path("C:/Windows/Fonts/simhei.ttf"),
      fs::path("C:/Windows/Fonts/msyh.ttc"),
      fs::path("C:/Windows/Fonts/simsun.ttc"),
  };

  for (const auto& candidate : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(candidate, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  return {};
}

}  // namespace

const lv_font_t* cjk_font_14() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 14);
  return font != nullptr ? font : &lv_font_montserrat_14;
}

const lv_font_t* cjk_font_16() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 16);
  return font != nullptr ? font : &lv_font_montserrat_16;
}

const lv_font_t* cjk_font_20() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 20);
  return font != nullptr ? font : &lv_font_montserrat_20;
}

const lv_font_t* cjk_font_72() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 72);
  return font != nullptr ? font : &lv_font_montserrat_48;
}

}  // namespace twsim::app::shell_font
