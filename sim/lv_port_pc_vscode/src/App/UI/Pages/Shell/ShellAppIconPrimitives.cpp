#include "App/UI/Pages/Shell/ShellAppIconPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/UiStyles.h"

#include <cstring>
#include <initializer_list>

namespace twsim::app {

using shell_asset::file_exists;

lv_obj_t* create_app_round_icon(lv_obj_t* parent, const AppVisualSpec& spec, lv_coord_t size) {
  lv_obj_t* icon_root = lv_obj_create(parent);
  if (icon_root == nullptr) {
    return nullptr;
  }

  ui_prepare_box(icon_root);
  lv_obj_set_size(icon_root, size, size);
  lv_obj_set_style_radius(icon_root, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(icon_root, lv_color_hex(spec.icon_bg), 0);
  lv_obj_set_style_bg_opa(icon_root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(icon_root, 0, 0);
  lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(icon_root, LV_OBJ_FLAG_CLICKABLE);

  const char* asset_path = spec.icon_asset_path;
  if (file_exists(asset_path)) {
    lv_obj_t* image = lv_image_create(icon_root);
    if (image == nullptr) {
      return nullptr;
    }
    const lv_coord_t image_size = static_cast<lv_coord_t>(size - 12);
    lv_obj_set_size(image, image_size, image_size);
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
    lv_image_set_src(image, asset_path);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(image);
    return icon_root;
  }

  if (spec.target == PageId::AppNfc) {
    lv_obj_t* wallet_body = lv_obj_create(icon_root);
    lv_obj_t* wallet_flap = lv_obj_create(icon_root);
    lv_obj_t* wallet_slot = lv_obj_create(icon_root);
    if (wallet_body == nullptr || wallet_flap == nullptr || wallet_slot == nullptr) {
      return nullptr;
    }

    for (lv_obj_t* part : {wallet_body, wallet_flap, wallet_slot}) {
      ui_prepare_box(part);
      lv_obj_set_style_border_width(part, 0, 0);
      lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_set_size(wallet_body, 32, 22);
    lv_obj_set_style_bg_color(wallet_body, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(wallet_body, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_body, 6, 0);
    lv_obj_align(wallet_body, LV_ALIGN_CENTER, 0, 4);

    lv_obj_set_size(wallet_flap, 30, 8);
    lv_obj_set_style_bg_color(wallet_flap, lv_color_hex(0xFDE68A), 0);
    lv_obj_set_style_bg_opa(wallet_flap, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_flap, 4, 0);
    lv_obj_align(wallet_flap, LV_ALIGN_CENTER, 0, -6);

    lv_obj_set_size(wallet_slot, 8, 3);
    lv_obj_set_style_bg_color(wallet_slot, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_bg_opa(wallet_slot, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wallet_slot, 2, 0);
    lv_obj_align(wallet_slot, LV_ALIGN_CENTER, 6, 4);
    return icon_root;
  }

  lv_obj_t* label = lv_label_create(icon_root);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  ui_apply_text(label, TextStyle::Title);
  lv_obj_set_style_text_font(label,
                             std::strlen(spec.icon_text == nullptr ? "" : spec.icon_text) > 1 ? &lv_font_montserrat_14
                                                                                               : &lv_font_montserrat_18,
                             0);
  lv_obj_set_style_text_color(label, lv_color_hex(spec.icon_fg), 0);
  lv_label_set_text(label, spec.icon_text == nullptr ? "?" : spec.icon_text);
  lv_obj_remove_flag(label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(label);
  return icon_root;
}

}  // namespace twsim::app
