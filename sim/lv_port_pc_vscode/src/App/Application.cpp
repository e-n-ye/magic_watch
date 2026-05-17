#include "App/Application.h"

#include <array>
#include <cstdio>
#include <filesystem>
#include <string>

#include "App/UI/Pages/CommonPages.h"
#include "App/UI/Pages/SettingsPages.h"
#include "App/UI/Pages/ShellPages.h"
#include "App/UI/Pages/ToolPages.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/libs/tiny_ttf/lv_tiny_ttf.h"

namespace twsim::app {

namespace {

constexpr const char* kTextWeChat = "\xE5\xBE\xAE\xE4\xBF\xA1";
constexpr const char* kTextSenderChenHongzhi = "\xE9\x99\x88\xE5\xAE\x8F\xE5\xBF\x97";
constexpr const char* kTextJustNow = "\xE5\x88\x9A\xE5\x88\x9A";
constexpr const char* kTextBattery = "\xE7\x94\xB5\xE9\x87\x8F";
constexpr const char* kTextBatteryLow = "\xE7\x94\xB5\xE9\x87\x8F\xE4\xBD\x8E";
constexpr const char* kTextBatteryBodyPrefix =
    "\xE6\x89\x8B\xE8\xA1\xA8\xE7\x94\xB5\xE9\x87\x8F\xE8\xBF\x87\xE4\xBD\x8E\xEF\xBC\x8C"
    "\xE8\xAF\xB7\xE5\x8F\x8A\xE6\x97\xB6\xE5\x85\x85\xE7\x94\xB5\xE3\x80\x82";

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
}

std::string resolve_lvgl_asset_path(const char* relative_asset_path) {
  namespace fs = std::filesystem;
  if (relative_asset_path == nullptr || relative_asset_path[0] == '\0') {
    return {};
  }

  const fs::path relative_path(relative_asset_path);
  const fs::path start = fs::current_path();
  const std::array<fs::path, 7> candidates {
      start,
      start / "..",
      start / ".." / "..",
      start / ".." / ".." / "..",
      start / ".." / ".." / ".." / "..",
      start / ".." / ".." / ".." / ".." / "..",
      fs::path("D:/MY_Desk/watch/magic_watch"),
  };

  for (const auto& base : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(base / relative_path, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  return {};
}

std::string resolve_windows_cjk_font_path() {
  namespace fs = std::filesystem;
  const std::array<fs::path, 5> candidates {
      fs::path("C:/Windows/Fonts/Deng.ttf"),
      fs::path("C:/Windows/Fonts/Dengl.ttf"),
      fs::path("C:/Windows/Fonts/simhei.ttf"),
      fs::path("C:/Windows/Fonts/msyh.ttc"),
      fs::path("C:/Windows/Fonts/simsun.ttc"),
  };

  for (const auto& candidate : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(candidate, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  return {};
}

const lv_font_t* cjk_font_14() {
  static std::string font_path = resolve_windows_cjk_font_path();
  static lv_font_t* font = font_path.empty() ? nullptr : lv_tiny_ttf_create_file(font_path.c_str(), 14);
  return font != nullptr ? font : &lv_font_montserrat_14;
}

class NotificationToastOverlay {
 public:
  void attach(DataCenter& data_center) {
    if (attached_) {
      return;
    }
    data_center_ = &data_center;
    notifications_subscription_ =
        data_center.subscribe(EventId::NotificationsChanged,
                              [this](const Event& event) {
                                const auto* model = std::get_if<NotificationCenterModel>(&event.payload);
                                if (model == nullptr || model->active_toast_notification_id.has_value()) {
                                  return;
                                }
                                hide();
                              });
    toast_subscription_ =
        data_center.subscribe(EventId::NotificationToastRequested,
                              [this](const Event& event) {
                                const auto* item = std::get_if<NotificationItem>(&event.payload);
                                if (item == nullptr) {
                                  return;
                                }
                                show(*item);
                              });
    attached_ = true;
  }

 private:
  static void hide_timer_cb(lv_timer_t* timer) {
    auto* self = static_cast<NotificationToastOverlay*>(lv_timer_get_user_data(timer));
    if (self == nullptr) {
      return;
    }
    self->hide_timer_ = nullptr;
    if (self->data_center_ != nullptr) {
      self->data_center_->clear_toast();
    } else {
      self->hide();
    }
  }

  void ensure_root() {
    if (root_ != nullptr) {
      if (lv_obj_get_parent(root_) != lv_layer_top()) {
        lv_obj_set_parent(root_, lv_layer_top());
      }
      return;
    }

    root_ = lv_obj_create(lv_layer_top());
    if (root_ == nullptr) {
      return;
    }
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(root_, 212, 60);
    lv_obj_align(root_, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x102030), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_90, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_set_style_radius(root_, 20, 0);
    lv_obj_set_style_pad_all(root_, 10, 0);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

    icon_holder_ = lv_obj_create(root_);
    if (icon_holder_ == nullptr) {
      return;
    }
    lv_obj_remove_flag(icon_holder_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(icon_holder_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(icon_holder_, 38, 38);
    lv_obj_align(icon_holder_, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(icon_holder_, lv_color_hex(0x1E293B), 0);
    lv_obj_set_style_bg_opa(icon_holder_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(icon_holder_, 0, 0);
    lv_obj_set_style_radius(icon_holder_, 14, 0);
    lv_obj_set_style_pad_all(icon_holder_, 0, 0);

    icon_image_ = lv_image_create(icon_holder_);
    icon_label_ = lv_label_create(icon_holder_);
    if (icon_image_ != nullptr) {
      lv_obj_center(icon_image_);
      lv_obj_add_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
      lv_image_set_inner_align(icon_image_, LV_IMAGE_ALIGN_STRETCH);
    }
    if (icon_label_ != nullptr) {
      lv_obj_center(icon_label_);
      lv_obj_set_style_text_font(icon_label_, &lv_font_montserrat_18, 0);
      lv_obj_set_style_text_color(icon_label_, lv_color_hex(0xFFFFFF), 0);
      lv_obj_add_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
    }

    title_label_ = lv_label_create(root_);
    body_label_ = lv_label_create(root_);
    if (title_label_ != nullptr) {
      lv_obj_set_width(title_label_, 146);
      lv_obj_align(title_label_, LV_ALIGN_TOP_LEFT, 50, 9);
      lv_obj_set_style_text_font(title_label_, cjk_font_14(), 0);
      lv_obj_set_style_text_color(title_label_, lv_color_hex(0xF8FAFC), 0);
      lv_label_set_long_mode(title_label_, LV_LABEL_LONG_CLIP);
    }
    if (body_label_ != nullptr) {
      lv_obj_set_width(body_label_, 146);
      lv_obj_align(body_label_, LV_ALIGN_TOP_LEFT, 50, 30);
      lv_obj_set_style_text_font(body_label_, cjk_font_14(), 0);
      lv_obj_set_style_text_color(body_label_, lv_color_hex(0xCBD5E1), 0);
      lv_label_set_long_mode(body_label_, LV_LABEL_LONG_CLIP);
    }
  }

  void show(const NotificationItem& item) {
    ensure_root();
    if (root_ == nullptr || title_label_ == nullptr || body_label_ == nullptr || icon_holder_ == nullptr) {
      return;
    }

    const bool is_message = item.category == NotificationCategory::Message;
    const std::string wechat_asset = resolve_lvgl_asset_path("assets/generated_icons/payment_wechat_green.png");

    if (icon_image_ != nullptr && !wechat_asset.empty() && is_message) {
      lv_image_set_src(icon_image_, wechat_asset.c_str());
      lv_obj_set_size(icon_image_, 24, 24);
      lv_obj_clear_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
      if (icon_label_ != nullptr) {
        lv_obj_add_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
      }
      lv_obj_set_style_bg_color(icon_holder_, lv_color_hex(0x19C37D), 0);
    } else if (icon_label_ != nullptr) {
      lv_label_set_text(icon_label_, is_message ? LV_SYMBOL_BELL : LV_SYMBOL_CHARGE);
      lv_obj_clear_flag(icon_label_, LV_OBJ_FLAG_HIDDEN);
      if (icon_image_ != nullptr) {
        lv_obj_add_flag(icon_image_, LV_OBJ_FLAG_HIDDEN);
      }
      lv_obj_set_style_bg_color(icon_holder_, is_message ? lv_color_hex(0x1D4ED8) : lv_color_hex(0x8A6A00), 0);
    }

    lv_label_set_text(title_label_, is_message ? item.source_label.c_str() : item.title.c_str());
    lv_label_set_text(body_label_, is_message ? item.body.c_str() : item.body.c_str());
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(root_);

    if (hide_timer_ != nullptr) {
      lv_timer_del(hide_timer_);
      hide_timer_ = nullptr;
    }
    hide_timer_ = lv_timer_create(&NotificationToastOverlay::hide_timer_cb, 2200U, this);
    if (hide_timer_ != nullptr) {
      lv_timer_set_repeat_count(hide_timer_, 1);
    }
  }

  void hide() {
    if (hide_timer_ != nullptr) {
      lv_timer_del(hide_timer_);
      hide_timer_ = nullptr;
    }
    if (root_ != nullptr) {
      lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
  }

  DataCenter* data_center_ {nullptr};
  bool attached_ {false};
  EventBus::Subscription notifications_subscription_;
  EventBus::Subscription toast_subscription_;
  lv_obj_t* root_ {nullptr};
  lv_obj_t* icon_holder_ {nullptr};
  lv_obj_t* icon_image_ {nullptr};
  lv_obj_t* icon_label_ {nullptr};
  lv_obj_t* title_label_ {nullptr};
  lv_obj_t* body_label_ {nullptr};
  lv_timer_t* hide_timer_ {nullptr};
};

NotificationToastOverlay g_notification_toast_overlay;

using MenuItem = MenuPage::Item;

std::vector<MenuItem> timing_items() {
  return {
      {"Stopwatch", {NavigationAction::Push, PageId::Stopwatch}, "Elapsed timing"},
      {"Timer", {NavigationAction::Push, PageId::Timer}, "Countdown mode"},
      {"Alarm", {NavigationAction::Push, PageId::Alarm}, "Weekly schedule"},
  };
}

std::vector<MenuItem> game_items() {
  return {
      {"2048", {NavigationAction::Push, PageId::Game2048}, "Puzzle placeholder"},
  };
}

}  // namespace

Application::Application(std::unique_ptr<hal::Device> device)
    : device_(std::move(device)),
      input_router_(page_manager_),
      state_machine_(data_center_, page_manager_) {}

bool Application::start() {
  if (!device_) {
    return false;
  }

  register_pages();
  device_->set_event_callback([this](const hal::Event& event) { handle_hal_event(event); });
  if (!state_machine_.start()) {
    return false;
  }
  g_notification_toast_overlay.attach(data_center_);

  device_->tick(0);
  return true;
}

void Application::tick(std::uint32_t elapsed_ms) {
  if (!device_) {
    return;
  }

  device_->tick(elapsed_ms);
}

void Application::register_pages() {
  auto register_app_placeholder = [this](PageId page_id, const char* title, const char* detail) {
    page_manager_.register_page(page_id,
                                [this, page_id, title, detail]() {
                                  return std::make_unique<PlaceholderPage>(data_center_, page_id, title, detail, false);
                                });
  };

  page_manager_.register_page(PageId::HomeRingHost, [this]() { return std::make_unique<HomeRingHostPage>(data_center_); });
  page_manager_.register_page(PageId::Watchface, [this]() { return std::make_unique<WatchfacePage>(data_center_); });
  page_manager_.register_page(
      PageId::HomeShortcutPayments,
      [this]() {
        return std::make_unique<PaymentsShortcutPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::HomeShortcutNfc,
      [this]() {
        return std::make_unique<NfcShortcutPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::HomeShortcutHealth,
      [this]() {
        return std::make_unique<HealthShortcutPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::HomeShortcutWeather,
      [this]() {
        return std::make_unique<WeatherShortcutPage>(data_center_);
      });
  page_manager_.register_page(PageId::Launcher, [this]() { return std::make_unique<LauncherPage>(data_center_); });
  page_manager_.register_page(PageId::Notifications,
                              [this]() { return std::make_unique<NotificationsPage>(data_center_); });
  page_manager_.register_page(PageId::NotificationWakePreview,
                              [this]() { return std::make_unique<NotificationWakePage>(data_center_); });
  page_manager_.register_page(PageId::QuickSettings,
                              [this]() { return std::make_unique<QuickSettingsPage>(data_center_); });
  page_manager_.register_page(PageId::PowerMenu, [this]() { return std::make_unique<PowerMenuPage>(data_center_); });
  page_manager_.register_page(
      PageId::ScreenOff,
      [this]() {
        return std::make_unique<PassiveShellPage>(
            data_center_, PageId::ScreenOff, "Screen Off", "Press P / Enter to wake\nLong press P for power");
      });
  page_manager_.register_page(
      PageId::PoweredOff,
      [this]() {
        return std::make_unique<PassiveShellPage>(
            data_center_, PageId::PoweredOff, "Powered Off", "Long press P to boot\nLegacy alias: key 5");
      });

  page_manager_.register_page(PageId::SettingsHome,
                              [this]() {
                                return std::make_unique<SettingsHomePage>(data_center_);
                              });
  page_manager_.register_page(PageId::TimingToolsHome,
                              [this]() {
                                return std::make_unique<MenuPage>(
                                    data_center_, PageId::TimingToolsHome, "Timing", "Stopwatch / timer / alarm", timing_items());
                              });
  page_manager_.register_page(PageId::GamesHome,
                              [this]() {
                                return std::make_unique<MenuPage>(
                                    data_center_, PageId::GamesHome, "Games", "Expandable game hub", game_items());
                              });

  page_manager_.register_page(PageId::Gps,
                              [this]() {
                                return std::make_unique<PlaceholderPage>(
                                    data_center_, PageId::Gps, "GPS", "Simulator placeholder for future mock location streams.");
                              });
  page_manager_.register_page(
      PageId::Recorder,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::Recorder, "Recorder", "Voice recording shell. Playback routing can attach later.");
      });
  page_manager_.register_page(
      PageId::AudioPlayer,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::AudioPlayer, "Audio Player", "Audio file playback placeholder.");
      });
  page_manager_.register_page(
      PageId::VideoPlayer,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::VideoPlayer, "Video Player", "Video file playback placeholder.");
      });
  page_manager_.register_page(
      PageId::Infrared,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::Infrared, "Infrared", "IR control placeholder for future device routing.");
      });
  page_manager_.register_page(
      PageId::Pedometer,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::Pedometer, "Pedometer", "Step counting placeholder wired for future IMU data.");
      });
  page_manager_.register_page(PageId::Lora,
                              [this]() {
                                return std::make_unique<PlaceholderPage>(
                                    data_center_, PageId::Lora, "LoRa", "LoRa placeholder page.");
                              });
  page_manager_.register_page(
      PageId::Calculator,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::Calculator, "Calculator", "Calculator placeholder page.");
      });

  page_manager_.register_page(
      PageId::SettingDisplay,
      [this]() {
        return std::make_unique<DisplaySettingsPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingDisplayBrightness,
      [this]() {
        return std::make_unique<DisplayBrightnessPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingDisplayManualBrightness,
      [this]() {
        return std::make_unique<DisplayManualBrightnessPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingDisplayTimeout,
      [this]() {
        return std::make_unique<DisplayTimeoutPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingDisplayRaiseToWake,
      [this]() {
        return std::make_unique<DisplayRaiseToWakePage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingDisplayRaiseToWakeStartTime,
      [this]() {
        return std::make_unique<DisplayRaiseToWakeTimePage>(
            data_center_,
            PageId::SettingDisplayRaiseToWakeStartTime,
            "\xE5\xBC\x80\xE5\xA7\x8B\xE6\x97\xB6\xE9\x97\xB4",
            true);
      });
  page_manager_.register_page(
      PageId::SettingDisplayRaiseToWakeEndTime,
      [this]() {
        return std::make_unique<DisplayRaiseToWakeTimePage>(
            data_center_,
            PageId::SettingDisplayRaiseToWakeEndTime,
            "\xE7\xBB\x93\xE6\x9D\x9F\xE6\x97\xB6\xE9\x97\xB4",
            false);
      });
  page_manager_.register_page(
      PageId::SettingDisplayKeepScreenOn,
      [this]() {
        return std::make_unique<DisplayKeepScreenOnPage>(data_center_);
      });
  page_manager_.register_page(
      PageId::SettingSound,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingSound,
            "\xE5\xA3\xB0\xE9\x9F\xB3\xE4\xB8\x8E\xE6\x8C\xAF\xE5\x8A\xA8",
            "\xE6\x9C\xAC\xE8\xBD\xAE\xE5\x85\x88\xE6\x94\xBE\xE7\xBB\x9F\xE4\xB8\x80\xE5\x8D\xA0\xE4\xBD\x8D\xE9\xA1\xB5\xEF\xBC\x8C"
            "\xE5\x90\x8E\xE7\xBB\xAD\xE5\x86\x8D\xE6\x8B\x86\xE5\x88\x86\xE9\x93\x83\xE5\xA3\xB0\xE3\x80\x81\xE6\x8C\xAF\xE5\x8A\xA8"
            "\xE4\xB8\x8E\xE9\x9D\x99\xE9\x9F\xB3\xE7\xBB\x86\xE8\x8A\x82\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingDoNotDisturb,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingDoNotDisturb,
            "\xE5\x8B\xBF\xE6\x89\xB0\xE6\xA8\xA1\xE5\xBC\x8F",
            "\xE5\x8B\xBF\xE6\x89\xB0\xE7\x9A\x84\xE5\xBC\x80\xE5\x85\xB3\xE3\x80\x81\xE5\xAE\x9A\xE6\x97\xB6\xE8\xA7\x84\xE5\x88\x99"
            "\xE4\xB8\x8E\xE5\x8F\xAF\xE6\x89\x93\xE6\x89\xB0\xE4\xBE\x8B\xE5\xA4\x96\xE5\x90\x8E\xE7\xBB\xAD\xE5\x86\x8D\xE8\xA1\xA5\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingNotifications,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingNotifications,
            "\xE6\xB6\x88\xE6\x81\xAF\xE9\x80\x9A\xE7\x9F\xA5",
            "\xE6\xB6\x88\xE6\x81\xAF\xE9\x80\x9A\xE7\x9F\xA5\xE7\x9A\x84\xE5\x88\x86\xE7\xBB\x84\xE3\x80\x81\xE4\xBA\xAE\xE5\xB1\x8F"
            "\xE4\xB8\x8E\xE6\x8F\x90\xE7\xA4\xBA\xE7\xAD\x96\xE7\x95\xA5\xE7\x95\x99\xE5\x88\xB0\xE5\x90\x8E\xE7\xBB\xAD\xE5\xB0\x8F\xE8\xBD\xAE\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingAppLayout,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingAppLayout,
            "\xE5\xBA\x94\xE7\x94\xA8\xE5\xB8\x83\xE5\xB1\x80",
            "\xE5\xBA\x94\xE7\x94\xA8\xE5\x88\x97\xE8\xA1\xA8\xE3\x80\x81\xE5\x88\x86\xE7\xBB\x84\xE4\xB8\x8E\xE5\xB8\x83\xE5\xB1\x80"
            "\xE5\x88\x87\xE6\x8D\xA2\xE6\x9A\x82\xE4\xB8\x8D\xE5\xB1\x95\xE5\xBC\x80\xEF\xBC\x8C\xE5\x85\x88\xE4\xBF\x9D\xE7\x95\x99\xE5\x85\xA5\xE5\x8F\xA3\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingSystemActions,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingSystemActions,
            "\xE7\xB3\xBB\xE7\xBB\x9F\xE6\x93\x8D\xE4\xBD\x9C",
            "\xE9\x87\x8D\xE5\x90\xAF\xE3\x80\x81\xE5\x85\xB3\xE6\x9C\xBA\xE3\x80\x81\xE6\x81\xA2\xE5\xA4\x8D\xE4\xB8\x8E\xE7\xA1\xAE"
            "\xE8\xAE\xA4\xE6\xB5\x81\xE7\xA8\x8B\xE5\x90\x8E\xE7\xBB\xAD\xE5\x86\x8D\xE5\xBB\xBA\xE9\x97\xAD\xE7\x8E\xAF\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingAbout,
      [this]() {
        return std::make_unique<SettingsPlaceholderPage>(
            data_center_,
            PageId::SettingAbout,
            "\xE5\x85\xB3\xE4\xBA\x8E",
            "\xE7\x89\x88\xE6\x9C\xAC\xE4\xBF\xA1\xE6\x81\xAF\xE3\x80\x81\xE8\xAE\xBE\xE5\xA4\x87\xE8\xAF\x86\xE5\x88\xAB\xE4\xB8\x8E"
            "\xE7\x89\x88\xE6\x9C\xAC\xE5\x8F\xB7\xE5\xB1\x95\xE7\xA4\xBA\xE5\x90\x8E\xE7\xBB\xAD\xE5\x86\x8D\xE8\xBF\x81\xE7\xA7\xBB\xE8\xBF\x9B\xE6\x9D\xA5\xE3\x80\x82");
      });
  page_manager_.register_page(
      PageId::SettingTimeDate,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingTimeDate, "Time & Date", "Clock, date, and timezone placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingBattery,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingBattery, "Battery", "Battery and charging placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingBluetooth,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingBluetooth, "Bluetooth", "Bluetooth settings placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingWifi,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingWifi, "Wi-Fi", "Wi-Fi settings placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingDeveloper,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingDeveloper, "Developer", "Developer mode placeholder with future diagnostics.");
      });
  page_manager_.register_page(PageId::SettingVersion,
                              [this]() {
                                return std::make_unique<PlaceholderPage>(
                                    data_center_, PageId::SettingVersion, "Version", "Version information placeholder.");
                              });
  register_app_placeholder(PageId::AppHeartRate, "Heart Rate", "Heart rate app placeholder page.");
  register_app_placeholder(PageId::AppBloodOxygen, "Blood Oxygen", "SpO2 app placeholder page.");
  register_app_placeholder(PageId::AppBreathing, "Breathing", "Breathing training placeholder page.");
  register_app_placeholder(PageId::AppStress, "Stress", "Stress app placeholder page.");
  register_app_placeholder(PageId::AppSleep, "Sleep", "Sleep app placeholder page.");
  register_app_placeholder(PageId::AppWeather, "Weather", "Weather app placeholder page.");
  register_app_placeholder(PageId::AppNfc, "NFC", "NFC wallet placeholder page.");
  register_app_placeholder(PageId::AppAlipay, "Alipay", "Alipay app placeholder page.");
  register_app_placeholder(PageId::AppWeChatPay, "WeChat Pay", "WeChat Pay app placeholder page.");
  page_manager_.register_page(
      PageId::Game2048,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::Game2048, "2048", "Game placeholder under the Games hub.");
      });

  page_manager_.register_page(PageId::Stopwatch,
                              [this]() { return std::make_unique<StopwatchPage>(data_center_); });
  page_manager_.register_page(PageId::Timer, [this]() { return std::make_unique<TimerPage>(data_center_); });
  page_manager_.register_page(PageId::Alarm, [this]() { return std::make_unique<AlarmPage>(data_center_); });
}

void Application::handle_hal_event(const hal::Event& event) {
  switch (event.kind) {
    case hal::EventKind::TimeUpdated:
      if (const auto* model = std::get_if<hal::TimeSample>(&event.payload)) {
        data_center_.publish_time({model->valid,
                                   model->year,
                                   model->month,
                                   model->day,
                                   model->hour,
                                   model->minute,
                                   model->second});
      }
      break;
    case hal::EventKind::BatteryChanged:
      if (const auto* model = std::get_if<hal::BatterySample>(&event.payload)) {
        const BatteryModel battery_model {
            model->present, model->charging, model->external_power, model->percent, model->millivolts};
        data_center_.publish_battery(battery_model);
        maybe_emit_battery_low_notification(battery_model);
      }
      break;
    case hal::EventKind::ButtonChanged:
    case hal::EventKind::CrownUpdated:
    case hal::EventKind::TouchUpdated:
      if (const auto input = input_router_.translate(event)) {
        data_center_.publish_input(*input);
      }
      break;
    case hal::EventKind::MotionUpdated:
      if (const auto* model = std::get_if<hal::MotionSample>(&event.payload)) {
        data_center_.publish_motion({model->valid, model->x_mg, model->y_mg, model->z_mg});
      }
      break;
    case hal::EventKind::DebugAction:
      if (const auto* model = std::get_if<hal::DebugSample>(&event.payload)) {
        switch (model->action) {
          case hal::DebugSample::Action::InjectMessageNotification:
            data_center_.push_notification(make_mock_message_notification());
            break;
          case hal::DebugSample::Action::InjectBatteryLowNotification: {
            const auto* battery = data_center_.battery() ? &(*data_center_.battery()) : nullptr;
            data_center_.push_notification(make_mock_battery_low_notification(battery ? battery->percent : 20));
            break;
          }
          case hal::DebugSample::Action::SimRaiseToWake:
          case hal::DebugSample::Action::SimRaiseDismiss:
            if (const auto input = input_router_.translate(event)) {
              data_center_.publish_input(*input);
            }
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }
}

NotificationItem Application::make_mock_message_notification() {
  NotificationItem item;
  item.id = "msg-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::Message;
  item.source_label = kTextWeChat;
  item.title = kTextSenderChenHongzhi;
  item.body = std::string("[2\xE6\x9D\xA1]") + kTextSenderChenHongzhi + ": happy";
  item.time_text = kTextJustNow;
  item.badge_text = "2\xE6\x9D\xA1";
  return item;
}

NotificationItem Application::make_mock_battery_low_notification(std::int16_t percent) {
  NotificationItem item;
  item.id = "battery-" + std::to_string(percent) + "-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::BatteryLow;
  item.source_label = kTextBattery;
  item.title = kTextBatteryLow;
  char body_buffer[96] = {};
  std::snprintf(body_buffer, sizeof(body_buffer), "%s %d%%", kTextBatteryBodyPrefix, static_cast<int>(percent));
  item.body = body_buffer;
  item.time_text = kTextJustNow;
  item.badge_text = std::to_string(static_cast<int>(percent)) + "%";
  return item;
}

#if 0
NotificationItem Application::make_mock_message_notification() {
  NotificationItem item;
  item.id = "msg-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::Message;
  item.source_label = "微信";
  item.title = "陈宏志";
  item.body = "[2条]陈宏志: happy";
  item.time_text = "刚刚";
  item.badge_text = "2条";
  return item;
}

NotificationItem Application::make_mock_battery_low_notification(std::int16_t percent) {
  NotificationItem item;
  item.id = "battery-" + std::to_string(percent) + "-" + std::to_string(next_notification_id_++);
  item.category = NotificationCategory::BatteryLow;
  item.source_label = "电量";
  item.title = "电量低";
  char body_buffer[96] = {};
  std::snprintf(body_buffer, sizeof(body_buffer), "手表电量过低，请及时充电。%d%%", static_cast<int>(percent));
  item.body = body_buffer;
  item.time_text = "刚刚";
  item.badge_text = std::to_string(static_cast<int>(percent)) + "%";
  return item;
}

#endif

void Application::maybe_emit_battery_low_notification(const BatteryModel& model) {
  if (model.percent > 20) {
    battery_low_notification_latched_ = false;
    return;
  }
  if (battery_low_notification_latched_) {
    return;
  }
  battery_low_notification_latched_ = true;
  data_center_.push_notification(make_mock_battery_low_notification(model.percent));
}

}  // namespace twsim::app
