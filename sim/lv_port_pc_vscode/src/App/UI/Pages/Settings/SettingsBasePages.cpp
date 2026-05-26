#include "App/UI/Pages/SettingsPages.h"

#include <variant>

#include "App/UI/Pages/Settings/SettingsPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

namespace {

constexpr lv_coord_t kHeaderTop = 13;

}  // namespace

SettingsPageBase::SettingsPageBase(DataCenter& data_center, PageId page_id, const char* title, bool show_back_button)
    : PageBase(data_center), page_id_(page_id), title_(title), show_back_button_(show_back_button) {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 const auto* model = std::get_if<TimeModel>(&event.payload);
                                 if (model != nullptr) {
                                   apply_time(*model);
                                 }
                               }));
}

PageId SettingsPageBase::id() const {
  return page_id_;
}

const char* SettingsPageBase::name() const {
  return page_name(page_id_);
}

lv_obj_t* SettingsPageBase::create_page_root() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root != nullptr) {
    style_page_root(root);
    lv_obj_set_size(root, display_width(), display_height());
  }
  return root;
}

void SettingsPageBase::build_header(lv_obj_t* root) {
  if (root == nullptr) {
    return;
  }

  if (show_back_button_ && create_back_button(root, &SettingsPageBase::back_event_cb, this) == nullptr) {
    return;
  }

  lv_obj_t* title = lv_label_create(root);
  time_label_ = lv_label_create(root);
  if (title == nullptr || time_label_ == nullptr) {
    return;
  }

  ui_prepare_label(title);
  lv_obj_set_style_text_font(title, cjk_font_20(), 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF7FBFF), 0);
  lv_label_set_text(title, title_);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, show_back_button_ ? 40 : 18, kHeaderTop);

  ui_prepare_label(time_label_);
  lv_obj_set_style_text_font(time_label_, cjk_font_20(), 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xF7FBFF), 0);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, kHeaderTop);

  if (const auto& time = data_center_.time(); time) {
    apply_time(*time);
  } else {
    lv_label_set_text(time_label_, "--:--");
  }
}

void SettingsPageBase::apply_time(const TimeModel& model) {
  set_time_label_text(time_label_, model);
}

const char* SettingsPageBase::title_text() const {
  return title_;
}

void SettingsPageBase::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SettingsPageBase*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

}  // namespace twsim::app
