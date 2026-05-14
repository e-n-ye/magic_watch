#include "App/UI/Pages/CommonPages.h"

#include <algorithm>
#include <cstdio>
#include <cstdint>

namespace twsim::app {

namespace {

void style_base_screen(lv_obj_t* root) {
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x071018), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root, 0, 0);
  lv_obj_set_style_pad_all(root, 0, 0);
}

lv_obj_t* create_back_button(lv_obj_t* root, lv_event_cb_t callback, void* user_data) {
  lv_obj_t* back = lv_button_create(root);
  if (back == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(back, 68, 30);
  lv_obj_align(back, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_obj_set_style_bg_color(back, lv_color_hex(0x243447), 0);
  lv_obj_set_style_bg_opa(back, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(back, 14, 0);
  lv_obj_set_style_border_width(back, 0, 0);
  lv_obj_add_event_cb(back, callback, LV_EVENT_CLICKED, user_data);

  lv_obj_t* label = lv_label_create(back);
  if (label == nullptr) {
    return nullptr;
  }
  lv_label_set_text(label, "Back");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(label);
  return back;
}

}  // namespace

MenuPage::MenuPage(DataCenter& data_center,
                   PageId page_id,
                   const char* title,
                   const char* subtitle,
                   std::vector<Item> items,
                   bool show_back_button,
                   bool enable_input_prototype)
    : PageBase(data_center),
      page_id_(page_id),
      title_(title),
      subtitle_(subtitle),
      items_(std::move(items)),
      show_back_button_(show_back_button),
      enable_input_prototype_(enable_input_prototype) {}

PageId MenuPage::id() const {
  return page_id_;
}

const char* MenuPage::name() const {
  return page_name(page_id_);
}

lv_obj_t* MenuPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_base_screen(root);

  if (show_back_button_ && create_back_button(root, &MenuPage::back_event_cb, this) == nullptr) {
    return nullptr;
  }

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* subtitle = lv_label_create(root);
  list_ = lv_obj_create(root);
  if (title == nullptr || subtitle == nullptr || list_ == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, title_);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 14, show_back_button_ ? 50 : 16);

  lv_label_set_text(subtitle, subtitle_);
  lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

  const lv_coord_t list_height = enable_input_prototype_ ? 126 : 154;
  const lv_coord_t list_bottom_offset = enable_input_prototype_ ? -28 : -8;
  lv_obj_set_size(list_, 220, list_height);
  lv_obj_align(list_, LV_ALIGN_BOTTOM_MID, 0, list_bottom_offset);
  lv_obj_set_flex_flow(list_, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(list_, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(list_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(list_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(list_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_pad_all(list_, 6, 0);
  lv_obj_set_style_pad_gap(list_, 6, 0);
  lv_obj_set_style_bg_color(list_, lv_color_hex(0x0F172A), 0);
  lv_obj_set_style_bg_opa(list_, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(list_, 1, 0);
  lv_obj_set_style_border_color(list_, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_radius(list_, 20, 0);

  if (enable_input_prototype_) {
    interaction_hint_label_ = lv_label_create(root);
    if (interaction_hint_label_ == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(interaction_hint_label_, 220);
    lv_label_set_long_mode(interaction_hint_label_, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(interaction_hint_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(interaction_hint_label_, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(interaction_hint_label_, lv_color_hex(0x7DD3FC), 0);
    lv_obj_align(interaction_hint_label_, LV_ALIGN_BOTTOM_MID, 0, -6);
    apply_interaction_hint("Drag / flick / edge back / Enter-Q-E");
  }

  for (std::size_t index = 0; index < items_.size(); ++index) {
    const auto& item = items_[index];
    lv_obj_t* button = lv_button_create(list_);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_set_height(button, 34);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x132133), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 14, 0);
    lv_obj_add_event_cb(button, &MenuPage::item_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_t* detail = lv_label_create(button);
    if (label == nullptr || detail == nullptr) {
      return nullptr;
    }
    lv_label_set_text(label, item.label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 12, -6);

    lv_label_set_text(detail, item.detail != nullptr ? item.detail : "");
    lv_obj_set_style_text_font(detail, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(detail, lv_color_hex(0x93C5FD), 0);
    lv_obj_align(detail, LV_ALIGN_LEFT_MID, 12, 8);

    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));
  }

  if (enable_input_prototype_) {
    bind_input_prototype();
  }

  return root;
}

void MenuPage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<MenuPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->items_.size()) {
    return;
  }

  self->request_navigation(self->items_[index].command);
}

void MenuPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<MenuPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void MenuPage::bind_input_prototype() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     if (list_ != nullptr) {
                                       lv_obj_scroll_by(list_, 0, 44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     apply_interaction_hint("Crown rotate CW");
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (list_ != nullptr) {
                                       lv_obj_scroll_by(list_, 0, -44 * std::max<std::int16_t>(1, command->value), LV_ANIM_ON);
                                     }
                                     apply_interaction_hint("Crown rotate CCW");
                                     break;
                                   case InputAction::ScrollDrag:
                                     apply_interaction_hint(command->value >= 0 ? "Touch drag down" : "Touch drag up");
                                     break;
                                   case InputAction::ScrollFlick:
                                     apply_interaction_hint(command->value >= 0 ? "Touch flick down" : "Touch flick up");
                                     break;
                                   case InputAction::NavigateBack:
                                     apply_interaction_hint("Edge back");
                                     break;
                                   case InputAction::CrownPress:
                                     apply_interaction_hint("Crown press");
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void MenuPage::apply_interaction_hint(const char* text) {
  if (interaction_hint_label_ == nullptr) {
    return;
  }

  char buffer[96] = {};
  std::snprintf(buffer, sizeof(buffer), "Prototype: %s", text);
  lv_label_set_text(interaction_hint_label_, buffer);
}

PlaceholderPage::PlaceholderPage(DataCenter& data_center,
                                 PageId page_id,
                                 const char* title,
                                 const char* detail,
                                 bool show_back_button)
    : PageBase(data_center),
      page_id_(page_id),
      title_(title),
      detail_(detail),
      show_back_button_(show_back_button) {}

PageId PlaceholderPage::id() const {
  return page_id_;
}

const char* PlaceholderPage::name() const {
  return page_name(page_id_);
}

lv_obj_t* PlaceholderPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_base_screen(root);

  if (show_back_button_ && create_back_button(root, &PlaceholderPage::back_event_cb, this) == nullptr) {
    return nullptr;
  }

  lv_obj_t* card = lv_obj_create(root);
  if (card == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(card, 210, 160);
  lv_obj_center(card);
  lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x101826), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, lv_color_hex(0x203040), 0);
  lv_obj_set_style_radius(card, 22, 0);
  lv_obj_set_style_pad_all(card, 16, 0);

  lv_obj_t* title = lv_label_create(card);
  lv_obj_t* detail = lv_label_create(card);
  if (title == nullptr || detail == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, title_);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_label_set_text(detail, detail_);
  lv_obj_set_width(detail, 178);
  lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(detail, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(detail, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  return root;
}

void PlaceholderPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<PlaceholderPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

}  // namespace twsim::app
