#include "watch_core/watch_core.h"

#include <stddef.h>
#include <string.h>

static void copy_metric_text(char * dst, const char * src)
{
    if (dst == NULL) {
        return;
    }

    if (src == NULL) {
        src = "";
    }

    size_t i = 0U;
    for (; i + 1U < WATCH_CORE_METRIC_TEXT_MAX && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

static bool pop_event(WatchCore * core, WatchCoreUiEvent * out_event)
{
    if (core == NULL || out_event == NULL || core->queue_count == 0U) {
        return false;
    }

    *out_event = core->event_queue[core->queue_head];
    core->queue_head = (uint8_t)((core->queue_head + 1U) % WATCH_CORE_EVENT_QUEUE_CAPACITY);
    core->queue_count--;
    return true;
}

void watch_core_init(WatchCore * core)
{
    if (core == NULL) {
        return;
    }

    memset(core, 0, sizeof(*core));

    copy_metric_text(core->model.health_metric_text[WATCH_CORE_HEALTH_FEATURE_HEART_RATE], "--");
    copy_metric_text(core->model.health_metric_text[WATCH_CORE_HEALTH_FEATURE_SPO2], "98 %");
    copy_metric_text(core->model.health_metric_text[WATCH_CORE_HEALTH_FEATURE_BREATHE], "18");
    copy_metric_text(core->model.health_metric_text[WATCH_CORE_HEALTH_FEATURE_STRESS], "34");

    core->current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
    core->current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
}

void watch_core_get_ui_snapshot(const WatchCore * core, WatchCoreUiModelSnapshot * out_snapshot)
{
    if (core == NULL || out_snapshot == NULL) {
        return;
    }

    *out_snapshot = core->model;
}

bool watch_core_set_health_metric(
    WatchCore * core,
    WatchCoreHealthFeature feature,
    const char * metric_text)
{
    if (core == NULL || !watch_core_health_feature_is_valid(feature)) {
        return false;
    }

    copy_metric_text(core->model.health_metric_text[feature], metric_text);
    return true;
}

WatchCoreUiEvent watch_core_make_health_card_clicked_event(WatchCoreHealthFeature feature)
{
    WatchCoreUiEvent event;
    event.type = WATCH_CORE_UI_EVENT_HEALTH_CARD_CLICKED;
    event.feature = feature;
    event.reserved = 0U;
    return event;
}

WatchCoreUiEvent watch_core_make_back_event(void)
{
    WatchCoreUiEvent event;
    event.type = WATCH_CORE_UI_EVENT_BACK;
    event.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    event.reserved = 0U;
    return event;
}

bool watch_core_push_event(WatchCore * core, WatchCoreUiEvent event)
{
    if (core == NULL || core->queue_count >= WATCH_CORE_EVENT_QUEUE_CAPACITY) {
        return false;
    }

    core->event_queue[core->queue_tail] = event;
    core->queue_tail = (uint8_t)((core->queue_tail + 1U) % WATCH_CORE_EVENT_QUEUE_CAPACITY);
    core->queue_count++;
    return true;
}

WatchCorePageIntent watch_core_process_next_event(WatchCore * core)
{
    WatchCorePageIntent none;
    none.type = WATCH_CORE_PAGE_INTENT_NONE;
    none.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;

    WatchCoreUiEvent event;
    if (core == NULL || !pop_event(core, &event)) {
        return none;
    }

    if (event.type == WATCH_CORE_UI_EVENT_HEALTH_CARD_CLICKED) {
        if (!watch_core_health_feature_is_valid(event.feature)) {
            return none;
        }

        core->current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL;
        core->current_page.feature = event.feature;
        return core->current_page;
    }

    if (event.type == WATCH_CORE_UI_EVENT_BACK) {
        if (core->current_page.type == WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL) {
            core->current_page.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
            core->current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
            return core->current_page;
        }
    }

    return none;
}

const char * watch_core_health_feature_name(WatchCoreHealthFeature feature)
{
    switch (feature) {
        case WATCH_CORE_HEALTH_FEATURE_HEART_RATE:
            return "Heart Rate";
        case WATCH_CORE_HEALTH_FEATURE_SPO2:
            return "SpO2";
        case WATCH_CORE_HEALTH_FEATURE_BREATHE:
            return "Breathing";
        case WATCH_CORE_HEALTH_FEATURE_STRESS:
            return "Stress";
        default:
            return "Unknown";
    }
}

const char * watch_core_ui_event_name(WatchCoreUiEventType type)
{
    switch (type) {
        case WATCH_CORE_UI_EVENT_HEALTH_CARD_CLICKED:
            return "HEALTH_CARD_CLICKED";
        case WATCH_CORE_UI_EVENT_BACK:
            return "BACK";
        case WATCH_CORE_UI_EVENT_NONE:
        default:
            return "NONE";
    }
}

const char * watch_core_page_intent_name(WatchCorePageIntentType type)
{
    switch (type) {
        case WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS:
            return "LOAD_HEALTH_SHORTCUTS";
        case WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL:
            return "LOAD_HEALTH_DETAIL";
        case WATCH_CORE_PAGE_INTENT_NONE:
        default:
            return "NONE";
    }
}

bool watch_core_health_feature_is_valid(WatchCoreHealthFeature feature)
{
    return feature == WATCH_CORE_HEALTH_FEATURE_HEART_RATE ||
           feature == WATCH_CORE_HEALTH_FEATURE_SPO2 ||
           feature == WATCH_CORE_HEALTH_FEATURE_BREATHE ||
           feature == WATCH_CORE_HEALTH_FEATURE_STRESS;
}
