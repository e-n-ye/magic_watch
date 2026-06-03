#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Daily/StepsMainPrimitives.h"
#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellClickGuard.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/Home/ShellHomeDailyCardPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomeHealthCardPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomeLayoutPrimitives.h"
#include "App/UI/Pages/Shell/Home/ShellHomePaymentCardPrimitives.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <utility>

using twsim::app::shell_click_guard::attach_click_guard;
using twsim::app::shell_click_guard::click_guard_allows;

namespace twsim::app {

namespace {

using shell_asset::file_exists;
using shell_asset::nfc_school_card_asset_path;
using shell_asset::nfc_school_card_inner_asset_path;
using shell_asset::payment_wechat_asset_path;
using shell_asset::payment_wechat_green_asset_path;

constexpr lv_coord_t kHomePagerStep = 15;

std::size_t decimal_digits(std::uint32_t value) {
  std::size_t digits = 1;
  while (value >= 10U) {
    value /= 10U;
    ++digits;
  }
  return digits;
}

const lv_font_t* home_shortcut_steps_font(std::uint32_t value) {
  return decimal_digits(value) >= 5 ? &lv_font_montserrat_12 : &lv_font_montserrat_14;
}

}  // namespace

HomeRingHostPage::HomeRingHostPage(DataCenter& data_center) : PageBase(data_center) {}

PageId HomeRingHostPage::id() const {
  return PageId::HomeRingHost;
}

const char* HomeRingHostPage::name() const {
  return "HomeRingHost";
}

void HomeRingHostPage::on_will_appear() {
  if (data_center_.time()) {
    apply_time(*data_center_.time());
  }
  if (data_center_.battery()) {
    apply_battery(*data_center_.battery());
  }
  if (data_center_.home_ring_preview()) {
    apply_home_ring_preview(*data_center_.home_ring_preview());
  } else {
    layout_surfaces_for_preview(static_cast<std::uint8_t>(settled_surface_index_), 0);
    set_track_x(static_cast<lv_coord_t>(-240 * static_cast<int>(settled_surface_index_)));
  }
  if (data_center_.steps()) {
    apply_steps(*data_center_.steps());
  }
}

void HomeRingHostPage::apply_time(const TimeModel& model) {
  if (minute_label_ == nullptr || renderer_ == nullptr) {
    return;
  }

  if (!model.valid) {
    lv_label_set_text(minute_label_, "--");
    render_state_.hour_text = "--";
    render_state_.minute_text = "--";
    render_state_.spread_index = config_.spread_index;
    renderer_->apply(render_state_);
    return;
  }

  char hour_buffer[4] = {};
  char minute_buffer[4] = {};
  unsigned display_hour = static_cast<unsigned>(model.hour % 12);
  if (display_hour == 0U) {
    display_hour = 12U;
  }
  std::snprintf(hour_buffer, sizeof(hour_buffer), "%u", display_hour);
  std::snprintf(minute_buffer, sizeof(minute_buffer), "%02u", static_cast<unsigned>(model.minute));
  lv_label_set_text(minute_label_, minute_buffer);
  render_state_.hour_text = hour_buffer;
  render_state_.minute_text = minute_buffer;
  render_state_.spread_index = config_.spread_index;
  renderer_->apply(render_state_);
}

void HomeRingHostPage::apply_battery(const BatteryModel& model) {
  if (battery_label_ == nullptr || renderer_ == nullptr) {
    return;
  }

  char buffer[32] = {};
  std::snprintf(buffer, sizeof(buffer), "%d%%%s", static_cast<int>(model.percent), model.charging ? " +" : "");
  lv_label_set_text(battery_label_, buffer);
  render_state_.battery_percent = model.percent;
  render_state_.spread_index = config_.spread_index;
  renderer_->apply(render_state_);
}

void HomeRingHostPage::apply_steps(const StepsModel& model) {
  steps_model_ = model;
  if (weather_steps_value_label_ == nullptr) {
    return;
  }

  if (!steps_model_.valid) {
    set_content_width_label(weather_steps_value_label_, "--", &lv_font_montserrat_14, 0xFFFFFF);
    lv_obj_align(weather_steps_value_label_, LV_ALIGN_TOP_LEFT, 2, 70);
    return;
  }

  char value_text[16] = {};
  std::snprintf(value_text, sizeof(value_text), "%lu", static_cast<unsigned long>(steps_model_.daily_steps));
  set_content_width_label(weather_steps_value_label_,
                          value_text,
                          home_shortcut_steps_font(steps_model_.daily_steps),
                          0xFFFFFF);
  lv_obj_align(weather_steps_value_label_, LV_ALIGN_TOP_LEFT, 2, 70);
}

std::size_t HomeRingHostPage::wrap_surface_index(int index) const {
  const int size = static_cast<int>(surfaces_.size());
  int wrapped = index % size;
  if (wrapped < 0) {
    wrapped += size;
  }
  return static_cast<std::size_t>(wrapped);
}

void HomeRingHostPage::set_track_x(lv_coord_t x) {
  if (track_ == nullptr) {
    return;
  }
  lv_obj_set_x(track_, x);
}

void HomeRingHostPage::layout_surfaces_for_preview(std::uint8_t base_index, std::int8_t direction) {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const bool wrap_to_weather = base_index == 0 && direction < 0;
  const bool wrap_to_watchface = base_index == surfaces_.size() - 1 && direction > 0;

  if (wrap_to_weather || wrap_to_watchface) {
    for (std::size_t index = 0; index < surfaces_.size(); ++index) {
      if (surfaces_[index] == nullptr) {
        continue;
      }
      lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>((index + 2) * screen_w), 0);
    }

    if (surfaces_[0] != nullptr) {
      lv_obj_set_pos(surfaces_[0], screen_w, 0);
    }
    if (surfaces_.back() != nullptr) {
      lv_obj_set_pos(surfaces_.back(), 0, 0);
    }
    return;
  }

  for (std::size_t index = 0; index < surfaces_.size(); ++index) {
    if (surfaces_[index] == nullptr) {
      continue;
    }
    lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>(index * screen_w), 0);
  }
}

void HomeRingHostPage::apply_home_ring_preview(const HomeRingPreviewModel& model) {
  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t base_x = static_cast<lv_coord_t>(-screen_w * static_cast<int>(model.base_index));
  const lv_coord_t clamped_progress = clamp_coord(model.progress, 0, screen_w);
  auto update_pager = [&](float shortcut_position, bool visible) {
    if (pager_root_ == nullptr || pager_active_dot_ == nullptr) {
      return;
    }
    if (!visible) {
      lv_obj_add_flag(pager_root_, LV_OBJ_FLAG_HIDDEN);
      return;
    }

    lv_obj_clear_flag(pager_root_, LV_OBJ_FLAG_HIDDEN);
    const float clamped = std::clamp(shortcut_position, 0.0f, 3.0f);
    const lv_coord_t offset = static_cast<lv_coord_t>(std::lround(clamped * static_cast<float>(kHomePagerStep)));
    lv_obj_set_x(pager_active_dot_, offset);
  };

  if (model.active && model.direction != 0) {
    layout_surfaces_for_preview(model.base_index, model.direction);
    const bool touches_watchface = (model.base_index == 0 && model.direction < 0) ||
                                   (model.base_index == static_cast<std::uint8_t>(surfaces_.size() - 1) &&
                                    model.direction > 0);
    if (touches_watchface) {
      const lv_coord_t offset = model.base_index == 0
                                    ? static_cast<lv_coord_t>(clamped_progress - screen_w)
                                    : static_cast<lv_coord_t>(-clamped_progress);
      set_track_x(offset);
    } else {
      const lv_coord_t offset = model.direction > 0 ? static_cast<lv_coord_t>(-clamped_progress)
                                                    : static_cast<lv_coord_t>(clamped_progress);
      set_track_x(static_cast<lv_coord_t>(base_x + offset));
    }

    if (touches_watchface) {
      update_pager(0.0f, false);
    } else if (model.base_index > 0) {
      const float ratio = static_cast<float>(clamped_progress) / static_cast<float>(screen_w);
      const float base_shortcut = static_cast<float>(model.base_index - 1);
      const float delta = model.direction > 0 ? ratio : -ratio;
      update_pager(base_shortcut + delta, true);
    } else {
      update_pager(0.0f, false);
    }
    return;
  }

  if (model.commit && model.direction != 0) {
    settled_surface_index_ = wrap_surface_index(static_cast<int>(model.base_index) + model.direction);
  } else {
    settled_surface_index_ = wrap_surface_index(model.base_index);
  }

  layout_surfaces_for_preview(static_cast<std::uint8_t>(settled_surface_index_), 0);
  set_track_x(static_cast<lv_coord_t>(-screen_w * static_cast<int>(settled_surface_index_)));
  if (settled_surface_index_ == 0) {
    update_pager(0.0f, false);
  } else {
    update_pager(static_cast<float>(settled_surface_index_ - 1), true);
  }
}

lv_obj_t* HomeRingHostPage::build() {
  const auto app_tile_event_cb = [](lv_event_t* event) {
    auto* self = static_cast<HomeRingHostPage*>(lv_event_get_user_data(event));
    if (self == nullptr || self->should_ignore_click()) {
      return;
    }
    lv_obj_t* target = lv_event_get_current_target_obj(event);
    if (target == nullptr || !click_guard_allows(target)) {
      return;
    }
    const auto raw = reinterpret_cast<std::uintptr_t>(lv_obj_get_user_data(target));
    self->request_navigation({NavigationAction::Push, static_cast<PageId>(raw)});
  };

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const auto layout = shell_home::make_surface_layout();

  track_ = lv_obj_create(root);
  if (track_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(track_);
  lv_obj_set_size(track_, static_cast<lv_coord_t>(screen_w * static_cast<int>(surfaces_.size())), screen_h);
  lv_obj_align(track_, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_opa(track_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(track_, 0, 0);
  lv_obj_set_style_pad_all(track_, 0, 0);
  lv_obj_remove_flag(track_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(track_, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

  for (std::size_t index = 0; index < surfaces_.size(); ++index) {
    surfaces_[index] = lv_obj_create(track_);
    if (surfaces_[index] == nullptr) {
      return nullptr;
    }
    ui_prepare_box(surfaces_[index]);
    ui_apply_surface(surfaces_[index], SurfaceStyle::Screen);
    lv_obj_set_size(surfaces_[index], screen_w, screen_h);
    lv_obj_set_pos(surfaces_[index], static_cast<lv_coord_t>(index * screen_w), 0);
    lv_obj_remove_flag(surfaces_[index], LV_OBJ_FLAG_SCROLLABLE);
  }

  pager_root_ = lv_obj_create(root);
  if (pager_root_ == nullptr) {
    return nullptr;
  }
  shell_home::style_pager_root(pager_root_, layout);

  for (std::size_t index = 0; index < pager_dots_.size(); ++index) {
    pager_dots_[index] = shell_home::create_pager_dot(pager_root_, lv_color_hex(0xB9C5D5), LV_OPA_10, false);
    if (pager_dots_[index] == nullptr) {
      return nullptr;
    }
    lv_obj_add_flag(pager_dots_[index], LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_remove_flag(pager_dots_[index], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(pager_dots_[index], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(pager_dots_[index], static_cast<lv_coord_t>(index * kHomePagerStep + 1), 3);
  }

  pager_active_dot_ = lv_obj_create(pager_root_);
  if (pager_active_dot_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(pager_active_dot_);
  lv_obj_set_size(pager_active_dot_, 9, 9);
  lv_obj_add_flag(pager_active_dot_, LV_OBJ_FLAG_IGNORE_LAYOUT);
  lv_obj_remove_flag(pager_active_dot_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(pager_active_dot_, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_radius(pager_active_dot_, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(pager_active_dot_, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_bg_opa(pager_active_dot_, LV_OPA_70, 0);
  lv_obj_set_style_border_width(pager_active_dot_, 0, 0);
  lv_obj_set_style_pad_all(pager_active_dot_, 0, 0);
  lv_obj_set_pos(pager_active_dot_, 0, 2);

  {
    lv_obj_t* surface = surfaces_[0];
    lv_obj_t* overlay = lv_obj_create(surface);
    lv_obj_t* battery_row = lv_obj_create(overlay);
    battery_icon_label_ = lv_label_create(battery_row);
    battery_label_ = lv_label_create(battery_row);
    style_stage_ = lv_obj_create(overlay);
    minute_label_ = lv_label_create(overlay);

    if (overlay == nullptr || battery_row == nullptr || battery_icon_label_ == nullptr || battery_label_ == nullptr ||
        style_stage_ == nullptr || minute_label_ == nullptr) {
      return nullptr;
    }

    ui_prepare_box(overlay);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_center(overlay);

    ui_prepare_box(battery_row);
    ui_set_flex_row(battery_row, 0, 4, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(battery_row, 53, 18);
    lv_obj_set_style_bg_opa(battery_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_row, 0, 0);
    lv_obj_align(battery_row, LV_ALIGN_TOP_MID, 0, 10);

    ui_prepare_label(battery_icon_label_);
    ui_apply_text(battery_icon_label_, TextStyle::Tiny);
    lv_obj_set_style_text_font(battery_icon_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(battery_icon_label_, lv_color_hex(0xF5F7FB), 0);
    lv_label_set_text(battery_icon_label_, LV_SYMBOL_CHARGE);

    ui_prepare_label(battery_label_);
    ui_apply_text(battery_label_, TextStyle::Tiny);
    lv_obj_set_style_text_font(battery_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(battery_label_, lv_color_hex(0xF5F7FB), 0);
    lv_label_set_text(battery_label_, "--%");

    ui_prepare_box(style_stage_);
    lv_obj_set_size(style_stage_, 240, 296);
    lv_obj_align(style_stage_, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(style_stage_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(style_stage_, 0, 0);
    lv_obj_set_style_pad_all(style_stage_, 0, 0);

    renderer_ = create_watchface_style_renderer(config_);
    if (!renderer_ || renderer_->build(style_stage_) == nullptr) {
      return nullptr;
    }

    ui_prepare_label(minute_label_);
    ui_apply_text(minute_label_, TextStyle::HeroSoft);
    lv_obj_set_style_text_font(minute_label_, &lv_font_montserrat_42, 0);
    lv_obj_set_style_text_color(minute_label_, lv_color_hex(0xD7E3F4), 0);
    lv_obj_set_width(minute_label_, 96);
    lv_obj_set_style_text_align(minute_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(minute_label_, 76, 234);
    lv_label_set_text(minute_label_, "--");
  }

  {
    lv_obj_t* surface = surfaces_[1];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    const lv_color_t tile_bg = lv_color_hex(0x0C1424);
    const lv_color_t tile_border = lv_color_hex(0x1E375A);
    const lv_color_t music_bg = lv_color_hex(0x4BE7E8);
    const lv_color_t music_fg = lv_color_hex(0xF7FFFE);
    lv_obj_t* stage = shell_home::create_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
      return nullptr;
    }

    const lv_coord_t music_y = 136;
    shell_home_payment::PaymentCardsView payment_cards {};
    lv_obj_t* music_card = lv_obj_create(stage);
    if (music_card == nullptr) {
      return nullptr;
    }

    const char* wechat_icon_path = payment_wechat_green_asset_path();
    if (!file_exists(wechat_icon_path)) {
      wechat_icon_path = payment_wechat_asset_path();
    }
    if (!shell_home_payment::create_payment_cards(stage, tile_bg, tile_border, wechat_icon_path, payment_cards)) {
      return nullptr;
    }

    for (auto [card, target] : {std::pair<lv_obj_t*, PageId> {payment_cards.alipay_card, PageId::AppAlipay},
                                std::pair<lv_obj_t*, PageId> {payment_cards.wechat_card, PageId::AppWeChatPay}}) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
    }

    ui_prepare_box(music_card);
    ui_apply_surface(music_card, SurfaceStyle::Panel);
    lv_obj_set_size(music_card, 220, 106);
    lv_obj_align(music_card, LV_ALIGN_TOP_LEFT, 0, music_y);
    lv_obj_set_style_radius(music_card, 24, 0);
    lv_obj_set_style_pad_all(music_card, 0, 0);
    lv_obj_set_style_bg_color(music_card, music_bg, 0);
    lv_obj_set_style_border_width(music_card, 0, 0);

    lv_obj_t* music_status = lv_label_create(music_card);
    lv_obj_t* music_prev = lv_label_create(music_card);
    lv_obj_t* music_play = lv_label_create(music_card);
    lv_obj_t* music_next = lv_label_create(music_card);
    if (music_status == nullptr || music_prev == nullptr || music_play == nullptr || music_next == nullptr) {
      return nullptr;
    }

    ui_prepare_label(music_status);
    ui_apply_text(music_status, TextStyle::Title);
    lv_obj_set_style_text_font(music_status, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(music_status, music_fg, 0);
    lv_obj_set_width(music_status, 180);
    lv_label_set_long_mode(music_status, LV_LABEL_LONG_DOT);
    lv_label_set_text(music_status, "Phone not playing");
    lv_obj_align(music_status, LV_ALIGN_TOP_LEFT, 16, 16);

    for (lv_obj_t* control : {music_prev, music_play, music_next}) {
      ui_prepare_label(control);
      ui_apply_text(control, TextStyle::Hero);
      lv_obj_set_style_text_color(control, music_fg, 0);
    }
    lv_obj_set_style_text_font(music_prev, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_font(music_play, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_font(music_next, &lv_font_montserrat_18, 0);
    lv_label_set_text(music_prev, LV_SYMBOL_PREV);
    lv_label_set_text(music_play, LV_SYMBOL_PLAY);
    lv_label_set_text(music_next, LV_SYMBOL_NEXT);
    lv_obj_align(music_prev, LV_ALIGN_BOTTOM_LEFT, 24, -16);
    lv_obj_align(music_play, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_align(music_next, LV_ALIGN_BOTTOM_RIGHT, -24, -16);
  }

  {
    lv_obj_t* surface = surfaces_[3];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    lv_obj_t* stage = shell_home::create_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
      return nullptr;
    }

    shell_home_health::HealthCardsView health_cards {};
    if (!shell_home_health::create_health_cards(stage, health_cards)) {
      return nullptr;
    }

    for (auto [card, target] : {std::pair<lv_obj_t*, PageId> {health_cards.heart_card, PageId::AppHeartRate},
                                std::pair<lv_obj_t*, PageId> {health_cards.blood_oxygen_card, PageId::AppBloodOxygen},
                                std::pair<lv_obj_t*, PageId> {health_cards.breathing_card, PageId::AppBreathing},
                                std::pair<lv_obj_t*, PageId> {health_cards.stress_card, PageId::AppStress}}) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
    }
  }

  {
    lv_obj_t* surface = surfaces_[2];
    const lv_color_t stage_bg = lv_color_hex(0x040812);
    const lv_color_t card_bg = lv_color_hex(0x0A101A);
    lv_obj_t* stage = shell_home::create_stage_root(surface, layout, stage_bg);
    if (stage == nullptr) {
      return nullptr;
    }

    shell_home_payment::NfcCardView nfc_view {};
    const char* nfc_asset_path = nfc_school_card_inner_asset_path();
    if (!file_exists(nfc_asset_path)) {
      nfc_asset_path = nfc_school_card_asset_path();
    }
    if (!shell_home_payment::create_nfc_card_view(stage, card_bg, nfc_asset_path, nfc_view)) {
      return nullptr;
    }
    attach_click_guard(nfc_view.card);
    lv_obj_add_event_cb(nfc_view.card, app_tile_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_set_user_data(nfc_view.card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(PageId::AppNfc)));
  }

  {
    lv_obj_t* surface = surfaces_[4];
    shell_home_daily::DailyCardsView daily_cards {};
    if (!shell_home_daily::create_daily_cards(surface, layout, daily_cards)) {
      return nullptr;
    }
    lv_label_set_text(daily_cards.weather_temp_label, "23C");
    lv_label_set_text(daily_cards.weather_range_label, "30C / 18C");
    lv_label_set_text(daily_cards.sleep_value_label, "7h 36m");
    lv_label_set_text(daily_cards.steps_value_label, "--");
    weather_steps_value_label_ = daily_cards.steps_value_label;

    for (auto [card, target] : {std::pair<lv_obj_t*, PageId> {daily_cards.weather_card, PageId::AppWeather},
                                std::pair<lv_obj_t*, PageId> {daily_cards.sleep_card, PageId::AppSleep},
                                std::pair<lv_obj_t*, PageId> {daily_cards.steps_card, PageId::Pedometer}}) {
      attach_click_guard(card);
      lv_obj_add_event_cb(card, app_tile_event_cb, LV_EVENT_CLICKED, this);
      lv_obj_set_user_data(card, reinterpret_cast<void*>(static_cast<std::uintptr_t>(target)));
    }

    if (const auto steps = data_center_.steps()) {
      apply_steps(*steps);
    }
  }

  track(data_center_.subscribe(EventId::TimeUpdated,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<TimeModel>(&event.payload)) {
                                   apply_time(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::BatteryChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<BatteryModel>(&event.payload)) {
                                   apply_battery(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::StepsChanged,
                               [this](const Event& event) {
                                 if (const auto* model = std::get_if<StepsModel>(&event.payload)) {
                                   apply_steps(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::HomeRingPreviewChanged,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_) {
                                   return;
                                 }
                                 if (const auto* model = std::get_if<HomeRingPreviewModel>(&event.payload)) {
                                   apply_home_ring_preview(*model);
                                 }
                               }));
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || renderer_ == nullptr) {
                                   return;
                                 }
                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr || settled_surface_index_ != 0) {
                                   return;
                                 }
                                 switch (command->action) {
                                   case InputAction::TouchActivity:
                                     renderer_->apply(render_state_);
                                     break;
                                   case InputAction::CrownRotateCW:
                                     if (renderer_->on_crown_delta(std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     if (renderer_->on_crown_delta(-std::max<std::int16_t>(1, command->value), config_)) {
                                       render_state_.spread_index = config_.spread_index;
                                       renderer_->apply(render_state_);
                                     }
                                     break;
                                   default:
                                     break;
                                 }
                               }));

  layout_surfaces_for_preview(0, 0);
  set_track_x(0);
  return root;
}

}  // namespace twsim::app
