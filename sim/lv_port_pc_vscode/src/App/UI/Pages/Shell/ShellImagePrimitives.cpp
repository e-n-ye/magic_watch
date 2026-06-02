#include "App/UI/Pages/Shell/ShellImagePrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"

namespace twsim::app {

using shell_asset::file_exists;

lv_obj_t* create_contain_image(lv_obj_t* parent,
                               const char* path,
                               lv_coord_t width,
                               lv_coord_t height,
                               lv_align_t align,
                               lv_coord_t x,
                               lv_coord_t y) {
  lv_obj_t* image = lv_image_create(parent);
  if (image == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(image, width, height);
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
  if (file_exists(path)) {
    lv_image_set_src(image, path);
  } else {
    lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_align(image, align, x, y);
  return image;
}

}  // namespace twsim::app
