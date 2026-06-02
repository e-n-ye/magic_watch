#pragma once

#include "lvgl/lvgl.h"

namespace twsim::app::shell_home_payment {

struct PaymentCardsView {
  lv_obj_t* alipay_card {nullptr};
  lv_obj_t* wechat_card {nullptr};
};

struct NfcCardView {
  lv_obj_t* card {nullptr};
};

bool create_payment_cards(lv_obj_t* parent,
                          lv_color_t tile_bg,
                          lv_color_t tile_border,
                          const char* wechat_icon_path,
                          PaymentCardsView& out);

bool create_nfc_card_view(lv_obj_t* parent, lv_color_t card_bg, const char* asset_path, NfcCardView& out);

}  // namespace twsim::app::shell_home_payment
