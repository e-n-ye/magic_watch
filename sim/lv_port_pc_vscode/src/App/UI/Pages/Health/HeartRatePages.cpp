#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

namespace twsim::app {

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace {

using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

}  // namespace

HeartRateInfoPage::HeartRateInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HeartRateInfoPage::id() const {
  return PageId::AppHeartRateInfo;
}

const char* HeartRateInfoPage::name() const {
  return page_name(PageId::AppHeartRateInfo);
}

void HeartRateInfoPage::on_will_appear() {
  refresh_header_time();
}

void HeartRateInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* HeartRateInfoPage::build() {
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
  lv_obj_t* title_label = create_steps_label(root, "心率说明", cjk_font_20(), 0xF8FAFC, 116);
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
      "心率\n\n"
      "心率是心脏每分钟跳动的次数（bpm），它是心血管健康的重要指标。"
      "成年人安静状态下的心率（静息心率）通常为55-80次/分，可能因年龄、性别、健身水平或其他生理因素产生个体差异。\n\n"
      "注意：本设备测量范围为30-240次/分。\n\n"
      "测量说明\n\n"
      "1. 将设备佩戴至腕骨一指距离，约1.5cm-2cm。\n\n"
      "2. 偏紧佩戴，测量期间保持静止。\n\n"
      "3. 受佩戴状态、皮肤特征、环境等因素影响，可能影响测量的准确性，甚至无法输出结果。\n\n"
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

void HeartRateInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void HeartRateInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<HeartRateInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void HeartRateInfoPage::bind_input() {
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

void HeartRateInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void HeartRateInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void HeartRateInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&HeartRateInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void HeartRateInfoPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

}  // namespace twsim::app
