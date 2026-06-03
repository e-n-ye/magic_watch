#pragma once

#include <cstdint>
#include <vector>

#include "App/UI/PageBase.h"

namespace twsim::app {

class LauncherPage : public PageBase {
 public:
  explicit LauncherPage(DataCenter& data_center);

  PageId id() const override;
  const char* name() const override;
  void on_will_appear() override;

 protected:
  lv_obj_t* build() override;

 private:
  struct Item {
    const char* section_title;
    const char* label;
    NavigationCommand command;
    const char* icon_text;
    const char* icon_asset;
    lv_color_t icon_bg;
    lv_color_t icon_fg;
  };

  static void back_event_cb(lv_event_t* event);
  static void item_event_cb(lv_event_t* event);
  void rebuild_layout_if_needed(bool force = false);
  void build_multi_column_layout(lv_obj_t* parent);
  void build_list_layout(lv_obj_t* parent);
  void build_categorized_layout(lv_obj_t* parent);
  void apply_crown_drag(bool forward, std::int16_t detents);
  void schedule_crown_release();
  void stop_crown_release_timer();
  static void crown_release_timer_cb(lv_timer_t* timer);
  void bind_input();
  lv_obj_t* list_root_ {nullptr};
  std::vector<Item> items_;
  LauncherLayoutMode current_layout_mode_ {LauncherLayoutMode::Categorized};
  lv_timer_t* crown_release_timer_ {nullptr};
};

}  // namespace twsim::app
