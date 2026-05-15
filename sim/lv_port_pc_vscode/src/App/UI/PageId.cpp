#include "App/UI/PageId.h"

namespace twsim::app {

const char* page_name(PageId page_id) {
  switch (page_id) {
    case PageId::Watchface:
      return "Watchface";
    case PageId::Launcher:
      return "Launcher";
    case PageId::Notifications:
      return "Notifications";
    case PageId::NotificationWakePreview:
      return "NotificationWakePreview";
    case PageId::QuickSettings:
      return "QuickSettings";
    case PageId::PowerMenu:
      return "PowerMenu";
    case PageId::ScreenOff:
      return "ScreenOff";
    case PageId::PoweredOff:
      return "PoweredOff";
    case PageId::HomeShortcutPayments:
      return "HomeShortcutPayments";
    case PageId::HomeShortcutNfc:
      return "HomeShortcutNfc";
    case PageId::HomeShortcutHealth:
      return "HomeShortcutHealth";
    case PageId::HomeShortcutWeather:
      return "HomeShortcutWeather";
    case PageId::Gps:
      return "Gps";
    case PageId::Recorder:
      return "Recorder";
    case PageId::AudioPlayer:
      return "AudioPlayer";
    case PageId::VideoPlayer:
      return "VideoPlayer";
    case PageId::Infrared:
      return "Infrared";
    case PageId::Pedometer:
      return "Pedometer";
    case PageId::Lora:
      return "Lora";
    case PageId::SettingsHome:
      return "SettingsHome";
    case PageId::SettingDisplay:
      return "SettingDisplay";
    case PageId::SettingTimeDate:
      return "SettingTimeDate";
    case PageId::SettingSound:
      return "SettingSound";
    case PageId::SettingBattery:
      return "SettingBattery";
    case PageId::SettingBluetooth:
      return "SettingBluetooth";
    case PageId::SettingWifi:
      return "SettingWifi";
    case PageId::SettingDeveloper:
      return "SettingDeveloper";
    case PageId::SettingVersion:
      return "SettingVersion";
    case PageId::TimingToolsHome:
      return "TimingToolsHome";
    case PageId::Stopwatch:
      return "Stopwatch";
    case PageId::Timer:
      return "Timer";
    case PageId::Alarm:
      return "Alarm";
    case PageId::GamesHome:
      return "GamesHome";
    case PageId::Game2048:
      return "Game2048";
    case PageId::Calculator:
      return "Calculator";
    default:
      return "Unknown";
  }
}

}  // namespace twsim::app
