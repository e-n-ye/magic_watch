#include "app/screen/watch_screen_manager.h"

#include "board/display/watch_lcd.h"

static uint16_t color_for_screen(watch_screen_id_t screen)
{
    switch (screen) {
    case WATCH_SCREEN_HOME:
        return WATCH_LCD_BLUE;
    case WATCH_SCREEN_DEBUG:
        return WATCH_LCD_GREEN;
    case WATCH_SCREEN_MENU:
        return WATCH_LCD_RED;
    default:
        return WATCH_LCD_GRAY;
    }
}

static watch_screen_id_t next_screen(watch_screen_id_t screen)
{
    switch (screen) {
    case WATCH_SCREEN_HOME:
        return WATCH_SCREEN_DEBUG;
    case WATCH_SCREEN_DEBUG:
        return WATCH_SCREEN_MENU;
    default:
        return WATCH_SCREEN_HOME;
    }
}

static watch_screen_id_t previous_screen(watch_screen_id_t screen)
{
    switch (screen) {
    case WATCH_SCREEN_HOME:
        return WATCH_SCREEN_MENU;
    case WATCH_SCREEN_MENU:
        return WATCH_SCREEN_DEBUG;
    default:
        return WATCH_SCREEN_HOME;
    }
}

void watch_screen_manager_init(watch_screen_manager_t *manager)
{
    if (manager == 0) {
        return;
    }

    manager->current = WATCH_SCREEN_HOME;
    manager->event_count = 0U;
    manager->dirty = 1U;
}

void watch_screen_manager_handle_event(watch_screen_manager_t *manager, const watch_app_event_t *event)
{
    watch_input_intent_t intent;

    if ((manager == 0) || (event == 0) || (event->type != WATCH_APP_EVENT_INPUT_INTENT)) {
        return;
    }

    intent = event->payload.input_intent;
    if (intent == WATCH_INPUT_INTENT_NONE) {
        return;
    }

    manager->event_count++;
    manager->dirty = 1U;

    switch (intent) {
    case WATCH_INPUT_INTENT_CROWN_CLOCKWISE:
        manager->current = next_screen(manager->current);
        break;
    case WATCH_INPUT_INTENT_CROWN_COUNTERCLOCKWISE:
        manager->current = previous_screen(manager->current);
        break;
    case WATCH_INPUT_INTENT_CONFIRM:
        manager->current = WATCH_SCREEN_DEBUG;
        break;
    case WATCH_INPUT_INTENT_LONG_PRESS:
        manager->current = WATCH_SCREEN_HOME;
        break;
    default:
        break;
    }
}

void watch_screen_manager_render(watch_screen_manager_t *manager)
{
    uint16_t bar_width;

    if ((manager == 0) || (manager->dirty == 0U)) {
        return;
    }

    bar_width = (uint16_t)((manager->event_count % 24U) * 10U);
    if (bar_width == 0U) {
        bar_width = 10U;
    }

    watch_lcd_fill(color_for_screen(manager->current));
    watch_lcd_fill_rect(0U, 0U, bar_width, 12U, WATCH_LCD_BLACK);
    manager->dirty = 0U;
}
