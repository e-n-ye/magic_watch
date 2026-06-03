#include "App/UI/Pages/Shell/Notifications/ShellNotificationPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"

namespace twsim::app::shell_notification {

using shell_asset::file_exists;
using shell_asset::payment_wechat_asset_path;
using shell_asset::payment_wechat_green_asset_path;

lv_color_t notification_card_color(NotificationCategory category) {
  return category == NotificationCategory::BatteryLow ? lv_color_hex(0x172636) : lv_color_hex(0x132033);
}

lv_color_t notification_accent_color(NotificationCategory category) {
  return category == NotificationCategory::BatteryLow ? lv_color_hex(0xFACC15) : lv_color_hex(0x22D3EE);
}

lv_color_t notification_read_card_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0x0E1824) : notification_card_color(item.category);
}

lv_color_t notification_primary_text_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0xD3DCE8) : lv_color_hex(0xF8FAFC);
}

lv_color_t notification_secondary_text_color(const NotificationItem& item) {
  return item.read ? lv_color_hex(0x94A3B8) : lv_color_hex(0xE2E8F0);
}

lv_obj_t* create_notification_icon(lv_obj_t* parent, const NotificationItem& item, bool compact) {
  lv_obj_t* holder = lv_obj_create(parent);
  if (holder == nullptr) {
    return nullptr;
  }

  const lv_coord_t size = compact ? 38 : 48;
  lv_obj_remove_flag(holder, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(holder, size, size);
  lv_obj_set_style_border_width(holder, 0, 0);
  lv_obj_set_style_radius(holder, compact ? 14 : 18, 0);
  lv_obj_set_style_pad_all(holder, 0, 0);

  if (item.category == NotificationCategory::Message && file_exists(payment_wechat_green_asset_path())) {
    lv_obj_set_style_bg_color(holder, lv_color_hex(0x19C37D), 0);
    lv_obj_set_style_bg_opa(holder, LV_OPA_COVER, 0);
    lv_obj_t* image = lv_image_create(holder);
    if (image == nullptr) {
      return holder;
    }
    lv_image_set_src(image, payment_wechat_green_asset_path());
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_STRETCH);
    lv_obj_set_size(image, compact ? 24 : 30, compact ? 24 : 30);
    lv_obj_center(image);
    return holder;
  }

  lv_obj_set_style_bg_color(holder,
                            item.category == NotificationCategory::BatteryLow ? lv_color_hex(0x8A6A00)
                                                                              : lv_color_hex(0x1D4ED8),
                            0);
  lv_obj_set_style_bg_opa(holder, LV_OPA_COVER, 0);

  lv_obj_t* label = lv_label_create(holder);
  if (label == nullptr) {
    return holder;
  }
  lv_obj_set_style_text_font(label, compact ? &lv_font_montserrat_14 : &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(label,
                    item.category == NotificationCategory::BatteryLow
                        ? (item.badge_text.empty() ? LV_SYMBOL_CHARGE : item.badge_text.c_str())
                        : LV_SYMBOL_BELL);
  lv_obj_center(label);
  return holder;
}

}  // namespace twsim::app::shell_notification
