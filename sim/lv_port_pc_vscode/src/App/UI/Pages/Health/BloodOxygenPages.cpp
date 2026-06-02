#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Health/HealthIconPrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace {

using shell_asset::sleep_icon_asset_path;
using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

}  // namespace

BloodOxygenAppPage::BloodOxygenAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenAppPage::id() const {
  return PageId::AppBloodOxygen;
}

const char* BloodOxygenAppPage::name() const {
  return page_name(PageId::AppBloodOxygen);
}

void BloodOxygenAppPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "血氧饱和度", cjk_font_20(), 0xF8FAFC, 120);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 16, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* chart_card = create_steps_panel(scroll_root_, card_w, 124, 0x0A1626);
  if (chart_card == nullptr) {
    return nullptr;
  }

  const lv_coord_t chart_left = 22;
  const lv_coord_t chart_top = 16;
  const lv_coord_t chart_w = card_w - 44;
  const lv_coord_t chart_h = 82;
  for (int i = 0; i < 5; ++i) {
    lv_obj_t* tick = lv_obj_create(chart_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, chart_h);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x5A82B2), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(chart_left + (chart_w * i) / 4), chart_top);
  }

  lv_obj_t* top_value = create_steps_label(chart_card, "95", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* bottom_value = create_steps_label(chart_card, "85", &lv_font_montserrat_18, 0x8DB9E3, 28);
  lv_obj_t* left_label = create_steps_label(chart_card, "00:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* mid_label = create_steps_label(chart_card, "12:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  lv_obj_t* right_label = create_steps_label(chart_card, "24:00", &lv_font_montserrat_16, 0x8DB9E3, 48);
  if (top_value == nullptr || bottom_value == nullptr || left_label == nullptr || mid_label == nullptr ||
      right_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(top_value, LV_ALIGN_TOP_RIGHT, -10, 6);
  lv_obj_align(bottom_value, LV_ALIGN_BOTTOM_RIGHT, -10, -12);
  lv_obj_align(left_label, LV_ALIGN_BOTTOM_LEFT, 14, -12);
  lv_obj_align(mid_label, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_align(right_label, LV_ALIGN_BOTTOM_RIGHT, -24, -12);

  lv_obj_t* dot = lv_obj_create(chart_card);
  if (dot == nullptr) {
    return nullptr;
  }
  ui_prepare_box(dot);
  lv_obj_set_size(dot, 8, 8);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, lv_color_hex(0xFF4F72), 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_align(dot, LV_ALIGN_TOP_RIGHT, -34, 18);

  lv_obj_t* value_label =
      create_steps_label(scroll_root_, "95%", &lv_font_montserrat_48, 0xF8FAFC, card_w, LV_LABEL_LONG_CLIP);
  lv_obj_t* update_label = create_steps_label(scroll_root_, "20:37更新", cjk_font_20(), 0xD8E9FF, card_w, LV_LABEL_LONG_CLIP);
  if (value_label == nullptr || update_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_align(update_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* measure_button = create_steps_panel(scroll_root_, card_w, 74, 0x111D2E);
  lv_obj_t* measure_label = measure_button == nullptr
                                ? nullptr
                                : create_steps_label(measure_button, "开始测量", cjk_font_20(), 0xFF4F72, card_w - 24);
  if (measure_button == nullptr || measure_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(measure_button, 26, 0);
  lv_obj_set_style_bg_color(measure_button, lv_color_hex(0x0D1624), 0);
  lv_obj_set_style_text_align(measure_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(measure_label);

  for (const auto& entry :
       {std::pair<const char*, PageId> {"设置", PageId::AppBloodOxygenSettings},
        std::pair<const char*, PageId> {"说明", PageId::AppBloodOxygenInfo}}) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 86, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    const bool is_settings = entry.second == PageId::AppBloodOxygenSettings;
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             0x4C1732,
                                             0xFF8FB3,
                                             nullptr,
                                             is_settings ? LV_SYMBOL_SETTINGS : "i",
                                             is_settings ? &lv_font_montserrat_18 : cjk_font_20());
    lv_obj_t* label = create_steps_label(card, entry.first, cjk_font_20(), 0xF8FAFC, card_w - 100);
    if (icon == nullptr || label == nullptr) {
      return nullptr;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 18, 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 74, 0);
    attach_click_guard(card);
    lv_obj_add_event_cb(card, entry_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(entry.second)));
  }

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
}

void BloodOxygenAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&BloodOxygenAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

BloodOxygenInfoPage::BloodOxygenInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId BloodOxygenInfoPage::id() const {
  return PageId::AppBloodOxygenInfo;
}

const char* BloodOxygenInfoPage::name() const {
  return page_name(PageId::AppBloodOxygenInfo);
}

void BloodOxygenInfoPage::on_will_appear() {
  refresh_header_time();
}

void BloodOxygenInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* BloodOxygenInfoPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* back_button = lv_button_create(root);
  lv_obj_t* back_label = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xD8E9FF, 18);
  lv_obj_t* title_label = create_steps_label(root, "血氧说明", cjk_font_20(), 0xF8FAFC, 116);
  time_label_ = create_steps_label(root, "--:--", &lv_font_montserrat_20, 0xE2F0FF, 64);
  if (back_button == nullptr || back_label == nullptr || title_label == nullptr || time_label_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 38, 38);
  lv_obj_align(back_button, LV_ALIGN_TOP_LEFT, 10, 8);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back_label, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back_label, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back_label, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 42, 12);
  lv_obj_align(time_label_, LV_ALIGN_TOP_RIGHT, -16, 12);

  scroll_root_ = create_sleep_scroll_root(root, screen_w, screen_h, 48, 0, 10);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  const char* info_text =
      "说明\n\n"
      "1. 血氧饱和度是血液中氧合血红蛋白（HbO2）容量占全部可结合血红蛋白（Hb）的百分比，"
      "它是人体呼吸循环的重要生理参数。\n\n"
      "2. 设备血氧饱和度参考值如下：\n"
      "日常活动：一般 ≥95%\n"
      "夜间睡眠：一般 ≥90%\n"
      "需要关注：<90%\n"
      "注意：本设备测量范围为80%-100%。\n\n"
      "3. 佩戴状态、皮肤特征、环境等因素都可能影响测试准确性，甚至导致测量失败。"
      "睡眠过程中，设备可能因睡眠姿势不同而倾斜，导致伪低血氧值出现。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "本品非医疗器械，结果仅供参考，不作为诊断治疗依据。";
  lv_obj_t* note_body =
      create_steps_label(scroll_root_, info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void BloodOxygenInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void BloodOxygenInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<BloodOxygenInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void BloodOxygenInfoPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     apply_crown_drag(true, command->value);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     apply_crown_drag(false, command->value);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void BloodOxygenInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void BloodOxygenInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void BloodOxygenInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ =
      lv_timer_create(&BloodOxygenInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void BloodOxygenInfoPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

}  // namespace twsim::app
