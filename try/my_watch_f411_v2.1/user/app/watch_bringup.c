#include "watch_bringup.h"

#include <stdint.h>

#include "app/input/watch_input_intent.h"
#include "app/lvgl_demo/watch_lvgl_debug_screen.h"
#include "app/watch_core_bridge.h"
#include "board/display/watch_lcd.h"
#include "services/input/watch_input_service.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

static void publish_intent(watch_input_intent_t intent)
{
    if (intent == WATCH_INPUT_INTENT_NONE) {
        return;
    }

    watch_lvgl_port_feed_input_intent(intent);
    watch_lvgl_debug_screen_on_input_intent(intent);
}

static void pump_input_events(void)
{
    watch_input_event_t event;
    watch_input_intent_t intent;

    do {
        event = watch_input_service_get_event();
        intent = watch_input_intent_from_event(event);
        publish_intent(intent);
    } while (event != WATCH_INPUT_NONE);
}

void watch_bringup_init(void)
{
    watch_core_bridge_init();
    watch_lcd_init();
    watch_lcd_backlight_on();
    watch_lvgl_port_init();
    watch_lvgl_debug_screen_init();
    watch_input_service_init();
}

void watch_bringup_scan_input_10ms(void)
{
    watch_input_service_scan_10ms();
}

void watch_bringup_task(void)
{
    pump_input_events();

    watch_lvgl_port_task();
    watch_lvgl_debug_screen_task();
}
