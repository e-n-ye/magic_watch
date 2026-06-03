#pragma once

#include "App/Common/AppEvents.h"

#include "lvgl/lvgl.h"

namespace twsim::app::shell_notification {

lv_color_t notification_card_color(NotificationCategory category);
lv_color_t notification_accent_color(NotificationCategory category);
lv_color_t notification_read_card_color(const NotificationItem& item);
lv_color_t notification_primary_text_color(const NotificationItem& item);
lv_color_t notification_secondary_text_color(const NotificationItem& item);
lv_obj_t* create_notification_icon(lv_obj_t* parent, const NotificationItem& item, bool compact);

}  // namespace twsim::app::shell_notification
