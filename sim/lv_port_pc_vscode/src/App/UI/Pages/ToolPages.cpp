#include "App/UI/Pages/ToolPages.h"

#include <algorithm>
#include <cstdio>
#include <cstdint>

namespace twsim::app {

namespace {

void style_tool_root(lv_obj_t* root) {
  lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(root, lv_color_hex(0x06111B), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root, 0, 0);
}

lv_obj_t* create_small_button(lv_obj_t* parent, const char* text, lv_event_cb_t cb, void* user_data, std::uintptr_t action) {
  lv_obj_t* button = lv_button_create(parent);
  if (button == nullptr) {
    return nullptr;
  }
  lv_obj_set_size(button, 48, 34);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x1E293B), 0);
  lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(button, 0, 0);
  lv_obj_set_style_radius(button, 14, 0);
  lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, user_data);
  lv_obj_set_user_data(button, reinterpret_cast<void*>(action));

  lv_obj_t* label = lv_label_create(button);
  if (label == nullptr) {
    return nullptr;
  }
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(label);
  return button;
}

}  // namespace

StopwatchPage::StopwatchPage(DataCenter& data_center) : PageBase(data_center) {}

StopwatchPage::~StopwatchPage() {
  if (timer_ != nullptr) {
    lv_timer_delete(timer_);
  }
}

PageId StopwatchPage::id() const {
  return PageId::Stopwatch;
}

const char* StopwatchPage::name() const {
  return "Stopwatch";
}

lv_obj_t* StopwatchPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_tool_root(root);

  if (create_small_button(root, "Back", &StopwatchPage::back_event_cb, this, 0) == nullptr) {
    return nullptr;
  }

  lv_obj_t* title = lv_label_create(root);
  time_label_ = lv_label_create(root);
  lv_obj_t* actions = lv_obj_create(root);
  if (title == nullptr || time_label_ == nullptr || actions == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Stopwatch");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

  lv_obj_align(time_label_, LV_ALIGN_CENTER, 0, -22);
  lv_obj_set_style_text_font(time_label_, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0x67E8F9), 0);

  lv_obj_set_size(actions, 220, 48);
  lv_obj_align(actions, LV_ALIGN_BOTTOM_MID, 0, -18);
  lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(actions, 0, 0);
  lv_obj_set_style_pad_gap(actions, 8, 0);
  lv_obj_set_style_bg_opa(actions, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(actions, 0, 0);
  lv_obj_remove_flag(actions, LV_OBJ_FLAG_SCROLLABLE);

  if (create_small_button(actions, "Start", &StopwatchPage::action_event_cb, this, 1) == nullptr ||
      create_small_button(actions, "Pause", &StopwatchPage::action_event_cb, this, 2) == nullptr ||
      create_small_button(actions, "Reset", &StopwatchPage::action_event_cb, this, 3) == nullptr) {
    return nullptr;
  }

  timer_ = lv_timer_create(&StopwatchPage::timer_cb, 100, this);
  update_display();
  return root;
}

void StopwatchPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<StopwatchPage*>(lv_event_get_user_data(event));
  if (self != nullptr) {
    self->request_navigation({NavigationAction::Pop, PageId::Watchface});
  }
}

void StopwatchPage::action_event_cb(lv_event_t* event) {
  auto* self = static_cast<StopwatchPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(lv_event_get_target_obj(event)));
  const auto now = std::chrono::steady_clock::now();
  if (action == 1 && !self->running_) {
    self->started_at_ = now;
    self->running_ = true;
  } else if (action == 2 && self->running_) {
    self->elapsed_before_pause_ +=
        std::chrono::duration_cast<std::chrono::milliseconds>(now - self->started_at_);
    self->running_ = false;
  } else if (action == 3) {
    self->running_ = false;
    self->elapsed_before_pause_ = std::chrono::milliseconds(0);
  }
  self->update_display();
}

void StopwatchPage::timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<StopwatchPage*>(lv_timer_get_user_data(timer));
  if (self != nullptr && self->running_) {
    self->update_display();
  }
}

void StopwatchPage::update_display() const {
  if (time_label_ == nullptr) {
    return;
  }
  auto elapsed = elapsed_before_pause_;
  if (running_) {
    elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started_at_);
  }

  const auto total_ms = elapsed.count();
  const int minutes = static_cast<int>((total_ms / 1000) / 60);
  const int seconds = static_cast<int>((total_ms / 1000) % 60);
  const int centiseconds = static_cast<int>((total_ms / 10) % 100);
  char buffer[32] = {};
  std::snprintf(buffer, sizeof(buffer), "%02d:%02d.%02d", minutes, seconds, centiseconds);
  lv_label_set_text(time_label_, buffer);
}

TimerPage::TimerPage(DataCenter& data_center) : PageBase(data_center) {}

TimerPage::~TimerPage() {
  if (timer_ != nullptr) {
    lv_timer_delete(timer_);
  }
}

PageId TimerPage::id() const {
  return PageId::Timer;
}

const char* TimerPage::name() const {
  return "Timer";
}

lv_obj_t* TimerPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_tool_root(root);

  if (create_small_button(root, "Back", &TimerPage::back_event_cb, this, 0) == nullptr) {
    return nullptr;
  }

  lv_obj_t* title = lv_label_create(root);
  time_label_ = lv_label_create(root);
  state_label_ = lv_label_create(root);
  lv_obj_t* top_actions = lv_obj_create(root);
  lv_obj_t* bottom_actions = lv_obj_create(root);
  if (title == nullptr || time_label_ == nullptr || state_label_ == nullptr ||
      top_actions == nullptr || bottom_actions == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Timer");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

  lv_obj_align(time_label_, LV_ALIGN_CENTER, 0, -28);
  lv_obj_set_style_text_font(time_label_, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(time_label_, lv_color_hex(0xFBBF24), 0);

  lv_obj_align(state_label_, LV_ALIGN_CENTER, 0, 6);
  lv_obj_set_style_text_font(state_label_, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(state_label_, lv_color_hex(0x94A3B8), 0);

  for (lv_obj_t* row : {top_actions, bottom_actions}) {
    lv_obj_set_size(row, 220, 40);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 8, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
  }
  lv_obj_align(top_actions, LV_ALIGN_BOTTOM_MID, 0, -58);
  lv_obj_align(bottom_actions, LV_ALIGN_BOTTOM_MID, 0, -14);

  if (create_small_button(top_actions, "-1m", &TimerPage::action_event_cb, this, 1) == nullptr ||
      create_small_button(top_actions, "+1m", &TimerPage::action_event_cb, this, 2) == nullptr ||
      create_small_button(bottom_actions, "Start", &TimerPage::action_event_cb, this, 3) == nullptr ||
      create_small_button(bottom_actions, "Pause", &TimerPage::action_event_cb, this, 4) == nullptr ||
      create_small_button(bottom_actions, "Reset", &TimerPage::action_event_cb, this, 5) == nullptr) {
    return nullptr;
  }

  timer_ = lv_timer_create(&TimerPage::timer_cb, 200, this);
  update_display();
  return root;
}

void TimerPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<TimerPage*>(lv_event_get_user_data(event));
  if (self != nullptr) {
    self->request_navigation({NavigationAction::Pop, PageId::Watchface});
  }
}

void TimerPage::action_event_cb(lv_event_t* event) {
  auto* self = static_cast<TimerPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(lv_event_get_target_obj(event)));
  const auto now = std::chrono::steady_clock::now();

  if (action == 1 && !self->running_) {
    self->configured_ = std::max(std::chrono::seconds(60), self->configured_ - std::chrono::minutes(1));
    self->remaining_before_start_ = self->configured_;
  } else if (action == 2 && !self->running_) {
    self->configured_ += std::chrono::minutes(1);
    self->remaining_before_start_ = self->configured_;
  } else if (action == 3 && !self->running_) {
    self->started_at_ = now;
    self->running_ = true;
  } else if (action == 4 && self->running_) {
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - self->started_at_);
    self->remaining_before_start_ =
        elapsed >= self->remaining_before_start_ ? std::chrono::seconds(0) : self->remaining_before_start_ - elapsed;
    self->running_ = false;
  } else if (action == 5) {
    self->running_ = false;
    self->remaining_before_start_ = self->configured_;
  }
  self->update_display();
}

void TimerPage::timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<TimerPage*>(lv_timer_get_user_data(timer));
  if (self != nullptr && self->running_) {
    self->update_display();
  }
}

void TimerPage::update_display() {
  if (time_label_ == nullptr || state_label_ == nullptr) {
    return;
  }

  auto remaining = remaining_before_start_;
  if (running_) {
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - started_at_);
    remaining = elapsed >= remaining_before_start_ ? std::chrono::seconds(0) : remaining_before_start_ - elapsed;
    if (remaining.count() == 0) {
      running_ = false;
      remaining_before_start_ = configured_;
      lv_label_set_text(state_label_, "Countdown done");
    }
  }

  char buffer[16] = {};
  const auto total_seconds = static_cast<int>(remaining.count());
  std::snprintf(buffer, sizeof(buffer), "%02d:%02d", total_seconds / 60, total_seconds % 60);
  lv_label_set_text(time_label_, buffer);
  if (!running_ && remaining.count() != 0) {
    lv_label_set_text(state_label_, "Ready");
  } else if (running_) {
    lv_label_set_text(state_label_, "Counting down");
  }
}

AlarmPage::AlarmPage(DataCenter& data_center) : PageBase(data_center) {}

PageId AlarmPage::id() const {
  return PageId::Alarm;
}

const char* AlarmPage::name() const {
  return "Alarm";
}

lv_obj_t* AlarmPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_tool_root(root);

  if (create_small_button(root, "Back", &AlarmPage::back_event_cb, this, 0) == nullptr) {
    return nullptr;
  }

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* weekday_row = lv_obj_create(root);
  summary_label_ = lv_label_create(root);
  lv_obj_t* adjust_row = lv_obj_create(root);
  lv_obj_t* arm_button = create_small_button(root, "Arm", &AlarmPage::arm_event_cb, this, 0);
  if (title == nullptr || weekday_row == nullptr || summary_label_ == nullptr ||
      adjust_row == nullptr || arm_button == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, "Alarm");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

  lv_obj_set_size(weekday_row, 220, 44);
  lv_obj_align(weekday_row, LV_ALIGN_TOP_MID, 0, 54);
  lv_obj_set_flex_flow(weekday_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(weekday_row, 0, 0);
  lv_obj_set_style_pad_gap(weekday_row, 4, 0);
  lv_obj_set_style_bg_opa(weekday_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(weekday_row, 0, 0);
  lv_obj_remove_flag(weekday_row, LV_OBJ_FLAG_SCROLLABLE);

  static const char* days[] = {"M", "T", "W", "T", "F", "S", "S"};
  for (std::size_t i = 0; i < weekdays_.size(); ++i) {
    lv_obj_t* button = lv_button_create(weekday_row);
    if (button == nullptr) {
      return nullptr;
    }
    lv_obj_set_size(button, 28, 32);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_add_event_cb(button, &AlarmPage::weekday_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(i)));
    lv_obj_t* label = lv_label_create(button);
    if (label == nullptr) {
      return nullptr;
    }
    lv_label_set_text(label, days[i]);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
  }

  lv_obj_set_width(summary_label_, 200);
  lv_label_set_long_mode(summary_label_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(summary_label_, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(summary_label_, lv_color_hex(0xFBBF24), 0);
  lv_obj_align(summary_label_, LV_ALIGN_CENTER, 0, -6);

  lv_obj_set_size(adjust_row, 220, 40);
  lv_obj_align(adjust_row, LV_ALIGN_BOTTOM_MID, 0, -58);
  lv_obj_set_flex_flow(adjust_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(adjust_row, 0, 0);
  lv_obj_set_style_pad_gap(adjust_row, 8, 0);
  lv_obj_set_style_bg_opa(adjust_row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(adjust_row, 0, 0);
  lv_obj_remove_flag(adjust_row, LV_OBJ_FLAG_SCROLLABLE);

  if (create_small_button(adjust_row, "-1h", &AlarmPage::adjust_event_cb, this, 1) == nullptr ||
      create_small_button(adjust_row, "+1h", &AlarmPage::adjust_event_cb, this, 2) == nullptr ||
      create_small_button(adjust_row, "-5m", &AlarmPage::adjust_event_cb, this, 3) == nullptr ||
      create_small_button(adjust_row, "+5m", &AlarmPage::adjust_event_cb, this, 4) == nullptr) {
    return nullptr;
  }

  lv_obj_align(arm_button, LV_ALIGN_BOTTOM_MID, 0, -14);
  update_display();
  return root;
}

void AlarmPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<AlarmPage*>(lv_event_get_user_data(event));
  if (self != nullptr) {
    self->request_navigation({NavigationAction::Pop, PageId::Watchface});
  }
}

void AlarmPage::weekday_event_cb(lv_event_t* event) {
  auto* self = static_cast<AlarmPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (index >= self->weekdays_.size()) {
    return;
  }
  self->weekdays_[index] = !self->weekdays_[index];
  self->update_display();
}

void AlarmPage::adjust_event_cb(lv_event_t* event) {
  auto* self = static_cast<AlarmPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto action = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
  if (action == 1) {
    self->hour_ = (self->hour_ + 23) % 24;
  } else if (action == 2) {
    self->hour_ = (self->hour_ + 1) % 24;
  } else if (action == 3) {
    self->minute_ = (self->minute_ + 55) % 60;
  } else if (action == 4) {
    self->minute_ = (self->minute_ + 5) % 60;
  }
  self->update_display();
}

void AlarmPage::arm_event_cb(lv_event_t* event) {
  auto* self = static_cast<AlarmPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->armed_ = !self->armed_;
  self->update_display();
}

void AlarmPage::update_display() {
  if (summary_label_ == nullptr) {
    return;
  }

  char days[32] = {};
  std::size_t offset = 0;
  static const char* short_days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  for (std::size_t i = 0; i < weekdays_.size(); ++i) {
    if (!weekdays_[i]) {
      continue;
    }
    offset += std::snprintf(days + offset, sizeof(days) - offset, "%s ", short_days[i]);
  }
  if (offset == 0) {
    std::snprintf(days, sizeof(days), "No weekdays");
  }

  char summary[128] = {};
  std::snprintf(summary,
                sizeof(summary),
                "%02d:%02d\n%s\nState: %s",
                hour_,
                minute_,
                days,
                armed_ ? "Armed" : "Idle");
  lv_label_set_text(summary_label_, summary);
}

}  // namespace twsim::app
