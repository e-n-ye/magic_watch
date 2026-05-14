#include "App/UI/Pages/WatchfacePage.h"

#include <cstdio>

namespace twsim::app {

namespace {

void style_screen(lv_obj_t* root) {
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x080B12), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(root, lv_color_hex(0xF1F5F9), 0);
}

}  // namespace

WatchfacePage::WatchfacePage(DataCenter& data_center) : PageBase(data_center) {}

PageId WatchfacePage::id() const {
  return PageId::Watchface;
}

const char* WatchfacePage::name() const {
  return "Watchface";
}

void WatchfacePage::on_will_appear() {
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
}

lv_obj_t* WatchfacePage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }

  style_screen(root);

  lv_obj_t* card = lv_obj_create(root);
  if (card == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(card, 236, 206);
  lv_obj_center(card);
  lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x101826), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(card, lv_color_hex(0x243247), 0);
  lv_obj_set_style_radius(card, 26, 0);
  lv_obj_set_style_pad_all(card, 18, 0);

  lv_obj_t* title = lv_label_create(card);
  time_label_ = lv_label_create(card);
  date_label_ = lv_label_create(card);
  battery_label_ = lv_label_create(card);
  lv_obj_t* open_button = lv_button_create(card);
  if (title == nullptr || time_label_ == nullptr || date_label_ == nullptr ||
      battery_label_ == nullptr || open_button == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "T-Watch S3 Plus");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0x8FB8FF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_label_set_text(time_label_, "--:--:--");
  lv_obj_set_style_text_font(time_label_, &lv_font_montserrat_36, 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align_to(time_label_, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 18);

  lv_label_set_text(date_label_, "Waiting for RTC stream");
  lv_obj_set_style_text_font(date_label_, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(date_label_, lv_color_hex(0x94A3B8), 0);
  lv_obj_align_to(date_label_, time_label_, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

  lv_label_set_text(battery_label_, "Battery stream offline");
  lv_obj_set_style_text_font(battery_label_, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(battery_label_, lv_color_hex(0x67E8F9), 0);
  lv_obj_set_width(battery_label_, 190);
  lv_label_set_long_mode(battery_label_, LV_LABEL_LONG_WRAP);
  lv_obj_align_to(battery_label_, date_label_, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);

  lv_obj_set_size(open_button, 132, 38);
  lv_obj_set_style_bg_color(open_button, lv_color_hex(0x1D4ED8), 0);
  lv_obj_set_style_bg_opa(open_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(open_button, 0, 0);
  lv_obj_set_style_radius(open_button, 16, 0);
  lv_obj_add_event_cb(open_button, &WatchfacePage::open_quick_status_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_align_to(open_button, battery_label_, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 14);

  lv_obj_t* open_button_label = lv_label_create(open_button);
  if (open_button_label == nullptr) {
    return nullptr;
  }
  lv_label_set_text(open_button_label, "Quick Status");
  lv_obj_set_style_text_font(open_button_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(open_button_label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(open_button_label);

  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_time(*model);
                                 }
                               }));

  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_battery(*model);
                                 }
                               }));

  return root;
}

void WatchfacePage::open_quick_status_event_cb(lv_event_t* event) {
  auto* self = static_cast<WatchfacePage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->request_navigation({NavigationAction::Push, PageId::QuickStatus});
}

void WatchfacePage::apply_time(const TimeModel& model) {
  if (time_label_ == nullptr || date_label_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(time_label_, "--:--:--");
    lv_label_set_text(date_label_, "Waiting for RTC stream");
    return;
  }

  char time_buffer[16] = {};
  char date_buffer[24] = {};
  std::snprintf(time_buffer,
                sizeof(time_buffer),
                "%02u:%02u:%02u",
                static_cast<unsigned>(model.hour),
                static_cast<unsigned>(model.minute),
                static_cast<unsigned>(model.second));
  std::snprintf(date_buffer,
                sizeof(date_buffer),
                "%04u-%02u-%02u",
                static_cast<unsigned>(model.year),
                static_cast<unsigned>(model.month),
                static_cast<unsigned>(model.day));
  lv_label_set_text(time_label_, time_buffer);
  lv_label_set_text(date_label_, date_buffer);
}

void WatchfacePage::apply_battery(const BatteryModel& model) {
  if (battery_label_ == nullptr) {
    return;
  }

  char buffer[96] = {};
  const char* source = model.external_power ? "USB" : "Battery";
  const char* state = model.charging ? "charging" : "steady";
  std::snprintf(buffer,
                sizeof(buffer),
                "%s %d%% | %s | %umV",
                source,
                static_cast<int>(model.percent),
                state,
                static_cast<unsigned>(model.millivolts));
  lv_label_set_text(battery_label_, buffer);
}

}  // namespace twsim::app
