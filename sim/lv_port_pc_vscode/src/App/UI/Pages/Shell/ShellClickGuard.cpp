#include "App/UI/Pages/Shell/ShellClickGuard.h"

#include <unordered_map>

namespace twsim::app::shell_click_guard {

namespace {

constexpr lv_coord_t kClickDragThreshold = 12;

struct ClickGestureState {
  lv_point_t press_point {0, 0};
  bool moved {false};
};

std::unordered_map<lv_obj_t*, ClickGestureState>& click_gesture_states() {
  static std::unordered_map<lv_obj_t*, ClickGestureState> states;
  return states;
}

void click_guard_event_cb(lv_event_t* event) {
  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr) {
    return;
  }

  auto& states = click_gesture_states();
  switch (lv_event_get_code(event)) {
    case LV_EVENT_PRESSED: {
      lv_point_t point {0, 0};
      if (lv_indev_t* indev = lv_event_get_indev(event)) {
        lv_indev_get_point(indev, &point);
      }
      states[target] = {point, false};
      break;
    }
    case LV_EVENT_PRESSING: {
      auto it = states.find(target);
      if (it == states.end()) {
        break;
      }
      lv_point_t point {0, 0};
      if (lv_indev_t* indev = lv_event_get_indev(event)) {
        lv_indev_get_point(indev, &point);
      }
      const lv_coord_t dx = point.x - it->second.press_point.x;
      const lv_coord_t dy = point.y - it->second.press_point.y;
      if (LV_ABS(dx) >= kClickDragThreshold || LV_ABS(dy) >= kClickDragThreshold) {
        it->second.moved = true;
      }
      break;
    }
    case LV_EVENT_PRESS_LOST: {
      auto it = states.find(target);
      if (it != states.end()) {
        it->second.moved = true;
      }
      break;
    }
    case LV_EVENT_DELETE:
      states.erase(target);
      break;
    default:
      break;
  }
}

}  // namespace

void attach_click_guard(lv_obj_t* object) {
  if (object == nullptr) {
    return;
  }
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESSED, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESSING, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_PRESS_LOST, nullptr);
  lv_obj_add_event_cb(object, click_guard_event_cb, LV_EVENT_DELETE, nullptr);
}

bool click_guard_allows(lv_obj_t* object) {
  if (object == nullptr) {
    return false;
  }

  auto& states = click_gesture_states();
  const auto it = states.find(object);
  if (it == states.end()) {
    return true;
  }

  const bool allows = !it->second.moved;
  states.erase(it);
  return allows;
}

}  // namespace twsim::app::shell_click_guard
