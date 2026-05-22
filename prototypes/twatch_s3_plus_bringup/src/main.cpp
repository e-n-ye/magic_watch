#include <Arduino.h>
#include <LV_Helper.h>
#include <LilyGoLib.h>
#include <lvgl.h>

namespace {

constexpr uint32_t kLogIntervalMs = 1000;
constexpr uint32_t kPmuLogIntervalMs = 2000;
constexpr uint32_t kTouchMoveLogIntervalMs = 150;
constexpr int16_t kTouchMoveThresholdPx = 3;
constexpr uint8_t kBringupBrightness = 160;

lv_obj_t *g_uptime_label = nullptr;
lv_obj_t *g_status_label = nullptr;
lv_obj_t *g_touch_label = nullptr;
lv_obj_t *g_pmu_label = nullptr;
uint32_t g_last_log_ms = 0;
uint32_t g_last_pmu_log_ms = 0;
uint32_t g_last_touch_log_ms = 0;
uint32_t g_touch_events = 0;
bool g_touch_pressed = false;
bool g_touch_seen = false;
lv_point_t g_last_touch_point = {0, 0};

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

const char *probeStatus(uint32_t probe_mask, uint32_t bit)
{
    return (probe_mask & bit) != 0 ? "ok" : "miss";
}

const char *yesNo(bool value)
{
    return value ? "Y" : "N";
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
    lv_obj_set_size(panel, 212, 146);
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x151b24), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_pad_all(panel, 10, 0);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 24);

    g_status_label = lv_label_create(panel);
    lv_label_set_long_mode(g_status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_status_label, 190);
    lv_obj_set_style_text_color(g_status_label, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_text_font(g_status_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_status_label, LV_ALIGN_TOP_LEFT, 0, 0);

    g_touch_label = lv_label_create(panel);
    lv_label_set_long_mode(g_touch_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_touch_label, 190);
    lv_obj_set_style_text_color(g_touch_label, lv_color_hex(0xbae6fd), 0);
    lv_obj_set_style_text_font(g_touch_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_touch_label, LV_ALIGN_TOP_LEFT, 0, 38);

    g_pmu_label = lv_label_create(panel);
    lv_label_set_long_mode(g_pmu_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_pmu_label, 190);
    lv_obj_set_style_text_color(g_pmu_label, lv_color_hex(0xfde68a), 0);
    lv_obj_set_style_text_font(g_pmu_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_pmu_label, LV_ALIGN_TOP_LEFT, 0, 76);

    g_uptime_label = lv_label_create(panel);
    lv_obj_set_style_text_color(g_uptime_label, lv_color_hex(0x7dd3fc), 0);
    lv_obj_set_style_text_font(g_uptime_label, &lv_font_montserrat_16, 0);
    lv_obj_align(g_uptime_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *footer = lv_label_create(screen);
    lv_label_set_text(footer, "touch + AXP2101 monitor");
    lv_obj_set_style_text_color(footer, lv_color_hex(0x64748b), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void updateBringupScreen()
{
    const uint32_t seconds = millis() / 1000;
    const uint32_t probe = watch.getDeviceProbe();
    const PmuSnapshot pmu = readPmuSnapshot();

    lv_label_set_text_fmt(
        g_status_label,
        "Disp 240x240 | Rot %u\nPMU %s | Touch %s | BMA %s",
        watch.getRotation(),
        probeStatus(probe, WATCH_PMU_ONLINE),
        probeStatus(probe, WATCH_TOUCH_ONLINE),
        probeStatus(probe, WATCH_BMA_ONLINE));

    if (g_touch_pressed) {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch press %d,%d\nEvents %lu",
            g_last_touch_point.x,
            g_last_touch_point.y,
            static_cast<unsigned long>(g_touch_events));
    } else if (g_touch_seen) {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch release %d,%d\nEvents %lu",
            g_last_touch_point.x,
            g_last_touch_point.y,
            static_cast<unsigned long>(g_touch_events));
    } else {
        lv_label_set_text_fmt(
            g_touch_label,
            "Touch idle\nEvents %lu",
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

}  // namespace

void setup()
{
    Serial.begin(115200);
    delay(300);

    Serial.println();
    Serial.println("[bringup] Magic Watch T-Watch S3 Plus minimal bring-up");
    Serial.println("[bringup] Scope: serial log + 240x240 display + minimal LVGL");

    watch.disableBootDisplay();
    if (!watch.begin(&Serial)) {
        Serial.println("[bringup] FATAL: watch.begin() failed. Check PMU, board power, and wiring.");
        while (true) {
            delay(1000);
        }
    }

    watch.setBrightness(kBringupBrightness);
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
    pollTouch();
    updateBringupScreen();
    logHeartbeat();
    logPmu();
    delay(5);
}
