#include "App/UI/Pages/Health/BloodOxygenPageHelpers.h"

namespace twsim::app {

const char* low_blood_oxygen_mode_text(LowBloodOxygenReminderMode mode) {
  switch (mode) {
    case LowBloodOxygenReminderMode::Threshold90:
      return "90%";
    case LowBloodOxygenReminderMode::Threshold85:
      return "85%";
    case LowBloodOxygenReminderMode::Threshold80:
      return "80%";
    case LowBloodOxygenReminderMode::Off:
    default:
      return "不提醒";
  }
}

}  // namespace twsim::app
