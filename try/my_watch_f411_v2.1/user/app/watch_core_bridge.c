#include "watch_core_bridge.h"

#include <string.h>

static WatchCore s_core;
static watch_core_bridge_state_t s_state;

void watch_core_bridge_init(void)
{
    WatchCoreUiEvent probe_event;

    watch_core_init(&s_core);
    watch_core_get_ui_snapshot(&s_core, &s_state.snapshot);

    probe_event = watch_core_make_back_event();
    if (watch_core_push_event(&s_core, probe_event)) {
        s_state.last_intent = watch_core_process_next_event(&s_core);
    } else {
        memset(&s_state.last_intent, 0, sizeof(s_state.last_intent));
    }

    s_state.initialized = 1U;
}

const watch_core_bridge_state_t * watch_core_bridge_state(void)
{
    return &s_state;
}
