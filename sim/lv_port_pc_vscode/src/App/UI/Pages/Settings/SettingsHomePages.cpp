#include "App/UI/Pages/SettingsPages.h"

#include <cstddef>
#include <cstdint>

#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextSettings = "\xE8\xAE\xBE\xE7\xBD\xAE";
constexpr const char* kTextDisplay = "\xE6\x98\xBE\xE7\xA4\xBA\xE4\xB8\x8E\xE4\xBA\xAE\xE5\xBA\xA6";
constexpr const char* kTextSound = "\xE5\xA3\xB0\xE9\x9F\xB3\xE4\xB8\x8E\xE6\x8C\xAF\xE5\x8A\xA8";
constexpr const char* kTextDnd = "\xE5\x8B\xBF\xE6\x89\xB0\xE6\xA8\xA1\xE5\xBC\x8F";
constexpr const char* kTextNotifications = "\xE6\xB6\x88\xE6\x81\xAF\xE9\x80\x9A\xE7\x9F\xA5";
constexpr const char* kTextAppLayout = "\xE5\xBA\x94\xE7\x94\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextSystemActions = "\xE7\xB3\xBB\xE7\xBB\x9F\xE6\x93\x8D\xE4\xBD\x9C";
constexpr const char* kTextAbout = "\xE5\x85\xB3\xE4\xBA\x8E";
constexpr const char* kTextBatterySettings = "电池";

constexpr lv_coord_t kListTop = 42;

}  // namespace

SettingsHomePage::SettingsHomePage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingsHome, kTextSettings, false),
      entries_ {{
          {LV_SYMBOL_TINT, 0xF0B323, kTextDisplay, PageId::SettingDisplay},
          {LV_SYMBOL_VOLUME_MAX, 0x22D67C, kTextSound, PageId::SettingSound},
          {LV_SYMBOL_BATTERY_FULL, 0x14C38E, kTextBatterySettings, PageId::SettingBattery},
          {LV_SYMBOL_MUTE, 0x3B82F6, kTextDnd, PageId::SettingDoNotDisturb},
          {LV_SYMBOL_BELL, 0x1DA1FF, kTextNotifications, PageId::SettingNotifications},
          {LV_SYMBOL_LIST, 0x3B82F6, kTextAppLayout, PageId::SettingAppLayout},
          {LV_SYMBOL_SETTINGS, 0x2AA8FF, kTextSystemActions, PageId::SettingSystemActions},
          {"i", 0x50A8FF, kTextAbout, PageId::SettingAbout},
      }} {}

lv_obj_t* SettingsHomePage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < entries_.size(); ++index) {
    const auto& entry = entries_[index];
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_add_event_cb(button, &SettingsHomePage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    if (create_home_tile_icon(button, entry.icon_text, entry.icon_bg) == nullptr) {
      return nullptr;
    }

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, entry.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 62, 0);
  }

  return root;
}

void SettingsHomePage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsHomePage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* button = lv_event_get_current_target_obj(event);
  if (button == nullptr || !click_guard_allows(button)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(button)));
  if (index >= self->entries_.size()) {
    return;
  }

  self->request_navigation({NavigationAction::Push, self->entries_[index].target});
}

}  // namespace twsim::app
