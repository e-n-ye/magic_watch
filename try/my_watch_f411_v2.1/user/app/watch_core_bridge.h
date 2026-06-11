#ifndef WATCH_CORE_BRIDGE_H
#define WATCH_CORE_BRIDGE_H

#include <stdint.h>

#include "watch_core/watch_core.h"

typedef struct {
    WatchCoreUiModelSnapshot snapshot;
    WatchCorePageIntent last_intent;
    uint8_t initialized;
} watch_core_bridge_state_t;

void watch_core_bridge_init(void);

const watch_core_bridge_state_t * watch_core_bridge_state(void);

#endif /* WATCH_CORE_BRIDGE_H */
