#ifndef WATCH_CORE_WATCH_CORE_H
#define WATCH_CORE_WATCH_CORE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WATCH_CORE_HEALTH_CARD_COUNT 4U
#define WATCH_CORE_METRIC_TEXT_MAX 16U
#define WATCH_CORE_EVENT_QUEUE_CAPACITY 8U

typedef enum {
    WATCH_CORE_HEALTH_FEATURE_HEART_RATE = 0,
    WATCH_CORE_HEALTH_FEATURE_SPO2 = 1,
    WATCH_CORE_HEALTH_FEATURE_BREATHE = 2,
    WATCH_CORE_HEALTH_FEATURE_STRESS = 3,
    WATCH_CORE_HEALTH_FEATURE_INVALID = 255
} WatchCoreHealthFeature;

typedef enum {
    WATCH_CORE_UI_EVENT_NONE = 0,
    WATCH_CORE_UI_EVENT_HEALTH_CARD_CLICKED = 1,
    WATCH_CORE_UI_EVENT_BACK = 2
} WatchCoreUiEventType;

typedef struct {
    WatchCoreUiEventType type;
    WatchCoreHealthFeature feature;
    uint16_t reserved;
} WatchCoreUiEvent;

typedef struct {
    char health_metric_text[WATCH_CORE_HEALTH_CARD_COUNT][WATCH_CORE_METRIC_TEXT_MAX];
} WatchCoreUiModelSnapshot;

typedef enum {
    WATCH_CORE_PAGE_INTENT_NONE = 0,
    WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS = 1,
    WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL = 2
} WatchCorePageIntentType;

typedef struct {
    WatchCorePageIntentType type;
    WatchCoreHealthFeature feature;
} WatchCorePageIntent;

typedef struct {
    WatchCoreUiModelSnapshot model;
    WatchCorePageIntent current_page;
    WatchCoreUiEvent event_queue[WATCH_CORE_EVENT_QUEUE_CAPACITY];
    uint8_t queue_head;
    uint8_t queue_tail;
    uint8_t queue_count;
} WatchCore;

void watch_core_init(WatchCore * core);

void watch_core_get_ui_snapshot(const WatchCore * core, WatchCoreUiModelSnapshot * out_snapshot);

bool watch_core_set_health_metric(
    WatchCore * core,
    WatchCoreHealthFeature feature,
    const char * metric_text);

WatchCoreUiEvent watch_core_make_health_card_clicked_event(WatchCoreHealthFeature feature);

WatchCoreUiEvent watch_core_make_back_event(void);

bool watch_core_push_event(WatchCore * core, WatchCoreUiEvent event);

WatchCorePageIntent watch_core_process_next_event(WatchCore * core);

const char * watch_core_health_feature_name(WatchCoreHealthFeature feature);

const char * watch_core_ui_event_name(WatchCoreUiEventType type);

const char * watch_core_page_intent_name(WatchCorePageIntentType type);

bool watch_core_health_feature_is_valid(WatchCoreHealthFeature feature);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WATCH_CORE_WATCH_CORE_H */
