#include "watch_core_bridge.h"

#include <string.h>

static WatchCore s_core;
static watch_core_bridge_state_t s_state;

static void watch_core_bridge_sync_state(void)
{
    watch_core_get_ui_snapshot(&s_core, &s_state.snapshot);
}

void watch_core_bridge_init(void)
{
    memset(&s_state, 0, sizeof(s_state));

    watch_core_init(&s_core);
    s_state.current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
    s_state.current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_intent.type = WATCH_CORE_PAGE_INTENT_NONE;
    s_state.last_intent.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_event.type = WATCH_CORE_UI_EVENT_NONE;
    s_state.last_event.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_event.reserved = 0U;

    (void)watch_core_set_health_metric(&s_core, WATCH_CORE_HEALTH_FEATURE_HEART_RATE, "78");
    (void)watch_core_set_health_metric(&s_core, WATCH_CORE_HEALTH_FEATURE_SPO2, "98 %");
    (void)watch_core_set_health_metric(&s_core, WATCH_CORE_HEALTH_FEATURE_BREATHE, "18");
    (void)watch_core_set_health_metric(&s_core, WATCH_CORE_HEALTH_FEATURE_STRESS, "34");

    watch_core_bridge_sync_state();

    s_state.initialized = 1U;
}

bool watch_core_bridge_set_health_metric(WatchCoreHealthFeature feature, const char *metric_text)
{
    bool updated;

    updated = watch_core_set_health_metric(&s_core, feature, metric_text);
    if (!updated) {
        return false;
    }

    watch_core_bridge_sync_state();
    return true;
}

bool watch_core_bridge_dispatch_event(WatchCoreUiEvent event)
{
    WatchCorePageIntent intent;

    if (s_state.initialized == 0U) {
        return false;
    }

    s_state.last_event = event;
    if (!watch_core_push_event(&s_core, event)) {
        return false;
    }

    intent = watch_core_process_next_event(&s_core);
    s_state.last_intent = intent;
    if (intent.type != WATCH_CORE_PAGE_INTENT_NONE) {
        s_state.current_page = intent;
    }

    watch_core_bridge_sync_state();
    return true;
}

const watch_core_bridge_state_t * watch_core_bridge_state(void)
{
    return &s_state;
}
