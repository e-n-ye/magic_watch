#include "App/UI/Pages/Health/HealthSwitchPrimitives.h"

#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

lv_obj_t* create_sleep_switch_track(lv_obj_t* parent) {
  lv_obj_t* track = lv_obj_create(parent);
  if (track == nullptr) {
    return nullptr;
  }
  ui_prepare_box(track);
  lv_obj_remove_flag(track, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(track, 64, 34);
  lv_obj_set_style_radius(track, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_border_width(track, 0, 0);

  lv_obj_t* thumb = lv_obj_create(track);
  if (thumb == nullptr) {
    return track;
  }
  ui_prepare_box(thumb);
  lv_obj_set_size(thumb, 28, 28);
  lv_obj_set_style_radius(thumb, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_border_width(thumb, 0, 0);
  lv_obj_set_style_bg_color(thumb, lv_color_hex(0xEAF6FF), 0);
  lv_obj_set_style_bg_opa(thumb, LV_OPA_COVER, 0);
  lv_obj_remove_flag(thumb, LV_OBJ_FLAG_CLICKABLE);
  return track;
}

void apply_sleep_switch_style(lv_obj_t* track, bool enabled) {
  if (track == nullptr) {
    return;
  }
  lv_obj_set_style_bg_color(track, lv_color_hex(enabled ? 0x14B8FF : 0x3A4F66), 0);
  lv_obj_set_style_bg_opa(track, LV_OPA_COVER, 0);

  lv_obj_t* thumb = lv_obj_get_child(track, 0);
  if (thumb == nullptr) {
    return;
  }
  lv_obj_align(thumb, enabled ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, enabled ? -3 : 3, 0);
}

}  // namespace twsim::app
