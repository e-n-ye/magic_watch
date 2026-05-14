#pragma once

#include <array>
#include <chrono>

#include "App/UI/PageBase.h"

namespace twsim::app {

class StopwatchPage : public PageBase {
 public:
  explicit StopwatchPage(DataCenter& data_center);
  ~StopwatchPage() override;

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void action_event_cb(lv_event_t* event);
  static void timer_cb(lv_timer_t* timer);

  void update_display() const;

  lv_obj_t* time_label_ {nullptr};
  lv_timer_t* timer_ {nullptr};
  bool running_ {false};
  std::chrono::steady_clock::time_point started_at_ {};
  std::chrono::milliseconds elapsed_before_pause_ {0};
};

class TimerPage : public PageBase {
 public:
  explicit TimerPage(DataCenter& data_center);
  ~TimerPage() override;

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void action_event_cb(lv_event_t* event);
  static void timer_cb(lv_timer_t* timer);

  void update_display();

  lv_obj_t* time_label_ {nullptr};
  lv_obj_t* state_label_ {nullptr};
  lv_timer_t* timer_ {nullptr};
  bool running_ {false};
  std::chrono::steady_clock::time_point started_at_ {};
  std::chrono::seconds configured_ {std::chrono::minutes(5)};
  std::chrono::seconds remaining_before_start_ {std::chrono::minutes(5)};
};

class AlarmPage : public PageBase {
 public:
  explicit AlarmPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);
  static void weekday_event_cb(lv_event_t* event);
  static void adjust_event_cb(lv_event_t* event);
  static void arm_event_cb(lv_event_t* event);

  void update_display();

  std::array<bool, 7> weekdays_ {{false, false, false, false, false, false, false}};
  int hour_ {7};
  int minute_ {30};
  bool armed_ {false};
  lv_obj_t* summary_label_ {nullptr};
};

}  // namespace twsim::app
