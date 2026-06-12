#ifndef WATCH_LITE_VIEW_H
#define WATCH_LITE_VIEW_H

#include <stdint.h>

#include "watch_core/watch_core.h"

typedef struct {
    void (*activate_health_card)(WatchCoreHealthFeature feature);
    void (*back)(void);
} watch_lite_view_callbacks_t;

void watch_lite_view_init(const watch_lite_view_callbacks_t *callbacks);
void watch_lite_view_apply_snapshot(const WatchCoreUiModelSnapshot *snapshot);
void watch_lite_view_apply_page_intent(const WatchCorePageIntent *intent);
void watch_lite_view_update_swipe_hint(uint16_t progress);

#endif /* WATCH_LITE_VIEW_H */
