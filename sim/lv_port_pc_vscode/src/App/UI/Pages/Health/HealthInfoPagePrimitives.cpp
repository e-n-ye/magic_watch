#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"

#include <cstdio>

#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

void apply_compact_time_label(lv_obj_t* label, const std::optional<TimeModel>& model) {
  if (label == nullptr) {
    return;
  }

  char buffer[8] = "--:--";
  if (model.has_value() && model->valid) {
    std::snprintf(buffer, sizeof(buffer), "%02u:%02u", model->hour, model->minute);
  }
  lv_label_set_text(label, buffer);
}

lv_obj_t* create_steps_label(lv_obj_t* parent,
                             const char* text,
                             const lv_font_t* font,
                             std::uint32_t color,
                             lv_coord_t width,
                             lv_label_long_mode_t long_mode) {
  lv_obj_t* label = lv_label_create(parent);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, long_mode);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_text(label, text);
  return label;
}

lv_obj_t* create_steps_panel(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, std::uint32_t bg) {
  lv_obj_t* panel = lv_obj_create(parent);
  if (panel == nullptr) {
    return nullptr;
  }
  ui_prepare_box(panel);
  ui_apply_surface(panel, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(panel, width, height);
  lv_obj_set_style_bg_color(panel, lv_color_hex(bg), 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0x17304A), 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_radius(panel, clamp_coord(scale_by_ratio(width, 12, 100), 22, 28), 0);
  lv_obj_set_style_shadow_width(panel, 0, 0);
  return panel;
}

lv_obj_t* create_sleep_scroll_root(lv_obj_t* root,
                                   lv_coord_t screen_w,
                                   lv_coord_t screen_h,
                                   lv_coord_t top,
                                   lv_coord_t bottom,
                                   lv_coord_t gap) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(scroll, screen_w - 16, screen_h - top - bottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, top);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  ui_set_flex_column(scroll, 0, gap);
  lv_obj_set_style_pad_top(scroll, 0, 0);
  lv_obj_set_style_pad_bottom(scroll, 10, 0);
  lv_obj_set_style_pad_left(scroll, 0, 0);
  lv_obj_set_style_pad_right(scroll, 0, 0);
  return scroll;
}

}  // namespace twsim::app
