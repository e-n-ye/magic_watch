#include "App/UI/Pages/Shell/ShellAssetHelpers.h"

#include "lvgl/src/misc/lv_fs.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <string>
#include <system_error>

namespace twsim::app::shell_asset {
namespace {

bool has_text(const char* text) {
  return text != nullptr && text[0] != '\0';
}

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
}

std::string resolve_lvgl_asset_path(const char* relative_asset_path) {
  if (!has_text(relative_asset_path)) {
    return {};
  }

  namespace fs = std::filesystem;
  const fs::path relative_path(relative_asset_path);
  const fs::path start = fs::current_path();
  std::array<fs::path, 7> candidates {
      start,
      start / "..",
      start / ".." / "..",
      start / ".." / ".." / "..",
      start / ".." / ".." / ".." / "..",
      start / ".." / ".." / ".." / ".." / "..",
      fs::path("D:/MY_Desk/watch/magic_watch"),
  };

  for (const auto& base : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(base / relative_path, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  std::printf("[magic_watch] asset not found: %s (cwd=%s)\n",
              relative_asset_path,
              start.generic_string().c_str());
  return {};
}

}  // namespace

const char* weather_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/weather_cloud_sun.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* sleep_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/sleep_bed.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* steps_icon_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/steps_foot.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_alipay_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_alipay.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_wechat_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_wechat.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* payment_wechat_green_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/payment_wechat_green.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* preferred_wechat_icon_asset_path() {
  return payment_wechat_green_asset_path() != nullptr ? payment_wechat_green_asset_path() : payment_wechat_asset_path();
}

const char* health_heart_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_heart.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_spo2_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_spo2.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_breathe_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_breathe.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* health_stress_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/health_stress.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* nfc_school_card_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/nfc_school_card.png");
  return path.empty() ? nullptr : path.c_str();
}

const char* nfc_school_card_inner_asset_path() {
  static const std::string path = resolve_lvgl_asset_path("assets/generated_icons/nfc_school_card_inner.png");
  return path.empty() ? nullptr : path.c_str();
}

bool file_exists(const char* path) {
  if (!has_text(path)) {
    return false;
  }
  lv_fs_file_t file;
  if (lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
    return false;
  }
  lv_fs_close(&file);
  return true;
}

}  // namespace twsim::app::shell_asset
