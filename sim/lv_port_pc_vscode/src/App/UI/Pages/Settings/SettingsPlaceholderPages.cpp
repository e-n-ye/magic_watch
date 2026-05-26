#include "App/UI/Pages/SettingsPages.h"

#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextSettings = "\xE8\xAE\xBE\xE7\xBD\xAE";
constexpr const char* kTextComingSoon =
    "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE4\xBF\x9D\xE7\x95\x99\xE9\xA1\xB5\xE9\x9D\xA2\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x8C"
    "\xE5\x90\x8E\xE7\xBB\xAD\xE6\x8C\x89\xE5\xB0\x8F\xE9\x97\xAD\xE7\x8E\xAF\xE7\xBB\xA7\xE7\xBB\xAD\xE8\xA1\xA5\xE9\xBD\x90\xE3\x80\x82";
constexpr const char* kTextPlaceholderTag = "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x8D\xA0\xE4\xBD\x8D";

constexpr lv_coord_t kListTop = 42;

TileVisual visual_for_placeholder_page(PageId page_id) {
  switch (page_id) {
    case PageId::SettingDisplay:
      return {LV_SYMBOL_TINT, 0xF0B323};
    case PageId::SettingSound:
      return {LV_SYMBOL_VOLUME_MAX, 0x22D67C};
    case PageId::SettingDoNotDisturb:
      return {LV_SYMBOL_MUTE, 0x3B82F6};
    case PageId::SettingNotifications:
      return {LV_SYMBOL_BELL, 0x1DA1FF};
    case PageId::SettingAppLayout:
      return {LV_SYMBOL_LIST, 0x3B82F6};
    case PageId::SettingSystemActions:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
    case PageId::SettingAbout:
      return {"i", 0x50A8FF};
    case PageId::SettingBattery:
      return {LV_SYMBOL_BATTERY_FULL, 0x14C38E};
    default:
      return {LV_SYMBOL_SETTINGS, 0x2AA8FF};
  }
}

}  // namespace

SettingsPlaceholderPage::SettingsPlaceholderPage(DataCenter& data_center,
                                                 PageId page_id,
                                                 const char* title,
                                                 const char* detail)
    : SettingsPageBase(data_center, page_id, title, true), detail_(detail) {}

lv_obj_t* SettingsPlaceholderPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  const TileVisual visual = visual_for_placeholder_page(id());
  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_t* card = create_card_shell(scroll, false);
  if (card == nullptr) {
    return nullptr;
  }
  if (create_home_tile_icon(card, visual.icon_text, visual.icon_bg) == nullptr) {
    return nullptr;
  }

  lv_obj_t* label = lv_label_create(card);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_style_text_font(label, cjk_font_20(), 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
  lv_label_set_text(label, title_text());
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);

  if (create_note_card(scroll, kTextPlaceholderTag, detail_) == nullptr) {
    return nullptr;
  }
  if (create_note_card(scroll, kTextSettings, kTextComingSoon) == nullptr) {
    return nullptr;
  }

  return root;
}

}  // namespace twsim::app
