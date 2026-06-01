#include "App/UI/Pages/Daily/StepsPagePrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

using shell_asset::file_exists;
using shell_asset::steps_icon_asset_path;

constexpr lv_coord_t kStepsScrollInset = 8;
constexpr lv_coord_t kStepsScrollTop = 8;
constexpr lv_coord_t kStepsScrollBottom = 0;

}  // namespace

lv_obj_t* create_steps_scroll_root(lv_obj_t* root, lv_coord_t screen_w, lv_coord_t screen_h) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(scroll, screen_w - kStepsScrollInset * 2, screen_h - kStepsScrollTop - kStepsScrollBottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, kStepsScrollTop);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  ui_set_flex_column(scroll, 0, 10);
  lv_obj_set_style_pad_top(scroll, 0, 0);
  lv_obj_set_style_pad_bottom(scroll, 10, 0);
  lv_obj_set_style_pad_left(scroll, 0, 0);
  lv_obj_set_style_pad_right(scroll, 0, 0);
  return scroll;
}

lv_obj_t* create_steps_icon(lv_obj_t* parent, lv_coord_t size, std::uint32_t bg, std::uint32_t fg, bool use_foot_asset) {
  lv_obj_t* icon = lv_obj_create(parent);
  if (icon == nullptr) {
    return nullptr;
  }
  ui_prepare_box(icon);
  lv_obj_set_size(icon, size, size);
  lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon, lv_color_hex(bg), 0);
  lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon, 0, 0);
  lv_obj_remove_flag(icon, LV_OBJ_FLAG_CLICKABLE);

  const char* asset_path = use_foot_asset ? steps_icon_asset_path() : nullptr;
  if (file_exists(asset_path)) {
    lv_obj_t* image = lv_image_create(icon);
    if (image != nullptr) {
      lv_image_set_src(image, asset_path);
      lv_obj_set_size(image, static_cast<lv_coord_t>(size * 2 / 3), static_cast<lv_coord_t>(size * 2 / 3));
      lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
      lv_obj_center(image);
      lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
      return icon;
    }
  }

  lv_obj_t* mark = lv_label_create(icon);
  if (mark == nullptr) {
    return icon;
  }
  ui_prepare_label(mark);
  lv_obj_set_style_text_font(mark, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(mark, lv_color_hex(fg), 0);
  lv_label_set_text(mark, use_foot_asset ? "S" : "i");
  lv_obj_center(mark);
  return icon;
}

}  // namespace twsim::app
