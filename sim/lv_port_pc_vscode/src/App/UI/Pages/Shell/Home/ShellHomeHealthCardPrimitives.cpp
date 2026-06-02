#include "App/UI/Pages/Shell/Home/ShellHomeHealthCardPrimitives.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellImagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app::shell_home_health {

namespace {

struct HealthTileSpec {
  lv_obj_t** out_card;
  const char* icon_path;
  const char* value;
  lv_color_t bg;
  bool emphasize;
  lv_coord_t icon_x;
  lv_coord_t icon_y;
  lv_coord_t icon_size;
};

bool create_health_card(lv_obj_t* parent,
                        lv_coord_t x,
                        lv_coord_t y,
                        const HealthTileSpec& spec,
                        lv_obj_t*& out_card) {
  out_card = lv_obj_create(parent);
  if (out_card == nullptr) {
    return false;
  }

  ui_prepare_box(out_card);
  ui_apply_surface(out_card, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(out_card, 106, 106);
  lv_obj_align(out_card, LV_ALIGN_TOP_LEFT, x, y);
  lv_obj_set_style_radius(out_card, 24, 0);
  lv_obj_set_style_pad_all(out_card, 0, 0);
  lv_obj_set_style_bg_color(out_card, spec.bg, 0);
  lv_obj_set_style_border_width(out_card, 0, 0);

  if (create_contain_image(
          out_card, spec.icon_path, spec.icon_size, spec.icon_size, LV_ALIGN_TOP_LEFT, spec.icon_x, spec.icon_y) == nullptr) {
    return false;
  }

  lv_obj_t* value = lv_label_create(out_card);
  if (value == nullptr) {
    return false;
  }

  ui_prepare_label(value);
  ui_apply_text(value, TextStyle::Title);
  lv_obj_set_style_text_font(value, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(value, spec.emphasize ? lv_color_hex(0xD7FBFF) : lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_width(value, 78);
  lv_label_set_long_mode(value, LV_LABEL_LONG_DOT);
  lv_label_set_text(value, spec.value);
  lv_obj_align(value, LV_ALIGN_BOTTOM_LEFT, 12, -14);
  return true;
}

}  // namespace

bool create_health_cards(lv_obj_t* parent, HealthCardsView& out) {
  constexpr lv_coord_t kTileGap = 8;
  constexpr lv_coord_t kTileWidth = 106;
  constexpr lv_coord_t kStartY = 15;

  const HealthTileSpec tiles[] {
      {&out.heart_card, shell_asset::health_heart_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 36},
      {&out.blood_oxygen_card, shell_asset::health_spo2_asset_path(), "--", lv_color_hex(0xFF4F72), false, 18, 12, 40},
      {&out.breathing_card, shell_asset::health_breathe_asset_path(), "Breathe", lv_color_hex(0x4DBDFF), true, 14, 13, 39},
      {&out.stress_card, shell_asset::health_stress_asset_path(), "--", lv_color_hex(0x0D1222), false, 18, 16, 44},
  };

  for (int index = 0; index < 4; ++index) {
    const lv_coord_t x = static_cast<lv_coord_t>((index % 2) * (kTileWidth + kTileGap));
    const lv_coord_t y = static_cast<lv_coord_t>(kStartY + (index / 2) * (kTileWidth + kTileGap));
    if (!create_health_card(parent, x, y, tiles[index], *tiles[index].out_card)) {
      return false;
    }
  }

  return true;
}

}  // namespace twsim::app::shell_home_health
