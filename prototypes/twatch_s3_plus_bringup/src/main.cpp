#include <Arduino.h>
#include <LV_Helper.h>
#include <LilyGoLib.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

#include "mwbridge/BatteryPowerService.h"
#include "mwbridge/DataCenter.h"
#include "mwbridge/EventBus.h"

namespace {

constexpr uint32_t kLogIntervalMs = 1000;
constexpr uint32_t kPmuLogIntervalMs = 2000;
constexpr uint32_t kPowerTaskPeriodMs = 1000;
constexpr uint32_t kBmaLogIntervalMs = 500;
constexpr uint32_t kTouchMoveLogIntervalMs = 150;
constexpr int16_t kTouchMoveThresholdPx = 3;
constexpr uint8_t kBringupBrightness = 160;
constexpr uint8_t kScreenOffBrightness = 0;
constexpr uint32_t kPmuSleepReleaseTimeoutMs = 5000;
constexpr uint32_t kPowerTaskBootstrapTimeoutMs = 1500;
constexpr uint32_t kPowerTaskStackWords = 4096;

lv_obj_t *g_uptime_label = nullptr;
lv_obj_t *g_status_label = nullptr;
lv_obj_t *g_touch_label = nullptr;
lv_obj_t *g_pmu_label = nullptr;
lv_obj_t *g_bma_label = nullptr;
uint32_t g_last_log_ms = 0;
uint32_t g_last_pmu_log_ms = 0;
uint32_t g_last_bma_log_ms = 0;
uint32_t g_last_touch_log_ms = 0;
uint32_t g_touch_events = 0;
bool g_touch_pressed = false;
bool g_touch_seen = false;
bool g_screen_on = true;
uint32_t g_screen_toggles = 0;
lv_point_t g_last_touch_point = {0, 0};
volatile bool g_pmu_irq = false;
esp_sleep_wakeup_cause_t g_wakeup_cause = ESP_SLEEP_WAKEUP_UNDEFINED;
RTC_DATA_ATTR uint32_t g_rtc_boot_count = 0;

struct PmuSnapshot {
    bool charging = false;
    bool discharging = false;
    bool vbus_in = false;
    uint8_t charge_status = 0;
    uint16_t batt_mv = 0;
    uint16_t vbus_mv = 0;
    uint16_t sys_mv = 0;
    int batt_percent = 0;
};

struct BmaSnapshot {
    bool ok = false;
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    uint8_t direction = 0;
};

struct BridgeBatteryObserverState {
    bool has_model = false;
    mwbridge::BatteryModel last_model {};
    uint32_t publish_count = 0;
};

struct BridgePowerTaskState {
    bool has_snapshot = false;
    PmuSnapshot last_snapshot {};
    uint32_t sample_count = 0;
};

mwbridge::EventBus g_bridge_event_bus;
mwbridge::DataCenter g_bridge_data_center(g_bridge_event_bus);
mwbridge::BatteryPowerService g_bridge_battery_service(g_bridge_data_center);
mwbridge::EventBus::Subscription g_bridge_battery_subscription;
BridgeBatteryObserverState g_bridge_battery_observer;
BridgePowerTaskState g_bridge_power_task_state;
TaskHandle_t g_power_task_handle = nullptr;
portMUX_TYPE g_bridge_lock = portMUX_INITIALIZER_UNLOCKED;

const char *probeStatus(uint32_t probe_mask, uint32_t bit)
{
    return (probe_mask & bit) != 0 ? "ok" : "miss";
}

const char *yesNo(bool value)
{
    return value ? "Y" : "N";
}

const char *wakeupCauseName(esp_sleep_wakeup_cause_t cause)
{
    switch (cause) {
    case ESP_SLEEP_WAKEUP_EXT0:
        return "ext0";
    case ESP_SLEEP_WAKEUP_EXT1:
        return "ext1";
    case ESP_SLEEP_WAKEUP_TIMER:
        return "timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        return "touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
        return "ulp";
    case ESP_SLEEP_WAKEUP_GPIO:
        return "gpio";
    case ESP_SLEEP_WAKEUP_UART:
        return "uart";
    default:
        return "none";
    }
}

const char *chargeStatusName(uint8_t status)
{
    static const char *kChargeStatus[] = {
        "tri",
        "pre",
        "cc",
        "cv",
        "done",
        "none",
    };

    return status < sizeof(kChargeStatus) / sizeof(kChargeStatus[0]) ? kChargeStatus[status] : "unk";
}

const char *bmaDirectionName(uint8_t direction)
{
    switch (direction) {
    case SensorBMA423::DIRECTION_BOTTOM_LEFT:
        return "bottom-left";
    case SensorBMA423::DIRECTION_TOP_RIGHT:
        return "top-right";
    case SensorBMA423::DIRECTION_TOP_LEFT:
        return "top-left";
    case SensorBMA423::DIRECTION_BOTTOM_RIGHT:
        return "bottom-right";
    case SensorBMA423::DIRECTION_BOTTOM:
        return "bottom";
    case SensorBMA423::DIRECTION_TOP:
        return "top";
    default:
        return "unknown";
    }
}

PmuSnapshot readPmuSnapshot()
{
    PmuSnapshot snapshot;
    snapshot.charging = watch.isCharging();
    snapshot.discharging = watch.isDischarge();
    snapshot.vbus_in = watch.isVbusIn();
    snapshot.charge_status = watch.getChargerStatus();
    snapshot.batt_mv = watch.getBattVoltage();
    snapshot.vbus_mv = watch.getVbusVoltage();
    snapshot.sys_mv = watch.getSystemVoltage();
    snapshot.batt_percent = watch.getBatteryPercent();
    return snapshot;
}

BmaSnapshot readBmaSnapshot()
{
    BmaSnapshot snapshot;
    snapshot.ok = watch.getAccelerometer(snapshot.x, snapshot.y, snapshot.z);
    if (snapshot.ok) {
        snapshot.direction = watch.direction();
    }
    return snapshot;
}

mwbridge::BatterySample toBridgeBatterySample(const PmuSnapshot &snapshot)
{
    mwbridge::BatterySample sample;
    sample.present = snapshot.batt_mv > 0;
    sample.charging = snapshot.charging;
    sample.external_power = snapshot.vbus_in;
    sample.percent = static_cast<std::int16_t>(snapshot.batt_percent);
    sample.millivolts = snapshot.batt_mv;
    return sample;
}

void handleBridgeBatteryChanged(void *context, const mwbridge::BatteryModel &model)
{
    auto *state = static_cast<BridgeBatteryObserverState *>(context);
    if (state == nullptr) {
        return;
    }

    portENTER_CRITICAL(&g_bridge_lock);
    state->has_model = true;
    state->last_model = model;
    ++state->publish_count;
    portEXIT_CRITICAL(&g_bridge_lock);
}

void storeBridgePmuSnapshot(const PmuSnapshot &snapshot)
{
    portENTER_CRITICAL(&g_bridge_lock);
    g_bridge_power_task_state.has_snapshot = true;
    g_bridge_power_task_state.last_snapshot = snapshot;
    ++g_bridge_power_task_state.sample_count;
    portEXIT_CRITICAL(&g_bridge_lock);
}

bool tryGetBridgePmuSnapshot(PmuSnapshot *snapshot, uint32_t *sample_count)
{
    if (snapshot == nullptr) {
        return false;
    }

    portENTER_CRITICAL(&g_bridge_lock);
    const bool has_snapshot = g_bridge_power_task_state.has_snapshot;
    if (has_snapshot) {
        *snapshot = g_bridge_power_task_state.last_snapshot;
    }
    if (sample_count != nullptr) {
        *sample_count = g_bridge_power_task_state.sample_count;
    }
    portEXIT_CRITICAL(&g_bridge_lock);
    return has_snapshot;
}

uint32_t readBridgePublishCount()
{
    portENTER_CRITICAL(&g_bridge_lock);
    const uint32_t publish_count = g_bridge_battery_observer.publish_count;
    portEXIT_CRITICAL(&g_bridge_lock);
    return publish_count;
}

void bridgeSampleBatteryOnce()
{
    const PmuSnapshot snapshot = readPmuSnapshot();
    storeBridgePmuSnapshot(snapshot);
    g_bridge_battery_service.handle_sample(toBridgeBatterySample(snapshot));
}

void bridgePowerTask(void *parameter)
{
    (void)parameter;
    const TickType_t period_ticks = pdMS_TO_TICKS(kPowerTaskPeriodMs);
    TickType_t wake_tick = xTaskGetTickCount();

    for (;;) {
        bridgeSampleBatteryOnce();
        vTaskDelayUntil(&wake_tick, period_ticks);
    }
}

bool waitForBridgeBootstrap(uint32_t timeout_ms)
{
    const uint32_t start_ms = millis();
    PmuSnapshot snapshot;
    while (millis() - start_ms < timeout_ms) {
        if (tryGetBridgePmuSnapshot(&snapshot, nullptr)) {
            return true;
        }
        delay(20);
    }
    return false;
}

void setPmuIrqFlag()
{
    g_pmu_irq = true;
}

void setScreenState(bool on, const char *reason)
{
    if (g_screen_on == on) {
        return;
    }

    g_screen_on = on;
    ++g_screen_toggles;
    watch.setBrightness(on ? kBringupBrightness : kScreenOffBrightness);
    Serial.printf(
        "[bringup-screen] state=%s reason=%s toggles=%lu brightness=%u\n",
        on ? "on" : "off",
        reason,
        static_cast<unsigned long>(g_screen_toggles),
        watch.getBrightness());
}

bool waitForPmuIrqRelease(uint32_t timeout_ms)
{
    const uint32_t start_ms = millis();
    while (digitalRead(BOARD_PMU_INT) == LOW) {
        if (millis() - start_ms >= timeout_ms) {
            return false;
        }
        delay(10);
    }
    return true;
}

void enterPmuButtonDeepSleep()
{
    if (!g_screen_on) {
        setScreenState(true, "sleep-test");
    }

    Serial.printf(
        "[bringup-sleep] pmu_deep_sleep_prepare boot=%lu wake=%s pmu_int=%d\n",
        static_cast<unsigned long>(g_rtc_boot_count),
        wakeupCauseName(g_wakeup_cause),
        digitalRead(BOARD_PMU_INT));

    for (int remaining = 3; remaining > 0; --remaining) {
        lv_label_set_text_fmt(
            g_status_label,
            "Deep sleep in %d\nWake: PMU side key\nBoot %lu",
            remaining,
            static_cast<unsigned long>(g_rtc_boot_count));
        lv_label_set_text(g_touch_label, "Long PEK accepted");
        lv_label_set_text(g_pmu_label, "Release PEK after black screen");
        lv_label_set_text(g_bma_label, "Short PEK should wake/restart");
        lv_label_set_text(g_uptime_label, "sleep test");
        lv_task_handler();
        delay(1000);
    }

    lv_label_set_text(g_status_label, "Deep sleep now");
    lv_task_handler();
    delay(250);

    Serial.printf(
        "[bringup-sleep] entering_deep_sleep wake=pmu ext1_pin=%d pmu_int=%d\n",
        BOARD_PMU_INT,
        digitalRead(BOARD_PMU_INT));
    watch.setBrightness(kScreenOffBrightness);
    g_screen_on = false;
    delay(150);

    watch.clearPMU();
    const bool released = waitForPmuIrqRelease(kPmuSleepReleaseTimeoutMs);
    Serial.printf(
        "[bringup-sleep] pmu_irq_release=%s pmu_int=%d\n",
        yesNo(released),
        digitalRead(BOARD_PMU_INT));

    watch.setSleepMode(PMU_BTN_WAKEUP);
    watch.sleep(0);
}

void buildBringupScreen()
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x080b10), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Magic Watch");
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    lv_obj_t *subtitle = lv_label_create(screen);
    lv_label_set_text(subtitle, "T-Watch S3 Plus bring-up");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xaab3c2), 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t *panel = lv_obj_create(screen);
    lv_obj_set_size(panel, 214, 158);
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x151b24), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_pad_all(panel, 10, 0);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 26);

    g_status_label = lv_label_create(panel);
    lv_label_set_long_mode(g_status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_status_label, 194);
    lv_obj_set_style_text_color(g_status_label, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_text_font(g_status_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_status_label, LV_ALIGN_TOP_LEFT, 0, 0);

    g_touch_label = lv_label_create(panel);
    lv_label_set_long_mode(g_touch_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_touch_label, 194);
    lv_obj_set_style_text_color(g_touch_label, lv_color_hex(0xbae6fd), 0);
    lv_obj_set_style_text_font(g_touch_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_touch_label, LV_ALIGN_TOP_LEFT, 0, 46);

    g_pmu_label = lv_label_create(panel);
    lv_label_set_long_mode(g_pmu_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_pmu_label, 194);
    lv_obj_set_style_text_color(g_pmu_label, lv_color_hex(0xfde68a), 0);
    lv_obj_set_style_text_font(g_pmu_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_pmu_label, LV_ALIGN_TOP_LEFT, 0, 68);

    g_bma_label = lv_label_create(panel);
    lv_label_set_long_mode(g_bma_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_bma_label, 194);
    lv_obj_set_style_text_color(g_bma_label, lv_color_hex(0xc4b5fd), 0);
    lv_obj_set_style_text_font(g_bma_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_bma_label, LV_ALIGN_TOP_LEFT, 0, 106);

    g_uptime_label = lv_label_create(panel);
    lv_obj_set_style_text_color(g_uptime_label, lv_color_hex(0x7dd3fc), 0);
    lv_obj_set_style_text_font(g_uptime_label, &lv_font_montserrat_16, 0);
    lv_obj_align(g_uptime_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *footer = lv_label_create(screen);
    lv_label_set_text(footer, "Short PEK: screen | Long PEK: PMU sleep");
    lv_obj_set_style_text_color(footer, lv_color_hex(0x64748b), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void updateBringupScreen()
{
    const uint32_t seconds = millis() / 1000;
    const uint32_t probe = watch.getDeviceProbe();
    const BmaSnapshot bma = readBmaSnapshot();
    PmuSnapshot pmu {};
    uint32_t sample_count = 0;
    const bool has_pmu_snapshot = tryGetBridgePmuSnapshot(&pmu, &sample_count);

    lv_label_set_text_fmt(
        g_status_label,
        "Wake %s | Boot %lu\nScr %s Rot %u Tog %lu\nPMU %s T %s BMA %s Sm %lu",
        wakeupCauseName(g_wakeup_cause),
        static_cast<unsigned long>(g_rtc_boot_count),
        g_screen_on ? "on" : "off",
        watch.getRotation(),
        static_cast<unsigned long>(g_screen_toggles),
        probeStatus(probe, WATCH_PMU_ONLINE),
        probeStatus(probe, WATCH_TOUCH_ONLINE),
        probeStatus(probe, WATCH_BMA_ONLINE),
        static_cast<unsigned long>(sample_count));

    if (g_touch_pressed) {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch press %d,%d | Ev %lu",
            g_last_touch_point.x,
            g_last_touch_point.y,
            static_cast<unsigned long>(g_touch_events));
    } else if (g_touch_seen) {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch rel %d,%d | Ev %lu",
            g_last_touch_point.x,
            g_last_touch_point.y,
            static_cast<unsigned long>(g_touch_events));
    } else {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch idle | Ev %lu",
            static_cast<unsigned long>(g_touch_events));
    }

    if (has_pmu_snapshot) {
        lv_label_set_text_fmt(
            g_pmu_label,
            "USB %s | Chg %s | Dis %s | %s\nBat %dmV %d%% | Sys %dmV",
            yesNo(pmu.vbus_in),
            yesNo(pmu.charging),
            yesNo(pmu.discharging),
            chargeStatusName(pmu.charge_status),
            pmu.batt_mv,
            pmu.batt_percent,
            pmu.sys_mv);
    } else {
        lv_label_set_text(g_pmu_label, "Power_Task bootstrap...");
    }

    if (bma.ok) {
        lv_label_set_text_fmt(
            g_bma_label,
            "BMA x:%d y:%d z:%d\nDir %s",
            bma.x,
            bma.y,
            bma.z,
            bmaDirectionName(bma.direction));
    } else {
        lv_label_set_text(g_bma_label, "BMA read failed");
    }

    lv_label_set_text_fmt(g_uptime_label, "uptime %lu s", static_cast<unsigned long>(seconds));
}

bool readLvglTouch(lv_point_t *point, bool *pressed)
{
    lv_indev_t *indev = lv_indev_get_next(nullptr);
    if (!indev) {
        return false;
    }

    lv_indev_get_point(indev, point);
#if LV_VERSION_CHECK(9,0,0)
    *pressed = lv_indev_get_state(indev) == LV_INDEV_STATE_PRESSED;
#else
    *pressed = indev->proc.state == LV_INDEV_STATE_PRESSED;
#endif
    return true;
}

void logTouchEvent(const char *event_name, const lv_point_t &point)
{
    Serial.printf(
        "[bringup-touch] %s x=%d y=%d rotation=%u events=%lu\n",
        event_name,
        point.x,
        point.y,
        watch.getRotation(),
        static_cast<unsigned long>(g_touch_events));
}

void pollTouch()
{
    lv_point_t point = g_last_touch_point;
    bool pressed = false;
    if (!readLvglTouch(&point, &pressed)) {
        return;
    }

    const uint32_t now = millis();
    const bool moved =
        abs(point.x - g_last_touch_point.x) >= kTouchMoveThresholdPx ||
        abs(point.y - g_last_touch_point.y) >= kTouchMoveThresholdPx;

    if (pressed) {
        if (!g_screen_on) {
            setScreenState(true, "touch");
        }
        if (!g_touch_pressed) {
            ++g_touch_events;
            g_touch_seen = true;
            g_last_touch_log_ms = now;
            logTouchEvent("press", point);
        } else if (moved && now - g_last_touch_log_ms >= kTouchMoveLogIntervalMs) {
            g_last_touch_log_ms = now;
            logTouchEvent("move", point);
        }
        g_last_touch_point = point;
    } else if (g_touch_pressed) {
        ++g_touch_events;
        g_last_touch_point = point;
        g_last_touch_log_ms = now;
        logTouchEvent("release", point);
    }

    g_touch_pressed = pressed;
}

void processPmuInterrupts()
{
    if (!g_pmu_irq) {
        return;
    }
    g_pmu_irq = false;

    const uint64_t status = watch.readPMU();
    const bool long_press = watch.isPekeyLongPressIrq();
    const bool short_press = watch.isPekeyShortPressIrq();

    Serial.printf(
        "[bringup-screen] pmu_irq=0x%08lx short=%s long=%s screen=%s\n",
        static_cast<unsigned long>(status),
        yesNo(short_press),
        yesNo(long_press),
        g_screen_on ? "on" : "off");

    watch.clearPMU();

    if (long_press) {
        enterPmuButtonDeepSleep();
        return;
    }

    if (short_press) {
        setScreenState(!g_screen_on, "pmu-short");
    }
}

void logHeartbeat()
{
    const uint32_t now = millis();
    if (now - g_last_log_ms < kLogIntervalMs) {
        return;
    }
    g_last_log_ms = now;

    Serial.printf(
        "[bringup] uptime=%lus probe=0x%08lx rotation=%u brightness=%u free_heap=%lu psram=%lu\n",
        static_cast<unsigned long>(now / 1000),
        static_cast<unsigned long>(watch.getDeviceProbe()),
        watch.getRotation(),
        watch.getBrightness(),
        static_cast<unsigned long>(ESP.getFreeHeap()),
        static_cast<unsigned long>(ESP.getFreePsram()));
}

void logPmu()
{
    const uint32_t now = millis();
    if (now - g_last_pmu_log_ms < kPmuLogIntervalMs) {
        return;
    }
    g_last_pmu_log_ms = now;

    PmuSnapshot pmu {};
    uint32_t sample_count = 0;
    if (!tryGetBridgePmuSnapshot(&pmu, &sample_count)) {
        Serial.println("[bringup-pmu] waiting_for_power_task_bootstrap");
        return;
    }
    Serial.printf(
        "[bringup-pmu] usb=%s charging=%s discharging=%s chg=%s batt=%umV vbus=%umV sys=%umV percent=%d free_heap=%lu task_samples=%lu bridge_pub=%lu\n",
        yesNo(pmu.vbus_in),
        yesNo(pmu.charging),
        yesNo(pmu.discharging),
        chargeStatusName(pmu.charge_status),
        pmu.batt_mv,
        pmu.vbus_mv,
        pmu.sys_mv,
        pmu.batt_percent,
        static_cast<unsigned long>(ESP.getFreeHeap()),
        static_cast<unsigned long>(sample_count),
        static_cast<unsigned long>(readBridgePublishCount()));
}

void logBma()
{
    const uint32_t now = millis();
    if (now - g_last_bma_log_ms < kBmaLogIntervalMs) {
        return;
    }
    g_last_bma_log_ms = now;

    const BmaSnapshot bma = readBmaSnapshot();
    if (!bma.ok) {
        Serial.printf("[bringup-bma] read_failed rotation=%u\n", watch.getRotation());
        return;
    }

    Serial.printf(
        "[bringup-bma] x=%d y=%d z=%d dir=%s rotation=%u\n",
        bma.x,
        bma.y,
        bma.z,
        bmaDirectionName(bma.direction),
        watch.getRotation());
}

}  // namespace

void setup()
{
    Serial.begin(115200);
    delay(50);

    Serial.println();
    Serial.println("[bringup] Magic Watch T-Watch S3 Plus minimal bring-up");
    Serial.println("[bringup] Scope: serial log + 240x240 display + minimal LVGL");
    const uint32_t setup_start_ms = millis();

    g_wakeup_cause = esp_sleep_get_wakeup_cause();
    ++g_rtc_boot_count;
    Serial.printf(
        "[bringup-sleep] boot=%lu wake=%s cause=%d\n",
        static_cast<unsigned long>(g_rtc_boot_count),
        wakeupCauseName(g_wakeup_cause),
        static_cast<int>(g_wakeup_cause));

    watch.disableBootDisplay();
    if (!watch.begin(&Serial)) {
        Serial.println("[bringup] FATAL: watch.begin() failed. Check PMU, board power, and wiring.");
        while (true) {
            delay(1000);
        }
    }
    Serial.printf("[bringup-boot] watch.begin_ms=%lu\n", static_cast<unsigned long>(millis() - setup_start_ms));

    watch.setBrightness(kBringupBrightness);
    watch.configAccelerometer();
    watch.enableAccelerometer();
    watch.attachPMU(setPmuIrqFlag);
    watch.clearPMU();
    g_bridge_battery_subscription =
        g_bridge_data_center.subscribe_battery_changed(handleBridgeBatteryChanged, &g_bridge_battery_observer);
    const BaseType_t power_task_created = xTaskCreatePinnedToCore(
        bridgePowerTask,
        "Power_Task",
        kPowerTaskStackWords,
        nullptr,
        1,
        &g_power_task_handle,
        1);
    if (power_task_created != pdPASS) {
        Serial.printf("[bringup-power] FATAL: create Power_Task failed result=%ld\n",
                      static_cast<long>(power_task_created));
        while (true) {
            delay(1000);
        }
    }
    const bool bridge_bootstrapped = waitForBridgeBootstrap(kPowerTaskBootstrapTimeoutMs);
    Serial.printf("[bringup-power] bootstrap=%s period_ms=%lu\n",
                  bridge_bootstrapped ? "ok" : "timeout",
                  static_cast<unsigned long>(kPowerTaskPeriodMs));
    beginLvglHelper(false);
    buildBringupScreen();
    updateBringupScreen();
    Serial.printf("[bringup-boot] first_screen_ms=%lu\n", static_cast<unsigned long>(millis() - setup_start_ms));

    Serial.printf(
        "[bringup] LVGL ready: hor=%d ver=%d rotation=%u probe=0x%08lx\n",
        BOARD_TFT_WIDTH,
        BOARD_TFT_HEIHT,
        watch.getRotation(),
        static_cast<unsigned long>(watch.getDeviceProbe()));
}

void loop()
{
    lv_task_handler();
    processPmuInterrupts();
    pollTouch();
    updateBringupScreen();
    logHeartbeat();
    logPmu();
    logBma();
    delay(5);
}
