#include "app/f411_ui_adapter.h"

#include <string.h>

#include "app/lvgl_demo/watch_lite_view.h"
#include "ui/lvgl_port/watch_lvgl_port.h"

typedef struct {
    WatchCore core;
    WatchCoreUiModelSnapshot snapshot;
    WatchCorePageIntent current_page;
    WatchCorePageIntent last_intent;
    WatchCoreUiEvent last_event;
    uint8_t initialized;
} f411_ui_adapter_state_t;

static f411_ui_adapter_state_t s_state;

static void f411_ui_adapter_sync_snapshot(void)
{
    watch_core_get_ui_snapshot(&s_state.core, &s_state.snapshot);
}

static void f411_ui_adapter_sync_view(void)
{
    watch_lite_view_apply_page_intent(&s_state.current_page);
    watch_lite_view_apply_snapshot(&s_state.snapshot);
}

static bool f411_ui_adapter_dispatch_event(WatchCoreUiEvent event)
{
    WatchCorePageIntent intent;

    if (s_state.initialized == 0U) {
        return false;
    }

    s_state.last_event = event;
    if (!watch_core_push_event(&s_state.core, event)) {
        return false;
    }

    intent = watch_core_process_next_event(&s_state.core);
    s_state.last_intent = intent;
    if (intent.type != WATCH_CORE_PAGE_INTENT_NONE) {
        s_state.current_page = intent;
    }

    f411_ui_adapter_sync_snapshot();
    f411_ui_adapter_sync_view();
    return true;
}

static void on_activate_health_card(WatchCoreHealthFeature feature)
{
    f411_ui_adapter_activate_health_card(feature);
}

static void on_back(void)
{
    f411_ui_adapter_back();
}

void f411_ui_adapter_init(void)
{
    static const watch_lite_view_callbacks_t s_callbacks = {
        on_activate_health_card,
        on_back,
    };

    if (s_state.initialized != 0U) {
        return;
    }

    memset(&s_state, 0, sizeof(s_state));

    watch_core_init(&s_state.core);
    s_state.current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
    s_state.current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_intent.type = WATCH_CORE_PAGE_INTENT_NONE;
    s_state.last_intent.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_event.type = WATCH_CORE_UI_EVENT_NONE;
    s_state.last_event.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    s_state.last_event.reserved = 0U;

    (void)watch_core_set_health_metric(&s_state.core, WATCH_CORE_HEALTH_FEATURE_HEART_RATE, "78");
    (void)watch_core_set_health_metric(&s_state.core, WATCH_CORE_HEALTH_FEATURE_SPO2, "98 %");
    (void)watch_core_set_health_metric(&s_state.core, WATCH_CORE_HEALTH_FEATURE_BREATHE, "18");
    (void)watch_core_set_health_metric(&s_state.core, WATCH_CORE_HEALTH_FEATURE_STRESS, "34");

    watch_lite_view_init(&s_callbacks);
    f411_ui_adapter_sync_snapshot();
    f411_ui_adapter_sync_view();
    watch_lite_view_update_swipe_hint(0U);

    s_state.initialized = 1U;
}

void f411_ui_adapter_task(void)
{
    bool swipe_commit;

    if (s_state.initialized == 0U) {
        return;
    }

    if (s_state.current_page.type == WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL) {
        watch_lite_view_update_swipe_hint(watch_lvgl_port_left_edge_swipe_back_progress());
    } else {
        watch_lite_view_update_swipe_hint(0U);
    }

    swipe_commit = watch_lvgl_port_take_left_edge_swipe_back();
    if (swipe_commit &&
        (s_state.current_page.type == WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL)) {
        watch_lite_view_update_swipe_hint(0U);
        f411_ui_adapter_back();
    }
}

void f411_ui_adapter_activate_health_card(WatchCoreHealthFeature feature)
{
    (void)f411_ui_adapter_dispatch_event(watch_core_make_health_card_clicked_event(feature));
}

void f411_ui_adapter_back(void)
{
    (void)f411_ui_adapter_dispatch_event(watch_core_make_back_event());
}
