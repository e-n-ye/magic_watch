#include "App/UI/Pages/Shell/ShellAppVisualRegistry.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "lvgl/lvgl.h"

#include <algorithm>
#include <array>

namespace twsim::app {

namespace {

using shell_asset::health_breathe_asset_path;
using shell_asset::health_heart_asset_path;
using shell_asset::health_spo2_asset_path;
using shell_asset::health_stress_asset_path;
using shell_asset::payment_alipay_asset_path;
using shell_asset::preferred_wechat_icon_asset_path;
using shell_asset::sleep_icon_asset_path;
using shell_asset::steps_icon_asset_path;
using shell_asset::weather_icon_asset_path;

}  // namespace

const AppVisualSpec* find_app_visual_spec(PageId target) {
  static const std::array<AppVisualSpec, 11> specs {{
      {PageId::SettingsHome, "Settings", LV_SYMBOL_SETTINGS, nullptr, 0x201C22, 0xC7CED8},
      {PageId::AppWeather, "Weather", nullptr, weather_icon_asset_path(), 0x1FA3E5, 0xF8FAFC},
      {PageId::Pedometer, "Steps", nullptr, steps_icon_asset_path(), 0x2A5A2E, 0xF8FAFC},
      {PageId::AppHeartRate, "Heart", nullptr, health_heart_asset_path(), 0xFAFAFA, 0x0F172A},
      {PageId::AppBloodOxygen, "SpO2", nullptr, health_spo2_asset_path(), 0x26D9D6, 0xF8FAFC},
      {PageId::AppSleep, "Sleep", nullptr, sleep_icon_asset_path(), 0x9162C0, 0xF8FAFC},
      {PageId::AppStress, "Stress", nullptr, health_stress_asset_path(), 0x23433A, 0xF8FAFC},
      {PageId::AppBreathing, "Breathe", nullptr, health_breathe_asset_path(), 0x8F8F8F, 0xF8FAFC},
      {PageId::AppNfc, "NFC", "CARD", nullptr, 0x8E9B50, 0xF8FAFC},
      {PageId::AppAlipay, "Alipay", nullptr, payment_alipay_asset_path(), 0x008CFF, 0xF8FAFC},
      {PageId::AppWeChatPay, "WeChat", nullptr, preferred_wechat_icon_asset_path(), 0xEBEBEB, 0x0F172A},
  }};

  const auto it = std::find_if(specs.begin(), specs.end(), [target](const AppVisualSpec& spec) { return spec.target == target; });
  return it != specs.end() ? &(*it) : nullptr;
}

}  // namespace twsim::app
