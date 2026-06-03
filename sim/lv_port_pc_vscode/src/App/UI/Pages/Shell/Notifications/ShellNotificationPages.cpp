#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Shell/Notifications/ShellNotificationPrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

using shell_asset::file_exists;
using shell_asset::payment_wechat_green_asset_path;
using shell_font::cjk_font_14;
using shell_font::cjk_font_16;

constexpr const char* kTextDismiss = "\xE5\xBF\xBD\xE7\x95\xA5";

}  // namespace

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

}  // namespace twsim::app
