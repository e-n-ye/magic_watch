#include "App/UI/Pages/Shell/Notifications/ShellNotificationPagePrimitives.h"

#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/UiStyles.h"

#include <initializer_list>
#include <tuple>

namespace twsim::app::shell_notification_page {

namespace {

using shell_font::cjk_font_14;
using shell_font::cjk_font_16;

constexpr const char* kTextClear = "\xE6\xB8\x85\xE7\xA9\xBA";
constexpr const char* kTextConfirm = "\xE7\xA1\xAE\xE8\xAE\xA4";
constexpr const char* kTextCancel = "\xE5\x8F\x96\xE6\xB6\x88";
constexpr const char* kTextNoMessages = "\xE6\x9A\x82\xE6\x97\xA0\xE6\xB6\x88\xE6\x81\xAF";
constexpr const char* kTextNotificationClearConfirmBody =
    "\xE6\xB8\x85\xE7\xA9\xBA\xE5\x90\x8E\xE5\xB0\x86\xE7\xA7\xBB\xE9\x99\xA4\xE5\xBD\x93\xE5\x89\x8D\xE9\x80\x9A"
    "\xE7\x9F\xA5\xE5\x88\x97\xE8\xA1\xA8\xE3\x80\x82\xE7\xA1\xAE\xE5\xAE\x9A\xE6\xB8\x85\xE7\xA9\xBA\xEF\xBC\x9F";

bool build_clear_button(lv_obj_t* parent, lv_obj_t*& out_button) {
  out_button = lv_button_create(parent);
  if (out_button == nullptr) {
    return false;
  }

  lv_obj_set_size(out_button, 208, 46);
  lv_obj_align(out_button, LV_ALIGN_TOP_MID, 0, 18);
  lv_obj_set_style_bg_color(out_button, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(out_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out_button, 0, 0);
  lv_obj_set_style_radius(out_button, 22, 0);

  lv_obj_t* clear_label = lv_label_create(out_button);
  if (clear_label == nullptr) {
    return false;
  }
  lv_obj_set_style_text_font(clear_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(clear_label, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(clear_label, kTextClear);
  lv_obj_center(clear_label);
  return true;
}

bool build_empty_state(lv_obj_t* parent, lv_obj_t*& out_empty_state) {
  out_empty_state = lv_obj_create(parent);
  if (out_empty_state == nullptr) {
    return false;
  }

  lv_obj_remove_flag(out_empty_state, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(out_empty_state, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(out_empty_state, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(out_empty_state, 0, 0);
  lv_obj_set_style_pad_all(out_empty_state, 0, 0);

  lv_obj_t* bubble = lv_obj_create(out_empty_state);
  if (bubble == nullptr) {
    return false;
  }
  lv_obj_remove_flag(bubble, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(bubble, 68, 68);
  lv_obj_align(bubble, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_bg_color(bubble, lv_color_hex(0x1D4ED8), 0);
  lv_obj_set_style_bg_opa(bubble, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(bubble, 0, 0);
  lv_obj_set_style_radius(bubble, LV_RADIUS_CIRCLE, 0);

  lv_obj_t* bubble_label = lv_label_create(bubble);
  lv_obj_t* empty_text = lv_label_create(out_empty_state);
  if (bubble_label == nullptr || empty_text == nullptr) {
    return false;
  }
  lv_obj_set_style_text_font(bubble_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(bubble_label, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(bubble_label, LV_SYMBOL_BELL);
  lv_obj_center(bubble_label);

  lv_obj_set_style_text_font(empty_text, cjk_font_16(), 0);
  lv_obj_set_style_text_color(empty_text, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(empty_text, kTextNoMessages);
  lv_obj_align(empty_text, LV_ALIGN_CENTER, 0, 34);
  return true;
}

bool build_detail_root(lv_obj_t* parent, NotificationsPagePrimitivesView& out) {
  out.detail_root = lv_obj_create(parent);
  if (out.detail_root == nullptr) {
    return false;
  }

  lv_obj_remove_flag(out.detail_root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(out.detail_root, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_opa(out.detail_root, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(out.detail_root, 0, 0);
  lv_obj_set_style_pad_all(out.detail_root, 0, 0);
  lv_obj_add_flag(out.detail_root, LV_OBJ_FLAG_HIDDEN);

  out.detail_back_button = lv_button_create(out.detail_root);
  lv_obj_t* detail_card = lv_obj_create(out.detail_root);
  if (out.detail_back_button == nullptr || detail_card == nullptr) {
    return false;
  }

  lv_obj_set_size(out.detail_back_button, 48, 34);
  lv_obj_align(out.detail_back_button, LV_ALIGN_TOP_LEFT, 14, 16);
  lv_obj_set_style_bg_color(out.detail_back_button, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_bg_opa(out.detail_back_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out.detail_back_button, 0, 0);
  lv_obj_set_style_radius(out.detail_back_button, 16, 0);

  lv_obj_t* detail_back_label = lv_label_create(out.detail_back_button);
  if (detail_back_label == nullptr) {
    return false;
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

  out.detail_source_label = lv_label_create(detail_card);
  out.detail_title_label = lv_label_create(detail_card);
  out.detail_body_label = lv_label_create(detail_card);
  out.detail_time_label = lv_label_create(detail_card);
  if (out.detail_source_label == nullptr || out.detail_title_label == nullptr || out.detail_body_label == nullptr ||
      out.detail_time_label == nullptr) {
    return false;
  }

  lv_obj_set_style_text_font(out.detail_source_label, cjk_font_14(), 0);
  lv_obj_set_style_text_color(out.detail_source_label, lv_color_hex(0x67E8F9), 0);
  lv_obj_align(out.detail_source_label, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_set_width(out.detail_title_label, 178);
  lv_label_set_long_mode(out.detail_title_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(out.detail_title_label, cjk_font_16(), 0);
  lv_obj_set_style_text_color(out.detail_title_label, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(out.detail_title_label, LV_ALIGN_TOP_LEFT, 0, 26);

  lv_obj_set_width(out.detail_body_label, 178);
  lv_label_set_long_mode(out.detail_body_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(out.detail_body_label, cjk_font_14(), 0);
  lv_obj_set_style_text_color(out.detail_body_label, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(out.detail_body_label, LV_ALIGN_TOP_LEFT, 0, 78);

  lv_obj_set_style_text_font(out.detail_time_label, cjk_font_14(), 0);
  lv_obj_set_style_text_color(out.detail_time_label, lv_color_hex(0xCBD5E1), 0);
  lv_obj_align(out.detail_time_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  return true;
}

bool build_clear_confirm_overlay(lv_obj_t* root, NotificationsPagePrimitivesView& out) {
  out.clear_confirm_overlay = lv_obj_create(root);
  if (out.clear_confirm_overlay == nullptr) {
    return false;
  }

  ui_prepare_box(out.clear_confirm_overlay);
  lv_obj_set_size(out.clear_confirm_overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_align(out.clear_confirm_overlay, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(out.clear_confirm_overlay, lv_color_hex(0x02060D), 0);
  lv_obj_set_style_bg_opa(out.clear_confirm_overlay, LV_OPA_90, 0);
  lv_obj_set_style_border_width(out.clear_confirm_overlay, 0, 0);
  lv_obj_set_style_radius(out.clear_confirm_overlay, 0, 0);
  lv_obj_add_flag(out.clear_confirm_overlay, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(out.clear_confirm_overlay);

  lv_obj_t* confirm_body = lv_label_create(out.clear_confirm_overlay);
  out.clear_confirm_cancel_button = lv_button_create(out.clear_confirm_overlay);
  out.clear_confirm_confirm_button = lv_button_create(out.clear_confirm_overlay);
  if (confirm_body == nullptr || out.clear_confirm_cancel_button == nullptr || out.clear_confirm_confirm_button == nullptr) {
    return false;
  }

  lv_obj_set_width(confirm_body, 176);
  lv_label_set_long_mode(confirm_body, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(confirm_body, cjk_font_16(), 0);
  lv_obj_set_style_text_color(confirm_body, lv_color_hex(0xF8FAFC), 0);
  lv_label_set_text(confirm_body, kTextNotificationClearConfirmBody);
  lv_obj_set_style_text_align(confirm_body, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(confirm_body, LV_ALIGN_CENTER, 0, -28);

  for (const auto [button, text, accent] : {
           std::tuple {out.clear_confirm_cancel_button, kTextCancel, lv_color_hex(0x334155)},
           std::tuple {out.clear_confirm_confirm_button, kTextConfirm, lv_color_hex(0x2563EB)}}) {
    lv_obj_set_size(button, 82, 44);
    lv_obj_set_style_bg_color(button, accent, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 20, 0);
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return false;
    }
    lv_obj_set_style_text_font(label, cjk_font_16(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF8FAFC), 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
  }

  lv_obj_align(out.clear_confirm_cancel_button, LV_ALIGN_CENTER, -48, 64);
  lv_obj_align(out.clear_confirm_confirm_button, LV_ALIGN_CENTER, 48, 64);
  return true;
}

bool build_drag_handle(lv_obj_t* parent, lv_obj_t*& out_drag_handle) {
  out_drag_handle = lv_obj_create(parent);
  if (out_drag_handle == nullptr) {
    return false;
  }

  lv_obj_remove_flag(out_drag_handle, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(out_drag_handle, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(out_drag_handle, 46, 7);
  lv_obj_align(out_drag_handle, LV_ALIGN_BOTTOM_MID, 0, -14);
  lv_obj_set_style_bg_color(out_drag_handle, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_bg_opa(out_drag_handle, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(out_drag_handle, 0, 0);
  lv_obj_set_style_radius(out_drag_handle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_shadow_width(out_drag_handle, 16, 0);
  lv_obj_set_style_shadow_color(out_drag_handle, lv_color_hex(0x60A5FA), 0);
  lv_obj_set_style_shadow_opa(out_drag_handle, static_cast<lv_opa_t>(140), 0);
  return true;
}

}  // namespace

bool create_notifications_page_primitives(lv_obj_t* root,
                                          lv_obj_t* sheet_container,
                                          NotificationsPagePrimitivesView& out) {
  if (root == nullptr || sheet_container == nullptr) {
    return false;
  }

  out.list_root = lv_obj_create(sheet_container);
  if (!build_clear_button(sheet_container, out.clear_button) || out.list_root == nullptr ||
      !build_empty_state(sheet_container, out.empty_state) || !build_detail_root(sheet_container, out) ||
      !build_clear_confirm_overlay(root, out) || !build_drag_handle(sheet_container, out.drag_handle)) {
    return false;
  }

  lv_obj_set_size(out.list_root, 208, 156);
  lv_obj_align(out.list_root, LV_ALIGN_TOP_MID, 0, 76);
  lv_obj_set_flex_flow(out.list_root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(out.list_root, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(out.list_root, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(out.list_root, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(out.list_root, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(out.list_root, 0, 0);
  lv_obj_set_style_pad_row(out.list_root, 10, 0);
  lv_obj_set_style_bg_opa(out.list_root, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(out.list_root, 0, 0);
  lv_obj_set_style_radius(out.list_root, 0, 0);
  return true;
}

}  // namespace twsim::app::shell_notification_page
