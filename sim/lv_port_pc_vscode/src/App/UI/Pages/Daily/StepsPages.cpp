#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Daily/StepsPagePrimitives.h"
#include "App/UI/Pages/Health/HealthInfoPagePrimitives.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <array>
#include <algorithm>

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
