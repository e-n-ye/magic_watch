#include "App/UI/Pages/Shell/Home/ShellHomePaymentCardPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/UiStyles.h"

namespace twsim::app::shell_home_payment {

namespace {

lv_obj_t* create_cover_image(lv_obj_t* parent,
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
  lv_image_set_inner_align(image, LV_IMAGE_ALIGN_COVER);
  if (shell_asset::file_exists(path)) {
    lv_image_set_src(image, path);
  } else {
    lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_align(image, align, x, y);
  return image;
}

bool build_payment_tile_content(lv_obj_t* card,
                                const char* icon_path,
                                lv_coord_t icon_x,
                                lv_coord_t icon_y,
                                lv_coord_t icon_size,
                                const char* label_text,
                                lv_coord_t label_x,
                                lv_coord_t label_y,
                                lv_coord_t label_w) {
  lv_obj_t* icon = lv_image_create(card);
  lv_obj_t* label = lv_label_create(card);
  if (icon == nullptr || label == nullptr) {
    return false;
  }
  lv_obj_set_size(icon, icon_size, icon_size);
  lv_image_set_inner_align(icon, LV_IMAGE_ALIGN_CONTAIN);
  lv_image_set_src(icon, icon_path);
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, icon_x, icon_y);

  ui_prepare_label(label);
  ui_apply_text(label, TextStyle::Title);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_width(label, label_w);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_label_set_text(label, label_text);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, label_x, label_y);
  return true;
}

}  // namespace

bool create_payment_cards(lv_obj_t* parent,
                          lv_color_t tile_bg,
                          lv_color_t tile_border,
                          const char* wechat_icon_path,
                          PaymentCardsView& out) {
  constexpr lv_coord_t kSmallW = 106;
  constexpr lv_coord_t kTopY = 15;
  constexpr lv_coord_t kCardGap = 8;

  out.alipay_card = lv_obj_create(parent);
  out.wechat_card = lv_obj_create(parent);
  if (out.alipay_card == nullptr || out.wechat_card == nullptr) {
    return false;
  }

  ui_prepare_box(out.alipay_card);
  ui_apply_surface(out.alipay_card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(out.alipay_card, kSmallW, 106);
  lv_obj_set_style_radius(out.alipay_card, 24, 0);
  lv_obj_set_style_pad_all(out.alipay_card, 0, 0);
  lv_obj_set_style_bg_color(out.alipay_card, tile_bg, 0);
  lv_obj_set_style_border_color(out.alipay_card, tile_border, 0);

  ui_prepare_box(out.wechat_card);
  ui_apply_surface(out.wechat_card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(out.wechat_card, kSmallW, 106);
  lv_obj_set_style_radius(out.wechat_card, 24, 0);
  lv_obj_set_style_pad_all(out.wechat_card, 0, 0);
  lv_obj_set_style_bg_color(out.wechat_card, tile_bg, 0);
  lv_obj_set_style_border_color(out.wechat_card, tile_border, 0);
  lv_obj_align(out.alipay_card, LV_ALIGN_TOP_LEFT, 0, kTopY);
  lv_obj_align(out.wechat_card, LV_ALIGN_TOP_LEFT, kSmallW + kCardGap, kTopY);

  return build_payment_tile_content(
             out.alipay_card, shell_asset::payment_alipay_asset_path(), 2, 2, 49, "Alipay", 16, 72, 72) &&
         build_payment_tile_content(out.wechat_card, wechat_icon_path, 7, 5, 40, "WeChat\nPay", 16, 58, 74);
}

bool create_nfc_card_view(lv_obj_t* parent, lv_color_t card_bg, const char* asset_path, NfcCardView& out) {
  lv_obj_t* title = lv_label_create(parent);
  lv_obj_t* subtitle = lv_label_create(parent);
  out.card = lv_obj_create(parent);
  if (title == nullptr || subtitle == nullptr || out.card == nullptr) {
    return false;
  }

  ui_prepare_label(title);
  ui_apply_text(title, TextStyle::HeroSoft);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_width(title, 93);
  lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(title, "School");
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 57, 18);

  ui_prepare_label(subtitle);
  ui_apply_text(subtitle, TextStyle::Title);
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x3B82F6), 0);
  lv_obj_set_width(subtitle, 151);
  lv_obj_set_style_text_align(subtitle, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(subtitle, "Tap card reader");
  lv_obj_align(subtitle, LV_ALIGN_TOP_LEFT, 30, 47);

  ui_prepare_box(out.card);
  ui_apply_surface(out.card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(out.card, 208, 120);
  lv_obj_align(out.card, LV_ALIGN_TOP_LEFT, 6, 75);
  lv_obj_set_style_radius(out.card, 28, 0);
  lv_obj_set_style_pad_all(out.card, 0, 0);
  lv_obj_set_style_bg_color(out.card, card_bg, 0);
  lv_obj_set_style_border_width(out.card, 0, 0);
  lv_obj_set_style_clip_corner(out.card, true, 0);

  return create_cover_image(out.card, asset_path, 209, 124, LV_ALIGN_TOP_LEFT, -2, -4) != nullptr;
}

}  // namespace twsim::app::shell_home_payment
