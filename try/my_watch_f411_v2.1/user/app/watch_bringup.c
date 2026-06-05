#include "watch_bringup.h"

#include <stdint.h>

#include "app/input/watch_input_intent.h"
#include "app/lvgl_demo/watch_lvgl_debug_screen.h"
#include "app/screen/watch_screen_manager.h"
#include "board/display/watch_lcd.h"
#include "core/event/watch_event_queue.h"
#include "services/input/watch_input_service.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

static watch_event_queue_t s_event_queue;
static watch_screen_manager_t s_screen_manager;

static void publish_intent(watch_input_intent_t intent)
{
    watch_app_event_t app_event;

    if (intent == WATCH_INPUT_INTENT_NONE) {
        return;
    }

    app_event.type = WATCH_APP_EVENT_INPUT_INTENT;
    app_event.payload.input_intent = intent;
    (void)watch_event_queue_push(&s_event_queue, app_event);
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

static void handle_app_event(const watch_app_event_t *event)
{
    watch_screen_manager_handle_event(&s_screen_manager, event);
}

void watch_bringup_init(void)
{
    watch_event_queue_init(&s_event_queue);
    watch_screen_manager_init(&s_screen_manager);
    watch_lcd_init();
    watch_lcd_backlight_on();
    watch_lvgl_port_init();
    watch_screen_manager_render(&s_screen_manager);
    watch_lvgl_debug_screen_init();
    watch_input_service_init();
}

void watch_bringup_scan_input_10ms(void)
{
    watch_input_service_scan_10ms();
}

void watch_bringup_task(void)
{
    watch_app_event_t app_event;

    pump_input_events();

    while (watch_event_queue_pop(&s_event_queue, &app_event) != 0U) {
        handle_app_event(&app_event);
        watch_screen_manager_render(&s_screen_manager);
    }

    watch_lvgl_port_task();
}
