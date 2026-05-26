#include "App/UI/Pages/SettingsPages.h"

#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr const char* kTextAppLayout = "\xE5\xBA\x94\xE7\x94\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextMultiColumnLayout =
    "\xE5\xA4\x9A\xE5\x88\x97\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextListLayout =
    "\xE5\x88\x97\xE8\xA1\xA8\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextCategorizedLayout =
    "\xE5\x88\x86\xE7\xB1\xBB\xE5\xB8\x83\xE5\xB1\x80";
constexpr const char* kTextAppLayoutNoteTitle =
    "\xE5\xBA\x94\xE7\x94\xA8\xE5\x85\xA5\xE5\x8F\xA3";
constexpr const char* kTextAppLayoutNoteBody =
    "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE6\x94\xB6\xE5\x8F\xA3\xE5\xBA\x94\xE7\x94\xA8"
    "\xE5\xB8\x83\xE5\xB1\x80\xE8\xAE\xBE\xE7\xBD\xAE\xE4\xB8\x8E\xE4\xBF\x9D\xE5\xAD\x98"
    "\xE5\x9B\x9E\xE6\x98\xBE\xEF\xBC\x8CLauncher \xE5\xAE\x9E\xE9\x99\x85\xE6\xB8\xB2\xE6\x9F\x93"
    "\xE5\x88\x87\xE6\x8D\xA2\xE7\x95\x99\xE5\x88\xB0\xE5\x90\x8E\xE7\xBB\xAD\xE5\xB0\x8F\xE8\xBD\xAE\xE3\x80\x82";

constexpr lv_coord_t kListTop = 42;
constexpr lv_coord_t kTileHorizontalPadding = 16;

DisplayPolicyModel current_display_policy(DataCenter& data_center) {
  return data_center.display_policy().value_or(DisplayPolicyModel {});
}

}  // namespace

AppLayoutSettingsPage::AppLayoutSettingsPage(DataCenter& data_center)
    : SettingsPageBase(data_center, PageId::SettingAppLayout, kTextAppLayout, true),
      options_ {{
          {LauncherLayoutMode::MultiColumn, kTextMultiColumnLayout, nullptr, nullptr},
          {LauncherLayoutMode::List, kTextListLayout, nullptr, nullptr},
          {LauncherLayoutMode::Categorized, kTextCategorizedLayout, nullptr, nullptr},
      }} {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event&) { refresh_selection(); }));
}

void AppLayoutSettingsPage::on_will_appear() {
  refresh_selection();
}

lv_obj_t* AppLayoutSettingsPage::build() {
  lv_obj_t* root = create_page_root();
  if (root == nullptr) {
    return nullptr;
  }
  build_header(root);

  lv_obj_t* scroll = create_scroll_column(root, kListTop, list_height());
  if (scroll == nullptr) {
    return nullptr;
  }

  for (std::size_t index = 0; index < options_.size(); ++index) {
    auto& option = options_[index];
    lv_obj_t* button = create_card_shell(scroll, true);
    if (button == nullptr) {
      return nullptr;
    }
    option.button = button;
    lv_obj_set_user_data(button, reinterpret_cast<void*>(index));
    lv_obj_add_event_cb(button, &AppLayoutSettingsPage::option_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label);
    lv_obj_set_style_text_font(label, cjk_font_20(), 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF6FAFF), 0);
    lv_label_set_text(label, option.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, kTileHorizontalPadding, 0);

    option.check_label = create_selection_dot(button);
    if (option.check_label == nullptr) {
      return nullptr;
    }
  }

  if (create_note_card(scroll, kTextAppLayoutNoteTitle, kTextAppLayoutNoteBody) == nullptr) {
    return nullptr;
  }

  refresh_selection();
  return root;
}

void AppLayoutSettingsPage::option_event_cb(lv_event_t* event) {
  auto* self = static_cast<AppLayoutSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->options_.size()) {
    return;
  }

  self->data_center_.set_launcher_layout_mode(self->options_[index].mode);
}

void AppLayoutSettingsPage::refresh_selection() {
  const auto policy = current_display_policy(data_center_);
  for (auto& option : options_) {
    apply_selection_dot(option.check_label, policy.launcher_layout_mode == option.mode);
  }
}

}  // namespace twsim::app
