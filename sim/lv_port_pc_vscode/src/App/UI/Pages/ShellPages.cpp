#include "App/UI/Pages/ShellPages.h"

#include "App/Common/DisplayPolicyRules.h"
#include "App/UI/Pages/Daily/StepsMainPrimitives.h"
#include "App/UI/Pages/Daily/StepsPagePrimitives.h"
#include "App/UI/Pages/Health/BloodOxygenPageHelpers.h"
#include "App/UI/Pages/Health/HealthIconPrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Health/HealthSwitchPrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellAppVisualRegistry.h"
#include "App/UI/Pages/Shell/ShellAppIconPrimitives.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/Home/ShellHomeHealthCardPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomeDailyCardPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomeLayoutPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomePaymentCardPrimitives.h"
#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPrimitives.h"
#include "App/UI/Pages/Shell/ShellImagePrimitives.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <chrono>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <string>
#include <tuple>
#include <utility>

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;
using twsim::app::shell_crown::apply_stream_crown_drag;
using twsim::app::shell_crown::kLauncherCrownReleaseDelayMs;
using twsim::app::shell_crown::release_stream_crown_drag;

namespace twsim::app {

namespace {

using shell_font::cjk_font_14;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;
using shell_font::cjk_font_72;
using shell_asset::file_exists;
using shell_asset::nfc_school_card_asset_path;
using shell_asset::nfc_school_card_inner_asset_path;

constexpr const char* kTextClear = "\xE6\xB8\x85\xE7\xA9\xBA";
constexpr const char* kTextConfirm = "\xE7\xA1\xAE\xE8\xAE\xA4";
constexpr const char* kTextCancel = "\xE5\x8F\x96\xE6\xB6\x88";
constexpr const char* kTextNoMessages = "\xE6\x9A\x82\xE6\x97\xA0\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextNotificationClearConfirmBody = "\xE6\xB8\x85\xE7\xA9\xBA\xE5\x90\x8E\xE5\xB0\x86\xE7\xA7\xBB\xE9\x99\xA4\xE5\xBD\x93\xE5\x89\x8D\xE9\x80\x9A\xE7\x9F\xA5\xE5\x88\x97\xE8\xA1\xA8\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE6\xB8\x85\xE7\xA9\xBA\xEF\xBC\x9F";
constexpr const char* kTextLongBatteryConfirmBody =
    "长续航模式开启后仅保留时间、计步、NFC功能。旋转表冠或充电可退出此功能。确定开启？";
constexpr lv_coord_t kQuickSettingsCloseDragThreshold = 96;
constexpr lv_coord_t kQuickSettingsCloseFlickThreshold = 20;
constexpr lv_coord_t kQuickSettingsMaxDragOffset = 220;
constexpr lv_coord_t kQuickSettingsOpenCommitThreshold = 148;
constexpr lv_coord_t kQuickSettingsSheetY = 46;
constexpr lv_coord_t kQuickSettingsSheetWidth = 228;
constexpr lv_coord_t kQuickSettingsSheetHeight = 228;
constexpr lv_coord_t kHomePagerStep = 15;

const char* quick_settings_log_path() {
  static const std::string path = (std::filesystem::current_path() / "quicksettings_debug.log").string();
  return path.c_str();
}

void append_quick_settings_log(const char* phase,
                               std::size_t index,
                               const char* kind,
                               bool suppress_next_click,
                               bool guard_allows,
                               lv_obj_t* target,
                               lv_event_t* event) {
  std::FILE* file = std::fopen(quick_settings_log_path(), "a");
  if (file == nullptr) {
    return;
  }

  lv_point_t point {0, 0};
  if (event != nullptr) {
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &point);
    }
  }

  const auto now = std::chrono::steady_clock::now().time_since_epoch();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  std::fprintf(file,
               "[%lld] %s idx=%zu kind=%s suppress=%d guard=%d target=%p x=%d y=%d\n",
               static_cast<long long>(ms),
               phase != nullptr ? phase : "Unknown",
               index,
               kind != nullptr ? kind : "Unknown",
               suppress_next_click ? 1 : 0,
               guard_allows ? 1 : 0,
               static_cast<void*>(target),
               static_cast<int>(point.x),
               static_cast<int>(point.y));
  std::fclose(file);
}

void reset_quick_settings_log() {
  std::FILE* file = std::fopen(quick_settings_log_path(), "w");
  if (file == nullptr) {
    return;
  }
  std::fclose(file);
}

lv_obj_t* create_close_chip(lv_obj_t* root, const char* label_text, lv_event_cb_t callback, void* user_data) {
  lv_obj_t* button = lv_button_create(root);
  if (button == nullptr) {
    return nullptr;
  }

  ui_prepare_box(button);
  ui_apply_surface(button, SurfaceStyle::ButtonSecondary);
  ui_set_touch_target(button);
  lv_obj_set_size(button, 72, 28);
  lv_obj_add_event_cb(button, callback, LV_EVENT_CLICKED, user_data);

  lv_obj_t* label = lv_label_create(button);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  ui_apply_text(label, TextStyle::Button);
  lv_label_set_text(label, label_text);
  lv_obj_center(label);
  return button;
}

void style_overlay_card(lv_obj_t* obj, lv_color_t color, lv_opa_t opa, lv_coord_t radius) {
  ui_prepare_box(obj);
  lv_obj_set_style_bg_color(obj, color, 0);
  lv_obj_set_style_bg_opa(obj, opa, 0);
  lv_obj_set_style_border_color(obj, lv_color_hex(0xD6C089), 0);
  lv_obj_set_style_border_opa(obj, LV_OPA_30, 0);
  lv_obj_set_style_border_width(obj, 1, 0);
  lv_obj_set_style_radius(obj, radius, 0);
}

void set_translate_y_exec(void* obj, int32_t value) {
  if (obj == nullptr) {
    return;
  }
  lv_obj_set_style_translate_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value), 0);
}

}  // namespace

}  // namespace twsim::app
