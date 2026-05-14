#include "App/UI/Assets/MonicaAssets.h"

extern "C" {
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_0_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_1_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_2_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_3_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_4_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_5_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_6_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_7_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_8_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_9_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_10_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_11_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_12_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_13_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_14_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_15_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_16_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_17_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_18_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_19_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_20_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_21_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_22_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_23_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_24_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_25_png);
LV_IMAGE_DECLARE(ui_img_anim_spring_wreath_26_png);
}

namespace {

constexpr std::size_t kFrameCount = 27;

const lv_image_dsc_t* const kSpringWreathFrames[kFrameCount] = {
    &ui_img_anim_spring_wreath_0_png,
    &ui_img_anim_spring_wreath_1_png,
    &ui_img_anim_spring_wreath_2_png,
    &ui_img_anim_spring_wreath_3_png,
    &ui_img_anim_spring_wreath_4_png,
    &ui_img_anim_spring_wreath_5_png,
    &ui_img_anim_spring_wreath_6_png,
    &ui_img_anim_spring_wreath_7_png,
    &ui_img_anim_spring_wreath_8_png,
    &ui_img_anim_spring_wreath_9_png,
    &ui_img_anim_spring_wreath_10_png,
    &ui_img_anim_spring_wreath_11_png,
    &ui_img_anim_spring_wreath_12_png,
    &ui_img_anim_spring_wreath_13_png,
    &ui_img_anim_spring_wreath_14_png,
    &ui_img_anim_spring_wreath_15_png,
    &ui_img_anim_spring_wreath_16_png,
    &ui_img_anim_spring_wreath_17_png,
    &ui_img_anim_spring_wreath_18_png,
    &ui_img_anim_spring_wreath_19_png,
    &ui_img_anim_spring_wreath_20_png,
    &ui_img_anim_spring_wreath_21_png,
    &ui_img_anim_spring_wreath_22_png,
    &ui_img_anim_spring_wreath_23_png,
    &ui_img_anim_spring_wreath_24_png,
    &ui_img_anim_spring_wreath_25_png,
    &ui_img_anim_spring_wreath_26_png,
};

}  // namespace

namespace twsim::app {

const lv_image_dsc_t* const* monica_spring_wreath_frames() {
  return kSpringWreathFrames;
}

std::size_t monica_spring_wreath_frame_count() {
  return kFrameCount;
}

bool monica_spring_wreath_available() {
  return true;
}

const char* monica_spring_wreath_status() {
  return "Monica wreath ready";
}

}  // namespace twsim::app
