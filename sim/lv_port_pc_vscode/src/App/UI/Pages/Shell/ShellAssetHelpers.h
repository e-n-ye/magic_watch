#pragma once

namespace twsim::app::shell_asset {

bool file_exists(const char* path);

const char* weather_icon_asset_path();
const char* sleep_icon_asset_path();
const char* steps_icon_asset_path();
const char* payment_alipay_asset_path();
const char* payment_wechat_asset_path();
const char* payment_wechat_green_asset_path();
const char* preferred_wechat_icon_asset_path();
const char* health_heart_asset_path();
const char* health_spo2_asset_path();
const char* health_breathe_asset_path();
const char* health_stress_asset_path();
const char* nfc_school_card_asset_path();
const char* nfc_school_card_inner_asset_path();

}  // namespace twsim::app::shell_asset
