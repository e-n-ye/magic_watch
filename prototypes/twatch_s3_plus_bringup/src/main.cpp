#include <Arduino.h>
#include <LV_Helper.h>
#include <LilyGoLib.h>
#include <esp_sleep.h>
#include <lvgl.h>

namespace {

constexpr uint32_t kLogIntervalMs = 1000;
constexpr uint32_t kPmuLogIntervalMs = 2000;
constexpr uint32_t kBmaLogIntervalMs = 500;
constexpr uint32_t kTouchMoveLogIntervalMs = 150;
constexpr int16_t kTouchMoveThresholdPx = 3;
constexpr uint8_t kBringupBrightness = 160;
constexpr uint8_t kScreenOffBrightness = 0;
constexpr uint32_t kDeepSleepTimerSeconds = 15;

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

void enterTimerDeepSleep()
{
    if (!g_screen_on) {
        setScreenState(true, "sleep-test");
    }

    Serial.printf(
        "[bringup-sleep] timer_deep_sleep_prepare seconds=%lu boot=%lu wake=%s\n",
        static_cast<unsigned long>(kDeepSleepTimerSeconds),
        static_cast<unsigned long>(g_rtc_boot_count),
        wakeupCauseName(g_wakeup_cause));

    for (int remaining = 3; remaining > 0; --remaining) {
        lv_label_set_text_fmt(
            g_status_label,
            "Deep sleep in %d\nWake: timer %lu s\nBoot %lu",
            remaining,
            static_cast<unsigned long>(kDeepSleepTimerSeconds),
            static_cast<unsigned long>(g_rtc_boot_count));
        lv_label_set_text(g_touch_label, "Long PEK accepted");
        lv_label_set_text(g_pmu_label, "Display will turn off");
        lv_label_set_text(g_bma_label, "ESP32-S3 restarts after wake");
        lv_label_set_text(g_uptime_label, "sleep test");
        lv_task_handler();
        delay(1000);
    }

    lv_label_set_text(g_status_label, "Deep sleep now");
    lv_task_handler();
    delay(250);

    Serial.printf(
        "[bringup-sleep] entering_deep_sleep wake=timer seconds=%lu\n",
        static_cast<unsigned long>(kDeepSleepTimerSeconds));
    delay(50);

    watch.setBrightness(kScreenOffBrightness);
    g_screen_on = false;
    delay(150);

    esp_sleep_enable_timer_wakeup(1000000ULL * kDeepSleepTimerSeconds);
    esp_deep_sleep_start();
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
    lv_label_set_text(footer, "Short PEK: screen | Long PEK: timer sleep");
    lv_obj_set_style_text_color(footer, lv_color_hex(0x64748b), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void updateBringupScreen()
{
    const uint32_t seconds = millis() / 1000;
    const uint32_t probe = watch.getDeviceProbe();
    const PmuSnapshot pmu = readPmuSnapshot();
    const BmaSnapshot bma = readBmaSnapshot();

    lv_label_set_text_fmt(
        g_status_label,
        "Wake %s | Boot %lu\nScr %s Rot %u Tog %lu\nPMU %s T %s BMA %s",
        wakeupCauseName(g_wakeup_cause),
        static_cast<unsigned long>(g_rtc_boot_count),
        g_screen_on ? "on" : "off",
        watch.getRotation(),
        static_cast<unsigned long>(g_screen_toggles),
        probeStatus(probe, WATCH_PMU_ONLINE),
        probeStatus(probe, WATCH_TOUCH_ONLINE),
        probeStatus(probe, WATCH_BMA_ONLINE));

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
        enterTimerDeepSleep();
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

    const PmuSnapshot pmu = readPmuSnapshot();
    Serial.printf(
        "[bringup-pmu] usb=%s charging=%s discharging=%s chg=%s batt=%umV vbus=%umV sys=%umV percent=%d\n",
        yesNo(pmu.vbus_in),
        yesNo(pmu.charging),
        yesNo(pmu.discharging),
        chargeStatusName(pmu.charge_status),
        pmu.batt_mv,
        pmu.vbus_mv,
        pmu.sys_mv,
        pmu.batt_percent);
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
    delay(300);

    Serial.println();
    Serial.println("[bringup] Magic Watch T-Watch S3 Plus minimal bring-up");
    Serial.println("[bringup] Scope: serial log + 240x240 display + minimal LVGL");

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

    watch.setBrightness(kBringupBrightness);
    watch.configAccelerometer();
    watch.enableAccelerometer();
    watch.attachPMU(setPmuIrqFlag);
    watch.clearPMU();
    beginLvglHelper(false);
    buildBringupScreen();
    updateBringupScreen();

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
