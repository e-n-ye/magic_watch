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
#include "App/UI/Pages/Shell/Notifications/ShellNotificationPrimitives.h"
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
using shell_asset::payment_wechat_green_asset_path;

constexpr const char* kTextClear = "\xE6\xB8\x85\xE7\xA9\xBA";
constexpr const char* kTextConfirm = "\xE7\xA1\xAE\xE8\xAE\xA4";
constexpr const char* kTextCancel = "\xE5\x8F\x96\xE6\xB6\x88";
constexpr const char* kTextNoMessages = "\xE6\x9A\x82\xE6\x97\xA0\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextDismiss = "\xE5\xBF\xBD\xE7\x95\xA5";
constexpr const char* kTextNotificationClearConfirmBody = "\xE6\xB8\x85\xE7\xA9\xBA\xE5\x90\x8E\xE5\xB0\x86\xE7\xA7\xBB\xE9\x99\xA4\xE5\xBD\x93\xE5\x89\x8D\xE9\x80\x9A\xE7\x9F\xA5\xE5\x88\x97\xE8\xA1\xA8\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE6\xB8\x85\xE7\xA9\xBA\xEF\xBC\x9F";
constexpr const char* kTextLongBatteryConfirmBody =
    "长续航模式开启后仅保留时间、计步、NFC功能。旋转表冠或充电可退出此功能。确定开启？";
constexpr lv_coord_t kNotificationsCloseDragThreshold = 96;
constexpr lv_coord_t kNotificationsCloseFlickThreshold = 20;
constexpr lv_coord_t kNotificationsMaxDragOffset = 220;
constexpr lv_coord_t kNotificationsOpenCommitThreshold = 148;
constexpr lv_coord_t kNotificationsSheetY = 14;
constexpr lv_coord_t kNotificationsSheetWidth = 228;
constexpr lv_coord_t kNotificationsSheetHeight = 268;
constexpr lv_coord_t kNotificationCardSwipeStartThreshold = 14;
constexpr lv_coord_t kNotificationCardSwipeDismissThreshold = 112;
constexpr lv_coord_t kNotificationCardSwipeMaxOffset = 138;
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

NotificationsPage::NotificationsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId NotificationsPage::id() const {
  return PageId::Notifications;
}

const char* NotificationsPage::name() const {
  return "Notifications";
}

void NotificationsPage::on_will_appear() {
  stop_preview_close_timer();
  shell_drag_offset_ = 0;
  open_preview_progress_ = 0;
  shell_drag_active_ = false;
  reset_notification_card_swipe_state();
  hide_clear_confirm_overlay();
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kNotificationsSheetY);
  }
  refresh_backdrop();
  refresh_content();
}

void NotificationsPage::on_will_disappear() {
  stop_preview_close_timer();
  reset_notification_card_swipe_state();
  detail_active_ = false;
  detail_notification_id_.clear();
  hide_clear_confirm_overlay();
}

lv_obj_t* NotificationsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x040B14);
  lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

  backdrop_root_ = lv_obj_create(root);
  sheet_container_ = lv_obj_create(root);
  if (backdrop_root_ == nullptr || sheet_container_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(backdrop_root_);
  lv_obj_set_size(backdrop_root_, 240, 296);
  lv_obj_align(backdrop_root_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(backdrop_root_, lv_color_hex(0x05080F), 0);
  lv_obj_set_style_bg_opa(backdrop_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(backdrop_root_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_root_, 0, 0);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* battery_row = lv_obj_create(backdrop_root_);
  backdrop_battery_icon_label_ = lv_label_create(battery_row);
  backdrop_battery_label_ = lv_label_create(battery_row);
  backdrop_style_stage_ = lv_obj_create(backdrop_root_);
  backdrop_minute_label_ = lv_label_create(backdrop_root_);
  if (battery_row == nullptr || backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr ||
      backdrop_style_stage_ == nullptr || backdrop_minute_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(backdrop_battery_icon_label_);
  ui_apply_text(backdrop_battery_icon_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_icon_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);

  ui_prepare_label(backdrop_battery_label_);
  ui_apply_text(backdrop_battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_label_, "--%");

  ui_prepare_box(backdrop_style_stage_);
  lv_obj_set_size(backdrop_style_stage_, 240, 296);
  lv_obj_align(backdrop_style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(backdrop_style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(backdrop_style_stage_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_style_stage_, 0, 0);

  backdrop_renderer_ = create_watchface_style_renderer(backdrop_config_);
  if (!backdrop_renderer_ || backdrop_renderer_->build(backdrop_style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(backdrop_minute_label_);
  ui_apply_text(backdrop_minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(backdrop_minute_label_, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(backdrop_minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(backdrop_minute_label_, 96);
  lv_obj_set_style_text_align(backdrop_minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(backdrop_minute_label_, 76, 234);
  lv_label_set_text(backdrop_minute_label_, "--");

  lv_obj_remove_flag(sheet_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(sheet_container_, kNotificationsSheetWidth, kNotificationsSheetHeight);
  lv_obj_align(sheet_container_, LV_ALIGN_TOP_MID, 0, kNotificationsSheetY);
  lv_obj_set_style_bg_color(sheet_container_, lv_color_hex(0x040B14), 0);
  lv_obj_set_style_bg_opa(sheet_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sheet_container_, 0, 0);
  lv_obj_set_style_radius(sheet_container_, 28, 0);
  lv_obj_set_style_pad_all(sheet_container_, 0, 0);
  lv_obj_set_style_shadow_width(sheet_container_, 28, 0);
  lv_obj_set_style_shadow_color(sheet_container_, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(sheet_container_, LV_OPA_40, 0);
  lv_obj_set_style_clip_corner(sheet_container_, true, 0);

  clear_button_ = lv_button_create(sheet_container_);
  list_root_ = lv_obj_create(sheet_container_);
  empty_state_ = lv_obj_create(sheet_container_);
  detail_root_ = lv_obj_create(sheet_container_);
  drag_handle_ = lv_obj_create(sheet_container_);
  clear_confirm_overlay_ = lv_obj_create(root);
  if (clear_button_ == nullptr || list_root_ == nullptr || empty_state_ == nullptr || detail_root_ == nullptr ||
      clear_confirm_overlay_ == nullptr ||
      drag_handle_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(clear_button_, 208, 46);
  lv_obj_align(clear_button_, LV_ALIGN_TOP_MID, 0, 18);
  lv_obj_set_style_bg_color(clear_button_, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(clear_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(clear_button_, 0, 0);
  lv_obj_set_style_radius(clear_button_, 22, 0);
  lv_obj_add_event_cb(clear_button_, &NotificationsPage::clear_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* clear_label = lv_label_create(clear_button_);
  if (clear_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(clear_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(clear_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(clear_label, kTextClear);
  lv_obj_center(clear_label);

  lv_obj_set_size(list_root_, 208, 156);
  lv_obj_align(list_root_, LV_ALIGN_TOP_MID, 0, 76);
  lv_obj_set_flex_flow(list_root_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(list_root_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(list_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(list_root_, 0, 0);
  lv_obj_set_style_pad_row(list_root_, 10, 0);
  lv_obj_set_style_bg_opa(list_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list_root_, 0, 0);
  lv_obj_set_style_radius(list_root_, 0, 0);

  lv_obj_remove_flag(empty_state_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(empty_state_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(empty_state_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(empty_state_, 0, 0);
  lv_obj_set_style_pad_all(empty_state_, 0, 0);

  lv_obj_t* bubble = lv_obj_create(empty_state_);
  if (bubble == nullptr) {
    return nullptr;
  }
  lv_obj_remove_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(bubble, 68, 68);
  lv_obj_align(bubble, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_bg_color(bubble, lv_color_hex(0x1D4ED8), 0);
  lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(bubble, 0, 0);
  lv_obj_set_style_radius(bubble, LV_RADIUS_CIRCLE, 0);

  lv_obj_t* bubble_label = lv_label_create(bubble);
  lv_obj_t* empty_text = lv_label_create(empty_state_);
  if (bubble_label == nullptr || empty_text == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(bubble_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(bubble_label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(bubble_label, LV_SYMBOL_BELL);
  lv_obj_center(bubble_label);

  lv_obj_set_style_text_font(empty_text, cjk_font_16(), 0);
  lv_obj_set_style_text_color(empty_text, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(empty_text, kTextNoMessages);
  lv_obj_align(empty_text, LV_ALIGN_CENTER, 0, 34);

  lv_obj_remove_flag(detail_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(detail_root_, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(detail_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(detail_root_, 0, 0);
  lv_obj_set_style_pad_all(detail_root_, 0, 0);
  lv_obj_add_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  detail_back_button_ = lv_button_create(detail_root_);
  lv_obj_t* detail_card = lv_obj_create(detail_root_);
  if (detail_back_button_ == nullptr || detail_card == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(detail_back_button_, 48, 34);
  lv_obj_align(detail_back_button_, LV_ALIGN_TOP_LEFT, 14, 16);
  lv_obj_set_style_bg_color(detail_back_button_, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(detail_back_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(detail_back_button_, 0, 0);
  lv_obj_set_style_radius(detail_back_button_, 16, 0);
  lv_obj_add_event_cb(detail_back_button_, &NotificationsPage::detail_back_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* detail_back_label = lv_label_create(detail_back_button_);
  if (detail_back_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(detail_back_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(detail_back_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(detail_back_label, LV_SYMBOL_LEFT);
  lv_obj_center(detail_back_label);

  lv_obj_remove_flag(detail_card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(detail_card, 208, 206);
  lv_obj_align(detail_card, LV_ALIGN_TOP_MID, 0, 58);
  lv_obj_set_style_bg_color(detail_card, lv_color_hex(0x132033), 0);
  lv_obj_set_style_bg_opa(detail_card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(detail_card, 0, 0);
  lv_obj_set_style_radius(detail_card, 22, 0);
  lv_obj_set_style_pad_all(detail_card, 14, 0);

  detail_source_label_ = lv_label_create(detail_card);
  detail_title_label_ = lv_label_create(detail_card);
  detail_body_label_ = lv_label_create(detail_card);
  detail_time_label_ = lv_label_create(detail_card);
  if (detail_source_label_ == nullptr || detail_title_label_ == nullptr || detail_body_label_ == nullptr ||
      detail_time_label_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_style_text_font(detail_source_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_source_label_, lv_color_hex(0x67E8F9), 0);
  lv_obj_align(detail_source_label_, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_set_width(detail_title_label_, 178);
  lv_label_set_long_mode(detail_title_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail_title_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(detail_title_label_, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(detail_title_label_, LV_ALIGN_TOP_LEFT, 0, 26);

  lv_obj_set_width(detail_body_label_, 178);
  lv_label_set_long_mode(detail_body_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail_body_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_body_label_, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(detail_body_label_, LV_ALIGN_TOP_LEFT, 0, 78);

  lv_obj_set_style_text_font(detail_time_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(detail_time_label_, lv_color_hex(0xCBD5E1), 0);
  lv_obj_align(detail_time_label_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  ui_prepare_box(clear_confirm_overlay_);
  lv_obj_set_size(clear_confirm_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_align(clear_confirm_overlay_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(clear_confirm_overlay_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(clear_confirm_overlay_, LV_OPA_90, 0);
  lv_obj_set_style_border_width(clear_confirm_overlay_, 0, 0);
  lv_obj_set_style_radius(clear_confirm_overlay_, 0, 0);
  lv_obj_add_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(clear_confirm_overlay_);

  lv_obj_t* confirm_body = lv_label_create(clear_confirm_overlay_);
  lv_obj_t* cancel_button = lv_button_create(clear_confirm_overlay_);
  lv_obj_t* confirm_button = lv_button_create(clear_confirm_overlay_);
  if (confirm_body == nullptr || cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }

  lv_obj_set_width(confirm_body, 176);
  lv_label_set_long_mode(confirm_body, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(confirm_body, cjk_font_16(), 0);
  lv_obj_set_style_text_color(confirm_body, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(confirm_body, kTextNotificationClearConfirmBody);
  lv_obj_set_style_text_align(confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(confirm_body, LV_ALIGN_CENTER, 0, -28);

  for (const auto [button, text, accent] : {std::tuple {cancel_button, kTextCancel, lv_color_hex(0x334155)},
                                            std::tuple {confirm_button, kTextConfirm, lv_color_hex(0x2563EB)}}) {
    lv_obj_set_size(button, 82, 44);
    lv_obj_set_style_bg_color(button, accent, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 20, 0);
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_text_font(label, cjk_font_16(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF8FAFC), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }
  lv_obj_align(cancel_button, LV_ALIGN_CENTER, -48, 64);
  lv_obj_align(confirm_button, LV_ALIGN_CENTER, 48, 64);
  lv_obj_add_event_cb(cancel_button, &NotificationsPage::clear_cancel_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_add_event_cb(confirm_button, &NotificationsPage::clear_confirm_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(drag_handle_, 46, 7);
  lv_obj_align(drag_handle_, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_set_style_bg_color(drag_handle_, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_bg_opa(drag_handle_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(drag_handle_, 0, 0);
  lv_obj_set_style_radius(drag_handle_, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_shadow_width(drag_handle_, 16, 0);
  lv_obj_set_style_shadow_color(drag_handle_, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_shadow_opa(drag_handle_, static_cast<lv_opa_t>(140), 0);

  bind_input();
  bind_notifications();
  bind_backdrop();
  refresh_backdrop();
  refresh_content();
  return root;
}

void NotificationsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationsPage::clear_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->show_clear_confirm_overlay();
}

void NotificationsPage::clear_confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->hide_clear_confirm_overlay();
  self->data_center_.clear_notifications();
}

void NotificationsPage::clear_cancel_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->hide_clear_confirm_overlay();
}

void NotificationsPage::detail_back_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->close_detail();
}

void NotificationsPage::notification_card_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  auto* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || self->list_root_ == nullptr) {
    return;
  }
  if (self->suppressed_card_click_target_ == target) {
    self->suppressed_card_click_target_ = nullptr;
    return;
  }
  if (!click_guard_allows(target)) {
    return;
  }
  const std::uint32_t index = static_cast<std::uint32_t>(lv_obj_get_index(target));
  if (index >= self->rendered_notification_ids_.size()) {
    return;
  }
  self->open_detail_for(self->rendered_notification_ids_[index]);
}

void NotificationsPage::notification_card_swipe_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationsPage*>(lv_event_get_user_data(event));
  auto* target = lv_event_get_current_target_obj(event);
  if (self == nullptr || target == nullptr || self->list_root_ == nullptr || self->detail_active_ ||
      self->clear_confirm_active_) {
    return;
  }

  const auto code = lv_event_get_code(event);
  if (code == LV_EVENT_PRESSED) {
    const auto index = static_cast<std::uint32_t>(lv_obj_get_index(target));
    if (index >= self->rendered_notification_ids_.size()) {
      self->reset_notification_card_swipe_state();
      return;
    }
    self->reset_notification_card_swipe_state();
    self->active_card_swipe_ = target;
    self->active_card_swipe_notification_id_ = self->rendered_notification_ids_[index];
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &self->active_card_swipe_press_point_);
    }
    return;
  }

  if (target != self->active_card_swipe_) {
    return;
  }

  if (code == LV_EVENT_PRESSING) {
    lv_point_t point = self->active_card_swipe_press_point_;
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &point);
    }
    const lv_coord_t dx = point.x - self->active_card_swipe_press_point_.x;
    const lv_coord_t dy = point.y - self->active_card_swipe_press_point_.y;

    if (!self->card_swipe_horizontal_capture_) {
      if (LV_ABS(dy) >= kNotificationCardSwipeStartThreshold &&
          LV_ABS(dy) > LV_ABS(dx) + 6) {
        self->card_swipe_vertical_lock_ = true;
        return;
      }
      if (self->card_swipe_vertical_lock_ || dx <= 0) {
        return;
      }
      if (dx < kNotificationCardSwipeStartThreshold || dx <= LV_ABS(dy) + 6) {
        return;
      }

      self->card_swipe_horizontal_capture_ = true;
      lv_obj_clear_flag(self->list_root_, LV_OBJ_FLAG_SCROLLABLE);
    }

    self->set_notification_card_swipe_offset(
        target, clamp_coord(dx, 0, kNotificationCardSwipeMaxOffset));
    return;
  }

  if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
    const bool should_dismiss =
        self->card_swipe_horizontal_capture_ &&
        self->active_card_swipe_offset_ >= kNotificationCardSwipeDismissThreshold &&
        !self->active_card_swipe_notification_id_.empty();
    const std::string dismiss_id = self->active_card_swipe_notification_id_;
    lv_obj_t* card = self->active_card_swipe_;
    const bool had_horizontal_capture = self->card_swipe_horizontal_capture_;
    const lv_coord_t release_offset = self->active_card_swipe_offset_;

    if (self->list_root_ != nullptr) {
      lv_obj_add_flag(self->list_root_, LV_OBJ_FLAG_SCROLLABLE);
    }
    self->active_card_swipe_ = nullptr;
    self->active_card_swipe_press_point_ = {};
    self->active_card_swipe_offset_ = 0;
    self->active_card_swipe_notification_id_.clear();
    self->card_swipe_horizontal_capture_ = false;
    self->card_swipe_vertical_lock_ = false;

    if (should_dismiss) {
      self->suppressed_card_click_target_ = card;
      self->data_center_.dismiss_notification(dismiss_id);
      return;
    }

    if (had_horizontal_capture && card != nullptr) {
      self->suppressed_card_click_target_ = card;
      lv_obj_set_style_translate_x(card, release_offset, 0);
      self->animate_notification_card_swipe_offset(card, 0);
    }
  }
}

void NotificationsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 if (clear_confirm_active_) {
                                   return;
                                 }

                                 const bool list_hidden =
                                     list_root_ == nullptr || lv_obj_has_flag(list_root_, LV_OBJ_FLAG_HIDDEN);

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     if (!list_hidden) {
                                       lv_obj_scroll_by(
                                           list_root_, 0, 44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (!list_hidden) {
                                       lv_obj_scroll_by(
                                           list_root_, 0, -44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     break;
                                    case InputAction::ScrollDrag:
                                      if (should_capture_shell_drag(*command)) {
                                        set_close_drag_offset(
                                            clamp_coord(static_cast<lv_coord_t>(-command->value),
                                                        0,
                                                        kNotificationsMaxDragOffset),
                                            false);
                                      }
                                      break;
                                   case InputAction::ScrollFlick:
                                      if (should_capture_shell_drag(*command)) {
                                        finish_drag_close(command->value, true);
                                      }
                                      break;
                                   case InputAction::ScrollRelease:
                                      if (shell_drag_active_ || shell_drag_offset_ > 0 || should_capture_shell_drag(*command)) {
                                        finish_drag_close(command->value, false);
                                      }
                                      break;
                                    default:
                                      break;
                                  }
                               }));
}

void NotificationsPage::set_open_preview_progress(lv_coord_t progress, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  open_preview_progress_ = clamp_coord(progress, 0, kNotificationsMaxDragOffset);
  const lv_coord_t hidden_y = static_cast<lv_coord_t>(-kNotificationsSheetHeight + 28);
  const lv_coord_t target_y =
      static_cast<lv_coord_t>(hidden_y + ((kNotificationsSheetY - hidden_y) * open_preview_progress_) / kNotificationsMaxDragOffset);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, sheet_container_);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&anim, 220);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::set_close_drag_offset(lv_coord_t offset, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  shell_drag_offset_ = clamp_coord(offset, 0, kNotificationsMaxDragOffset);
  const lv_coord_t target_y = static_cast<lv_coord_t>(kNotificationsSheetY - shell_drag_offset_);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, sheet_container_);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&anim, 220);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::stop_preview_close_timer() {
  if (preview_close_timer_ != nullptr) {
    lv_timer_del(preview_close_timer_);
    preview_close_timer_ = nullptr;
  }
}

void NotificationsPage::preview_close_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<NotificationsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->preview_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationsPage::finish_drag_close(std::int16_t release_delta, bool flick_close) {
  if (root_ == nullptr) {
    return;
  }

  shell_drag_active_ = false;
  const bool should_close =
      shell_drag_offset_ >= kNotificationsCloseDragThreshold ||
      (flick_close && release_delta <= -kNotificationsCloseFlickThreshold);
  if (should_close) {
    shell_drag_offset_ = 0;
    request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
    return;
  }

  shell_drag_offset_ = 0;
  set_close_drag_offset(0, true);
}

bool NotificationsPage::is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const {
  if (drag_handle_ == nullptr) {
    return false;
  }
  lv_area_t coords {};
  lv_obj_get_coords(drag_handle_, &coords);
  return x >= coords.x1 - 20 && x <= coords.x2 + 20 && y >= coords.y1 - 24 && y <= coords.y2 + 18;
}

bool NotificationsPage::should_capture_shell_drag(const InputCommand& command) const {
  if (clear_confirm_active_) {
    return false;
  }
  if (shell_drag_active_) {
    return command.value <= 0 || shell_drag_offset_ > 0;
  }
  if (command.value >= 0) {
    return false;
  }
  if (is_handle_drag_start_zone(command.x, command.y)) {
    const_cast<NotificationsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  if (list_root_ == nullptr || lv_obj_has_flag(list_root_, LV_OBJ_FLAG_HIDDEN)) {
    const_cast<NotificationsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  return false;
}

void NotificationsPage::bind_notifications() {
  track(data_center_.subscribe(EventId::NotificationsChanged,
                               [this](const Event&) {
                                 if (root_ == nullptr) {
                                   return;
                                 }
                                 refresh_content();
                               }));
}

void NotificationsPage::bind_backdrop() {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_backdrop_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_backdrop_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::ShellPreviewRequested,
                               [this](const Event& event) {
                                 const auto* preview = std::get_if<ShellPreviewModel>(&event.payload);
                                 if (preview == nullptr || preview->page_id != PageId::Notifications ||
                                     root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 if (!preview->active) {
                                   shell_drag_active_ = false;
                                   shell_drag_offset_ = 0;
                                   if (open_preview_progress_ >= kNotificationsOpenCommitThreshold) {
                                     set_open_preview_progress(kNotificationsMaxDragOffset, true);
                                   } else {
                                     open_preview_progress_ = 0;
                                     set_open_preview_progress(0, true);
                                     stop_preview_close_timer();
                                     preview_close_timer_ =
                                         lv_timer_create(&NotificationsPage::preview_close_timer_cb, 240U, this);
                                     if (preview_close_timer_ != nullptr) {
                                       lv_timer_set_repeat_count(preview_close_timer_, 1);
                                     }
                                   }
                                   return;
                                 }

                                 stop_preview_close_timer();
                                 if (preview->commit) {
                                   open_preview_progress_ = kNotificationsMaxDragOffset;
                                   set_open_preview_progress(kNotificationsMaxDragOffset, true);
                                   return;
                                 }

                                 set_open_preview_progress(clamp_coord(static_cast<lv_coord_t>(preview->progress),
                                                                       0,
                                                                       kNotificationsMaxDragOffset),
                                                           false);
                               }));
}

void NotificationsPage::refresh_backdrop() {
  if (const auto& time = data_center_.time(); time) {
    apply_backdrop_time(*time);
  }
  if (const auto& battery = data_center_.battery(); battery) {
    apply_backdrop_battery(*battery);
  }
}

void NotificationsPage::apply_backdrop_time(const TimeModel& model) {
  if (backdrop_minute_label_ == nullptr || backdrop_renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(backdrop_minute_label_, "--");
    backdrop_render_state_.hour_text = "--";
    backdrop_render_state_.minute_text = "--";
    backdrop_render_state_.spread_index = backdrop_config_.spread_index;
    backdrop_renderer_->apply(backdrop_render_state_);
    return;
  }

  std::uint8_t hour = model.hour % 12;
  if (hour == 0) {
    hour = 12;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", static_cast<unsigned>(hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));

  lv_label_set_text(backdrop_minute_label_, minute_buffer);
  backdrop_render_state_.hour_text = hour_buffer;
  backdrop_render_state_.minute_text = minute_buffer;
  backdrop_render_state_.spread_index = backdrop_config_.spread_index;
  backdrop_renderer_->apply(backdrop_render_state_);
}

void NotificationsPage::apply_backdrop_battery(const BatteryModel& model) {
  if (backdrop_battery_label_ == nullptr || backdrop_battery_icon_label_ == nullptr) {
    return;
  }

  if (!model.present) {
    lv_label_set_text(backdrop_battery_label_, "--%");
    backdrop_render_state_.battery_percent = -1;
    return;
  }

  char battery_buffer[8] = {};
  std::snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", static_cast<int>(model.percent));
  lv_label_set_text(backdrop_battery_label_, battery_buffer);
  lv_label_set_text(backdrop_battery_icon_label_, model.charging ? LV_SYMBOL_CHARGE : LV_SYMBOL_BATTERY_FULL);
  backdrop_render_state_.battery_percent = model.percent;
}

void NotificationsPage::refresh_content() {
  if (list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr || detail_root_ == nullptr) {
    return;
  }

  if (detail_active_) {
    if (data_center_.find_notification(detail_notification_id_) == nullptr) {
      close_detail();
      return;
    }
    refresh_detail_content();
    return;
  }

  refresh_list_content();
}

void NotificationsPage::refresh_list_content() {
  if (list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr || detail_root_ == nullptr) {
    return;
  }

  reset_notification_card_swipe_state();
  rendered_notification_ids_.clear();
  lv_obj_clean(list_root_);
  lv_obj_add_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  const auto notifications = data_center_.notifications();
  const bool has_items = notifications && !notifications->items.empty();
  if (!has_items) {
    lv_obj_add_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  lv_obj_clear_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);

  for (auto it = notifications->items.rbegin(); it != notifications->items.rend(); ++it) {
    const NotificationItem& item = *it;
    lv_obj_t* card = lv_obj_create(list_root_);
    if (card == nullptr) {
      return;
    }
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(card, 12, 0);
    lv_obj_set_style_bg_color(card, shell_notification::notification_read_card_color(item), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 20, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(card, &NotificationsPage::notification_card_swipe_event_cb, LV_EVENT_PRESS_LOST, this);

    lv_obj_t* icon = shell_notification::create_notification_icon(card, item, true);
    lv_obj_t* status_chip = lv_obj_create(card);
    lv_obj_t* status_label = status_chip == nullptr ? nullptr : lv_label_create(status_chip);
    lv_obj_t* source = lv_label_create(card);
    lv_obj_t* title = lv_label_create(card);
    lv_obj_t* body = lv_label_create(card);
    lv_obj_t* time = lv_label_create(card);
    if (icon == nullptr || status_chip == nullptr || status_label == nullptr || source == nullptr || title == nullptr ||
        body == nullptr || time == nullptr) {
      return;
    }

    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_remove_flag(status_chip, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(status_chip, 46, 22);
    lv_obj_align(status_chip, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(status_chip,
                              item.read ? lv_color_hex(0x223247) : lv_color_hex(0x0F3B5A),
                              0);
    lv_obj_set_style_bg_opa(status_chip, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(status_chip, 0, 0);
    lv_obj_set_style_radius(status_chip, 11, 0);
    lv_obj_set_style_pad_all(status_chip, 0, 0);

    lv_label_set_text(status_label, item.read ? "已读" : "未读");
    lv_obj_set_style_text_font(status_label, cjk_font_14(), 0);
    lv_obj_set_style_text_color(status_label,
                                item.read ? lv_color_hex(0xB7C4D4) : lv_color_hex(0x67E8F9),
                                0);
    lv_obj_center(status_label);

    lv_label_set_text(source, item.source_label.c_str());
    lv_obj_set_width(source, 104);
    lv_label_set_long_mode(source, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(source, cjk_font_14(), 0);
    lv_obj_set_style_text_color(source, shell_notification::notification_accent_color(item.category), 0);
    lv_obj_align(source, LV_ALIGN_TOP_LEFT, 52, 2);

    lv_label_set_text(title, item.title.c_str());
    lv_obj_set_style_text_font(title, cjk_font_16(), 0);
    lv_obj_set_style_text_color(title, shell_notification::notification_primary_text_color(item), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 48);

    lv_label_set_text(body, item.body.c_str());
    lv_obj_set_width(body, 182);
    lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(body, cjk_font_14(), 0);
    lv_obj_set_style_text_color(body, shell_notification::notification_secondary_text_color(item), 0);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 0, 82);
    lv_obj_update_layout(body);

    lv_label_set_text(time, item.time_text.c_str());
    lv_obj_set_style_text_font(time, cjk_font_14(), 0);
    lv_obj_set_style_text_color(time, lv_color_hex(0xCBD5E1), 0);
    lv_obj_align_to(time, body, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

    rendered_notification_ids_.push_back(item.id);
  }
}

void NotificationsPage::refresh_detail_content() {
  if (detail_root_ == nullptr || list_root_ == nullptr || empty_state_ == nullptr || clear_button_ == nullptr ||
      detail_source_label_ == nullptr || detail_title_label_ == nullptr || detail_body_label_ == nullptr ||
      detail_time_label_ == nullptr) {
    return;
  }

  const NotificationItem* item = data_center_.find_notification(detail_notification_id_);
  if (item == nullptr) {
    close_detail();
    return;
  }

  lv_obj_add_flag(list_root_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(empty_state_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(clear_button_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(detail_root_, LV_OBJ_FLAG_HIDDEN);

  lv_label_set_text(detail_source_label_, item->source_label.c_str());
  lv_obj_set_style_text_color(detail_source_label_, shell_notification::notification_accent_color(item->category), 0);
  lv_label_set_text(detail_title_label_, item->title.c_str());
  lv_label_set_text(detail_body_label_, item->body.c_str());
  lv_label_set_text(detail_time_label_, item->time_text.c_str());
}

void NotificationsPage::open_detail_for(std::string_view id) {
  detail_notification_id_ = std::string(id);
  detail_active_ = true;
  data_center_.mark_notification_read(id);
  refresh_content();
}

void NotificationsPage::close_detail() {
  detail_active_ = false;
  detail_notification_id_.clear();
  refresh_content();
}

void NotificationsPage::show_clear_confirm_overlay() {
  if (clear_confirm_overlay_ == nullptr) {
    return;
  }
  reset_notification_card_swipe_state();
  clear_confirm_active_ = true;
  lv_obj_clear_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(clear_confirm_overlay_);
}

void NotificationsPage::hide_clear_confirm_overlay() {
  clear_confirm_active_ = false;
  if (clear_confirm_overlay_ != nullptr) {
    lv_obj_add_flag(clear_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void NotificationsPage::set_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t offset) {
  if (card == nullptr) {
    return;
  }
  active_card_swipe_offset_ = clamp_coord(offset, 0, kNotificationCardSwipeMaxOffset);
  lv_obj_set_style_translate_x(card, active_card_swipe_offset_, 0);
}

void NotificationsPage::animate_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t target_offset) {
  if (card == nullptr) {
    return;
  }

  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, card);
  lv_anim_set_exec_cb(&anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_style_translate_x(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value), 0);
    }
  });
  lv_anim_set_values(&anim,
                     lv_obj_get_style_translate_x(card, LV_PART_MAIN),
                     clamp_coord(target_offset, 0, kNotificationCardSwipeMaxOffset));
  lv_anim_set_duration(&anim, 180);
  lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
  lv_anim_start(&anim);
}

void NotificationsPage::reset_notification_card_swipe_state() {
  if (active_card_swipe_ != nullptr) {
    lv_obj_set_style_translate_x(active_card_swipe_, 0, 0);
  }
  if (list_root_ != nullptr) {
    lv_obj_add_flag(list_root_, LV_OBJ_FLAG_SCROLLABLE);
  }
  active_card_swipe_ = nullptr;
  active_card_swipe_press_point_ = {};
  active_card_swipe_offset_ = 0;
  active_card_swipe_notification_id_.clear();
  card_swipe_horizontal_capture_ = false;
  card_swipe_vertical_lock_ = false;
}

NotificationWakePage::NotificationWakePage(DataCenter& data_center) : PageBase(data_center) {}

PageId NotificationWakePage::id() const {
  return PageId::NotificationWakePreview;
}

const char* NotificationWakePage::name() const {
  return "NotificationWakePreview";
}

void NotificationWakePage::on_will_appear() {
  refresh_content();
  start_auto_close_timer();
}

void NotificationWakePage::on_will_disappear() {
  stop_auto_close_timer();
}

lv_obj_t* NotificationWakePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02060D);

  icon_container_ = lv_obj_create(root);
  preview_card_ = lv_obj_create(root);
  dismiss_button_ = lv_button_create(root);
  if (icon_container_ == nullptr || preview_card_ == nullptr || dismiss_button_ == nullptr) {
    return nullptr;
  }

  lv_obj_remove_flag(icon_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(icon_container_, 52, 52);
  lv_obj_align(icon_container_, LV_ALIGN_TOP_MID, 0, 18);
  lv_obj_set_style_bg_color(icon_container_, lv_color_hex(0x12304A), 0);
  lv_obj_set_style_bg_opa(icon_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon_container_, 0, 0);
  lv_obj_set_style_radius(icon_container_, 18, 0);
  lv_obj_set_style_pad_all(icon_container_, 0, 0);

  icon_image_ = lv_image_create(icon_container_);
  icon_label_ = lv_label_create(icon_container_);
  if (icon_image_ == nullptr || icon_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_center(icon_image_);
  lv_image_set_inner_align(icon_image_, LV_IMAGE_ALIGN_STRETCH);
  lv_obj_center(icon_label_);
  lv_obj_set_style_text_font(icon_label_, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(icon_label_, lv_color_hex(0xFFFFFF), 0);

  lv_obj_set_size(preview_card_, 210, 126);
  lv_obj_align(preview_card_, LV_ALIGN_TOP_MID, 0, 82);
  lv_obj_set_style_bg_color(preview_card_, lv_color_hex(0x14263A), 0);
  lv_obj_set_style_bg_opa(preview_card_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(preview_card_, 0, 0);
  lv_obj_set_style_radius(preview_card_, 22, 0);
  lv_obj_set_style_pad_all(preview_card_, 14, 0);
  lv_obj_add_event_cb(preview_card_, &NotificationWakePage::open_notifications_event_cb, LV_EVENT_CLICKED, this);

  source_label_ = lv_label_create(preview_card_);
  title_label_ = lv_label_create(preview_card_);
  body_label_ = lv_label_create(preview_card_);
  time_label_ = lv_label_create(preview_card_);
  if (source_label_ == nullptr || title_label_ == nullptr || body_label_ == nullptr || time_label_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_style_text_font(source_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(source_label_, lv_color_hex(0x67E8F9), 0);
  lv_obj_align(source_label_, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_set_style_text_font(title_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(title_label_, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title_label_, LV_ALIGN_TOP_LEFT, 0, 24);

  lv_obj_set_width(body_label_, 182);
  lv_label_set_long_mode(body_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(body_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(body_label_, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(body_label_, LV_ALIGN_TOP_LEFT, 0, 56);

  lv_obj_set_style_text_font(time_label_, cjk_font_14(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xCBD5E1), 0);
  lv_obj_align(time_label_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_set_size(dismiss_button_, 174, 44);
  lv_obj_align(dismiss_button_, LV_ALIGN_BOTTOM_MID, 0, -16);
  lv_obj_set_style_bg_color(dismiss_button_, lv_color_hex(0x1A3148), 0);
  lv_obj_set_style_bg_opa(dismiss_button_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(dismiss_button_, 0, 0);
  lv_obj_set_style_radius(dismiss_button_, 22, 0);
  lv_obj_add_event_cb(dismiss_button_, &NotificationWakePage::dismiss_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t* dismiss_label = lv_label_create(dismiss_button_);
  if (dismiss_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_font(dismiss_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(dismiss_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(dismiss_label, kTextDismiss);
  lv_obj_center(dismiss_label);

  bind_notifications();
  refresh_content();
  return root;
}

void NotificationWakePage::dismiss_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  if (!self->current_notification_id_.empty()) {
    self->data_center_.dismiss_notification(self->current_notification_id_);
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationWakePage::open_notifications_event_cb(lv_event_t* event) {
  auto* self = static_cast<NotificationWakePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::OpenNotifications, PageId::Notifications});
}

void NotificationWakePage::timeout_cb(lv_timer_t* timer) {
  auto* self = static_cast<NotificationWakePage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->auto_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void NotificationWakePage::bind_notifications() {
  track(data_center_.subscribe(EventId::NotificationsChanged,
                               [this](const Event&) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }
                                 refresh_content();
                                 start_auto_close_timer();
                               }));
}

void NotificationWakePage::refresh_content() {
  const NotificationItem* item = data_center_.latest_notification();
  if (item == nullptr || icon_container_ == nullptr || source_label_ == nullptr || title_label_ == nullptr ||
      body_label_ == nullptr || time_label_ == nullptr || icon_image_ == nullptr || icon_label_ == nullptr) {
    return;
  }

  current_notification_id_ = item->id;
  lv_obj_set_style_bg_color(icon_container_,
                            item->category == NotificationCategory::BatteryLow ? lv_color_hex(0x8A6A00)
                                                                              : lv_color_hex(0x19C37D),
                            0);

  if (item->category == NotificationCategory::Message && file_exists(payment_wechat_green_asset_path())) {
    lv_image_set_src(icon_image_, payment_wechat_green_asset_path());
    lv_obj_set_size(icon_image_, 30, 30);
    lv_obj_clear_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_label_set_text(icon_label_,
                      item->category == NotificationCategory::BatteryLow
                          ? (item->badge_text.empty() ? LV_SYMBOL_CHARGE : item->badge_text.c_str())
                          : LV_SYMBOL_BELL);
    lv_obj_clear_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
  }

  lv_label_set_text(source_label_, item->source_label.c_str());
  lv_obj_set_style_text_color(source_label_, shell_notification::notification_accent_color(item->category), 0);
  lv_label_set_text(title_label_, item->title.c_str());
  lv_label_set_text(body_label_, item->body.c_str());
  lv_label_set_text(time_label_, item->time_text.c_str());
}

void NotificationWakePage::start_auto_close_timer() {
  stop_auto_close_timer();
  auto_close_timer_ = lv_timer_create(&NotificationWakePage::timeout_cb, 5000U, this);
  if (auto_close_timer_ != nullptr) {
    lv_timer_set_repeat_count(auto_close_timer_, 1);
  }
}

void NotificationWakePage::stop_auto_close_timer() {
  if (auto_close_timer_ != nullptr) {
    lv_timer_del(auto_close_timer_);
    auto_close_timer_ = nullptr;
  }
}

QuickSettingsPage::QuickSettingsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId QuickSettingsPage::id() const {
  return PageId::QuickSettings;
}

const char* QuickSettingsPage::name() const {
  return "QuickSettings";
}

void QuickSettingsPage::on_will_appear() {
  PageBase::on_will_appear();
  reset_quick_settings_log();
  hide_toggle_toast();
  stop_preview_close_timer();
  shell_drag_offset_ = 0;
  open_preview_progress_ = 0;
  shell_drag_active_ = false;
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kQuickSettingsSheetY);
  }
  refresh_backdrop();
  if (suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= suppress_click_deadline_) {
    suppress_next_click_ = false;
    suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
  }
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
}

void QuickSettingsPage::on_will_disappear() {
  stop_toast_timer();
  stop_preview_close_timer();
  hide_toggle_toast();
  hide_long_battery_confirm();
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
  PageBase::on_will_disappear();
}

lv_obj_t* QuickSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x050913);
  lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

  backdrop_root_ = lv_obj_create(root);
  sheet_container_ = lv_obj_create(root);
  toast_container_ = lv_obj_create(root);
  long_battery_confirm_overlay_ = lv_obj_create(root);
  if (backdrop_root_ == nullptr || sheet_container_ == nullptr || toast_container_ == nullptr ||
      long_battery_confirm_overlay_ == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(backdrop_root_, 240, 296);
  lv_obj_align(backdrop_root_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(backdrop_root_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(backdrop_root_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(backdrop_root_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_root_, 0, 0);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(backdrop_root_, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t* backdrop_overlay = lv_obj_create(backdrop_root_);
  lv_obj_t* battery_row = lv_obj_create(backdrop_root_);
  backdrop_battery_icon_label_ = lv_label_create(battery_row);
  backdrop_battery_label_ = lv_label_create(battery_row);
  backdrop_style_stage_ = lv_obj_create(backdrop_root_);
  backdrop_minute_label_ = lv_label_create(backdrop_root_);
  if (backdrop_overlay == nullptr || battery_row == nullptr || backdrop_battery_icon_label_ == nullptr ||
      backdrop_battery_label_ == nullptr || backdrop_style_stage_ == nullptr || backdrop_minute_label_ == nullptr) {
    return nullptr;
  }

  ui_prepare_box(backdrop_overlay);
  lv_obj_set_size(backdrop_overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(backdrop_overlay, lv_color_hex(0x01040A), 0);
  lv_obj_set_style_bg_opa(backdrop_overlay, LV_OPA_20, 0);
  lv_obj_set_style_border_width(backdrop_overlay, 0, 0);
  lv_obj_set_style_pad_all(backdrop_overlay, 0, 0);
  lv_obj_align(backdrop_overlay, LV_ALIGN_TOP_LEFT, 0, 0);

  ui_prepare_box(battery_row);
  ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(battery_row, 53, 18);
  lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(battery_row, 0, 0);
  lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

  ui_prepare_label(backdrop_battery_icon_label_);
  ui_apply_text(backdrop_battery_icon_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_icon_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);

  ui_prepare_label(backdrop_battery_label_);
  ui_apply_text(backdrop_battery_label_, TextStyle::Tiny);
  lv_obj_set_style_text_font(backdrop_battery_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(backdrop_battery_label_, lv_color_hex(0xF5F7FB), 0);
  lv_label_set_text(backdrop_battery_label_, "--%");

  ui_prepare_box(backdrop_style_stage_);
  lv_obj_set_size(backdrop_style_stage_, 240, 296);
  lv_obj_align(backdrop_style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(backdrop_style_stage_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(backdrop_style_stage_, 0, 0);
  lv_obj_set_style_pad_all(backdrop_style_stage_, 0, 0);

  backdrop_renderer_ = create_watchface_style_renderer(backdrop_config_);
  if (!backdrop_renderer_ || backdrop_renderer_->build(backdrop_style_stage_) == nullptr) {
    return nullptr;
  }

  ui_prepare_label(backdrop_minute_label_);
  ui_apply_text(backdrop_minute_label_, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(backdrop_minute_label_, &lv_font_montserrat_42, 0);
  lv_obj_set_style_text_color(backdrop_minute_label_, lv_color_hex(0xD7E3F4), 0);
  lv_obj_set_width(backdrop_minute_label_, 96);
  lv_obj_set_style_text_align(backdrop_minute_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_pos(backdrop_minute_label_, 76, 234);
  lv_label_set_text(backdrop_minute_label_, "--");

  lv_obj_set_size(sheet_container_, kQuickSettingsSheetWidth, kQuickSettingsSheetHeight);
  lv_obj_set_style_bg_color(sheet_container_, lv_color_hex(0x09131F), 0);
  lv_obj_set_style_bg_opa(sheet_container_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sheet_container_, 0, 0);
  lv_obj_set_style_radius(sheet_container_, 32, 0);
  lv_obj_set_style_pad_top(sheet_container_, 18, 0);
  lv_obj_set_style_pad_bottom(sheet_container_, 18, 0);
  lv_obj_set_style_pad_left(sheet_container_, 16, 0);
  lv_obj_set_style_pad_right(sheet_container_, 16, 0);
  lv_obj_set_style_shadow_width(sheet_container_, 34, 0);
  lv_obj_set_style_shadow_color(sheet_container_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_shadow_opa(sheet_container_, LV_OPA_50, 0);
  lv_obj_set_style_border_color(sheet_container_, lv_color_hex(0x15263A), 0);
  lv_obj_set_style_border_opa(sheet_container_, LV_OPA_50, 0);
  lv_obj_set_style_border_width(sheet_container_, 1, 0);
  lv_obj_remove_flag(sheet_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_align(sheet_container_, LV_ALIGN_TOP_MID, 0, 0);

  toast_label_ = lv_label_create(toast_container_);
  if (toast_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(toast_container_);
  lv_obj_set_style_bg_color(toast_container_, lv_color_hex(0x6A7D97), 0);
  lv_obj_set_style_bg_opa(toast_container_, LV_OPA_80, 0);
  lv_obj_set_style_border_width(toast_container_, 0, 0);
  lv_obj_set_style_radius(toast_container_, 22, 0);
  lv_obj_set_style_pad_left(toast_container_, 18, 0);
  lv_obj_set_style_pad_right(toast_container_, 18, 0);
  lv_obj_set_style_pad_top(toast_container_, 8, 0);
  lv_obj_set_style_pad_bottom(toast_container_, 8, 0);
  lv_obj_remove_flag(toast_container_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(toast_container_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_align(toast_container_, LV_ALIGN_TOP_MID, 0, 28);
  lv_obj_set_size(toast_container_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_move_foreground(toast_container_);

  ui_prepare_label(toast_label_);
  ui_apply_text(toast_label_, TextStyle::Body);
  lv_obj_set_style_text_font(toast_label_, cjk_font_16(), 0);
  lv_obj_set_style_text_color(toast_label_, lv_color_hex(0xF8FBFF), 0);
  lv_label_set_long_mode(toast_label_, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_set_size(toast_label_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_text_align(toast_label_, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(toast_label_, "");
  lv_obj_center(toast_label_);

  ui_prepare_box(long_battery_confirm_overlay_);
  lv_obj_set_size(long_battery_confirm_overlay_, LV_PCT(100), LV_PCT(100));
  lv_obj_align(long_battery_confirm_overlay_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(long_battery_confirm_overlay_, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(long_battery_confirm_overlay_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(long_battery_confirm_overlay_, 0, 0);
  lv_obj_set_style_radius(long_battery_confirm_overlay_, 0, 0);
  lv_obj_add_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* confirm_body = lv_label_create(long_battery_confirm_overlay_);
  lv_obj_t* cancel_button = lv_button_create(long_battery_confirm_overlay_);
  lv_obj_t* confirm_button = lv_button_create(long_battery_confirm_overlay_);
  if (confirm_body == nullptr || cancel_button == nullptr || confirm_button == nullptr) {
    return nullptr;
  }

  ui_prepare_label(confirm_body);
  lv_obj_set_width(confirm_body, 196);
  lv_obj_set_style_text_font(confirm_body, cjk_font_20(), 0);
  lv_obj_set_style_text_color(confirm_body, lv_color_hex(0xF7FBFF), 0);
  lv_obj_set_style_text_align(confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(confirm_body, LV_LABEL_LONG_WRAP);
  lv_label_set_text(confirm_body, kTextLongBatteryConfirmBody);
  lv_obj_align(confirm_body, LV_ALIGN_TOP_MID, 0, 34);

  for (lv_obj_t* button : {cancel_button, confirm_button}) {
    attach_click_guard(button);
    ui_prepare_box(button);
    lv_obj_set_size(button, 92, 50);
    lv_obj_set_style_radius(button, 18, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_add_event_cb(button, &QuickSettingsPage::long_battery_confirm_event_cb, LV_EVENT_CLICKED, this);
  }
  lv_obj_align(cancel_button, LV_ALIGN_BOTTOM_LEFT, 24, -34);
  lv_obj_align(confirm_button, LV_ALIGN_BOTTOM_RIGHT, -24, -34);
  lv_obj_set_style_bg_color(cancel_button, lv_color_hex(0x17314C), 0);
  lv_obj_set_style_bg_opa(cancel_button, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(confirm_button, lv_color_hex(0x11B8FF), 0);
  lv_obj_set_style_bg_opa(confirm_button, LV_OPA_COVER, 0);
  lv_obj_set_user_data(cancel_button, reinterpret_cast<void*>(0U));
  lv_obj_set_user_data(confirm_button, reinterpret_cast<void*>(1U));

  for (const auto [button, text] : {std::pair {cancel_button, LV_SYMBOL_CLOSE}, std::pair {confirm_button, LV_SYMBOL_OK}}) {
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xE8FBFF), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }
  lv_obj_move_foreground(long_battery_confirm_overlay_);

  drag_handle_ = lv_obj_create(sheet_container_);
  if (drag_handle_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* grid = lv_obj_create(sheet_container_);
  if (grid == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(grid, 196, 196);
  lv_obj_align(grid, LV_ALIGN_CENTER, 0, 6);
  lv_obj_set_layout(grid, LV_LAYOUT_GRID);
  static lv_coord_t columns[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t rows[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(grid, columns, rows);
  lv_obj_set_style_pad_all(grid, 0, 0);
  lv_obj_set_style_pad_row(grid, 8, 0);
  lv_obj_set_style_pad_column(grid, 8, 0);
  lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(grid, 0, 0);
  lv_obj_set_style_radius(grid, 0, 0);
  lv_obj_remove_flag(grid, LV_OBJ_FLAG_SCROLLABLE);

  for (std::size_t index = 0; index < toggles_.size(); ++index) {
    const auto row = static_cast<lv_coord_t>(index / 3);
    const auto col = static_cast<lv_coord_t>(index % 3);
    lv_obj_t* button = lv_button_create(grid);
    if (button == nullptr) {
      return nullptr;
    }
    toggles_[index].button = button;
    lv_obj_set_grid_cell(button, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_size(button, 58, 58);
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x15294A), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_long_press_event_cb, LV_EVENT_LONG_PRESSED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_release_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_press_lost_event_cb, LV_EVENT_PRESS_LOST, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* icon = lv_label_create(button);
    if (icon == nullptr) {
      return nullptr;
    }
    toggles_[index].icon_label = icon;
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_24, 0);
    lv_label_set_text(icon, toggles_[index].icon_text);
    lv_obj_center(icon);
    apply_toggle_visual(index);
  }

  lv_obj_set_size(drag_handle_, 52, 7);
  lv_obj_align(drag_handle_, LV_ALIGN_TOP_MID, 0, 4);
  lv_obj_set_style_bg_color(drag_handle_, lv_color_hex(0x4B5E7A), 0);
  lv_obj_set_style_bg_opa(drag_handle_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(drag_handle_, 0, 0);
  lv_obj_set_style_radius(drag_handle_, LV_RADIUS_CIRCLE, 0);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(drag_handle_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_move_foreground(drag_handle_);

  bind_input();
  bind_display_policy();
  bind_backdrop();
  refresh_backdrop();
  set_open_preview_progress(0, false);
  return root;
}

void QuickSettingsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::toggle_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("CLICK_ENTER", index, label, self->suppress_next_click_, true, target, event);
  const bool guard_allows = target != nullptr && click_guard_allows(target);
  append_quick_settings_log("CLICK_GUARD", index, label, self->suppress_next_click_, guard_allows, target, event);
  if (target == nullptr || !guard_allows) {
    return;
  }
  if (self->suppress_next_click_ &&
      self->suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= self->suppress_click_deadline_) {
    append_quick_settings_log("CLICK_SUPPRESS_EXPIRED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
  }
  if (self->suppress_next_click_) {
    append_quick_settings_log("CLICK_SWALLOWED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
    return;
  }

  if (index >= self->toggles_.size()) {
    append_quick_settings_log("CLICK_INDEX_OOB", index, label, self->suppress_next_click_, true, target, event);
    return;
  }

  auto& toggle = self->toggles_[index];
  append_quick_settings_log("CLICK_HANDLE", index, toggle.label, self->suppress_next_click_, true, target, event);
  if (toggle.kind == ToggleKind::OpenSettings) {
    append_quick_settings_log("CLICK_NAV_SETTINGS", index, toggle.label, self->suppress_next_click_, true, target, event);
    self->request_navigation({NavigationAction::LaunchApp, PageId::SettingsHome});
    return;
  }

  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->notification_wake_enabled;
    self->data_center_.set_notification_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
  } else if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
    self->data_center_.set_raise_to_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(!current ? "抬腕亮屏已开启" : "抬腕亮屏已关闭");
  } else if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = self->data_center_.display_policy();
    const bool current = policy && policy->keep_screen_on_duration_ms > 0U;
    self->data_center_.set_keep_screen_on_duration_ms(current ? 0U : 300000U);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(current ? "持续亮屏已关闭" : "持续亮屏5分钟");
  } else if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = self->data_center_.power_mode();
    const bool current = mode && mode->long_battery_mode_enabled;
    if (current) {
      self->data_center_.set_long_battery_mode_enabled(false);
      toggle.mode = 0;
    } else {
      self->show_long_battery_confirm();
      return;
    }
  } else {
    toggle.mode = toggle.mode == 0 ? 1 : 0;
  }
  self->apply_toggle_visual(index);
  append_quick_settings_log("CLICK_DONE", index, toggle.label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toggle_long_press_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  const char* label = index < self->toggles_.size() ? self->toggles_[index].label : "oob";
  append_quick_settings_log("LONG_PRESS_ENTER", index, label, self->suppress_next_click_, true, target, event);
  if (index >= self->toggles_.size()) {
    return;
  }

  self->stop_toast_timer();
  self->hide_toggle_toast();
  if (!self->toggles_[index].detail_page.has_value()) {
    return;
  }
  self->suppress_next_click_ = true;
  self->suppress_click_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(1200);
  self->long_press_source_button_ = target;
  self->suppress_global_clicks_for(std::chrono::milliseconds(520));
  append_quick_settings_log(
      "LONG_PRESS_NAV", index, self->toggles_[index].label, self->suppress_next_click_, true, target, event);
  self->request_navigation({NavigationAction::LaunchApp, *self->toggles_[index].detail_page});
}

void QuickSettingsPage::toggle_release_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("RELEASE", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
}

void QuickSettingsPage::toggle_press_lost_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("PRESS_LOST", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
  self->long_press_source_button_ = nullptr;
  append_quick_settings_log("PRESS_LOST_SOURCE_CLEARED", index, label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toast_timeout_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->toast_timer_ = nullptr;
  self->hide_toggle_toast();
}

void QuickSettingsPage::long_battery_confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto confirmed = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)) == 1U;
  self->hide_long_battery_confirm();
  if (!confirmed) {
    return;
  }

  self->data_center_.set_long_battery_mode_enabled(true);
}

void QuickSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::ScrollDrag:
                                     if (should_capture_shell_drag(*command)) {
                                       set_close_drag_offset(
                                           clamp_coord(static_cast<lv_coord_t>(command->value),
                                                       0,
                                                       kQuickSettingsMaxDragOffset),
                                           false);
                                     }
                                     break;
                                   case InputAction::ScrollFlick:
                                     if (should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, true);
                                     }
                                     break;
                                   case InputAction::ScrollRelease:
                                     if (shell_drag_active_ || shell_drag_offset_ > 0 || should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, false);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void QuickSettingsPage::bind_display_policy() {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event& event) {
                                 const auto* policy = std::get_if<DisplayPolicyModel>(&event.payload);
                                 if (policy == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   switch (toggles_[index].kind) {
                                     case ToggleKind::NotifyWake:
                                       toggles_[index].mode = policy->notification_wake_enabled ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::RaiseToWake:
                                       toggles_[index].mode = policy->raise_to_wake_mode != RaiseToWakeMode::Off ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::AodFiveMinutes:
                                       toggles_[index].mode = policy->keep_screen_on_duration_ms > 0U ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     default:
                                       break;
                                   }
                                 }
                               }));
  track(data_center_.subscribe(EventId::PowerModeChanged,
                               [this](const Event& event) {
                                 const auto* mode = std::get_if<PowerModeModel>(&event.payload);
                                 if (mode == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   if (toggles_[index].kind == ToggleKind::LongBattery) {
                                     toggles_[index].mode = mode->long_battery_mode_enabled ? 1 : 0;
                                     apply_toggle_visual(index);
                                   }
                                 }
                               }));
}

void QuickSettingsPage::bind_backdrop() {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_backdrop_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_backdrop_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::ShellPreviewRequested,
                               [this](const Event& event) {
                                 const auto* preview = std::get_if<ShellPreviewModel>(&event.payload);
                                 if (preview == nullptr || preview->page_id != PageId::QuickSettings ||
                                     root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 if (!preview->active) {
                                   shell_drag_active_ = false;
                                   shell_drag_offset_ = 0;
                                   if (open_preview_progress_ >= kQuickSettingsOpenCommitThreshold) {
                                     set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   } else {
                                     open_preview_progress_ = 0;
                                     set_open_preview_progress(0, true);
                                     stop_preview_close_timer();
                                     preview_close_timer_ =
                                         lv_timer_create(&QuickSettingsPage::preview_close_timer_cb, 240U, this);
                                     if (preview_close_timer_ != nullptr) {
                                       lv_timer_set_repeat_count(preview_close_timer_, 1);
                                     }
                                   }
                                   return;
                                 }

                                 stop_preview_close_timer();
                                 if (preview->commit) {
                                   open_preview_progress_ = kQuickSettingsMaxDragOffset;
                                   set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   return;
                                 }

                                 set_open_preview_progress(clamp_coord(static_cast<lv_coord_t>(preview->progress),
                                                                       0,
                                                                       kQuickSettingsMaxDragOffset),
                                                           false);
                               }));
}

void QuickSettingsPage::refresh_backdrop() {
  if (const auto& time = data_center_.time(); time) {
    apply_backdrop_time(*time);
  }
  if (const auto& battery = data_center_.battery(); battery) {
    apply_backdrop_battery(*battery);
  }
}

void QuickSettingsPage::apply_backdrop_time(const TimeModel& model) {
  if (backdrop_minute_label_ == nullptr || backdrop_renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(backdrop_minute_label_, "--");
    backdrop_render_state_.hour_text = "--";
    backdrop_render_state_.minute_text = "--";
    backdrop_render_state_.spread_index = backdrop_config_.spread_index;
    backdrop_renderer_->apply(backdrop_render_state_);
    return;
  }

  std::uint8_t hour = model.hour % 12;
  if (hour == 0) {
    hour = 12;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", static_cast<unsigned>(hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));

  lv_label_set_text(backdrop_minute_label_, minute_buffer);
  backdrop_render_state_.hour_text = hour_buffer;
  backdrop_render_state_.minute_text = minute_buffer;
  backdrop_render_state_.spread_index = backdrop_config_.spread_index;
  backdrop_renderer_->apply(backdrop_render_state_);
}

void QuickSettingsPage::apply_backdrop_battery(const BatteryModel& model) {
  if (backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr) {
    return;
  }

  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);
  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%", static_cast<int>(model.percent));
  lv_label_set_text(backdrop_battery_label_, buffer);
}

void QuickSettingsPage::apply_toggle_visual(std::size_t index) {
  if (index >= toggles_.size() || toggles_[index].button == nullptr || toggles_[index].icon_label == nullptr) {
    return;
  }

  auto& toggle = toggles_[index];
  const bool active = is_toggle_active(toggle);

  lv_obj_set_style_bg_color(toggle.button, active ? lv_color_hex(0x1493FF) : lv_color_hex(0x15294A), 0);
  lv_obj_set_style_bg_opa(toggle.button, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(toggle.icon_label, lv_color_hex(0xF5FAFF), 0);
}

bool QuickSettingsPage::is_toggle_active(const ToggleState& toggle) const {
  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->notification_wake_enabled;
  }
  if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
  }
  if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = data_center_.display_policy();
    return policy && policy->keep_screen_on_duration_ms > 0U;
  }
  if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = data_center_.power_mode();
    return mode && mode->long_battery_mode_enabled;
  }
  if (toggle.kind == ToggleKind::OpenSettings) {
    return false;
  }
  return toggle.mode != 0;
}

void QuickSettingsPage::show_long_battery_confirm() {
  if (long_battery_confirm_overlay_ == nullptr) {
    return;
  }
  hide_toggle_toast();
  lv_obj_clear_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(long_battery_confirm_overlay_);
}

void QuickSettingsPage::hide_long_battery_confirm() {
  if (long_battery_confirm_overlay_ != nullptr) {
    lv_obj_add_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void QuickSettingsPage::show_toggle_toast(const char* text) {
  if (toast_container_ == nullptr || toast_label_ == nullptr || text == nullptr) {
    return;
  }
  lv_label_set_text(toast_label_, text);
  lv_obj_update_layout(toast_label_);
  const lv_coord_t label_width = static_cast<lv_coord_t>(lv_obj_get_width(toast_label_) + 1);
  const lv_coord_t label_height = static_cast<lv_coord_t>(lv_obj_get_height(toast_label_) + 1);
  lv_obj_set_size(toast_container_,
                  static_cast<lv_coord_t>(label_width + 32),
                  static_cast<lv_coord_t>(label_height + 16));
  lv_obj_update_layout(toast_container_);
  lv_obj_clear_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(toast_container_);
  stop_toast_timer();
  toast_timer_ = lv_timer_create(&QuickSettingsPage::toast_timeout_cb, 3000U, this);
  if (toast_timer_ != nullptr) {
    lv_timer_set_repeat_count(toast_timer_, 1);
  }
}

void QuickSettingsPage::hide_toggle_toast() {
  if (toast_container_ == nullptr) {
    return;
  }
  lv_obj_add_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
}

void QuickSettingsPage::stop_toast_timer() {
  if (toast_timer_ != nullptr) {
    lv_timer_del(toast_timer_);
    toast_timer_ = nullptr;
  }
}

void QuickSettingsPage::set_open_preview_progress(lv_coord_t progress, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  open_preview_progress_ = clamp_coord(progress, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t hidden_y = 296;
  const lv_coord_t target_y = static_cast<lv_coord_t>(
      hidden_y - ((hidden_y - kQuickSettingsSheetY) * open_preview_progress_) / kQuickSettingsMaxDragOffset);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);

}

void QuickSettingsPage::set_close_drag_offset(lv_coord_t offset, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  shell_drag_offset_ = clamp_coord(offset, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t target_y = static_cast<lv_coord_t>(kQuickSettingsSheetY + shell_drag_offset_);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);

}

void QuickSettingsPage::stop_preview_close_timer() {
  if (preview_close_timer_ != nullptr) {
    lv_timer_del(preview_close_timer_);
    preview_close_timer_ = nullptr;
  }
}

void QuickSettingsPage::preview_close_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->preview_close_timer_ = nullptr;
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::finish_drag_close(std::int16_t release_delta, bool flick_close) {
  if (root_ == nullptr) {
    return;
  }

  shell_drag_active_ = false;
  const bool should_close =
      shell_drag_offset_ >= kQuickSettingsCloseDragThreshold ||
      (flick_close && release_delta >= kQuickSettingsCloseFlickThreshold);
  if (should_close) {
    shell_drag_offset_ = 0;
    request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
    return;
  }

  shell_drag_offset_ = 0;
  set_close_drag_offset(0, true);
}

bool QuickSettingsPage::is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const {
  if (drag_handle_ == nullptr) {
    return false;
  }
  lv_area_t coords {};
  lv_obj_get_coords(drag_handle_, &coords);
  return x >= coords.x1 - 20 && x <= coords.x2 + 20 && y >= coords.y1 - 18 && y <= coords.y2 + 24;
}

bool QuickSettingsPage::should_capture_shell_drag(const InputCommand& command) const {
  if (shell_drag_active_) {
    return command.value >= 0 || shell_drag_offset_ > 0;
  }
  if (command.value <= 0) {
    return false;
  }
  if (is_handle_drag_start_zone(command.x, command.y)) {
    const_cast<QuickSettingsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  return false;
}

}  // namespace twsim::app
