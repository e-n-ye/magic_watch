#include "App/UI/Pages/Shell/Home/ShellHomeLayoutPrimitives.h"

#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app::shell_home {

lv_obj_t* create_pager_dot(lv_obj_t* parent, lv_color_t color, lv_opa_t opa, bool active) {
  lv_obj_t* dot = lv_obj_create(parent);
  if (dot == nullptr) {
    return nullptr;
  }

  ui_prepare_box(dot);
  lv_obj_set_size(dot, active ? 9 : 7, active ? 9 : 7);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, color, 0);
  lv_obj_set_style_bg_opa(dot, active ? LV_OPA_70 : opa, 0);
  return dot;
}

SurfaceLayout make_surface_layout() {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t safe_margin_x = clamp_coord(scale_by_ratio(screen_w, 4, 100), 8, 10);
  const lv_coord_t chip_top = clamp_coord(scale_by_ratio(screen_h, 4, 100), 8, 12);
  const lv_coord_t stage_top = clamp_coord(scale_by_ratio(screen_h, 7, 100), 18, 22);
  const lv_coord_t stage_w = screen_w - safe_margin_x * 2;
  const lv_coord_t stage_h = clamp_coord(scale_by_ratio(screen_h, 82, 100), 220, 244);
  const lv_coord_t stage_radius = clamp_coord(scale_by_ratio(stage_w, 10, 100), 22, 26);
  const lv_coord_t stage_pad = clamp_coord(scale_by_ratio(stage_w, 3, 100), 6, 10);
  const lv_coord_t stage_gap = clamp_coord(scale_by_ratio(stage_h, 2, 100), 4, 8);
  const lv_coord_t title_h = clamp_coord(scale_by_ratio(stage_h, 8, 100), 16, 20);
  const lv_coord_t hero_h = clamp_coord(scale_by_ratio(stage_h, 32, 100), 54, 64);
  const lv_coord_t card_gap = clamp_coord(scale_by_ratio(stage_w, 3, 100), 6, 10);
  const lv_coord_t available_h = stage_h - stage_pad * 2 - title_h - hero_h - stage_gap * 2;
  const lv_coord_t card_w = (stage_w - stage_pad * 2 - card_gap) / 2;
  const lv_coord_t card_h = (available_h - card_gap) / 2;
  const lv_coord_t pager_bottom = clamp_coord(scale_by_ratio(screen_h, 2, 100), 2, 8);
  return {screen_w,
          screen_h,
          safe_margin_x,
          chip_top,
          safe_margin_x,
          stage_top,
          stage_w,
          stage_h,
          stage_radius,
          stage_pad,
          stage_gap,
          title_h,
          hero_h,
          card_gap,
          card_w,
          card_h,
          pager_bottom};
}

void style_surface_stage(lv_obj_t* stage,
                         lv_coord_t width,
                         lv_coord_t height,
                         lv_coord_t radius,
                         lv_color_t bg_color) {
  ui_prepare_box(stage);
  ui_apply_surface(stage, SurfaceStyle::Panel);
  lv_obj_set_size(stage, width, height);
  lv_obj_set_style_bg_color(stage, bg_color, 0);
  lv_obj_set_style_border_width(stage, 0, 0);
  lv_obj_set_style_radius(stage, radius, 0);
  lv_obj_set_style_pad_all(stage, 0, 0);
}

lv_obj_t* create_stage_root(lv_obj_t* root, const SurfaceLayout& layout, lv_color_t stage_bg) {
  lv_obj_t* stage = lv_obj_create(root);
  if (stage == nullptr) {
    return nullptr;
  }
  style_surface_stage(stage, layout.stage_w, layout.stage_h, layout.stage_radius, stage_bg);
  lv_obj_align(stage, LV_ALIGN_TOP_MID, 0, layout.stage_top);
  lv_obj_set_style_shadow_width(stage, 0, 0);
  lv_obj_set_style_shadow_opa(stage, LV_OPA_TRANSP, 0);
  return stage;
}

void style_pager_root(lv_obj_t* pager_root, const SurfaceLayout& layout) {
  ui_prepare_box(pager_root);
  lv_obj_set_size(pager_root, 54, 14);
  lv_obj_set_style_bg_opa(pager_root, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(pager_root, 0, 0);
  lv_obj_set_style_pad_all(pager_root, 0, 0);
  lv_obj_align(pager_root, LV_ALIGN_BOTTOM_MID, 0, -layout.pager_bottom);
  lv_obj_add_flag(pager_root, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(pager_root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(pager_root, LV_OBJ_FLAG_CLICKABLE);
}

void set_single_line_label(lv_obj_t* label, lv_coord_t width) {
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
}

}  // namespace twsim::app::shell_home
