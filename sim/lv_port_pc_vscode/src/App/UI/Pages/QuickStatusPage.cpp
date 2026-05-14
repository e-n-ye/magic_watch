#include "App/UI/Pages/QuickStatusPage.h"

#include <cstdio>

namespace twsim::app {

QuickStatusPage::QuickStatusPage(DataCenter& data_center) : PageBase(data_center) {}

PageId QuickStatusPage::id() const {
  return PageId::QuickStatus;
}

const char* QuickStatusPage::name() const {
  return "QuickStatus";
}

void QuickStatusPage::on_will_appear() {
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
}

lv_obj_t* QuickStatusPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }

  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x05080F), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* back_button = lv_button_create(root);
  detail_label_ = lv_label_create(root);
  if (title == nullptr || back_button == nullptr || detail_label_ == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Quick Status / HSM-controlled");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xE2E8F0), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 18, 18);

  lv_obj_set_size(back_button, 88, 34);
  lv_obj_set_style_bg_color(back_button, lv_color_hex(0x334155), 0);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(back_button, 0, 0);
  lv_obj_set_style_radius(back_button, 14, 0);
  lv_obj_add_event_cb(back_button, &QuickStatusPage::back_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_align(back_button, LV_ALIGN_TOP_RIGHT, -18, 16);

  lv_obj_t* back_label = lv_label_create(back_button);
  if (back_label == nullptr) {
    return nullptr;
  }
  lv_label_set_text(back_label, "Back");
  lv_obj_set_style_text_font(back_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(back_label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(back_label);

  lv_label_set_text(detail_label_,
                    "Event bus: waiting...\n"
                    "Page stack: push/pop\n"
                    "HAL: simulator backend\n"
                    "IMU: placeholder contract");
  lv_obj_set_style_text_font(detail_label_, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(detail_label_, lv_color_hex(0x94A3B8), 0);
  lv_obj_set_width(detail_label_, 250);
  lv_label_set_long_mode(detail_label_, LV_LABEL_LONG_WRAP);
  lv_obj_align(detail_label_, LV_ALIGN_TOP_LEFT, 18, 74);

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

void QuickStatusPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickStatusPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void QuickStatusPage::apply_time(const TimeModel& model) {
  if (detail_label_ == nullptr) {
    return;
  }

  char buffer[256] = {};
  std::snprintf(buffer,
                sizeof(buffer),
                "Time stream:\n"
                "  %04u-%02u-%02u\n"
                "  %02u:%02u:%02u\n"
                "\n"
                "Battery stream:\n"
                "  waiting for next update\n"
                "\n"
                "Architecture:\n"
                "  MDA + EventBus + PageStack + HSM stub",
                static_cast<unsigned>(model.year),
                static_cast<unsigned>(model.month),
                static_cast<unsigned>(model.day),
                static_cast<unsigned>(model.hour),
                static_cast<unsigned>(model.minute),
                static_cast<unsigned>(model.second));
  lv_label_set_text(detail_label_, buffer);
}

void QuickStatusPage::apply_battery(const BatteryModel& model) {
  if (detail_label_ == nullptr) {
    return;
  }

  char buffer[256] = {};
  const auto time = data_center_.time();
  const unsigned year = time ? static_cast<unsigned>(time->year) : 0U;
  const unsigned month = time ? static_cast<unsigned>(time->month) : 0U;
  const unsigned day = time ? static_cast<unsigned>(time->day) : 0U;
  const unsigned hour = time ? static_cast<unsigned>(time->hour) : 0U;
  const unsigned minute = time ? static_cast<unsigned>(time->minute) : 0U;
  const unsigned second = time ? static_cast<unsigned>(time->second) : 0U;

  std::snprintf(buffer,
                sizeof(buffer),
                "Time stream:\n"
                "  %04u-%02u-%02u\n"
                "  %02u:%02u:%02u\n"
                "\n"
                "Battery stream:\n"
                "  percent: %d%%\n"
                "  charging: %s\n"
                "  external: %s\n"
                "  mv: %u\n"
                "\n"
                "Architecture:\n"
                "  HSM owns navigation state",
                year,
                month,
                day,
                hour,
                minute,
                second,
                static_cast<int>(model.percent),
                model.charging ? "yes" : "no",
                model.external_power ? "yes" : "no",
                static_cast<unsigned>(model.millivolts));
  lv_label_set_text(detail_label_, buffer);
}

}  // namespace twsim::app
