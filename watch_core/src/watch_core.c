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

static WatchCorePageIntent watch_core_make_none_intent(void)
{
    WatchCorePageIntent none;

    none.type = WATCH_CORE_PAGE_INTENT_NONE;
    none.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    return none;
}

static WatchCorePageIntent watch_core_make_intent_from_state(const WatchCorePageState * page_state)
{
    WatchCorePageIntent intent;

    if (page_state == NULL) {
        return watch_core_make_none_intent();
    }

    if (page_state->type == WATCH_CORE_PAGE_HEALTH_DETAIL) {
        intent.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL;
        intent.feature = page_state->feature;
        return intent;
    }

    intent.type = WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS;
    intent.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    return intent;
}

static bool watch_core_power_state_is_valid(WatchCorePowerState state)
{
    return state == WATCH_CORE_POWER_STATE_SCREEN_ON ||
           state == WATCH_CORE_POWER_STATE_SCREEN_OFF;
}

static WatchCoreBatteryState watch_core_make_absent_battery_state(void)
{
    WatchCoreBatteryState state;

    state.present = false;
    state.charging = false;
    state.percent = 0U;
    return state;
}

static bool watch_core_normalize_battery_state(
    WatchCoreBatteryState input,
    WatchCoreBatteryState * out_state)
{
    if (out_state == NULL) {
        return false;
    }

    if (!input.present) {
        *out_state = watch_core_make_absent_battery_state();
        return true;
    }

    if (input.percent > 100U) {
        return false;
    }

    *out_state = input;
    return true;
}

static WatchCorePowerAction watch_core_make_none_power_action(WatchCorePowerState state)
{
    WatchCorePowerAction action;

    action.type = WATCH_CORE_POWER_ACTION_NONE;
    action.source_state = state;
    action.target_state = state;
    return action;
}

static bool watch_core_power_action_is_valid(WatchCorePowerAction action)
{
    if (!watch_core_power_state_is_valid(action.source_state) ||
        !watch_core_power_state_is_valid(action.target_state)) {
        return false;
    }

    if (action.type == WATCH_CORE_POWER_ACTION_NONE) {
        return action.source_state == action.target_state;
    }

    if (action.type == WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF) {
        return action.source_state == WATCH_CORE_POWER_STATE_SCREEN_ON &&
               action.target_state == WATCH_CORE_POWER_STATE_SCREEN_OFF;
    }

    if (action.type == WATCH_CORE_POWER_ACTION_WAKE_SCREEN) {
        return action.source_state == WATCH_CORE_POWER_STATE_SCREEN_OFF &&
               action.target_state == WATCH_CORE_POWER_STATE_SCREEN_ON;
    }

    return false;
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
    core->model.battery = watch_core_make_absent_battery_state();

    core->current_page.type = WATCH_CORE_PAGE_HEALTH_SHORTCUTS;
    core->current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
    core->power_controller.current_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
}

void watch_core_get_ui_snapshot(const WatchCore * core, WatchCoreUiModelSnapshot * out_snapshot)
{
    if (core == NULL || out_snapshot == NULL) {
        return;
    }

    *out_snapshot = core->model;
}

void watch_core_get_current_page_state(const WatchCore * core, WatchCorePageState * out_page_state)
{
    if (core == NULL || out_page_state == NULL) {
        return;
    }

    *out_page_state = core->current_page;
}

void watch_core_get_power_state(const WatchCore * core, WatchCorePowerState * out_power_state)
{
    if (core == NULL || out_power_state == NULL) {
        return;
    }

    *out_power_state = core->power_controller.current_state;
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

bool watch_core_set_battery_state(WatchCore * core, WatchCoreBatteryState state)
{
    WatchCoreBatteryState normalized_state;

    if (core == NULL || !watch_core_normalize_battery_state(state, &normalized_state)) {
        return false;
    }

    core->model.battery = normalized_state;
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
    WatchCorePageIntent none = watch_core_make_none_intent();

    WatchCoreUiEvent event;
    if (core == NULL || !pop_event(core, &event)) {
        return none;
    }

    if (event.type == WATCH_CORE_UI_EVENT_HEALTH_CARD_CLICKED) {
        if (!watch_core_health_feature_is_valid(event.feature)) {
            return none;
        }

        core->current_page.type = WATCH_CORE_PAGE_HEALTH_DETAIL;
        core->current_page.feature = event.feature;
        return watch_core_make_intent_from_state(&core->current_page);
    }

    if (event.type == WATCH_CORE_UI_EVENT_BACK) {
        if (core->current_page.type == WATCH_CORE_PAGE_HEALTH_DETAIL) {
            core->current_page.type = WATCH_CORE_PAGE_HEALTH_SHORTCUTS;
            core->current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
            return watch_core_make_intent_from_state(&core->current_page);
        }
    }

    return none;
}

WatchCorePageIntent watch_core_process_pending_events(WatchCore * core)
{
    WatchCorePageIntent last_intent = watch_core_make_none_intent();

    if (core == NULL) {
        return last_intent;
    }

    while (core->queue_count > 0U) {
        WatchCorePageIntent intent = watch_core_process_next_event(core);
        if (intent.type != WATCH_CORE_PAGE_INTENT_NONE) {
            last_intent = intent;
        }
    }

    return last_intent;
}

WatchCorePowerAction watch_core_request_power_action(const WatchCore * core, WatchCorePowerRequest request)
{
    WatchCorePowerState current_state;

    if (core == NULL) {
        return watch_core_make_none_power_action(WATCH_CORE_POWER_STATE_SCREEN_ON);
    }

    current_state = core->power_controller.current_state;

    if (request == WATCH_CORE_POWER_REQUEST_SCREEN_OFF &&
        current_state == WATCH_CORE_POWER_STATE_SCREEN_ON) {
        WatchCorePowerAction action;

        action.type = WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF;
        action.source_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
        action.target_state = WATCH_CORE_POWER_STATE_SCREEN_OFF;
        return action;
    }

    if (request == WATCH_CORE_POWER_REQUEST_WAKE &&
        current_state == WATCH_CORE_POWER_STATE_SCREEN_OFF) {
        WatchCorePowerAction action;

        action.type = WATCH_CORE_POWER_ACTION_WAKE_SCREEN;
        action.source_state = WATCH_CORE_POWER_STATE_SCREEN_OFF;
        action.target_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
        return action;
    }

    return watch_core_make_none_power_action(current_state);
}

bool watch_core_commit_power_action(
    WatchCore * core,
    WatchCorePowerAction action,
    bool platform_applied)
{
    if (core == NULL ||
        !watch_core_power_action_is_valid(action) ||
        action.type == WATCH_CORE_POWER_ACTION_NONE ||
        core->power_controller.current_state != action.source_state) {
        return false;
    }

    if (platform_applied) {
        core->power_controller.current_state = action.target_state;
    }

    return true;
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

const char * watch_core_page_name(WatchCorePageType type)
{
    switch (type) {
        case WATCH_CORE_PAGE_HEALTH_DETAIL:
            return "HEALTH_DETAIL";
        case WATCH_CORE_PAGE_HEALTH_SHORTCUTS:
        default:
            return "HEALTH_SHORTCUTS";
    }
}

bool watch_core_health_feature_is_valid(WatchCoreHealthFeature feature)
{
    return feature == WATCH_CORE_HEALTH_FEATURE_HEART_RATE ||
           feature == WATCH_CORE_HEALTH_FEATURE_SPO2 ||
           feature == WATCH_CORE_HEALTH_FEATURE_BREATHE ||
           feature == WATCH_CORE_HEALTH_FEATURE_STRESS;
}
