#include "App/UI/Pages/Daily/StepsPages.h"

#include "App/UI/Pages/Daily/StepsMainPrimitives.h"
#include "App/UI/Pages/Daily/StepsPagePrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <array>
#include <algorithm>
#include <cstdio>

namespace twsim::app {

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace {

using shell_crown::apply_stream_crown_drag;
using shell_crown::kLauncherCrownReleaseDelayMs;
using shell_crown::release_stream_crown_drag;
using shell_font::cjk_font_16;

}  // namespace

StepsDataInfoPage::StepsDataInfoPage(DataCenter& data_center) : PageBase(data_center) {}

StepsAppPage::StepsAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId StepsAppPage::id() const {
  return PageId::Pedometer;
}

const char* StepsAppPage::name() const {
  return page_name(PageId::Pedometer);
}

void StepsAppPage::on_will_appear() {
  if (const auto steps = data_center_.steps()) {
    steps_model_ = *steps;
  }
  refresh_steps_view();
}

void StepsAppPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* StepsAppPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t viewport_h = screen_h - kStepsScrollTop - kStepsScrollBottom;
  const lv_coord_t card_w = screen_w - kStepsScrollInset * 2;
  const lv_coord_t overview_h = viewport_h;
  const lv_coord_t metric_card_h = viewport_h;

  scroll_root_ = create_steps_scroll_root(root, screen_w, screen_h);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* overview = create_steps_panel(scroll_root_, card_w, overview_h, 0x050A12);
  if (overview == nullptr) {
    return nullptr;
  }

  lv_obj_t* arc_outer = create_steps_arc(overview, 174, 16, 0xF97316, 1, 400);
  steps_arc_ = create_steps_arc(overview, 138, 15, 0xF6D34D, static_cast<std::int32_t>(steps_model_.daily_steps), 6000);
  lv_obj_t* arc_inner = create_steps_arc(overview, 102, 14, 0x14B8FF, 0, 30);
  if (arc_outer == nullptr || steps_arc_ == nullptr || arc_inner == nullptr) {
    return nullptr;
  }
  lv_obj_align(arc_outer, LV_ALIGN_TOP_MID, 0, 14);
  lv_obj_align(steps_arc_, LV_ALIGN_TOP_MID, 0, 32);
  lv_obj_align(arc_inner, LV_ALIGN_TOP_MID, 0, 50);

  if (!create_steps_metric_line(overview, static_cast<lv_coord_t>(overview_h - 122), 0xF97316, "1", "/400") ||
      !create_steps_metric_line(overview, static_cast<lv_coord_t>(overview_h - 38), 0x14B8FF, "0", "/30")) {
    return nullptr;
  }

  {
    const lv_coord_t y = static_cast<lv_coord_t>(overview_h - 76);
    lv_obj_t* icon = create_steps_icon(overview, 34, 0x1A2430, 0xF6D34D, true);
    steps_metric_value_label_ = create_steps_label(overview, "0", &lv_font_montserrat_28, 0xF8FAFC, LV_SIZE_CONTENT);
    steps_metric_target_label_ = create_steps_label(overview, "/6000", &lv_font_montserrat_16, 0xB9C7D9, LV_SIZE_CONTENT);
    if (icon == nullptr || steps_metric_value_label_ == nullptr || steps_metric_target_label_ == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_bg_color(icon, lv_color_mix(lv_color_hex(0xF6D34D), lv_color_hex(0x0A1626), LV_OPA_70), 0);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 18, y);
    lv_obj_align(steps_metric_value_label_, LV_ALIGN_TOP_LEFT, 64, y - 1);
    lv_obj_align_to(steps_metric_target_label_, steps_metric_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  }

  lv_obj_t* kcal = create_steps_distribution_card(scroll_root_,
                                                  card_w,
                                                  metric_card_h,
                                                  "卡路里",
                                                  "1",
                                                  "千卡",
                                                  "目标 400千卡",
                                                  0xF97316,
                                                  false);
  lv_obj_t* steps = create_steps_panel(scroll_root_, card_w, metric_card_h);
  lv_obj_t* active = create_steps_distribution_card(scroll_root_,
                                                    card_w,
                                                    metric_card_h,
                                                    "中高强度活动",
                                                    "0",
                                                    "分钟",
                                                    "目标 30分钟",
                                                    0x14B8FF,
                                                    false);
  if (kcal == nullptr || steps == nullptr || active == nullptr) {
    return nullptr;
  }

  {
    lv_obj_t* icon = create_steps_icon(steps, 36, 0x1A2430, 0xF6D34D, true);
    lv_obj_t* title_label = create_steps_label(steps, "步数", cjk_font_16(), 0xF8FAFC, card_w - 76);
    lv_obj_t* divider = lv_obj_create(steps);
    steps_card_value_label_ = create_steps_label(steps, "0", &lv_font_montserrat_42, 0xF8FAFC, LV_SIZE_CONTENT);
    steps_card_unit_label_ = create_steps_label(steps, "步", cjk_font_16(), 0xF8FAFC, LV_SIZE_CONTENT);
    lv_obj_t* target_label = create_steps_label(steps, "目标 6000步", cjk_font_16(), 0xAFC4DA, card_w - 36);
    if (icon == nullptr || title_label == nullptr || divider == nullptr || steps_card_value_label_ == nullptr ||
        steps_card_unit_label_ == nullptr || target_label == nullptr) {
      return nullptr;
    }

    const lv_coord_t divider_y = clamp_coord(scale_by_ratio(metric_card_h, 28, 100), 58, 66);
    const lv_coord_t value_y = clamp_coord(scale_by_ratio(metric_card_h, 35, 100), 76, 86);
    const lv_coord_t target_y = clamp_coord(scale_by_ratio(metric_card_h, 57, 100), 118, 132);
    const lv_coord_t tick_y = clamp_coord(scale_by_ratio(metric_card_h, 68, 100), 140, 154);
    const lv_coord_t tick_label_y = clamp_coord(scale_by_ratio(metric_card_h, 88, 100), 184, 198);

    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 20, 16);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 66, 22);

    ui_prepare_box(divider);
    lv_obj_set_size(divider, card_w - 36, 1);
    lv_obj_set_style_bg_color(divider, lv_color_hex(0x23354A), 0);
    lv_obj_set_style_bg_opa(divider, LV_OPA_70, 0);
    lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, divider_y);

    lv_obj_align(steps_card_value_label_, LV_ALIGN_TOP_LEFT, 18, value_y);
    lv_obj_align_to(steps_card_unit_label_, steps_card_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
    lv_obj_align(target_label, LV_ALIGN_TOP_LEFT, 22, target_y);

    lv_coord_t xs[] = {24, static_cast<lv_coord_t>(card_w / 2), static_cast<lv_coord_t>(card_w - 24)};
    const char* labels[] = {"0", "12", "24"};
    for (std::size_t i = 0; i < 3; ++i) {
      lv_obj_t* tick = lv_obj_create(steps);
      lv_obj_t* tick_label = create_steps_label(steps, labels[i], &lv_font_montserrat_16, 0x8DB9E3, 32);
      if (tick == nullptr || tick_label == nullptr) {
        return nullptr;
      }
      ui_prepare_box(tick);
      lv_obj_set_size(tick, 1, 46);
      lv_obj_set_style_bg_color(tick, lv_color_hex(0x5AA4DD), 0);
      lv_obj_set_style_bg_opa(tick, LV_OPA_70, 0);
      lv_obj_align(tick, LV_ALIGN_TOP_LEFT, xs[i], tick_y);
      lv_obj_align(tick_label, LV_ALIGN_TOP_LEFT, static_cast<lv_coord_t>(xs[i] - 10), tick_label_y);
    }

    lv_obj_t* dot = lv_obj_create(steps);
    if (dot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(dot);
    lv_obj_set_size(dot, 7, 7);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0xF6D34D), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_align(dot, LV_ALIGN_TOP_LEFT, card_w - 30, clamp_coord(scale_by_ratio(metric_card_h, 67, 100), 136, 174));
  }

  lv_obj_t* info = create_steps_panel(scroll_root_, card_w, 78, 0x111D2E);
  lv_obj_t* info_icon = info == nullptr ? nullptr : create_steps_icon(info, 38, 0x075985, 0x7DD3FC, false);
  lv_obj_t* info_label = info == nullptr ? nullptr : create_steps_label(info, "数据说明", cjk_font_16(), 0xF8FAFC, 132);
  if (info == nullptr || info_icon == nullptr || info_label == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(info, 18, 0);
  lv_obj_align(info_icon, LV_ALIGN_LEFT_MID, 20, 0);
  lv_obj_align(info_label, LV_ALIGN_LEFT_MID, 72, 0);
  attach_click_guard(info);
  lv_obj_add_event_cb(info, data_info_event_cb, LV_EVENT_CLICKED, this);

  track(data_center_.subscribe(EventId::StepsChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<StepsModel>(&event.payload)) {
                                   apply_steps(*model);
                                 }
                               }));

  bind_input();
  on_will_appear();
  return root;
}

void StepsAppPage::data_info_event_cb(lv_event_t* event) {
  auto* self = static_cast<StepsAppPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Push, PageId::PedometerDataInfo});
}

void StepsAppPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void StepsAppPage::apply_steps(const StepsModel& model) {
  steps_model_ = model;
  refresh_steps_view();
}

void StepsAppPage::refresh_steps_view() {
  char value_text[24] = {};
  std::snprintf(value_text, sizeof(value_text), "%lu", static_cast<unsigned long>(steps_model_.daily_steps));

  if (steps_metric_value_label_ != nullptr) {
    set_content_width_label(steps_metric_value_label_, value_text, steps_overview_value_font(steps_model_.daily_steps), 0xF8FAFC);
  }
  if (steps_metric_target_label_ != nullptr && steps_metric_value_label_ != nullptr) {
    lv_obj_align_to(steps_metric_target_label_, steps_metric_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 1);
  }
  if (steps_card_value_label_ != nullptr) {
    set_content_width_label(steps_card_value_label_, value_text, steps_card_value_font(steps_model_.daily_steps), 0xF8FAFC);
  }
  if (steps_card_unit_label_ != nullptr && steps_card_value_label_ != nullptr) {
    lv_obj_align_to(steps_card_unit_label_, steps_card_value_label_, LV_ALIGN_OUT_RIGHT_MID, -2, 3);
  }
  if (steps_arc_ != nullptr) {
    const std::int32_t clamped_steps = std::clamp<std::int32_t>(static_cast<std::int32_t>(steps_model_.daily_steps), 0, 6000);
    lv_arc_set_angles(steps_arc_, 180, 180 + std::clamp(clamped_steps * 180 / 6000, 6, 180));
  }
}

void StepsAppPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&StepsAppPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void StepsAppPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

void StepsAppPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<StepsAppPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void StepsAppPage::bind_input() {
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

PageId StepsDataInfoPage::id() const {
  return PageId::PedometerDataInfo;
}

const char* StepsDataInfoPage::name() const {
  return page_name(PageId::PedometerDataInfo);
}

void StepsDataInfoPage::on_will_disappear() {
  stop_crown_release_timer();
}

lv_obj_t* StepsDataInfoPage::build() {
  struct InfoBlock {
    const char* title;
    const char* body;
    std::uint32_t accent;
    bool foot;
  };

  static const std::array<InfoBlock, 4> kInfoBlocks {{
      {"步数",
       "步数用于估计日常活动量，低于5000步通常属于静坐少动；8000-10000步更接近活跃生活方式。",
       0xF6D34D,
       true},
      {"卡路里",
       "卡路里反映体力活动消耗的能量，本页仅展示模拟数据，后续可由传感器和算法替换。",
       0xF97316,
       false},
      {"中高强度活动",
       "中高强度活动在心率或步频达到阈值时记录，用于观察每天有效运动时长。",
       0x14B8FF,
       false},
      {"站立",
       "久坐少动可能带来健康风险，日常佩戴手表时建议每小时起身活动一次。",
       0x7DD3FC,
       false},
  }};

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t card_w = std::min<lv_coord_t>(screen_w - 18, 224);
  scroll_root_ = create_steps_scroll_root(root, screen_w, screen_h);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }

  lv_obj_t* header = create_steps_panel(scroll_root_, card_w, 58, 0x07111D);
  lv_obj_t* back_button = header == nullptr ? nullptr : lv_button_create(header);
  lv_obj_t* back = back_button == nullptr ? nullptr : create_steps_label(back_button, "<", &lv_font_montserrat_20, 0xCDEBFF, 18);
  lv_obj_t* title =
      header == nullptr ? nullptr : create_steps_label(header, "数据说明", cjk_font_16(), 0xF8FAFC, 126);
  lv_obj_t* time = header == nullptr ? nullptr : create_steps_label(header, "17:47", &lv_font_montserrat_18, 0xD8E9FF, 58);
  if (header == nullptr || back_button == nullptr || back == nullptr || title == nullptr || time == nullptr) {
    return nullptr;
  }
  lv_obj_set_style_radius(header, 20, 0);
  ui_prepare_box(back_button);
  lv_obj_set_size(back_button, 40, 40);
  lv_obj_align(back_button, LV_ALIGN_LEFT_MID, 6, 0);
  lv_obj_set_style_bg_opa(back_button, LV_OPA_TRANSP, 0);
  attach_click_guard(back_button);
  lv_obj_add_event_cb(back_button, &StepsDataInfoPage::back_event_cb, LV_EVENT_CLICKED, this);
  ui_set_touch_target(back_button, 18);
  lv_obj_add_flag(back, LV_OBJ_FLAG_EVENT_BUBBLE);
  lv_obj_remove_flag(back, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(back, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_align(title, LV_ALIGN_LEFT_MID, 34, 0);
  lv_obj_align(time, LV_ALIGN_RIGHT_MID, -14, 0);

  for (const InfoBlock& block : kInfoBlocks) {
    lv_obj_t* card = create_steps_panel(scroll_root_, card_w, 170, 0x07111D);
    lv_obj_t* icon = card == nullptr ? nullptr : create_steps_icon(card, 30, 0x1A2430, block.accent, block.foot);
    lv_obj_t* label = card == nullptr ? nullptr : create_steps_label(card, block.title, cjk_font_16(), 0xF8FAFC, card_w - 64);
    lv_obj_t* body = card == nullptr ? nullptr : create_steps_label(card, block.body, cjk_font_16(), 0xD8E9FF, card_w - 32, LV_LABEL_LONG_WRAP);
    if (card == nullptr || icon == nullptr || label == nullptr || body == nullptr) {
      return nullptr;
    }
    lv_obj_set_style_radius(card, 18, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 14, 14);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 52, 17);
    lv_obj_align(body, LV_ALIGN_TOP_LEFT, 16, 58);
  }

  bind_input();
  return root;
}

void StepsDataInfoPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<StepsDataInfoPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }
  self->request_navigation({NavigationAction::Pop, PageId::Watchface});
}

void StepsDataInfoPage::apply_crown_drag(bool forward, std::int16_t detents) {
  stop_crown_release_timer();
  apply_stream_crown_drag(scroll_root_, forward, detents);
  schedule_crown_release();
}

void StepsDataInfoPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&StepsDataInfoPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void StepsDataInfoPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

void StepsDataInfoPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<StepsDataInfoPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->crown_release_timer_.release();
  release_stream_crown_drag(self->scroll_root_);
}

void StepsDataInfoPage::bind_input() {
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

}  // namespace twsim::app
