#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "App/Common/EnumClassHash.h"
#include "App/UI/PageBase.h"

namespace twsim::app {

enum class PageTransition {
  None,
  MoveLeft,
  MoveRight,
  MoveUp,
  MoveDown,
  Fade,
};

class PageManager {
 public:
  using Factory = std::function<std::unique_ptr<PageBase>()>;
  struct State {
    std::vector<PageId> stack;
    std::optional<PageId> temporary;
  };

  void register_page(PageId page_id, Factory factory);

  bool set_root(PageId page_id, PageTransition transition = PageTransition::None);
  bool push(PageId page_id, PageTransition transition = PageTransition::MoveLeft);
  bool pop(PageTransition transition = PageTransition::MoveRight);
  bool show_temporary(PageId page_id, PageTransition transition = PageTransition::Fade);
  bool dismiss_temporary(PageTransition transition = PageTransition::Fade);
  State capture_state() const;
  bool restore_state(const State& state, PageTransition transition = PageTransition::Fade);
  bool set_root_preserving_current(PageId page_id, PageTransition transition = PageTransition::None);
  bool restore_state_preserving_target(const State& state, PageTransition transition = PageTransition::Fade);

  std::optional<PageId> current_page_id() const;
  std::optional<PageId> stack_top_page_id() const;
  std::optional<PageId> temporary_page_id() const;
  std::size_t stack_depth() const;

 private:
  PageBase* acquire(PageId page_id);
  static lv_screen_load_anim_t to_lvgl_transition(PageTransition transition);
  bool load(PageId page_id,
            PageTransition transition,
            bool notify_current_disappear = true,
            bool notify_target_appear = true);

  std::unordered_map<PageId, Factory, EnumClassHash> factories_;
  std::unordered_map<PageId, std::unique_ptr<PageBase>, EnumClassHash> pages_;
  std::vector<PageId> stack_;
  std::optional<PageId> temporary_page_;
  std::optional<PageId> visible_page_;
};

}  // namespace twsim::app
