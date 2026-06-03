#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPages.h"

#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/Pages/Shell/QuickSettings/ShellQuickSettingsPrimitives.h"

#include <chrono>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <string>

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace twsim::app {

namespace {

constexpr lv_coord_t kQuickSettingsCloseDragThreshold = 96;
constexpr lv_coord_t kQuickSettingsCloseFlickThreshold = 20;
constexpr lv_coord_t kQuickSettingsMaxDragOffset = 220;
constexpr lv_coord_t kQuickSettingsOpenCommitThreshold = 148;
constexpr lv_coord_t kQuickSettingsSheetY = 46;

const char* quick_settings_log_path() {
  static const std::string path = (std::filesystem::current_path() / "quicksettings_debug.log").string();
  return path.c_str();
}

void append_quick_settings_log(const char* phase,
                               std::size_t index,
                               const char* kind,
                               bool suppress_next_click,
                               bool guard_allows,
                               lv_obj_t* target,
                               lv_event_t* event) {
  std::FILE* file = std::fopen(quick_settings_log_path(), "a");
  if (file == nullptr) {
    return;
  }

  lv_point_t point {0, 0};
  if (event != nullptr) {
    if (lv_indev_t* indev = lv_event_get_indev(event)) {
      lv_indev_get_point(indev, &point);
    }
  }

  const auto now = std::chrono::steady_clock::now().time_since_epoch();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  std::fprintf(file,
               "[%lld] %s idx=%zu kind=%s suppress=%d guard=%d target=%p x=%d y=%d\n",
               static_cast<long long>(ms),
               phase != nullptr ? phase : "Unknown",
               index,
               kind != nullptr ? kind : "Unknown",
               suppress_next_click ? 1 : 0,
               guard_allows ? 1 : 0,
               static_cast<void*>(target),
               static_cast<int>(point.x),
               static_cast<int>(point.y));
  std::fclose(file);
}

void reset_quick_settings_log() {
  std::FILE* file = std::fopen(quick_settings_log_path(), "w");
  if (file == nullptr) {
    return;
  }
  std::fclose(file);
}

}  // namespace

QuickSettingsPage::QuickSettingsPage(DataCenter& data_center) : PageBase(data_center) {}

PageId QuickSettingsPage::id() const {
  return PageId::QuickSettings;
}

const char* QuickSettingsPage::name() const {
  return "QuickSettings";
}

void QuickSettingsPage::on_will_appear() {
  PageBase::on_will_appear();
  reset_quick_settings_log();
  hide_toggle_toast();
  stop_preview_close_timer();
  shell_drag_offset_ = 0;
  open_preview_progress_ = 0;
  shell_drag_active_ = false;
  if (sheet_container_ != nullptr) {
    lv_obj_set_y(sheet_container_, kQuickSettingsSheetY);
  }
  refresh_backdrop();
  if (suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= suppress_click_deadline_) {
    suppress_next_click_ = false;
    suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
  }
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
}

void QuickSettingsPage::on_will_disappear() {
  stop_toast_timer();
  stop_preview_close_timer();
  hide_toggle_toast();
  hide_long_battery_confirm();
  if (!suppress_next_click_) {
    long_press_source_button_ = nullptr;
  }
  PageBase::on_will_disappear();
}

lv_obj_t* QuickSettingsPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x050913);
  lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);

  shell_quick_settings::QuickSettingsPrimitivesView primitives {};
  if (!shell_quick_settings::create_quick_settings_primitives(root, primitives)) {
    return nullptr;
  }
  backdrop_root_ = primitives.backdrop_root;
  backdrop_battery_icon_label_ = primitives.backdrop_battery_icon_label;
  backdrop_battery_label_ = primitives.backdrop_battery_label;
  backdrop_style_stage_ = primitives.backdrop_style_stage;
  backdrop_minute_label_ = primitives.backdrop_minute_label;
  sheet_container_ = primitives.sheet_container;
  toast_container_ = primitives.toast_container;
  toast_label_ = primitives.toast_label;
  long_battery_confirm_overlay_ = primitives.long_battery_confirm_overlay;
  drag_handle_ = primitives.drag_handle;
  if (backdrop_root_ == nullptr || backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr ||
      backdrop_style_stage_ == nullptr || backdrop_minute_label_ == nullptr || sheet_container_ == nullptr ||
      toast_container_ == nullptr || toast_label_ == nullptr || long_battery_confirm_overlay_ == nullptr ||
      primitives.long_battery_confirm_body == nullptr || primitives.long_battery_confirm_cancel_button == nullptr ||
      primitives.long_battery_confirm_confirm_button == nullptr || drag_handle_ == nullptr ||
      primitives.toggle_grid == nullptr) {
    return nullptr;
  }

  backdrop_renderer_ = create_watchface_style_renderer(backdrop_config_);
  if (!backdrop_renderer_ || backdrop_renderer_->build(backdrop_style_stage_) == nullptr) {
    return nullptr;
  }

  for (lv_obj_t* button :
       {primitives.long_battery_confirm_cancel_button, primitives.long_battery_confirm_confirm_button}) {
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &QuickSettingsPage::long_battery_confirm_event_cb, LV_EVENT_CLICKED, this);
  }
  lv_obj_set_user_data(primitives.long_battery_confirm_cancel_button, reinterpret_cast<void*>(0U));
  lv_obj_set_user_data(primitives.long_battery_confirm_confirm_button, reinterpret_cast<void*>(1U));

  for (std::size_t index = 0; index < toggles_.size(); ++index) {
    lv_obj_t* button = primitives.toggle_buttons[index];
    lv_obj_t* icon = primitives.toggle_icon_labels[index];
    if (button == nullptr || icon == nullptr) {
      return nullptr;
    }
    toggles_[index].button = button;
    attach_click_guard(button);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_long_press_event_cb, LV_EVENT_LONG_PRESSED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_release_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(button, &QuickSettingsPage::toggle_press_lost_event_cb, LV_EVENT_PRESS_LOST, this);
    lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));
    toggles_[index].icon_label = icon;
    lv_label_set_text(icon, toggles_[index].icon_text);
    apply_toggle_visual(index);
  }

  bind_input();
  bind_display_policy();
  bind_backdrop();
  refresh_backdrop();
  set_open_preview_progress(0, false);
  return root;
}

void QuickSettingsPage::close_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::toggle_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("CLICK_ENTER", index, label, self->suppress_next_click_, true, target, event);
  const bool guard_allows = target != nullptr && click_guard_allows(target);
  append_quick_settings_log("CLICK_GUARD", index, label, self->suppress_next_click_, guard_allows, target, event);
  if (target == nullptr || !guard_allows) {
    return;
  }
  if (self->suppress_next_click_ &&
      self->suppress_click_deadline_ != std::chrono::steady_clock::time_point {} &&
      std::chrono::steady_clock::now() >= self->suppress_click_deadline_) {
    append_quick_settings_log(
        "CLICK_SUPPRESS_EXPIRED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
  }
  if (self->suppress_next_click_) {
    append_quick_settings_log("CLICK_SWALLOWED", index, label, self->suppress_next_click_, true, target, event);
    self->suppress_next_click_ = false;
    self->suppress_click_deadline_ = std::chrono::steady_clock::time_point {};
    self->long_press_source_button_ = nullptr;
    return;
  }

  if (index >= self->toggles_.size()) {
    append_quick_settings_log("CLICK_INDEX_OOB", index, label, self->suppress_next_click_, true, target, event);
    return;
  }

  auto& toggle = self->toggles_[index];
  append_quick_settings_log("CLICK_HANDLE", index, toggle.label, self->suppress_next_click_, true, target, event);
  if (toggle.kind == ToggleKind::OpenSettings) {
    append_quick_settings_log(
        "CLICK_NAV_SETTINGS", index, toggle.label, self->suppress_next_click_, true, target, event);
    self->request_navigation({NavigationAction::LaunchApp, PageId::SettingsHome});
    return;
  }

  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->notification_wake_enabled;
    self->data_center_.set_notification_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
  } else if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = self->data_center_.display_policy();
    const bool current = !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
    self->data_center_.set_raise_to_wake_enabled(!current);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(!current ? "抬腕亮屏已开启" : "抬腕亮屏已关闭");
  } else if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = self->data_center_.display_policy();
    const bool current = policy && policy->keep_screen_on_duration_ms > 0U;
    self->data_center_.set_keep_screen_on_duration_ms(current ? 0U : 300000U);
    toggle.mode = current ? 0 : 1;
    self->show_toggle_toast(current ? "持续亮屏已关闭" : "持续亮屏5分钟");
  } else if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = self->data_center_.power_mode();
    const bool current = mode && mode->long_battery_mode_enabled;
    if (current) {
      self->data_center_.set_long_battery_mode_enabled(false);
      toggle.mode = 0;
    } else {
      self->show_long_battery_confirm();
      return;
    }
  } else {
    toggle.mode = toggle.mode == 0 ? 1 : 0;
  }
  self->apply_toggle_visual(index);
  append_quick_settings_log("CLICK_DONE", index, toggle.label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toggle_long_press_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr) {
    return;
  }
  const auto index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  const char* label = index < self->toggles_.size() ? self->toggles_[index].label : "oob";
  append_quick_settings_log("LONG_PRESS_ENTER", index, label, self->suppress_next_click_, true, target, event);
  if (index >= self->toggles_.size()) {
    return;
  }

  self->stop_toast_timer();
  self->hide_toggle_toast();
  if (!self->toggles_[index].detail_page.has_value()) {
    return;
  }
  self->suppress_next_click_ = true;
  self->suppress_click_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(1200);
  self->long_press_source_button_ = target;
  self->suppress_global_clicks_for(std::chrono::milliseconds(520));
  append_quick_settings_log(
      "LONG_PRESS_NAV", index, self->toggles_[index].label, self->suppress_next_click_, true, target, event);
  self->request_navigation({NavigationAction::LaunchApp, *self->toggles_[index].detail_page});
}

void QuickSettingsPage::toggle_release_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("RELEASE", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
}

void QuickSettingsPage::toggle_press_lost_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  std::size_t index = static_cast<std::size_t>(-1);
  const char* label = "null";
  if (target != nullptr) {
    index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
    if (index < self->toggles_.size()) {
      label = self->toggles_[index].label;
    }
  }
  append_quick_settings_log("PRESS_LOST", index, label, self->suppress_next_click_, true, target, event);
  if (target == nullptr || target != self->long_press_source_button_) {
    return;
  }
  self->long_press_source_button_ = nullptr;
  append_quick_settings_log(
      "PRESS_LOST_SOURCE_CLEARED", index, label, self->suppress_next_click_, true, target, event);
}

void QuickSettingsPage::toast_timeout_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->toast_timer_.release();
  self->hide_toggle_toast();
}

void QuickSettingsPage::long_battery_confirm_event_cb(lv_event_t* event) {
  auto* self = static_cast<QuickSettingsPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto confirmed = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)) == 1U;
  self->hide_long_battery_confirm();
  if (!confirmed) {
    return;
  }

  self->data_center_.set_long_battery_mode_enabled(true);
}

void QuickSettingsPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 switch (command->action) {
                                   case InputAction::ScrollDrag:
                                     if (should_capture_shell_drag(*command)) {
                                       set_close_drag_offset(
                                           clamp_coord(static_cast<lv_coord_t>(command->value),
                                                       0,
                                                       kQuickSettingsMaxDragOffset),
                                           false);
                                     }
                                     break;
                                   case InputAction::ScrollFlick:
                                     if (should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, true);
                                     }
                                     break;
                                   case InputAction::ScrollRelease:
                                     if (shell_drag_active_ || shell_drag_offset_ > 0 ||
                                         should_capture_shell_drag(*command)) {
                                       finish_drag_close(command->value, false);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

void QuickSettingsPage::bind_display_policy() {
  track(data_center_.subscribe(EventId::DisplayPolicyChanged,
                               [this](const Event& event) {
                                 const auto* policy = std::get_if<DisplayPolicyModel>(&event.payload);
                                 if (policy == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   switch (toggles_[index].kind) {
                                     case ToggleKind::NotifyWake:
                                       toggles_[index].mode = policy->notification_wake_enabled ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::RaiseToWake:
                                       toggles_[index].mode =
                                           policy->raise_to_wake_mode != RaiseToWakeMode::Off ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     case ToggleKind::AodFiveMinutes:
                                       toggles_[index].mode = policy->keep_screen_on_duration_ms > 0U ? 1 : 0;
                                       apply_toggle_visual(index);
                                       break;
                                     default:
                                       break;
                                   }
                                 }
                               }));
  track(data_center_.subscribe(EventId::PowerModeChanged,
                               [this](const Event& event) {
                                 const auto* mode = std::get_if<PowerModeModel>(&event.payload);
                                 if (mode == nullptr) {
                                   return;
                                 }
                                 for (std::size_t index = 0; index < toggles_.size(); ++index) {
                                   if (toggles_[index].kind == ToggleKind::LongBattery) {
                                     toggles_[index].mode = mode->long_battery_mode_enabled ? 1 : 0;
                                     apply_toggle_visual(index);
                                   }
                                 }
                               }));
}

void QuickSettingsPage::bind_backdrop() {
  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_backdrop_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_backdrop_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::ShellPreviewRequested,
                               [this](const Event& event) {
                                 const auto* preview = std::get_if<ShellPreviewModel>(&event.payload);
                                 if (preview == nullptr || preview->page_id != PageId::QuickSettings ||
                                     root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }

                                 if (!preview->active) {
                                   shell_drag_active_ = false;
                                   shell_drag_offset_ = 0;
                                   if (open_preview_progress_ >= kQuickSettingsOpenCommitThreshold) {
                                     set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   } else {
                                     open_preview_progress_ = 0;
                                     set_open_preview_progress(0, true);
                                     stop_preview_close_timer();
                                     preview_close_timer_.reset(
                                         lv_timer_create(&QuickSettingsPage::preview_close_timer_cb, 240U, this));
                                     if (preview_close_timer_.get() != nullptr) {
                                       lv_timer_set_repeat_count(preview_close_timer_.get(), 1);
                                     }
                                   }
                                   return;
                                 }

                                 stop_preview_close_timer();
                                 if (preview->commit) {
                                   open_preview_progress_ = kQuickSettingsMaxDragOffset;
                                   set_open_preview_progress(kQuickSettingsMaxDragOffset, true);
                                   return;
                                 }

                                 set_open_preview_progress(clamp_coord(static_cast<lv_coord_t>(preview->progress),
                                                                       0,
                                                                       kQuickSettingsMaxDragOffset),
                                                           false);
                               }));
}

void QuickSettingsPage::refresh_backdrop() {
  if (const auto& time = data_center_.time(); time) {
    apply_backdrop_time(*time);
  }
  if (const auto& battery = data_center_.battery(); battery) {
    apply_backdrop_battery(*battery);
  }
}

void QuickSettingsPage::apply_backdrop_time(const TimeModel& model) {
  if (backdrop_minute_label_ == nullptr || backdrop_renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(backdrop_minute_label_, "--");
    backdrop_render_state_.hour_text = "--";
    backdrop_render_state_.minute_text = "--";
    backdrop_render_state_.spread_index = backdrop_config_.spread_index;
    backdrop_renderer_->apply(backdrop_render_state_);
    return;
  }

  std::uint8_t hour = model.hour % 12;
  if (hour == 0) {
    hour = 12;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", static_cast<unsigned>(hour));
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));

  lv_label_set_text(backdrop_minute_label_, minute_buffer);
  backdrop_render_state_.hour_text = hour_buffer;
  backdrop_render_state_.minute_text = minute_buffer;
  backdrop_render_state_.spread_index = backdrop_config_.spread_index;
  backdrop_renderer_->apply(backdrop_render_state_);
}

void QuickSettingsPage::apply_backdrop_battery(const BatteryModel& model) {
  if (backdrop_battery_icon_label_ == nullptr || backdrop_battery_label_ == nullptr) {
    return;
  }

  lv_label_set_text(backdrop_battery_icon_label_, LV_SYMBOL_CHARGE);
  char buffer[8] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%", static_cast<int>(model.percent));
  lv_label_set_text(backdrop_battery_label_, buffer);
}

void QuickSettingsPage::apply_toggle_visual(std::size_t index) {
  if (index >= toggles_.size() || toggles_[index].button == nullptr || toggles_[index].icon_label == nullptr) {
    return;
  }

  auto& toggle = toggles_[index];
  const bool active = is_toggle_active(toggle);

  lv_obj_set_style_bg_color(toggle.button, active ? lv_color_hex(0x1493FF) : lv_color_hex(0x15294A), 0);
  lv_obj_set_style_bg_opa(toggle.button, LV_OPA_COVER, 0);
  lv_obj_set_style_text_color(toggle.icon_label, lv_color_hex(0xF5FAFF), 0);
}

bool QuickSettingsPage::is_toggle_active(const ToggleState& toggle) const {
  if (toggle.kind == ToggleKind::NotifyWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->notification_wake_enabled;
  }
  if (toggle.kind == ToggleKind::RaiseToWake) {
    const auto policy = data_center_.display_policy();
    return !policy || policy->raise_to_wake_mode != RaiseToWakeMode::Off;
  }
  if (toggle.kind == ToggleKind::AodFiveMinutes) {
    const auto policy = data_center_.display_policy();
    return policy && policy->keep_screen_on_duration_ms > 0U;
  }
  if (toggle.kind == ToggleKind::LongBattery) {
    const auto mode = data_center_.power_mode();
    return mode && mode->long_battery_mode_enabled;
  }
  if (toggle.kind == ToggleKind::OpenSettings) {
    return false;
  }
  return toggle.mode != 0;
}

void QuickSettingsPage::show_long_battery_confirm() {
  if (long_battery_confirm_overlay_ == nullptr) {
    return;
  }
  hide_toggle_toast();
  lv_obj_clear_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(long_battery_confirm_overlay_);
}

void QuickSettingsPage::hide_long_battery_confirm() {
  if (long_battery_confirm_overlay_ != nullptr) {
    lv_obj_add_flag(long_battery_confirm_overlay_, LV_OBJ_FLAG_HIDDEN);
  }
}

void QuickSettingsPage::show_toggle_toast(const char* text) {
  if (toast_container_ == nullptr || toast_label_ == nullptr || text == nullptr) {
    return;
  }
  lv_label_set_text(toast_label_, text);
  lv_obj_update_layout(toast_label_);
  const lv_coord_t label_width = static_cast<lv_coord_t>(lv_obj_get_width(toast_label_) + 1);
  const lv_coord_t label_height = static_cast<lv_coord_t>(lv_obj_get_height(toast_label_) + 1);
  lv_obj_set_size(toast_container_,
                  static_cast<lv_coord_t>(label_width + 32),
                  static_cast<lv_coord_t>(label_height + 16));
  lv_obj_update_layout(toast_container_);
  lv_obj_clear_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
  lv_obj_move_foreground(toast_container_);
  stop_toast_timer();
  toast_timer_.reset(lv_timer_create(&QuickSettingsPage::toast_timeout_cb, 3000U, this));
  if (toast_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(toast_timer_.get(), 1);
  }
}

void QuickSettingsPage::hide_toggle_toast() {
  if (toast_container_ == nullptr) {
    return;
  }
  lv_obj_add_flag(toast_container_, LV_OBJ_FLAG_HIDDEN);
}

void QuickSettingsPage::stop_toast_timer() {
  if (toast_timer_) {
    toast_timer_.reset();
  }
}

void QuickSettingsPage::set_open_preview_progress(lv_coord_t progress, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  open_preview_progress_ = clamp_coord(progress, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t hidden_y = 296;
  const lv_coord_t target_y = static_cast<lv_coord_t>(
      hidden_y - ((hidden_y - kQuickSettingsSheetY) * open_preview_progress_) / kQuickSettingsMaxDragOffset);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);
}

void QuickSettingsPage::set_close_drag_offset(lv_coord_t offset, bool animated) {
  if (sheet_container_ == nullptr) {
    return;
  }

  shell_drag_offset_ = clamp_coord(offset, 0, kQuickSettingsMaxDragOffset);
  const lv_coord_t target_y = static_cast<lv_coord_t>(kQuickSettingsSheetY + shell_drag_offset_);
  if (!animated) {
    lv_obj_set_y(sheet_container_, target_y);
    return;
  }

  lv_anim_t sheet_anim;
  lv_anim_init(&sheet_anim);
  lv_anim_set_var(&sheet_anim, sheet_container_);
  lv_anim_set_exec_cb(&sheet_anim, [](void* obj, int32_t value) {
    if (obj != nullptr) {
      lv_obj_set_y(static_cast<lv_obj_t*>(obj), static_cast<lv_coord_t>(value));
    }
  });
  lv_anim_set_values(&sheet_anim, lv_obj_get_y(sheet_container_), target_y);
  lv_anim_set_duration(&sheet_anim, 220);
  lv_anim_set_path_cb(&sheet_anim, lv_anim_path_ease_out);
  lv_anim_start(&sheet_anim);
}

void QuickSettingsPage::stop_preview_close_timer() {
  if (preview_close_timer_) {
    preview_close_timer_.reset();
  }
}

void QuickSettingsPage::preview_close_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<QuickSettingsPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }
  self->preview_close_timer_.release();
  self->request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
}

void QuickSettingsPage::finish_drag_close(std::int16_t release_delta, bool flick_close) {
  if (root_ == nullptr) {
    return;
  }

  shell_drag_active_ = false;
  const bool should_close =
      shell_drag_offset_ >= kQuickSettingsCloseDragThreshold ||
      (flick_close && release_delta >= kQuickSettingsCloseFlickThreshold);
  if (should_close) {
    shell_drag_offset_ = 0;
    request_navigation({NavigationAction::CloseShellSurface, PageId::Watchface});
    return;
  }

  shell_drag_offset_ = 0;
  set_close_drag_offset(0, true);
}

bool QuickSettingsPage::is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const {
  if (drag_handle_ == nullptr) {
    return false;
  }
  lv_area_t coords {};
  lv_obj_get_coords(drag_handle_, &coords);
  return x >= coords.x1 - 20 && x <= coords.x2 + 20 && y >= coords.y1 - 18 && y <= coords.y2 + 24;
}

bool QuickSettingsPage::should_capture_shell_drag(const InputCommand& command) const {
  if (shell_drag_active_) {
    return command.value >= 0 || shell_drag_offset_ > 0;
  }
  if (command.value <= 0) {
    return false;
  }
  if (is_handle_drag_start_zone(command.x, command.y)) {
    const_cast<QuickSettingsPage*>(this)->shell_drag_active_ = true;
    return true;
  }
  return false;
}

}  // namespace twsim::app
