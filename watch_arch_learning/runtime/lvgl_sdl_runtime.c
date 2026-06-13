#include "lvgl_sdl_runtime.h"

#include <SDL.h>

#include "lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"

bool watch_arch_runtime_init(int32_t width, int32_t height, const char *title)
{
    lv_display_t *display;

    lv_init();

    display = lv_sdl_window_create(width, height);
    if (display == NULL) {
        return false;
    }

    lv_display_set_default(display);
    lv_sdl_window_set_title(display, title);

    if (lv_sdl_mouse_create() == NULL) {
        return false;
    }

    return true;
}

void watch_arch_runtime_run(void)
{
    while (true) {
        uint32_t sleep_time_ms = lv_timer_handler();

        if (sleep_time_ms == LV_NO_TIMER_READY) {
            sleep_time_ms = LV_DEF_REFR_PERIOD;
        }
        if (sleep_time_ms > 16U) {
            sleep_time_ms = 16U;
        }

        SDL_Delay(sleep_time_ms);
    }
}
