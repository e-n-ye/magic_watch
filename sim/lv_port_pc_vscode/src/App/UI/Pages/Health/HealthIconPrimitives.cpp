#include "App/UI/Pages/Health/HealthIconPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

using shell_asset::file_exists;

lv_obj_t* create_sleep_round_icon(lv_obj_t* parent,
                                  lv_coord_t size,
                                  std::uint32_t bg,
                                  std::uint32_t fg,
                                  const char* asset_path,
                                  const char* fallback_text,
                                  const lv_font_t* fallback_font) {
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

  lv_obj_t* label = lv_label_create(icon);
  if (label == nullptr) {
    return icon;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, fallback_font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(fg), 0);
  lv_label_set_text(label, fallback_text);
  lv_obj_center(label);
  return icon;
}

}  // namespace twsim::app
