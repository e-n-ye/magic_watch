#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app::shell_notification_page {

struct NotificationsPagePrimitivesView {
  lv_obj_t* clear_button {nullptr};
  lv_obj_t* list_root {nullptr};
  lv_obj_t* empty_state {nullptr};
  lv_obj_t* detail_root {nullptr};
  lv_obj_t* detail_back_button {nullptr};
  lv_obj_t* detail_source_label {nullptr};
  lv_obj_t* detail_title_label {nullptr};
  lv_obj_t* detail_body_label {nullptr};
  lv_obj_t* detail_time_label {nullptr};
  lv_obj_t* clear_confirm_overlay {nullptr};
  lv_obj_t* clear_confirm_cancel_button {nullptr};
  lv_obj_t* clear_confirm_confirm_button {nullptr};
  lv_obj_t* drag_handle {nullptr};
};

bool create_notifications_page_primitives(lv_obj_t* root,
                                          lv_obj_t* sheet_container,
                                          NotificationsPagePrimitivesView& out);

}  // namespace twsim::app::shell_notification_page
