#include "App/UI/Pages/Shell/Launcher/ShellLauncherPages.h"

#include "App/UI/Pages/Shell/ShellAppIconPrimitives.h"
#include "App/UI/Pages/Shell/ShellAppVisualRegistry.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellCrownScrollHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;
using twsim::app::shell_crown::apply_stream_crown_drag;
using twsim::app::shell_crown::kLauncherCrownReleaseDelayMs;

namespace twsim::app {

namespace {

constexpr lv_coord_t kLauncherScrollTop = 10;
constexpr lv_coord_t kLauncherScrollBottom = 0;
constexpr lv_coord_t kLauncherScrollInset = 8;
constexpr lv_coord_t kLauncherScrollPadTop = 10;
constexpr lv_coord_t kLauncherScrollPadBottom = 12;
constexpr lv_coord_t kLauncherScrollGap = 10;
constexpr lv_coord_t kLauncherSectionPad = 10;
constexpr lv_coord_t kLauncherSectionGap = 8;
constexpr lv_coord_t kLauncherSectionHeaderBottom = 8;
constexpr lv_coord_t kLauncherTileWidth = 60;
constexpr lv_coord_t kLauncherTileHeight = 84;
constexpr lv_coord_t kLauncherMultiColumnTileHeight = 58;
constexpr lv_coord_t kLauncherIconSize = 52;
constexpr lv_coord_t kLauncherColumnGap = 8;
constexpr lv_coord_t kLauncherRowGap = 10;
constexpr lv_coord_t kLauncherMultiColumnElasticSpacerHeight = 40;
constexpr lv_coord_t kLauncherListRowHeight = 62;
constexpr lv_coord_t kLauncherListIconSize = 38;

LauncherLayoutMode current_launcher_layout_mode(DataCenter& data_center) {
  return data_center.display_policy().value_or(DisplayPolicyModel {}).launcher_layout_mode;
}

lv_obj_t* create_launcher_scroll_root(lv_obj_t* root, lv_coord_t screen_w, lv_coord_t screen_h) {
  lv_obj_t* scroll = lv_obj_create(root);
  if (scroll == nullptr) {
    return nullptr;
  }

  lv_obj_set_size(scroll, screen_w - kLauncherScrollInset * 2, screen_h - kLauncherScrollTop - kLauncherScrollBottom);
  lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, kLauncherScrollTop);
  lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll, 0, 0);
  lv_obj_set_style_radius(scroll, 0, 0);
  return scroll;
}

}  // namespace

LauncherPage::LauncherPage(DataCenter& data_center) : PageBase(data_center) {
  struct LauncherEntrySpec {
    const char* section_title;
    PageId target;
  };

  const std::array<LauncherEntrySpec, 11> launcher_targets {{
      {"System", PageId::SettingsHome},
      {"Daily", PageId::AppWeather},
      {"Daily", PageId::Pedometer},
      {"Daily", PageId::AppSleep},
      {"Health", PageId::AppHeartRate},
      {"Health", PageId::AppBloodOxygen},
      {"Health", PageId::AppStress},
      {"Health", PageId::AppBreathing},
      {"Wallet", PageId::AppNfc},
      {"Wallet", PageId::AppAlipay},
      {"Wallet", PageId::AppWeChatPay},
  }};

  items_.reserve(launcher_targets.size());
  for (const auto& entry : launcher_targets) {
    const AppVisualSpec* spec = find_app_visual_spec(entry.target);
    if (spec == nullptr) {
      continue;
    }
    items_.push_back({entry.section_title,
                      spec->label,
                      {NavigationAction::LaunchApp, entry.target},
                      spec->icon_text,
                      spec->icon_asset_path,
                      lv_color_hex(spec->icon_bg),
                      lv_color_hex(spec->icon_fg)});
  }
}

PageId LauncherPage::id() const {
  return PageId::Launcher;
}

const char* LauncherPage::name() const {
  return "Launcher";
}

void LauncherPage::on_will_appear() {
  rebuild_layout_if_needed();
  if (list_root_ != nullptr) {
    lv_obj_update_layout(list_root_);
    lv_obj_scroll_to_y(list_root_, 0, LV_ANIM_OFF);
  }
}

lv_obj_t* LauncherPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  rebuild_layout_if_needed(true);

  bind_input();
  return root;
}

void LauncherPage::rebuild_layout_if_needed(bool force) {
  const LauncherLayoutMode target_mode = current_launcher_layout_mode(data_center_);
  if (!force && list_root_ != nullptr && current_layout_mode_ == target_mode) {
    return;
  }

  current_layout_mode_ = target_mode;
  stop_crown_release_timer();

  if (list_root_ != nullptr) {
    lv_obj_delete(list_root_);
    list_root_ = nullptr;
  }
  if (root_ == nullptr) {
    return;
  }

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  list_root_ = create_launcher_scroll_root(root_, screen_w, screen_h);
  if (list_root_ == nullptr) {
    return;
  }

  switch (current_layout_mode_) {
    case LauncherLayoutMode::MultiColumn:
      build_multi_column_layout(list_root_);
      break;
    case LauncherLayoutMode::List:
      build_list_layout(list_root_);
      break;
    case LauncherLayoutMode::Categorized:
    default:
      build_categorized_layout(list_root_);
      break;
  }
}

void LauncherPage::build_multi_column_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 12, 0);
  lv_obj_set_style_pad_right(parent, 12, 0);
  lv_obj_set_style_pad_row(parent, kLauncherRowGap, 0);
  lv_obj_set_style_pad_column(parent, kLauncherColumnGap, 0);

  for (std::size_t index = 0; index < items_.size(); ++index) {
    const Item& item = items_[index];
    lv_obj_t* tile = lv_button_create(parent);
    if (tile == nullptr) {
      return;
    }
    ui_prepare_box(tile);
    ui_set_touch_target(tile, 8);
    lv_obj_set_size(tile, kLauncherTileWidth, kLauncherMultiColumnTileHeight);
    lv_obj_set_style_bg_opa(tile, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(tile, 0, 0);
    lv_obj_set_style_shadow_width(tile, 0, 0);
    lv_obj_set_style_pad_all(tile, 0, 0);
    lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
    attach_click_guard(tile);
    lv_obj_add_event_cb(tile, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(tile, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    const AppVisualSpec render_spec {
        item.command.target,
        item.label,
        item.icon_text,
        item.icon_asset,
        lv_color_to_u32(item.icon_bg),
        lv_color_to_u32(item.icon_fg),
    };
    lv_obj_t* icon = create_app_round_icon(tile, render_spec, kLauncherIconSize);
    lv_obj_t* label = lv_label_create(tile);
    if (icon == nullptr || label == nullptr) {
      return;
    }
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 0);
    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Tiny);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
  }

  lv_obj_t* elastic_spacer = lv_obj_create(parent);
  if (elastic_spacer == nullptr) {
    return;
  }
  lv_obj_set_width(elastic_spacer, LV_PCT(100));
  lv_obj_set_height(elastic_spacer, kLauncherMultiColumnElasticSpacerHeight);
  lv_obj_set_style_bg_opa(elastic_spacer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(elastic_spacer, 0, 0);
  lv_obj_set_style_pad_all(elastic_spacer, 0, 0);
  lv_obj_remove_flag(elastic_spacer, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_remove_flag(elastic_spacer, LV_OBJ_FLAG_SCROLLABLE);
}

void LauncherPage::build_list_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 0, 0);
  lv_obj_set_style_pad_right(parent, 0, 0);
  lv_obj_set_style_pad_row(parent, kLauncherScrollGap, 0);

  for (std::size_t index = 0; index < items_.size(); ++index) {
    const Item& item = items_[index];
    lv_obj_t* row = lv_button_create(parent);
    if (row == nullptr) {
      return;
    }
    ui_prepare_box(row);
    ui_apply_surface(row, SurfaceStyle::PanelSubtle);
    ui_set_touch_target(row, 8);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, kLauncherListRowHeight);
    lv_obj_set_style_bg_color(row, lv_color_hex(0x0C131D), 0);
    lv_obj_set_style_border_color(row, lv_color_hex(0x1A2635), 0);
    lv_obj_set_style_radius(row, 22, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    attach_click_guard(row);
    lv_obj_add_event_cb(row, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(row, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

    const AppVisualSpec render_spec {
        item.command.target,
        item.label,
        item.icon_text,
        item.icon_asset,
        lv_color_to_u32(item.icon_bg),
        lv_color_to_u32(item.icon_fg),
    };
    lv_obj_t* icon = create_app_round_icon(row, render_spec, kLauncherListIconSize);
    lv_obj_t* label = lv_label_create(row);
    if (icon == nullptr || label == nullptr) {
      return;
    }
    lv_obj_align(icon, LV_ALIGN_LEFT_MID, 14, 0);
    ui_prepare_label(label);
    ui_apply_text(label, TextStyle::Title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF7FBFF), 0);
    lv_obj_set_width(label, 136);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_label_set_text(label, item.label);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 66, 0);
  }
}

void LauncherPage::build_categorized_layout(lv_obj_t* parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_top(parent, kLauncherScrollPadTop, 0);
  lv_obj_set_style_pad_bottom(parent, kLauncherScrollPadBottom, 0);
  lv_obj_set_style_pad_left(parent, 0, 0);
  lv_obj_set_style_pad_right(parent, 0, 0);
  lv_obj_set_style_pad_row(parent, kLauncherScrollGap, 0);
  lv_obj_set_style_pad_column(parent, 0, 0);

  std::size_t index = 0;
  while (index < items_.size()) {
    const char* section_title = items_[index].section_title;
    lv_obj_t* section = lv_obj_create(parent);
    lv_obj_t* section_title_label = lv_label_create(section);
    lv_obj_t* section_grid = lv_obj_create(section);
    if (section == nullptr || section_title_label == nullptr || section_grid == nullptr) {
      return;
    }

    ui_prepare_box(section);
    ui_apply_surface(section, SurfaceStyle::PanelSubtle);
    lv_obj_set_width(section, LV_PCT(100));
    lv_obj_set_layout(section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(section, kLauncherSectionPad, 0);
    lv_obj_set_style_pad_gap(section, kLauncherSectionGap, 0);
    lv_obj_set_style_bg_color(section, lv_color_hex(0x0C131D), 0);
    lv_obj_set_style_border_color(section, lv_color_hex(0x1A2635), 0);
    lv_obj_set_style_radius(section, 22, 0);
    lv_obj_remove_flag(section, LV_OBJ_FLAG_SCROLLABLE);

    ui_prepare_label(section_title_label);
    ui_apply_text(section_title_label, TextStyle::Eyebrow);
    lv_label_set_text(section_title_label, section_title);
    lv_obj_set_style_pad_bottom(section_title_label, kLauncherSectionHeaderBottom, 0);

    ui_prepare_box(section_grid);
    lv_obj_set_width(section_grid, LV_PCT(100));
    lv_obj_set_layout(section_grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(section_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(section_grid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(section_grid, 0, 0);
    lv_obj_set_style_pad_row(section_grid, kLauncherRowGap, 0);
    lv_obj_set_style_pad_column(section_grid, kLauncherColumnGap, 0);
    lv_obj_set_style_bg_opa(section_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(section_grid, 0, 0);
    lv_obj_remove_flag(section_grid, LV_OBJ_FLAG_SCROLLABLE);

    std::size_t section_count = 0;
    while (index < items_.size() && std::strcmp(items_[index].section_title, section_title) == 0) {
      const Item& item = items_[index];
      lv_obj_t* tile = lv_button_create(section_grid);
      if (tile == nullptr) {
        return;
      }
      ui_prepare_box(tile);
      ui_set_touch_target(tile, 8);
      lv_obj_set_size(tile, kLauncherTileWidth, kLauncherTileHeight);
      lv_obj_set_style_bg_opa(tile, LV_OPA_TRANSP, 0);
      lv_obj_set_style_border_width(tile, 0, 0);
      lv_obj_set_style_shadow_width(tile, 0, 0);
      lv_obj_set_style_pad_all(tile, 0, 0);
      lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
      attach_click_guard(tile);
      lv_obj_add_event_cb(tile, &LauncherPage::item_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(tile, reinterpret_cast<void*>(static_cast<std::uintptr_t>(index)));

      const AppVisualSpec render_spec {
          item.command.target,
          item.label,
          item.icon_text,
          item.icon_asset,
          lv_color_to_u32(item.icon_bg),
          lv_color_to_u32(item.icon_fg),
      };
      lv_obj_t* icon = create_app_round_icon(tile, render_spec, kLauncherIconSize);
      lv_obj_t* label = lv_label_create(tile);
      if (icon == nullptr || label == nullptr) {
        return;
      }
      lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 0);
      ui_prepare_label(label);
      ui_apply_text(label, TextStyle::Tiny);
      lv_obj_set_width(label, kLauncherTileWidth);
      lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_set_style_text_color(label, lv_color_hex(0xDCE6F4), 0);
      lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
      lv_label_set_text(label, item.label);
      lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

      ++index;
      ++section_count;
    }

    const std::size_t remainder = section_count % 3U;
    if (remainder != 0U) {
      const std::size_t filler_count = 3U - remainder;
      for (std::size_t filler_index = 0; filler_index < filler_count; ++filler_index) {
        lv_obj_t* filler = lv_obj_create(section_grid);
        if (filler == nullptr) {
          return;
        }
        lv_obj_set_size(filler, kLauncherTileWidth, kLauncherTileHeight);
        lv_obj_set_style_bg_opa(filler, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(filler, 0, 0);
        lv_obj_set_style_pad_all(filler, 0, 0);
        lv_obj_remove_flag(filler, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(filler, LV_OBJ_FLAG_SCROLLABLE);
      }
    }
  }
}

void LauncherPage::back_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  self->request_navigation({NavigationAction::ReturnHome, PageId::Watchface});
}

void LauncherPage::item_event_cb(lv_event_t* event) {
  auto* self = static_cast<LauncherPage*>(lv_event_get_user_data(event));
  if (self == nullptr || self->should_ignore_click()) {
    return;
  }

  lv_obj_t* target = lv_event_get_current_target_obj(event);
  if (target == nullptr || !click_guard_allows(target)) {
    return;
  }

  const auto item_index = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target)));
  if (item_index >= self->items_.size()) {
    return;
  }

  self->request_navigation(self->items_[item_index].command);
}

void LauncherPage::apply_crown_drag(bool forward, std::int16_t detents) {
  if (list_root_ == nullptr) {
    return;
  }

  stop_crown_release_timer();
  apply_stream_crown_drag(list_root_, forward, detents);
  schedule_crown_release();
}

void LauncherPage::schedule_crown_release() {
  stop_crown_release_timer();
  crown_release_timer_.reset(lv_timer_create(&LauncherPage::crown_release_timer_cb, kLauncherCrownReleaseDelayMs, this));
  if (crown_release_timer_.get() != nullptr) {
    lv_timer_set_repeat_count(crown_release_timer_.get(), 1);
  }
}

void LauncherPage::stop_crown_release_timer() {
  if (!crown_release_timer_) {
    return;
  }
  crown_release_timer_.reset();
}

void LauncherPage::crown_release_timer_cb(lv_timer_t* timer) {
  auto* self = static_cast<LauncherPage*>(lv_timer_get_user_data(timer));
  if (self == nullptr || self->list_root_ == nullptr) {
    return;
  }

  self->crown_release_timer_.release();
  const std::int32_t current_y = lv_obj_get_scroll_y(self->list_root_);
  const std::int32_t scroll_max =
      std::max<std::int32_t>(0, lv_obj_get_scroll_top(self->list_root_) + lv_obj_get_scroll_bottom(self->list_root_));
  const std::int32_t clamped_y = std::clamp(current_y, 0, scroll_max);
  lv_obj_scroll_to_y(self->list_root_, clamped_y, LV_ANIM_ON);
}

void LauncherPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || list_root_ == nullptr) {
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
