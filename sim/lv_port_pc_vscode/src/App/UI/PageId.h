#pragma once

namespace twsim::app {

enum class PageId {
  Watchface,
  Launcher,
  Notifications,
  QuickSettings,
  PowerMenu,
  ScreenOff,
  PoweredOff,
  HomeShortcutPayments,
  HomeShortcutNfc,
  HomeShortcutHealth,
  HomeShortcutWeather,
  Gps,
  Recorder,
  AudioPlayer,
  VideoPlayer,
  Infrared,
  Pedometer,
  Lora,
  SettingsHome,
  SettingDisplay,
  SettingTimeDate,
  SettingSound,
  SettingBattery,
  SettingBluetooth,
  SettingWifi,
  SettingDeveloper,
  SettingVersion,
  TimingToolsHome,
  Stopwatch,
  Timer,
  Alarm,
  GamesHome,
  Game2048,
  Calculator,
};

const char* page_name(PageId page_id);

}  // namespace twsim::app
