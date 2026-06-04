#ifndef WATCH_SCREEN_MANAGER_H
#define WATCH_SCREEN_MANAGER_H

#include <stdint.h>

#include "core/event/watch_event_queue.h"

typedef enum {
    WATCH_SCREEN_HOME = 0,
    WATCH_SCREEN_DEBUG,
    WATCH_SCREEN_MENU,
} watch_screen_id_t;

typedef struct {
    watch_screen_id_t current;
    uint32_t event_count;
    uint8_t dirty;
} watch_screen_manager_t;

void watch_screen_manager_init(watch_screen_manager_t *manager);
void watch_screen_manager_handle_event(watch_screen_manager_t *manager, const watch_app_event_t *event);
void watch_screen_manager_render(watch_screen_manager_t *manager);

#endif /* WATCH_SCREEN_MANAGER_H */
