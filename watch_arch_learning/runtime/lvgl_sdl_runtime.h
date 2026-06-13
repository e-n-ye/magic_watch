#ifndef WATCH_ARCH_LVGL_SDL_RUNTIME_H
#define WATCH_ARCH_LVGL_SDL_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

bool watch_arch_runtime_init(int32_t width, int32_t height, const char *title);
void watch_arch_runtime_run(void);

#endif /* WATCH_ARCH_LVGL_SDL_RUNTIME_H */
