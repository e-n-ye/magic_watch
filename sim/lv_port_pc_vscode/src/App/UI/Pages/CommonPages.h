#pragma once

#include <vector>

#include "App/UI/PageBase.h"

namespace twsim::app {

class MenuPage : public PageBase {
 public:
  struct Item {
    const char* label;
    NavigationCommand command;
    const char* detail;
  };

  MenuPage(DataCenter& data_center,
           PageId page_id,
           const char* title,
           const char* subtitle,
           std::vector<Item> items,
           bool show_back_button = true,
           bool enable_input_prototype = false);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void item_event_cb(lv_event_t* event);
  static void back_event_cb(lv_event_t* event);
  void bind_input_prototype();
  void apply_interaction_hint(const char* text);

  PageId page_id_;
  const char* title_;
  const char* subtitle_;
  std::vector<Item> items_;
  bool show_back_button_ {true};
  bool enable_input_prototype_ {false};
  lv_obj_t* list_ {nullptr};
  lv_obj_t* interaction_hint_label_ {nullptr};
};

class PlaceholderPage : public PageBase {
 public:
  PlaceholderPage(DataCenter& data_center,
                  PageId page_id,
                  const char* title,
                  const char* detail,
                  bool show_back_button = true);

  PageId id() const override;
  const char* name() const override;

 protected:
  lv_obj_t* build() override;

 private:
  static void back_event_cb(lv_event_t* event);

  PageId page_id_;
  const char* title_;
  const char* detail_;
  bool show_back_button_ {true};
};

}  // namespace twsim::app
