#ifndef WATCH_CORE_BRIDGE_H
#define WATCH_CORE_BRIDGE_H

#include <stdbool.h>
#include <stdint.h>

#include "watch_core/watch_core.h"

typedef struct {
    WatchCoreUiModelSnapshot snapshot;
    WatchCorePageIntent last_intent;
    WatchCorePageIntent current_page;
    WatchCoreUiEvent last_event;
    uint8_t initialized;
} watch_core_bridge_state_t;

void watch_core_bridge_init(void);
bool watch_core_bridge_set_health_metric(WatchCoreHealthFeature feature, const char *metric_text);
bool watch_core_bridge_dispatch_event(WatchCoreUiEvent event);

const watch_core_bridge_state_t * watch_core_bridge_state(void);

#endif /* WATCH_CORE_BRIDGE_H */
