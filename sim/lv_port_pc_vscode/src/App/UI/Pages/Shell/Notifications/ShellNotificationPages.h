#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "App/UI/LvglLifecycle.h"
#include "App/UI/PageBase.h"
#include "App/UI/Watchface/WatchfaceStyle.h"

namespace twsim::app {

class NotificationsPage : public PageBase {
 public:
  explicit NotificationsPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void close_event_cb(lv_event_t* event);
  static void clear_event_cb(lv_event_t* event);
  static void clear_confirm_event_cb(lv_event_t* event);
  static void clear_cancel_event_cb(lv_event_t* event);
  static void detail_back_event_cb(lv_event_t* event);
  static void notification_card_event_cb(lv_event_t* event);
  static void notification_card_swipe_event_cb(lv_event_t* event);
  void bind_input();
  void bind_notifications();
  void bind_backdrop();
  void refresh_content();
  void refresh_list_content();
  void refresh_detail_content();
  void open_detail_for(std::string_view id);
  void close_detail();
  void refresh_backdrop();
  void apply_backdrop_time(const TimeModel& model);
  void apply_backdrop_battery(const BatteryModel& model);
  void set_open_preview_progress(lv_coord_t progress, bool animated);
  void set_close_drag_offset(lv_coord_t offset, bool animated);
  void stop_preview_close_timer();
  static void preview_close_timer_cb(lv_timer_t* timer);
  void finish_drag_close(std::int16_t release_delta, bool flick_close);
  bool is_handle_drag_start_zone(std::int16_t x, std::int16_t y) const;
  bool should_capture_shell_drag(const InputCommand& command) const;
  void show_clear_confirm_overlay();
  void hide_clear_confirm_overlay();
  void set_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t offset);
  void animate_notification_card_swipe_offset(lv_obj_t* card, lv_coord_t target_offset);
  void reset_notification_card_swipe_state();

  lv_obj_t* backdrop_root_ {nullptr};
  lv_obj_t* backdrop_battery_icon_label_ {nullptr};
  lv_obj_t* backdrop_battery_label_ {nullptr};
  lv_obj_t* backdrop_minute_label_ {nullptr};
  lv_obj_t* backdrop_style_stage_ {nullptr};
  lv_obj_t* list_root_ {nullptr};
  lv_obj_t* empty_state_ {nullptr};
  lv_obj_t* clear_button_ {nullptr};
  lv_obj_t* detail_root_ {nullptr};
  lv_obj_t* detail_back_button_ {nullptr};
  lv_obj_t* detail_source_label_ {nullptr};
  lv_obj_t* detail_title_label_ {nullptr};
  lv_obj_t* detail_body_label_ {nullptr};
  lv_obj_t* detail_time_label_ {nullptr};
  lv_obj_t* clear_confirm_overlay_ {nullptr};
  lv_obj_t* drag_handle_ {nullptr};
  lv_obj_t* sheet_container_ {nullptr};
  lv_coord_t shell_drag_offset_ {0};
  lv_coord_t open_preview_progress_ {0};
  lv_coord_t active_card_swipe_offset_ {0};
  bool shell_drag_active_ {false};
  bool detail_active_ {false};
  bool clear_confirm_active_ {false};
  bool card_swipe_horizontal_capture_ {false};
  bool card_swipe_vertical_lock_ {false};
  std::string detail_notification_id_;
  std::string active_card_swipe_notification_id_;
  std::vector<std::string> rendered_notification_ids_;
  lv_obj_t* active_card_swipe_ {nullptr};
  lv_obj_t* suppressed_card_click_target_ {nullptr};
  lv_point_t active_card_swipe_press_point_ {};
  lv_timer_t* preview_close_timer_ {nullptr};
  WatchfaceConfig backdrop_config_ {default_watchface_config()};
  WatchfaceRenderState backdrop_render_state_ {};
  std::unique_ptr<IWatchfaceStyleRenderer> backdrop_renderer_;
};

class NotificationWakePage : public PageBase {
 public:
  explicit NotificationWakePage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;
  void on_will_disappear() override;

 protected:
  lv_obj_t* build() override;

 private:
  static void dismiss_event_cb(lv_event_t* event);
  static void open_notifications_event_cb(lv_event_t* event);
  static void timeout_cb(lv_timer_t* timer);
  void bind_notifications();
  void refresh_content();
  void start_auto_close_timer();
  void stop_auto_close_timer();

  lv_obj_t* preview_card_ {nullptr};
  lv_obj_t* icon_container_ {nullptr};
  lv_obj_t* icon_image_ {nullptr};
  lv_obj_t* icon_label_ {nullptr};
  lv_obj_t* source_label_ {nullptr};
  lv_obj_t* title_label_ {nullptr};
  lv_obj_t* body_label_ {nullptr};
  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* dismiss_button_ {nullptr};
  std::string current_notification_id_;
  LvglTimerGuard auto_close_timer_;
};

}  // namespace twsim::app
