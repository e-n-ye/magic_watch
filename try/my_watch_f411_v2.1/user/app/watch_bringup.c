#include "watch_bringup.h"

#include <stdint.h>

#include "board/display/watch_lcd.h"
#include "services/input/watch_input_service.h"

static uint32_t s_event_count;

static void show_event_color(watch_input_event_t event)
{
    uint16_t color = WATCH_LCD_GRAY;
    uint16_t bar_width;

    switch (event) {
    case WATCH_INPUT_BACK_SHORT:
        color = WATCH_LCD_YELLOW;
        break;
    case WATCH_INPUT_BACK_LONG:
        color = WATCH_LCD_MAGENTA;
        break;
    case WATCH_INPUT_WAKE_SHORT:
        color = WATCH_LCD_CYAN;
        break;
    case WATCH_INPUT_WAKE_LONG:
        color = WATCH_LCD_WHITE;
        break;
    case WATCH_INPUT_ENCODER_PRESS:
        color = WATCH_LCD_BLUE;
        break;
    case WATCH_INPUT_ENCODER_LONG:
        color = WATCH_LCD_MAGENTA;
        break;
    case WATCH_INPUT_ENCODER_CW:
        color = WATCH_LCD_GREEN;
        break;
    case WATCH_INPUT_ENCODER_CCW:
        color = WATCH_LCD_RED;
        break;
    default:
        return;
    }

    s_event_count++;
    bar_width = (uint16_t)((s_event_count % 24U) * 10U);
    if (bar_width == 0U) {
        bar_width = 10U;
    }

    watch_lcd_fill(color);
    watch_lcd_fill_rect(0U, 0U, bar_width, 12U, WATCH_LCD_BLACK);
}

void watch_bringup_init(void)
{
    s_event_count = 0U;
    watch_lcd_init();
    watch_lcd_backlight_on();
    watch_lcd_show_bringup_pattern();
    watch_input_service_init();
}

void watch_bringup_scan_input_10ms(void)
{
    watch_input_service_scan_10ms();
}

void watch_bringup_task(void)
{
    watch_input_event_t event;

    do {
        event = watch_input_service_get_event();
        show_event_color(event);
    } while (event != WATCH_INPUT_NONE);
}
