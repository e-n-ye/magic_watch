#include "App/Application.h"

#include "App/UI/Pages/CommonPages.h"
#include "App/UI/Pages/ShellPages.h"
#include "App/UI/Pages/ToolPages.h"

namespace twsim::app {

namespace {

using MenuItem = MenuPage::Item;
using ShortcutMetric = HomeShortcutPage::Metric;
using ShortcutConfig = HomeShortcutPage::Config;

std::vector<MenuItem> settings_items() {
  return {
      {"Display", {NavigationAction::Push, PageId::SettingDisplay}, "Brightness and screen"},
      {"Time & Date", {NavigationAction::Push, PageId::SettingTimeDate}, "Clock and calendar"},
      {"Sound", {NavigationAction::Push, PageId::SettingSound}, "Sound and vibration"},
      {"Battery", {NavigationAction::Push, PageId::SettingBattery}, "Power and charging"},
      {"Bluetooth", {NavigationAction::Push, PageId::SettingBluetooth}, "Nearby devices"},
      {"Wi-Fi", {NavigationAction::Push, PageId::SettingWifi}, "Connections"},
      {"Developer", {NavigationAction::Push, PageId::SettingDeveloper}, "Diagnostics and logs"},
      {"Version", {NavigationAction::Push, PageId::SettingVersion}, "Build information"},
  };
}

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

ShortcutConfig payments_shortcut_config() {
  return {
      PageId::HomeShortcutPayments,
      "",
      "Payments",
      "",
      "Now",
      "2 ready",
      "",
      {{
          {"WeChat", "Ready", ""},
          {"Alipay", "Ready", ""},
          {"Music", "Play", ""},
          {"Recent", "2", ""},
      }},
  };
}

ShortcutConfig nfc_shortcut_config() {
  return {
      PageId::HomeShortcutNfc,
      "",
      "NFC",
      "",
      "Ready",
      "Transit",
      "",
      {{
          {"Transit", "Metro", ""},
          {"Door", "Office", ""},
          {"Phone", "Tap", ""},
          {"Wallet", "1", ""},
      }},
  };
}

ShortcutConfig health_shortcut_config() {
  return {
      PageId::HomeShortcutHealth,
      "",
      "Health",
      "",
      "Heart",
      "74 bpm",
      "",
      {{
          {"SpO2", "98%", ""},
          {"Breath", "3m", ""},
          {"Mood", "Calm", ""},
          {"Stress", "Low", ""},
      }},
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
  page_manager_.register_page(PageId::Watchface, [this]() { return std::make_unique<WatchfacePage>(data_center_); });
  page_manager_.register_page(
      PageId::HomeShortcutPayments,
      [this]() {
        return std::make_unique<HomeShortcutPage>(data_center_, payments_shortcut_config());
      });
  page_manager_.register_page(
      PageId::HomeShortcutNfc,
      [this]() {
        return std::make_unique<HomeShortcutPage>(data_center_, nfc_shortcut_config());
      });
  page_manager_.register_page(
      PageId::HomeShortcutHealth,
      [this]() {
        return std::make_unique<HomeShortcutPage>(data_center_, health_shortcut_config());
      });
  page_manager_.register_page(
      PageId::HomeShortcutWeather,
      [this]() {
        return std::make_unique<WeatherShortcutPage>(data_center_);
      });
  page_manager_.register_page(PageId::Launcher, [this]() { return std::make_unique<LauncherPage>(data_center_); });
  page_manager_.register_page(PageId::Notifications,
                              [this]() { return std::make_unique<NotificationsPage>(data_center_); });
  page_manager_.register_page(PageId::QuickSettings,
                              [this]() { return std::make_unique<QuickSettingsPage>(data_center_); });
  page_manager_.register_page(PageId::PowerMenu, [this]() { return std::make_unique<PowerMenuPage>(data_center_); });
  page_manager_.register_page(
      PageId::ScreenOff,
      [this]() {
        return std::make_unique<PassiveShellPage>(
            data_center_, PageId::ScreenOff, "Screen Off", "Press key 5 or Enter to wake\nLong press key 5 for power");
      });
  page_manager_.register_page(
      PageId::PoweredOff,
      [this]() {
        return std::make_unique<PassiveShellPage>(
            data_center_, PageId::PoweredOff, "Powered Off", "Long press key 5 to boot");
      });

  page_manager_.register_page(PageId::SettingsHome,
                              [this]() {
                                return std::make_unique<MenuPage>(
                                    data_center_,
                                    PageId::SettingsHome,
                                    "Settings",
                                    "Scroll prototype / system configuration",
                                    settings_items(),
                                    true,
                                    true);
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
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingDisplay, "Display", "Brightness and screen behavior placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingTimeDate,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingTimeDate, "Time & Date", "Clock, date, and timezone placeholder.");
      });
  page_manager_.register_page(
      PageId::SettingSound,
      [this]() {
        return std::make_unique<PlaceholderPage>(
            data_center_, PageId::SettingSound, "Sound", "Sound and vibration placeholder.");
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
        data_center_.publish_battery(
            {model->present, model->charging, model->external_power, model->percent, model->millivolts});
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
    default:
      break;
  }
}

}  // namespace twsim::app
