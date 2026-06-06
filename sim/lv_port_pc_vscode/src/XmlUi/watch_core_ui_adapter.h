#ifndef MAGIC_WATCH_XML_UI_WATCH_CORE_UI_ADAPTER_H
#define MAGIC_WATCH_XML_UI_WATCH_CORE_UI_ADAPTER_H

#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "watch_core/watch_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_point_t press_point;
    bool active;
    bool moved;
} WatchCoreUiClickGuardState;

struct WatchCoreUiAdapter;

typedef struct {
    struct WatchCoreUiAdapter * adapter;
    const char * card_id;
    WatchCoreUiClickGuardState guard;
} WatchCoreHealthHitTarget;

typedef struct WatchCoreUiAdapter {
    WatchCore * core;
    lv_obj_t * active_screen;
    bool subjects_initialized;
    lv_subject_t health_metric_subjects[WATCH_CORE_HEALTH_CARD_COUNT];
    char health_metric_subject_buffers[WATCH_CORE_HEALTH_CARD_COUNT][WATCH_CORE_METRIC_TEXT_MAX];
    char health_metric_subject_previous_buffers[WATCH_CORE_HEALTH_CARD_COUNT][WATCH_CORE_METRIC_TEXT_MAX];
    WatchCoreHealthHitTarget health_hit_targets[WATCH_CORE_HEALTH_CARD_COUNT];
} WatchCoreUiAdapter;

bool watch_core_ui_adapter_init(WatchCoreUiAdapter * adapter, WatchCore * core);

bool watch_core_ui_adapter_load_health_shortcuts(WatchCoreUiAdapter * adapter);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAGIC_WATCH_XML_UI_WATCH_CORE_UI_ADAPTER_H */
