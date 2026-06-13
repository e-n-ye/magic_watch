#include "app_direct.h"
#include "lvgl_sdl_runtime.h"

#if defined(_WIN32)
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    (void)argc;
    (void)argv;

    if (!watch_arch_runtime_init(240, 280, "Watch Architecture Lab - Stage 00")) {
        return 1;
    }

    app_direct_init();
    watch_arch_runtime_run();
    return 0;
}
