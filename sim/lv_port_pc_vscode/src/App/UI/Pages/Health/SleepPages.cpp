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

using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_asset::sleep_icon_asset_path;
using shell_font::cjk_font_16;
using shell_font::cjk_font_20;

}  // namespace

SleepAppPage::SleepAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId SleepAppPage::id() const {
  return PageId::AppSleep;
}

const char* SleepAppPage::name() const {
  return page_name(PageId::AppSleep);
}

void SleepAppPage::on_will_appear() {
  refresh_header_time();
}

void SleepAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = screen_w - 16;

  lv_obj_t* title_label = create_steps_label(root, "睡眠", cjk_font_20(), 0xF8FAFC, 80);
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

  lv_obj_t* last_night_card = create_steps_panel(scroll_root_, card_w, 168, 0x0A1626);
  if (last_night_card == nullptr) {
    return nullptr;
  }
  lv_obj_t* last_night_icon =
      create_sleep_round_icon(last_night_card, 52, 0x0A4F76, 0x7DD3FC, sleep_icon_asset_path(), "Z", cjk_font_20());
  lv_obj_t* last_night_label = create_steps_label(last_night_card,
                                                  "无睡眠记录，请佩戴入睡",
                                                  cjk_font_20(),
                                                  0xF8FAFC,
                                                  card_w - 40,
                                                  LV_LABEL_LONG_WRAP);
  if (last_night_icon == nullptr || last_night_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(last_night_icon, LV_ALIGN_TOP_MID, 0, 28);
  lv_obj_align(last_night_label, LV_ALIGN_TOP_MID, 0, 96);
  lv_obj_set_style_text_align(last_night_label, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t* week_card = create_steps_panel(scroll_root_, card_w, 206, 0x0A1626);
  if (week_card == nullptr) {
    return nullptr;
  }
  lv_obj_t* week_icon =
      create_sleep_round_icon(week_card, 42, 0x0A4F76, 0x7DD3FC, sleep_icon_asset_path(), "Z", cjk_font_20());
  lv_obj_t* week_title = create_steps_label(week_card, "近7天睡眠", cjk_font_20(), 0xF8FAFC, card_w - 90);
  lv_obj_t* divider = lv_obj_create(week_card);
  lv_obj_t* week_value =
      create_steps_label(week_card, "--小时--分钟", cjk_font_20(), 0xF8FAFC, card_w - 36);
  lv_obj_t* week_subtitle = create_steps_label(week_card, "平均时长", cjk_font_16(), 0xAFC4DA, card_w - 36);
  if (week_icon == nullptr || week_title == nullptr || divider == nullptr || week_value == nullptr ||
      week_subtitle == nullptr) {
    return nullptr;
  }
  lv_obj_align(week_icon, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(week_title, LV_ALIGN_TOP_LEFT, 68, 22);
  ui_prepare_box(divider);
  lv_obj_set_size(divider, card_w - 36, 1);
  lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
  lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
  lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, 72);
  lv_obj_align(week_value, LV_ALIGN_TOP_LEFT, 18, 94);
  lv_obj_align(week_subtitle, LV_ALIGN_TOP_LEFT, 18, 130);

  const lv_coord_t tick_y = 152;
  const lv_coord_t tick_label_y = 184;
  const lv_coord_t chart_left = 28;
  const lv_coord_t chart_right = card_w - 28;
  const lv_coord_t chart_width = chart_right - chart_left;
  for (int i = 0; i < 7; ++i) {
    lv_obj_t* tick = lv_obj_create(week_card);
    if (tick == nullptr) {
      return nullptr;
    }
    ui_prepare_box(tick);
    lv_obj_set_size(tick, 1, 34);
    lv_obj_set_style_bg_color(tick, lv_color_hex(0x486784), 0);
    lv_obj_set_style_bg_opa(tick, LV_OPA_50, 0);
    const lv_coord_t x = static_cast<lv_coord_t>(chart_left + (chart_width * i) / 6);
    lv_obj_align(tick, LV_ALIGN_TOP_LEFT, x, tick_y);
    if (i == 0 || i == 6) {
      lv_obj_t* date_label = create_steps_label(week_card, i == 0 ? "5/18" : "5/24", &lv_font_montserrat_14, 0x8DB9E3, 44);
      if (date_label == nullptr) {
        return nullptr;
      }
      lv_obj_align(date_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(x - 12), tick_label_y);
    }
  }

  for (const auto& entry :
       {std::pair<const char*, PageId> {"设置", PageId::AppSleepSettings},
        std::pair<const char*, PageId> {"说明", PageId::AppSleepInfo}}) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 86, 0x102033);
    if (card == nullptr) {
      return nullptr;
    }
    const char* fallback = entry.second == PageId::AppSleepSettings ? LV_SYMBOL_SETTINGS : "i";
    lv_obj_t* icon = create_sleep_round_icon(card,
                                             44,
                                             entry.second == PageId::AppSleepSettings ? 0x114C9D : 0x0A4F76,
                                             0x7DD3FC,
                                             nullptr,
                                             fallback,
                                             entry.second == PageId::AppSleepSettings ? &lv_font_montserrat_18
                                                                                      : cjk_font_20());
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

void SleepAppPage::entry_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepAppPage*>(lv_event_get_user_data(event));
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

void SleepAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_ = nullptr;
  release_stream_crown_drag(self->scroll_root_);
}

void SleepAppPage::bind_input() {
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

void SleepAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepAppPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_ = lv_timer_create(&SleepAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this);
  if (crown_release_timer_ != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_, 1);
  }
}

void SleepAppPage::stop_crown_release_timer() {
  if (crown_release_timer_ == nullptr) {
    return;
  }
  lv_timer_delete(crown_release_timer_);
  crown_release_timer_ = nullptr;
}

SleepInfoPage::SleepInfoPage(DataCenter& data_center) : PageBase(data_center) {}

PageId SleepInfoPage::id() const {
  return PageId::AppSleepInfo;
}

const char* SleepInfoPage::name() const {
  return page_name(PageId::AppSleepInfo);
}

void SleepInfoPage::on_will_appear() {
  refresh_header_time();
}

void SleepInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* SleepInfoPage::build() {
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
  lv_obj_t* title_label = create_steps_label(root, "睡眠说明", cjk_font_20(), 0xF8FAFC, 116);
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

  const char* sleep_info_text =
      "佩戴手表入睡，手表将自动记录睡眠信息。手表端只展示最近3条睡眠片段，更多数据请至App端查看。\n\n"
      "推荐在设置中开启“睡眠高精度监测”的设置项，可以帮助睡眠算法更准确地评估你的睡眠状态，识别REM快速眼动期睡眠。\n\n"
      "大于等于3小时的长睡眠，设备会根据可获取的数据展示该片段内的平均心率，小于3小时的零星小睡，只展示入睡醒来时间。小于20分钟的睡眠不会被记录。\n\n"
      "如果睡眠质量很差，睡眠中手腕动作过多，可能会导致睡眠心率测量和血氧测量失败。";

  lv_obj_t* note_body =
      create_steps_label(scroll_root_, sleep_info_text, cjk_font_16(), 0xEAF2FF, card_w - 20, LV_LABEL_LONG_WRAP);
  if (note_body == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_text_line_space(note_body, 8, 0);
  lv_obj_align(note_body, LV_ALIGN_TOP_LEFT, 10, 8);

  bind_input();
  on_will_appear();
  return root;
}

void SleepInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<SleepInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void SleepInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<SleepInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void SleepInfoPage::bind_input() {
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

void SleepInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void SleepInfoPage::refresh_header_time() {
  apply_compact_time_label(time_label_, data_center_.time());
}

void SleepInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&SleepInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void SleepInfoPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

}  // namespace twsim::app
