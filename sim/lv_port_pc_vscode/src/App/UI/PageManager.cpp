#include "App/UI/PageManager.h"

#include <algorithm>

namespace twsim::app {

void PageManager::register_page(PageId page_id, Factory factory) {
  factories_[page_id] = std::move(factory);
}

bool PageManager::set_root(PageId page_id, PageTransition transition) {
  temporary_page_.reset();
  stack_.clear();
  stack_.push_back(page_id);
  return load(page_id, transition);
}

bool PageManager::set_root_preserving_current(PageId page_id, PageTransition transition) {
  temporary_page_.reset();
  stack_.clear();
  stack_.push_back(page_id);
  return load(page_id, transition, false, true);
}

bool PageManager::push(PageId page_id, PageTransition transition) {
  if (std::find(stack_.begin(), stack_.end(), page_id) != stack_.end()) {
    return false;
  }

  temporary_page_.reset();
  stack_.push_back(page_id);
  return load(page_id, transition);
}

bool PageManager::pop(PageTransition transition) {
  if (stack_.size() <= 1) {
    return false;
  }

  temporary_page_.reset();
  stack_.pop_back();
  return load(stack_.back(), transition);
}

std::optional<PageId> PageManager::current_page_id() const {
  return visible_page_;
}

std::optional<PageId> PageManager::stack_top_page_id() const {
  if (stack_.empty()) {
    return std::nullopt;
  }
  return stack_.back();
}

std::optional<PageId> PageManager::temporary_page_id() const {
  return temporary_page_;
}

std::size_t PageManager::stack_depth() const {
  return stack_.size();
}

bool PageManager::show_temporary(PageId page_id, PageTransition transition) {
  if (stack_.empty()) {
    return false;
  }

  temporary_page_ = page_id;
  return load(page_id, transition);
}

bool PageManager::dismiss_temporary(PageTransition transition) {
  if (!temporary_page_ || stack_.empty()) {
    return false;
  }

  temporary_page_.reset();
  return load(stack_.back(), transition);
}

PageManager::State PageManager::capture_state() const {
  return State {stack_, temporary_page_};
}

bool PageManager::restore_state(const State& state, PageTransition transition) {
  if (state.stack.empty()) {
    return false;
  }

  stack_ = state.stack;
  temporary_page_ = state.temporary;
  const PageId target = temporary_page_.value_or(stack_.back());
  return load(target, transition);
}

bool PageManager::restore_state_preserving_target(const State& state, PageTransition transition) {
  if (state.stack.empty()) {
    return false;
  }

  stack_ = state.stack;
  temporary_page_ = state.temporary;
  const PageId target = temporary_page_.value_or(stack_.back());
  return load(target, transition, true, false);
}

PageBase* PageManager::acquire(PageId page_id) {
  const auto existing = pages_.find(page_id);
  if (existing != pages_.end()) {
    return existing->second.get();
  }

  const auto factory = factories_.find(page_id);
  if (factory == factories_.end()) {
    return nullptr;
  }

  auto page = factory->second();
  if (!page) {
    return nullptr;
  }

  PageBase* raw_page = page.get();
  pages_.emplace(page_id, std::move(page));
  return raw_page;
}

lv_screen_load_anim_t PageManager::to_lvgl_transition(PageTransition transition) {
  switch (transition) {
    case PageTransition::MoveLeft:
      return LV_SCREEN_LOAD_ANIM_MOVE_LEFT;
    case PageTransition::MoveRight:
      return LV_SCREEN_LOAD_ANIM_MOVE_RIGHT;
    case PageTransition::MoveUp:
      return LV_SCREEN_LOAD_ANIM_MOVE_TOP;
    case PageTransition::MoveDown:
      return LV_SCREEN_LOAD_ANIM_MOVE_BOTTOM;
    case PageTransition::Fade:
      return LV_SCREEN_LOAD_ANIM_FADE_IN;
    case PageTransition::None:
    default:
      return LV_SCREEN_LOAD_ANIM_NONE;
  }
}

bool PageManager::load(PageId page_id,
                       PageTransition transition,
                       bool notify_current_disappear,
                       bool notify_target_appear) {
  if (notify_current_disappear && visible_page_ && *visible_page_ != page_id) {
    if (auto* current_page = acquire(*visible_page_)) {
      current_page->on_will_disappear();
    }
  }

  PageBase* next_page = acquire(page_id);
  if (next_page == nullptr || !next_page->ensure_created()) {
    return false;
  }

  const lv_screen_load_anim_t anim = to_lvgl_transition(transition);
  if (notify_target_appear) {
    next_page->on_will_appear();
  }
  lv_screen_load_anim(next_page->root(), anim, anim == LV_SCREEN_LOAD_ANIM_NONE ? 0U : 220U, 0U, false);
  visible_page_ = page_id;
  return true;
}

}  // namespace twsim::app
