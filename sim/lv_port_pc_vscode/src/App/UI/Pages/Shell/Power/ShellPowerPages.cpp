#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Shell/ShellPagePrimitives.h"

#include <array>
#include <cstdint>
#include <utility>

namespace twsim::app {

PowerMenuPage::PowerMenuPage(DataCenter& data_center) : PageBase(data_center) {}

PageId PowerMenuPage::id() const {
  return PageId::PowerMenu;
}

const char* PowerMenuPage::name() const {
  return "PowerMenu";
}

lv_obj_t* PowerMenuPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x05080F);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  lv_obj_t* list = lv_obj_create(root);
  if (title == nullptr || subtitle == nullptr || list == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Power");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  lv_label_set_text(subtitle, "Long press 5 opens this page");
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

  lv_obj_set_size(list, 190, 130);
  lv_obj_center(list);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(list, 8, 0);
  lv_obj_set_style_pad_gap(list, 8, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(list, 1, 0);
  lv_obj_set_style_border_color(list, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(list, 20, 0);
  lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLLABLE);

  const std::array<std::pair<const char*, NavigationCommand>, 3> buttons {{
      {"Power Off", {NavigationAction::PowerOff, PageId::PoweredOff}},
      {"Restart", {NavigationAction::Restart, PageId::Watchface}},
      {"Cancel", {NavigationAction::ReturnHome, PageId::Watchface}},
  }};

  for (std::size_t index = 0; index < buttons.size(); ++index) {
    lv_obj_t* button = lv_button_create(list);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_set_height(button, 34);
    lv_obj_set_style_bg_color(button,
                              index == 0 ? lv_color_hex(0x6F1D1B)
                                         : (index == 1 ? lv_color_hex(0x5A401A) : lv_color_hex(0x1F3C5A)),
                              0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 14, 0);
    lv_obj_add_event_cb(button, &PowerMenuPage::action_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    lv_label_set_text(label, buttons[index].first);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
  }

  return root;
}

void PowerMenuPage::action_event_cb(lv_event_t* event) {
  auto* self = static_cast<PowerMenuPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  switch (index) {
    case 0:
      self->request_navigation({NavigationAction::PowerOff, PageId::PoweredOff});
      break;
    case 1:
      self->request_navigation({NavigationAction::Restart, PageId::Watchface});
      break;
    case 2:
    default:
      self->request_navigation({NavigationAction::ReturnHome, PageId::Watchface});
      break;
  }
}

PassiveShellPage::PassiveShellPage(DataCenter& data_center, PageId page_id, const char* title, const char* detail)
    : PageBase(data_center), page_id_(page_id), title_(title), detail_(detail) {}

PageId PassiveShellPage::id() const {
  return page_id_;
}

const char* PassiveShellPage::name() const {
  return page_name(page_id_);
}

lv_obj_t* PassiveShellPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x000000);

  if (page_id_ == PageId::ScreenOff) {
    lv_obj_t* debug_hint = lv_label_create(root);
    if (debug_hint == nullptr) {
      return root;
    }
    lv_label_set_text(debug_hint, "screen off");
    lv_obj_set_style_text_font(debug_hint, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(debug_hint, lv_color_hex(0x101010), 0);
    lv_obj_align(debug_hint, LV_ALIGN_BOTTOM_MID, 0, -8);
    return root;
  }

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* detail = lv_label_create(root);
  if (title == nullptr || detail == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, title_);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -18);

  lv_label_set_text(detail, detail_);
  lv_obj_set_width(detail, 180);
  lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(detail, lv_color_hex(0x94A3B8), 0);
  lv_obj_align(detail, LV_ALIGN_CENTER, 0, 24);
  return root;
}

}  // namespace twsim::app
