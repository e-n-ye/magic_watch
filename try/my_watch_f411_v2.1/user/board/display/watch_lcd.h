#ifndef WATCH_LCD_H
#define WATCH_LCD_H

#include <stdint.h>

#define WATCH_LCD_BLACK   0x0000U
#define WATCH_LCD_WHITE   0xFFFFU
#define WATCH_LCD_RED     0xF800U
#define WATCH_LCD_GREEN   0x07E0U
#define WATCH_LCD_BLUE    0x001FU
#define WATCH_LCD_YELLOW  0xFFE0U
#define WATCH_LCD_CYAN    0x07FFU
#define WATCH_LCD_MAGENTA 0xF81FU
#define WATCH_LCD_GRAY    0x8410U

void watch_lcd_init(void);
void watch_lcd_backlight_on(void);
void watch_lcd_backlight_set(uint8_t percent);
void watch_lcd_fill(uint16_t color);
void watch_lcd_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void watch_lcd_draw_rgb565(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *pixels);
void watch_lcd_show_bringup_pattern(void);

#endif /* WATCH_LCD_H */
