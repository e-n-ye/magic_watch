#pragma once

#include <cstdint>
#include <string>

#include "App/Common/AppEvents.h"

namespace twsim::app {

class DataCenter;

DisplayPolicyModel current_display_policy(DataCenter& data_center);
const char* timeout_text(std::uint32_t timeout_ms);
std::string brightness_detail_text(const DisplayPolicyModel& policy);
const char* keep_screen_on_text(std::uint32_t duration_ms);
const char* raise_to_wake_text(const DisplayPolicyModel& policy);
std::string format_hhmm(std::uint8_t hour, std::uint8_t minute);
std::string two_digit_options(int max_value);
const char* screen_off_display_text(const DisplayPolicyModel& policy);
const char* screen_off_style_text(const DisplayPolicyModel& policy);

}  // namespace twsim::app
