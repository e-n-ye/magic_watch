#include <Arduino.h>
#include <LV_Helper.h>
#include <LilyGoLib.h>
#include <lvgl.h>

namespace {

constexpr uint32_t kLogIntervalMs = 1000;
constexpr uint8_t kBringupBrightness = 160;

lv_obj_t *g_uptime_label = nullptr;
lv_obj_t *g_status_label = nullptr;
uint32_t g_last_log_ms = 0;

const char *probeStatus(uint32_t probe_mask, uint32_t bit)
{
    return (probe_mask & bit) != 0 ? "ok" : "miss";
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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 24);

    lv_obj_t *subtitle = lv_label_create(screen);
    lv_label_set_text(subtitle, "T-Watch S3 Plus bring-up");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xaab3c2), 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 58);

    lv_obj_t *panel = lv_obj_create(screen);
    lv_obj_set_size(panel, 210, 108);
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x151b24), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_pad_all(panel, 10, 0);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 16);

    g_status_label = lv_label_create(panel);
    lv_label_set_long_mode(g_status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_status_label, 190);
    lv_obj_set_style_text_color(g_status_label, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_text_font(g_status_label, &lv_font_montserrat_12, 0);
    lv_obj_align(g_status_label, LV_ALIGN_TOP_LEFT, 0, 0);

    g_uptime_label = lv_label_create(panel);
    lv_obj_set_style_text_color(g_uptime_label, lv_color_hex(0x7dd3fc), 0);
    lv_obj_set_style_text_font(g_uptime_label, &lv_font_montserrat_20, 0);
    lv_obj_align(g_uptime_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *footer = lv_label_create(screen);
    lv_label_set_text(footer, "240x240 LVGL minimal screen");
    lv_obj_set_style_text_color(footer, lv_color_hex(0x64748b), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -18);
}

void updateBringupScreen()
{
    const uint32_t seconds = millis() / 1000;
    const uint32_t probe = watch.getDeviceProbe();

    lv_label_set_text_fmt(
        g_status_label,
        "Display 240 x 240\nPMU %s | Touch %s\nBMA %s | Rot %u",
        probeStatus(probe, WATCH_PMU_ONLINE),
        probeStatus(probe, WATCH_TOUCH_ONLINE),
        probeStatus(probe, WATCH_BMA_ONLINE),
        watch.getRotation());

    lv_label_set_text_fmt(g_uptime_label, "uptime %lu s", static_cast<unsigned long>(seconds));
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
    updateBringupScreen();
    logHeartbeat();
    lv_task_handler();
    delay(5);
}
